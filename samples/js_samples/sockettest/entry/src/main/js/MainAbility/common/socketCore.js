/**
 * socketCore.js — Socket 核心操作层
 *
 * 包含：创建 / 连接 / 发送 / 接收 / 销毁
 * 所有 export 函数的第一个参数 ctx 为页面组件 this
 *
 * ────────── 原生 API 清单（来自 @ohos.net.socket）──────────
 *   socket.createSocket()                      创建客户端对象
 *   client.socket(af, type, proto, cb)         底层 socket() 调用
 *   client.connect(addr, cb)               连接服务端
 *   client.send(data, flags, cb)           发送数据
 *   client.recv(bufLen, flags, cb)         接收数据
 *   client.close(cb)                       关闭网络连接
 *   client.destroy(cb)                         释放客户端内存
 * 若以上接口签名发生变化，请重点核查本文件中带【原生 API】标记的位置。
 * ──────────────────────────────────────────────────────────
 */

/**
 * 【原生 API 引入说明】
 * 此处从 common/ 目录导入系统模块。若 LiteOS 环境不允许在 common/ 中
 * 引入 @ohos 模块，有两种解决方案：
 *   方案 A：在 index.js 中 import，通过 ctx._socketModule 传入
 *   方案 B：将本文件合并回 index.js（见底部合并说明）
 */
import socket from '@ohos.net.socket';
import codec from './codec';

// ==========================================
// 共享常量 —— 复用同一个空数组，避免重复 new
// ==========================================
var EMPTY_U8 = new Uint8Array(0);

// ==========================================
// Socket 数据对象工厂
// ==========================================
function createSkObj(id, type) {
    return {
        id: id,
        type: type,
        client: null,
        isConnected: false,
        isReceiving: false,
        shouldAutoReceive: true,

        txCount: 0,
        rxCount: 0,
        lastReceiveTime: 0,

        // 【GC 优化】内联解码残留字节，替代原 binaryChunks 累积
        leftoverBytes: EMPTY_U8,

        // 定时器 ID
        receiveTimeoutId: null,
        businessTimerId: null, // setInterval，需 clearInterval
        batchTimeoutId: null,
        stressTimeoutId: null,
        restartTimeoutId: null,
        randomStressTimerId: null,

        // 预绑定回调（prepareClient 中赋值，热路径零闭包分配）
        _onCreated: null,
        _onConnected: null,
        _onRecvResult: null,
        _onSendDone: null,
        _doRecvBound: null,

        // ====== 压测模式 ======
        _stressMode: false,
        _prePayload: null, // 预分配的发送载荷
        _stressSendBound: null,

        // ====== 各测试的计数器（由 testRunner 按需写入）======
        batchCount: 0,
        batchTotal: 3,
        stressSent: 0,
        stressTotal: 10,
        stressStartRecv: 0,
        stressStartTime: 0,
        randomStressSent: 0,
        randomStressStartRecv: 0,
        randomStressEndTime: 0,
        binaryTestIndex: 0,
        currentBinaryDataType: ''
    };
}

// ==========================================
// 初始化 native 客户端 & 绑定回调
// ==========================================
function prepareClient(ctx, sk, type) {
    // 销毁可能存在的残留 client
    if (sk.client) {
        sk.client.destroy();
    }

    // 【原生 API】创建 socket 客户端对象
    sk.client = socket.createSocket();
    if (!sk.client) {
        console.error('[S' + sk.id + '] createSocket failed!');
        return;
    }

    sk.isConnected = false;
    sk.isReceiving = false;
    sk.txCount = 0;
    sk.rxCount = 0;
    sk.leftoverBytes = EMPTY_U8;
    clearTimers(sk);

    // 【GC 优化·核心】所有热路径回调只 bind 一次，后续重复使用同一函数引用
    sk._onCreated = _onCreated.bind(null, ctx, sk);
    sk._onConnected = _onConnected.bind(null, ctx, sk);
    sk._onRecvResult = _handleRecv.bind(null, ctx, sk);
    sk._onSendDone = _onSendDone.bind(null, sk);
    sk._doRecvBound = _doRecv.bind(null, ctx, sk);

    // 【原生 API】底层 socket() 系统调用
    sk.client.socket(type, sk._onCreated);
}

