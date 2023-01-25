// c4: A stack-based VM/OS for PCs and development boards

#include "c4.h"

// ----------------------------------
// The Virtual Machine / OS
// ----------------------------------
ST_T st;
STK_T stk;
CELL locals[LOCALS_SZ], lstk[LSTK_SZ+1], tempWords[10];
CELL sp, rsp, lsp, lb, isError, sb, rb, lexicon;
CELL BASE, STATE, oHERE, oVHERE, seed, t1, t2, ir;
byte *code, *vars, *y, *pc;
DICT_E *dictEnd;
char word[32], tib[256], *in;

void vmReset() {
    sb = 2, rb = (STK_SZ-2);
    sp = sb - 1, rsp = rb + 1;
    HERE = oHERE = 2;
    lsp = lb = VHERE = oVHERE = lexicon = 0;
    for (int i = 0; i < MEM_SZ; i++) { st.bytes[i] = 0; }
    code = &st.bytes[CELL_SZ*3];
    vars = (code+CODE_SZ+4);
    dictEnd = (DICT_E*)(&st.bytes[MEM_SZ]);
    LAST = MEM_SZ;
    systemWords();
}

inline void push(CELL v) { stk.i[++sp] = v; }
inline CELL pop() { return stk.i[sp--]; }
inline void rpush(CELL v) { stk.i[--rsp] = v; }
inline CELL rpop() { return stk.i[rsp++]; }
inline float fpop() { return stk.f[sp--]; }

#ifdef NEEDS_ALIGN
WORD GET_WORD(byte *l) { return *l | (*(l + 1) << 8); }
long GET_LONG(byte *l) { return GET_WORD(l) | GET_WORD(l + 2) << 16; }
void SET_WORD(byte *l, WORD v) { *l = (v & 0xff); *(l + 1) = (byte)(v >> 8); }
void SET_LONG(byte *l, long v) { SET_WORD(l, v & 0xFFFF); SET_WORD(l + 2, (WORD)(v >> 16)); }
#else
inline WORD GET_WORD(byte *a) { return *(WORD*)a; }
inline long GET_LONG(byte *a) { return *(long*)a; }
inline void SET_WORD(byte *a, WORD v) { *(WORD*)a = v; }
inline void SET_LONG(byte *a, long v) { *(long*)a = v; }
#endif // NEEDS_ALIGN

void CComma(CELL v) { code[HERE++] = (byte)v; }
void WComma(CELL v) { SET_WORD(&code[HERE], (WORD)v); HERE += 2; }
void Comma(CELL v) { SET_LONG(&code[HERE], v); HERE += CELL_SZ; }
char Lower(char c) { return BTW(c,'A','Z') ? c|0x20 : c; }

int strLen(const char *str) {
    int l = 0;
    while (*(str++)) { ++l; }
    return l;
}

int strEq(const char *x, const char *y) {
    while (*x && *y && (*x == *y)) { ++x; ++y; }
    return (*x || *y) ? 0 : 1;
}

int strEqI(const char *x, const char *y) {
    while (*x && *y) {
        if (Lower(*x) != Lower(*y)) { return 0; }
        ++x; ++y;
    }
    return (*x || *y) ? 0 : 1;
}

char *strCpy(char *d, const char *s) {
    char *x = d;
    while (*s) { *(x++) = *(s++); }
    *x = 0;
    return d;
}

char *strCat(char *d, const char *s) {
    char *x = d + strLen(d);
    strCpy(x, s);
    return d;
}

char *strCatC(char *d, char c) {
    char *x = d + strLen(d);
    *(x++) = c;
    *(x) = 0;
    return d;
}

char *rTrim(char *d) {
    char *x = d + strLen(d);
    while ((d <= x) && (*x <= ' ')) { *(x--) = 0; }
    return d;
}

void printStringF(const char *fmt, ...) {
    char *buf = (char*)&vars[VARS_SZ - 100];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 100, fmt, args);
    va_end(args);
    printString(buf);
}

void printBase(CELL num, CELL base) {
    UCELL n = (UCELL) num, isNeg = 0;
    if ((base == 10) && (num < 0)) { isNeg = 1; n = -num; }
    char *cp = (char *)&vars[VARS_SZ];
    *(cp--) = 0;
    do {
        int x = (n % base) + '0';
        n = n / base;
        *(cp--) = ('9' < x) ? (x+7) : x;
    } while (n);
    if (isNeg) { printChar('-'); }
    printString(cp+1);
}

