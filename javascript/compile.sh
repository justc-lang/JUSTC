#!/bin/bash

# MIT License
#
# Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set -e

OUTPUT_DIR="${1:-development}"
SAFE_DIR=$(echo "$OUTPUT_DIR" | sed 's|/|_|g') || "${{ env.DEFAULT_DIR }}"
mkdir -p "javascript/$SAFE_DIR"

emcc --version
EMCCVERSION=$(emcc --version)

echo $OUTPUT_DIR
echo $SAFE_DIR
echo "dirpath=$SAFE_DIR" >> $GITHUB_OUTPUT

wget -q https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
tar -xzf wabt-1.0.34-ubuntu.tar.gz
sudo cp wabt-1.0.34/bin/* /usr/local/bin/
rm -rf wabt-1.0.34 wabt-1.0.34-ubuntu.tar.gz

git clone --depth 1 --branch v1.3.2 https://github.com/USCiLab/cereal.git _deps/cereal-src
CEREAL_INCLUDE="-I./_deps/cereal-src/include"

SOURCE_FILES="core/entry/jsapi.cpp core/lexer.cpp core/parser.cpp core/parser/json.cpp core/serializer/json.cpp core/keywords.cpp \
core/fetch.cpp core/serializer/xml.cpp core/serializer/yaml.cpp core/utility.cpp core/import.cpp core/lang/luau.cpp core/built-in/http/http.cpp \
core/built-in/math/math.cpp core/built-in/binary/binary.cpp core/built-in/string/string.cpp core/unicode.cpp core/builtins.cpp core/serializer/justo.cpp \
core/parser/justo.cpp core/cpptypes.cpp core/justb.cpp core/compiler/justb.cpp core/loader/justb.cpp"

LUAU_FILES="luau/Ast/src/Ast.cpp luau/Ast/src/Confusables.cpp luau/Ast/src/Lexer.cpp luau/Ast/src/Location.cpp luau/Ast/src/Parser.cpp \
luau/Common/src/StringUtils.cpp luau/Ast/src/TimeTrace.cpp luau/Compiler/src/Builtins.cpp luau/Compiler/src/BuiltinFolding.cpp \
luau/Compiler/src/BytecodeBuilder.cpp luau/Compiler/src/Compiler.cpp luau/Compiler/src/ConstantFolding.cpp luau/Compiler/src/CostModel.cpp \
luau/Compiler/src/lcode.cpp luau/Compiler/src/TableShape.cpp luau/Compiler/src/ValueTracking.cpp luau/VM/src/lapi.cpp luau/VM/src/laux.cpp \
luau/VM/src/lbaselib.cpp luau/VM/src/lbitlib.cpp luau/VM/src/lbuiltins.cpp luau/VM/src/lcorolib.cpp luau/VM/src/ldblib.cpp luau/VM/src/ldebug.cpp \
luau/VM/src/ldo.cpp luau/VM/src/lfunc.cpp luau/VM/src/lgc.cpp luau/VM/src/linit.cpp luau/VM/src/lmathlib.cpp luau/VM/src/lmem.cpp luau/VM/src/lobject.cpp \
luau/VM/src/loslib.cpp luau/VM/src/lperf.cpp luau/VM/src/lstate.cpp luau/VM/src/lstring.cpp luau/VM/src/lstrlib.cpp luau/VM/src/ltable.cpp \
luau/VM/src/ltablib.cpp luau/VM/src/ltm.cpp luau/VM/src/ludata.cpp luau/VM/src/lutf8lib.cpp luau/VM/src/lvmexecute.cpp luau/VM/src/lvmload.cpp \
luau/VM/src/lvmutils.cpp luau/Ast/src/Allocator.cpp luau/Ast/src/Cst.cpp luau/Ast/src/PrettyPrinter.cpp luau/Compiler/src/Types.cpp luau/VM/src/lbuffer.cpp \
luau/VM/src/lbuflib.cpp luau/VM/src/lgcdebug.cpp luau/VM/src/lnumprint.cpp luau/VM/src/lveclib.cpp"
LUAU_INCLUDE="-I./luau/Ast/include -I./luau/Common/include -I./luau/Compiler/include -I./luau/VM/include"

COMMON_FLAGS="-s EXPORTED_FUNCTIONS=[\"_malloc\",\"_free\",\"_registerFunction\",\"_unregisterFunction\",\"_clearUserFunctions\",\"_registerGlobal\",\"_getGlobal\",\"_hasGlobal\",\"_unregisterGlobal\",\"_clearGlobals\",\"_registerPointer\",\"_getPointer\",\"_unregisterPointer\",\"_clearPointers\",\"_addVariableUpdateListener\",\"_clearVariableUpdateListeners\",\"_justoParse\",\"_lexer\",\"_parser\",\"_parse\",\"_free_string\",\"_version\",\"_load\"] \
-s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"addFunction\",\"removeFunction\",\"UTF8ToString\",\"stringToUTF8\",\"setValue\",\"getValue\",\"HEAPU8\",\"HEAP32\",\"HEAP16\",\"HEAP8\"] \
-s MODULARIZE=1 \
-s EXPORT_ALL=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s INVOKE_RUN=0 \
-std=c++17 \
-s DISABLE_EXCEPTION_CATCHING=0 \
-s EXPORT_NAME='__justc__' \
-s ASSERTIONS=0 \
-s ASYNCIFY=1 \
-s FETCH=1 \
-O3 \
-flto \
-s TOTAL_STACK=16MB \
-s TOTAL_MEMORY=64MB \
-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
-s MAXIMUM_MEMORY=512MB \
-s ALLOW_TABLE_GROWTH=1 \
--bind \
-s DEMANGLE_SUPPORT=0 \
-I./third-party \
$LUAU_INCLUDE $CEREAL_INCLUDE"

WEB_FLAGS="-s ENVIRONMENT=web,worker \
-s ASYNCIFY_IMPORTS=['fetch','emscripten_fetch','emscripten_fetch_close','use_luau']"
WEB_OUTPUT="javascript/$SAFE_DIR/justc.core.js"

NODE_FLAGS="-s ENVIRONMENT=node \
-s ASYNCIFY_IMPORTS=['fetch','emscripten_fetch','emscripten_fetch_close']"
NODE_OUTPUT="javascript_output/$SAFE_DIR/justc.node.js"

JSOUT_DIR="javascript_output/$SAFE_DIR"

web() {
    set +e
    emcc $SOURCE_FILES $LUAU_FILES \
        -o $WEB_OUTPUT \
        $COMMON_FLAGS \
        $WEB_FLAGS \
        -D __JUSTC_WEB__
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

nodejs() {
    mkdir -p $JSOUT_DIR
    set +e
    emcc $SOURCE_FILES $LUAU_FILES \
        -o $NODE_OUTPUT \
        $COMMON_FLAGS \
        $NODE_FLAGS
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

web && \
nodejs

mv javascript/$SAFE_DIR/justc.core.wasm $JSOUT_DIR/justc.wasm
sed -i 's/justc\.core\.wasm/justc.wasm/g' javascript/$SAFE_DIR/justc.core.js
mv javascript/$SAFE_DIR/justc.core.js $JSOUT_DIR/justc.core.js
mv javascript/core.js $JSOUT_DIR/justc.js
mv javascript/test.js $JSOUT_DIR/test.js

JUSTC_VERSION=$(justc --raw-version 2>/dev/null || echo "undefined")
JUSTC_NAME="Just an Ultimate Site Tool Configuration language"
JUSTC_HELP=$(justc -h || echo "undefined")

if [[ "$JUSTC_VERSION" == "undefined" ]]; then
    echo -e "::error::Invalid JUSTC version." && exit 1
elif [[ "$JUSTC_HELP" == "undefined" ]]; then
    echo -e "::error::Invalid JUSTC --help." && exit 1
fi
OUTPUT_VERSION="$JUSTC_VERSION ($SAFE_DIR)"
if [[ "$JUSTC_VERSION" == "$SAFE_DIR" ]]; then
    OUTPUT_VERSION="$JUSTC_VERSION"
fi

JSONString() {
    local str="$1"
    local -i i len=${#str}
    local c
    local result=""
    local ord hex

    for (( i=0; i<len; i++ )); do
        c="${str:i:1}"
        case "$c" in
            '"') result+='\"' ;;
            '\\') result+='\\' ;;
            $'\b') result+='\b' ;;
            $'\f') result+='\f' ;;
            $'\n') result+='\n' ;;
            $'\r') result+='\r' ;;
            $'\t') result+='\t' ;;
            *)
                ord=$(printf '%d' "'$c")
                if (( ord < 0x20 || ord == 0x7F )); then
                    printf -v hex '\\u%04x' "$ord"
                    result+="$hex"
                else
                    result+="$c"
                fi
                ;;
        esac
    done

    echo -n "$result"
}
echo "console.log(\"$(JSONString "$JUSTC_HELP")\");" > javascript/help.js

for file in $JSOUT_DIR/justc.core.js $JSOUT_DIR/justc.js $JSOUT_DIR/justc.node.js javascript/help.js; do
    printf "/*\n\n%s\n\n*/\n\n/*\n\n$JUSTC_NAME v$OUTPUT_VERSION\n\n*/\n\n" "$(cat LICENSE)" | cat - "$file" > temp.js && mv temp.js "$file"
