import router from '@system.router';
import app from '@system.app';

export default {
    data:
    {
    },
    onInit() {
    },
    replaceWebsockets() {
        router.replace({
            uri: 'pages/websockets/websockets',
        });
    },

    replaceHttps() {
        router.replace({
            uri: 'pages/https/https',
        });
    },

    replaceWebsocket() {
        router.replace({
            uri: 'pages/websocket/websocket',
        });
    },

    replaceHttp() {
        router.replace({
            uri: 'pages/http/http',
        });
    },
    onHide() {
    },
    onDestroy() {
    }
}
