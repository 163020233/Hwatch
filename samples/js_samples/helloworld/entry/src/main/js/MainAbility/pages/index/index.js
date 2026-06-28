import app from '@system.app';
export default {
    data: {
        title: ''
    },
    onInit() {
        this.title = this.$t('strings.world');
    },
}