done
for file in justc justc.node; do
    wasm2wat $JSOUT_DIR/$file.wasm > $JSOUT_DIR/$file.wat
    {
        head -n1 "$JSOUT_DIR/$file.wat"
        echo "  (@custom \"justc\" \"$JUSTC_NAME\")"
        echo "  (@custom \"justc.website\" \"https://just.js.org/justc\")"
        echo "  (@custom \"justc.license\" \"MIT License. https://just.js.org/justc/license.txt\")"
        echo "  (@custom \"justc.copyright\" \"Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>\")"
        echo "  (@custom \"justc.version\" \"$OUTPUT_VERSION\")"
        tail -n +2 "$JSOUT_DIR/$file.wat"
    } > $JSOUT_DIR/$file.tmp
    wat2wasm $JSOUT_DIR/$file.tmp --enable-annotations -o $JSOUT_DIR/$file.wasm
    rm -f $JSOUT_DIR/$file.tmp $JSOUT_DIR/$file.wat
done

npm i strc
mv javascript/compress.js $JSOUT_DIR/compress.js

mkdir -p $JSOUT_DIR/JUSTC/core
mkdir -p $JSOUT_DIR/JUSTC/javascript
srcfile=$JSOUT_DIR/JUSTC/index.json
echo "[" > $srcfile

