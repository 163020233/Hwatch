import router from '@system.router';
import ble from '@ohos.bluetooth.ble';

export default {
    data:
    {
    },
    onInit() {
        console.info('onInit===================');
        this.OnBle();
    },
    OnBle() {
        function onScanReceiveEvent(data) {
            let value = new Uint8Array(data[0].data);
            console.info('BLE scan device find result = '+ JSON.stringify(data));
            console.info('BLE scan device find result = '+ value);
        }
        let ret = ble.on("BLEDeviceFind", onScanReceiveEvent);
        console.info("BLEDeviceFind on ret: "+ ret)
        function onAdvReceiveEvent(data) {
            console.info('bluetooth advertising state = ' + JSON.stringify(data));
        }
        ret = ble.on('advertisingStateChange', onAdvReceiveEvent);
        console.info("advertisingStateChange on ret: "+ ret)
    },
    OffBle() {
        function onReceiveEvent(data) {
            console.info('bluetooth advertising state = ' + JSON.stringify(data));
        }
        let ret = ble.off('advertisingStateChange', onReceiveEvent);
        console.info('advertisingStateChange off ret: '+ ret);
        function onReceiveEvent(code, data) {
            if (code != 0) {
                return
            }
            console.info('bluetooth device find = '+ JSON.stringify(data));
        }
        ret = ble.off('BLEDeviceFind', onReceiveEvent);
        console.info('BLEDeviceFind off ret: '+ ret);
    },
    replaceClient() {
        router.replace({
            uri: 'pages/client/client',
        });
    },
    replaceServer() {
        router.replace({
            uri: 'pages/server/server',
        });
    },
    onDestroy() {
        console.info('onDestroy===================');
    }
}