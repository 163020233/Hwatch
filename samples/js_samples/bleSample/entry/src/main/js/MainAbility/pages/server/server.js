import router from '@system.router';
import ble from '@ohos.bluetooth.ble';

let device = null;
let server = null;
let advHandle = 0xFF;
let charval = 0x10;
let deviceId = null;
export default {
    data:
    {
    },
    onInit() {
    },
    startAdvertising() {
        let setting = {
            interval: 150,
            txPower: -7,
            connectable: true
        };
        let manuArray  = new Uint8Array(7);
        manuArray[0] = 0x50;
        manuArray[1] = 0x11;
        manuArray[2] = 0x22;
        manuArray[3] = 0x33;
        manuArray[4] = 0x44;
        manuArray[5] = 0x55;
        manuArray[6] = 0x66;
        let manufactureDataUnit = { // 长度1字节、类型1字节、Id 2字节、值长度7字节 == 11字节
            manufactureId: 0x574B,
            manufactureValue: manuArray.buffer,
        };
        let serviceValueArray  = new Uint8Array(2);
        serviceValueArray[0] = 0x2;
        serviceValueArray[1] = 0x3;
        let serviceDataUnit = { // 长度1字节 类型1字节 id 2字节 + 值2字节== 6字节
            serviceUuid: "1888",
            serviceValue: serviceValueArray.buffer,
        };
        let advData = {
            serviceUuids: [], // 长度1字节 类型1字节 id 2字节 == 4字节
            manufactureData: [manufactureDataUnit],
            serviceData: [],
            includeDeviceName: false,
        };
        let advResponse = {
            serviceUuids: ["1887"],
            manufactureData: [manufactureDataUnit],
            serviceData: [serviceDataUnit],
            includeDeviceName: true,
        };
        let ret = ble.startAdvertising(setting, advData, advResponse);
        console.info("startAdvertising ret:" + ret);
    },
    stopAdvertising() {
        let ret = ble.stopAdvertising();
        console.info("stopAdvertising ret:" + ret);
    },
    startAdvertisingWithCallback() {
        let setting = {
            interval: 160,
            txPower: -7,
            connectable: true
        };
        let manuArray  = new Uint8Array(2);
        manuArray[0] = 0x50;
        manuArray[1] = 0x11;
        let manufactureDataUnit = { // 长度1字节、类型1字节、Id 2字节、值长度7字节 == 11字节
            manufactureId: 0x574B,
            manufactureValue: manuArray.buffer,
        };
        let serviceDataUnit = {
            serviceUuid: "1326",
            serviceValue: [0x7, 0x7],
        };
        let advData = {
            serviceUuids: ["1992"],
            manufactureData: [manufactureDataUnit],
            serviceData: [serviceDataUnit],
            includeDeviceName: true,
        };
        let advResponse = {
            serviceUuids: ["1890"],
            manufactureData: [manufactureDataUnit],
            serviceData: [serviceDataUnit],
            includeDeviceName: true,
        };
        let advertisingParams = {
            advertisingSettings: setting,
            advertisingData: advData,
            duration: 0
        }
        ble.startAdvertising(advertisingParams, (err, outAdvHandle) => {
            if (err != 0) {
                console.info('startAdvertising callback failed');
                return;
            } else {
                advHandle = outAdvHandle;
                console.info("advHandle: " + advHandle);
            }
        });
    },
    stopAdvertisingWithCallback() {
        ble.stopAdvertising(advHandle, (err) => {
            if (err != 0) {
                console.info('stopAdvertising callback failed');
                return;
            }
        });
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
    OnServer() {
        console.info("CharacteristicRead start!")
        function CharacteristicRead(CharacteristicReadReq) {
            console.info('CharacteristicRead success: ' + JSON.stringify(CharacteristicReadReq));
        }
        server.on('characteristicRead', CharacteristicRead);
        console.info("CharacteristicWrite start!")
        function CharacteristicWrite(characteristicWriteReq) {
            console.info('CharacteristicWrite success: ' + JSON.stringify(characteristicWriteReq));
            // 读取ArrayBuffer内容
            const buffer = characteristicWriteReq.value;
            console.info('value length:'+ buffer.byteLength); // 先打印长度，确认是否有数据
            const uint8View = new Uint8Array(buffer);
            console.info('value content:'+ uint8View);
        }
        server.on('CharacteristicWrite', CharacteristicWrite);
        console.info("DescriptorRead start!")
        function DescriptorRead(descriptorReadReq) {
            console.info('DescriptorRead success: ' + JSON.stringify(descriptorReadReq));
        }
        server.on('descriptorRead', DescriptorRead);
        console.info("DescriptorWrite start!")
        function DescriptorWrite(descriptorWriteReq) {
            console.info('DescriptorWrite success: ' + JSON.stringify(descriptorWriteReq));
            // 读取ArrayBuffer内容
            const buffer = descriptorWriteReq.value;
            console.info('value length:'+ buffer.byteLength); // 先打印长度，确认是否有数据
            const uint8View = new Uint8Array(buffer);
            console.info('value content:'+ uint8View);
        }
        server.on('DescriptorWrite', DescriptorWrite);
        console.info("ConnectionStateChange start!")
        function ConnectionStateChange(code, state) {
            if (code != 0) {
                console.error('ConnectionStateChange failed!');
                return
            }
            console.info('bluetooth connect state changed');
            let connectState = state.state;
            console.info('bluetooth connect state value:' + connectState);
            deviceId = state.deviceId;
            console.info('bluetooth connect deviceId:' + deviceId);
        }
        server.on('connectionStateChange', ConnectionStateChange);
        console.info("BLEMtuChange start!")
        function BLEMtuChange(mtu) {
            console.info('BLEMtuChange success: ' + mtu);
        }
        server.on('BLEMtuChange', BLEMtuChange);
    },
    createGattServer() {
        console.info("create server start!")
        server = ble.createGattServer();
        if (server == undefined) {
            console.info("create server failed!")
        }
        this.OnServer();
    },
    addService() {
        this.createGattServer();
        let descriptors = [];
        let arrayBuffer = new ArrayBuffer(2);
        let descV = new Uint8Array(arrayBuffer);
        descV[0] = 0x01;
        descV[1] = 0x10;
        let descriptor = {
            serviceUuid: '180f',
            characteristicUuid: '2a19',
            descriptorUuid: '2902',
            descriptorValue: arrayBuffer,
        };
        descriptors[0] = descriptor;
        let arrayBufferC = new ArrayBuffer(1);
        let cccV = new Uint8Array(arrayBufferC);
        cccV[0] = 0x58;
        let propertie = {
            write: true,
            writeNoResponse: true,
            read: true,
            notify: true,
            indicate: true,
        };
        let characteristic = {
            serviceUuid: '180f',
            characteristicUuid: '2a19',
            characteristicValue: arrayBufferC,
            descriptors: descriptors,
            properties: propertie,
        };
        let serviceIn = {
            serviceUuid: '180f',
            isPrimary: true,
            characteristics: [characteristic],
            includeServices	: [],
        };

        server.addService(serviceIn, (err, resultService) => {
            if (err != 0) {
                console.info('addService callback failed');
            } else {
                console.info('Add service success: ' + JSON.stringify(resultService));
                this.serviceHandle = resultService;
                if (resultService.characteristics && resultService.characteristics.length > 0) {
                    resultService.characteristics.forEach(char => {
                        console.info(`Characteristic ${char.characteristicUuid} handle: ${char.characteristicValueHandle}`);
                    });
                }
            }
        });
    },
    removeServer() {
        server.removeService(this.serviceHandle.serviceUuid, (err) => {
            if (err) {
                console.error('Remove service failed: ' + err);
            } else {
                console.info('Remove service success');
            }
        });
    },
    closeServer() {
        this.serviceHandle = null;
        this.OffServer();
        let ret = server.close();
        console.info("close ret" + ret);
    },
    sendNotification() {
        let bufferDesc = new ArrayBuffer(1);
        let uint8View = new Uint8Array(bufferDesc);
        uint8View[0] = charval;
        let notifyCharacteristic = {
            serviceUuid: '180f',
            characteristicUuid: '2a19',
            characteristicValue: bufferDesc,
            confirm: true // false = 通知, true = 指示
        };
        // 连接回调返回大端deviceid，传大端deviceid，对端设备的deviceid可能变化
        server.notifyCharacteristicChanged(deviceId, notifyCharacteristic, (err) => {
            if (err) {
                console.error('Send notification failed for device ' + ': ' + err);
            } else {
                charval++;
                console.info('Notification sent successfully to device:');
            }
        });
    },
    OffServer() {
        server.off('characteristicRead');
        server.off('characteristicWrite');
        server.off('descriptorRead');
        server.off('descriptorWrite');
        server.off('connectionStateChange');
        server.off('BLEMtuChange');
    },
    onDestroy() {
        console.info('onDestroy===================');
        this.OffBle();
        device.off('BLECharacteristicChange');
        device.off('BLEConnectionStateChange');
    }
}