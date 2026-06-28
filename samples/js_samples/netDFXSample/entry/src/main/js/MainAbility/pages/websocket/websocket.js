import router from '@system.router';
import webSocket from '@ohos.net.webSocket';

let ws = null;
let sendData = new ArrayBuffer(1024);
let view = new Uint8Array(sendData);

export default {
    data:
    {
        wsUrl: '',   // only for test: wss://echo.websocket.org
        openSuccess: false,
        start: null,
        end: null,
    },
    onInit() {
        this.WebSocketInit();
        this.generateData();
    },
    generateData() {
        for (let i = 0; i < 1024; i++) {
            view[i] = Math.floor(Math.random() * 256);
        }
    },
    repeatArrayBuffer(times) {
        const length = sendData.byteLength;
        const resultBuffer = new ArrayBuffer(length * times);
        const resultView = new Uint8Array(resultBuffer);

        for (let i = 0; i < times; i++) {
            resultView.set(view, i * length);
        }

        return resultBuffer;
    },
    WebSocketInit() {
        if (this.wsUrl == null) {
            console.info("Please set the WebSocket service URL first");
            return;
        }

        ws = webSocket.createWebSocket();

        if (ws != null) {
            let isOpenSuccess = ws.on('open', (err, value) => {
                console.info("JS: on open: status: " + JSON.stringify(value.status));
                console.info("JS: on open: message: " + JSON.stringify(value.message));
                this.openSuccess = true;
                this.end = Date.now();
                let duration = this.end - this.start;
                console.info("wss connect is " + duration + " ms");
            });
            let isMessageSuccess = ws.on('message', (err, value) => {
                if (value instanceof ArrayBuffer) {
                    const view = new Uint8Array(value);
                    console.info("JS: on message: bufferSize is " + value.byteLength);
                }
                this.end = Date.now();
                let duration = this.end - this.start;
                console.info("wss send data " + duration + " ms");
                console.info('================== WSS Send TEST END ==================!');
            });
            let isCloseSuccess = ws.on('close', (err, value) => {
                console.info("JS: close, code is " + value.code);
                console.info("JS: close, reason is " + value.reason);
                this.openSuccess = false;
            });
            let isErrorSuccess = ws.on('error', (err, value) => {
                console.info("JS: on error, code is " + value.code);
                console.info("JS: on error, reason is " + value.reason);
            });
            if (isOpenSuccess && isMessageSuccess && isCloseSuccess && isErrorSuccess) {
                console.info("JS: subscribe finished!!!");
            } else {
                console.info("JS: subscribe Failed!!!");
            }
        } else {
            console.info("JS: createWebsocket failed!");
        }
    },

    WebSocketConnect() {
        console.info('JS: Start Connect()----------------------------------\n');
        if (ws != null) {
            this.start = Date.now();
            ws.connect(this.wsUrl,
                (err, value) => {
                    if (!err) {
                        console.info("JS: Successfully started the connect " + value);
                    } else {
                        console.info("JS: Connection failed. Err:" + JSON.stringify(err.code));
                        console.info("JS: Connection failed. Err:" + JSON.stringify(err.message));
                    }
                });
        } else {
            console.info("JS: on error, ws does not exist!");
        }
    },
    Send128B() {
        console.info('================== WSS Send 128B TEST START ==================!');
        this.Send(view.slice(0, 128).buffer);
    },
    Send400B() {
        console.info('================== WSS Send 400B TEST START ==================!');
        this.Send(view.slice(0, 400).buffer);
    },
    Send1KB() {
        console.info('================== WSS Send 1KB TEST START ==================!');
        this.Send(sendData);
    },
    Send4KB() {
        console.info('================== WSS Send 4KB TEST START ==================!');
        this.Send(this.repeatArrayBuffer(4));
    },
    Send16KB() {
        console.info('================== WSS Send 16KB TEST START ==================!');
        this.Send(this.repeatArrayBuffer(16));
    },
    Send64KB() {
        console.info('================== WSS Send 64KB TEST START ==================!');
        this.Send(this.repeatArrayBuffer(64));
    },
    SendAppend128B() {
        console.info('================== WSS Send Append 128B TEST START ==================!');
        this.SendAppend(128);
    },
    SendAppend400B() {
        console.info('================== WSS Send Append 400B TEST START ==================!');
        this.SendAppend(400);
    },
    SendAppend1KB() {
        console.info('================== WSS Send Append 1KB TEST START ==================!');
        this.SendAppend(1024);
    },
    SendAppend4KB() {
        console.info('================== WSS Send Append 4KB TEST START ==================!');
        this.SendAppend(4096);
    },
    SendAppend16KB() {
        console.info('================== WSS Send Append 16KB TEST START ==================!');
        this.SendAppend(16384);
    },
    SendAppend64KB() {
        console.info('================== WSS Send Append 16KB TEST START ==================!');
        this.SendAppend(65536);
    },
    Send(buffer) { // 最大支持20K
        if (this.openSuccess) {
            this.start = Date.now();
            ws.send(buffer, (err, value) => {
                if (!err) {
                    console.info("JS: Sent successfully");
                } else {
                    console.info("JS: Failed to send the message. Err:" + JSON.stringify(err));
                    console.info("JS: send failed code: " + err.code);
                    console.info("JS: send failed message: " + err.message);
                }
            });
        } else {
            console.info("JS: Can not send message, no open client!");
        }
    },
    SendAppend(length) { // 最大支持20K
        if (this.openSuccess) {
            this.start = Date.now();
            let ret = ws.sendInit(length);
            if (ret != 0) {
                console.info("JS: sendInit failed, ret is " + ret);
                return;
            }
            if (length <= 1024) {
                let buffer = sendData.slice(0, length);
                ret = ws.sendFinish(buffer);
                if (ret != 0) {
                    console.info("JS: sendFinish failed, ret is " + ret);
                    return;
                }
                return;
            }
            let count = length / 1024 - 1;
            for (let i = 0; i < count; i++) {
                ret = ws.sendUpdate(sendData);
                if (ret != 0) {
                    console.info("JS: sendUpdate failed, ret is " + ret);
                    return;
                }
            }

            ret = ws.sendFinish(sendData);
            if (ret != 0) {
                console.info("JS: sendFinish failed, ret is " + ret);
                return;
            }
        } else {
            console.info("JS: Can not send message, no open client!");
        }
    },
    CloseConnection() {
        ws.close((err) => {
            if (!err) {
                console.info("JS: Closed connection successfully");
            } else {
                console.info("JS: Failed to close the connection.");
            }
        });
    },
    replaceWebsockets() {
        router.replace({
            uri: 'pages/websockets/websockets',
        });
    },

    replaceHttps() {
        router.replace({
            uri: 'pages/https/https',
        });
    },

    replaceHttp() {
        router.replace({
            uri: 'pages/http/http',
        });
    },
    onHide() {
    },
    onDestroy() {
        this.CloseConnection();
    }
}
