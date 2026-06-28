/**
 * codec.js — 字符串 / 二进制编解码工具
 *
 * 【原生 API 说明】
 *   使用 encodeURIComponent / decodeURIComponent 实现 UTF-8 编解码。
 *   若底层平台支持 TextEncoder / TextDecoder，可替换本文件获得更优性能。
 */

export default {

    /**
     * 字符串 → Uint8Array（UTF-8）
     * 【原生 API】encodeURIComponent —— 若不可用需自行实现 UTF-8 编码
     */
    strToUint8Array: function (str) {
        if (!str) {
            return new Uint8Array(0);
        }
        var encoded = encodeURIComponent(str);
        var buf = new Uint8Array(encoded.length);
        var idx = 0;
        for (var i = 0; i < encoded.length; i++) {
            if (encoded.charAt(i) === '%') {
                buf[idx++] = parseInt(encoded.substring(i + 1, i + 3), 16);
                i += 2;
            } else {
                buf[idx++] = encoded.charCodeAt(i);
            }
        }
        // subarray 返回视图（零拷贝）；若需独立副本可改 .slice(0, idx)
        return buf.subarray(0, idx);
    },

    /**
     * Uint8Array → 字符串（UTF-8）
     * 【原生 API】decodeURIComponent —— 若不可用需自行实现 UTF-8 解码
     */
    uint8ArrayToStr: function (arr) {
        if (!arr || arr.length === 0) {
            return '';
        }
        var s = '';
        for (var i = 0; i < arr.length; i++) {
            var h = arr[i].toString(16);
            s += '%' + (h.length === 1 ? '0' + h : h);
        }
        try {
            return decodeURIComponent(s);
        } catch (e) {
            return '[Decode Error]';
        }
    },

    /** Uint8Array → HEX 日志字符串 */
    toHexLog: function (arr, maxLen) {
        if (!arr) {
            return '';
        }
        var lim = Math.min(arr.length, maxLen || 50);
        var hex = '';
        for (var i = 0; i < lim; i++) {
            var h = arr[i].toString(16).toUpperCase();
            hex += (h.length === 1 ? '0' + h : h) + ' ';
        }
        return arr.length > lim ? hex + '...[Trunc]' : hex;
    }
};