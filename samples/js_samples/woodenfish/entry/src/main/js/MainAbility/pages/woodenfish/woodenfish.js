import player from '@system.audio'
import vibrator from '@system.vibrator';
import router from '@system.router';

export default {
    data: {
        isShow: false,
        autoClickInterval: 500,
        autoMode: false,
        playStatus: -1,
        isAutoPlaying: false,
        intervalId: null,
        count: -1,
        music: "resources/base/media/woodenKnock.mp3",
        imageSrc: "/common/images/smallWoodenfish.png",
        streamType: 0x00400000,
        volume: 1
    },
    onInit() {
        this.playStatus = 2;
        this.count = 0;
        if (this.autoMode == true) {
            this.isShow = true;
            this.startAutoPlay();
        }
        player.onplay = () => {
            console.log("MusicDemo onplay start");
            this.playStatus = 0;
        };
        player.onended = () => {
            this.playStatus = 2;
            player.reset();
            console.log("Music ended");
            this.imageSrc = "/common/images/smallWoodenfish.png";
        }
    },
//用户手动点击
    manualClick() {
        if (this.autoMode == true) {
            //自动模式下，手动点击一下会暂停/继续自动点击，
            if (this.isAutoPlaying == true) {
                this.stopAutoPlay();
            } else {
                this.startAutoPlay();
            }
        } else {
            //非自动模式下
            this.knock();
        }
    },
//敲击木鱼
    knock() {
        //音频正在播放时忽略下次点击
        if (this.playStatus == 2) {
            player.reset();
            this.imageSrc = "/common/images/bigWoodenfish.png";
            this.count++;
            this.playMusic();
            this.vibrate();
        } else {
            console.log('Click ignored');
        }
    },
    playMusic() {
        console.info("play music " + this.count)
        player.srcInner = this.music;
        player.streamType = this.streamType;
        player.volume = this.volume;
        this.playStatus = 0;
        player.play();
    },
    stopPlay() {
        console.log("stopPlay");
        player.onplay = null;
        player.onended = null;
    },
    startAutoPlay() {
        this.intervalId = setInterval(this.knock, this.autoClickInterval);
        this.isAutoPlaying = true;
    },
    stopAutoPlay() {
        clearInterval(this.intervalId);
        this.isAutoPlaying = false;
    },
    vibrate() {
        vibrator.vibrate({
            mode: 'short',
            success: () => {
                console.info('----vibrate--success----');
            },
            fail: () => {
                console.info('---vibrate-fail----:');
            },
            complete: () => {
                console.info('---vibrate-complete--');
            }
        });
    },
    swipeToDashboard(e) {
        player.reset();
        this.stopPlay();
        if (e.direction == "right") {
            router.replace({
                uri: "pages/dashboard/dashboard",
            })
        }
    },
    onHide() {
        player.reset();
    }
}

