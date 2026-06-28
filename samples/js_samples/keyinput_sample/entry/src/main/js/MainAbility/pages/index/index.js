import app from "@system.app"
import router from '@system.router';

export default {
    data: {
        pages: [
            {
                title: "keyinput Listener",
                path: "pages/keyinput/keyinput"
            },
            {
                title: "List scrollToEnd",
                path: "pages/list_scrollToEnd/list_scrollToEnd"
            },
            {
                title: "List scroll",
                path: "pages/list_scroll/list_scroll"
            },
            {
                title: "rotation",
                path: "pages/rotation/rotation"
            },
        ]
    },
    onShow() {

    },
    onClick(item){
        router.replace({
            uri:item,
        })
    },
    swipe(e){
        if (e.direction == "right" && e.distance > 200) {
            app.terminate()
        }
    }
}