void fDotS() {
    if (sp<sb) { sp=sb-1; }
    printString("(");
    for (int d = sb; d <= sp; d++) {
        if (sb < d) { printChar(' '); }
        printBase(stk.i[d], BASE);
    }
    printString(")");
}

byte *doType(byte *a, int l, int delim) {
    if (l < 0) { l = 0; while (a[l]!=delim) { ++l; } }
    byte *e = a+l;
    while (a < e) {
        char c = (char)*(a++);
        if (c == '%') {
            c = *(a++); 
            switch (c) {
            case 'b': printBase(pop(), 2);          break;
            case 'c': printChar((char)pop());       break;
            case 'd': printBase(pop(), 10);         break;
            case 'e': printChar(27);                break;
            case 'f': printStringF("%f", fpop());   break;
            case 'g': printStringF("%g", fpop());   break;
            case 'i': printBase(pop(), BASE);       break;
            case 'n': printString("\r\n");          break;
            case 'q': printChar('"');               break;
            case 's': printString((char*)pop());    break;
            case 't': printChar(9);                 break;
            case 'x': printBase(pop(), 16);         break;
            default: printChar(c);                  break;
            }
        } else { printChar(c); }
    }
    if (delim) { ++e; }
    return e;
}

void fCommaOp() {
    ir = *(pc++);
    if (ir == '1') { CComma(pop()); }
    else if (ir == '2') { WComma(pop()); }
    else if (ir == '4') { Comma(pop()); }
    // if (oHERE < HERE) { oHERE = HERE; }
}

void fWord() {
    byte *wd = BTOS;
    while (*in && (*in < 33)) { ++in; }
    int l = 0;
    while (*in && (32 < *in)) {
        *(wd++) = *(in++);
        ++l;
    }
    *wd = 0;
    push(l);
}

int getWord(char* wd) {
    push((CELL)wd);
    fWord();
    int l = pop();
    DROP1;
    return l;
}

int isTempWord(const char* nm) {
    return ((nm[0] == 'T') && BTW(nm[1], '0', '9') && (nm[2] == 0));
}

DICT_E *doFindInLex(const char *name, int lex) {
    int len = strLen(name);
    DICT_E *dp = (DICT_E*)&st.bytes[LAST];
    while (dp < dictEnd) {
        if ((len == dp->len) && (dp->lexicon == lex) && strEq(dp->name, name)) {
            return dp;
        }
        dp++;
    }
    return 0;
}

DICT_E *doFindInternal(const char *name) {
    DICT_E *ret = doFindInLex(name, lexicon);
    if (ret == NULL) { ret = doFindInLex(name, 0); }
    return ret;
}

void fCreate() {
    push(getWord(word));
    if (TOS == 0) { return; }
    if (isTempWord(word)) {
        tempWords[word[1] - '0'] = HERE;
        return;
    }
    if (doFindInternal(word)) { printStringF("-redef:[%s]-", word); }
    LAST -= sizeof(DICT_E);
    DICT_E *dp = (DICT_E*)&st.bytes[LAST];
    dp->xt = (USHORT)HERE;
    dp->flags = 0;
    dp->lexicon = (byte)lexicon;
    if (NAME_LEN <= strLen(word)) {
        printStringF("-[%s] truncated-", word);
        word[NAME_LEN - 1] = 0;
    }
    strCpy(dp->name, word);
    dp->len = strLen(dp->name);
}

int handleInput(int c, int echo) {
    if (c == 13) {
        *in = 0;
        if (echo) { printChar(' '); }
        return 1;
    }
    if (isBackSpace(c) && (tib < in)) {
        in--;
        if (echo) { char b[] = { 8, 32, 8, 0 }; printString(b); }
        return 0;
    }
    if (c == 9) { c = 32; }
    if (BTW(c, 32, 126)) { *(in++) = c; if (echo) { printChar(c); } }
    return 0;
}

void fAccept() {
    DICT_E *idle = doFindInternal("idle");
    in = tib;
    while (1) {
        if (!charAvailable()) {
            if (idle) { run(idle->xt); }
            continue;
        }
        if (handleInput(getChar(), 1)) { return; }
    }
}

