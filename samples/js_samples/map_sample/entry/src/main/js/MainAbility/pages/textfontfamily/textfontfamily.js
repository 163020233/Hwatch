import router from '@system.router';
export default {
    data: {
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    },
}