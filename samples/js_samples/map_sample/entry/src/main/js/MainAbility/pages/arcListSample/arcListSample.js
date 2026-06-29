export default {
    data: {
        appList:[],
    },
    onInit() {
        this.appList = [
            {src:'common/leftHead.png', value: '应用1'},
            {src:'common/leftHead.png', value: '应用2'},
            {src:'common/leftHead.png', value: '应用3'},
            {src:'common/leftHead.png', value: '应用4'},
            {src:'common/leftHead.png', value: '应用5'},
            {src:'common/leftHead.png', value: '应用6'},
            {src:'common/leftHead.png', value: '应用7'},
            {src:'common/leftHead.png', value: '应用8'},
            {src:'common/leftHead.png', value: '应用9'},
            {src:'common/leftHead.png', value: '应用10'},
            {src:'common/leftHead.png', value: '应用11'},
            {src:'common/leftHead.png', value: '应用12'},
            {src:'common/leftHead.png', value: '应用13'},
            {src:'common/leftHead.png', value: '应用14'},
            {src:'common/leftHead.png', value: '应用15'},
            {src:'common/leftHead.png', value: '应用16'},
        ];
    },
    scrollEnd(endState, endComponentIndex) {
        console.log(`list onscrollend is triggered, end component index=${endComponentIndex}`);
        console.log(`list onscrollend is triggered, state=${endState}`);
    },
    clickscrollTo() {
        this.$refs.arcList.scrollTo({ index: 0 });
    },
    clickscrollToEnd() {
        this.$refs.arcList.scrollToEnd();
    },
    clickscroll() {
        this.$refs.arcList.scroll({ rotation: -10 });
    },
};
