import router from '@system.router';
import ble from '@ohos.bluetooth.ble';

let device = null;
let deviceId = '1c:1d:2b:d2:5a:d2';
let descBuffer = new ArrayBuffer(2);
let descArray = new Uint8Array(descBuffer);
let descriptor = {
    serviceUuid: '180f',
    characteristicUuid: '2a19',
    descriptorUuid: '2902',
    descriptorValue: descBuffer,
};
let characterBuffer = new ArrayBuffer(1);
let characterArray = new Uint8Array(characterBuffer);
let characteristic = {
    serviceUuid: '180f',
    characteristicUuid: '2a19',
    characteristicValue: characterBuffer,
    descriptors: [descriptor],
    properties: {
        write: true,
        writeNoResponse: true,
        read: true,
        notify: true,
        indicate: true,
    },
};

// let deviceId = 'd2:95:b4:0f:a9:b9';
// let descBuffer = new ArrayBuffer(2);
// let descArray = new Uint8Array(descBuffer);
// let descriptor = {
//     serviceUuid: '1812',
//     characteristicUuid: '2a4d',
//     descriptorUuid: '2902',
//     descriptorValue: descBuffer,
//     descriptorHandle:143
// };
// let characterBuffer = new ArrayBuffer(4);
// let characterArray = new Uint8Array(characterBuffer);
// let characteristic = {
//     serviceUuid: '1812',
//     characteristicUuid: '2a32',
//     characteristicValue: characterBuffer,
//     descriptors: [],
//     properties: {
//         write: true,
//         writeNoResponse: true,
//         read: true,
//         notify: false,
//         indicate: false,
//     },
//     characteristicValueHandle: 148,
// };
// let descBuffer = new ArrayBuffer(2);
// let descArray = new Uint8Array(descBuffer);
// let descriptor = {
//     serviceUuid: '1812',
//     characteristicUuid: '2a4d',
//     descriptorUuid: '2902',
//     descriptorValue: descBuffer,
//     descriptorHandle:143
// };
// let characterBuffer = new ArrayBuffer(4);
// let characterArray = new Uint8Array(characterBuffer);
// let characteristic = {
//     serviceUuid: '1812',
//     characteristicUuid: '2a4d',
//     characteristicValue: characterBuffer,
//     descriptors: [],
//     properties: {
//         write: true,
//         writeNoResponse: true,
//         read: true,
//         notify: true,
//         indicate: true,
//     },
//     characteristicValueHandle: 141,
// };
export default {
    data:
    {
    },
    onInit() {
    },
    getMtu() {
        console.info("mtu============" + device.Mtu);
    },
    getDeviceMtu() {
        device.getDeviceMtu(deviceId, (err, data)=> {
            console.info('get device mtu err ' + JSON.stringify(err));
            console.info('mtu: ' + JSON.stringify(data));
        });
    },
    getDeviceNameAndRssiValueWithoutDeviceId() {
        device.getDeviceName((err, data) => {
            console.info('device name err ' + JSON.stringify(err));
            console.info('device name' + JSON.stringify(data));
        });
        // getRssiValue成功，打印对应的rssi
        device.getRssiValue((err, rssi)=> {
            console.info('rssi err ' + JSON.stringify(err));
            console.info('rssi value' + JSON.stringify(rssi));
        });
    },
    getDeviceNameAndRssiValueWithDeviceId() {
        device.getDeviceName((err, data) => {
            console.info('device name err ' + JSON.stringify(err));
            console.info('device name' + JSON.stringify(data));
        }, deviceId);
        // getRssiValue成功，打印对应的rssi
        device.getRssiValue((err, rssi)=> {
            console.info('rssi err ' + JSON.stringify(err));
            console.info('rssi value' + JSON.stringify(rssi));
        }, deviceId);
    },
    startBLEScanWithOnBLEDeviceFind() {
        let scanFilter = {
            deviceId: deviceId,
        };
        let scanOptions = {
            interval: 48, // 300 太大
            dutyMode: 0,
            matchMode: 1,
            phyType: 1,
            reportMode: 1,
        }
        let ret = ble.startBLEScan([scanFilter],scanOptions);
        console.info('startBLEScan ret: '+ ret);
    },
    stopBLEScan() {
        let ret = ble.stopBLEScan();
        console.info('stopBLEScan ret: '+ ret);
    },
    createGattClientWithDeviceId() {
        // 测试对端deviceID: d2:95:b4:0f:a9:b9
        device = ble.createGattClientDevice(deviceId);
        if (device == undefined) {
            console.info("create device failed!")
        }
        this.OnDevice();
    },
    createGattClientWithoutDeviceId() {
        device = ble.createGattClientDevice();
        if (device == undefined) {
            console.info("create device failed!")
        }
        this.OnDevice();
    },
    connectWithDeviceId() {
        this.createGattClientWithoutDeviceId();
        let ret = device.connect(deviceId);
        console.info("connect ret" + ret);
    },
    connectWithoutDeviceId() {
        this.createGattClientWithDeviceId();
        let ret = device.connect();
        console.info("connect ret" + ret);
    },
    disconnectWithDeviceId() {
        let ret = device.disconnect(deviceId);
        console.info("disconnect ret" + ret);
    },
    disconnectWithoutDeviceId() {
        let ret = device.disconnect();
        console.info("disconnect ret" + ret);
    },
    close() {
        this.OffDevice();
        let ret = device.close();
        console.info("close ret" + ret);
    },
    getServicesWithDeviceId() {
        let getServices = (code, gattServices) => {
            if (code != 0) {
                console.info('bluetooth code is ' + code);
                return;
            }
            let services = gattServices;
            console.info('bluetooth services size is ', services.length);
            for (let i = 0; i < services.length; i++) {
                console.info('==============services start===========================');
                console.info('bluetooth serviceUuid is ' + services[i].serviceUuid);
                console.info('bluetooth services.characteristics size is ', services[i].characteristics.length);
                for (let j = 0; j < services[i].characteristics.length; j++) {
                    console.info('==========characteristics start==================');
                    console.info('bluetooth services.characteristics serviceUuid is ' + services[i].characteristics[j].serviceUuid);
                    console.info('bluetooth services.characteristics characteristicUuid is ' + services[i].characteristics[j].characteristicUuid);
                    console.info('bluetooth services.characteristics proporty write is ' + services[i].characteristics[j].properties.write);
                    console.info('bluetooth services.characteristics proporty writeNoResponse is ' + services[i].characteristics[j].properties.writeNoResponse);
                    console.info('bluetooth services.characteristics proporty read is ' + services[i].characteristics[j].properties.read);
                    console.info('bluetooth services.characteristics proporty notify is ' + services[i].characteristics[j].properties.notify);
                    console.info('bluetooth services.characteristics proporty indicate is ' + services[i].characteristics[j].properties.indicate);
                    console.info('bluetooth services.characteristics handle is ' + services[i].characteristics[j].characteristicValueHandle);
                    console.info('bluetooth services.decreptor size is ' + services[i].characteristics[j].descriptors.length);
                    for (let h = 0; h < services[i].characteristics[j].descriptors.length; h++) {
                        console.info('================descriptors start===============================');
                        console.info('bluetooth services.characteristics.descriptors serviceUuid is ' + services[i].characteristics[j].descriptors[h].serviceUuid);
                        console.info('bluetooth services.characteristics.descriptors characteristicUuid is ' + services[i].characteristics[j].descriptors[h].characteristicUuid);
                        console.info('bluetooth services.characteristics.descriptors descriptorUuid is ' + services[i].characteristics[j].descriptors[h].descriptorUuid);
                        console.info('bluetooth services.characteristics.descriptors descriptorHandle is ' + services[i].characteristics[j].descriptors[h].descriptorHandle);
                        console.info('===========descriptors end==========================' + '=service index=' + i + '=characteristic index=' + j + '=descriptors index='+ h)
                    }
                    console.info('================characteristics end=========================='+ '=service index=' + i + '=characteristic index=' + j);
                }
                console.info('=================services end================================' + '=service index=' + i);
            }
        }
        device.getServices(getServices, deviceId);
    },
    getServicesWithoutDeviceId() {
        let getServices = (code, gattServices) => {
            if (code != 0) {
                console.info('bluetooth code is ' + code);
                return;
            }
            let services = gattServices;
            console.info('bluetooth services size is ', services.length);
            for (let i = 0; i < services.length; i++) {
                console.info('==============services start===========================');
                console.info('bluetooth serviceUuid is ' + services[i].serviceUuid);
                console.info('bluetooth services.characteristics size is ', services[i].characteristics.length);
                for (let j = 0; j < services[i].characteristics.length; j++) {
                    console.info('==========characteristics start==================');
                    console.info('bluetooth services.characteristics serviceUuid is ' + services[i].characteristics[j].serviceUuid);
                    console.info('bluetooth services.characteristics characteristicUuid is ' + services[i].characteristics[j].characteristicUuid);
                    console.info('bluetooth services.characteristics proporty write is ' + services[i].characteristics[j].properties.write);
                    console.info('bluetooth services.characteristics proporty writeNoResponse is ' + services[i].characteristics[j].properties.writeNoResponse);
                    console.info('bluetooth services.characteristics proporty read is ' + services[i].characteristics[j].properties.read);
                    console.info('bluetooth services.characteristics proporty notify is ' + services[i].characteristics[j].properties.notify);
                    console.info('bluetooth services.characteristics proporty indicate is ' + services[i].characteristics[j].properties.indicate);
                    console.info('bluetooth services.characteristics handle is ' + services[i].characteristics[j].characteristicValueHandle);
                    console.info('bluetooth services.decreptor size is ' + services[i].characteristics[j].descriptors.length);
                    for (let h = 0; h < services[i].characteristics[j].descriptors.length; h++) {
                        console.info('================descriptors start===============================');
                        console.info('bluetooth services.characteristics.descriptors serviceUuid is ' + services[i].characteristics[j].descriptors[h].serviceUuid);
                        console.info('bluetooth services.characteristics.descriptors characteristicUuid is ' + services[i].characteristics[j].descriptors[h].characteristicUuid);
                        console.info('bluetooth services.characteristics.descriptors descriptorUuid is ' + services[i].characteristics[j].descriptors[h].descriptorUuid);
                        console.info('bluetooth services.characteristics.descriptors descriptorHandle is ' + services[i].characteristics[j].descriptors[h].descriptorHandle);
                        console.info('===========descriptors end==========================' + '=service index=' + i + '=characteristic index=' + j + '=descriptors index='+ h)
                    }
                    console.info('================characteristics end=========================='+ '=service index=' + i + '=characteristic index=' + j);
                }
                console.info('=================services end================================' + '=service index=' + i);
            }
        }
        device.getServices(getServices);
    },
    writeCharacteristicValueWriteLoop() { // 读写成对校
        let getServices = (code, gattServices) => {
            if (code != 0) {
                console.info('bluetooth code is ' + code);
                return;
            }
            let services = gattServices;
            console.info('bluetooth services size is ', services.length);
            const intervalId = setInterval(() => {
                function writeCharacteristicValueCallBack(code) {
                    if (code != 0) {
                        console.info('writeCharacteristicValueCallBack ret: ' + code);
                        this.disconnect();
                        console.error('end intervalId:' + this.intervalId);
                        clearInterval(intervalId);
                        return;
                    }
                    console.info('bluetooth writeCharacteristicValue success');
                }
                characterArray[0] = 0x10;
                device.writeCharacteristicValue(characteristic, 1, writeCharacteristicValueCallBack);
            }, 2000);
            console.error('start intervalId:' + intervalId);
        }
        function ConnectStateChanged(code, state) {
            if (code != 0) {
                console.error('ConnectStateChanged failed!');
                this.close();
                return
            }
            console.info('bluetooth connect state changed');
            let connectState = state.state;
            console.info('bluetooth connect state value:' + connectState);
            if (connectState == 2) {
                device.getServices(getServices);
            } else {
                clearInterval(this.intervalId);
            }
        }
        device.on('BLEConnectionStateChange', ConnectStateChanged);
        this.connectWithDeviceId();
    },
    writeCharacteristicValueWriteWithDeviceId() { // 读写成对校验
        function writeCharacteristicValueCallBack(code) {
            if (code != null) {
                console.info('writeCharacteristicValueCallBack ret: ' + code);
                return;
            }
            console.info('bluetooth writeCharacteristicValue success');
        }
        characterArray[0] = 0x11;
        device.writeCharacteristicValue(characteristic, 1, writeCharacteristicValueCallBack, deviceId);
    },
    writeCharacteristicValueWriteWithoutDeviceId() { // 读写成对校验
        function writeCharacteristicValueCallBack(code) {
            if (code != null) {
                console.info('writeCharacteristicValueCallBack ret: ' + code);
                return;
            }
            console.info('bluetooth writeCharacteristicValue success');
        }
        characterArray[0] = 0x12;
        device.writeCharacteristicValue(characteristic, 1, writeCharacteristicValueCallBack);
    },
    readCharacteristicValueWithDeviceId() { // 读写成对校验
        function readCcc(code, BLECharacteristic) {
            if (code != 0) {
                console.info('readCcc ret: ' + code);
                return;
            }
            console.info('bluetooth characteristic uuid: ' + BLECharacteristic.characteristicUuid);
            console.info('bluetooth characteristic handle: ' + BLECharacteristic.characteristicValueHandle);
            let value = new Uint8Array(BLECharacteristic.characteristicValue);
            console.info('bluetooth characteristic value: ' + value[0] + ',' + value[1] + ',' + value[2] + ',' +
            value[3]);
        }

        device.readCharacteristicValue(characteristic, readCcc, deviceId);
    },
    readCharacteristicValueWithoutDeviceId() { // 读写成对校验
        function readCcc(code, BLECharacteristic) {
            if (code != 0) {
                console.info('readCcc ret: ' + code);
                return;
            }
            console.info('bluetooth characteristic uuid: ' + BLECharacteristic.characteristicUuid);
            console.info('bluetooth characteristic handle: ' + BLECharacteristic.characteristicValueHandle);
            let value = new Uint8Array(BLECharacteristic.characteristicValue);
            console.info('bluetooth characteristic value: ' + value[0] + ',' + value[1] + ',' + value[2] + ',' +
            value[3]);
        }
        device.readCharacteristicValue(characteristic, readCcc);
    },
    writeCharacteristicValueWriteNoResponseWithDeviceId() {
        function writeCharacteristicValueCallBack(code) {
            if (code != null) {
                console.info('writeCharacteristicValueCallBack ret: ' + code);
                return;
            }
            console.info('bluetooth writeCharacteristicValue success');
        }
        characterArray[0] = 0x13;
        device.writeCharacteristicValue(characteristic, 2, writeCharacteristicValueCallBack, deviceId);
    },
    writeCharacteristicValueWriteNoResponseWithoutDeviceId() {
        function writeCharacteristicValueCallBack(code) {
            if (code != null) {
                console.info('writeCharacteristicValueCallBack ret: ' + code);
                return;
            }
            console.info('bluetooth writeCharacteristicValue success');
        }
        characterArray[0] = 0x14;
        device.writeCharacteristicValue(characteristic, 2, writeCharacteristicValueCallBack);
    },

    readDescriptorValueWithDeviceId() {
        function readDesc(code, BLEDescriptor) {
            if (code != 0) {
                console.info('readDesc ret: ' + code);
                return;
            }
            console.info('bluetooth descriptor uuid: ' + BLEDescriptor.descriptorUuid);
            console.info('bluetooth descriptor handle: ' + BLEDescriptor.descriptorHandle);
            let value = new Uint8Array(BLEDescriptor.descriptorValue);
            console.info('bluetooth descriptor value: ' + value[0] + ',' + value[1]);
        }
        device.readDescriptorValue(descriptor, readDesc, deviceId);
    },
    readDescriptorValueWithoutDeviceId() {
        function readDesc(code, BLEDescriptor) {
            if (code != 0) {
                console.info('readDesc ret: ' + code);
                return;
            }
            console.info('bluetooth descriptor uuid: ' + BLEDescriptor.descriptorUuid);
            console.info('bluetooth descriptor handle: ' + BLEDescriptor.descriptorHandle);
            let value = new Uint8Array(BLEDescriptor.descriptorValue);
            console.info('bluetooth descriptor value: ' + value[0] + ',' + value[1]);
        }
        device.readDescriptorValue(descriptor, readDesc);
    },

    writeDescriptorValueWithDeviceId() {
        descArray[0] = 0x00;
        device.writeDescriptorValue(descriptor, (err) => {
            if (err != 0) {
                console.info('writeDescriptorValue callback failed');
            } else {
                console.info('writeDescriptorValue callback successful');
            }
        }, deviceId);
    },
    writeDescriptorValueWithoutDeviceId() {
        descArray[0] = 0x01;
        device.writeDescriptorValue(descriptor, (err) => {
            if (err != 0) {
                console.info('writeDescriptorValue callback failed');
            } else {
                console.info('writeDescriptorValue callback successful');
            }
        });
    },

    setCharacteristicChangeNotificationWithDeviceId() {
        descArray[0] = 0x01;
        device.setCharacteristicChangeNotification(characteristic, true, (err) => {
            if (err != 0) {
                console.info('notifyCharacteristicChanged callback failed');
            } else {
                console.info('notifyCharacteristicChanged callback successful');
            }
        }, deviceId);
    },
    setCharacteristicChangeNotificationWithoutDeviceId() {
        descArray[0] = 0x01;
        device.setCharacteristicChangeNotification(characteristic, true, (err) => {
            if (err != 0) {
                console.info('notifyCharacteristicChanged callback failed');
            } else {
                console.info('notifyCharacteristicChanged callback successful');
            }
        });
    },
    setCharacteristicChangeIndicationWithDeviceId() {
        descArray[0] = 0x01;
        device.setCharacteristicChangeIndication(characteristic, true, (err) => {
            if (err != 0) {
                console.info('setCharacteristicChangeIndication callback failed');
            } else {
                console.info('setCharacteristicChangeIndication callback successful');
            }
        }, deviceId);
    },
    setCharacteristicChangeIndicationWithoutDeviceId() {
        descArray[0] = 0x01;
        device.setCharacteristicChangeIndication(characteristic, true, (err) => {
            if (err != 0) {
                console.info('setCharacteristicChangeIndication callback failed');
            } else {
                console.info('setCharacteristicChangeIndication callback successful');
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
    OnDevice() {
        function CharacteristicChange(code, deviceId, characteristicChangeReq) {
            if (code != 0) {
                console.error('CharacteristicChange failed!');
                return
            }
            console.info("deviceId: "+ deviceId)
            let serviceUuid = characteristicChangeReq.serviceUuid;
            console.info("serviceUuid: "+ serviceUuid)
            let characteristicUuid = characteristicChangeReq.characteristicUuid;
            console.info("characteristicUuid: "+ characteristicUuid)
            let value = new Uint8Array(characteristicChangeReq.characteristicValue);
            console.info("value: "+ value)
        }
        console.info("BLECharacteristicChange start!");
        device.on('BLECharacteristicChange', CharacteristicChange);
        function ConnectStateChanged(code, state) {
            if (code != 0) {
                console.error('ConnectStateChanged failed!');
                return
            }
            console.info('bluetooth connect state changed');
            let connectState = state.state;
            console.info('bluetooth connect state value:' + connectState);
        }
        console.info("BLEConnectionStateChange start!");
        device.on('BLEConnectionStateChange', ConnectStateChanged);
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
    OffDevice() {
        device.off('BLECharacteristicChange');
        device.off('BLEConnectionStateChange');
    },
    onDestroy() {
        console.info('onDestroy===================');
        this.OffBle();
        device.off('BLECharacteristicChange');
        device.off('BLEConnectionStateChange');
    }
}