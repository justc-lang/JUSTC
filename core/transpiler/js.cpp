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

#include "js.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace JUSTCTranspiler {

std::string JavaScript::indent(int level, bool minify) {
    if (minify) return "";
    return std::string(level * 4, ' ');
}

std::string JavaScript::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string JavaScript::getJSKeyword(const std::string& justcKeyword) {
    static const std::unordered_map<std::string, std::string> keywordMap = {
        {"if", "if"},
        {"else", "else"},
        {"elseif", "else if"},
        {"while", "while"},
        {"for", "for"},
        {"function", "function"},
        {"return", "return"},
        {"echo", "console.log"},
        {"log", "console.log"},
        {"true", "true"},
        {"false", "false"},
        {"null", "null"},
        {"undefined", "undefined"},
        {"var", "let"},
        {"local", "let"},
        {"const", "const"},
        {"global", "var"},
        {"scope", ""},
        {"output", ""},
        {"allow", ""},
        {"disallow", ""},
    };
    
    auto it = keywordMap.find(justcKeyword);
    if (it != keywordMap.end()) {
        return it->second;
    }
    return justcKeyword;
}

std::string JavaScript::getJSOperator(const std::string& justcOp) {
    static const std::unordered_map<std::string, std::string> opMap = {
        {"is", "==="},
        {"isn't", "!=="},
        {"and", "&&"},
        {"or", "||"},
        {"not", "!"},
        {"nor", "!"},
        {"nand", "!"},
        {"xor", "^"},
        {"xnor", "=="},
        {"imply", "=>"},
        {"nimply", "!=>"},
        {"==", "==="},
        {"!=", "!=="},
        {"~=", "=="},
        {"andn't", "!&&"},
        {"orn't", "!||"},
        {"isif", "?"},
        {"then", "?"},
        {"then't", "!"},
        {"else", ":"},
        {"elsen't", "!"},
        {"..", "+"},
        {"**", "**"},
        {"//", "//"},
        {"--", "--"},
        {"++", "++"},
        {"=", "="},
        {"+", "+"},
        {"-", "-"},
        {"*", "*"},
        {"/", "/"},
        {"%", "%"},
        {"<", "<"},
        {">", ">"},
        {"<=", "<="},
        {">=", ">="},
        {"<<", "<<"},
        {">>", ">>"},
        {"&", "&"},
        {"|", "|"},
        {"^", "^"},
        {"~", "~"},
        {"&&", "&&"},
        {"||", "||"},
        {"?", "?"},
        {":", ":"},
        {"??", "??"},
        {"?:", "?:"},
        {"|>", ".then("},
    };
    
    auto it = opMap.find(justcOp);
    if (it != opMap.end()) {
        return it->second;
    }
    return justcOp;
}

std::string JavaScript::transpileValue(const ParserToken& token, bool minify) {
    if (token.type == "string") {
        return "\"" + escapeString(token.value) + "\"";
    } else if (token.type == "number") {
        return token.value;
    } else if (token.type == "boolean") {
        return token.value == "true" || token.value == "True" || token.value == "TRUE" || 
               token.value == "yes" || token.value == "Yes" || token.value == "YES" ||
               token.value == "y" || token.value == "Y" ? "true" : "false";
    } else if (token.type == "null") {
        return "null";
    } else if (token.type == "undefined") {
        return "undefined";
    } else if (token.type == "identifier") {
        return token.value;
    } else if (token.type == "link") {
        return "\"" + token.value + "\"";
    } else if (token.type == "hex") {
        return "0x" + token.value;
    } else if (token.type == "binary") {
        return "0b" + token.value;
    } else if (token.type == "octal") {
        return "0o" + token.value;
    } else if (token.type == "keyword") {
        return getJSKeyword(token.value);
    } else if (token.type == "JavaScript") {
        return "eval(`" + escapeString(token.value) + "`)";
    } else if (token.type == "Luau") {
        return "/* Luau: " + escapeString(token.value) + " */";
    } else if (token.type == "JUSTO") {
        return "/* JUSTO: " + escapeString(token.value) + " */";
    } else {
        return token.value;
    }
}

std::string JavaScript::transpileOperator(const std::string& op, bool minify) {
    return getJSOperator(op);
}

