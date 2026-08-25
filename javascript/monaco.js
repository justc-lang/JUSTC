const monacoScript = document.createElement('script');
monacoScript.src = 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.29.1/min/vs/loader.min.js';
const monacoElement = document.createElement('div');
monacoElement.id = "editor";
monacoElement.style = "height: 80vh; outline: 2px solid gray; overflow-y: clip; border-radius: 2px;";

const monacoJUSTClang = {
        keywords: [
            "type", "global", "local", "strict",
            "import", "export", "exports", "require",
            "run", "output", "return", "specified",
            "everything", "disabled", "as", "allow",
            "disallow", "JavaScript", "safe", "Luau",
            "class", "function", "public", "private",
            "static", "const", "define", "undefine",
            "echo", "log", "logfile", "space", "var",
            "new", "lgt", "goto", "isolated", "if",
            "for", "while", "lambda", "from", "options",
            "struct", "set", "to", "extends", "await",
            "async", "try", "catch", "finally", "enum",
            "throw", "this",

            "is", "isn't", "isif", "then", "elseif", "else",
            "isifn't", "elseifn't", "then't", "elsen't",
            "or", "orn't", "and", "andn't", "not", "nand",
            "nor", "xor", "xnor", "imply", "nimply",

            "true", "false", "yes", "no", "y", "n",
            "null", "nil",

            "constructor", "destructor",
        ],

        typeKeywords: [
            'number', 'string', 'boolean', 'null', 'link', 'path', 'binary', 'octal', 'hexadecimal', 'object', 'json', 'array', 'nan', 'infinity', 'data', 'auto',
            'num', 'str', 'bool', 'nil', 'bin', 'oct', 'hex', 'obj', 'inf',

            "int8", "int16", "int32", "int64", "int128",
            "uint8", "uint16", "uint32", "uint64",
            "uint128", "float32", "float64", "float128",
            "cuint8", "cuint16", "cuint32", "cuint64",

            "Window",
        ],

        operators: [
            '!=', '<=', '>=', '<<', '>>', '&&', '!&', '||', '!|',
            '=!', '?!', '..', '::', '??', '==', '?=', '**',
            '=', '!', '&', '|', '/', ':', '*', '-', '+', '^',
            '<', '>', '?', '%', '~', '#', '?:', '|>', '~='
        ],

        symbols: /[=><!~?:&|+\-*\/\^%]+/,

        escapes: /\\(?:[abfnrtv\\"']|x[0-9A-Fa-f]{1,4}|u[0-9A-Fa-f]{4}|U[0-9A-Fa-f]{8})/,

        builtinFunctions: /\b(echo|log|logfile|valueof|String(::(((grapheme|codePoint|byte)(Reverse|Slice|Length)|(reverse|slice|length))|trim|repeat|(start|end)sWith|size|lower|upper|normalize|equalsIgnoreCase|isWhitespace)|)|Number|Link|Binary(::((to|from)(Text|DataURL))|)|Octal|Hexadecimal|typeid|typeof|JSON(\.(parse|stringify)|)|file|size|env|config|parseInt|JUSTC(\.(parse(r|)|execute|stringify|version|lexer)|)|TIME|PI|backslash|version|HTTP\.(GET|POST|PUT|PATCH|DELETE|HEAD|OPTIONS)|math\.(a(bs|cos|sin|tan(2|))|ceil|cos|clamp|cube|double|exp|factorial|floor|hypot|isPrime|lerp|log(10|)|max|min|pow|random|round|sign|sin|sqrt|square|tan|to(Degrees|Radians))|parse(Num|Int)|Data|JavaScript(\.(execute|isAllowed)|)|Luau(\.(execute|compile|isAllowed)|)|JUSTO(\.(parse|stringify|version)|)|Array::(join|includes|indexOf|lastIndexOf|reverse|forEach|push|unshift))\b/i,

        constants: /\b(True|TRUE|False|FALSE|Yes|YES|No|NO|Y|N|Null|NULL|Nil|NIL|Infinity|NaN|undefined)\b/,

        numberBeforeShift: /[\d\w\)\]\}]\s*<<$/,

        luauEmbeddingStart: /(?:^|[^\w\d\)\]\}\s])<<$/,

        tokenizer: {
            root: [
                [/@numberBeforeShift/, { token: '@rematch', next: '@shiftOperator' }],
                [/@luauEmbeddingStart/, { token: 'keyword.luau', next: '@luauEmbedded', nextEmbedded: 'lua' }],

                [/--.*$/, 'comment'],
                [/-\{/, { token: 'comment', next: '@multiLineComment' }],

                [new RegExp(String.fromCharCode(34)), { token: 'string.quote', bracket: '@open', next: '@string' }],
                [new RegExp(String.fromCharCode(39)), { token: 'string.quote', bracket: '@open', next: '@singleQuoteString' }],

                [/<(?![<\s])/, { token: 'string.link', next: '@link' }],

                [/\{\{/, { token: 'keyword.js', next: '@jsEmbedded', nextEmbedded: 'javascript' }],

                [/0[xX][0-9a-fA-F_]+(B|b|)/, 'number.hex'],
                [/0[bB][01_]+(B|b|)/, 'number.binary'],
                [/0[oO][0-7_]+(B|b|)/, 'number.octal'],
                [/\d[\d_]*([\.,]\d[\d_]*)?([eE][+-]?\d[\d_]*)?(B|b|)/, 'number'],
                [/[\.,]\d(B|b|)/, 'number'],

                [/@builtinFunctions(?=\s*\()/, 'type.identifier'],

                [/@constants/, 'constant'],

                [/[a-zA-Z_][\w']*/, {
                    cases: {
                        '@keywords': 'keyword',
                        '@typeKeywords': 'type',
                        '@default': 'identifier'
                    }
                }],

                [/\$[a-zA-Z_][\w']*/, 'variable'],

                [/<</, { token: '@rematch', next: '@shiftOperator' }],
                [/>>/, 'operator'],
                [/[=><!~?:&|+\-*\/\^%]/, {
                    cases: {
                        '@operators': 'operator',
                        '@default': ''
                    }
                }],

                [/[()\[\]{}]/, '@brackets'],
                [/[,.:;]/, 'delimiter'],

                { include: '@whitespace' },
            ],

            shiftOperator: [
                [/<</, { token: 'operator', next: '@pop' }],
                [/./, { token: '@rematch', next: '@pop' }]
            ],

            string: [
                [/[^\\"]+/, 'string'],
                [/@escapes/, 'string.escape'],
                [/\\./, 'string.escape.invalid'],
                [new RegExp(String.fromCharCode(34)), { token: 'string.quote', bracket: '@close', next: '@pop' }]
            ],

            singleQuoteString: [
                [/[^\\']+/, 'string'],
                [/@escapes/, 'string.escape'],
                [/\\./, 'string.escape.invalid'],
                [new RegExp(String.fromCharCode(39)), { token: 'string.quote', bracket: '@close', next: '@pop' }]
            ],

            link: [
                [/[^>]+/, 'string.link'],
                [/>/, { token: 'string.link', bracket: '@close', next: '@pop' }]
            ],

            multiLineComment: [
                [/[^-{}]+/, 'comment'],
                [/-\}/, { token: 'comment', next: '@pop' }],
                [/[-{}]/, 'comment']
            ],

            jsEmbedded: [
                [/\}\}/, { token: 'keyword.js', next: '@pop', nextEmbedded: '@pop' }],
                [/./, '']
            ],

            luauEmbedded: [
                [/>>/, { token: 'keyword.luau', next: '@pop', nextEmbedded: '@pop' }],
                [/./, '']
            ],

            whitespace: [
                [/[ \t\r\n]+/, 'white'],
            ]
    }};

monacoScript.onload = function() {
    require.config({
        paths: {
            "vs": "https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.29.1/min/vs/"
        }
    });

    window.MonacoEnvironment = {
        getWorkerUrl: function(workerId, label) {
            return `data:text/javascript;charset=utf-8,${encodeURIComponent(`
                self.MonacoEnvironment = {
                    baseUrl: "https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.29.1/min/"
                };
                importScripts("https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.29.1/min/vs/base/worker/workerMain.js");
            `)}`;
        }
    };

    require(["vs/editor/editor.main"], function() {
        monaco.languages.register({ id: 'justc' });
        monaco.languages.setMonarchTokensProvider('justc', monacoJUSTClang);

        monaco.languages.setLanguageConfiguration('justc', {
            brackets: [
                ['(', ')'],
                ['[', ']'],
                ['{', '}'],
                ['"', '"'],
                ["'", "'"],
                ['<', '>']
            ],
            autoClosingPairs: [
                { open: '{{', close: '}}', notIn: ['string', 'comment'] },
                { open: '<<', close: '>>', notIn: ['string', 'comment'] },
                { open: '-{', close: '}-', notIn: ['string', 'comment'] },
                { open: '(', close: ')' },
                { open: '[', close: ']' },
                { open: '{', close: '}' },
                { open: '"', close: '"', notIn: ['string'] },
                { open: "'", close: "'", notIn: ['string'] },
                { open: '<', close: '>' },
            ],
            surroundingPairs: [
                { open: '(', close: ')' },
                { open: '[', close: ']' },
                { open: '{', close: '}' },
                { open: '"', close: '"' },
                { open: "'", close: "'" },
                { open: '<', close: '>' }
            ],
            comments: {
                lineComment: '--',
                blockComment: ['-{', '}-']
            }
        });

        const JUSTC_BUILTINS = {
            "echo": {
                text: "echo ${1:str}"
            },
            "log": {
                text: "log ${1:str}"
            },
            "logfile": {
                text: "logfile ${1:path}"
            },
            "valueof": {
                text: "valueof(${1:varname})"
            },
            "String": {
                text: "String(${1:any})"
            },
            "String::length": {
                text: "String::length(${1:str})"
            },
            "String::graphemeLength": {
                text: "String::graphemeLength(${1:str})"
            },
            "String::codePointLength": {
                text: "String::codePointLength(${1:str})"
            },
            "String::byteLength": {
                text: "String::byteLength(${1:str})"
            },
            "String::slice": {
                text: "String::slice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::graphemeSlice": {
                text: "String::graphemeSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::codePointSlice": {
                text: "String::codePointSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::byteSlice": {
                text: "String::byteSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::reverse": {
                text: "String::reverse(${1:str})"
            },
            "String::graphemeReverse": {
                text: "String::graphemeReverse(${1:str})"
            },
            "String::codePointReverse": {
                text: "String::codePointReverse(${1:str})"
            },
            "String::byteReverse": {
                text: "String::byteReverse(${1:str})"
            },
            "String::size": {
                text: "String::size(${1:str})"
            },
            "String::trim": {
                text: "String::trim(${1:str})"
            },
            "String::repeat": {
                text: "String::repeat(${1:str})"
            },
            "String::startsWith": {
                text: "String::startsWith(${1:str}, ${2:prefix})"
            },
            "String::endsWith": {
                text: "String::endsWith(${1:str}, ${2:postfix})"
            },
            "String::lower": {
                text: "String::lower(${1:str})"
            },
            "String::upper": {
                text: "String::upper(${1:str})"
            },
            "String::normalize": {
                text: "String::normalizeNFC(${1:str}, ${2:form})"
            },
            "String::equalsIgnoreCase": {
                text: "String::equalsIgnoreCase(${1:a}, ${2:b})"
            },
            "String::isWhitespace": {
                text: "String::isWhitespace(${1:str})"
            },
            "Number": {
                text: "Number(${1:any})"
            },
            "Link": {
                text: "Link(${1:str})"
            },
            "Binary": {
                text: "Binary(${1:any})"
            },
            "Binary::toText": {
                text: "Binary::toText(${1:bin})"
            },
            "Binary::fromText": {
                text: "Binary::fromText(${1:str})"
            },
            "Binary::toDataURL": {
                text: "Binary::toDataURL(${1:bin})"
            },
            "Binary::fromDataURL": {
                text: "Binary::fromDataURL(${1:str})"
            },
            "Octal": {
                text: "Octal(${1:num})"
            },
            "Hexadecimal": {
                text: "Hexadecimal(${1:num})"
            },
            "typeid": {
                text: "typeid ${1:any}"
            },
            "typeof": {
                text: "typeof ${1:any}"
            },
            "JSON.parse": {
                text: "JSON.parse(${1:str})"
            },
            "JSON.stringify": {
                text: "JSON.stringify(${1:obj})"
            },
            "file": {},
            "size": {},
            "env": {},
            "config": {},
            "parseInt": {},
        };
        monaco.languages.registerCompletionItemProvider('justc', {
            provideCompletionItems(model, position) {
                const suggestions = [];

                for (const keyword of monacoJUSTClang.keywords.filter(k => k.length > 1)) {
                    suggestions.push({
                        label: keyword,
                        kind: monaco.languages.CompletionItemKind.Keyword,
                        insertText: keyword
                    });
                }

                for (const [fn, data] of Object.entries(JUSTC_BUILTINS)) {
                    suggestions.push({
                        label: fn,
                        kind: monaco.languages.CompletionItemKind.Function,
                        insertText: data.text || fn,
                        insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet
                    });
                }

                return { suggestions };
            }
        });

        monaco.languages.registerDocumentSemanticTokensProvider('justc', {
            getLegend: function() {
                return {
                    tokenTypes: ['type'],
                    tokenModifiers: []
                };
            },
            provideDocumentSemanticTokens: function(model, lastResultId, token) {
                const lines = model.getLinesContent();
                const tokens = [];
                let line = 0;
                let char = 0;
                let offset = 0;
                const definedTypes = new Set();

                for (let i = 0; i < lines.length; i++) {
                    const lineContent = lines[i];
                    const structMatch = lineContent.match(/^\s*struct\s+([a-zA-Z_][\w']*)/);
                    if (structMatch) {
                        definedTypes.add(structMatch[1]);
                    }
                }

                for (let i = 0; i < lines.length; i++) {
                    const lineContent = lines[i];
                    
                    let match;
                    const structRegex = /^\s*struct\s+([a-zA-Z_][\w']*)/g;
                    while ((match = structRegex.exec(lineContent)) !== null) {
                        const startPos = match.index + match[0].indexOf(match[1]);
                        tokens.push({
                            startLine: i,
                            startChar: startPos,
                            length: match[1].length,
                            tokenType: 0
                        });
                    }

                    const declRegex = /\b(local|const|global|var)\s+([a-zA-Z_][\w']*)/g;
                    while ((match = declRegex.exec(lineContent)) !== null) {
                        const startPos = match.index + match[0].indexOf(match[2]);
                        tokens.push({
                            startLine: i,
                            startChar: startPos,
                            length: match[2].length,
                            tokenType: 0
                        });
                    }
                }

                for (let i = 0; i < lines.length; i++) {
                    const lineContent = lines[i];
                    for (const type of definedTypes) {
                        const regex = new RegExp(`\\b${type}\\b`, 'g');
                        let match;
                        while ((match = regex.exec(lineContent)) !== null) {
                            const beforeMatch = lineContent.substring(0, match.index);
                            if (!beforeMatch.match(/\b(struct|local|const|global|var)\s+$/)) {
                                tokens.push({
                                    startLine: i,
                                    startChar: match.index,
                                    length: type.length,
                                    tokenType: 0
                                });
                            }
                        }
                    }
                }

                tokens.sort((a, b) => {
                    if (a.startLine !== b.startLine) return a.startLine - b.startLine;
                    return a.startChar - b.startChar;
                });

                return {
                    tokens: tokens
                };
            },
            releaseDocumentSemanticTokens: function(lastResultId) {}
        }, {
            documentSelector: [{ language: 'justc' }],
            priority: 100
        });

        var editor = monaco.editor.create(document.getElementById("editor"), {
            value: `-- Type JUSTC code here...`,
            language: "justc",
            fontSize: 14,
            minimap: { enabled: true },
            scrollBeyondLastLine: false,
            automaticLayout: true,
            lineNumbers: "on",
            folding: true,
            lineDecorationsWidth: 10,
            lineNumbersMinChars: 3,
            "semanticHighlighting.enabled": true
        });

        const editorElement = document.getElementById("editor");
        const resizeObserver = new ResizeObserver(() => {
            editor.layout({
                width: editorElement.clientWidth,
                height: editorElement.clientHeight
            });
        });
        resizeObserver.observe(editorElement);

        document.documentElement.setAttribute('justc','');
        window.JUSTC_MonacoEditor = editor;
    });
};

monacoScript.onerror = function() {
    monacoElement.innerHTML = "<p style='color: red; padding: 20px;'>Failed to load code editor. Please check your internet connection.</p>";
};

document.head.appendChild(monacoScript);
document.body.appendChild(monacoElement);

document.body.appendChild(document.createElement('hr'));
