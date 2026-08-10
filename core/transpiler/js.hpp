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

#ifndef JUSTC_TRANSPILER_JS_HPP
#define JUSTC_TRANSPILER_JS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "../parser.h"
#include "../lexer.h"

namespace JUSTCTranspiler {

class JavaScript {
public:
    static std::string transpile(const std::string& code, bool minify = false);
    static std::string transpile(const std::vector<ParserToken>& tokens, bool minify = false);
    
private:
    static std::string transpileToken(const ParserToken& token, bool minify);
    static std::string transpileExpression(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileStatement(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileCondition(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileFunction(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileLoop(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileVariableDeclaration(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileObject(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileArray(const std::vector<ParserToken>& tokens, size_t& pos, bool minify);
    static std::string transpileValue(const ParserToken& token, bool minify);
    static std::string transpileOperator(const std::string& op, bool minify);
    
    static std::string getJSKeyword(const std::string& justcKeyword);
    static std::string getJSOperator(const std::string& justcOp);
    
    static std::string indent(int level, bool minify);
    static std::string escapeString(const std::string& str);
};

}

#endif
