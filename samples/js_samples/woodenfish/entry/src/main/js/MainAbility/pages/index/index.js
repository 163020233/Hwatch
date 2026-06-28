import router from '@system.router';
import app from '@system.app';

export default {
    onInit() {
    },
    onShow() {
        setTimeout(this.toNav, 500)
    },
    toNav() {
        router.replace({
            uri: "pages/dashboard/dashboard"
        })
    },
    swipeExit(e) {
        if (e.direction == "right") {
            console.log("start exit");
            app.terminate();
            console.log("exit");
        }
    },
    data: {
    }
}
