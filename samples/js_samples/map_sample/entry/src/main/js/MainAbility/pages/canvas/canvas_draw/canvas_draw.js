export default {
    data: {
        canvas:null,
        x:0,
        y:0,
    },
    touchstart(e){
        //eg: {"type":"UNKNOWN","timestamp":20353,"point":{"x":69,"y":212}}
        console.info("touchstart : "+JSON.stringify(e))
        this.x = e.point.x
        this.y = e.point.y
    },
    touchcancel(e){
        //eg: {"type":"UNKNOWN","timestamp":62550,"point":{"x":263,"y":409}}
        console.info("touchcancel : "+JSON.stringify(e))
    },
    onShow(){
        const canvas = this.$refs.all;
        let ctx = canvas.getContext('2d');
        ctx.lineWidth = 6;
        this.canvas = ctx;
    },
    touchmove(e){
        //eg: {"type":"UNKNOWN","timestamp":24787,"point":{"x":178,"y":215},"touches":[{"globalX":178,"globalY":215}]}
        console.info("touchmove : "+JSON.stringify(e));
        let x = e.point.x;
        let y = e.point.y;
        this.canvas.beginPath();
        this.canvas.strokeStyle = '#000000'
        this.canvas.moveTo(this.x , this.y);
        this.canvas.lineTo(x, y);
        this.canvas.stroke();
        this.x = x
        this.y = y

    },
    touchend(e){
        //eg: {"type":"UNKNOWN","timestamp":62314,"point":{"x":267,"y":422},"touches":[{"globalX":267,"globalY":422}]}
        console.info("touchend : "+JSON.stringify(e))
    },
}
