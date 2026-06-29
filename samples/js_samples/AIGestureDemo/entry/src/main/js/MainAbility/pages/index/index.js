import router from '@system.router';
import app from '@system.app';
import keyinput from '@system.keyinput'

export default {
    data: {
        index: 0,
        max: 3,
    },
    onPrv() {
        if (this.index > 0) {
            this.index--
        }
    },
    onNext() {
        if (this.index < 3) {
            this.index++
        }

    },
    onChange(param) {
        this.index = param.index
    },
    onRouter() {
        router.replace({
            uri: "/pages/second/index",
        })
    },
    onExit() {
        app.terminate()
    },
    onInit() {
        let that = this;
        keyinput.setGestureListener({
            "callback": function (data) {
                console.info("Gestureback index data : " + JSON.stringify(data))
                let code = data.gesture
                if (code == 0) {
                    that.onRouter();
                } else if (code == 1) {
                    that.onExit();
                } else if (code == 2) {
                    that.onNext();
                } else if (code == 3) {
                    that.onPrv();
                }
            }
        })
    }
}
