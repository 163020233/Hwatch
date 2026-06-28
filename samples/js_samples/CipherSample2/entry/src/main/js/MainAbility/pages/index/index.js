import cipher from '@system.cipher';

export default {
    data:
    {
        key: getApp().data.getKey(),
        text: "1234567890",
    },
    onInit() {
    },
    TRNG() {
        // 生成16字节随机数（十六进制）
        cipher.generateRandomBytes({
            length: 8,
            success: function(result) {
                console.info('Random bytes:' + result); // 例如："4f2c9a1e87b0d3f5..."
            },
            fail: function(error) {
                console.info('Error:' + error);
            }
        });
        // 生成1到100之间的随机数
        cipher.generateRandomNumber({
            min: -10,
            max: 100,
            success: function(result) {
                console.info('Random number:' + result); // 例如："42"
            },
            fail: function(error) {
                console.info('Error:' + error);
            }
        });
    },
    MD5() {
        cipher.md5FromString({
            text: "123456",
            success: function(result) {
                console.info("md5FromString 01 MD5 : "+ result);
            },
            fail: function(error) {
                console.info("md5FromString 01 MD5 Error: " + error);
            }
        });
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.md5FromString({
            // 长度超过4096
            text: txtStr,
            success: function(result) {
                console.info("md5FromString 02 MD5 : " + result);
            },
            fail: function(error) {
                console.info("md5FromString 02 MD5 Error: " + error);
            }
        });
        cipher.md5FromString({
            // 空字符串
            text: "",
            success: function(result) {
                console.info("md5FromString 03 MD5 : " + result);
            },
            fail: function(error) {
                console.info("md5FromString 03 MD5 Error: " + error);
            }
        });
        cipher.md5FromBytes({
            // 空字符串
            data: "",
            success: function(md5Result) {
                console.info("md5FromString 04 MD5 : " + md5Result);
            },
            fail: function(error) {
                console.info("md5FromString 04 MD5 failed: "+ error);
            }
        });
        let dataStr = '';
        for (let i = 0; i < 5; i++) {
            dataStr += this.key;
        }
        cipher.md5FromBytes({
            // 长度超过64
            data: dataStr,
            success: function(md5Result) {
                console.info("md5FromString 05 MD5: " + md5Result);
            },
            fail: function(error) {
                console.info("md5FromString 05 MD5 failed: " + error);
            }
        });
    },
    BASE64DEC() {
        cipher.base64Decode({
            text: "SGVsbG8gT3Blbkhhcm1vbnk=",
            success: function(result) {
                console.info("base64Decode 01 Decoded: " + result);
            },
            fail: function(error) {
                console.info("base64Decode 01 Error: " + error);
            }
        });
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.base64Decode({
            // 长度超过4096
            text: txtStr,
            success: function(result) {
                console.info("base64Decode 02 Decoded: " + result);
            },
            fail: function(error) {
                console.info("base64Decode 02 Error: " + error);
            }
        });
        cipher.base64Decode({
            // 空字符串
            text: "",
            success: function(result) {
                console.info("base64Decode 03 Decoded: " + result);
            },
            fail: function(error) {
                console.info("base64Decode 03 Error: " + error);
            }
        });
    },
    BASE64ENC() {
        cipher.base64Encode({
            text: "app_key=405ff10b3e50472fa1600adbc7a6ee1f&client_os_type=4&device_id=1111322312312312313213212123132312&device_id_type=UUID&nonce=1111322312312312313213212123132312&timestamp=1111322312312312313213212123132312&version=1.0",
            success: function(result) {
                console.info("base64Encode 01 Encoded: " + result);
            },
            fail: function(error) {
                console.info("base64Encode 01 Error: " + error);
            }
        });
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.base64Encode({
            // 长度超过4096
            text: txtStr,
            success: function(result) {
                console.info("base64Encode 02 Encoded: " + result);
            },
            fail: function(error) {
                console.info("base64Encode 02 Error: " + error);
            }
        });
        cipher.base64Encode({
            // 空字符串
            text: "",
            success: function(result) {
                console.info("base64Encode 03 Encoded: " + result);
            },
            fail: function(error) {
                console.info("base64Encode 03 Error: " + error);
            }
        });
    },
    HMACSHA256() {
        let that = this;
        cipher.hmacSha256({
            text: "YXBwX2tleT00MDVmZjEwYjNlNTA0NzJmYTE2MDBhZGJjN2E2ZWUxZiZjbGllbnRfb3NfdHlwZT00JmRldmljZV9pZD0xMTExMzIyMzEyMzEyMzEyMzEzMjEzMjEyMTIzMTMyMzEyJmRldmljZV9pZF90eXBlPVVVSUQmbm9uY2U9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ0aW1lc3RhbXA9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ2ZXJzaW9uPTEuMA==",
            key: that.key,
            success: function(result) {
                console.info("hmacSha256 01 : " + result);
            },
            fail: function(error) {
                console.info("hmacSha256 01 Error:  " + error);
            }
        });
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.hmacSha256({
            // 长度超过4096
            text: txtStr,
            key: that.key,
            success: function(result) {
                console.info("hmacSha256 02 : " + result);
            },
            fail: function(error) {
                console.info("HMAC-SHA256 02 Error: " + error);
            }
        });
        let keyStr = '';
        for (let i = 0; i < 5; i++) {
            keyStr += this.key;
        }
        cipher.hmacSha256({
            text: "YXBwX2tleT00MDVmZjEwYjNlNTA0NzJmYTE2MDBhZGJjN2E2ZWUxZiZjbGllbnRfb3NfdHlwZT00JmRldmljZV9pZD0xMTExMzIyMzEyMzEyMzEyMzEzMjEzMjEyMTIzMTMyMzEyJmRldmljZV9pZF90eXBlPVVVSUQmbm9uY2U9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ0aW1lc3RhbXA9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ2ZXJzaW9uPTEuMA==",
            // 长度超过64
            key: keyStr,
            success: function(result) {
                console.info("hmacSha256 03 : " + result);
            },
            fail: function(error) {
                console.info("hmacSha256 03 Error: " + error);
            }
        });
        cipher.hmacSha256({
            // 空字符串
            text: "",
            key: that.key,
            success: function(result) {
                console.info("hmacSha256 04 : " + result);
            },
            fail: function(error) {
                console.info("hmacSha256 04 Error: " + error);
            }
        });
        cipher.hmacSha256({
            text: "YXBwX2tleT00MDVmZjEwYjNlNTA0NzJmYTE2MDBhZGJjN2E2ZWUxZiZjbGllbnRfb3NfdHlwZT00JmRldmljZV9pZD0xMTExMzIyMzEyMzEyMzEyMzEzMjEzMjEyMTIzMTMyMzEyJmRldmljZV9pZF90eXBlPVVVSUQmbm9uY2U9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ0aW1lc3RhbXA9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ2ZXJzaW9uPTEuMA==",
            // 空字符串
            key: "",
            success: function(result) {
                console.info("hmacSha256 05 : " + result);
            },
            fail: function(error) {
                console.info("hmacSha256 05 Error: " + error);
            }
        });
    },
    HMACSHA256FromByte() {
        console.info("=== start ===");
        let testData = "app_key=405ff10b3e50472fa1600adbc7a6ee1f&client_os_type=4&device_id=1111322312312312313213212123132312&device_id_type=UUID&nonce=1111322312312312313213212123132312&timestamp=1111322312312312313213212123132312&version=1.0";
        let testKey = this.key;
        console.info("date: " + testData);
        console.info("key: " + testKey);
        // 步骤1: Base64编码
        cipher.base64Encode({
            text: testData,
            success: function(base64Encoded) {
                console.info("hmacSha256Bytes 01 Base64: " + base64Encoded);

                // 步骤2: HMAC-SHA256计算（返回ArrayBuffer）
                cipher.hmacSha256Bytes({
                    text: base64Encoded,
                    key: testKey,
                    success: function(hmacBytes) {
                        console.info("hmacSha256Bytes 01 HMAC-SHA256(ArrayBuffer): "+ hmacBytes);

                        // 步骤3: MD5计算（输入为ArrayBuffer）
                        cipher.md5FromBytes({
                            data: hmacBytes,
                            success: function(md5Result) {
                                console.info("hmacSha256Bytes 01 MD5: " + md5Result);
                            },
                            fail: function(error) {
                                console.info("hmacSha256Bytes 01 MD5 failed: " + error);
                            }
                        });
                    },
                    fail: function(error) {
                        console.info("v 01 HMAC-SHA256 failed: " + error);
                    }
                });
            },
            fail: function(error) {
                console.info("hmacSha256Bytes 01 Base64 failed: " + error);
            }
        });
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.hmacSha256Bytes({
            // 长度超过4096
            text: txtStr,
            key: testKey,
            success: function(hmacBytes) {
                console.info("hmacSha256Bytes 02 HMAC-SHA256(ArrayBuffer): " + hmacBytes);
            },
            fail: function(error) {
                console.info("hmacSha256Bytes 02 HMAC-SHA256 failed: " + error);
            }
        });
        let keyStr = '';
        for (let i = 0; i < 5; i++) {
            keyStr += this.key;
        }
        cipher.hmacSha256Bytes({
            text: "YXBwX2tleT00MDVmZjEwYjNlNTA0NzJmYTE2MDBhZGJjN2E2ZWUxZiZjbGllbnRfb3NfdHlwZT00JmRldmljZV9pZD0xMTExMzIyMzEyMzEyMzEyMzEzMjEzMjEyMTIzMTMyMzEyJmRldmljZV9pZF90eXBlPVVVSUQmbm9uY2U9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ0aW1lc3RhbXA9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ2ZXJzaW9uPTEuMA",
            // 长度超过64
            key: keyStr,
            success: function(hmacBytes) {
                console.info("hmacSha256Bytes 03 HMAC-SHA256(ArrayBuffer): "+ hmacBytes);
            },
            fail: function(error) {
                console.info("hmacSha256Bytes 03 HMAC-SHA256 failed: "+ error);
            }
        });
        cipher.hmacSha256Bytes({
            // 空字符串
            text: "",
            key: testKey,
            success: function(hmacBytes) {
                console.info("hmacSha256Bytes 04 HMAC-SHA256(ArrayBuffer): "+ hmacBytes);
            },
            fail: function(error) {
                console.info("hmacSha256Bytes 04 HMAC-SHA256 failed: "+ error);
            }
        });
        cipher.hmacSha256Bytes({
            text: "YXBwX2tleT00MDVmZjEwYjNlNTA0NzJmYTE2MDBhZGJjN2E2ZWUxZiZjbGllbnRfb3NfdHlwZT00JmRldmljZV9pZD0xMTExMzIyMzEyMzEyMzEyMzEzMjEzMjEyMTIzMTMyMzEyJmRldmljZV9pZF90eXBlPVVVSUQmbm9uY2U9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ0aW1lc3RhbXA9MTExMTMyMjMxMjMxMjMxMjMxMzIxMzIxMjEyMzEzMjMxMiZ2ZXJzaW9uPTEuMA",
            // 空字符串
            key: "",
            success: function(hmacBytes) {
                console.info("hmacSha256Bytes 05 HMAC-SHA256(ArrayBuffer): " + hmacBytes);
            },
            fail: function(error) {
                console.info("hmacSha256Bytes 05 HMAC-SHA256 failed: " + error);
            }
        });
    },
    testSaveAndVerify(keyId) {
        console.info(`\n=== verify test (KeyId: 0x${keyId.toString(16)}) ===`);
        const testKeyData = 'a1b2c3d4e5f678901234567890123456';

        // 1. 保存密钥
        cipher.saveKeyToNv({
            keyId: keyId,
            keyData: testKeyData,
            overwrite: false,
            success: (msg) => {
                console.info('saveKeyToNv 01 Save new key success: ' + msg);

                // 2. 读取并验证密钥
                cipher.getKeyFromNv({
                    keyId: keyId,
                    success: (retrievedData) => {
                        console.info('saveKeyToNv 01 read key data: ' + retrievedData);
                        if (retrievedData === testKeyData) {
                            console.info('saveKeyToNv 01 YES: key compare success');
                        } else {
                            console.info('saveKeyToNv 01 no: key compare failed');
                        }

                        // 3. 测试覆盖保存
                        this.testOverwrite(keyId);
                    },
                    fail: (error) => {
                        console.error('saveKeyToNv 01 read key failed: '+ error);
                    }
                });
            },
            fail: (error) => {
                console.error('saveKeyToNv 01 save key failed: '+ error);
            }
        });
    },

    testOverwrite(keyId) {
        console.info(`\n=== test key overwrite (KeyId: 0x${keyId.toString(16)}) ===`);
        const newKeyData = 'deadbeefcafe1234567890abcdef1234';

        // 1. 先尝试不覆盖保存（应该失败）
        cipher.saveKeyToNv({
            keyId: keyId,
            keyData: newKeyData,
            overwrite: false,
            success: (msg) => {
                console.info('testOverwrite 01 NO: overwrite success: '+ msg);
            },
            fail: (error) => {
                console.info('testOverwrite 01 YES: excepted: overwrite failed: '+ error);

                // 2. 使用覆盖保存
                cipher.saveKeyToNv({
                    keyId: keyId,
                    keyData: newKeyData,
                    overwrite: true,
                    success: (msg) => {
                        console.info('testOverwrite 01 YES: overwrite success: '+ msg);

                        // 3. 验证覆盖后的数据
                        cipher.getKeyFromNv({
                            keyId: keyId,
                            success: (retrievedData) => {
                                console.info('testOverwrite 01 read key data after overwrite: '+ retrievedData);
                                if (retrievedData === newKeyData) {
                                    console.info('testOverwrite 01 YES: Expect overwrite compare success');
                                } else {
                                    console.info('testOverwrite 01 NO: overwrite compare failed');
                                }

                                // 4. 清理测试密钥
                                this.cleanupTestKey(keyId);
                            },
                            fail: (error) => {
                                console.error('testOverwrite 01 read failed after overwrite: '+ error);
                            }
                        });
                    },
                    fail: (error) => {
                        console.error('testOverwrite 01 save failed after overwrite: '+ error);
                    }
                });
            }
        });
    },
    NvTest() {
        console.info("=== nv test start ===");

        // 1. 获取已使用的密钥ID
        cipher.getUsedKeyIds({
            count: 5,
            success: (hexString) => {
                console.info('getUsedKeyIds 01 used key ID: '+ hexString);
                if (hexString) {
                    const keyIds = hexString.split(',').map(hex => parseInt(hex, 16));
                    console.info('getUsedKeyIds 01 used ID after parse: '+ keyIds);
                }
            },
            fail: (error) => {
                console.error('getUsedKeyIds 01 read used key failed: '+ error);
            }
        });

        // 2. 获取可用的密钥ID并测试保存
        cipher.getAvailableKeyIds({
            count: 3,
            success: (hexString) => {
                console.info('getAvailableKeyIds 01 availiable key ID: '+ hexString);
                if (hexString) {
                    const keyIds = hexString.split(',').map(hex => parseInt(hex, 16));
                    console.info('getAvailableKeyIds 01 availiable key ID after parse: '+ keyIds);

                    // 使用第一个可用keyId进行测试
                    if (keyIds.length > 0) {
                        this.testSaveAndVerify(keyIds[0]);
                    }
                }
            },
            fail: (error) => {
                console.error('getAvailableKeyIds 01 read available key failed: '+ error);
            }
        });
        // 4. 输入异常长度秘钥
        let txtStr = '';
        for (let i = 0; i < 410; i++) {
            txtStr += this.text;
        }
        cipher.saveKeyToNv({
            keyId: 2701,
            // 长度超过4060
            keyData: txtStr,
            overwrite: false,
            success: (msg) => {
                console.info('Save new key success: '+ msg);
            },
            fail: (error) => {
                console.error('save key failed: '+ error);
            }
        });
    },

    cleanupTestKey(keyId) {
        console.info(`\n=== test delete key (KeyId: 0x${keyId.toString(16)}) ===`);
        cipher.deleteKeyFromNv({
            keyId: keyId,
            success: (msg) => {
                console.info('YES: delete key success : '+ msg);
                console.info("=== nv test end!!! all success.===");
            },
            fail: (error) => {
                console.error('NO: delete key failed: '+ error);
                console.info("=== nv test end!!! (some error)===");
            }
        });
    }
}