byte *doFile(CELL ir, byte *pc) {
    switch (*(pc++)) {
    case 'O': fOpen(); break;
    case 'D': fDelete(); break;
    case 'I': fList(); break;
    case 'S': fSaveSys(); break;
    case 's': if (fLoadSys()) { pc = 0; } break;
    case 'L': fLoad((char*)pop()); break;
    case 'i': push((CELL)stdin); break;
    case 'o': push((CELL)stdout); break;
    case 'R': fGetC(); break;
    case 'r': fRead(); break;
    case 'G': fGetS(); break;
    case 'W': fPutC(); break;
    case 'w': fWrite(); break;
    case 'C': fClose(); break;
    case 'E': fSeek(); break;
    case 'T': fTell(); break;
    }
    return pc;
}

CELL doRand() {
    if (seed == 0) { seed = doTimer(); }
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed & 0x7FFFFFFF;
}

void fStore() { SET_LONG(BTOS, NOS); DROP2; }
void fFetch() { TOS = GET_LONG(BTOS); }
void fCharOp() {
    ir = *(pc++); if (ir == '@') { TOS = *BTOS; }
    else if (ir == '!') { *BTOS = (byte)NOS; DROP2; }
}
void fWordOp() {
    ir = *(pc++); if (ir == '@') { TOS = GET_WORD(BTOS); }
    else if (ir == '!') { SET_WORD(BTOS, (WORD)TOS); DROP2; }
}
void fType() { t1 = pop(); y = (byte*)pop(); while (t1--) printChar(*(y++)); }
void fTypeQ() { printString((char*)pop()); }
void fTypeF1() { pc = doType(pc, -1, '"'); }
void fTypeF2() { doType((byte*)pop(), -1, 0); }
void fDup() { t1 = TOS; push(t1); }
void fSwap() { t1 = TOS; TOS = NOS; NOS = t1; }
void fOver() { t1 = NOS; push(t1); }
void fDrop() { DROP1; }
void fSlashMod() { t1 = NOS; t2 = TOS; NOS = t1 / t2; TOS = t1 % t2; }
void fMod()  { NOS %= TOS; DROP1; }
void fAdd()  { NOS += TOS; DROP1; }
void fSub()  { NOS -= TOS; DROP1; }
void fMult() { NOS *= TOS; DROP1; }
void fDiv()  { NOS /= TOS; DROP1; }
void fEmit() { printChar((char)pop()); }
void fDot()  { printBase(pop(), BASE); }
void fNum() {
    push(*(pc-1) - '0');
    while (BTW(*pc, '0', '9')) { TOS = (TOS * 10) + *(pc++) - '0'; }
}
void fCall() {
    if (*(pc + 2) != ';') { rpush(pc - code + 2); }
    pc = CA(GET_WORD(pc));
}
void fRet() { if (rsp > rb) { pc = 0; rsp = rb + 1; } else { pc = CA(rpop()); } }
void fGt() { NOS = (NOS >  TOS) ? 1 : 0; DROP1; }
void fLt() { NOS = (NOS <  TOS) ? 1 : 0; DROP1; }
void fEq() { NOS = (NOS == TOS) ? 1 : 0; DROP1; }
void fLNot() { TOS = (TOS) ? 0 : 1; }
void fIf() { if (pop() == 0) { pc = CA(GET_WORD(pc)); } else { pc += 2; } }
void fIf2() { if (pop() == 0) { while (*pc != ')') { ++pc; } } }
void fInc() { ++TOS; }
void fDec() { --TOS; }
void fExecute() { rpush(pc - code); pc = CA(pop()); }
void fFloat() {
    switch (*(pc++)) {
    case '.': printStringF("%g", fpop()); break;
    case '$': { float x = FTOS; FTOS = FNOS; FNOS = x; } break;
    case 'i': FTOS = (float)TOS; break;
    case 'o': TOS = (CELL)FTOS; break;
    case '+': FNOS += FTOS; DROP1; break;
    case '-': FNOS -= FTOS; DROP1; break;
    case '*': FNOS *= FTOS; DROP1; break;
    case '/': FNOS /= FTOS; DROP1; break;
    case '<': NOS = (FNOS < FTOS) ? 1 : 0; DROP1; break;
    case '>': NOS = (FNOS > FTOS) ? 1 : 0; DROP1; break;
    }
}
void fGoto() { pc = CA(GET_WORD(pc)); }
void fRetOps() {
    ir = *(pc++);
    if (ir == '<') { rpush(pop()); }      // <R
    if (ir == '>') { push(rpop()); }      // R>
    if (ir == '@') { push(stk.i[rsp]); }  // R@
}
void fKey() {
    ir = *(pc++); if (ir == '@') { push(getChar()); }  // K@
    else if (ir == '?') { push(charAvailable()); }     // K?
}
void fStrOps() {
    switch (*(pc++)) {
    case 'e': TOS += strLen(CTOS);             break;  // STR-END
    case 'a': strCat(CTOS, CNOS); DROP2;       break;  // STR-CAT
    case 'c': strCatC(CTOS, (char)NOS); DROP2; break;  // STR-CATC
    case '=': NOS = strEq(CTOS, CNOS); DROP1;  break;  // STR-EQ
    case 'i': NOS = strEqI(CTOS, CNOS); DROP1; break;  // STR-EQI
    case 'l': TOS = (CELL)strLen(CTOS);        break;  // STR-LEN
    case 'r': TOS = (CELL)rTrim(CTOS);         break;  // STR-RTRIM
    case 't': *CTOS = 0;                       break;  // STR-TRUNC
    case 'y': strCpy(CTOS, CNOS); DROP2;       break;  // STR-CPY
    }
}
void fDo() { lsp += 3; L2 = (CELL)pc; L0 = pop(); L1 = pop(); }
void fIndex() { push(L0); }
void fIndex2() { t1 = (2 < lsp) ? lstk[lsp-3] : 0; push(t1); }
void fLeave() { if (3 <= lsp) { lsp -= 3; } }
void fLoop() { if (++L0 < L1) { pc = (byte*)L2; return; } lsp -= 3; }
void fPlusLoop() {
    t1 = L0; L0 += pop();
    if ((t1 < L1) && (L0 < L1)) { pc = (byte*)L2; }
    else if ((t1 > L1) && (L0 > L1)) { pc = (byte*)L2; }
    else { lsp -= 3; }
}
void fBegin() { lsp += 3; L0 = (CELL)pc; }
void fWhile() { if (pop()) { pc = (byte*)L0; } else { lsp -= 3; } }
void fLocAdd() { if ((lb + 10) < LOCALS_SZ) { lb += 10; } }
void fLocRem() { if (lb > 9) { lb -= 10; } }
void fLocIncCell() { locals[lb + *(pc++)-'0'] += CELL_SZ; }
void fLocInc() { ++locals[lb + *(pc++)-'0']; }
void fLocDec() { --locals[lb + *(pc++)-'0']; }
void fLocGet() { push(locals[lb + *(pc++)-'0']); }
void fLocSet() { locals[lb + *(pc++)-'0'] = pop(); }
void fBitOp() {
    switch (*(pc++)) {
    case '~': TOS = ~TOS; return;
    case '&': NOS &= TOS; DROP1; return;
    case '^': NOS ^= TOS; DROP1; return;
    case '|': NOS |= TOS; DROP1; return;
    case 'L': NOS = (NOS << TOS); DROP1; return;
    case 'R': NOS = (NOS >> TOS); DROP1; return;
    default: --pc; printChar(32);
    }
}
void fFileOp() { pc = doFile(ir, pc); }
void fNegate() { TOS = -TOS; }
void fAbs() { if (TOS < 0) fNegate(); }
void fSQuote() { t1 = 0; push((CELL)pc); while (*(pc++)) { ++t1; } push(t1); }
void fZQuote() { fSQuote(); DROP1; }
void fBLit() { push(*(pc++)); }
void fWLit() { push(GET_WORD(pc)); pc += 2; }
void fLit() { push(GET_LONG(pc)); pc += 4; }
void fVarAddr() { t1 = GET_LONG(pc); pc += 4; push((CELL)&vars[t1]); }
void fUser() { pc = doUser(*pc, pc+1); }
void fSystem() { y = (byte*)pop(); system((char*)y); }
void fExt() {
    switch (*(pc++)) {
    case ']': fPlusLoop();                    break;
    case 'S': fDotS();                        break;
    case 'R': push(doRand());                 break;
    case 'A': VHERE += pop(); oVHERE = VHERE; break;
    case 'T': push(doTimer());                break;
    case 'Y': fSystem();                      break;
    case 'D': doWords();                      break;
    case 'W': doSleep();                      break;
    case 'C': fCreate();                      break;
    case 'Z': vmReset();                      break;
    }
}
void X() { ir = *(pc-1);  if (ir) { printStringF("-invIr:%d-", ir); } pc = 0; }
void N() {}

