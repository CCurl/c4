// c4: A stack-based VM/OS for PCs and development boards

#include "c4.h"

// ----------------------------------
// The Virtual Machine / OS
// ----------------------------------
CELL sp, rsp, lsp, lb, isError, sb, rb, fsp;
CELL BASE, stks[STK_SZ], locals[LOCALS_SZ], lstk[LSTK_SZ+1], seed;
float fstk[FLT_SZ];

byte *code, *vars, mem[MEM_SZ], *y, *in;
DICT_E *dict;
CELL &HERE  = (CELL&)mem[0];
CELL &VHERE = (CELL&)mem[CELL_SZ];
CELL &LAST  = (CELL&)mem[CELL_SZ*2];

void vmReset() {
    lsp = lb = 0, fsp = 0;
    sb = 2, rb = (STK_SZ-2);
    sp = sb - 1, rsp = rb + 1;
    LAST = 0;
    HERE = tHERE = 2;
    VHERE = tVHERE = 0;
    for (int i = 0; i < MEM_SZ; i++) { mem[i] = 0; }
    code = &mem[CELL_SZ*3];
    vars = &mem[(CELL_SZ*3)+CODE_SZ+4];
    dict = (DICT_E*)&mem[(CELL_SZ*3)+CODE_SZ+4+VARS_SZ+4];
    systemWords();
}

void push(CELL v) { stks[++sp] = v; }
CELL pop() { return stks[sp--]; }

void fpush(float v) { fstk[++fsp] = v; }
float fpop() { return fstk[fsp--]; }

void rpush(CELL v) { stks[--rsp] = v; }
CELL rpop() { return stks[rsp++]; }

#ifdef NEEDS_ALIGN
WORD GET_WORD(byte* l) { return *l | (*(l + 1) << 8); }
long GET_LONG(byte* l) { return GET_WORD(l) | GET_WORD(l + 2) << 16; }
void SET_WORD(byte* l, WORD v) { *l = (v & 0xff); *(l + 1) = (byte)(v >> 8); }
void SET_LONG(byte* l, long v) { SET_WORD(l, v & 0xFFFF); SET_WORD(l + 2, (WORD)(v >> 16)); }
#else
WORD GET_WORD(byte* l) { return *(WORD *)l; }
long GET_LONG(byte* l) { return *(long *)l; }
void SET_WORD(byte* l, WORD v) { *(WORD *)l = v; }
void SET_LONG(byte* l, long v) { *(long *)l = v; }
#endif // NEEDS_ALIGN

char Lower(char c) { return BTW(c,'A','Z') ? c|0x20 : c; }

int strLen(const char* str) {
    int l = 0;;
    while (*(str++)) { ++l; }
    return l;
}

int strEq(const char* x, const char* y) {
    while (*x && *y && (*x == *y)) { ++x; ++y; }
    return (*x || *y) ? 0 : 1;
}

int strEqI(const char* x, const char* y) {
    while (*x && *y) {
        if (Lower(*x) != Lower(*y)) { return 0; }
        ++x; ++y;
    }
    return (*x || *y) ? 0 : 1;
}

char* strCpy(char* d, const char* s) {
    char* x = d;
    while (*s) { *(x++) = *(s++); }
    *x = 0;
    return d;
}

char* strCat(char* d, const char* s) {
    char* x = d + strLen(d);
    strCpy(x, s);
    return d;
}

char* strCatC(char* d, char c) {
    char* x = d + strLen(d);
    *x = c;
    *(x + 1) = 0;
    return d;
}

char* rTrim(char* d) {
    char* x = d + strLen(d);
    while ((d <= x) && (*x <= ' ')) { *(x--) = 0; }
    return d;
}

void printStringF(const char* fmt, ...) {
    char* buf = (char*)&vars[VARS_SZ - 100];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 100, fmt, args);
    va_end(args);
    printString(buf);
}

void printBase(CELL num, CELL base) {
    UCELL n = (UCELL) num, isNeg = 0;
    if ((base == 10) && (num < 0)) { isNeg = 1; n = -num; }
    char* cp = (char *)&vars[VARS_SZ];
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
        printBase(stks[d], BASE);
    }
    printString(")");
}

