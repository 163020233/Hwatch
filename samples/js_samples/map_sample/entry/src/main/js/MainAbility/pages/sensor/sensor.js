import router from '@system.router';
import app from '@system.app';
import sensor from '@system.sensor';
export default {
    data: {
        steps: 0,
        pressure: 0,
        heartRate: 0,
        BodyState: false,
        BodyState2: false,
        Data: '',
        Acce_x: 0,
        Acce_y: 0,
        Acce_z: 0,
        gyro_x: 0,
        gyro_y: 0,
        gyro_z: 0,
        timeoutID: 0
    },
    onInit() {
        this.SubscribeStepCounter();
        this.SubscribeBarometer();
        this.SubscribeHeartRate();
        this.SubscribeOnBodyState();
        this.SubscribeAccelerometer();
        this.SubscribeGyroscope();
        this.GetOnBodyState();
        this.timeoutID = setTimeout(() => {
            console.info("      updata  info    ");
            this.Data = '';
            this.Data =  'steps: ' + this.steps + '\n'
                + 'pressure: ' + this.pressure + '\n'
                + 'heartRate: ' + this.heartRate + '\n'
                + 'BodyState: ' + this.BodyState + '\n'
                + 'BodyState2: ' + this.BodyState2 + '\n'
                + 'Acce_x: ' + this.Acce_x +'\n'
                + 'Acce_y: ' + this.Acce_y +'\n'
                + 'Acce_z: ' + this.Acce_z +'\n'
                + 'Gyro_x: ' + this.gyro_x +'\n'
                + 'Gyro_y: ' + this.gyro_y +'\n'
                + 'Gyro_z: ' + this.gyro_z +'\n';
        }, 300);
    },
    onHide() {
        this.UnsubscribeStepCounter();
        this.UnsubscribeBarometer();
        this.UnsubscribeHeartRate();
        this.UnsubscribeOnBodyState();
        this.UnsubscribeAccelerometer();
        this.UnsubscribeGyroscope();
        clearTimeout(this.timeoutID);
    },
    SubscribeStepCounter() {
        sensor.subscribeStepCounter({
            success: (data) => {
                console.info("----sensor.subscribeStepCounter--success--", data.steps); // 获取到模式时调用的回调函数
                this.steps = data.steps;

            },
            fail: (data, code) => {
                console.info("----sensor.subscribeStepCounter--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeStepCounter() {
        sensor.unsubscribeStepCounter();
    },
    SubscribeBarometer() {
        sensor.subscribeBarometer({
            success: (data) => {
                console.info("----sensor.subscribeBarometer--success--", data.pressure); // 获取到模式时调用的回调函数
                this.pressure = data.pressure;
            },
            fail: (data, code) => {
                console.info("----sensor.subscribeBarometer--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeBarometer() {
        sensor.unsubscribeBarometer();
    },
    SubscribeHeartRate() {
        sensor.subscribeHeartRate({
            success: (data) => {
                console.info("----sensor.subscribeHeartRate--success--", data.heartRate); // 获取到模式时调用的回调函数
                this.heartRate = data.heartRate;
            },
            fail: (data, code) => {
                console.info("----sensor.subscribeHeartRate--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeHeartRate() {
        sensor.unsubscribeHeartRate();
    },
    SubscribeOnBodyState() {
        sensor.subscribeOnBodyState({
            success: (data) => {
                console.info("----sensor.subscribeOnBodyState--success--", data.value); // 获取到模式时调用的回调函数
                this.BodyState = data.value;
            },
            fail: (data, code) => {
                console.info("----sensor.subscribeOnBodyState--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeOnBodyState() {
        sensor.unsubscribeOnBodyState();
    },
    GetOnBodyState() {
        sensor.getOnBodyState({
            success: (data) => {
                console.info("----sensor.getOnBodyState--success--", data.value); // 设置成功时调用的回调函数
                this.BodyState2 = data.value;
            },
            fail: (data, code) => {
                console.info("----sensor.getOnBodyState--fail--", data, code); // 设置失败时调用的回调函数
            },
            complete: () => {
                console.info("----sensor.getOnBodyState--complete--"); // 执行完成后调用的回调函数
            }
        });
    },

    SubscribeAccelerometer() {
        sensor.subscribeAccelerometer({
            success: (data) => {
                console.info("----sensor.subscribeAccelerometer--success--", data.x); // 获取到模式时调用的回调函数
                this.Acce_x = data.x;
                this.Acce_y = data.y;
                this.Acce_z = data.z;
            },
            fail: (data, code) => {
                console.info("----sensor.subscribeAccelerometer--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeAccelerometer() {
        sensor.unsubscribeAccelerometer();
    },

    SubscribeGyroscope() {
        sensor.subscribeGyroscope({
            success: (data) => {
                console.info("----sensor.subscribeGyroscope--success--", data.x); // 获取到模式时调用的回调函数
                this.gyro_x = data.x;
                this.gyro_y = data.y;
                this.gyro_z = data.z;
            },
            fail: (data, code) => {
                console.info("----sensor.subscribeGyroscope--fail--:", data, code); // 获取模式失败时调用的回调函数
            }
        });
    },
    UnsubscribeGyroscope() {
        sensor.unsubscribeGyroscope();
    },

    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