void (*q[128])() = {
    X,fBLit,fWLit,X,fLit,X,X,X,X,X,N,X,X,N,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,                   //   0:31
    N,fStore,fTypeF1,fDup,fSwap,fOver,fSlashMod,fBLit,fIf2,N,fMult,fAdd,fEmit,fSub,fDot,fDiv,     //  32:47
    fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fCall,fRet,fLt,fEq,fGt,fIf,                 //  48:63
    fFetch,X,X,fCharOp,fDec,fExecute,fFloat,fGoto,X,fIndex,fIndex2,fKey,X,fMod,X,fCommaOp,        //  64:79
    fInc,X,fRetOps,fStrOps,fType,X,X,X,X,X,fTypeF2,fDo,fDrop,fLoop,fLeave,fNegate,                //  80:95
    fZQuote,fAbs,fBitOp,fLocIncCell,fLocDec,X,fFileOp,X,X,fLocInc,X,X,fLocAdd,fLocRem,X,X,        //  96:111
    X,X,fLocGet,fLocSet,fTypeQ,fUser,fVarAddr,fWordOp,fExt,X,X,fBegin,fSQuote,fWhile,fLNot,X };   // 112:127

void run(WORD start) {
    pc = CA(start);
    lsp = isError = 0;
    if (sp < sb) { sp = sb - 1; }
    if (rsp > rb) { rsp = rb + 1; }
    while (pc) { q[*(pc++)](); }
}