// ==========================================
// 内部回调（不导出，通过 bind 附着在 sk 上）
// ==========================================
function _onCreated(ctx, sk, err, result) {
    if (err) {
        console.error('[S' + sk.id + '] socket() failed: ' +
            (err ? JSON.stringify(err) : 'No result'));
        return;
    }
    console.info('[S' + sk.id + '] Created');

    var port = sk.type === 'TCP' ? ctx._tcpServerPort : ctx._udpServerPort;
    var address = { address: ctx._serverIp, port: parseInt(port) };

    // 【原生 API】connect
    sk.client.connect(address, sk._onConnected);
}

function _onConnected(ctx, sk, err, result) {
    if (err) {
        console.error('[S' + sk.id + '] connect() failed: ' +
            (err ? JSON.stringify(err) : 'No result'));
        return;
    }
    sk.isConnected = true;
    ctx.updateUI();
    console.info('[S' + sk.id + '] Connected!');

    // 欢迎消息（一次性，不影响 GC）
    sendMsg(ctx, sk, 'Hello from Client [S' + sk.id + ']!', null);
    console.info(' Connected! MultiActive: ' + ctx._isMultiConcurrentActive);
    // 启动接收循环
    startRecv(ctx, sk);

    // ===== 多并发压测模式专属 =====
    if (ctx._isMultiConcurrentActive) {
        sk._stressMode = true;
        console.info(' Entering Stress Mode...');
        // 【GC 优化·核心】预分配固定发送载荷，后续每次 send 直接复用
        var prefix = '[S' + sk.id + '] STRESS_';
        var targetLen = sk.type === 'TCP' ? 256 : 32;
        var padLen = targetLen - prefix.length;
        var pad = padLen > 0 ? new Array(padLen + 1).join('A') : '';
        sk._prePayload = codec.strToUint8Array(prefix + pad);

        // 预绑定发送回调，setInterval 中不再产生新闭包
        sk._stressSendBound = function () {
            // console.info('[HEARTBEAT] S' + sk.id + ' at ' + Date.now());
            if (sk.isConnected) {
                // console.info(' Timer Tick! Connected: ' + sk.isConnected);
                sendMsg(ctx, sk, sk._prePayload, null);
            }
        };

        var intervalMs = 200;
        sk.businessTimerId = setInterval(sk._stressSendBound, intervalMs);
    }
}

function _onSendDone(sk, err, result) {
    if (err) {
        console.warn('[S' + sk.id + '] Send fail: ' + JSON.stringify(err));
    }
    console.info('send successful');
}

// ==========================================
// 发送
// ==========================================
function sendMsg(ctx, sk, payload, cb) {
    // console.info("hahahaha");
    if (!sk || !sk.isConnected) {
        if (cb) {
            cb('Not connected', { success: false });
        }
        return;
    }
    var binary;
    if (typeof payload === 'string') {
        binary = codec.strToUint8Array(payload);
    } else if (payload instanceof Uint8Array || payload instanceof ArrayBuffer) {
        binary = payload;
    } else {
        if (cb) {
            cb('Invalid payload', { success: false });
        }
        return;
    }

    sk.txCount++;
    // 【原生 API】send
    sk.client.send(binary, cb || sk._onSendDone);
}

// ==========================================
// 接收循环
// ==========================================
function startRecv(ctx, sk) {
    if (sk.isReceiving || !sk.isConnected) {
        return;
    }
    sk.isReceiving = true;
    sk.shouldAutoReceive = true;
    sk.lastReceiveTime = Date.now();
    if (sk.receiveTimeoutId) {
        clearTimeout(sk.receiveTimeoutId);
        sk.receiveTimeoutId = null;
    }
    _doRecv(ctx, sk);
}

