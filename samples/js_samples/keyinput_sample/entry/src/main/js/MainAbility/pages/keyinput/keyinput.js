import key from '@system.keyinput';
import router from '@system.router';

function Rotatecallback(data) {
    console.info("Rotatecallback : " + JSON.stringify(data))
}

function onPressf(data){
    console.info("onPressf : " + JSON.stringify(data))
}

export default {
    data: {
        Test:"Test"
    },
    onShow() {
        let that = this
        key.setKeyListener({
            "callback": function(data){
                console.info("Keycallback data : "+JSON.stringify(data)+" Test : "+that.Test)
            }
        })
        key.setRotateListener({
            "callback": Rotatecallback
        })
        key.onPress = onPressf;
        key.onClick = (data) => {
            console.info(" onClickf : "+JSON.stringify(data))
        }
    },
    release(){
        key.onPress = null;
        key.onClick = null;
        key.setKeyListener();
        key.setRotateListener();
    },
    swipe(e){
        console.info(" swipe "+JSON.stringify(e))
        if (e.direction == "right") {
            this.release();
            router.replace({
                uri: 'pages/index/index',
            })
        }
    },
    onDestroy(){
        this.release();
    }
}