std::string JavaScript::transpileToken(const ParserToken& token, bool minify) {
    if (token.type == "keyword") {
        return getJSKeyword(token.value);
    } else if (token.type == "string") {
        return "\"" + escapeString(token.value) + "\"";
    } else if (token.type == "link") {
        return "\"" + token.value + "\"";
    } else {
        return token.value;
    }
}

std::string JavaScript::transpileExpression(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    
    while (pos < tokens.size()) {
        const auto& token = tokens[pos];
        
        if (token.type == ";" || token.type == "," || token.type == ".") {
            pos++;
            break;
        }
        
        if (token.type == "(") {
            pos++;
            result << "(" << transpileExpression(tokens, pos, minify) << ")";
            continue;
        }
        
        if (token.type == ")") {
            pos++;
            break;
        }
        
        if (token.type == "[" || token.type == "{") {
            if (token.type == "[") {
                result << transpileArray(tokens, pos, minify);
            } else {
                result << transpileObject(tokens, pos, minify);
            }
            continue;
        }
        
        if (token.type == "keyword" && (token.value == "echo" || token.value == "log")) {
            pos++;
            result << "console.log(";
            result << transpileExpression(tokens, pos, minify);
            result << ")";
            continue;
        }
        
        if (token.type == "minus" || token.type == "-" ||
            token.type == "+" || token.type == "*" || token.type == "/" ||
            token.type == "%" || token.type == "**" || token.type == ".." ||
            token.type == "==" || token.type == "!=" || token.type == "~=" ||
            token.type == "<" || token.type == ">" || token.type == "<=" || token.type == ">=" ||
            token.type == "&&" || token.type == "||" || token.type == "!" ||
            token.type == "&" || token.type == "|" || token.type == "^" || token.type == "~" ||
            token.type == "<<" || token.type == ">>" ||
            token.type == "?" || token.type == ":") {
            result << " " << transpileOperator(token.value, minify) << " ";
            pos++;
            continue;
        }

        if (token.type == "keyword") {
            std::string jsKeyword = getJSKeyword(token.value);
            if (jsKeyword.empty()) {
                pos++;
                continue;
            }
            result << jsKeyword;
            pos++;
            
            if (token.value == "if" || token.value == "while" || token.value == "for") {
                result << transpileCondition(tokens, pos, minify);
                continue;
            }
            
            if (token.value == "function") {
                result << " ";
                if (pos < tokens.size() && tokens[pos].type == "identifier") {
                    result << tokens[pos].value;
                    pos++;
                }
                result << transpileFunction(tokens, pos, minify);
                continue;
            }
            
            if (token.value == "return") {
                result << " ";
                result << transpileExpression(tokens, pos, minify);
                continue;
            }
            
            if (token.value == "var" || token.value == "const") {
                result << " ";
                result << transpileVariableDeclaration(tokens, pos, minify);
                continue;
            }
        }
        
        result << transpileValue(token, minify);
        pos++;
    }
    
    return result.str();
}

std::string JavaScript::transpileCondition(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    
    if (pos < tokens.size() && tokens[pos].type == "(") {
        pos++;
        result << "(" << transpileExpression(tokens, pos, minify) << ")";
    }
    
    if (pos < tokens.size() && tokens[pos].type == "{") {
        pos++;
        result << " {" << transpileStatement(tokens, pos, minify) << "}";
    }
    
    return result.str();
}

std::string JavaScript::transpileFunction(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    
    if (pos < tokens.size() && tokens[pos].type == "(") {
        pos++;
        result << "(" << transpileExpression(tokens, pos, minify) << ")";
    }
    
    if (pos < tokens.size() && tokens[pos].type == "{") {
        pos++;
        result << " {" << transpileStatement(tokens, pos, minify) << "}";
    }
    
    return result.str();
}

std::string JavaScript::transpileVariableDeclaration(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    
    if (pos < tokens.size() && tokens[pos].type == "identifier") {
        result << tokens[pos].value;
        pos++;
    }
    
    if (pos < tokens.size() && (tokens[pos].type == "=" || tokens[pos].value == "is")) {
        pos++;
        result << " = " << transpileExpression(tokens, pos, minify);
    }
    
    return result.str();
}