// ----------------------------------
// The Assembler / Parser
// ----------------------------------

typedef struct {
    const char *name;
    const char *op;
} PRIM_T;

// Words that directly map to VM operations
PRIM_T prims[] = {
    // Stack
    { "DROP", "\\" },       { "DUP", "#" },         { "OVER", "%" },
    { "SWAP", "$" },        { "NIP", "$\\" },       { "1+", "P" },
    { "1-", "D" },          { "2+", "PP" },         { "2*", "#+" },
    { "2/", "2/" },         { "2DUP", "%%" },       { "2DROP", "\\\\" },
    { "ROT", "R<$R>$" },    { "-ROT", "$R<$R>" },   { "TUCK", "$%" },
    // Memory
    { "@", "@"},            { "C@", "C@"},          { "W@", "w@"},
    { "!", "!"},            { "C!", "C!"},          { "W!", "w!"},
    { "+!", "$%@+$!"},
    // Math
    { "+", "+"},            { "-", "-"},            { "/", "/"},
    { "*", "*"},            { "ABS", "a"},          { "/MOD", "&"},
    { "MOD", "M"},         { "NEGATE", "_"},
    // Input/output
    { "(.)", "."},          { ".", ".32,"},         { "CR", "13,10,"},
    { "EMIT", ","},         { "KEY", "K@"},         { "KEY?", "K?"},
    { "QTYPE", "t"},        { "ZTYPE", "Z"},        { "COUNT", "#Sl"},
    { "TYPE", "T"},         { "SPACE", "32,"},      { "SPACES", "0[32,]"},
    // Logical / flow control
    { ".IF", "("},          { ".THEN", ")"},        { "EXIT", ";"},
    { "DO", "["},           { "LOOP", "]"},         { "+LOOP", "x]"},
    { "I", "I"},            { "J", "J"},            { "UNLOOP", "^"},
    { "BEGIN", "{"},        { "WHILE", "}"},        { "UNTIL", "~}"},
    { "AGAIN", "1}"},       { "TRUE", "1"},         { "FALSE", "0"},
    { "=", "="},            { "<", "<"},            { ">", ">"},
    { "<=", ">~"},          { ">=", "<~"},          { "<>", "=~"},
    { "!=", "=~"},          { "0=", "~"},           { "NOT", "~"},
    // String
    { "STR-LEN", "Sl"},     { "STR-END", "Se"},     { "STR-CAT", "Sa"},
    { "STR-CATC", "Sc"},    { "STR-CPY", "Sy"},     { "STR-EQ", "S="},
    { "STR-EQI", "Si"},     { "STR-TRUNC", "St"},   { "STR-RTRIM", "Sr"},
    // Binary/bitwise
    { "AND", "b&"},         { "OR", "b|"},          { "XOR", "b^"},
    { "INVERT", "b~"},      { "LSHIFT", "bL"},      { "RSHIFT", "bR"},
    // Float
    { "I>F", "Fi"},         { "F>I", "Fo"},         { "F+", "F+"},
    { "F-", "F-"},          { "F*", "F*"},          { "F/", "F/"},
    { "F<", "F<"},          { "F>", "F>"},          { "F.", "F."},
    // System
    { "ALLOT", "xA"},       { "BL", "32"},          { "BYE", "uQ"},
    { "C,", "O1"},          { "W,", "O2"},          { ",", "O4"},           
    { "MAX", "%%<($)\\"},   { "MIN", "%%>($)\\"},   { "EXECUTE", "E"},
    { ">R", "R<"},          { "R>", "R>"},          { "R@", "R@"},
    { "RAND", "xR"},        { "RESET", "xZ"},       { ".S", "xS"},
    { "TIMER", "xT"},       { "MS", "xW"},          { "CREATE", "xC"},
    { "+TMPS", "l"},        { "-TMPS", "m"},
    { "SYSTEM", "xY"},      { "WORDS", "xD"},
#ifdef __FILES__
    // Extension: FILE operations
    { "FOPEN-R", "1fO"},    { "FOPEN-W", "2fO"},    { "FOPEN-RW", "3fO"},
    { "FGETC", "fR"},       { "FREAD", "fr"},       { "FGETS", "fG"},
    { "FPUTC", "fW"},       { "FWRITE", "fw"},      { "FCLOSE", "fC"},
    { "FSEEK", "fE"},       { "FTELL", "fT"},       { "FDELETE", "fD"},
    { "FLIST", "fI"},       { "STDIN", "fi"},       { "STDOUT", "fo"},
    { "SAVE-SYS", "fS"},    { "LOAD-SYS", "fs"},    { "SLOAD", "fL"},
#endif
#ifdef __PIN__
    // Extension: PIN operations ... for dev boards
    { "pin-in","uPI"},      { "pin-out","uPO"},     { "pin-up","uPU"},
    { "pin!","uPWD"},       { "pin@","uPRD"},
    { "pina!","uPWA"},      { "pina@","uPRA"},
#endif
#ifdef __EDITOR__
    // Extension: A simple block editor
    { "EDIT","uE"},         // |EDIT|zE|(n--)|Edit block n|
#endif
#ifdef __GAMEPAD__
    // Extension: GAMEPAD operations
    { "gp-button","xGB"},
#endif
    {0,0}
};

