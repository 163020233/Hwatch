import router from '@ohos.router';
import keyinput from '@system.keyinput'

export default {
    data: {
        title: ''
    },
    onBack() {
        router.replace({
            uri: "/pages/index/index"
        })
    },

    onInit() {
        let that = this
        keyinput.setGestureListener({
            "callback": function (data) {
                console.info("Gestureback second data : " + JSON.stringify(data))
                if (data.gesture == 1) {
                    that.onBack();
                }
            }
        })
    }
}
