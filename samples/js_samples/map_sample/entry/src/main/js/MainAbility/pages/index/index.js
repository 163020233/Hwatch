import router from '@system.router';
import app from '@system.app';
export default {
    data: {
    },
    replaceMapPage() {
         router.replace({
             uri: 'pages/map/map',
         });
    },
    replaceTextFontFamilyPage() {
        router.replace({
            uri: 'pages/textfontfamily/textfontfamily',
        });
    },
    replacePickerFontFamilyPage() {
        router.replace({
            uri: 'pages/pickerfontfamily/pickerfontfamily',
        });
    },
    replaceGetDeviceInfoPage() {
        router.replace({
            uri: 'pages/getdeviceinfo/getdeviceinfo',
        });
    },
    replaceBatteryPage() {
        router.replace({
            uri: 'pages/battery/battery',
        });
    },
    replaceSensorPage() {
        router.replace({
            uri: 'pages/sensor/sensor',
        });
    },
    replaceVibratorPage() {
        router.replace({
            uri: 'pages/vibrator/vibrator',
        });
    },
    replaceBrightnessPage() {
        router.replace({
            uri: 'pages/brightness/brightness',
        });
    },
    replaceCanvasPage(){
        router.replace({
            uri: 'pages/canvas_draw/canvas_draw',
        });
    },
    replaceCalculatorPage(){
        router.replace({
            uri: 'pages/calculator/calculator',
        });
    },
    replaceNetworkPage(){
        router.replace({
            uri: 'pages/network/network',
        });
    },
    replaceCanvasDrawPage(){
        router.replace({
            uri: 'pages/canvas/canvas_draw/canvas_draw',
        });
    },
    replaceCanvasDrawImagePage(){
        router.replace({
            uri: 'pages/canvas/canvas_drawimage/canvas_drawimage',
        });
    },
    replaceFileReadRawFile(){
        router.replace({
            uri: 'pages/filereadrawfile/filereadrawfile',
        });
    },
    replaceListItemAdapter(){
        router.replace({
            uri: 'pages/listitemadapter/index',
        });
    },
    replaceArcList() {
        router.replace({
            uri: 'pages/arcListSample/arcListSample',
        });
    },
    replaceInitRouterToPage(){
        router.replace({
            uri: 'pages/initroutertopage/index',
        });
    },
    replaceScroll() {
        router.replace({
            uri: 'pages/scrollview/scrollview',
        });
    },
    replaceTimerAppGlobal() {
        router.replace({
            uri: 'pages/timerGlobal/index',
        });
    },
    replaceFileWriteSubarray() {
        router.replace({
            uri: 'pages/filewritesubarray/index',
        });
    },
    replaceImageAnimator() {
        router.replace({
            uri: 'pages/imageAnimator/imageAnimator',
        });
    },
    replaceMarqueeSample() {
        router.replace({
            uri: 'pages/marqueeSample/marqueeSample',
        });
    },
    replaceText() {
        router.replace({
            uri: 'pages/text/index',
        });
    },
    exit() {
        app.terminate();
    }
}
