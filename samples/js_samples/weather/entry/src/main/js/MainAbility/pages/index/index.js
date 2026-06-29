import app from '@system.app';

const SCREEN_RADIUS = 227
const DRAW_RADIUS = 2
const DRAW_END_ANGEL = 6.28
const DRAW_SPACE_WIDTH = 130
const DRAW_PRE_X = 97
const DRAW_CUR_X = DRAW_PRE_X + DRAW_SPACE_WIDTH
const DRAW_NEXT_X = DRAW_CUR_X + DRAW_SPACE_WIDTH
const DRAW_FEATURE_X = DRAW_NEXT_X + DRAW_SPACE_WIDTH / 2
const DRAW_HEIGHT_Y = 215
const DRAW_LOW_Y = 235

var enableSwipeExit = true

export default {
    data: {
        index: 0,
        images: [
            {
                src: "/common/images/A100_018.png",
                width: 55,
                height: 55
            },
            {
                src: "/common/images/A100_018_2.png",
                width: 55,
                height: 55
            }
        ],
        rains: [
            {
                src: "/common/images/rain_1.png",
                width: 454,
                height: 454
            },
            {
                src: "/common/images/rain_2.png",
                width: 454,
                height: 454
            },
            {
                src: "/common/images/rain_3.png",
                width: 454,
                height: 454
            },
        ],
        isDraw: false
    },
    onShow() {
        const canvas = this.$refs.home;
        let ctx = canvas.getContext('2d');

        ctx.beginPath();
        ctx.lineWidth = 6;
        ctx.strokeStyle = '#837e8c'
        ctx.arc(SCREEN_RADIUS, SCREEN_RADIUS, 224, -0.7, 4.71)
        ctx.stroke();

        ctx.beginPath();
        ctx.strokeStyle = '#d3af1b'
        ctx.arc(SCREEN_RADIUS, SCREEN_RADIUS, 224, -1.57, -0.7)
        ctx.stroke();
    },
    drawTemperatureLine(ctx, isTop, y1, y2, y3) {
        ctx.beginPath();
        ctx.strokeStyle = isTop ? '#857f61' : '#68788c'
        ctx.moveTo(DRAW_PRE_X, y1);
        ctx.lineTo(DRAW_CUR_X, y2);
        ctx.stroke();

        ctx.beginPath();
        ctx.strokeStyle = isTop ? '#dac163' : '#a6c7d5'
        ctx.moveTo(DRAW_CUR_X, y2);
        ctx.lineTo(DRAW_NEXT_X, y3);
        ctx.lineTo(DRAW_FEATURE_X, y3);
        ctx.stroke();
    },
    drawPoint(ctx, x, y) {
        ctx.beginPath();
        ctx.arc(x, y, DRAW_RADIUS, 0, DRAW_END_ANGEL)
        ctx.stroke();
    },
    swiperChange(data) {
        console.log("Moji swiper: " + data.index)
        this.index = data.index
        if (this.index == 1 && !this.isDraw) {
            const canvas = this.$refs.next;
            let ctx = canvas.getContext('2d');

            ctx.lineWidth = 3;
            // draw top line
            this.drawTemperatureLine(ctx, true, DRAW_HEIGHT_Y, DRAW_HEIGHT_Y, DRAW_HEIGHT_Y - 5)

            // draw bottom line
            this.drawTemperatureLine(ctx, false, DRAW_LOW_Y, DRAW_LOW_Y + 4, DRAW_LOW_Y)

            ctx.lineWidth = DRAW_RADIUS * 2;
            ctx.strokeStyle = '#ffffff'
            // draw top circle point
            this.drawPoint(ctx, DRAW_PRE_X, DRAW_HEIGHT_Y)
            this.drawPoint(ctx, DRAW_CUR_X, DRAW_HEIGHT_Y)
            this.drawPoint(ctx, DRAW_NEXT_X, DRAW_HEIGHT_Y - 5)

            // draw bottom circle point
            this.drawPoint(ctx, DRAW_PRE_X, DRAW_LOW_Y)
            this.drawPoint(ctx, DRAW_CUR_X, DRAW_LOW_Y + 4)
            this.drawPoint(ctx, DRAW_NEXT_X, DRAW_LOW_Y)
            this.isDraw = true
        }
        enableSwipeExit = true
    },
    swipeEvent(e){
        if(enableSwipeExit && e.direction == "right" && this.index == 0) {
            app.terminate()
            console.log("Moji exit");
        }
        enableSwipeExit = false
    },
}
