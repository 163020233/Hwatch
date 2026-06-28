import cipher from '@system.cipher';
import router from '@ohos.router';

export default {
    data: {
        title: '',
        text:'',
        text1:'',
        key:getApp().data.getKey(),
        iv:getApp().data.getIv(),
        text16: "16-length text!!",
        text14: "14-length text",
        text18: "18-length text....",
    },
    click(params) {
        let that = this;
        switch (params) {
            case "nopadding_encrypt":
                that.aes(
                    that.text16, // 文本长度为16倍数
                    "encrypt",
                    "AES/CBC/NoPadding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "nopadding_decrypt":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/NoPadding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_encrypt_14":
                that.aes(
                    that.text14,
                    "encrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_decrypt_14":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_encrypt_16":
                that.aes(
                    that.text16,
                    "encrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_decrypt_16":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_encrypt_18":
                that.aes(
                    that.text18,
                    "encrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS5padding_decrypt_18":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS5Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;

            case "PKCS7padding_encrypt_14":
                that.aes(
                    that.text14,
                    "encrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS7padding_decrypt_14":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS7padding_encrypt_16":
                that.aes(
                    that.text16,
                    "encrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS7padding_decrypt_16":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS7padding_encrypt_18":
                that.aes(
                    that.text18,
                    "encrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
            case "PKCS7padding_decrypt_18":
                that.aes(
                    that.text1,
                    "decrypt",
                    "AES/CBC/PKCS7Padding",
                    that.key,
                    that.iv,
                    0,
                    16
                );
                break;
        }
    },
    aes(text,action,transformation,key,iv,ivOffset,ivLen){
        let that = this;
        cipher.aes({
            success:function(param){
                console.info(" "+transformation+" "+action+"  success : "+param);
                that.text = param;
                that.text1 = param;
            },
            fail:function(param){
                console.info(" "+transformation+" "+action+"  fail : "+param);
                that.text = param;
                that.text1 = param;
            },
            text:text,
            action:action,
            transformation:transformation,
            key:key,
            iv:iv,
            ivOffset:ivOffset,
            ivLen:ivLen,
        })
    },
    exit(e){
        if(e.direction == "right"){
            router.replace({
                uri: "pages/index/index"
            })
        }
    }
}
