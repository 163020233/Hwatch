import router from '@system.router';
import battery from '@system.battery';
export default {
    data: {
        charging: true,
        level: 0,
    },
    onInit() {
        this.GetBatteryStatus();
    },
    GetBatteryStatus() {
        battery.getStatus({
            success: (data) => {
                console.info("----BatteryStatus：", data.charging ? "IsCharging" : "Is Not Charging");
                console.info("----Battery level:", data.level * 100 + "%");
                this.charging = data.charging;
                this.level = data.level;
            },
            fail: (data, code) => {
                console.info("-----Failed to obtain battery status:", data, code); // 获取电池状态失败时调用的回调函数
            },
            complete: () => {
                console.info("-----Execution completed"); // 执行完成后调用的回调函数
            }
        });
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