byte *doType(byte *a, int l, int delim) {
    if (l < 0) { l = 0; while (a[l]!=delim) { ++l; } }
    byte* e = a+l;
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

void fWord() {
    byte *wd = AOS;
    while (*in && (*in < 33)) { ++in; }
    int l = 0;
    while (*in && (32 < *in)) {
        *(wd++) = *(in++);
        ++l;
    }
    *wd = 0;
    push(l);
}

byte* doFile(CELL ir, byte* pc) {
    ir = *(pc++);
    if (ir == 'O') { fOpen(); }
    else if (ir == 'D') { fDelete(); }
    else if (ir == 'L') { fList(); }
    else if (ir == 's') { fSave(); }
    else if (ir == 'l') { fLoad(); pc = 0; }
    else if (TOS == 0) { printString("-nofp-"); return pc; }
    else if (ir == 'R') { fGetC(); }
    else if (ir == 'G') { fGetS(); }
    else if (ir == 'W') { fWrite(); }
    else if (ir == 'C') { fClose(); }
    return pc;
}

CELL doRand() {
    if (seed == 0) { seed = doTimer(); }
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed & 0x7FFFFFFF;
}

CELL t1, t2;
byte *pc, ir;

void fStore() { SET_LONG(AOS, NOS); DROP2; }
void fFetch() { TOS = GET_LONG(AOS); }
void fCharOp() {
    ir = *(pc++); if (ir == '@') { TOS = *AOS; }
    else if (ir == '!') { *AOS = (byte)NOS; DROP2; }
}
void fWordOp() {
    ir = *(pc++); if (ir == '@') { TOS = GET_WORD(AOS); }
    else if (ir == '!') { SET_WORD(AOS, (WORD)TOS); DROP2; }
}
void fType() { t1 = pop(); y = (byte*)pop(); while (t1--) printChar(*(y++)); }
void fTypeQ() { printString((char*)pop()); }
void fTypeF1() { pc = doType(pc, -1, '"'); }
void fTypeF2() { doType((byte*)pop(), -1, 0); }
void fDup() { push(TOS); }
void fSwap() { t1 = TOS; TOS = NOS; NOS = t1; }
void fOver() { push(NOS); }
void fDrop() { DROP1; }
void fSlashMod() { t1 = NOS; t2 = TOS; NOS = t1 / t2; TOS = t1 % t2; }
void fAdd()  { t1 = pop(); TOS += t1; }
void fSub()  { t1 = pop(); TOS -= t1; }
void fMult() { t1 = pop(); TOS *= t1; }
void fDiv()  { t1 = pop(); TOS /= t1; }
void fEmit()  { printChar((char)pop()); }
void fDot()  { printBase(pop(), BASE); }
void fNum() {
    push(ir - '0');
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
    ir = *(pc++); if (ir == '.') { printStringF("%g", fpop()); }         // FLOAT ops
    else if (ir == '#') { fpush(FTOS); }
    else if (ir == '$') { float x = FTOS; FTOS = FNOS; FNOS = x; }
    else if (ir == '%') { fpush(FNOS); }
    else if (ir == '\\') { FDROP; }
    else if (ir == 'i') { fpush((float)pop()); }
    else if (ir == 'o') { push((CELL)fpop()); }
    else if (ir == '+') { FNOS += FTOS; FDROP; }
    else if (ir == '-') { FNOS -= FTOS; FDROP; }
    else if (ir == '*') { FNOS *= FTOS; FDROP; }
    else if (ir == '/') { FNOS /= FTOS; FDROP; }
    else if (ir == '<') { push((FNOS < FTOS) ? 1 : 0); FDROP; FDROP; }
    else if (ir == '>') { push((FNOS > FTOS) ? 1 : 0); FDROP; FDROP; }
}
void fGoto() { pc = CA(GET_WORD(pc)); }
void fRetOps() {
    ir = *(pc++);
    if (ir == '<') { rpush(pop()); }     // <R
    if (ir == '>') { push(rpop()); }     // R>
    if (ir == '@') { push(stks[rsp]); }  // R@
}
void fKey() {
    ir = *(pc++); if (ir == '@') { push(getChar()); }  // K@
    else if (ir == '?') { push(charAvailable()); }     // K?
}
void fStrOps() {
    ir = *(pc++); if (ir == 'e') { TOS += strLen(CTOS); }     // STR-END
    else if (ir == 'a') { strCat(CTOS, CNOS); DROP2; }        // STR-CAT
    else if (ir == 'c') { strCatC(CTOS, (char)NOS); DROP2; }  // STR-CATC
    else if (ir == '=') { NOS = strEq(CTOS, CNOS); DROP1; }   // STR-EQ
    else if (ir == 'i') { NOS = strEqI(CTOS, CNOS); DROP1; }  // STR-EQI
    else if (ir == 'l') { TOS = (CELL)strLen(CTOS); }         // STR-LEN
    else if (ir == 'r') { TOS = (CELL)rTrim(CTOS); }          // STR-RTRIM
    else if (ir == 't') { *CTOS = 0; }                        // STR-TRUNC
    else if (ir == 'y') { strCpy(CTOS, CNOS); DROP2; }        // STR-CPY
}
void fDo() { lsp += 3; L2 = (CELL)pc; L0 = pop(); L1 = pop(); }
void fIndex() { push(L0); }
void fIndex2() { t1 = (2 < lsp) ? lsp-3 : 0; push(lstk[t1]); }
void fLeave() { if (3 <= lsp) { lsp -= 3; } }
void fLoop() { ++L0; if (L0 < L1) { pc = (byte*)L2; return; } lsp -= 3; }
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
void fLocIncCell() { t1 = *(pc++) - '0'; if (BTW(t1, 0, 9)) { locals[lb + t1] += CELL_SZ; } }
void fLocInc() { t1 = *(pc++) - '0'; if (BTW(t1, 0, 9)) { ++locals[lb + t1]; } }
void fLocDec() { t1 = *(pc++) - '0'; if (BTW(t1, 0, 9)) { --locals[lb + t1]; } }
void fLocGet() { t1 = *(pc++) - '0'; if (BTW(t1, 0, 9)) { push(locals[lb + t1]); } }
void fLocSet() { t1 = *(pc++) - '0'; if (BTW(t1, 0, 9)) { locals[lb + t1] = pop(); } }
void fBitOp() {
    ir = *(pc++); if (ir == '~') { TOS = ~TOS; }
    else if (ir == '%') { NOS %= TOS; DROP1; }
    else if (ir == '&') { NOS &= TOS; DROP1; }
    else if (ir == '^') { NOS ^= TOS; DROP1; }
    else if (ir == '|') { NOS |= TOS; DROP1; }
    else if (ir == 'L') { NOS = (NOS << TOS); DROP1; }
    else if (ir == 'R') { NOS = (NOS >> TOS); DROP1; }
    else { --pc; printChar(32); }
}
void fFileOp() { pc = doFile(ir, pc); }
void fNegate() { TOS = -TOS; }
void fAbs() { if (TOS < 0) fNegate(); }
void fZQuote() { push((CELL)pc); while (*(pc++)) {} }
void fBLit() { push(*(pc++)); }
void fWLit() { push(GET_WORD(pc)); pc += 2; }
void fLit() { push(GET_LONG(pc)); pc += 4; }
void fVarAddr() { t1 = GET_LONG(pc); pc += 4; push((CELL)&vars[t1]); }
void fUser() { pc = doUser(*pc, pc+1); }
void fExt() {
    ir = *(pc++);
    if (ir == ']') { fPlusLoop(); }
    else if (ir == 'S') { fDotS(); }                            // .S
    else if (ir == 'R') { push(doRand()); }                      // RAND
    else if (ir == 'A') { VHERE += pop(); tVHERE = VHERE; }      // ALLOT
    else if (ir == 'T') { push(doTimer()); }                     // TIMER
    else if (ir == 'Y') { y = (byte*)pop(); system((char*)y); }  // SYSTEM
    else if (ir == 'D') { doWords(); }                           // WORDS
    else if (ir == 'W') { doSleep(); }                           // MS
    else if (ir == 'Z') { vmReset(); }
}
void X() { if (ir) { printStringF("-invIr:%d-", ir); } pc = 0; }
void N() {}

void (*q[128])() = {
    X,fBLit,fWLit,X,fLit,X,X,X,X,X,N,X,X,N,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,                           //   0:31
    N,fStore,fTypeF1,fDup,fSwap,fOver,fSlashMod,fBLit,fIf2,N,fMult,fAdd,fEmit,fSub,fDot,fDiv,             //  32:47
    fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fNum,fCall,fRet,fLt,fEq,fGt,fIf,                         //  48:63
    fFetch,X,X,fCharOp,fDec,fExecute,fFloat,fGoto,X,fIndex,fIndex2,fKey,X,X,X,X,                          //  64:79
    fInc,X,fRetOps,fStrOps,fType,X,X,X,X,X,fTypeF2,fDo,fDrop,fLoop,fLeave,fNegate,                        //  80:95
    fZQuote,fAbs,fBitOp,fCharOp,fLocDec,X,fFileOp,X,X,fLocInc,X,X,fLocAdd,fLocRem,X,X,  //  96:111
    X,X,fLocGet,fLocSet,fTypeQ,fUser,fVarAddr,fWordOp,fExt,X,X,fBegin,X,fWhile,fLNot,X };        // 112:127

void run(WORD start) {
    pc = CA(start);
    lsp = isError = 0;
    if (sp < sb) { sp = sb - 1; }
    if (rsp > rb) { rsp = rb + 1; }
    if (fsp < 0) { fsp = 0; }
    if (FLT_SZ <= fsp) { fsp = (FLT_SZ-1); }
    while (pc) { ir = *(pc++); q[ir](); }
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
    { "DROP", "\\" }
    , { "DUP", "#" }
    , { "OVER", "%" }
    , { "SWAP", "$" }
    , { "NIP", "$\\" }
    , { "1+", "P" }
    , { "1-", "D" }
    , { "2+", "PP" }
    , { "2*", "#+" }
    , { "2/", "2/" }
    , { "2DUP", "%%" }
    , { "2DROP", "\\\\" }
    , { "ROT", "R<$R>$" }
    , { "-ROT", "$R<$R>" }
    , { "TUCK", "$%" }
    // Memory
    , { "@", "@" }
    , { "C@", "C@" }
    , { "W@", "w@" }
    , { "!", "!" }
    , { "C!", "C!" }
    , { "W!", "w!" }
    , { "+!", "$%@+$!" }
    // Math
    , { "+", "+" }
    , { "-", "-" }
    , { "/", "/" }
    , { "*", "*" }
    , { "ABS", "a" }
    , { "/MOD", "&" }
    , { "MOD", "b%" }
    , { "NEGATE", "_" }
    // Input/output
    , { "(.)", "." }
    , { ".", ".32," }
    , { "CR", "13,10," }
    , { "EMIT", "," }
    , { "KEY", "K@" }
    , { "KEY?", "K?" }
    , { "QTYPE", "t" }
    , { "ZTYPE", "Z" }
    , { "COUNT", "#Sl" }
    , { "TYPE", "T" }
    , { "SPACE", "32," }
    , { "SPACES", "0[32,]" }
    // Logical / flow control
    , { ".IF", "(" }
    , { ".THEN", ")" }
    , { "DO", "[" }
    , { "LOOP", "]" }
    , { "+LOOP", "x]" }
    , { "I", "I" }
    , { "J", "J" }
    , { "UNLOOP", "^" }
    , { "BEGIN", "{" }
    , { "WHILE", "}" }
    , { "UNTIL", "~}" }
    , { "AGAIN", "1}" }
    , { "TRUE", "1" }
    , { "FALSE", "0" }
    , { "=", "=" }
    , { "<", "<" }
    , { ">", ">" }
    , { "<=", ">~" }
    , { ">=", "<~" }
    , { "<>", "=~" }
    , { "!=", "=~" }
    , { "0=", "~" }
    , { "EXIT", ";" }
    // String
    , { "STR-LEN", "Sl" }
    , { "STR-END", "Se" }
    , { "STR-CAT", "Sa" }
    , { "STR-CATC", "Sc" }
    , { "STR-CPY", "Sy" }
    , { "STR-EQ", "S=" }
    , { "STR-EQI", "Si" }
    , { "STR-TRUNC", "St" }
    , { "STR-RTRIM", "Sr" }
    // Binary/bitwise
    , { "AND", "b&" }
    , { "OR", "b|" }
    , { "XOR", "b^" }
    , { "INVERT", "b~" }
    , { "LSHIFT", "bL" }
    , { "RSHIFT", "bR" }
    // Float
    , { "I>F", "Fi" }  // In
    , { "F>I", "Fo" }  // Out
    , { "F+", "F+" }  // Add
    , { "F-", "F-" }  // Sub
    , { "F*", "F*" }  // Mult
    , { "F/", "F/" }  // Div
    , { "F<", "F<" }  // LT
    , { "F>", "F>" }  // GT
    , { "F.", "F." }  // PRINT
    , { "FDUP", "F#" }
    , { "FOVER", "F%" }
    , { "FSWAP", "F$" }
    , { "FDROP", "F\\" }
    // System
    , { "ALLOT", "xA" }
    , { "BL", "32" }
    , { "BYE", "uQ" }
    , { "EXECUTE", "E" }
    , { "MAX", "%%<($)\\" }
    , { "MIN", "%%>($)\\" }
    , { "MS", "xW" }
    , { "NOP", " " }
    , { "NOT", "~" }
    , { ">R", "R<" }
    , { "R>", "R>" }
    , { "R@", "R@" }
    , { "RAND", "xR" }
    , { "RESET", "xZ" }
    , { ".S", "xS" }
    , { "SYSTEM", "xY" }
    , { "TIMER", "xT" }
    , { "+TMPS", "l" }
    , { "-TMPS", "m" }
    , { "WORDS", "xD" }
#ifdef __FILES__
    // Extension: FILE operations
    , { "FOPEN", "fO" }
    , { "FGETC", "fR" }
    , { "FGETS", "fG" }
    , { "FPUTC", "fW" }
    , { "FCLOSE", "fC" }
    , { "FDELETE", "fD" }
    , { "FLIST", "fL" }
    , { "FSAVE", "fs" }
    , { "FLOAD", "fl" }
#endif
#ifdef __PIN__
    // Extension: PIN operations ... for dev boards
    , { "pin-in","uPI" }          // open input
    , { "pin-out","uPO" }         // open output
    , { "pin-up","uPU" }          // open input-pullup
    , { "pin!","uPWD" }           // Pin write: digital
    , { "pin@","uPRD" }           // Pin read: digital
    , { "pina!","uPWA" }          // Pin write: analog
    , { "pina@","uPRA" }          // Pin read: analog
#endif
#ifdef __EDITOR__
    // Extension: A simple block editor
    , { "EDIT","uE" }         // |EDIT|zE|(n--)|Edit block n|
#endif
#ifdef __GAMEPAD__
    // Extension: GAMEPAD operations
    , { "gp-button","xGB" }
#endif
    , {0,0}
};

char word[32];
CELL STATE, tHERE, tVHERE, tempWords[10];

void CComma(CELL v) { code[tHERE++] = (byte)v; }
void Comma(CELL v) { SET_LONG(&code[tHERE], v); tHERE += CELL_SZ; }
void WComma(WORD v) { SET_WORD(&code[tHERE], v); tHERE += 2; }

void doExec() {
    if (STATE) {
        HERE = tHERE;
        VHERE = tVHERE;
    }
    else {
        CComma(0);
        run((WORD)HERE);
        tHERE = HERE;
        tVHERE = VHERE;
    }
}

int isTempWord(const char *nm) {
    return ((nm[0] == 'T') && BTW(nm[1], '0', '9') && (nm[2] == 0));
}

void doCreate(const char *name, byte f) {
    doExec();
    if (isTempWord(name)) {
        tempWords[name[1]-'0'] = tHERE;
        STATE = 1;
        return;
    }
    DICT_E *dp = &dict[LAST];
    dp->xt = (USHORT)HERE;
    dp->flags = f;
    strCpy(dp->name, name);
    dp->name[NAME_LEN-1] = 0;
    dp->len = strLen(dp->name);
    STATE = 1;
    ++LAST;
}

int doFindInternal(const char* name) {
    // Regular lookup
    int len = strLen(name);
    for (int i = LAST - 1; i >= 0; i--) {
        DICT_E* dp = &dict[i];
        if ((len == dp->len) && strEq(dp->name, name)) {
            return i;
        }
    }
    return -1;
}

int doFind(const char *name) {
    // Temporary word?
    if (isTempWord(name) && (tempWords[name[1]-'0'])) {
        push(tempWords[name[1]-'0']);
        push(0);
        return 1;
    }

    // Regular lookup
    int i = doFindInternal(name);
    if (0 <= i) {
        push(dict[i].xt);
        push(dict[i].flags);
        return 1;
    }
    return 0;
}

int doSee(const char* wd) {
    int i = doFindInternal(wd);
    if (i<0) { printString("-nf-"); return 1; }
    CELL start = dict[i].xt;
    CELL end = HERE;
    if ((i+1) < LAST) { end = dict[i + 1].xt; }

    printStringF("%s (%d): ", wd, start);
    for (int i = start; i < end; i++) {
        byte c = code[i];
        if (BTW(c, 32, 126)) { printChar(c); }
        else { printStringF("(%d)",c); }
    }
    return 1;
}

void doWords() {
    int n = 0;
    for (int i = LAST-1; i >= 0; i--) {
        DICT_E* dp = &dict[i];
        printString(dp->name);
        if ((++n) % 10 == 0) { printString("\r\n"); }
        else { printChar(9); }
    }
    PRIM_T *x = prims;
    while (x->name) {
        printStringF("%s\t", x->name);
        ++x;
        if ((++n) % 10 == 0) { printString("\r\n"); }
    }
}

int getWord(char *wd) {
    push((CELL)wd);
    fWord();
    int l = pop();
    DROP1;
    return l;
}

int doNumber(int t) {
    CELL num = pop();
    if (t == 'v') {
        CComma('v');
        Comma(num);
    } else if (t == 4) {
        CComma(4);
        Comma(num);
    } else if (BTW(num,33,127)) {
        CComma('\'');
        CComma(num);
    } else if ((num & 0xFF) == num) {
        CComma(1);
        CComma(num);
    } else if ((num & 0xFFFF) == num) {
        CComma(2);
        WComma((WORD)num);
    } else {
        CComma(4);
        Comma(num);
    }
    return 1;
}

char *sprintF(char *dst, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(dst, 256, fmt, args);
    va_end(args);
    return dst;
}

int isNum(const char *wd) {
    CELL x = 0;
    int base = BASE, isNeg = 0, lastCh = '9';
    if ((wd[0]=='\'') && (wd[2]==wd[0]) && (wd[3]==0)) { push(wd[1]); return 1; }
    if (*wd == '#') { base = 10;  ++wd; }
    if (*wd == '$') { base = 16;  ++wd; }
    if (*wd == '%') { base = 2;  ++wd; lastCh = '1'; }
    if (base < 10) { lastCh = '0' + base - 1; }
    if ((*wd == '-') && (base == 10)) { isNeg = 1;  ++wd; }
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
    if (isNeg) { x = -x; }
    push(x);
    return 1;
}

char *isRegOp(const char *wd) {
    if ((wd[0] == 'r') && BTW(wd[1], '0', '9') && (!wd[2])) { return (char*)wd; }
    if ((wd[0] == 's') && BTW(wd[1], '0', '9') && (!wd[2])) { return (char*)wd; }
    if ((wd[0] == 'i') && BTW(wd[1], '0', '9') && (!wd[2])) { return (char*)wd; }
    if ((wd[0] == 'd') && BTW(wd[1], '0', '9') && (!wd[2])) { return (char*)wd; }
    if ((wd[0] == 'c') && BTW(wd[1], '0', '9') && (!wd[2])) { return (char*)wd; }
    return 0;
}

int doPrim(const char *wd) {
    // Words c4 can map directly into its VML (Virtual Machine Language)
    const char *vml = isRegOp(wd);

    for (int i = 0; prims[i].op && (!vml); i++) {
        if (strEqI(prims[i].name, wd)) { vml = prims[i].op; }
    }

    if (!vml) { return 0; } // Not found

    if (BTW(vml[0],'0','9') && BTW(code[tHERE-1],'0','9')) { CComma(' '); }
    for (int j = 0; vml[j]; j++) { CComma(vml[j]); }
    return 1;
}

int doQuote() {
    in++;
    CComma('`');
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
        doExec();
        run((WORD)xt);
    } else {
        CComma(':');
        WComma((WORD)xt);
    }
    return 1;
}

