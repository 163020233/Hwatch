export default {
    data: {
        images: [
            {
                src: "/common/images/0.png",
                width: 18,
                height: 27
            },
            {
                src: "/common/images/1.png",
                width: 11,
                height: 27
            },
            {
                src: "/common/images/2.png",
                width: 18,
                height: 27
            },
        ],


    },
    onHide() {
      this.$refs.animator1.stop()
      this.$refs.animator2.stop()
    },
};