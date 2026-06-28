import netConnect from '@system.network';
import router from '@system.router';
export default {
    data: {
        type: 2, // 1 br, 2 ble, 3 btPan, 30 cat1
    },
    onInit() {
    },
    getConnectState() {
        this.GetConnectState(1);
        this.GetConnectState(2);
        this.GetConnectState(3);
        this.GetConnectState(30);
    },
    registerListener() {
        this.RegisterListener(1);
        this.RegisterListener(2);
        this.RegisterListener(3);
        this.RegisterListener(30);
    },
    unRegisterListener() {
        this.UnRegisterListener(1);
        this.UnRegisterListener(2);
        this.UnRegisterListener(3);
        this.UnRegisterListener(30);
    },
    getIpAddress() {
        this.GetIpAddress(0);
        this.GetIpAddress(3);
        this.GetIpAddress(4);
        this.GetIpAddress(5);
        this.GetIpAddress(30);
        this.GetIpAddress();
    },
    RegisterListener(type) {
        // 网络连接状态回调注册
        netConnect.registerConnectListener(type, (dataInfo) => {
            console.info("type:" + dataInfo.type + " state:" + dataInfo.state + " description:" + dataInfo.description);
        }
        );
        console.info("type: " + type + " callback register success");
    },
    UnRegisterListener(type) {
        // 网络连接状态回调注销
        netConnect.unregisterConnectListener(type);
        console.info("type: " + type + " callback unRegister success");
    },
    GetConnectState(type) {
        let status = netConnect.getConnectState(type);
        console.info("getConnectState type:" + status.type + " state:" + status.state + " status:" + status.description);
    },
    GetIpAddress(type) {
        let getIpAddr = (err, ipAddrArray) => {
            if (err != 0) {
                console.err('not find ip addr');
                return;
            }
            for (let i = 0; i < ipAddrArray.length; i++) {
                console.info('ip addr is ' + ipAddrArray[i]);
            }
        }

        if (type == undefined) {
            console.info('get ip addr of default net type');
            netConnect.getIpAddress(getIpAddr);
        } else {
            console.info('get ip addr of net type ' + type);
            netConnect.getIpAddress(type, getIpAddr);
        }
    },
    onHide() {
        this.unRegisterListener();
    },
    replaceHomePage() {
        router.replace({
            uri: 'pages/index/index',
        });
    },
};