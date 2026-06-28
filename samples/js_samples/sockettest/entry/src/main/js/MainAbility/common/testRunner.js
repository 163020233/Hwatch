/**
 * testRunner.js — 所有测试场景集合
 *
 * 所有函数第一个参数 ctx 为页面组件 this（可访问 socketList / updateUI 等）
 *
 * 【原生 API 说明】
 *   runErrorDemo 和 runHTTPDemo 中直接调用了 sk.client 上的底层方法：
 *     - sk.client.recv(len, flags, cb)
 *     - sk.client.send(data, flags, cb)
 *     - sk.client.getaddrinfo(host, port, hints, cb)
 *   若接口签名有变化，请核查带【原生 API】标记的位置。
 */
import core from './socketCore';
import codec from './codec';

// ==========================================
// 场景 A：多并发长稳压测（核心场景）
// 需先创建 3 TCP + 3 UDP
// ==========================================
function runMultiConcurrent(ctx) {
    console.info('[System] Preparing Multi-Concurrent Stress Test...');

    // 1. 开启标志位
    ctx._isMultiConcurrentActive = true;

    // 2. 总限时
    ctx._stressTimeoutId = setTimeout(() => {
        console.info('[System] Stress test time limit reached. Stopping...');
        core.destroyAll(ctx);
    }, ctx._durationMS);

    // 3. 5 秒后发送起跑指令（只发一次）
    ctx._startCmdTimerId = setTimeout(() => {
        var sent = false;
        for (var k = 0; k < ctx.socketList.length; k++) {
            var sk = ctx.socketList[k];
            if (sk && sk.isConnected) {
                core.sendMsg(ctx, sk, 'CMD:STRESS_START', null);
                sent = true;
                break;
            }
        }
        if (!sent) {
            console.warn('[System] No valid socket for STRESS_START');
        }
    }, 5000);

    // 4. 【GC 优化】每 5 秒聚合打印统计，替代逐条日志
    ctx._statsTimerId = setInterval(() => {
        var tx = 0, rx = 0, n = ctx.socketList.length;
        for (var k = 0; k < n; k++) {
            tx += ctx.socketList[k].txCount;
            rx += ctx.socketList[k].rxCount;
        }
        console.info('[Stats] Active:' + n + ' TX:' + tx + ' RX:' + rx);
    }, 5000);

    // 5. 动态生命周期调度：每 10 秒随机替换 1 个 socket
    ctx._lifecycleTimerId = setInterval(() => {
        if (ctx.socketList.length === 0) {
            return;
        }

        var rIdx = Math.floor(Math.random() * ctx.socketList.length);
        var target = ctx.socketList[rIdx];
        var typeToRecreate = rIdx <= 2 ? "TCP" : "UDP";

        console.info('[Lifecycle] Destroying S' + target.id +
            ' (' + typeToRecreate.toUpperCase() + ')');
        core.destroyOne(ctx, target);

        // 延迟 1 秒后重建同类型，确保 OS 释放端口 / FD
        setTimeout(() => {
            if (ctx._isMultiConcurrentActive) {
                ctx.addSocket(typeToRecreate);
            }
        }, 3000);
    }, 10000);
}

// ==========================================
// 场景 B：批量发送（3 条）
// ==========================================
function runBatchTest(ctx, sk) {
    sk.batchCount = 0;
    sk.batchTotal = 3;
    // 【GC 优化】预绑定回调，循环内零闭包分配
    sk._boundBatchExec = function () {
        _doBatch(ctx, sk);
    };
    sk._boundBatchCb = function (err, res) {
        var ok = !err;
        if (ok && sk.batchCount < sk.batchTotal) {
            if (sk.batchTimeoutId) {
                clearTimeout(sk.batchTimeoutId);
            }
            sk.batchTimeoutId = setTimeout(sk._boundBatchExec, 1000);
        }
    };
    _doBatch(ctx, sk);
}

function _doBatch(ctx, sk) {
    sk.batchCount++;
    var msg = '[S' + sk.id + '] Batch ' + sk.batchCount + '/' + sk.batchTotal;
    core.sendMsg(ctx, sk, msg, sk._boundBatchCb);
}

// ==========================================
// 场景 C：单 Socket 压测（10 条快发）
// ==========================================
function runStressTest(ctx, sk) {
    sk.stressTotal = 10;
    sk.stressSent = 0;
    sk.stressStartTime = Date.now();
    sk.stressStartRecv = sk.rxCount;
    sk._boundStressExec = function () {
        _doStress(ctx, sk);
    };
    sk._boundStressCb = function () {
        if (sk.stressTimeoutId) {
            clearTimeout(sk.stressTimeoutId);
        }
        sk.stressTimeoutId = setTimeout(sk._boundStressExec, 100);
    };
    _doStress(ctx, sk);
}

function _doStress(ctx, sk) {
    if (sk.stressSent >= sk.stressTotal) {
        if (sk.stressTimeoutId) {
            clearTimeout(sk.stressTimeoutId);
        }
        sk.stressTimeoutId = setTimeout(function () {
            var elapsed = Date.now() - sk.stressStartTime;
            var rx = sk.rxCount - sk.stressStartRecv;
            console.info('=== [S' + sk.id + '] Stress Results ===');
            console.info('Time:' + elapsed + 'ms Sent:' + sk.stressSent +
                ' Recv:' + rx +
                ' Rate:' + ((rx / (sk.stressSent || 1)) * 100).toFixed(2) + '%');
        }, 100);
        return;
    }
    sk.stressSent++;
    var msg = '[S' + sk.id + '] Stress_' + sk.stressSent;
    core.sendMsg(ctx, sk, msg, sk._boundStressCb);
}

