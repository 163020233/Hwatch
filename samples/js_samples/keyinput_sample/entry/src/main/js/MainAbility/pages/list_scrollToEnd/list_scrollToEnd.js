import router from '@system.router';

export default {
    data: {
        isShow:true,
        todolist: [
                "add list item 1",
        ],
        listid:0,
        scrollid:0
    },
    onShow() {
        this.listid = setInterval(() => {
            if (this.todolist[0].length < 1050) {
                console.info(" this.todolist[0].length < 1050 ")
                let temp = this.todolist[0] + "add list item 1";
                this.todolist[0] = temp;
                this.todolist = this.todolist;
            } else {
                console.info(" this.todolist[0].length > 1050 ")
            }
        }, 2500)
        this.scrollid = setInterval(() => {
            const list = this.$refs.list;
            list.scrollToEnd(this.code);
        }, 1000)
    },
    swipe(e) {
        if (e.direction == "right" && e.distance > 200) {
            router.replace({
                uri: 'pages/index/index',
            })
        }
    },
    onHide(){
        clearInterval(this.listid);
        clearInterval(this.scrollid);
    }
}
