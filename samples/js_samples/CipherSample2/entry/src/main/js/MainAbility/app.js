let key = "";
let iv = "";
let data = "";

function generateRandomBase() {
    let chardata = "";
    for (let i = 48; i <= 122; i++) {
        let charCode = null;
        if (i <= 57) {
            charCode = i;
        } else if (i >= 65 && i <= 90) {
            charCode = i;
        } else if (i >= 97 && i <= 122) {
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
    data:{
        getKey:getKey,
        getIv:getIv
    },
    onCreate() {
        generateRandomBase()
        // 此处key和iv仅作为参考,商用实现时请使用真实的key和iv
        key = getKeyString()
        iv = getIvString()
        console.info('Application onCreate');
    },
    onDestroy() {
        key = null;
        iv = null;
        data = null;
        console.info('Application onDestroy');
    }
};
