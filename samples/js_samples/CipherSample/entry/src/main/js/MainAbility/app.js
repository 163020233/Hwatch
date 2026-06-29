import cipher from '@system.cipher';

let key = "";
let iv = "";
let data = "";

function generateRandomBase16() {
    let chardata = "";
    for (let i = 43; i <= 122; i++) {
        let charCode = null;
        if (i >= 47 && i <= 57) {
            charCode = i;
        } else if (i >= 65 && i <= 90) {
            charCode = i;
        } else if (i >= 97 && i <= 122) {
            charCode = i;
        } else if (i === 43) {
            charCode = i;
        }
        if (charCode) {
            chardata += String.fromCharCode(charCode)
        }
    }
    data = chardata
}

function getKey() {
    return key
}

function getIv() {
    return iv
}

function getKeyString() {
    // 请填充正确key值
    let result = '';
    if (result.length < 1) {
        console.error("Please Enter a correct Key value")
    }
    return result;
}

function getIvString() {
    // 请填充正确iv值
    let result = '';
    if (result.length < 1) {
        console.error("Please Enter a correct Iv value")
    }
    return result;
}


export default {
    data: {
        getKey:getKey,
        getIv:getIv,
    },
    onCreate() {
        generateRandomBase16()
        // 此处key和iv仅作为参考,商用实现时请使用真实的key和iv
        let charkey = getKeyString()
        let chariv = getIvString()
        cipher.base64Encode({
            text:charkey,
            success:function(result){
                console.info("base64Encode charkey success : "+result)
                key = result
            },
            fail:function(error){
                console.info("base64Encode charkey error : "+error)
            }
        })
        cipher.base64Encode({
            text:chariv,
            success:function(result){
                console.info("base64Encode chariv success : "+result)
                iv = result
            },
            fail:function(error){
                console.info("base64Encode chariv error : "+error)
            }
        })
        console.info('Application onCreate ');
    },
    onDestroy() {
        key = null;
        iv = null;
        data = null;
        console.info('Application onDestroy');
    }
};
