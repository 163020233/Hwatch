import app from '@system.app';
export default {
    data: {
        title: 'World',
        // 初始化方向是 空
        orientation: '',
        times: 0,
    },
    onShow() {
        // 在onshow中初始化
        this.orientation = "up";
    },
    Changeangle() {
        if(this.times == 0) {
            this.orientation = "left";
            this.times = this.times + 1;
        } else if (this.times == 1) {
            this.orientation = "down";
            this.times = this.times + 1;
        } else if (this.times == 2) {
            this.orientation = "right";
            this.times = this.times + 1;
        } else if (this.times == 3) {
            this.orientation = "up";
            this.times = 0;
        }
    },
    swipeGrids(e){
        if (e.direction === "right" && e.distance > 180) {
            app.terminate();
        }
    }
}
