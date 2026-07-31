/*

MIT License

Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <sstream>

namespace JUSTC {

Value API::convertFromCore(const ::Value& v) {

    switch(v.type) {

        case DataType::BOOLEAN:
            return Value(v.boolean_value);

        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            return Value(v.number_value);

        case DataType::STRING:
        case DataType::LINK:
        case DataType::PATH:
            return Value(v.string_value);

        case DataType::JSON_ARRAY: {

            Array arr;

            for (const auto& item : v.array_elements)
                arr.push_back(
                    convertFromCore(item)
                );

            return arr;
        }

        case DataType::JSON_OBJECT:
        case DataType::JUSTC_OBJECT: {

            Object obj;

            for (const auto& [k,val] : v.properties)
                obj[k]=convertFromCore(val.value);

            return obj;
        }

        default:
            return {};
    }
}

::Value API::convertToCore(
    const Value& value
) {

    if (value.isNull())
        return ::Value::createNull();

    if (value.isBool())
        return ::Value::createBoolean(
            value.get<bool>()
        );

    if (value.isNumber())
        return ::Value::createNumber(
            value.get<double>()
        );

    if (value.isString())
        return ::Value::createString(
            value.get<std::string>()
        );

    if (value.isArray()) {

        std::vector<::Value> arr;

        for (auto& item:
            value.get<Array>())
        {
            arr.push_back(
                convertToCore(item)
            );
        }

        return ::Value::createJsonArray(arr);
    }

    if (value.isObject()) {

        std::unordered_map<
            std::string,
            ::Value
        > obj;

        for (auto& [k,v]:
            value.get<Object>())
        {
            obj[k]=convertToCore(v);
        }

        return ::Value::createJsonObject(obj);
    }

    return ::Value::createNull();
}

Object API::parse(
    const std::string& code,
    bool execute,
    bool async
) {

    auto lexed =
        Lexer::parse(code);

    auto result =
        Parser::parseTokens(
            lexed.second,
            execute,
            async,
            code
        );

    Object out;

    for (auto& [k,v] :
        result.returnValues)
    {
        out[k]=convertFromCore(v);
    }

    return out;
}

std::string escapeString(
    const std::string& str
){

    std::string out;

    for(char c : str){

        switch(c){

            case '\\':
                out+="\\\\";
                break;

            case '"':
                out+="\\\"";
                break;

            case '\n':
                out+="\\n";
                break;

            case '\t':
                out+="\\t";
                break;

            default:
                out+=c;
        }
    }

    return out;
}

std::string stringifyValue(
    const Value& v
){

    if(v.isNull())
        return "nil";

    if(v.isBool())
        return v.get<bool>()
            ?"y":"n";

    if(v.isNumber()){
        std::ostringstream ss;
        ss << v.get<double>();
        return ss.str();
    }

    if(v.isString())
        return "\"" +
            escapeString(v.get<std::string>())
            + "\"";

    if(v.isArray()){

        std::string out="[";

        bool first=true;

        for(const auto& item:
            v.get<Array>())
        {
            if(!first)
                out+=", ";

            first=false;

            out+=
                stringifyValue(
                    item
                );
        }

        out+="]";

        return out;
    }

    if(v.isObject()){

        std::string out="{";

        bool first=true;

        for(
            const auto& [k,v]:
            v.get<Object>()
        ){

            if(!first)
                out+=",";

            first=false;

            out+="\"";
            out+=escapeString(k);
            out+="\"=";

            out+=
                stringifyValue(v);
        }

        out+="}";

        return out;
    }

    return "";
}

std::string API::stringify(
    const Object& object
){

    std::string out;

    bool first=true;

    for(
        auto& [k,v]:
        object
    ){

        if(!first)
            out+=",";

        first=false;

        out+="\"";
        out+=k;
        out+="\"=";

        out+=
            stringifyValue(v);
    }

    out+=".";

    return out;
}

std::pair<
    std::string,
    std::vector<ParserToken>
>
API::lexer(
    const std::string& code
){
    return Lexer::parse(code,true);
}

ParseResult API::parser(
    const std::vector<ParserToken>& tokens,
    bool execute,
    bool async,
    const std::string& input
){
    return Parser::parseTokens(
        tokens,
        execute,
        async,
        input
    );
}

}
