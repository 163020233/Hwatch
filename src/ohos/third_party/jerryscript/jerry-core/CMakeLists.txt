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

set(COMPONENT_NAME "jerryscript")

set(PUBLIC_HEADER
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api
    ${OHOS_DIR}/third_party/jerryscript/jerry-ext/include/jerryscript-ext
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/include
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/debugger
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ext
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jcontext
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jrt
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/lit
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/regexp
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm
    ${OHOS_DIR}/third_party/jerryscript/jerry-core
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api/jerry-debugger-transport.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api/jerry-debugger.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api/jerry-snapshot.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/api/jerry.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/debugger/debugger.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-alloc.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-gc.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-collection.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-conversion.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-errol.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-external-pointers.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-number.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-string.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers-value.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-helpers.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-init-finalize.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-lcache.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-literal-storage.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-module.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base/ecma-property-hashmap.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-iterator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-prototype-unscopables.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-arraybuffer-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-arraybuffer.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-boolean-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-boolean.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-dataview-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-dataview.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-date-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-date.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-error-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-error.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-evalerror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-evalerror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-function-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-function.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator-function.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-generator.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-global.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-date.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-error.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-json.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers-sort.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-helpers.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-intrinsic.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-iterator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-json.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map-iterator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-map.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-math.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-number-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-number.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-object-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-promise-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-promise.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-proxy.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-rangeerror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-rangeerror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-referenceerror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-referenceerror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-reflect.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-regexp-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-regexp.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set-iterator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-set.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string-iterator-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-string.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-symbol-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-symbol.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-syntaxerror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-syntaxerror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-type-error-thrower.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-typeerror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-typeerror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-urierror-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-urierror.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakmap-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakmap.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakset-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtin-weakset.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/ecma-builtins.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float32array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float32array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float64array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-float64array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int16array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int16array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int32array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int32array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int8array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-int8array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray-helpers.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-typedarray.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint16array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint16array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint32array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint32array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8array-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8array.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8clampedarray-prototype.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/builtin-objects/typedarray/ecma-builtin-uint8clampedarray.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-array-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-arraybuffer-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-boolean-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-comparison.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-container-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-conversion.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-dataview-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-eval.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-exceptions.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-function-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-get-put-value.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-iterator-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-jobqueue.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-lex-env.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-number-arithmetic.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-number-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects-arguments.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects-general.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-objects.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-promise-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-proxy-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-reference.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-regexp-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-string-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-symbol-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/operations/ecma-typedarray-object.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ext/ext-utils.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ext/heapdump.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/ext/tracker.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jcontext/jcontext.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem/jmem-allocator.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem/jmem-heap.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem/jmem-poolman.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/jrt/jrt-fatals.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/lit/lit-char-helpers.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/lit/lit-magic-strings.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/lit/lit-strings.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/byte-code.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/common.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-lexer.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-expr.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-mem.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-module.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-statm.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-tagged-template-literal.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser-util.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-parser.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-scanner-ops.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-scanner-util.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/js/js-scanner.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/regexp/re-bytecode.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/regexp/re-compiler.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/parser/regexp/re-parser.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-arithmetics.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-bitwise.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/opcodes-ecma-relational-equality.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/opcodes.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/vm-stack.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/vm-utils.c
    ${OHOS_DIR}/third_party/jerryscript/jerry-core/vm/vm.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-strict-prototypes
    -Wno-error=unused-function
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    JERRY_FUNCTION_NAME
    JERRY_FUNCTION_BACKTRACE
    JERRY_STACK_LIMIT=0
    JERRY_GLOBAL_HEAP_SIZE=\(576\)
    JERRY_ERROR_MESSAGES=1
    JERRY_LINE_INFO=1
    JERRY_ES2015=1
    JERRY_ES2015_BUILTIN_TYPEDARRAY=1
    JERRY_ES2015_BUILTIN_SET=1
    JERRY_ES2015_BUILTIN_PROMISE=1
    JERRY_ES2015_BUILTIN_PROXY=1
    JERRY_ES2015_MODULE_SYSTEM=1
    JERRY_ES2015_BUILTIN_MAP=1
    JERRY_ES2015_BUILTIN_WEAKMAP=0
    JERRY_ES2015_BUILTIN_WEAKSET=0
    JERRY_ES2015_BUILTIN_DATAVIEW=0
    JERRY_ES2015_BUILTIN_REFLECT=0
    JERRY_BUILTIN_REGEXP=1
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core/ecma/base "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core/include "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core/jmem "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core/jrt "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-core/lit "*.h")
install_sdk(${OHOS_DIR}/third_party/jerryscript/jerry-ext/include/jerryscript-ext "*.h")