int doParseWord(char *wd) {
    if (strEq(word, "//"))  { doExec(); return 0; }
    if (strEq(word, "\\"))  { doExec(); return 0; }
    if (isNum(wd))          { return doNumber(0); }
    if (doPrim(wd))         { return 1; }
    if (doFind(wd))         { return doWord(); }
    if (strEq(wd, ".\""))   { return doDotQuote(); }
    if (strEq(wd, "\""))    { return doQuote(); }
    if (strEqI(wd, "load")) { return doLoad(); }

    if (strEq(wd, ":")) {
        doExec();
        if (getWord(wd) == 0) { return 0; }
        doCreate(wd, 0);
        return 1;
    }

    if (strEq(wd, ";")) {
        CComma(';');
        doExec();
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
        push(tHERE);
        WComma(0);
        return 1;
    }

    if (strEqI(wd, "ELSE")) {
        CELL tgt = pop();
        CComma('G');
        push(tHERE);
        WComma(0);
        SET_WORD(CA(tgt), (WORD)tHERE);
        return 1;
    }

    if (strEqI(wd, "THEN")) {
        CELL tgt = pop();
        SET_WORD(CA(tgt), (WORD)tHERE);
        return 1;
    }

    if (strEqI(wd, "VARIABLE")) {
        if (getWord(wd)) {
            push((CELL)VHERE);
            VHERE += CELL_SZ;
            tVHERE = VHERE;
            doCreate(wd, 0);
            doNumber('v');
            CComma(';');
            doExec();
            STATE = 0;
            return 1;
        }
        else { return 0; }
    }

    if (strEqI(wd, "CONSTANT")) {
        if (getWord(wd)) {
            doCreate(wd, 0);
            doNumber(4);
            CComma(';');
            doExec();
            STATE = 0;
            return 1;
        }
        else { return 0; }
    }

    if (strEqI(wd, "'")) {
        doExec();
        if (getWord(wd) == 0) { return 0; }
        push(doFind(wd));
        return 1;
    }

    if (strEqI(wd, "FORGET")) {
        // Forget the last word
        HERE = tHERE = dict[LAST].xt;
        --LAST;
        return 1;
    }

    if (strEqI(wd, "SEE")) {
        doExec();
        if (getWord(wd) == 0) { return 0; }
        return doSee(wd);
    }

    printStringF("[%s]??", wd);
    if (STATE == 1) { STATE = 0; --LAST; }
    tHERE = HERE;
    tVHERE = VHERE;
    code[tHERE] = 0;
    return 0;
}

