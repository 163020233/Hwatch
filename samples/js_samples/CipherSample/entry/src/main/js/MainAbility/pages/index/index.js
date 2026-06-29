import router from '@ohos.router';
import app from '@system.app';

export default {
    data: {
        list:["hash_256","aes_cbc","aes_ecb"],
    },
    stringToBytes(str) {
        const bytes = new Uint8Array(str.length);
        for (let i = 0; i < str.length; i++) {
            bytes[i] = str.charCodeAt(i);
        }
        return bytes;
    },
    bytesToBase64(bytes) {
        const base64Chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
        let result = '';

        for (let i = 0; i < bytes.length; i += 3) {
            const byte1 = bytes[i];
            const byte2 = i + 1 < bytes.length ? bytes[i + 1] : 0;
            const byte3 = i + 2 < bytes.length ? bytes[i + 2] : 0;

            const char1 = byte1 >> 2;
            const char2 = ((byte1 & 0x03) << 4) | (byte2 >> 4);
            const char3 = ((byte2 & 0x0F) << 2) | (byte3 >> 6);
            const char4 = byte3 & 0x3F;

            result += base64Chars[char1] + base64Chars[char2];
            result += i + 1 < bytes.length ? base64Chars[char3] : '=';
            result += i + 2 < bytes.length ? base64Chars[char4] : '=';
        }
        return result;
    },
    onShow(){},
    click(params){
        switch (params){
            case "hash_256":
                router.replace({
                    uri: "pages/hash_256/index"
                })
                break;
            case "aes_cbc":
                router.replace({
                    uri: "pages/aes_cbc/index"
                })
                break;
            case "aes_ecb":
                router.replace({
                    uri: "pages/aes_ecb/index"
                })
                break;
        }
    },
    onback(e){
        if (e.direction === "right" && e.distance > 180) {
            app.terminate();
        }
    }
}