std::string JavaScript::transpileObject(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    result << "{";
    
    if (pos < tokens.size() && tokens[pos].type == "{") pos++;
    
    while (pos < tokens.size() && tokens[pos].type != "}") {
        result << transpileExpression(tokens, pos, minify);
        if (pos < tokens.size() && tokens[pos].type == ",") {
            result << ",";
            pos++;
        }
    }
    
    if (pos < tokens.size() && tokens[pos].type == "}") pos++;
    result << "}";
    
    return result.str();
}

std::string JavaScript::transpileArray(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    result << "[";
    
    if (pos < tokens.size() && tokens[pos].type == "[") pos++;
    
    while (pos < tokens.size() && tokens[pos].type != "]") {
        result << transpileExpression(tokens, pos, minify);
        if (pos < tokens.size() && tokens[pos].type == ",") {
            result << ",";
            pos++;
        }
    }
    
    if (pos < tokens.size() && tokens[pos].type == "]") pos++;
    result << "]";
    
    return result.str();
}

std::string JavaScript::transpileStatement(const std::vector<ParserToken>& tokens, size_t& pos, bool minify) {
    std::stringstream result;
    int indentLevel = 0;
    
    while (pos < tokens.size()) {
        const auto& token = tokens[pos];
        
        if (token.type == "}" || token.type == "]" || token.type == ")") {
            pos++;
            break;
        }
        
        if (token.type == "{") {
            pos++;
            result << " {" << transpileStatement(tokens, pos, minify);
            result << indent(indentLevel, minify) << "}";
            continue;
        }
        
        if (token.type == "keyword") {
            if (token.value == "if" || token.value == "elseif" || token.value == "while" || token.value == "for") {
                result << transpileCondition(tokens, pos, minify);
                continue;
            }
            
            if (token.value == "else") {
                pos++;
                result << " else ";
                if (pos < tokens.size() && tokens[pos].type == "keyword" && tokens[pos].value == "if") {
                    result << transpileCondition(tokens, pos, minify);
                } else if (pos < tokens.size() && tokens[pos].type == "{") {
                    pos++;
                    result << "{" << transpileStatement(tokens, pos, minify) << "}";
                }
                continue;
            }
            
            if (token.value == "function") {
                pos++;
                result << "function ";
                if (pos < tokens.size() && tokens[pos].type == "identifier") {
                    result << tokens[pos].value;
                    pos++;
                }
                result << transpileFunction(tokens, pos, minify);
                continue;
            }
            
            if (token.value == "return") {
                pos++;
                result << "return ";
                if (pos < tokens.size()) {
                    result << transpileExpression(tokens, pos, minify);
                }
                if (!minify) result << ";";
                continue;
            }
            
            if (token.value == "echo" || token.value == "log") {
                pos++;
                result << "console.log(";
                if (pos < tokens.size()) {
                    result << transpileExpression(tokens, pos, minify);
                }
                result << ")";
                if (!minify) result << ";";
                continue;
            }
            
            if (token.value == "var" || token.value == "const") {
                pos++;
                result << getJSKeyword(token.value) << " ";
                result << transpileVariableDeclaration(tokens, pos, minify);
                if (!minify) result << ";";
                continue;
            }
        }
        
        result << transpileExpression(tokens, pos, minify);
        if (!minify && pos < tokens.size() && (tokens[pos].type == ";" || tokens[pos].type == ",")) {
            result << ";";
        }
    }
    
    return result.str();
}

std::string JavaScript::transpile(const std::string& code, bool minify) {
    try {
        auto lexerResult = Lexer::parse(code);
        return transpile(lexerResult.second, minify);
    } catch (const std::exception& e) {
        return "// Error: " + std::string(e.what());
    }
}

std::string JavaScript::transpile(const std::vector<ParserToken>& tokens, bool minify) {
    std::stringstream result;
    size_t pos = 0;
    
    if (!minify) {
        result << "\"use strict\";\n\n";
    } else {
        result << "\"use strict\";";
    }
    
    while (pos < tokens.size()) {
        const auto& token = tokens[pos];

        if (token.type == "--" || token.type == "//") {
            pos++;
            continue;
        }
        
        std::string expr = transpileExpression(tokens, pos, minify);
        if (!expr.empty()) {
            if (!minify) result << "\n";
            result << expr;
            if (!minify && pos < tokens.size() && tokens[pos].type != ";" && tokens[pos].type != "}") {
                result << ";";
            }
        }
    }
    
    return result.str();
}

}