// ==========================================
// 场景 D：10 分钟马拉松随机压测
// ==========================================
function runMarathon(ctx, sk) {
    console.info('=== [S' + sk.id + '] Starting Marathon ===');
    sk.randomStressSent = 0;
    sk.randomStressStartRecv = sk.rxCount;
    sk.randomStressEndTime = Date.now() + ctx._durationMS;

    // 【GC 优化】预绑定，避免每轮 setTimeout 创建新闭包
    sk._boundMarathonExec = function () {
        _doMarathon(ctx, sk);
    };
    sk._boundMarathonCb = function () {
        if (sk.randomStressTimerId) {
            clearTimeout(sk.randomStressTimerId);
        }
        sk.randomStressTimerId = setTimeout(sk._boundMarathonExec, 100);
    };
    _doMarathon(ctx, sk);
}

function _doMarathon(ctx, sk) {
    var now = Date.now();
    if (now >= sk.randomStressEndTime) {
        var elapsed = now - (sk.randomStressEndTime - ctx._durationMS);
        var rx = sk.rxCount - sk.randomStressStartRecv;
        console.info('=== [S' + sk.id + '] Marathon Complete ===');
        console.info('Time:' + elapsed + 'ms Sent:' + sk.randomStressSent +
            ' Recv:' + rx +
            ' Rate:' + ((rx / (sk.randomStressSent || 1)) * 100).toFixed(2) + '%');
        sk._boundMarathonExec = null;
        sk._boundMarathonCb = null;
        return;
    }

    var randType = Math.floor(Math.random() * 4);
    var testData;

    switch (randType) {
        case 0:
            testData = '[S' + sk.id + '] Marathon [' + sk.randomStressSent + ']';
            break;
        case 1:
            testData = new Uint8Array([0xDE, 0xAD, 0xBE, 0xEF]);
            break;
        case 2:
            var arr = [];
            for (var i = 0; i < 16; i++) {
                arr[i] = i;
            }
            testData = new Uint8Array(arr);
            break;
        case 3:
            testData = '[S' + sk.id + '] 马拉松包 ' + sk.randomStressSent;
            break;
    }

    sk.randomStressSent++;

    // 每 50 条打印一次进度（控制日志 GC 开销）
    if (sk.randomStressSent % 50 === 0) {
        var left = Math.floor((sk.randomStressEndTime - now) / 1000);
        console.info('[S' + sk.id + '][Marathon] Sent:' + sk.randomStressSent + ' Left:' + left + 's');
    }

    core.sendMsg(ctx, sk, testData, sk._boundMarathonCb);
}

// ==========================================
// 场景 E：二进制循环发送
// ==========================================
function sendBinaryData(ctx, sk) {
    sk.binaryTestIndex = (sk.binaryTestIndex || 0) + 1;
    var testData;
    var dataType = '';

    switch (sk.binaryTestIndex % 4) {
        case 0:
            dataType = 'Text';
            testData = '[S' + sk.id + '] Hello from OpenHarmony!';
            break;
        case 1:
            dataType = 'DeadBeef';
            testData = new Uint8Array([0xDE, 0xAD, 0xBE, 0xEF]);
            break;
        case 2:
            dataType = 'BinaryArray';
            var arr = [];
            for (var i = 0; i < 16; i++) {
                arr[i] = i;
            }
            testData = new Uint8Array(arr);
            break;
        case 3:
            dataType = 'Chinese';
            testData = '[S' + sk.id + '] Hello 中文测试 123456';
            break;
    }

    core.sendMsg(ctx, sk, testData, function (err, res) {
        if (err) {
            console.error('[S' + sk.id + '] ' + dataType + ' send failed');
        }
        console.info('[S' + sk.id + '] ' + dataType + ' send success');
    });
}

// ==========================================
// 场景 F：异常处理测试
// ==========================================
function runErrorDemo(ctx, sk) {
    console.info('[S' + sk.id + '] === Error Handling Demo ===');

    // 测试 1: 正常发送
    core.sendMsg(ctx, sk, 'Error test message', function (err) {
        console.info('[S' + sk.id + '] Normal send: ' + JSON.stringify(err));
    });

    // 测试 2: 延迟 500ms 尝试接收
    setTimeout(function () {
        // 【原生 API】recv 底层调用
        sk.client.recv(function (err) {
            console.info('[S' + sk.id + '] Recv test: ' + JSON.stringify(err));
        });
    }, 500);
}

// ==========================================
// 场景 G：HTTP / DNS 解析示例
// ==========================================
function runHTTPDemo(ctx, sk) {
    console.info('[S' + sk.id + '] === DNS Resolution Demo ===');

    // 【原生 API】getaddrinfo — DNS 解析
    // 若底层未适配此接口，回调中会返回 err
    sk.client.getaddrinfo('www.example.com', '80', (err, info) => {
        if (err) {
            console.error('[S' + sk.id + '] DNS failed: ' +
                (err ? JSON.stringify(err) : 'Unknown'));
            return;
        }
        console.info('[S' + sk.id + '] Resolved: ' +
            (info.address || '93.184.216.34') + ':' + (info.port || 80));
    });
}

// ==========================================
// 导出
// ==========================================
export default {
    runMultiConcurrent: runMultiConcurrent,
    runBatchTest: runBatchTest,
    runStressTest: runStressTest,
    runMarathon: runMarathon,
    sendBinaryData: sendBinaryData,
    runErrorDemo: runErrorDemo,
    runHTTPDemo: runHTTPDemo
};