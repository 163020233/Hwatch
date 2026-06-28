import router from '@system.router';

export default {
    data: {
        defaultTime: "",
        time: "",
    },
    handleChange(data) {
        this.time = this.fill(data.hour) + ":" + this.fill(data.minute);
    },
    fill(value) {
        return (value > 9 ? "" : "0") + value;
    },
    replaceHomefoPage() {
        router.replace({
            uri: 'pages/index/index',
        });
    },
}

