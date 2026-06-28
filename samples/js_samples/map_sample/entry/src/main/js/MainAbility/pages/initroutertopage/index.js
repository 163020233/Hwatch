import router from '@system.router';

export default {
    data: {
    },
    onInit(){
        console.info("onInit router page to index")
        router.replace({
            uri: 'pages/index/index',
        });
    }
}
