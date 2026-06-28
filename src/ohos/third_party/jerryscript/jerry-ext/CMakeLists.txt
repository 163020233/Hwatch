# Copyright JS Foundation and other contributors, http://js.foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(COMPONENT_NAME "jerry-ext_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/include
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/common
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/arg
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handle-scope
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/arg/arg-js-iterator-helper.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/arg/arg-transform-functions.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/arg/arg.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-common.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-rp.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-serial.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-sha1.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-tcp.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/debugger/debugger-ws.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handle-scope/handle-scope-allocator.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handle-scope/handle-scope.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handler/handler-assert.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handler/handler-gc.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handler/handler-print.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handler/handler-register.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/handler/handler-resource-name.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/module/module.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

