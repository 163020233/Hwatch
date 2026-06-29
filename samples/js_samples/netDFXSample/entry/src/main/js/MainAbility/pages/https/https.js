import router from '@system.router';
import app from '@system.app';
import http from '@ohos.net.http';

let globalHttpRequest = null;
let sendData = new ArrayBuffer(1024);
let view = new Uint8Array(sendData);

export default {
    data:
    {
        putHttpUrl: '',  // only for test: https://httpbin.org/put
        getHttpUrl: '', // 后接数据长度，如https://httpbin.org/bytes/1024，表示返回一个 1024 字节的二进制数据块。
        streamHttpUrl: '', // 后接数据流次数，如https://httpbin.org/stream/10，表示每秒发送 1 个 JSON 字符串，共发送 10 个。
        postHttpUrl: '',  // only for test: https://httpbin.org/post
        start: null,
        end: null,
    },
    onInit() {
        this.initHttpRequest();
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
    initHttpRequest() {
        if (globalHttpRequest == null) {
            globalHttpRequest = http.createHttp();
            // 事件订阅 dataReceive: 连接开始时触发回调
            globalHttpRequest.on('dataReceive', (arrayBuffer) => {
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
        }
    },
    Get128B() {
        console.info('================== Get 128B TEST START ==================!');
        this.Get("128");
    },
    Get400B() {
        console.info('================== Get 400B TEST START ==================!');
        this.Get("400");
    },
    Get1KB() {
        console.info('================== Get 1KB TEST START ==================!');
        this.Get("1024");
    },
    Get4KB() {
        console.info('================== Get 4KB TEST START ==================!');
        this.Get("4096");
    },
    Get16KB() {
        console.info('================== Get 16KB TEST START ==================!');
        this.Get("16384");
    },
    Get64KB() {
        console.info('================== Get 64KB TEST START ==================!');
        this.Get("65536");
    },
    Put128B() {
        console.info('================== Put 128B TEST START ==================!');
        this.Put(view.slice(0,128).buffer, "128");
    },
    Put400B() {
        console.info('================== Put 400B TEST START ==================!');
        this.Put(view.slice(0,400).buffer, "400");
    },
    Put1KB() {
        console.info('================== Put 1KB TEST START ==================!');
        this.Put(sendData, "1024");
    },
    Put4KB() {
        console.info('================== Put 4KB TEST START ==================!');
        this.Put(this.repeatArrayBuffer(4), "1024");
    },
    Put16KB() {
        console.info('================== Put 16KB TEST START ==================!');
        this.Put(this.repeatArrayBuffer(16), "16384");
    },
    Put64KB() {
        console.info('================== Put 64KB TEST START ==================!');
        this.Put(this.repeatArrayBuffer(64), "65536");
    },
    Post128B() {
        console.info('================== Post 128B TEST START ==================!');
        this.Post(view.slice(0,128).buffer, "128");
    },
    Post400B() {
        console.info('================== Post 400B TEST START ==================!');
        this.Post(view.slice(0,400).buffer, "400");
    },
    Post1KB() {
        console.info('================== Post 1KB TEST START ==================!');
        this.Post(sendData, "1024");
    },
    Post4KB() {
        console.info('================== Post 4KB TEST START ==================!');
        this.Post(this.repeatArrayBuffer(4), "1024");
    },
    Post16KB() {
        console.info('================== Post 16KB TEST START ==================!');
        this.Post(this.repeatArrayBuffer(16), "16384");
    },
    Post64KB() {
        console.info('================== Post 64KB TEST START ==================!');
        this.Post(this.repeatArrayBuffer(64), "65536");
    },
    Stream128B() {
        console.info('================== Stream 128B TEST START ==================!');
        this.Stream("128");
    },
    Stream400B() {
        console.info('================== Stream 400B TEST START ==================!');
        this.Stream("400");
    },
    Stream1KB() {
        console.info('================== Stream 1KB TEST START ==================!');
        this.Stream("1024");
    },
    Stream4KB() {
        console.info('================== Stream 4KB TEST START ==================!');
        this.Stream("4096");
    },
    Stream16KB() {
        console.info('================== Stream 16KB TEST START ==================!');
        this.Stream("16384");
    },
    Stream64KB() {
        console.info('================== Stream 64KB TEST START ==================!');
        this.Stream("65536");
    },
    Download128B() {
        console.info('================== Download 128B TEST START ==================!');
        this.Download("128");
    },
    Download400B() {
        console.info('================== Download 400B TEST START ==================!');
        this.Download("400");
    },
    Download1KB() {
        console.info('================== Download 1KB TEST START ==================!');
        this.Download("1024");
    },
    Download4KB() {
        console.info('================== Download 4KB TEST START ==================!');
        this.Download("4096");
    },
    Download16KB() {
        console.info('================== Download 16KB TEST START ==================!');
        this.Download("16384");
    },
    Download64KB() {
        console.info('================== Download 64KB TEST START ==================!');
        this.Download("65536");
    },
    Upload128B() {
        console.info('================== Upload 128B TEST START ==================!');
        this.Upload("128");
    },
    Upload400B() {
        console.info('================== Upload 400B TEST START ==================!');
        this.Upload("400");
    },
    Upload1KB() {
        console.info('================== Upload 1KB TEST START ==================!');
        this.Upload("1024");
    },
    Upload4KB() {
        console.info('================== Upload 4KB TEST START ==================!');
        this.Upload("4096");
    },
    Upload16KB() {
        console.info('================== Upload 16KB TEST START ==================!');
        this.Upload("16384");
    },
    Upload64KB() {
        console.info('================== Upload 64KB TEST START ==================!');
        this.Upload("65536");
    },
    Get(length) { // 最大传输数据量没有限制
        this.start = Date.now();
        let url = this.getHttpUrl + length;
        globalHttpRequest.requestInStream(
            url,
            {
                method: "GET",
                clientCert: null,
                extraData: null,
                expectDataType: 2,
            },
            (err, data) => {
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Get is " + duration + " ms");
                        console.info('================== Get TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                }
            }
        );
    },
    Put(buffer, length) { // 最大传输数据量没有限制
        this.start = Date.now();
        globalHttpRequest.requestInStream(
            this.putHttpUrl,
            {
                method: "PUT",
                header: {
                    "Content-Type": "application/octet-stream",
                    "X-Upload-Size": length,
                },
                clientCert: null,
                extraData: buffer,
                // STRING:0 OBJECT:1 ARRAY_BUFFER:2
                expectDataType: 2
            },
            (err, data) => {
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Put is " + duration + " ms");
                        console.info('================== Put TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                }
            }
        );
    },
    Post(buffer, length) { // 最大传输数据量没有限制
        this.start = Date.now();
        globalHttpRequest.requestInStream(
            this.postHttpUrl,
            {
                method: "POST",
                header: {
                    "Content-Type": "application/octet-stream",
                },
                clientCert: null,
                extraData: buffer,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Stream is " + duration + " ms");
                        console.info('================== Stream TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                }
            }
        );
    },
    Stream(length) { // 最大传输数据量没有限制
        this.start = Date.now();
        let url = this.getHttpUrl + length;
        globalHttpRequest.requestInStream(
            url,
            {
                method: "GET",
                header: {
                    "Content-Type": "application/octet-stream",
                },
                clientCert: null,
                extraData: null,
                expectDataType: 2,
            },
            (err, data) => {
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Get is " + duration + " ms");
                        console.info('================== Stream TEST END ==================!');
                    } else {
                        console.error('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else if (err.code == 2300018) {
                    console.info('Http request in progress: responseCode: ' + JSON.stringify(data.responseCode));
                } else {
                    console.error('Http request Fail: err.code: ' + JSON.stringify(err.code));
                }
            }
        );
    },
    Download(length) { // 最大传输数据量没有限制
        this.start = Date.now();
        let url = this.getHttpUrl + length;
        let savePath = "internal://app/save" + length + ".txt"
        globalHttpRequest.download(
            url,
            {
                header: {
                    "Content-Type": "application/octet-stream",
                },
                filePath: savePath,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Download is " + duration + " ms");
                    } else {
                        console.info('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else {
                    console.info('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    console.info('Http request Fail: ' + data.result);
                }
                console.info('================== Download TEST END ==================!');
            }
        );
    },
    Upload(length) { // 最大支持上传2K数据
        this.start = Date.now();
        let readPath = "internal://app/save" + length + ".txt"
        globalHttpRequest.upload(
            this.postHttpUrl,
            {
                header: {
                    "Content-Type": "application/octet-stream",
                    "Content-Length": length,
                },
                filePath: readPath,
            },
            (err, data) => {
                console.info('Result CallBack');
                if (err.code == 0) {
                    if (data.responseCode == 200) {
                        console.info('Http request Success: responseCode: ' + JSON.stringify(data.responseCode));
                        this.end = Date.now();
                        let duration = this.end - this.start;
                        console.info("https Upload is " + duration + " ms");
                    } else {
                        console.info('Http request error: responseCode: ' + JSON.stringify(data.responseCode));
                    }
                } else {
                    console.info('Http request Fail: err.code: ' + JSON.stringify(err.code));
                    console.info('Http request Fail: ' + data.result);
                }
                console.info('================== Upload TEST END ==================!');
            }
        );
    },
    replaceWebsockets() {
        router.replace({
            uri: 'pages/websockets/websockets',
        });
    },

    replaceWebsocket() {
        router.replace({
            uri: 'pages/websocket/websocket',
        });
    },

    replaceHttp() {
        router.replace({
            uri: 'pages/http/http',
        });
    },

    onHide() {
        if (globalHttpRequest != null) {
            globalHttpRequest.destroy();
        }
    },
}
