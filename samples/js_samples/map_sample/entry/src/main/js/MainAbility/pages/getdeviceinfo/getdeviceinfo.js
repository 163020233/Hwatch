import Device from '@system.device';
import router from '@system.router';
export default {
    data: {
        brand: '',
        manufacturer: '',
        model: '',
        product: '',
        language: '',
        region: '',
        windowWidth: 0,
        windowHeight: 0,
        screenDensity: 0,
        screenShape: '',
        apiVersion: 0,
        deviceType: '',
        defaultTime: "",
        time: "",
    },
    onInit() {
        this.getDeviceInfo();
    },
    getDeviceInfo() {
        Device.getInfo({
            success: (data) => {
                this.brand = data.brand;
                this.manufacturer = data.manufacturer;
                this.model = data.model;
                this.product = data.product;
                this.language = data.language;
                this.region = data.region;
                this.windowWidth = data.windowWidth;
                this.windowHeight = data.windowHeight;
                this.screenDensity = data.screenDensity;
                this.screenShape = data.screenShape;
                this.apiVersion = data.apiVersion;
                this.deviceType = data.deviceType;
            },
            fail: (data, code) => {
                console.error(`获取设备信息失败, 错误码: ${code}, 错误信息: ${data}`);
            },
            complete: () => {
                console.log('设备信息获取完成');
            },
        });
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    },
};