function _doRecv(ctx, sk) {
    // 防御性检查：socket 可能已在定时器排队期间被销毁
    if (!sk || !sk.client || !sk.shouldAutoReceive || !sk.isConnected) {
        if (sk) {
            sk.isReceiving = false;
        }
        return;
    }
    // 【原生 API】recv
    sk.client.recv(sk._onRecvResult);
}

function _handleRecv(ctx, sk, err, result) {
    // 防御性检查
    if (!sk || !sk.client) {
        return;
    }

    if (err) {
        // 22=EINVAL / 9=EBADF → socket 已不可用，停止循环
        if (err.code === 22 || err.code === 9) {
            sk.shouldAutoReceive = false;
            sk.isReceiving = false;
        }
        return;
    }

    if (result && result.data) {
        sk.rxCount++;
        sk.lastReceiveTime = Date.now();

        // 【GC 优化·核心】压测模式完全跳过解码，仅计数
        if (!sk._stressMode) {
            // 普通模式：内联解码 & 打印
            // 【原生 API 说明】result.data 可能是 Uint8Array 或 ArrayBuffer
            var raw = result.data instanceof Uint8Array
                ? result.data
                : new Uint8Array(result.data);
            _decodeAndLog(sk, raw);
        }
    }

    // 调度下一次接收（复用预绑定函数，不创建新闭包）
    if (sk.shouldAutoReceive && sk.isConnected) {
        if (sk.receiveTimeoutId) {
            clearTimeout(sk.receiveTimeoutId);
        }
        // 【GC 优化】压测模式拉长间隔可自定义拉长，进一步降低定时器分配频率
        var delay = sk._stressMode ? 100 : 100;
        sk.receiveTimeoutId = setTimeout(sk._doRecvBound, delay);
    }
}

// ==========================================
// 内联 UTF-8 解码（替代原 chunk 累积 + 定时批量解码）
// ==========================================
function _decodeAndLog(sk, chunk) {
    var data;
    if (sk.leftoverBytes.length > 0) {
        // 合并残留 + 新数据（仅此处分配 1 个临时数组）
        data = new Uint8Array(sk.leftoverBytes.length + chunk.length);
        data.set(sk.leftoverBytes, 0);
        data.set(chunk, sk.leftoverBytes.length);
    } else {
        // 无残留 → 直接使用 chunk（零分配）
        data = chunk;
    }

    // 找 UTF-8 安全截断点
    var splitIdx = _findUtf8Split(data);

    if (splitIdx > 0) {
        // subarray 创建视图（不拷贝），再传入 decode
        var str = codec.uint8ArrayToStr(data.subarray(0, splitIdx));

        // 防串包校验
        var reg = new RegExp('\\[S(\\d+)\\]');
        var m = str.match(reg);
        if (m && parseInt(m[1], 10) !== sk.id) {
            console.error('[FATAL] Cross-packet! S' + sk.id + ' got S' + m[1]);
        }
        console.info('[S' + sk.id + '][RX] <- ' + str);
    } else if (chunk.length > 0) {
        console.info('[S' + sk.id + '][RX] <- [HEX] ' + codec.toHexLog(chunk, 50));
    }

    // 保存残留（必须拷贝，因为 data 可能引用接收缓冲区）
    if (splitIdx < data.length) {
        var leftLen = data.length - splitIdx;
        sk.leftoverBytes = new Uint8Array(leftLen);
        sk.leftoverBytes.set(data.subarray(splitIdx));
    } else {
        sk.leftoverBytes = EMPTY_U8; // 复用共享空数组
    }
}

function _findUtf8Split(data) {
    var len = data.length;
    for (var j = 1; j <= 4 && j <= len; j++) {
        var b = data[len - j];
        if (b >= 0x80 && b < 0xC0) {
            continue;
        } // continuation byte
        if (b >= 0xC0) {
            var expected = 0;
            if (b < 0xE0) {
                expected = 2;
            } else if (b < 0xF0) {
                expected = 3;
            } else if (b <= 0xF4) {
                expected = 4;
            }
            if (j < expected) {
                return len - j;
            }
            break;
        }
        break; // ASCII
    }
    return len;
}

