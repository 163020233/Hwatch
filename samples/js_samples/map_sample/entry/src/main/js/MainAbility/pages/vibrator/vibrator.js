import router from '@system.router';
import app from '@system.app';
import vibrator from '@system.vibrator';
import brightness from '@system.brightness';
export default {
    data: {
    },
    onInit() {
        this.SetVibrator();
    },
    SetVibrator() {
        vibrator.vibrate({
            mode: 'short', // 长震动模式
            success: () => {
                console.info('----vibrate--success----'); // 在控制台打印成功消息
            },
            fail: (data, code) => {
                console.info('---vibrate-fail----:', data); // 在控制台打印失败消息
                console.info('---vibrate-fail----:', code); // 在控制台打印错误码
            },
            complete: () => {
                console.info('---vibrate-complete--'); // 在控制台打印完成消息
            }
        });
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
