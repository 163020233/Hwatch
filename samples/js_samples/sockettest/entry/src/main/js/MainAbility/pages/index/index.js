/**
 * index.js — 主页面入口
 *
 * 职责：UI 状态管理 + 按钮事件路由
 * 所有底层操作委托给 socketCore / testRunner
 *
 * 【引入说明】
 * 若 LiteOS 环境不支持从 common/ 目录 import 本地文件，请将
 * codec.js / socketCore.js / testRunner.js 的内容合并到本文件。
 * 合并方法：去掉各文件的 import/export，把函数平铺到 export default 对象中。
 */
import core from '../../common/socketCore';
import tests from '../../common/testRunner';

export default {
    data: {
        displayInfo: 'No socket'
    },

    onInit: function () {
        console.info('Socket Demo initialized');
        this._serverIp = ''; // 对端IP地址
        this._tcpServerPort = ''; // TCP 对端端口号
        this._udpServerPort = ''; // UDP 对端端口号
        this._durationMS = 10 * 60 * 1000; // 压测限时 10 分钟

        // 多实例管理
        this.socketList = [];
        this.currentIndex = -1;
        this.socketMaxCount = 6;

        // 全局定时器（多并发压测专用）
        this._lifecycleTimerId = null;
        this._startCmdTimerId = null;
        this._stressTimeoutId = null;
        this._statsTimerId = null;
        this._isMultiConcurrentActive = false;
    },

    // ==========================================
    // Socket 管理
    // ==========================================
    getAvailableSocketId: function () {
        var usedIds = [];
        for (var i = 0; i < this.socketList.length; i++) {
            usedIds.push(this.socketList[i].id);
        }
        var id = 1;
        while (usedIds.indexOf(id) !== -1) {
            id++;
        }
        return id;
    },

    addSocket: function (type) {
        if (this.socketList.length >= this.socketMaxCount) {
            console.error('Max socket limit: ' + this.socketMaxCount);
            return;
        }
        var newId = this.getAvailableSocketId();
        console.info('=== Creating ' + type.toUpperCase() + ' [S' + newId + '] ===');

        var sk = core.createSkObj(newId, type);
        core.prepareClient(this, sk, type);

        this.socketList.push(sk);
        this.socketList.sort(function (a, b) {
            return a.id - b.id;
        });
        this.currentIndex = this.socketList.indexOf(sk);
        this.updateUI();
    },

    getCurrentSocket: function () {
        if (this.currentIndex < 0 || this.currentIndex >= this.socketList.length) {
            return null;
        }
        return this.socketList[this.currentIndex];
    },

    // ==========================================
    // UI 切换
    // ==========================================
    switchNext: function () {
        if (this.socketList.length === 0) {
            return;
        }
        this.currentIndex = (this.currentIndex + 1) % this.socketList.length;
        this.updateUI();
    },

    switchPrev: function () {
        if (this.socketList.length === 0) {
            return;
        }
        this.currentIndex = (this.currentIndex - 1 + this.socketList.length) % this.socketList.length;
        this.updateUI();
    },

    updateUI: function () {
        if (this.currentIndex < 0 || this.socketList.length === 0) {
            this.displayInfo = 'No socket';
        } else {
            var sk = this.socketList[this.currentIndex];
            this.displayInfo = 'S' + sk.id + ' [' + sk.type.toUpperCase() + '] ' +
                (sk.isConnected ? 'OK' : '...');
        }
    },

    // ==========================================
    // 按钮处理（全部委托到模块）
    // ==========================================
    runTCPDemo: function () {
        this.addSocket('TCP');
    },
    runUDPDemo: function () {
        this.addSocket('UDP');
    },

    sendAnotherMessage: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.error('No valid socket');
        }
        core.sendMsg(this, sk, '[S' + sk.id + '] Test msg ' + (sk.txCount + 1), null);
    },

    sendBinaryData: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.error('No valid socket');
        }
        tests.sendBinaryData(this, sk);
    },

    testSendMultipleMessages: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.error('No connection');
        }
        tests.runBatchTest(this, sk);
    },

    runStressTest: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.error('No connection');
        }
        tests.runStressTest(this, sk);
    },

    runTenMinRandomStressTest: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return;
        }
        tests.runMarathon(this, sk);
    },

    runMultiConcurrentStressTest: function () {
        tests.runMultiConcurrent(this);
    },

    runErrorHandlingDemo: function () {
        var sk = this.getCurrentSocket();
        if (!sk || sk.fd < 0) {
            return console.info('No active socket');
        }
        tests.runErrorDemo(this, sk);
    },

    runHTTPClientDemo: function () {
        var sk = this.getCurrentSocket();
        if (!sk || !sk.client) {
            return console.error('No socket instance');
        }
        tests.runHTTPDemo(this, sk);
    },

    stopReceiving: function () {
        var sk = this.getCurrentSocket();
        if (sk) {
            core.stopRecv(sk);
            console.info('[S' + sk.id + '] Receive stopped');
        }
    },

    restartReceiving: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return;
        }
        core.stopRecv(sk);
        var self = this;
        if (sk.restartTimeoutId) {
            clearTimeout(sk.restartTimeoutId);
        }
        sk.restartTimeoutId = setTimeout(function () {
            if (sk.isConnected && sk.fd >= 0) {
                core.startRecv(self, sk);
            }
        }, 250);
    },

    showConnectionStatus: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.info('No active socket');
        }
        console.info('=== [S' + sk.id + '] Status ===');
        console.info('FD:' + sk.fd + ' Type:' + sk.type + ' Connected:' + sk.isConnected);
        console.info('TX:' + sk.txCount + ' RX:' + sk.rxCount);
    },

    closeCurrentSocket: function () {
        var sk = this.getCurrentSocket();
        if (!sk) {
            return console.info('No socket to close');
        }
        core.destroyOne(this, sk);
    },

    destroySocketClient: function () {
        this.closeCurrentSocket();
    },

    forceClearAllSockets: function () {
        core.destroyAll(this);
    },

    onDestroy: function () {
        console.info('=== Page onDestroy ===');
        core.destroyAll(this);
    }
};