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

set(COMPONENT_NAME "jerry-port-default_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/include
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-date.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-debugger.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-external-context.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-fatal.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-io.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-port/default/default-module.c
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

