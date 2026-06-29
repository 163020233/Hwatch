import player from '@system.audio'
import router from '@system.router';

var timestamp = 0
var lastVolume = 0

export default {
    data: {
        playStatus: 2, //0:play,1:pause,2:stop
        duration: 0,
        currentTime: 0,
        curIndex: 0,
        progress: 0,
        showVolume: false,
        canExit: true,
        volume: 0,
        isListPlay: false,
        isListPlayExit: false,
        musicList: [
            {
                src: "/user/music/chun_feng_chu.mp3",
                name: "运动听单",
                singer: "今日畅听歌曲,春风吹 欢迎收听",
                pathType: 0 // 0: system path, 1: inner path
            },
            {
                src: "resources/base/media/alarm.mp3",
                name: "运动听单",
                singer: "日本最神秘歌曲,Lemon 欢迎收听",
                pathType: 1
            },
            {
                src: "/user/music/lan_jing.mp3",
                name: "运动听单",
                singer: "今日畅听歌曲,化身孤岛的鲸 欢迎收听",
                pathType: 0
            },
            {
                src: "http://openaudio.cos.tx.xmcdn.com/group64/M0B/02/30/wKgMaV1whaLAcRGQAE_0uZWMCp8270.mp3",
                name: "运动听单",
                singer: "http网络在线听歌",
                pathType: 0
            },
        ],
        curMusic: { //current music info
            curMusicName: "",
            singer: "",
        },
    },
    onInit() {
        this.curMusic.curMusicName = this.musicList[this.curIndex].name;
        this.curMusic.singer = this.musicList[this.curIndex].singer;
        //get music duration
        player.onplay = () => {
            console.log("MusicDemo onplay start");
            this.playStatus = 0;
        };
        player.onended = () => {
            console.log("MusicDemo onended start");
            this.playStatus = 2;

            if (!this.isListPlay) {
                // reset player
                player.reset();
                this.changeSong()
            }
            console.log("MusicDemo onended end");
        }
        player.ontimeupdate = () => {
            if (this.playStatus != 0) {
                return
            }
            this.duration = player.duration;
            this.currentTime = player.currentTime
            console.info("MusicDemo dur:" + this.duration + ", cur:" + this.currentTime)
            if (this.duration != 0) {
                this.progress = this.currentTime * 100.0 / this.duration;
            } else {
                this.progress = 0;
            }
        }
        player.streamType = 0x00400000
        if (this.isListPlay) {
            let srcList = []
            for (let index = 0; index < this.musicList.length; index++) {
                if (this.musicList[index].src[0] === "/") {
                    srcList.push(this.musicList[index].src)
                } else {
                    srcList.push("internal://" + this.musicList[index].src)
                }
            }
            console.info("srcList + " + srcList)
            player.srcList(srcList)
            player.loop = true
            if (this.curIndex === 1) {
                player.srcInner = this.musicList[this.curIndex].src;
            } else {
                player.src = this.musicList[this.curIndex].src;
            }
        }
    },
    playMusic() {
        console.info("MusicDemo getPlayState: " + this.playStatus)
        if (this.playStatus == 1) {
            player.volume = 1
            if (!this.isListPlay) {
                player.loop = false
            }
            player.play();
            this.playStatus = 0;
            console.log("MusicDemo playMusic:play");
            return;
        } else if (this.playStatus == 0) {
            this.playImage = "common/ic_music_play.png";
            player.pause();
            this.playStatus = 1;
            console.log("MusicDemo playMusic:pause");
            return;
        }

        this.curMusic.curMusicName = this.musicList[this.curIndex].name;
        this.curMusic.singer = this.musicList[this.curIndex].singer;
        player.streamType = 0x00400000
        if (!this.isListPlay) {
            if (this.musicList[this.curIndex].pathType == 0) {
                player.src = this.musicList[this.curIndex].src;
            } else {
                player.srcInner = this.musicList[this.curIndex].src;
            }
        }

        player.volume = 1
        if (!this.isListPlay) {
            player.loop = false
        }
        player.play();
        this.playStatus = 0;
        console.log("MusicDemo playMusic:play");
    },
    changeSong() {
        console.log("MusicDemo ChangeSong:start");
        if (this.playStatus == 0 || this.playStatus == 1) {
            this.playStatus = 2;
        }
        this.curIndex = (this.curIndex + 1) % 4;

        this.playMusic();
        console.log("MusicDemo ChangeSong:end");
    },
    stopPlay() {
        console.log("stopPlay");
        player.onplay = null;
        player.onended = null;
        player.ontimeupdate = null;
    },
    changeVolume(e) {
        console.log("MusicDemo ChangeVolume: " + e.progress)
        let temp_timestamp = new Date().getTime()
        if (temp_timestamp - timestamp > 250) {
            timestamp = temp_timestamp
            player.volume = e.progress / 100.0
        } else {
            lastVolume = e.progress / 100.0
        }
    },
    swipeSliderVolume(e) {
        player.volume = lastVolume
    },
    showVolume() {
        this.showVolume = true
        this.canExit = false
        this.volume = player.volume * 100
    },
    hideVolume() {
        this.showVolume = false
        setTimeout(this.setCanExit, 500)
    },
    clickVolumeDiv() {
    },
    setCanExit() {
        this.canExit = true
    },
    swipeQuitVolume(e) {
        if (e.direction == "right") {
            if (this.showVolume) {
                this.hideVolume()
            }
        }
    },
    swipeExit(e) {
        if (e.direction == "right" && this.canExit) {
            this.exitApp()
        }
    },
    backPlay(isExit) {
        if (isExit) {
            player.backgroundPlay = true
            player.reset();
            this.stopPlay();
        } else {
            player.backgroundPlay = false
        }
        router.replace({
            uri: "pages/playlist/playlist"
        })
    },
    exitApp() {
        console.log("MusicDemo start back");
        if (this.isListPlay) {
            this.isListPlayExit = true
            return
        }
        player.reset();
        this.stopPlay();
        router.replace({
            uri: "pages/playlist/playlist"
        })
        console.log("MusicDemo end back");
    },
}
