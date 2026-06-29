import router from '@system.router';
import app from '@system.app';
import http from '@ohos.net.http';
import file from '@system.file';

let globalHttpRequest = null;

export default {
    data:
    {
        httpRequestUrl: null,  // only for test: https://httpbin.org/anything
        httpStreamUrl: null,   // only for test: https://www.baidu.com/img/PCtm_d9c8750bed0b3c7d089fa7d55720d6cf.png
        httpDownloadUrl: null, // only for test: https://www.baidu.com/img/PCtm_d9c8750bed0b3c7d089fa7d55720d6cf.png
        httpUploadUrl: null,   // need URL that support upload
        intervalId: -1,
        requestInProgress: false,
        path: ""
    },
    onInit() {
    },
    stopDataReceive() {
        this.requestInProgress = false;
        globalHttpRequest?.destroy();
        globalHttpRequest = null;
    },
    getRequest() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        this.Get();
    },
    getRequestLoop() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Get, 2000);
    },
    putRequest() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        this.Put();
    },
    putRequestLoop() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Put, 2000);
    },
    deleteRequest() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        this.Delete();
    },
    deleteRequestLoop() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Delete, 2000);
    },
    postRequest() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        this.Post();
    },
    postRequestLoop() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Post, 2000);
    },
    headRequest() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        this.Head();
    },
    headRequestLoop() {
        if (this.httpRequestUrl == null) {
            console.info("Please set the httpRequestUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Head, 2000);
    },
    download() {
        if (this.httpDownloadUrl == null) {
            console.info("Please set the httpDownloadUrl first");
            return;
        }

        this.initHttpRequest();
        this.Download();
    },
    upload() {
        if (this.httpUploadUrl == null) {
            console.info("Please set the httpUploadUrl first");
            return;
        }

        this.initHttpRequest();
        let context = this;
        file.get({
            uri: "internal://app/baidu.png",
            success: function(data) {
                console.log('list call success ' + JSON.stringify(data.fileList));
                context.Upload(data.uri, data.length);
            },
            fail: function(data, code) {
                console.log('list call fail');
            },
        });
    },
    requestInStream() {
        if (this.httpStreamUrl == null) {
            console.info("Please set the httpStreamUrl first");
            return;
        }

        this.initHttpRequest();
        this.Stream();
    },
    requestInStreamOnce() {
        if (this.httpStreamUrl == null) {
            console.info("Please set the httpStreamUrl first");
            return;
        }

        this.initHttpRequest();
        this.StreamOnce();
    },
    requestInStreamOff() {
        if (this.httpStreamUrl == null) {
            console.info("Please set the httpStreamUrl first");
            return;
        }

        this.initHttpRequest();
        this.StreamOff();
    },
    requestInStreamLoop() {
        if (this.httpStreamUrl == null) {
            console.info("Please set the httpStreamUrl first");
            return;
        }

        this.initHttpRequest();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
        this.intervalId = setInterval(this.Stream, 3000);
    },
    initHttpRequest() {
        if (globalHttpRequest == null) {
            globalHttpRequest = http.createHttp();
        }
    },

    Get() {
        console.info('================== Get TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        let httpRequest = globalHttpRequest;
        httpRequest.request(
            // 请求大于2k的数据会报2300018
            this.httpRequestUrl,
            {
                method: "GET",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: null,
                // STRING:0 OBJECT:1 ARRAY_BUFFER:2
                expectDataType: 0
            },
            (err, data) => {
                console.info('Result CallBack errCode = ' + err.code);
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    }
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                this.requestInProgress = false;
                console.info('================== Get TEST END ==================!');
            }
        );
    },
    Put() {
        console.info('================== Put TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        let httpRequest = globalHttpRequest;
        httpRequest.request(
            this.httpRequestUrl,
            {
                method: "PUT",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: "{ \"name\": \"www\", \"email\": \"111@qq.com\", \"age\": 30 }",
                // STRING:0 OBJECT:1 ARRAY_BUFFER:2
                expectDataType: 2
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    }
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                this.requestInProgress = false;
                console.info('================== Put TEST END ==================!');
            }
        );
    },
    Delete() {
        console.info('================== Delete TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        let httpRequest = globalHttpRequest;
        httpRequest.request(
            this.httpRequestUrl,
            {
                method: "DELETE",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: null,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    }
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                this.requestInProgress = false;
                console.info('================== Delete TEST END ==================!');
            }
        );
    },
    Post() {
        console.info('================== Post TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        let httpRequest = globalHttpRequest;
        httpRequest.request(
            this.httpRequestUrl,
            {
                method: "POST",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: "{ \"name\": \"xyz\", \"password\": \"111\" }",
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    }
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                this.requestInProgress = false;
                console.info('================== Post TEST END ==================!');
            }
        );
    },
    Head() {
        console.info('================== Head TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        let httpRequest = globalHttpRequest;
        httpRequest.request(
            this.httpRequestUrl,
            {
                method: "HEAD",
                header: {
                    ParameterOne: "",
                    ParameterTwo: "",
                },
                clientCert: null,
                extraData: null,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Success: data: ' + JSON.stringify(data.result));
                    } else {
                        console.info('Http request Fail: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    }
                } else {
                    console.info('Connect Error, please check network connection, errCode = ' + err.code);
                }
                this.requestInProgress = false;
                console.info('================== Head TEST END ==================!');
            }
        );

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
            this.httpStreamUrl,
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
    // once功能测试
    StreamOnce() {
        console.info('================== StreamOnce TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }

        // 事件订阅 dataReceive: 连接开始时触发回调
        globalHttpRequest.once('dataReceive', (arrayBuffer) => {
            // console.info('dataReceive: arrayBuffer String------:' + JSON.stringify(str));
            console.info('dataReceive: arrayBufferSize-------:' + arrayBuffer.length);
        });
        // 事件订阅 dataEnd: 传输数据完毕时触发回调
        globalHttpRequest.once('dataEnd', () => {
            console.info('dataEnd: dataEnd----------------------------------');
        });
        // 事件订阅 dataReceiveProgress: 有限大小的数据请求，每次循环获取数据时从progress参数得到传输进度
        globalHttpRequest.once('dataReceiveProgress', (progress) => {
            console.info('dataReceiveProgress: receiveSize-------:' + JSON.stringify(progress.receiveSize));
            console.info('dataReceiveProgress: totalSize-------:' + JSON.stringify(progress.totalSize));
        });
        globalHttpRequest.requestInStream(
            this.httpStreamUrl,
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
                        console.info('================== StreamOnce TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        this.requestInProgress = false;
                        console.info('================== StreamOnce TEST END ==================!');
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                    console.info('Http request in progress: data: ' + JSON.stringify(data.result));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    // console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    this.requestInProgress = false;
                    console.info('================== StreamOnce TEST END ==================!');
                }
            }
        );
    },
    // off功能测试，打开再关闭所有订阅
    StreamOff() {
        console.info('================== StreamOff TEST START ==================!');
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
        globalHttpRequest.off('dataReceive');
        globalHttpRequest.off('dataEnd');
        globalHttpRequest.off('dataReceiveProgress');

        globalHttpRequest.requestInStream(
            this.httpStreamUrl,
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
                        console.info('================== StreamOff TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        this.requestInProgress = false;
                        console.info('================== StreamOff TEST END ==================!');
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                    console.info('Http request in progress: data: ' + JSON.stringify(data.result));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    // console.info('Http request Fail: data: ' + JSON.stringify(data.result));
                    this.requestInProgress = false;
                    console.info('================== StreamOff TEST END ==================!');
                }
            }
        );
    },
    //下载图片
    Download() {
        console.info('================== Download TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }
        globalHttpRequest.download(
            this.httpDownloadUrl,
            {
                header: null,
                filePath: "internal://app/baidu.png"
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Data transfer finished----------------------------------');
                        this.requestInProgress = false;
                        console.info('================== Download TEST END ==================!');
                    } else {
                        console.info('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        this.requestInProgress = false;
                        console.info('================== Download TEST END ==================!');
                    }
                } else {
                    console.info('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    console.info('Http request Fail: ' + data.result);
                    this.requestInProgress = false;
                    console.info('================== Download TEST END ==================!');
                }
            }
        );
    },
    Upload(uri, length) {
        console.info('================== Upload TEST START ==================!');
        if (this.requestInProgress) {
            console.info('Can not send Request, another request in progress!');
            return;
        } else {
            this.requestInProgress = true;
        }

        globalHttpRequest.upload(
            this.httpUploadUrl,
            {
                header: {
                    "Content-Type": "image/png",
                    "Content-Length": String(length),
                },
                filePath: uri
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        console.info('Data transfer finished----------------------------------');
                        this.requestInProgress = false;
                        console.info('================== Upload TEST END ==================!');
                    } else {
                        console.info('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                        this.requestInProgress = false;
                        console.info('================== Upload TEST END ==================!');
                    }
                } else {
                    console.info('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    console.info('Http request Fail: ' + data.result);
                    this.requestInProgress = false;
                    console.info('================== Upload TEST END ==================!');
                }
            }
        );
    },
    onHide() {
        globalHttpRequest.destroy();
        if (this.intervalId != -1) {
            clearInterval(this.intervalId);
        }
    },
    replaceHomePage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}