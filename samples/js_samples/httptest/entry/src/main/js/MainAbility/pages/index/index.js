import router from '@system.router';
import app from '@system.app';
export default {
    data: {
    },
    replaceBasePage() {
        router.replace({
            uri: 'pages/httpbase/httpbase',
        });
    },
    replaceComplexPage() {
        router.replace({
            uri: 'pages/httpcomplex/httpcomplex',
        });
    },
}