function stopRecv(sk) {
    if (!sk) {
        return;
    }
    sk.shouldAutoReceive = false;
    sk.isReceiving = false;
}

// ==========================================
// 定时器 & 引用清理
// ==========================================
function clearTimers(sk) {
    if (!sk) {
        return;
    }
    var tIds = [
        'receiveTimeoutId', 'batchTimeoutId', 'stressTimeoutId',
        'restartTimeoutId', 'randomStressTimerId'
    ];
    for (var i = 0; i < tIds.length; i++) {
        if (sk[tIds[i]]) {
            clearTimeout(sk[tIds[i]]);
            sk[tIds[i]] = null;
        }
    }
    // businessTimerId 使用 setInterval，必须用 clearInterval
    if (sk.businessTimerId) {
        console.warn('[DEBUG] Timer CLEARED for S' + sk.id); // 加这行
        clearInterval(sk.businessTimerId);
        sk.businessTimerId = null;
    }
}

/**
 * 【GC 优化】显式断开全部引用，帮助 JerryScript mark-sweep 回收
 * 必须在 native close/destroy 完成 **之后** 调用
 */
function nullifyRefs(sk) {
    sk._onCreated = null;
    sk._onConnected = null;
    sk._onRecvResult = null;
    sk._onSendDone = null;
    sk._doRecvBound = null;
    sk._stressSendBound = null;
    sk._prePayload = null;
    sk.leftoverBytes = null;
    // testRunner 可能动态挂载的绑定函数
    sk._boundBatchExec = null;
    sk._boundBatchCb = null;
    sk._boundStressExec = null;
    sk._boundStressCb = null;
    sk._boundMarathonExec = null;
    sk._boundMarathonCb = null;
    sk.client = null;
}

// ==========================================
// 销毁
// ==========================================
function destroyOne(ctx, sk) {
    if (!sk) {
        return;
    }
    sk.shouldAutoReceive = false;
    clearTimers(sk);

    // 【原生 API】close → destroy 顺序不能反
    sk.client.close((err, value) => {
        if (err) {
            console.info('error code:' + err.code);
        }
        sk.client.destroy();
        nullifyRefs(sk);
    });

    var idx = ctx.socketList.indexOf(sk);
    if (idx !== -1) {
        ctx.socketList.splice(idx, 1);
    }
    ctx.currentIndex = ctx.socketList.length > 0 ? 0 : -1;
    ctx.updateUI();
}

function destroyAll(ctx) {
    console.info('=== Force Clearing All Sockets ===');
    ctx._isMultiConcurrentActive = false;

    // 清理全局定时器
    if (ctx._lifecycleTimerId) {
        clearInterval(ctx._lifecycleTimerId);
        ctx._lifecycleTimerId = null;
    }
    if (ctx._startCmdTimerId) {
        clearTimeout(ctx._startCmdTimerId);
        ctx._startCmdTimerId = null;
    }
    if (ctx._stressTimeoutId) {
        clearTimeout(ctx._stressTimeoutId);
        ctx._stressTimeoutId = null;
    }
    if (ctx._statsTimerId) {
        clearInterval(ctx._statsTimerId);
        ctx._statsTimerId = null;
    }

    var list = ctx.socketList;
    for (var i = 0; i < list.length; i++) {
        var sk = list[i];
        sk.shouldAutoReceive = false;
        clearTimers(sk);
        // IIFE 保证异步回调中 s 指向正确的 socket 对象
        sk.client.close((err, value) => {
            if (err) {
                console.info('error code:' + err.code);
            }
            console.info('close success');
            sk.client.destroy();
            nullifyRefs(sk);
        });
    }

    ctx.socketList = [];
    ctx.currentIndex = -1;
    ctx.updateUI();
    console.info('=== All Sockets Cleared ===');
}

// ==========================================
// 导出
// ==========================================
export default {
    createSkObj: createSkObj,
    prepareClient: prepareClient,
    clearTimers: clearTimers,
    sendMsg: sendMsg,
    startRecv: startRecv,
    stopRecv: stopRecv,
    destroyOne: destroyOne,
    destroyAll: destroyAll
};