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

(()=>{
    "use strict";

    const JUSTC = {};
    const JUSTO = {};
    const JUSTB = {};
    JUSTC.Checks = {
        funcRegex: /^function\s*\w*\s*\(\)\s*\{\s*\[native code\]\s*\}$/,
        objRegex: /^\[object\s*\w*\]$/,
        sysFunc: (...functions) => {
            for (const func of functions) {
                if (typeof func != 'function' || !JUSTC.Checks.funcRegex.test(func+'')) throw new JUSTC.Error(JUSTC.Errors.environment);
            }
        },
        sysObj: (...objects) => {
            for (const obj of objects) {
                if (typeof obj != 'object' || !JUSTC.Checks.objRegex.test(obj+'')) throw new JUSTC.Error(JUSTC.Errors.environment);
            }
        }
    };

    const isAMD     =   typeof define === 'function' && define.amd;
    const isModule  = typeof module === 'object' && module.exports;
    const isBrowser =                          !isAMD && !isModule;

    if (!isBrowser && typeof require === 'function') {
        try {
            JUSTC.NodeWASM = require('./justc.node.js')
        } catch (_) {}
    }

    const globalThis_ = isBrowser ? []["filter"]["constructor"]("return globalThis")() || []["filter"]["constructor"]("return this")() || globalThis : globalThis || self || (isAMD ? {__justc__} : this) || {__justc__};
    if (!isBrowser) globalThis_.window = {}; const ___________NULL___________ = null;

    const OBJECT    = isBrowser ? globalThis_.Object        : Object;
    const json_     = isBrowser ? globalThis_.JSON          : JSON;
    const ARRAY     = isBrowser ? globalThis_.Array         : Array;
    const DOCUMENT  = isBrowser ? globalThis_.document      : ___________NULL___________;
    const __URL__   = isBrowser ? globalThis_.URL           : URL;
    const STRING    = isBrowser ? globalThis_.String        : String;
    const ERR       = isBrowser ? globalThis_.Error         : Error;
    const CONSOLE   = isBrowser ? globalThis_.console       : console;
    const MAP       = isBrowser ? globalThis_.Map           : Map;
    const BLOB      = isBrowser ? globalThis_.Blob          : ___________NULL___________;
    const FETCH     = isBrowser ? globalThis_.fetch         : fetch;
    const SCRIPT    = isBrowser ? DOCUMENT.currentScript    : ___________NULL___________;
    const INT       = isBrowser ? globalThis_.parseInt      : parseInt;
    const INTL      = isBrowser ? globalThis_.Intl          : Intl;
    const MATH      = isBrowser ? globalThis_.Math          : Math;
    const PROMISE   = isBrowser ? globalThis_.Promise       : Promise;
    const ISNAN     = isBrowser ? globalThis_.isNaN         : isNaN;
    const ISFINITE  = isBrowser ? globalThis_.isFinite      : isFinite;
    const BIGINT    = isBrowser ? globalThis_.BigInt        : BigInt;
    const TXTENCDR  = isBrowser ? globalThis_.TextEncoder   : TextEncoder;
    const UI8A      = isBrowser ? globalThis_.Uint8Array    : Uint8Array;
    const AB        = isBrowser ? globalThis_.ArrayBuffer   : ArrayBuffer;
    const NUMBER    = isBrowser ? globalThis_.Number        : Number;

    const isSafari = isBrowser ? /^((?!chrome|android).)*safari/i.test(globalThis_.navigator.userAgent) : false;
    const isNode   = isModule ? typeof process !== 'undefined' && process.versions && process.versions.node : false;
    if (isBrowser && !SCRIPT) throw new JUSTC.Error(JUSTC.Errors.environment);

    const isDev = isBrowser && globalThis_.window.location.hostname == 'just.js.org' && DOCUMENT.documentElement.getAttribute('JUSTC') == 'dev';

    JUSTC.VERSION = null;
    JUSTC['+VERSION'] = null;
    JUSTC.GetVersion = function() {
        return JUSTC.VERSION === null ? false : (JUSTC.VERSION + (
            JUSTC['+VERSION'] === null || JUSTC['+VERSION'] === JUSTC.VERSION ? '' : (' ' + JUSTC['+VERSION'])
        ));
    };
    JUSTC.SetVersion = function(version) {
        if (!JUSTC.GetVersion()) JUSTC.VERSION = version;
    };
    JUSTC.Errors = {
        initWasm: 'JUSTC WebAssembly module hasn\'t been initialized yet.',
        wrongInputType: 'Argument 0 should be a string.',
        wasmFailed: 'Failed to initialize JUSTC WebAssembly module:',
        wasmInitFailed: 'Unable to initialize JUSTC WebAssembly module.',
        executionError: 'JUSTC/core/js.cpp error:',
        arrayInput: 'Array cannot be converted to JUSTC.',
        objectInput: 'Provided input is not valid object.',
        arg0: 'Invalid argument 0. Run "JUSTC = \'help\'" for help.',
        lexerError: 'JUSTC/core/lexer.cpp error: ',
        parseError: 'JUSTC/core/parser.cpp error: ',
        jsonInput: 'Argument 0 should be an object.',
        lexerInput: 'Provided input is not valid core.lexer output.',
        boolInput: 'Argument 1 should be a boolean.',
        environment: 'Invalid or compromised environment.',
        outputMode: 'Invalid output format.',
        version: (v) => `Cannot parse lexer tokens as the current JUSTC version is ${JUSTC.VERSION}, and the tokens appear to have been generated by the JUSTC lexer version ${v}.`,
        redefine: 'JUSTC cannot be redefined.',
        fetchWasm: 'Failed to fetch JUSTC WebAssembly module:',
        redefineO: 'JUSTO cannot be redefined.',
        redefineB: 'JUSTB cannot be redefined.',
        boolInput0: 'Argument 0 should be a boolean.',
        Luau: 'To run Luau, use the standard JUSTC build. The current build excludes Luau.',
        numInput: 'Argument 1 should be a number.',
        arrInput: 'Argument 1 should be an array with two items: [boolean, string].',
    };

    if (isBrowser) {
        JUSTC.Checks.sysFunc(OBJECT, ARRAY, __URL__, STRING, ERR, MAP, BLOB, FETCH, INT, PROMISE, ISNAN, ISFINITE, BIGINT, TXTENCDR, UI8A, AB);
        JUSTC.Checks.sysObj(json_, CONSOLE, globalThis_, DOCUMENT, INTL, MATH);
        if (!isSafari) JUSTC.Checks.sysFunc(
            OBJECT.entries, OBJECT.defineProperty, OBJECT.freeze,
            json_.parse, json_.stringify,
            ARRAY.isArray, ARRAY.from,
            __URL__.parse,
            CONSOLE.log, CONSOLE.info, CONSOLE.error, CONSOLE.warn, CONSOLE.group, CONSOLE.groupEnd,
            INTL.Segmenter, Math.min, Math.max
        );
        JUSTC.Checks.sysFunc(DOCUMENT.createElement);
        JUSTC.Checks.sysObj(
            DOCUMENT.head,
            globalThis_.window
        );
        JUSTC.Checks.sysFunc(
            DOCUMENT.head.appendChild, DOCUMENT.head.removeChild
        );
    }

    JUSTC.JUSTC = undefined;
    JUSTC.Error = class extends ERR {};
    JUSTC.ErrorEnabled = true;
    JUSTC.CoreLogsEnabled = false;
    JUSTC.Silent = false;
    JUSTC.Experiments = false;
    JUSTC.Initialized = false;
    JUSTC.HasLuau = false;

    if (!isBrowser && !JUSTC.JUSTC && !JUSTC.WASM && JUSTC.NodeWASM) {JUSTC.JUSTC = JUSTC.NodeWASM}
    else if (isBrowser && globalThis_.__justc__) throw new JUSTC.Error(JUSTC.Errors.environment);

    JUSTC.Console = function(type, ...args) {
        if (!JUSTC.Silent) {
            CONSOLE[type](...args);
        }
    };
    JUSTC.ErrorIfEnabled = function(...args) {
        if (JUSTC.ErrorEnabled) {
            throw new JUSTC.Error(...args);
        }
    };

    JUSTC.Core = {};
    JUSTC.CoreScript = function(code, name) {
        try {
            const resultptr = JUSTC.WASM.ccall(
                name,
                'number',
                ['string'],
                [code]
            );
            const resultjson = JUSTC.WASM.UTF8ToString(resultptr);
            JUSTC.WASM.ccall(
                'free_string',
                null,
                ['number'],
                [resultptr]
            );
            try {
                return json_.parse(resultjson);
            } catch (_) {
                return JUSTO.parse(resultjson);
            }
        } catch (error) {
            throw new JUSTC.Error(JUSTC.Errors[name + 'Error'], error);
        }
    };
    JUSTC.Core.Lexer = function Lexer(code) {
        if (!JUSTC.WASM) throw new JUSTC.Error(JUSTC.Errors.initWasm);
        if (!code || typeof code != 'string' || code.length < 1) throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
        const result = JUSTC.CoreScript(code, 'lexer');
        if (result.error) {
            throw new JUSTC.Error((result.lexer ? JUSTC.Errors.lexerError : (JUSTC.Errors.executionError + " ")) + result.error);
        } else {
            return result || {};
        }
    };
    JUSTC.Core.Parser = function Parser(code) {
        if (!JUSTC.WASM) throw new JUSTC.Error(JUSTC.Errors.initWasm);
        if (!code || typeof code != 'object') throw new JUSTC.Error(JUSTC.Errors.lexerInput);
        const result = JUSTC.CoreScript(code, 'parser');
        if (result.error) {
            throw new JUSTC.Error((result.parser ? JUSTC.Errors.parseError : (JUSTC.Errors.executionError + " ")) + result.error);
        } else {
            return result.return || {};
        }
    };

    JUSTC.UTF16Base64 = {
        encode: function(str) {
            const codeUnits = [];
            for (let i = 0; i < str.length; i++) {
                codeUnits.push(str.charCodeAt(i));
            }
            
            const bytes = new UI8A(codeUnits.length * 2);
            for (let i = 0; i < codeUnits.length; i++) {
                const code = codeUnits[i];
                bytes[i * 2] = code & 0xFF;
                bytes[i * 2 + 1] = (code >> 8) & 0xFF;
            }
            
            return this._bytesToBase64(bytes);
        },
        
        decode: function(base64) {
            const bytes = this._base64ToBytes(base64);
            
            if (bytes.length % 2 !== 0) {
                throw new JUSTC.Error('Invalid UTF-16 data: odd number of bytes');
            }
            
            const codeUnits = [];
            for (let i = 0; i < bytes.length; i += 2) {
                const low = bytes[i];
                const high = bytes[i + 1];
                const code = (high << 8) | low;
                codeUnits.push(code);
            }
            
            return String.fromCharCode(...codeUnits);
        },

        _chars: 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/',
        
        _bytesToBase64: function(bytes) {
            const chars = this._chars;
            let result = '';
            const len = bytes.length;
            
            for (let i = 0; i < len; i += 3) {
                const a = bytes[i];
                const b = i + 1 < len ? bytes[i + 1] : 0;
                const c = i + 2 < len ? bytes[i + 2] : 0;
                
                const n = (a << 16) | (b << 8) | c;
                
                result += chars[(n >> 18) & 0x3F];
                result += chars[(n >> 12) & 0x3F];
                result += i + 1 < len ? chars[(n >> 6) & 0x3F] : '=';
                result += i + 2 < len ? chars[n & 0x3F] : '=';
            }
            
            return result;
        },
        
        _base64ToBytes: function(base64) {
            const decodeTable = new UI8A(256);
            const chars = this._chars;
            
            for (let i = 0; i < 256; i++) {
                decodeTable[i] = 0x40;
            }
            for (let i = 0; i < chars.length; i++) {
                decodeTable[chars.charCodeAt(i)] = i;
            }
            
            let clean = '';
            for (let i = 0; i < base64.length; i++) {
                const c = base64[i];
                if (c !== ' ' && c !== '\n' && c !== '\r' && c !== '\t') {
                    clean += c;
                }
            }
            
            if (clean.length === 0) {
                return new UI8A(0);
            }
            
            if (clean.length % 4 !== 0) {
                throw new JUSTC.Error('Invalid Base64 length');
            }
            
            for (let i = 0; i < clean.length; i++) {
                const c = clean[i];
                if (c !== '=' && decodeTable[c.charCodeAt(0)] === 0x40) {
                    throw new JUSTC.Error('Invalid Base64 character: ' + c);
                }
            }
            
            const result = [];
            let padding = 0;
            
            for (let i = 0; i < clean.length; i += 4) {
                const a = decodeTable[clean.charCodeAt(i)];
                const b = decodeTable[clean.charCodeAt(i + 1)];
                const c = clean[i + 2] === '=' ? 0x40 : decodeTable[clean.charCodeAt(i + 2)];
                const d = clean[i + 3] === '=' ? 0x40 : decodeTable[clean.charCodeAt(i + 3)];
                
                if (clean[i + 2] === '=') padding++;
                if (clean[i + 3] === '=') padding++;
                
                const n = (a << 18) | (b << 12) | (c << 6) | d;
                
                result.push((n >> 16) & 0xFF);
                if (padding < 2) {
                    result.push((n >> 8) & 0xFF);
                }
                if (padding < 1) {
                    result.push(n & 0xFF);
                }
            }
            
            return new UI8A(result);
        },

        random: function() {
            return this._chars[MATH.round(MATH.random() * 64)]
        },
        byte: function() {
            return this.random() + this.random() + this.random() + this.random()
        }
    };
    JUSTC.Base64toUTF16 = function(b64) {
        try {
            return [true, JUSTC.UTF16Base64.decode(b64)]
        } catch (_) {
            return [false, JUSTC.UTF16Base64.decode(b64) + JUSTC.UTF16Base64.byte()]
        }
    };
    JUSTC.UTF16toBase64 = function(data) {
        if (typeof data != 'object' || !ARRAY.isArray(data) || data.length != 2) throw new JUSTC.Error(JUSTC.Errors.arrInput);
        if (typeof data[0] != 'boolean') throw new JUSTC.Error(JUSTC.Errors.arrInput);
        if (typeof data[1] != 'string') throw new JUSTC.Error(JUSTC.Errors.arrInput);
        return JUSTC.UTF16Base64.encode(data[1]).slice(0, data[0] ? -4 : undefined);
    };

    JUSTC.PrivateFunctions = {
        All: {
            Lexer: {
                NeedsWASM: true,
                Name: "core.lexer",
                Return: JUSTC.Core.Lexer
            },
            Parser: {
                NeedsWASM: true,
                Name: "core.parser",
                Return: function Parser(JavaScriptObjectNotation) {
                    if (typeof JavaScriptObjectNotation != 'object') throw new JUSTC.Error(JUSTC.Errors.jsonInput);
                    if (
                        typeof JavaScriptObjectNotation.input != 'string' ||
                        typeof JavaScriptObjectNotation.tokens != 'object' || !ARRAY.isArray(JavaScriptObjectNotation.tokens) ||
                        typeof JavaScriptObjectNotation.version != 'string'
                    ) throw new JUSTC.Error(JUSTC.Errors.lexerInput);
                    if (!JUSTC.GetVersion()) JUSTC.Public.version;
                    if (JavaScriptObjectNotation.version != JUSTC.VERSION && (()=>{
                        try {
                            const currVerInt = INT(STRING(JUSTC.VERSION).replaceAll('.',''), 10);
                            const inptVerInt = INT(STRING(JavaScriptObjectNotation.version).replaceAll('.',''), 10);
                            return currVerInt < inptVerInt;
                        } catch (_) {
                            return true
                        }
                    })()) throw new JUSTC.Error(JUSTC.Errors.version(JavaScriptObjectNotation.version));
                    for (const token of JavaScriptObjectNotation.tokens) {
                        if (typeof token != 'object' || token.type === undefined || token.start === undefined || typeof token.start != 'number' || token.value === undefined) throw new JUSTC.Error(JUSTC.Errors.lexerInput);
                    }
                    return JUSTC.Core.Parser(JavaScriptObjectNotation);
                }
            },
            CoreErrors: {
                NeedsWASM: false,
                Name: "errorsEnabled",
                Return: ()=>{return(JUSTC.ErrorEnabled)}
            },
            CoreLogs: {
                NeedsWASM: false,
                Name: "logsEnabled",
                Return: ()=>{return(JUSTC.CoreLogsEnabled)}
            },
            Silent: {
                NeedsWASM: false,
                Name: "isSilent",
                Return: ()=>{return(JUSTC.Silent)}
            },
            env: {
                NeedsWASM: false,
                Name: "detectedEnvironment",
                Return: ()=>{return(
                    isAMD ? 'AMD' : isModule ? 'Module/CommonJS' : isBrowser ? 'Browser' : 'unknown' + JUSTC.NodeWASM != undefined ? " (Node.js)" : ''
                )}
            },
            CLI: {
                NeedsWASM: false,
                Name: "core.cli",
                Return: ()=>{
                    require('./cli.js');
                    return {
                        readFile: function(filename) {
                            const fs = require('fs');
                            return fs.readFileSync(filename, 'utf8');
                        },
                        writeFile: function(filename, content) {
                            const fs = require('fs');
                            fs.writeFileSync(filename, content, 'utf8');
                        }
                    }
                }
            },
            HasLuau: {
                NeedsWASM: true,
                Name: "hasLuau",
                Return: ()=>{return JUSTC.HasLuau}
            },
            Luau: {
                NeedsWASM: true,
                Name: "internal.luau",
                Return: function Luau(code) {
                    if (!JUSTC.HasLuau) throw new JUSTC.Error(JUSTC.Errors.Luau);
                    if (!code || typeof code != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_luau_eval',
                        'number',
                        ['string'],
                        [code]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    const type = raw.slice(0,1);
                    const str = raw.slice(1);
                    switch (type) {
                        case '1':
                            return NUMBER(str);
                        case '2':
                            return str == 'true';
                        case '3':
                            return null;
                        case '4':
                        case '5':
                            return JUSTC.TryCatchLog(()=>json_.parse(str), str);
                        case '6':
                            throw new JUSTC.Error(str);
                        default:
                            return str;
                    }
                }
            },
            LuauComp: {
                NeedsWASM: true,
                Name: "internal.luau.compile",
                Return: function Luau(code) {
                    if (!JUSTC.HasLuau) throw new JUSTC.Error(JUSTC.Errors.Luau);
                    if (!code || typeof code != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    return JUSTC.WASM.ccall(
                        'internal_luau_comp',
                        'number',
                        ['string'],
                        [code]
                    ) === 1;
                }
            },
            ZlibComp: {
                NeedsWASM: true,
                Name: "internal.compress.zlib",
                Return: function ZLIB(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_zlib_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            ZlibDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.zlib",
                Return: function ZLIB(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_zlib_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            GzipComp: {
                NeedsWASM: true,
                Name: "internal.compress.gzip",
                Return: function GZIP(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_gzip_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            GzipDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.gzip",
                Return: function GZIP(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_gzip_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            Bzip2Comp: {
                NeedsWASM: true,
                Name: "internal.compress.bzip2",
                Return: function BZIP2(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_bzip2_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            Bzip2Dcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.bzip2",
                Return: function BZIP2(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_bzip2_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            LzmaComp: {
                NeedsWASM: true,
                Name: "internal.compress.lzma",
                Return: function LZMA(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_lzma_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            LzmaDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.lzma",
                Return: function LZMA(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_lzma_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            ZstdComp: {
                NeedsWASM: true,
                Name: "internal.compress.zstd",
                Return: function ZSTD(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_zstd_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            ZstdDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.zstd",
                Return: function ZSTD(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_zstd_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            Lz4Comp: {
                NeedsWASM: true,
                Name: "internal.compress.lz4",
                Return: function LZ4(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_lz4_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            Lz4Dcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.lz4",
                Return: function LZ4(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_lz4_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            SnappyComp: {
                NeedsWASM: true,
                Name: "internal.compress.snappy",
                Return: function SNAPPY(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_snappy_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            SnappyDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.snappy",
                Return: function SNAPPY(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_snappy_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
            DeflateComp: {
                NeedsWASM: true,
                Name: "internal.compress.deflate",
                Return: function DEFLATE(data, level = 6) {
                    if (!data || typeof data != 'string') throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
                    if (!level || typeof level != 'number') throw new JUSTC.Error(JUSTC.Errors.numInput);
                    const ptr = JUSTC.WASM.ccall(
                        'internal_deflate_comp',
                        'number',
                        ['string', 'number'],
                        [JUSTC.UTF16Base64.encode(data), NUMBER(level)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.Base64toUTF16(raw);
                }
            },
            DeflateDcmp: {
                NeedsWASM: true,
                Name: "internal.decompress.deflate",
                Return: function DEFLATE(data) {
                    const ptr = JUSTC.WASM.ccall(
                        'internal_deflate_dcmp',
                        'number',
                        ['string'],
                        [JUSTC.UTF16toBase64(data)]
                    );
                    const raw = JUSTC.WASM.UTF8ToString(ptr);
                    JUSTC.WASM.ccall('free_string', null, ['number'], [ptr]);
                    return JUSTC.UTF16Base64.decode(raw);
                }
            },
        },
        Available: [
            'errorsEnabled',
            'logsEnabled',
            'isSilent',
            'detectedEnvironment',
        ],
        WhatToName: {
            "core.lexer": "Lexer",
            "core.parser": "Parser",
            "errorsEnabled": "CoreErrors",
            "logsEnabled": "CoreLogs",
            "isSilent": "Silent",
            "detectedEnvironment": "env",
            "core.cli": "CLI",
            "hasLuau": "HasLuau",
            "internal.luau": "Luau",
            "internal.luau.compile": "LuauComp",
            "internal.compress.zlib": "ZlibComp",
            "internal.decompress.zlib": "ZlibDcmp",
            "internal.compress.gzip": "GzipComp",
            "internal.decompress.gzip": "GzipDcmp",
            "internal.compress.bzip2": "Bzip2Comp",
            "internal.decompress.bzip2": "Bzip2Dcmp",
            "internal.compress.lzma": "LzmaComp",
            "internal.decompress.lzma": "LzmaDcmp",
            "internal.compress.zstd": "ZstdComp",
            "internal.decompress.zstd": "ZstdDcmp",
            "internal.compress.lz4": "Lz4Comp",
            "internal.decompress.lz4": "Lz4Dcmp",
            "internal.compress.snappy": "SnappyComp",
            "internal.decompress.snappy": "SnappyDcmp",
            "internal.compress.deflate": "DeflateComp",
            "internal.decompress.deflate": "DeflateDcmp",
        },
    };

    JUSTC.ScriptsAdded = [];
    JUSTC.UNPKG = 'https://unpkg.com/justc/';
    JUSTC.Config = {
        locateFile(path) {
            if (path.endsWith('.wasm')) {
                return JUSTC.UNPKG + path;
            }
            return path;
        }
    };
    JUSTC.Initialize = async function(excludeLuau) {
        if (isBrowser) {
            try {
                const urlprefix = isDev ? SCRIPT.src.slice(0,-8) : JUSTC.UNPKG;
                JUSTC.FetchSource = async (path, _error) => {
                    if (JUSTC.CoreLogsEnabled) JUSTC.Console("log", "Fetching", path + "...");
                    return await(await FETCH(urlprefix + path).catch((error)=>{
                        JUSTC.Console("error", _error, typeof error === 'string' ? error : 'Failed to fetch.');
                    })).text();
                };
                const corewasmjs = await JUSTC.FetchSource(excludeLuau ? 'justc_noluau.core.js' : 'justc.core.js', JUSTC.Errors.fetchWasm);
                for (const src of [corewasmjs].filter(src => !JUSTC.ScriptsAdded.includes(src))) {
                    const script = DOCUMENT.createElement('script');
                    script.textContent = src;
                    script.type = 'text/javascript';
                    DOCUMENT.head.appendChild(script);
                    if (JUSTC.CoreLogsEnabled) JUSTC.Console("log", "Created script element in document.head.");
                    JUSTC.ScriptsAdded.push(src);
                }
                JUSTC.JUSTC = globalThis_.__justc__;
            } catch (error) {
                JUSTC.Console("error", JUSTC.Errors.fetchWasm, error);
            }
        }
        try {
            JUSTC.WASM = isDev || !isBrowser ? await JUSTC.JUSTC() : await JUSTC.JUSTC(JUSTC.Config);
            if (JUSTC.CoreLogsEnabled) JUSTC.Console("log", "JUSTC WebAssembly module initialized.");
            JUSTC.JUSTC = null;
            delete JUSTC.JUSTC;
            JUSTC.Initialized = true;
        } catch (error) {
            JUSTC.Console("error", JUSTC.Errors.wasmFailed, error);
            JUSTC.Initialized = false;
        }
    };
    JUSTC.InitWASM = async function InitializeJUSTC(excludeLuau, attempt = 0) {
        while (!JUSTC.WASM) {
            attempt++;
            await JUSTC.Initialize(excludeLuau);
            if (attempt > 10) {
                JUSTC.Initialized = false;
                throw new JUSTC.Error(JUSTC.Errors.wasmInitFailed);
            }
        };
        if (JUSTC.WASM) {
            for (const [_unused, prfunc] of OBJECT.entries(JUSTC.PrivateFunctions.All)) {
                if (prfunc.NeedsWASM && !JUSTC.PrivateFunctions.Available.includes(prfunc.Name)) {
                    JUSTC.PrivateFunctions.Available.push(prfunc.Name);
                }
            }
            JUSTC.Initialized = true;
            JUSTC.HasLuau = JUSTC.WASM.ccall(
                'has_luau',
                'number',
                [], []
            ) === 1;
        } else {
            JUSTC.Initialized = false;
        }
    };
    JUSTC.IgnoreLogs = [
        'LUAU', 'JAVASCRIPT', 'CHARTYPE'
    ];
    JUSTC.DisplayLogs = function(result) {
        if (result.logfile && result.logfile.file && result.logfile.file != '') {
            throw new JUSTC.Error("Logfile cannot be created in browser.");
        };
        if (result.logs && ARRAY.isArray(result.logs)) {
            result.logs.forEach(log => {
                if (log.type != 'ECHO' && !JUSTC.IgnoreLogs.includes(log.type)) {
                    JUSTC.Console(
                        log.type == 'ERROR' ? 'error' : 'log',
                        `[JUSTC] (${log.time})`, log.message
                    );
                }
            });
        }
    };

    JUSTC.DefaultOutputMode = 'json';

    JUSTC.TryCatchLog = function(func, log) {
        try {
            return func()
        } catch (error) {
            CONSOLE.log(log);
            throw error
        }
    };

    JUSTC.Parse = function(code, execute = false, outputMode = JUSTC.DefaultOutputMode) {
        try {
            const resultPtr = JUSTC.WASM.ccall(
                'parse',
                'number',
                ['string', 'boolean', 'boolean', 'string'],
                [code, execute, false, outputMode]
            );

            const resultJson = JUSTC.WASM.UTF8ToString(resultPtr);
            JUSTC.WASM.ccall('free_string', null, ['number'], [resultPtr]);

            const result = JUSTC.TryCatchLog(()=>json_.parse(resultJson), resultJson);

            return result
        } catch (error) {
            CONSOLE.error(JUSTC.Errors.executionError, error);
            throw error;
        }
    };
    JUSTC.AsyncParse = async function(code, execute, outputMode = JUSTC.DefaultOutputMode) {
        return new PROMISE((resolve, reject) => {
            try {
                setTimeout(() => {
                    try {
                        const resultPtr = JUSTC.WASM.ccall(
                            'parse',
                            'number',
                            ['string', 'boolean', 'boolean', 'string'],
                            [code, execute, true, outputMode]
                        );

                        const resultJson = JUSTC.WASM.UTF8ToString(resultPtr);
                        JUSTC.WASM.ccall('free_string', null, ['number'], [resultPtr]);

                        const result = JUSTC.TryCatchLog(()=>json_.parse(resultJson), resultJson);

                        resolve(result)
                    } catch (error) {
                        console.error(JUSTC.Errors.executionError, error);
                        reject(new JUSTC.Error(JUSTC.Errors.executionError, error))
                    }
                }, 0)
            } catch (error) {
                reject(error)
            }
        })
    };

    JUSTC.CheckWASM = function() {
        if (!JUSTC.WASM) {
            throw new JUSTC.Error(JUSTC.Errors.initWasm);
        }
    };
    JUSTC.CheckInput = function(input) {
        if (!input || typeof input != 'string') {
            JUSTC.ErrorIfEnabled(STRING(input).length < 32 ? `"${STRING(input)}" is not valid JUSTC.` : ()=>{
                try {
                    if (!input.name) throw new JUSTC.Error('');
                    return `Provided ["${input.name}"] is not valid JUSTC.`;
                } catch (err) {
                    return "Provided code is not valid JUSTC.";
                }
            });
            return true;
        } else return false;
    };

    JUSTC.fromJSON = function(input) {
        if (ARRAY.isArray(input)) {
            throw new JUSTC.Error(JUSTC.Errors.arrayInput);
        } else {
            let output = '';
            for (const [name, value] of OBJECT.entries(json_.parse(json_.stringify(input)))) {
                output += (output.length > 0 ? ',' : '') + `${name}=${
                    typeof value === 'string' ? `"${value}"` :
                    value === true ? 'y' :
                    value === false ? 'n' :
                    value === null ? '' :
                    value
                }`;
            };
            return output + '.';
        }
    };

    JUSTC.OutputModes = [
        'json', 'xml', 'yaml', 'justo', 'justb'
    ];

    JUSTC.VFS = isBrowser ? class VirtualFileSystem {
        constructor() {
            this.files = new MAP();
        }

        createFile(filename, content, options = {}) {
            const {
                mimeType = 'text/plain',
                language = 'text',
                syntaxHighlighting = true
            } = options;

            this.files.set(filename, { content, mimeType, language });

            if (syntaxHighlighting) {
                this._registerFileInDevTools(filename, content, language);
            }

            return filename;
        }

        _registerFileInDevTools(filename, content, _language) {
            const blob = new BLOB([content], { type: 'text/plain' });
            const url = __URL__.createObjectURL(blob);

            const script = DOCUMENT.createElement('script');
            script.textContent = `//# sourceMappingURL=data:application/json;base64,${btoa(json_.stringify({
                version: 3,
                file: filename,
                sources: [filename],
                sourcesContent: [content],
                mappings: ""
            }))}`;

            DOCUMENT.head.appendChild(script);
            DOCUMENT.head.removeChild(script);
            __URL__.revokeObjectURL(url);
        }

        getFile(filename) {
            return this.files.get(filename);
        }

        listFiles() {
            return ARRAY.from(this.files.keys());
        }
    } : undefined;
    JUSTC.CurrentVFS = isBrowser && !isSafari ? new JUSTC.VFS() : undefined;

    JUSTO._pointers = new MAP();
    JUSTO._pointers.set('nan', NaN);
    JUSTO._pointers.set('inf', Infinity);
    JUSTO.parse = function(justoString) {
        if (typeof justoString !== 'string') throw new JUSTC.Error('JUSTO.parse requires a string');

        let pos = 0;
        const input = justoString;

        function peek() { return pos < input.length ? input[pos] : '\0'; }
        function advance() { return pos < input.length ? input[pos++] : '\0'; }
        function skipWhitespace() {
            while (pos < input.length && /\s/.test(input[pos])) pos++;
        }
        function match(c) {
            skipWhitespace();
            if (peek() === c) {
                advance();
                return true;
            }
            return false;
        }

        function parseString(quote) {
            skipWhitespace();
            if (peek() !== quote) return '';
            advance();

            let result = '';
            while (pos < input.length && peek() !== quote) {
                if (peek() === '\\') {
                    advance();
                    if (peek() === '\\') result += '\\';
                    else if (peek() === '"') result += '"';
                    else if (peek() === "'") result += "'";
                    else result += '\\' + peek();
                    advance();
                } else {
                    result += peek();
                    advance();
                }
            }
            if (peek() === quote) advance();
            return result;
        }

        function parseIdentifier() {
            skipWhitespace();
            let result = '';
            if (peek() == '"') return parseString('"');
            while (pos < input.length && /[a-zA-Z0-9_]/.test(peek())) {
                result += peek();
                advance();
            }
            return result;
        }

        function parseNumber() {
            skipWhitespace();
            let numStr = '';
            let hasDot = false;

            if (peek() === '-') {
                numStr += '-';
                advance();
            }

            while (pos < input.length && (/[0-9]/.test(peek()) || peek() === '.')) {
                if (peek() === '.') {
                    if (hasDot) break;
                    hasDot = true;
                }
                numStr += peek();
                advance();
            }

            return parseFloat(numStr);
        }

        function parseBoolean() {
            skipWhitespace();
            if (peek() === '1') {
                advance();
                return true;
            } else if (peek() === '0') {
                advance();
                return false;
            }
            return false;
        }

        function parseValue() {
            skipWhitespace();
            const c = peek();

            if (c === 'n') {
                advance();
                return parseNumber();
            }
            else if (c === '"') {
                return parseString('"');
            }
            else if (c === "'") {
                const ptrName = parseString("'");
                if (JUSTO._pointers.has(ptrName)) {
                    return JUSTO._pointers.get(ptrName);
                }
                return null;
            }
            else if (c === '1' || c === '0') {
                return parseBoolean();
            }
            else if (c === ';') {
                advance();
                return null;
            }
            else if (c === 'o') {
                advance();
                return parseObject();
            }
            else if (c === 'a') {
                advance();
                return parseArray();
            }

            return null;
        }

        function parseObject() {
            if (!match('{')) return {};

            const obj = {};
            while (!match('}')) {
                skipWhitespace();
                const key = parseIdentifier();
                if (!key) break;

                if (!match(':')) break;

                const val = parseValue();
                obj[key] = val;

                skipWhitespace();
                if (peek() === ';') advance();
            }

            return obj;
        }

        function parseArray() {
            if (!match('[')) return [];

            const arr = [];
            while (!match(']')) {
                const val = parseValue();
                arr.push(val);

                skipWhitespace();
                if (peek() === ',') advance();
            }

            return arr;
        }

        return parseValue();
    };
    JUSTO.stringify = function(value) {
        if (value === null || value === undefined) return ';';
        if (typeof value === 'number') {
            if (ISNAN(value)) return "'nan'";
            if (!ISFINITE(value)) return "'inf'";
            return 'n' + STRING(value);
        }
        if (typeof value === 'string') {
            let escaped = value.replace(/\\/g, '\\\\').replace(/"/g, '\\"');
            return '"' + escaped + '"';
        }
        if (typeof value === 'boolean') return value ? '1' : '0';
        if (ARRAY.isArray(value)) {
            let result = 'a[';
            for (let i = 0; i < value.length; i++) {
                if (i > 0) result += ',';
                result += JUSTO.stringify(value[i]);
            }
            result += ']';
            return result;
        }
        if (typeof value === 'object') {
            let result = 'o{';
            let first = true;
            for (const [key, val] of OBJECT.entries(value)) {
                if (!first) result += ';';
                first = false;
                result += key + ':' + JUSTO.stringify(val);
            }
            result += '}';
            return result;
        }
        return ';';
    };
    JUSTO.registerPointer = function(name, value) {
        if (typeof name !== 'string') throw new JUSTC.Error('Pointer name must be a string');
        JUSTO._pointers.set(name, value);
    };
    JUSTO.unregisterPointer = function(name) {
        if (typeof name !== 'string') throw new JUSTC.Error('Pointer name must be a string');
        JUSTO._pointers.delete(name);
    };
    JUSTO.getPointer = function(name) {
        if (typeof name !== 'string') throw new JUSTC.Error('Pointer name must be a string');
        return JUSTO._pointers.get(name);
    };

    JUSTC.JavaScriptImports = {};
    JUSTC.JavaScriptImports.Functions = new MAP();
    JUSTC.JavaScriptImports.CallbackPtr = null;
    JUSTC.JavaScriptImports.CallFunction = (namePtr, argsJUSTOPtr, resultJUSTOPtrPtr) => {
        const name = JUSTC.WASM.UTF8ToString(namePtr);
        const argsJUSTO = JUSTC.WASM.UTF8ToString(argsJUSTOPtr);

        const JSFunc = JUSTC.JavaScriptImports.Functions.get(name);
        if (!JSFunc) {
            const nullJUSTO = ';';
            const resultPtr = JUSTC.WASM._malloc(nullJUSTO.length + 1);
            JUSTC.WASM.stringToUTF8(nullJUSTO, resultPtr, nullJUSTO.length + 1);
            JUSTC.WASM.setValue(resultJUSTOPtrPtr, resultPtr, '*');
            return;
        }

        try {
            const argsArray = JUSTO.parse(argsJUSTO);

            const result = JSFunc(...argsArray);

            const resultJUSTO = JUSTO.stringify(result);

            const resultPtr = JUSTC.WASM._malloc(resultJUSTO.length + 1);
            JUSTC.WASM.stringToUTF8(resultJUSTO, resultPtr, resultJUSTO.length + 1);
            JUSTC.WASM.setValue(resultJUSTOPtrPtr, resultPtr, '*');
        } catch (error) {
            JUSTC.Console('error', 'JUSTC JavaScript User Function error:', error);
            const nullJUSTO = ';';
            const resultPtr = JUSTC.WASM._malloc(nullJUSTO.length + 1);
            JUSTC.WASM.stringToUTF8(nullJUSTO, resultPtr, nullJUSTO.length + 1);
            JUSTC.WASM.setValue(resultJUSTOPtrPtr, resultPtr, '*');
        }
    };
    JUSTC.JavaScriptListeners = {};
    JUSTC.JavaScriptListeners.VariableUpdate = {};
    JUSTC.JavaScriptListeners.VariableUpdate.Callbacks = new MAP;
    JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr = null;
    JUSTC.JavaScriptListeners.VariableUpdate.Callback = (namePtr, valueJUSTOPtr) => {
        const name = JUSTC.WASM.UTF8ToString(namePtr);
        const valueJUSTO = JUSTC.WASM.UTF8ToString(valueJUSTOPtr);
        const value = JUSTO.parse(valueJUSTO);

        for (const callback of JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.values()) {
            try {
                callback(name, value);
            } catch (e) {
                JUSTC.Console('error', 'JUSTC variable update listener error:', e);
            }
        }
    };
    JUSTC.JavaScriptListeners.VariableUpdate.ID = 0;
    JUSTC.JavaScriptListeners.VariableUpdate.Add = (callback) => {
        JUSTC.CheckWASM();
        if (typeof callback !== 'function') throw new JUSTC.Error('Callback must be a function');

        const id = JUSTC.JavaScriptListeners.VariableUpdate.ID++;
        JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.set(id, callback);

        if (JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.size === 1 && !JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr) {
            JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr = JUSTC.WASM.addFunction(JUSTC.JavaScriptListeners.VariableUpdate.Callback, 'vii');
            JUSTC.WASM.ccall('addVariableUpdateListener', 'number', ['number'], [JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr]);
        }

        return id;
    };
    JUSTC.JavaScriptListeners.VariableUpdate.Remove = (id) => {
        JUSTC.CheckWASM();
        if (!id) return;

        JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.delete(id);

        if (JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.size === 0 && JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr) {
            JUSTC.WASM.removeFunction(JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr);
            JUSTC.WASM.ccall('clearVariableUpdateListeners', null, [], []);
            JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr = null;
        }
    };
    JUSTC.JavaScriptListeners.VariableUpdate.Clear = () => {
        JUSTC.CheckWASM();

        JUSTC.JavaScriptListeners.VariableUpdate.Callbacks.clear();

        if (JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr) {
            JUSTC.WASM.removeFunction(JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr);
            JUSTC.WASM.ccall('clearVariableUpdateListeners', null, [], []);
            JUSTC.JavaScriptListeners.VariableUpdate.CallbackPtr = null;
        }
    };

    JUSTC.Check = (code) => {
        if (JUSTC.CheckInput(code)) throw new JUSTC.Error(JUSTC.Errors.wrongInputType);
        JUSTC.CheckWASM();
    };
    JUSTC.RegisterImports = function(imports) {
        if (isBrowser && !isSafari && imports && ARRAY.isArray(imports) && imports.length > 0) setTimeout(() => {
            try {
                for (const [url, content, type] of imports) {
                    JUSTC.CurrentVFS.createFile(url, content, {mimeType: type});
                }
            } catch (_) {}
        }, 0);
    };
    JUSTC.RunAsync = async (code, doExecute, ...args) => {
        JUSTC.Check(code);
        const result = await JUSTC.AsyncParse(code, doExecute, ...args);
        if (result.error) {
            throw new JUSTC.Error(result.error);
        } else {
            JUSTC.RegisterImports(result.imported);
            if (doExecute) JUSTC.DisplayLogs(result);
            return result.return || {};
        }
    };
    JUSTC.Taskify = (doExecute = false, outputMode = JUSTC.DefaultOutputMode, code) => {
        const tasks = [];
        for (const code_ of code) {
            tasks.push(async()=>{
                return await JUSTC.RunAsync(code_, doExecute, outputMode);
            });
        }
        return tasks
    };
    JUSTC.AsyncOutput = async function(bool, args) {
        if (!JUSTC.Initialized) await JUSTC.InitWASM();

        let outputMode = JUSTC.DefaultOutputMode;
        let start = 0;
        let end = args.length;
        const lastArg = args[end - 1];
        if (JUSTC.OutputModes.includes(lastArg)) {
            outputMode = lastArg;
            end -= 1;
        } else if (JUSTC.OutputModes.includes(args[0])) {
            outputMode = args[0];
            start += 1;
        };
        return await PROMISE.all(await PROMISE.all(JUSTC.Taskify(bool, outputMode, args.slice(start, end))))
    };
    JUSTC.Output = {
        parse: isBrowser || !JUSTC.Experiments ? function(code, outputMode = JUSTC.DefaultOutputMode) {
            JUSTC.Check(code);
            if (!JUSTC.OutputModes.includes(outputMode)) throw new JUSTC.Error(JUSTC.Errors.outputMode);

            const result = JUSTC.Parse(code, false, outputMode);
            if (result.error) {
                throw new JUSTC.Error(result.error);
            } else {
                JUSTC.RegisterImports(result.imported);
                return result.return || {};
            }
        } : async function(...code) {
            return await JUSTC.AsyncOutput(false, code)
        },
        execute: isBrowser || !JUSTC.Experiments ? function(code, outputMode = JUSTC.DefaultOutputMode) {
            JUSTC.Check(code);
            if (!JUSTC.OutputModes.includes(outputMode)) throw new JUSTC.Error(JUSTC.Errors.outputMode);

            const result = JUSTC.Parse(code, true, outputMode);
            if (result.error) {
                throw new JUSTC.Error(result.error);
            } else {
                JUSTC.RegisterImports(result.imported);
                JUSTC.DisplayLogs(result);
                return result.return || {};
            }
        } : async function(...code) {
            return await JUSTC.AsyncOutput(true, code)
        },
        initialize: isBrowser ? async function(excludeLuau = false) {
            if (typeof excludeLuau != 'boolean') throw new JUSTC.Error(JUSTC.Errors.boolInput0);
            await JUSTC.InitWASM(excludeLuau);
        } : async function() {
            await JUSTC.InitWASM();
        },
        stringify: function(JavaScriptObjectNotation) {
            if (typeof JavaScriptObjectNotation != 'object') throw new JUSTC.Error(JUSTC.Errors.objectInput);
            return JUSTC.fromJSON(JavaScriptObjectNotation);
        },
        parseAsync: isBrowser && JUSTC.Experiments ? async function(...code) {
            return await JUSTC.AsyncOutput(false, code);
        } : undefined,
        executeAsync: isBrowser && JUSTC.Experiments ? async function(...code) {
            return await JUSTC.AsyncOutput(true, code);
        } : undefined,

        globals: {
            register: function(name, value, isConst = true) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');

                const justoValue = JUSTO.stringify(value);

                const result = JUSTC.WASM.ccall(
                    'registerGlobal',
                    'number',
                    ['string', 'string'],
                    [name, justoValue]
                );

                return result === 1;
            },
            get: function(name) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');

                const resultPtr = JUSTC.WASM.ccall('getGlobal', 'number', ['string'], [name]);
                const justoValue = JUSTC.WASM.UTF8ToString(resultPtr);
                JUSTC.WASM.ccall('free_string', null, ['number'], [resultPtr]);

                return JUSTO.parse(justoValue);
            },
            has: function(name) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');

                const result = JUSTC.WASM.ccall('hasGlobal', 'number', ['string'], [name]);
                return result === 1;
            },
            unregister: function(name) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');

                const result = JUSTC.WASM.ccall('unregisterGlobal', 'number', ['string'], [name]);
                return result === 1;
            },
            clear: function() {
                JUSTC.CheckWASM();
                JUSTC.WASM.ccall('clearGlobals', null, [], []);
            },
        },
        userFunctions: {
            register: function(name, func, isConst = true) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');
                if (typeof func !== 'function') throw new JUSTC.Error('Function must be a function');

                JUSTC.JavaScriptImports.Functions.set(name, func);
                if (!JUSTC.JavaScriptImports.CallbackPtr) {
                    JUSTC.JavaScriptImports.CallbackPtr = JUSTC.WASM.addFunction(JUSTC.JavaScriptImports.CallFunction, 'viii');
                }

                const result = JUSTC.WASM.ccall(
                    'registerFunction',
                    'number',
                    ['string', 'number', 'number'],
                    [name, JUSTC.JavaScriptImports.CallbackPtr, isConst ? 1 : 0]
                );
                return result === 1;
            },
            unregister: function(name) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');
                JUSTC.JavaScriptImports.Functions.delete(name);
                const result = JUSTC.WASM.ccall('unregisterFunction', 'number', ['string'], [name]);
                return result === 1;
            },
            clear: function() {
                JUSTC.CheckWASM();
                JUSTC.JavaScriptImports.Functions.clear();
                JUSTC.WASM.ccall('clearUserFunctions', null, [], []);
            },
            has: function(name) {
                JUSTC.CheckWASM();
                if (typeof name !== 'string') throw new JUSTC.Error('Name must be a string');
                return JUSTC.JavaScriptImports.Functions.has(name);
            },
        },
        listeners: {
            variableUpdate: {
                register: function(callback) {
                    return JUSTC.JavaScriptListeners.VariableUpdate.Add(callback);
                },
                unregister: function(id) {
                    return JUSTC.JavaScriptListeners.VariableUpdate.Remove(id);
                },
                clear: function() {
                    JUSTC.JavaScriptListeners.VariableUpdate.Clear();
                },
            },
        },
        classes: {
            unregister: function(id) {
                JUSTC.CheckWASM();
                if (typeof id !== 'number' && typeof id !== 'bigint' && typeof id !== 'string') throw new JUSTC.Error('ClassID must be a BigInt, a number, or a string');
                if (typeof id == 'string' && !/^\d{1,20}$/.test(id) && !/^0x[0-9a-fA-F]{1,16}$/.test(id)) throw new JUSTC.Error('Invalid ClassID');
                if (!(BIGINT(id) <= 18446744073709551615n) || id < 0) throw new JUSTC.Error('ClassID must be within UInt64 range');
                return JUSTC.WASM.ccall(
                    'unregisterClass',
                    'number',
                    ['string'],
                    [STRING(id)]
                ) === 1;
            },
            clear: function() {
                JUSTC.CheckWASM();
                JUSTC.WASM.ccall('clearClasses', null, [], []);
            }
        },
        pointers: {
            free: function(id) {
                JUSTC.CheckWASM();
                if (typeof id !== 'number' && typeof id !== 'bigint' && typeof id !== 'string') throw new JUSTC.Error('Pointer must be a BigInt, a number, or a string');
                if (typeof id == 'string' && !/^\d{1,20}$/.test(id) && !/^0x[0-9a-fA-F]{1,16}$/.test(id)) throw new JUSTC.Error('Invalid Pointer');
                if (!(BIGINT(id) <= 18446744073709551615n) || id < 0) throw new JUSTC.Error('Pointer must be within UInt64 range');
                return JUSTC.WASM.ccall(
                    'freePointer',
                    'number',
                    ['string'],
                    [STRING(id)]
                ) === 1;
            },
            clear: function() {
                JUSTC.CheckWASM();
                JUSTC.WASM.ccall('clearJUSTCPointers', null, [], []);
            }
        },
        cleanup: function() {
            JUSTC.CheckWASM();
            JUSTC.WASM.ccall('cleanup', null, [], []);
        }
    };
    JUSTC.Output.compile = isBrowser || !JUSTC.Experiments ? function(code) {
        return (new TXTENCDR()).encode(STRING(JUSTC.Output.execute(code, 'justb')));
    } : async function (code) {
        return (new TXTENCDR()).encode(STRING(await JUSTC.AsyncOutput(true, [code, 'justb'])));
    };
    JUSTC.Output.compileAsync = isBrowser && JUSTC.Experiments ? async function (code) {
        return (new TXTENCDR()).encode(STRING(await JUSTC.AsyncOutput(true, [code, 'justb'])));
    } : undefined;
    JUSTC.Public = {
        get [Symbol.toStringTag]() {
            return 'JUSTC'
        },
        get ["version"]() {
            JUSTC.CheckWASM();
            const resultptr = JUSTC.WASM.ccall("version", "number");
            const result = JUSTC.WASM.UTF8ToString(resultptr);
            JUSTC.WASM.ccall(
                'free_string',
                null,
                ['number'],
                [resultptr]
            );
            if (!JUSTC.GetVersion()) {
                JUSTC.SetVersion(result);
            }
            return result
        }
    };
    for (const [name, value] of OBJECT.entries(JUSTC.Output)) {
        if (value != undefined) OBJECT.defineProperty(JUSTC.Public, name, {
            value,
            writable: false,
            configurable: false,
            enumerable: true
        });
    };
    JUSTC.Private = function(what) {
        if (!what || typeof what != 'string' || what.length < 1) throw new JUSTC.Error(JUSTC.Errors.arg0);
        if (JUSTC.PrivateFunctions.Available.includes(what)) {
            return JUSTC.PrivateFunctions.All[JUSTC.PrivateFunctions.WhatToName[what]].Return;
        } else {
            throw new JUSTC.Error(`JUSTC.requestPermissions: "${what}" is either not available or does not exist.`);
        }
    };
    JUSTC.HiddenOutput = {
        requestPermissions: function(what) {
            if (ARRAY.isArray(what)) {
                const output = [];
                for (const item of what) {
                    output.push(JUSTC.Private(item));
                }
                return output
            }
            return JUSTC.Private(what);
        }
    };
    for (const [name, value] of OBJECT.entries(JUSTC.HiddenOutput)) {
        OBJECT.defineProperty(JUSTC.Public, name, {
            value,
            writable: false,
            configurable: false,
            enumerable: false
        })
    };

    JUSTO.Output = function(where) {
        OBJECT.defineProperty(where, 'JUSTO', {
            get: function() {
                const JUSTOPublic = {
                    parse: function ParseJUSTO(code) {
                        return JUSTO.parse(code);
                    },
                    stringify: function StringifyJUSTO(JavaScriptObjectNotation) {
                        return JUSTO.stringify(JavaScriptObjectNotation);
                    },
                    get ['version']() {
                        return JUSTC.Public.version;
                    },
                };
                OBJECT.defineProperty(JUSTOPublic, 'pointers', {
                    value: {
                        register: function RegisterPointer(key, value) {
                            JUSTO.registerPointer(key, value);
                        },
                        unregister: function UnregisterPointer(key) {
                            JUSTO.unregisterPointer(key);
                        },
                        get: function GetPointer(key) {
                            JUSTO.getPointer(key);
                        },
                    },
                    writable: false,
                    configurable: false,
                    enumerable: false
                });
                return JUSTOPublic;
            },
            set: function() {
                JUSTC.ErrorIfEnabled(JUSTC.Errors.redefineO);
            },
            configurable: false
        });
    };
    JUSTB.Output = function(where) {
        OBJECT.defineProperty(where, 'JUSTB', {
            get: function() {
                return OBJECT.freeze({
                    load: function loadJUSTB(binary, outputMode = JUSTC.DefaultOutputMode) {
                        JUSTC.CheckWASM();
                        if (!JUSTC.OutputModes.includes(outputMode)) throw new JUSTC.Error(JUSTC.Errors.outputMode);

                        let data;
                        if (binary instanceof UI8A) {
                            data = binary;
                        } else if (binary instanceof AB) {
                            data = new UI8A(binary);
                        } else if (typeof binary === 'string') {
                            const encoder = new TXTENCDR();
                            data = encoder.encode(binary);
                        } else {
                            throw new JUSTC.Error('JUSTB.load expects Uint8Array, ArrayBuffer, or binary string.');
                        }

                        const ptr = JUSTC.WASM._malloc(data.length + 1);
                        if (!ptr) throw new JUSTC.Error('JUSTB.load: Failed to allocate memory in WASM.');

                        JUSTC.WASM.HEAPU8.set(data, ptr);
                        JUSTC.WASM.HEAPU8[ptr + data.length] = 0;

                        let result = new JUSTC.Error();
                        try {
                            const resultPtr = JUSTC.WASM.ccall(
                                'load',
                                'number',
                                ['number', 'string'],
                                [ptr, outputMode]
                            );

                            const resultJson = JUSTC.WASM.UTF8ToString(resultPtr);
                            JUSTC.WASM.ccall('free_string', null, ['number'], [resultPtr]);

                            result = JUSTC.TryCatchLog(() => json_.parse(resultJson), resultJson).return || {};
                        } finally {
                            JUSTC.WASM._free(ptr);
                        }
                        return result;
                    },
                    get ['version']() {
                        return JUSTC.Public.version;
                    },
                });
            },
            set: function() {
                JUSTC.ErrorIfEnabled(JUSTC.Errors.redefineB);
            },
            configurable: false
        });
    };

    JUSTC.CreateAsyncExports = function() {
        const exports = {};
        for (const [name, value] of OBJECT.entries(JUSTC.Output)) {
            exports[name] = typeof value === 'function' ? async function(...args) {
                if (!JUSTC.Initialized) await JUSTC.InitWASM();
                return value.constructor.name === "AsyncFunction" ? await value(...args) : value(...args);
            } : value;
        }
        for (const [name, value] of OBJECT.entries(JUSTC.HiddenOutput)) {
            OBJECT.defineProperty(exports, name, {
                value: async function(...args) {
                    if (!JUSTC.Initialized) await JUSTC.InitWASM();
                    return value.constructor.name === "AsyncFunction" ? await value(...args) : value(...args);
                },
                writable: false,
                configurable: false,
                enumerable: false
            });
        }
        OBJECT.defineProperty(exports, Symbol.toStringTag, {
            value: 'JUSTC',
            configurable: false
        });
        OBJECT.defineProperty(exports, "defineWASM", {
            value: function(WASMmodule) {
                if (!JUSTC.JUSTC) {
                    JUSTC.JUSTC = WASMmodule;
                }
            },
            writable: false,
            configurable: false,
            enumerable: false
        });
        JUSTO.Output(exports);
        JUSTB.Output(exports);

        return exports;
    };

    if (isBrowser) {
        if ("JUSTC" in globalThis_.window || "$JUSTC" in globalThis_.window || "JUSTO" in globalThis_.window) throw new JUSTC.Error(JUSTC.Errors.environment);
        OBJECT.defineProperty(globalThis_.window, 'JUSTC', {
            get: function() {
                return OBJECT.freeze(JUSTC.Public);
            },
            set: function(command) {
                if (typeof command === 'string' && command.length > 0) {
                    const vars = {
                        "version": "Public.version",
                        "core.lexer": "PrivateFunctions.All.Lexer.Return",
                        "core.parser": "PrivateFunctions.All.Parser.Return",
                        "detected.env": "PrivateFunctions.All.env.Return",
                        "options.silent": "Silent",
                        "core.errors": "ErrorEnabled",
                        "core.logs": "CoreLogsEnabled",
                        "options.experiments": "Experiments",
                        "options.defaultOutputMode": "DefaultOutputMode",
                    };
                    const types = {
                        "version": "string",
                        "options.silent": "boolean",
                        "options.core.errors": "boolean",
                        "options.core.logs": "boolean",
                        "options.experiments": "boolean",
                        "options.defaultOutputMode": "string",
                    };
                    const unredefinable = [
                        "version", "core.lexer", "core.parser", "detected.env"
                    ];
                    const bools = {
                        "true": true,
                        "yes": true,
                        "y": true,
                        "1": true,
                        "+": true,
                        "!0": true,
                        "false": false,
                        "no": false,
                        "n": false,
                        "0": false,
                        "-": false,
                        "!1": false,
                    };

                    const args = command.split(' ').filter(arg => arg);
                    function print_(arg) {
                        CONSOLE.info(typeof JUSTC[vars[arg]] === 'function' ? JUSTC[vars[arg]]() : JUSTC[vars[arg]])
                    };
                    if (args.length == 1 && args[0] in vars) print_(args[0]);
                    else {
                        switch (args[0]) {
                            case 'print':
                                if (args.length != 2 ||!(args[1]in vars)) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else print_(args[1]);
                                break;
                            case 'set':
                                if (args.length != 3 ||!(args[1]in vars)||
                                    typeof JUSTC[vars[args[1]]]==='function' || (()=>{
                                        const varname = args[1];
                                        const vartype = types[varname];
                                        const set = args[2];
                                        switch (vartype) {
                                            case 'boolean':
                                                return!(set in bools);
                                            case 'string':
                                                return (varname === 'options.defaultOutputMode' && !JUSTC.OutputModes.includes(set));
                                            default:
                                                return false;
                                        }
                                    })() || unredefinable.includes(args[1])
                                ) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else if (types[args[1]] === 'boolean') {
                                    JUSTC[vars[args[1]]] = bools[args[2]];
                                } else {
                                    JUSTC[vars[args[1]]] = args[2];
                                }
                                break;
                            case 'switch':
                                if (args.length != 2 ||!(args[1]in vars||
                                    types[args[1]] != 'boolean'
                                )) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else {
                                    JUSTC[vars[args[1]]] = !JUSTC[vars[args[1]]];
                                }
                                break;
                            case 'enable':
                            case 'allow':
                                if (args.length != 2 ||!(args[1]in vars||
                                    types[args[1]] != 'boolean'
                                )) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else {
                                    JUSTC[vars[args[1]]] = true;
                                }
                                break;
                            case 'disable':
                            case 'disallow':
                                if (args.length != 2 ||!(args[1]in vars||
                                    types[args[1]] != 'boolean'
                                )) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else {
                                    JUSTC[vars[args[1]]] = false;
                                }
                                break;
                            case 'execute':
                            case 'parse':
                                if (args.length < 2) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else {
                                    CONSOLE.log(JUSTC.Output[args[0]](args.slice(1).join(' ')))
                                }
                                break;
                            case 'help':
                                CONSOLE.info('https://just.js.org/justc');
                                break;
                            case 'run':
                                if (args.length < 3 ||![
                                    'core.lexer',
                                    'core.parser'
                                ].includes(args[1])) JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                else {
                                    const func = args[1] === 'core.lexer' ? JUSTC.PrivateFunctions.All.Lexer.Return : JUSTC.PrivateFunctions.All.Parser.Return;
                                    CONSOLE.log(func(args.slice(2).join(' ')))
                                }
                                break;
                            default:
                                JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                                break;
                        }
                    }
                } else {
                    JUSTC.ErrorIfEnabled(JUSTC.Errors.redefine);
                }
            },
            configurable: false
        });
        OBJECT.defineProperty(globalThis_.window, '$JUSTC', {
            get: function() {
                return JUSTC.Output.execute;
            },
            set: function() {
                JUSTC.ErrorIfEnabled('$'+JUSTC.Errors.redefine);
            },
            configurable: false
        });
        JUSTO.Output(globalThis_.window);
        JUSTB.Output(globalThis_.window);
    } else if (isModule) {
        if (isNode) JUSTC.PrivateFunctions.Available.push('core.cli');
        module.exports = JUSTC.CreateAsyncExports();
    } else if (isAMD) {
        define(['require'], function(require) {
            return JUSTC.CreateAsyncExports();
        });
    } else {
        throw new JUSTC.Error('Unsupported environment.');
    }
})();
