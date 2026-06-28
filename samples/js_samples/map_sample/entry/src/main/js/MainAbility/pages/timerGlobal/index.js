import router from '@system.router';

export default {
    data: {},
    onShow(){
        console.info("onshow")
        setInterval(function (){
            console.info("js page setInterval")
        },2000)
        setTimeout(function (){
            console.info("js page setTimeout")
        },5000)
    },
    createGlobalTimer() {
        console.info("onclick createGlobalTimer")
        getApp().data.setAppGlobalTimer();
    },
    deleteGlobalTimer() {
        console.info("onclick deleteGlobalTimer")
        getApp().data.deleteAppGlobalTimer();
    },
    exit() {
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