bool isASM(const char* ln) {
    if ((ln[0]=='s') && (ln[1]==':') && (ln[2]==' ')) {
        run((byte*)ln-code+3);
        return 1;
    }
    return 0;
}

void doParse(const char *line) {
    in = (byte*)line;
    if (isASM(line)) { return; }
    while (getWord(word)) {
        if (tHERE < HERE) { tHERE = HERE; }
        if (tVHERE < VHERE) { tVHERE = VHERE; }
        if (doParseWord(word) == 0) { return; }
    }
    doExec();
}

void doOK() {
    if (STATE) { printString(" ... "); return; }
    printString("\r\nc4:");
    fDotS();
    printString("> ");
}

void systemWords() {
    BASE = 10;
    char *cp = (char*)(&vars[VARS_SZ-32]);
    sprintF(cp, ": code-sz %d ;",   CODE_SZ);        doParse(cp);
    sprintF(cp, ": dict-sz %d ;",   DICT_SZ);        doParse(cp);
    sprintF(cp, ": mem-sz %d ;",    MEM_SZ);         doParse(cp);
    sprintF(cp, ": vars-sz %d ;",   VARS_SZ);        doParse(cp);
    sprintF(cp, ": mem %lu ;",      (UCELL)&mem[0]); doParse(cp);
    sprintF(cp, ": cb %lu ;",       (UCELL)code);    doParse(cp);
    sprintF(cp, ": db %lu ;",       (UCELL)dict);    doParse(cp);
    sprintF(cp, ": vb %lu ;",       (UCELL)vars);    doParse(cp);
    sprintF(cp, ": (here) %lu ;",   (UCELL)&HERE);   doParse(cp);
    sprintF(cp, ": (last) %lu ;",   (UCELL)&LAST);   doParse(cp);
    sprintF(cp, ": (vhere) %lu ;",  (UCELL)&VHERE);  doParse(cp);
    sprintF(cp, ": base %lu ;",     (UCELL)&BASE);   doParse(cp);
    sprintF(cp, ": >in %lu ;",      (UCELL)&in);     doParse(cp);
    sprintF(cp, ": CELL %lu ;",     (UCELL)CELL_SZ); doParse(cp);
}

#if __BOARD__ == PC
#include "pc.inc"
#endif
