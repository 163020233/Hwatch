import router from '@system.router';
import app from '@system.app';
import http from '@ohos.net.http';
import webSocket from '@ohos.net.webSocket';

let globalHttpRequest1 = null;
let globalHttpRequest2 = null;
let globalHttpRequest3 = null;
let globalHttpRequest4 = null;

let globalWsClient1 = null;
let globalWsClient2 = null;
let globalWsClient3 = null;
let globalWsClient4 = null;


export default {
    data:
    {
        intervalId1: -1,
        wsUrl1: null,    // need URL that support establishing 4 connections with the same IP
        wsUrl2: null,    // only for test: wss://echo.websocket.org
        httpUrl: null,   // only for test: https://www.baidu.com/img/PCtm_d9c8750bed0b3c7d089fa7d55720d6cf.png
        stopHttpRequest:true,
        closeWsClient:true,
        path: "",
    },
    onInit() {
    },
    initHttpRequest() {
        if (globalHttpRequest1 == null) {
            globalHttpRequest1 = http.createHttp();
        }
        if (globalHttpRequest2 == null) {
            globalHttpRequest2 = http.createHttp();
        }
        if (globalHttpRequest3 == null) {
            globalHttpRequest3 = http.createHttp();
        }
        if (globalHttpRequest4 == null) {
            globalHttpRequest4 = http.createHttp();
        }
        this.stopHttpRequest = false;
    },
    initWsClient() {
        if (this.wsUrl1 == null) {
            console.info("Please set the wsUrl1 first");
            return;
        }

        if (globalWsClient1 == null) {
            globalWsClient1 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient1, this.wsUrl1);
        }
        if (globalWsClient2 == null) {
            globalWsClient2 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient2, this.wsUrl1);
        }
        if (globalWsClient3 == null) {
            globalWsClient3 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient3, this.wsUrl1);
        }
        if (globalWsClient4 == null) {
            globalWsClient4 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient4, this.wsUrl1);
        }

        this.closeWsClient = false;
    },
    initHttpAndWebSocket() {
        if (this.wsUrl2 == null || this.httpUrl == null) {
            console.info("Please set the httpUrl and wsUrl2 first");
            return;
        }

        if (globalHttpRequest1 == null) {
            globalHttpRequest1 = http.createHttp();
        }
        if (globalHttpRequest2 == null) {
            globalHttpRequest2 = http.createHttp();
        }
        if (globalWsClient1 == null) {
            globalWsClient1 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient1, this.wsUrl2);
        }
        if (globalWsClient2 == null) {
            globalWsClient2 = this.initWebSocket();
            this.WebSocketConnect(globalWsClient2, this.wsUrl2);
        }
        this.closeWsClient = false;
        this.stopHttpRequest = false;
    },
    stopHttpRequest() {
        this.stopHttpRequest = true;
        if (globalHttpRequest1 != null) {
            globalHttpRequest1.destroy();
            globalHttpRequest1 = null;
        }

        if (globalHttpRequest2 != null) {
            globalHttpRequest2.destroy();
            globalHttpRequest2 = null;
        }

        if (globalHttpRequest3 != null) {
            globalHttpRequest3.destroy();
            globalHttpRequest3 = null;
        }

        if (globalHttpRequest4 != null) {
            globalHttpRequest4.destroy();
            globalHttpRequest4 = null;
        }
        if (this.intervalId1 != -1) {
            clearInterval(this.intervalId1);
        }
        this.intervalId1 = -1;
        console.info('Stop All Http Request!');
    },
    closeWsClient() {
        this.closeWsClient = true;
        if (globalWsClient1 != null) {
            globalWsClient1.destroy((err) => {});
            globalWsClient1 = null;
        }
        if (globalWsClient2 != null) {
            globalWsClient2.destroy((err) => {});
            globalWsClient2 = null;
        }
        if (globalWsClient3 != null) {
            globalWsClient3.destroy((err) => {});
            globalWsClient3 = null;
        }
        if (globalWsClient4 != null) {
            globalWsClient4.destroy((err) => {});
            globalWsClient4 = null;
        }
        if (this.intervalId1 != -1) {
            clearInterval(this.intervalId1);
        }
        this.intervalId1 = -1;
        console.info('Close All Websocket Client!');
    },
    download() {
        if (this.httpUrl == null) {
            console.info("Please set the httpUrl first");
            return;
        }

        if (globalHttpRequest1 == null) {
            globalHttpRequest1 = http.createHttp();
        }
        if (globalHttpRequest2 == null) {
            globalHttpRequest2 = http.createHttp();
        }
        this.stopHttpRequest = false;
        this.Download(globalHttpRequest1, 5, 'test1.png', this.httpUrl);
        this.Download(globalHttpRequest2, 5, 'test2.png', this.httpUrl);
    },

    multiHttpRequest() {
        if (this.httpUrl == null) {
            console.info("Please set the httpUrl first");
            return;
        }

        this.initHttpRequest();
        this.Get(globalHttpRequest1);
        this.Get(globalHttpRequest2);
        this.Get(globalHttpRequest3);
        this.Get(globalHttpRequest4);
    },
    multiHttpRequestLoop() {
        if (this.httpUrl == null) {
            console.info("Please set the httpUrl first");
            return;
        }

        if (this.intervalId1 != -1) {
            clearInterval(this.intervalId1);
        }
        this.intervalId1 = setInterval(this.multiHttpRequest, 2000);
    },
    multiWebsocket() {
        this.SendMessage(globalWsClient1, 1);
        this.SendMessage(globalWsClient2, 2);
        this.SendMessage(globalWsClient3, 3);
        this.SendMessage(globalWsClient4, 4);
    },
    multiWebsocketLoop() {
        if (this.intervalId1 != -1) {
            clearInterval(this.intervalId1);
        }
        this.intervalId1 = setInterval(this.multiWebsocket, 2000);
    },
    httpAndWebSocketTest() {
        this.Get(globalHttpRequest1);
        this.Get(globalHttpRequest2);
        this.SendMessage(globalWsClient1, 1);
        this.SendMessage(globalWsClient2, 2);
    },
    httpAndWebSocketTestLoop() {
        if (this.intervalId1 != -1) {
            clearInterval(this.intervalId1);
        }
        this.intervalId1 = setInterval(this.httpAndWebSocketTest, 2000);
    },
    //下载图片
    Download(httpRequest, count, name, url) {
        console.info('================== Download TEST START ==================!');
        if (this.stopHttpRequest == true || count == 0) {
            return;
        }

        let filePath = 'internal://app/' + name + count;
        httpRequest.download(
            url,
            {
                header: null,
                filePath: filePath
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Data transfer finished----------------------------------');
                        console.info('================== Download TEST END ==================!');
                    } else {
                        console.info('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('================== Download TEST END ==================!');
                    }
                } else {
                    console.info('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    console.info('================== Download TEST END ==================!');
                }
                // 此处仅为验证递归场景。下载多个文件推荐使用循环，而非递归，避免嵌套过深。
                this.Download(httpRequest, --count, name, url);
            }
        );
    },
    Get(httpRequest) {
        console.info('================== Get TEST START ==================!');
        if (this.stopHttpRequest == true) {
            return;
        }
        httpRequest.request(
            // 请求大于2k的数据会报2300018
            this.httpUrl,
            {
                method: "GET",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: null,
                // STRING:0 OBJECT:1 ARRAY_BUFFER:2
                expectDataType: 0,
            },
            (err, data) => {
                console.info('Result CallBack errCode = ' + err.code);
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request data > 2KB');
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                console.info('================== Get TEST END ==================!');
            }
        );
    },
    initWebSocket()
    {
        let wsClient = webSocket.createWebSocket();
        if (wsClient != undefined) {
            wsClient.on('open', (err, value) => {
                console.info("JS: on open: status: " + JSON.stringify(value.status));
                console.info("JS: on open: message: " + JSON.stringify(value.message));
            });
            wsClient.on('message', (err, value) => {
                console.info("JS: on message:" + JSON.stringify(value));
            });
            wsClient.on('close', (err, value) => {
                console.info("JS: close, code is " + value.code);
                console.info("JS: close, reason is " + value.reason);
            });
            wsClient.on('error', (err, value) => {
            });
        } else {
            wsClient = null;
            console.info("JS: createWebsocket failed!");
        }
        return wsClient;
    },
    WebSocketConnect(wsClient, ip) {
        wsClient.connect(ip,
            (err, value) => {
                if (!err) {
                    console.info("JS: Successfully started the connect " + value);
                } else {
                    console.info("JS: Connection failed. Err:" + JSON.stringify(err.code));
                    console.info("JS: Connection failed. Err:" + JSON.stringify(err.message));
                }
            });
    },
    SendMessage(wsClient, id) {
        if (this.closeWsClient == true) {
            return;
        }
        wsClient.send("Hello, server! form client " + id, (err, value) => {
                if (!err) {
                    console.info("JS: Sent successfully");
                } else {
                    console.info("JS: Failed to send the message. Err:" + JSON.stringify(err));
                    console.info("JS: send failed code: " + err.code);
                    console.info("JS: send failed message: " + err.message);
                }
            });
    },
    onHide() {
        this.stopHttpRequest();
        this.closeWsClient();
    },
    replaceHomePage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}