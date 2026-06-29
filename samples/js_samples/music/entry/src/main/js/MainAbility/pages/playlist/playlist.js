import app from '@system.app';
import router from '@system.router';
import player from '@system.audio';

// index.js
export default {
    data: {
        musicList: [
            {
                index: 1,
                src: "/user/music/chun_feng_chu.mp3",
                name: "运动听单",
                singer: "今日畅听歌曲,春风吹 欢迎收听",
                check: false
            },
            {
                index: 2,
                src: "/user/music/alarm.mp3",
                name: "运动听单",
                singer: "日本最神秘歌曲,Lemon 欢迎收听",
                check: false
            },
            {
                index: 3,
                src: "/user/music/lan_jing.mp3",
                name: "运动听单",
                singer: "今日畅听歌曲,化身孤岛的鲸 欢迎收听",
                check: false
            },
            {
                index: 4,
                src: "http://openaudio.cos.tx.xmcdn.com/group64/M0B/02/30/wKgMaV1whaLAcRGQAE_0uZWMCp8270.mp3",
                name: "运动听单",
                singer: "http网络在线听歌",
                check: false
            },
        ],
    },
    onInit(){
        let that = this;
        let sings = [];
        for (let i = 0; i < that.musicList.length; i++) {
            sings[i] = that.musicList[i].src
        }
        player.getAlbumInfo({
            success: function success(params) {
                console.info("player.getAlbumInfo success : "+JSON.stringify(params))
                for (let i = 0; i < params.length; i++) {
                    let tempSrc = params[i].src;
                    let index = that.getPaths(tempSrc);
                    if(index === 3){
                        that.musicList[index].name = "网络音频资源";
                        that.musicList[index].singer = "网络歌手";
                    }else{
                        that.musicList[index].name = params[i].title;
                        that.musicList[index].singer = params[i].artist;
                    }
                }
            },
            fail:function fail(params){
                console.info("player.getAlbumInfo fail : "+JSON.stringify(params))
            }
        },sings)
    },
    getPaths(path){
        for (let i = 0; i < this.musicList.length; i++) {
            let tempSrc = this.musicList[i].src;
            if(tempSrc === path){
                return i;
            }
        }
    },
    goPlay(index) {
        router.replace({
            uri: "pages/play/play",
            params: {
                curIndex: index - 1
            }
        })
    },
    goList(e) {
        if (e.checked) {
            // 列表播放
            console.info("goList")
            let curIndex = 0
            for (let index = 0; index < this.musicList.length; index++) {
                if (this.musicList[index].check) {
                    curIndex = this.musicList[index].index - 1
                }
            }
            router.replace({
                uri: "pages/play/play",
                params: {
                    isListPlay: true,
                    curIndex: curIndex,
                }
            })
        }
    },
    changeCheckbox(index, param) {
        if (param.checked) {
            for (let x = 0; x < this.musicList.length; x++) {
                if (index - 1 == x) {
                    if (this.musicList[x].check) {
                        this.musicList[x].check = false
                    } else {
                        this.musicList[x].check = true
                    }
                } else {
                    this.musicList[x].check = false
                }
            }
        } else {
            this.musicList[index -1].check = false
        }
    },
    swipeExit(e) {
        if (e.direction == "right") {
            this.exitApp()
        }
    },
    exitApp() {
        console.log("MusicDemo start exit");
        app.terminate();
        console.log("MusicDemo end exit");
    },
}