void doExec(int isCompiling) {
    if (isCompiling) {
        oHERE = HERE;
        oVHERE = VHERE;
    }
    else {
        CComma(0);
        run((WORD)oHERE);
        HERE = oHERE;
        VHERE = oVHERE;
    }
}

int doFind(const char *name) {
    // Temporary word?
    if (isTempWord(name) && (tempWords[name[1]-'0'])) {
        push(tempWords[name[1]-'0']);
        push(0);
        return 1;
    }

    // Regular lookup
    DICT_E *dp = doFindInternal(name);
    if (dp) {
        push(dp->xt);
        push(dp->flags);
        return 1;
    }
    return 0;
}

void doWords() {
    int n = 0;
    DICT_E *dp = (DICT_E*)&st.bytes[LAST];
    while (dp < dictEnd) {
        if (dp->lexicon == lexicon) {
            printString(dp->name);
            if ((++n) % 10 == 0) { printString("\r\n"); }
            else { printChar(9); }
        }
        dp++;
    }
    if (lexicon) { return; }
    PRIM_T *x = prims;
    while (x->name) {
        printString(x->name);
        printChar(9);
        ++x;
        if ((++n) % 10 == 0) { printString("\r\n"); }
    }
}

int doLiteral(int t) {
    CELL num = pop();
    if (t == 'v')                 { CComma('v'); Comma(num);  }
    else if (BTW(num, 0, 0xFF))   { CComma(1);   CComma(num); }
    else if (BTW(num, 0, 0xFFFF)) { CComma(2);   WComma(num); }
    else                          { CComma(4);   Comma(num);  }
    return 1;
}

