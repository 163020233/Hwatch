import router from '@system.router';
import app from '@system.app';

export default {
    data: {
    },
    manualMode() {
        router.replace({
            uri: "pages/woodenfish/woodenfish",
            params: {
                autoMode: false
            }
        })
    },
    autoMode() {
        router.replace({
            uri: "pages/woodenfish/woodenfish",
            params: {
                autoMode: true
            }
        })
    },
    swipeExit(e) {
        if (e.direction == "right") {
            console.log("Woodenfish start exit");
            app.terminate();
            console.log("Woodenfish exit");
        }
    },
}
