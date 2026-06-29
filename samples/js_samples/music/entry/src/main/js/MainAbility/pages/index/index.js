import app from '@system.app';
import router from '@system.router';

export default {
    data: {},
    onInit() {
    },
    onShow() {
        setTimeout(this.toNav, 2000)
    },
    toNav() {
        router.replace({
            uri: "pages/playlist/playlist"
        })
    },
    swipeEvent(e) {
        if (e.direction == "right") {
            app.terminate()
            console.log("Xima exit");
        }
    },
}