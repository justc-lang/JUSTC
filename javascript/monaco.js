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
            "for", "while", "lambda",

            "is", "isn't", "isif", "then", "elseif", "else",
            "isifn't", "elseifn't", "then't", "elsen't",
            "or", "orn't", "and", "andn't", "not", "nand",
            "nor", "xor", "xnor", "imply", "nimply",

            "true", "false", "yes", "no", "y", "n",
            "null", "nil",
        ],

        typeKeywords: [
            'number', 'string', 'boolean', 'null', 'link', 'path', 'binary', 'octal', 'hexadecimal', 'object', 'json', 'array', 'nan', 'infinity', 'data', 'auto',
            'num', 'str', 'bool', 'nil', 'bin', 'oct', 'hex', 'obj', 'inf',

            "int8", "int16", "int32", "int64", "int128",
            "uint8", "uint16", "uint32", "uint64",
            "uint128", "float32", "float64", "float128",
            "cuint8", "cuint16", "cuint32", "cuint64",
        ],

        operators: [
            '!=', '<=', '>=', '<<', '>>', '&&', '!&', '||', '!|',
            '=!', '?!', '..', '::', '??', '==', '?=', '**',
            '=', '!', '&', '|', '/', ':', '*', '-', '+', '^',
            '<', '>', '?', '%', '~', '#', '?:', '|>', '~='
        ],

        symbols: /[=><!~?:&|+\-*\/\^%]+/,

        escapes: /\\(?:[abfnrtv\\"']|x[0-9A-Fa-f]{1,4}|u[0-9A-Fa-f]{4}|U[0-9A-Fa-f]{8})/,

        builtinFunctions: /\b(echo|log|logfile|valueof|String(::((Grapheme|CodePoint|Byte|)(Reverse|Slice|Length)|Trim|Repeat|(Start|End)sWith|Size|Lower|Upper|NormalizeNF(K|)(C|D)|EqualsIgnoreCase|IsWhitespace)|)|Number|Link|Binary(::((To|From)(Text|DataURL))|)|Octal|Hexadecimal|typeid|typeof|JSON(\.(Parse|Stringify)|)|file|size|env|config|parseInt|JUSTC(\.(Parse(r|)|Execute|Stringify|Version|Lexer)|)|TIME|PI|Backslash|Version|HTTP\.(GET|POST|PUT|PATCH|DELETE|HEAD|OPTIONS)|Math\.(A(bs|cos|sin|tan(2|))|Ceil|Cos|Clamp|Cube|Double|Exp|Factorial|Floor|Hypot|IsPrime|Lerp|Log(10|)|Max|Min|Pow|Random|Round|Sign|Sin|Sqrt|Square|Tan|To(Degrees|Radians))|Parse(Num|Int)|Data|JavaScript(\.(Execute|IsAllowed)|)|Luau(\.(Execute|Compile|IsAllowed)|)|JUSTO(\.(Parse|Stringify|Version)|)|Array::(Join|Includes|IndexOf|LastIndexOf|Reverse|ForEach))\b/i,

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
            "String::Length": {
                text: "String::Length(${1:str})"
            },
            "String::GraphemeLength": {
                text: "String::GraphemeLength(${1:str})"
            },
            "String::CodePointLength": {
                text: "String::CodePointLength(${1:str})"
            },
            "String::ByteLength": {
                text: "String::ByteLength(${1:str})"
            },
            "String::Slice": {
                text: "String::Slice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::GraphemeSlice": {
                text: "String::GraphemeSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::CodePointSlice": {
                text: "String::CodePointSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::ByteSlice": {
                text: "String::ByteSlice(${1:str}, ${2:start}, ${3:end})"
            },
            "String::Reverse": {
                text: "String::Reverse(${1:str})"
            },
            "String::GraphemeReverse": {
                text: "String::GraphemeReverse(${1:str})"
            },
            "String::CodePointReverse": {
                text: "String::CodePointReverse(${1:str})"
            },
            "String::ByteReverse": {
                text: "String::ByteReverse(${1:str})"
            },
            "String::Size": {
                text: "String::Size(${1:str})"
            },
            "String::Trim": {
                text: "String::Trim(${1:str})"
            },
            "String::Repeat": {
                text: "String::Repeat(${1:str})"
            },
            "String::StartsWith": {
                text: "String::StartsWith(${1:str}, ${2:prefix})"
            },
            "String::EndsWith": {
                text: "String::StartsWith(${1:str}, ${2:postfix})"
            },
            "String::Lower": {
                text: "String::Lower(${1:str})"
            },
            "String::Upper": {
                text: "String::Upper(${1:str})"
            },
            "String::NormalizeNFC": {
                text: "String::NormalizeNFC(${1:str})"
            },
            "String::NormalizeNFD": {
                text: "String::NormalizeNFD(${1:str})"
            },
            "String::NormalizeNFKC": {
                text: "String::NormalizeNFKC(${1:str})"
            },
            "String::NormalizeNFKD": {
                text: "String::NormalizeNFKD(${1:str})"
            },
            "String::EqualsIgnoreCase": {
                text: "String::EqualsIgnoreCase(${1:a}, ${2:b})"
            },
            "String::IsWhitespace": {
                text: "String::IsWhitespace(${1:str})"
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
            "Binary::ToText": {
                text: "Binary::ToText(${1:bin})"
            },
            "Binary::FromText": {
                text: "Binary::FromText(${1:str})"
            },
            "Binary::ToDataURL": {
                text: "Binary::ToText(${1:bin})"
            },
            "Binary::FromDataURL": {
                text: "Binary::ToText(${1:str})"
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
            "JSON.Parse": {
                text: "JSON.Parse(${1:str})"
            },
            "JSON.Stringify": {
                text: "JSON.Stringify(${1:obj})"
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
            lineNumbersMinChars: 3
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