char *sprintF(char *dst, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(dst, 256, fmt, args);
    va_end(args);
    return dst;
}

int isDecimal(const char *wd) {
    CELL x = 0, isNeg = 0;
    if (*wd == '-') { isNeg = 1;  ++wd; }
    if (!BTW(*wd,'0','9')) { return 0; }
    while (BTW(*wd, '0', '9')) { x=(x*10)+(*(wd++)-'0'); }
    if (*wd && (*wd != '.')) { return 0; }
    x = isNeg ? -x : x;
    if (*wd == 0) { push(x); return 1; }
    // Must be a '.', make it a float
    ++wd;
    float fx = (float)x, d = 10;
    while (BTW(*wd, '0', '9')) { fx += (*(wd++) - '0') / d; d *= 10; }
    if (*wd) { return 0; }
    push(0);
    FTOS = fx;
    return 1;
}


int isNum(const char *wd) {
    if ((wd[0]=='\'') && (wd[2]=='\'') && (wd[3] == 0)) { push(wd[1]); return 1; }
    CELL x = 0;
    int base = BASE, lastCh = '9';
    if (*wd == '#') { base = 10;  ++wd; }
    if (*wd == '$') { base = 16;  ++wd; }
    if (*wd == '%') { base = 2;  ++wd; lastCh = '1'; }
    if (base < 10) { lastCh = '0' + base - 1; }
    if (base == 10) { return isDecimal(wd); }
    if (*wd == 0) { return 0; }
    while (*wd) {
        char c = *(wd++);
        int t = -1;
        if (BTW(c, '0', lastCh)) { t = c - '0'; }
        if ((base == 16) && (BTW(c, 'A', 'F'))) { t = c - 'A' + 10; }
        if ((base == 16) && (BTW(c, 'a', 'f'))) { t = c - 'a' + 10; }
        if (t < 0) { return 0; }
        x = (x * base) + t;
    }
    push(x);
    return 1;
}

char *isRegOp(const char *wd) {
    if (!BTW(wd[1], '0', '9') || (wd[2])) { return 0; }
    if (BTW(wd[0], 'r', 's')) { return (char*)wd; }
    if (BTW(wd[0], 'c', 'd')) { return (char*)wd; }
    if (wd[0] == 'i') { return (char*)wd; }
    return 0;
}

int doPrim(const char *wd) {
    // Words c4 can map directly into its VML (Virtual Machine Language)
    const char *vml = isRegOp(wd);

    for (int i = 0; prims[i].op && (!vml); i++) {
        if (strEqI(prims[i].name, wd)) { vml = prims[i].op; }
    }

    if (!vml) { return 0; } // Not found

    if (BTW(vml[0],'0','9') && BTW(code[HERE-1],'0','9')) { CComma(' '); }
    for (int j = 0; vml[j]; j++) { CComma(vml[j]); }
    return 1;
}

int doQuote(char op) {
    in++;
    CComma(op);
    while (*in && (*in != '"')) { CComma(*(in++)); }
    CComma(0);
    if (*in) { ++in; }
    return 1;
}

int doDotQuote() {
    ++in;
    CComma('"');
    while (*in && (*in != '"')) { CComma(*(in++)); }
    CComma('"');
    if (*in) { ++in; }
    return 1;
}

int doWord() {
    CELL flg = pop();
    CELL xt = pop();
    if (flg & BIT_IMMEDIATE) {
        doExec(STATE);
        run((WORD)xt);
    } else {
        CComma(':');
        WComma((WORD)xt);
    }
    return 1;
}

int QState(int sb) {
    if (STATE != sb) {
        if (STATE) { printString("-comp-"); }
        else { printString("-noComp-"); }
    }
    return (STATE == sb) ? 0 : 1;
}