SOURCE_FILES+=" core/entry/cli.cpp core/lexer.h core/parser.h core/parser/json.hpp core/serializer/json.hpp core/keywords.h core/fetch.h core/version.h core/json.hpp \
core/serializer/xml.hpp core/serializer/yaml.hpp core/utility.h core/import.hpp core/parser.emscripten.h core/lang/js.cpp core/lang/js.hpp core/lang/luau.hpp \
core/built-in/http/http.hpp core/utility.emscripten.h core/built-in/math/math.hpp core/built-in/binary/binary.hpp core/built-in/s.hpp core/lexer.emscripten.h \
core/entry/lib.cpp core/entry/lib.hpp LICENSE README.md core/built-in/string/string.hpp core/unicode.hpp core/unicode.emscripten.h core/builtins.h core/global.h \
core/justo.hpp core/entry/types.hpp core/entry/impl.hpp core/serializer/justo.hpp core/parser/justo.hpp core/cpptypes.h core/justb.hpp core/compiler/justb.hpp \
core/loader/justb.hpp javascript/core.js javascript/core.d.ts core/just.config.js core/cli.js"

OUTPUT_URL="https://just.js.org/justc/$SAFE_DIR"

for file in $SOURCE_FILES; do
    if [ -f "$file" ]; then
        echo "\"$OUTPUT_URL/JUSTC/$file\"," >> $srcfile
        node $JSOUT_DIR/compress.js $file || echo "::error::Failed to compress"
        mkdir -p $JSOUT_DIR/JUSTC/$(dirname $file)
        cp $file $JSOUT_DIR/JUSTC/$file
    fi
done
head -c-2 $srcfile > $srcfile.tmp && mv $srcfile.tmp $srcfile
echo "]" >> $srcfile
cp javascript/core.txt $JSOUT_DIR/JUSTC/javascript/core.js
cp javascript/index.d.txt $JSOUT_DIR/JUSTC/javascript/core.d.ts
cp javascript/cli.js.txt $JSOUT_DIR/JUSTC/javascript/cli.js
cp javascript/help.js $JSOUT_DIR/JUSTC/javascript/help.js
echo "{\"version\":3,\"file\":\"$OUTPUT_URL/justc.js\",\"sources\":$(cat $srcfile),\"mappings\":\"\"}" > $JSOUT_DIR/justc.js.map
printf "%s\n//# sourceMappingURL=$OUTPUT_URL/justc.js.map" "$(cat $JSOUT_DIR/justc.js)" > temp.js && mv temp.js "$JSOUT_DIR/justc.js"

mv javascript/test.html $JSOUT_DIR/test.html
mv javascript/test.css $JSOUT_DIR/docs.css
mv javascript/test.justc $JSOUT_DIR/test.justc
rm $JSOUT_DIR/compress.js

cp $JSOUT_DIR/justc.js $JSOUT_DIR/index.js
mv javascript/index.d.ts $JSOUT_DIR/index.d.ts
mv javascript/cli.js.txt $JSOUT_DIR/cli.js
mv javascript/help.js $JSOUT_DIR/help.js
node javascript/npm.js "$JUSTC_VERSION" && mv javascript/package.json $JSOUT_DIR/package.json
mv javascript/monaco.js $JSOUT_DIR/monaco.js

cp README.md $JSOUT_DIR/JUSTC/javascript/README.md
cp README.md $JSOUT_DIR/README.md

printf "#!/usr/bin/env node\n\n%s" "$(cat $JSOUT_DIR/cli.js)" > temp.js && mv temp.js "$JSOUT_DIR/cli.js"

for FILE in $JSOUT_DIR/*; do
    echo "::debug::$FILE"
done

ls -la $JSOUT_DIR/
file $JSOUT_DIR/justc.core.wasm
hexdump -C $JSOUT_DIR/justc.core.wasm | head -20
