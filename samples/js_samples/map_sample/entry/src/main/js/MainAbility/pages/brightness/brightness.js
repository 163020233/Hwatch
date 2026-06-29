import router from '@system.router';
import brightness from '@system.brightness';
export default {
    data: {
        mode: 0,
        value: 0,
    },
    onInit() {
        this.SetMode();
        this.GetMode();
        this.SetValue();
        this.GetValue();
    },
    SetMode() {
        brightness.setMode({
            mode: 1, // 设置为自动调节模式
            success: () => {
                console.info("----brightness.setMode--success--"); // 设置成功时调用的回调函数
            },
            fail: (data, code) => {
                console.info("----brightness.setMode--fail--", data, code); // 设置失败时调用的回调函数
            },
            complete: () => {
                console.info("----brightness.setMode--complete--"); // 执行完成后调用的回调函数
            }
        });
    },
    GetMode() {
        brightness.getMode({
            success: (data) => {
                console.info("----brightness.getMode--success--", data.mode); // 获取到模式时调用的回调函数
                this.mode = data.mode;
            },
            fail: (data, code) => {
                console.info("----brightness.getMode--fail--:", data, code); // 获取模式失败时调用的回调函数
            },
            complete: () => {
                console.info("----brightness.getMode--complete--"); // 执行完成后调用的回调函数
            }
        });
    },
    SetValue() {
        brightness.setValue({
            value: 50, // 设置为自动调节模式
            success: () => {
                console.info("----brightness.setValue--success--"); // 设置成功时调用的回调函数
            },
            fail: (data, code) => {
                console.info("----brightness.setValue--fail--", data, code); // 设置失败时调用的回调函数
            },
            complete: () => {
                console.info("----brightness.setValue--complete--"); // 执行完成后调用的回调函数
            }
        });
    },
    GetValue() {
        brightness.getValue({
            success: (data) => {
                console.info("----brightness.getValue--success--", data.value); // 获取到模式时调用的回调函数
                this.value = data.value;
            },
            fail: (data, code) => {
                console.info("----brightness.getValue--fail--:", data, code); // 获取模式失败时调用的回调函数
            },
            complete: () => {
                console.info("----brightness.getValue--complete--"); // 执行完成后调用的回调函数
            }
        });
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