int doParseWord() {
    char *wd = (char*)pop();
    if (strEq(word, "//"))   { doExec(STATE); return 0; }
    if (strEq(word, "\\"))   { doExec(STATE); return 0; }
    if (isNum(wd))           { return doLiteral(0); }
    if (doPrim(wd))          { return 1; }
    if (doFind(wd))          { return doWord(); }
    if (strEq(wd, ".\""))    { return doDotQuote(); }
    if (strEq(wd, "\""))     { return doQuote('`'); }
    if (strEqI(wd, "S\""))   { return doQuote('|'); }
    if (strEqI(wd, "LOAD"))  { if (getWord(wd)) { fLoad(wd); } return 0; }

    if (strEq(wd, ":")) {
        if (QState(0)) { return 0; }
        doExec(STATE);
        fCreate();
        if (pop()==0) { return 0; }
        STATE = 1;
        return 1;
    }

    if (strEq(wd, ";")) {
        if (QState(1)) { return 0; }
        CComma(';');
        doExec(STATE);
        STATE = 0;
        return 1;
    }

    if (strEq(wd, "(")) {
        while (*in && (*in != ')')) { ++in; }
        if (*in == ')') { ++in; }
        return 1;
    }

    if (strEqI(wd, "IF")) {
        CComma('?');
        push(HERE);
        WComma(0);
        return 1;
    }

    if (strEqI(wd, "THEN")) {
        CELL tgt = pop();
        SET_WORD(CA(tgt), (WORD)HERE);
        return 1;
    }

    if (strEqI(wd, "VARIABLE")) {
        if (QState(0)) { return 0; }
        doExec(STATE);
        fCreate();
        if (pop() == 0) { return 0; }
        push((CELL)VHERE);
        doLiteral('v');
        CComma(';');
        doExec(1);
        VHERE += CELL_SZ;
        oVHERE = VHERE;
        return 1;
    }

    if (strEqI(wd, "CONSTANT")) {
        if (QState(0)) { return 0; }
        doExec(STATE);
        fCreate();
        if (pop() == 0) { return 0; }
        doLiteral(0);
        CComma(';');
        doExec(1);
        return 1;
    }

    if (strEqI(wd, "'")) {
        doExec(STATE);
        if (getWord(wd) == 0) { return 0; }
        push(doFind(wd));
        return 1;
    }

    printStringF("[%s]??", wd);
    if (STATE == 1) { STATE = 0; LAST += DICT_SZ; }
    HERE = oHERE;
    VHERE = oVHERE;
    code[HERE] = 0;
    return 0;
}

void doParse(const char *line) {
    in = (char*)line;
    while (getWord(word)) {
        push((CELL)word);
        if (doParseWord() == 0) { return; }
    }
    doExec(STATE);
}

void doOK() {
    if (STATE) { printString(" ... "); return; }
    printString("\r\nc4:");
    fDotS();
    printString("> ");
}

void systemWords() {
    BASE = 10;
    char *cp = (char*)(&vars[10]);
    sprintF(cp, ": code-sz %d ;",  CODE_SZ);         doParse(cp);
    sprintF(cp, ": dict-sz %d ;",  DICT_SZ);         doParse(cp);
    sprintF(cp, ": mem-sz %d ;",   MEM_SZ);          doParse(cp);
    sprintF(cp, ": vars-sz %d ;",  VARS_SZ);         doParse(cp);
    sprintF(cp, ": mem %lu ;",     (UCELL)&MEM[0]);  doParse(cp);
    sprintF(cp, ": cb %lu ;",      (UCELL)code);     doParse(cp);
    sprintF(cp, ": vb %lu ;",      (UCELL)vars);     doParse(cp);
    sprintF(cp, ": (here) %lu ;",  (UCELL)&HERE);    doParse(cp);
    sprintF(cp, ": (ohere) %lu ;", (UCELL)&oHERE);   doParse(cp);
    sprintF(cp, ": (last) %lu ;",  (UCELL)&LAST);    doParse(cp);
    sprintF(cp, ": (vhere) %lu ;", (UCELL)&VHERE);   doParse(cp);
    sprintF(cp, ": (lex) %lu ;",   (UCELL)&lexicon); doParse(cp);
    sprintF(cp, ": base %lu ;",    (UCELL)&BASE);    doParse(cp);
    sprintF(cp, ": >in %lu ;",     (UCELL)&in);      doParse(cp);
    sprintF(cp, ": CELL %lu ;",    (UCELL)CELL_SZ);  doParse(cp);
}

#if __BOARD__ == PC
#include "pc.inc"
#endif
