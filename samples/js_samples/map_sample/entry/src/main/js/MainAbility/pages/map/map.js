import router from '@system.router';
import app from '@system.app';

export default {
    data: {
        title: 'World',
        search_input: '1235',
        data2 : 'abc',
        data3 : '',
        intervalID:-1
    },
    exit() {
      app.terminate();
    },
    onShow() {
        this.intervalID = setInterval(() => {
            console.info('search_input, ' + this.data2);
        }, 1000);
    },
    onDestroy() {
        if (this.intervalID > 0) {
            clearInterval(this.intervalID)
        }
    },
    oncl(){
        console.info("onClick " + this.data3)
    },
    onChange(data){
        console.info("onChange:" + data)
        this.data3 = data
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    },
}
