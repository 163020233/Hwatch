import router from '@system.router';

export default {
    data: {
        title: 'World',
        // 初始化方向是 空
        orientation: '',
        times: 0,
        ctx: null
    },
    onShow() {
        const canvas = this.$refs.canvas;
        this.ctx = canvas.getContext("2d");
        this.img = new Image();
        this.img.onload = () => {
            console.info("Image load success ");
        };
        this.img.src = '/common/images/test.png';
    },
    Changeangle() {
        if (this.times == 0) {
            this.ctx.clear();
            this.ctx.setTransform(1, 0.5, 0, 1, 0, 0);
            this.ctx.drawImage(this.img, 50, 100);
            this.times++
        } else {
            this.ctx.clear();
            this.times = 0;
        }
    },
    swipeExit(e) {
        if (e.direction == "right") {
            router.replace({
                uri: 'pages/index/index',
            });
        }
    }
}
