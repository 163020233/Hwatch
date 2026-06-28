import cipher from '@system.cipher';
import router from '@ohos.router';

export default {
    data: {
        title: '',
        text:'',
        text1:'',
        textStr:'ABCDEF123456',

    },
    all_hash(){
        let that = this;
        cipher.sha({
            success:function(param){
                console.info("sha256 success : "+param);
                that.text = param;
            },
            fail:function(param){
                console.info("sha256 fail : "+param);
                that.text = param;
            },
            text:that.textStr,
        })
    },
    sub_hash(){
        let that = this;

        let mid = that.textStr.length/2;

        let test1 = that.textStr.slice(0,mid);
        let test2 = that.textStr.slice(mid);

        cipher.shaInit();

        cipher.shaUpdate(test1);
        cipher.shaUpdate(test2);

        cipher.shaFinal({
            success: function (param) {
                console.info("sha256Finish success : " + param);
                that.text = param;
            },
            fail: function (param) {
                console.info("sha256Finish fail : " + param);
                that.text = param;
            },
        });
    },
    exit(e){
        if(e.direction == "right"){
            router.replace({
                uri: "pages/index/index"
            })
        }

    }
}
