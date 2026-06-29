import router from '@system.router';
import key from "@system.keyinput"


export default {
    data: {
        data: ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50"],
        todolist: [{
                       title: '0',
                   }],
        isList: false,
        isPickerview: false,
        isSwiper: false,
        isSlider: false,
    },
    onInit() {
        for (var index = 1; index < 100; index++) {
            this.todolist.push({
                title: index
            })
        }
    },
    onShow() {
        let list = this.$refs.list;
        let pickerview = this.$refs.pickerview;
        let swiper = this.$refs.swiper;
        let slider = this.$refs.slider;
        let that = this;
        key.setRotateListener({
            "callback": function (res) {
                console.info(" rotateListener : " + JSON.stringify(res) + "  rotate : " + res.rotate)
                if (that.isList) {
                    list.scroll({
                        "rotation": res.rotate
                    })
                }
                if (that.isPickerview) {
                    pickerview.scroll({
                        "rotation": res.rotate
                    })
                }
                if (that.isSwiper) {
                    swiper.scroll({
                        "rotation": res.rotate
                    })
                }
                if (that.isSlider) {
                    slider.scroll({
                        "rotation": res.rotate
                    })
                }

            },
        })
    },
    fun_list() {
        if (this.isList) {
            this.isList = false;
        } else {
            this.isList = true;
        }
    },
    fun_pickerview() {
        if (this.isPickerview) {
            this.isPickerview = false;

        } else {
            this.isPickerview = true;
        }
    },
    fun_swiper() {
        if (this.isSwiper) {
            this.isSwiper = false;
        } else {
            this.isSwiper = true;
        }
    },
    fun_slider() {
        if (this.isSlider) {
            this.isSlider = false;
        } else {
            this.isSlider = true;
        }
    },
    onswipe(e) {
        if (e.direction == "right" && e.distance > 200) {
            router.replace({
                uri: 'pages/index/index',
            })
        }
    }
}