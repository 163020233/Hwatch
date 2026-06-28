import router from '@system.router';
import app from '@system.app';
import webSocket from '@ohos.net.webSocket';
import http from '@ohos.net.http';

let globalHttpRequest = null;
let ws = null;

export default {
    data:
    {
        wsUrl: null,   // only for test: wss://echo.websocket.org
        httpUrl:null,  // only for test: https://www.baidu.com/img/PCtm_d9c8750bed0b3c7d089fa7d55720d6cf.png
        openSuccess: false,
        requestInProgress: false,
        wsIntervalId: -1,
        httpIntervalId: -1,

    },
    onInit() {
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
            });
            let isMessageSuccess = ws.on('message', (err, value) => {
                if (value instanceof ArrayBuffer) {
                    const view = new Uint8Array(value);
                    console.info("JS: on message: bufferSize is " + value.byteLength);
                    console.info(view);
                } else {
                    console.info("JS: on message:" + JSON.stringify(value));
                }
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
    WebSocketConnectWithOption() {
        console.info('JS: Start Connect()----------------------------------\n');
        if (ws != null) {
            ws.connect(this.wsUrl,
                {
                    header: {
                        ParameterOne: "",
                        ParameterTwo: "",
                    },
                    clientCert: null,
                },
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
    SendMessage() {
        if (this.openSuccess) {
            ws.send("Hello, server!\u{1F600}", (err, value) => {
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
    SendBinary() {
        if (this.openSuccess) {
            const data = new ArrayBuffer(1024);
            const view = new Uint8Array(data);
            view[0] = 11;
            view[1] = 22;
            view[1022] = 22;
            view[1023] = 33;
            ws.send(data, (err, value) => {
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
    SendAppend() {
        if (this.openSuccess) {
            const data = new ArrayBuffer(1024);
            const view = new Uint8Array(data);
            let ret = ws.sendInit(1024 * 3);
            if (ret != 0) {
                console.info("JS: sendInit failed, ret is " + ret);
                return;
            }

            ret = ws.sendUpdate(data);
            if (ret != 0) {
                console.info("JS: sendUpdate failed, ret is " + ret);
                return;
            }

            ret = ws.sendUpdate(data);
            if (ret != 0) {
                console.info("JS: sendUpdate failed, ret is " + ret);
                return;
            }

            ret = ws.sendFinish(data);
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
    SendMessageLoop() {
        if (this.wsIntervalId != -1) {
            clearInterval(this.wsIntervalId);
        }
        this.wsIntervalId = setInterval(this.SendMessage, 1000);
    },
    SendBinaryLoop() {
        if (this.wsIntervalId != -1) {
            clearInterval(this.wsIntervalId);
        }
        this.wsIntervalId = setInterval(this.SendBinary, 1000);
    },
    requestInStreamLoop() {
        if (this.httpUrl == null) {
            console.info("Please set the Http test URL first");
            return;
        }

        if (globalHttpRequest == null) {
            globalHttpRequest = http.createHttp();
        }
        this.httpIntervalId = setInterval(this.Stream, 2000);
    },
    Stream() {
        console.info('================== Stream TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        // 事件订阅 dataReceive: 连接开始时触发回调
        globalHttpRequest.on('dataReceive', (arrayBuffer) => {
            // console.info('dataReceive: arrayBuffer String------:' + JSON.stringify(str));
            console.info('dataReceive: arrayBufferSize-------:' + arrayBuffer.length);
        });
        // 事件订阅 dataEnd: 传输数据完毕时触发回调
        globalHttpRequest.on('dataEnd', () => {
            console.info('dataEnd: dataEnd----------------------------------');
        });
        // 事件订阅 dataReceiveProgress: 有限大小的数据请求，每次循环获取数据时从progress参数得到传输进度
        globalHttpRequest.on('dataReceiveProgress', (progress) => {
            console.info('dataReceiveProgress: receiveSize-------:' + JSON.stringify(progress.receiveSize));
            console.info('dataReceiveProgress: totalSize-------:' + JSON.stringify(progress.totalSize));
        });
        globalHttpRequest.requestInStream(
            this.httpUrl,
            {
                method: "GET",
                header: null,
                clientCert: null,
                extraData: null,
                expectDataType: 0,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                        console.info('Data transfer finished----------------------------------');
                        this.requestInProgress = false;
                        console.info('================== Stream TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        this.requestInProgress = false;
                        console.info('================== Stream TEST END ==================!');
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                    console.info('Http request in progress: data: ' + JSON.stringify(data.result));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    // console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    this.requestInProgress = false;
                    console.info('================== Stream TEST END ==================!');
                }
            }
        );
    },
    onHide() {
        if (globalHttpRequest != null) {
            globalHttpRequest.destroy();
        }
        if (this.wsIntervalId != -1) {
            clearInterval(this.wsIntervalId);
        }
        if (this.httpIntervalId != -1) {
            clearInterval(this.httpIntervalId);
        }
    },
}
