import router from '@system.router';

export default {
    data: {
        picker_data: ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"],
        todolist: [
            {
                title: 'aaa',
            },
            {
                title: 'bbb'
            },
            {
                title: 'ccc'
            },
            {
                title: 'ddd'
            },
            {
                title: 'eee'
            },
            {
                title: 'fff'
            },
            {
                title: 'ggg'
            },
            {
                title: 'hhh'
            },
            {
                title: 'iii'
            },
            {
                title: 'jjj'
            },
            {
                title: 'kkk'
            },
        ],
    },
    fun_list() {
        console.info("fun_list")
        const list = this.$refs.list;
        list.rotation({
            "focus": true
        })
    },
    fun_pickerview() {
        console.info("fun_pickerview")
        const pickerview = this.$refs.pickerview;
        pickerview.rotation({
            "focus": true
        })
    },
    fun_swiper() {
        console.info("fun_pickerview")
        const swiper = this.$refs.swiper;
        swiper.rotation({
            "focus": true
        })
    },
    fun_slider() {
        console.info("fun_slider")
        const slider = this.$refs.slider;
        slider.rotation({
            "focus": true
        })
    },
    swipe(e) {
        if (e.direction == "right" && e.distance > 200) {
            router.replace({
                uri: 'pages/index/index',
            })
        }
    }
}

