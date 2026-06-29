/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import file from '@system.file';

export default {
    data:{
        text:"",
    },
    onCl(){
        const buffer = new ArrayBuffer(8);
        const originalArray = new Uint8Array(buffer);
        originalArray.set([10, 20, 30, 40, 50, 60, 70, 80]);
        const subView = originalArray.subarray(2, 6); // 索引2到5，包含 [30, 40, 50, 60]
        console.info(originalArray); // 输出: Uint8Array(8) [10,20,30,40,50,60,70,80]
        console.info(subView); // 输出: Uint8Array(4) [30, 40, 50, 60]

        file.writeArrayBuffer({
            uri: 'internal://app/test.bin',
            buffer: subView,
            success: function(data) {
                console.log('write file success');
            },
            fail: function(data, code) {
                console.log('write file fail');
            }
        });
    }
}