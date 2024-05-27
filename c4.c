#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define CODE_SZ       0xFFFF
#define VARS_SZ       0xFFFF
#define DICT_SZ       0xFFFF
#define LASTPRIM         BYE
#define STK_SZ            64
#define btwi(n,l,h)   ((l<=n) && (n<=h))
#define NCASE         goto next; case
#define BCASE         break; case

#define here          code[0]
#define last          code[1]
#define vhere         code[2]
#define base          code[3]
#define state         code[4]
#define lex           code[5]
#define TOS           stk[sp].i
#define NOS           stk[sp-1].i
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]

#if __LONG_MAX__ > __INT32_MAX__
#define CELL_SZ   8
#define FLT_T     double
#define addrFmt ": %s $%llx ;"
#else
#define CELL_SZ   4
#define FLT_T     float
#define addrFmt ": %s $%lx ;"
#endif

typedef long cell;
typedef unsigned long ucell;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef union { FLT_T f; cell i; } SE_T;
typedef struct { ushort xt; byte sz, fl, lx, ln; char nm[32]; } DE_T;

SE_T stk[STK_SZ];
byte dict[DICT_SZ+1];
byte vars[VARS_SZ];
cell aStk[STK_SZ];
short sp, rsp, lsp, aSp;
cell lstk[60], rstk[STK_SZ];
char tib[128], wd[32], * toIn;
ushort code[CODE_SZ+1];

typedef struct { short op; const char *name; byte imm; } PRIM_T;

#define PRIMS \
    X(EXIT,   "EXIT",    0) \
    X(DUP,    "DUP",     0) \
    X(SWAP,   "SWAP",    0) \
    X(DROP,   "DROP",    0) \
    X(OVER,   "OVER",    0) \
    X(FET,    "@",       0) \
    X(CFET,   "C@",      0) \
    X(WFET,   "W@",      0) \
    X(STO,    "!",       0) \
    X(CSTO,   "C!",      0) \
    X(WSTO,   "W!",      0) \
    X(ADD,    "+",       0) \
    X(SUB,    "-",       0) \
    X(MUL,    "*",       0) \
    X(DIV,    "/",       0) \
    X(SLMOD,  "/MOD",    0) \
    X(INC,    "1+",      0) \
    X(DEC,    "1-",      0) \
    X(LT,     "<",       0) \
    X(EQ,     "=",       0) \
    X(GT,     ">",       0) \
    X(EQ0,    "0=",      0) \
    X(AND,    "AND",     0) \
    X(OR,     "OR",      0) \
    X(XOR,    "XOR",     0) \
    X(COM,    "COM",     0) \
    X(DO,     "DO",      0) \
    X(INDEX,  "I",       0) \
    X(LOOP,   "LOOP",    0) \
    X(ANEW,   "+A",      0) \
    X(AGET,   "A",       0) \
    X(ASET,   ">A",      0) \
    X(AFREE,  "-A",      0) \
    X(TOR,    ">R",      0) \
    X(RAT,    "R@",      0) \
    X(RFROM,  "R>",      0) \
    X(EMIT,   "EMIT",    0) \
    X(DOT,    "(.)",     0) \
    X(COLON,  ":",       1) \
    X(SEMI,   ";",       1) \
    X(IMM,  "IMMEDIATE", 1) \
    X(CREATE, "CREATE",  1) \
    X(COMMA,  ",",       0) \
    X(CLK,    "TIMER",   0) \
    X(SEE,    "SEE",     1) \
    X(COUNT,  "COUNT",   0) \
    X(TYPE,   "TYPE",    0) \
    X(QUOTE,  "\"",      1) \
    X(DOTQT,  ".\"",     1) \
    X(TOCODE, ">CODE",   0) \
    X(TOVARS, ">VARS",   0) \
    X(TODICT, ">DICT",   0) \
    X(RAND,   "RAND",    0) \
    X(BYE,    "BYE",     0)

#define X(op, name, imm) op,

enum _PRIM  {
    STOP, LIT1, LIT2,
    JMP, JMPZ, JMPNZ,
    PRIMS
};

#undef X
#define X(op, name, imm) { op, name, imm },

PRIM_T prims[] = {
    PRIMS
    {0, 0, 0}
};

void sys_load();
void push(cell x) { stk[++sp].i = x; }
cell pop() { return (0<sp) ? stk[sp--].i : 0; }
void rpush(cell x) { rstk[++rsp] = x; }
cell rpop() { return (0<rsp) ? rstk[rsp--] : 0; }
void storeCell(cell a, cell val) { *(cell*)(a) = val; }
void storeWord(cell a, cell val) { *(ushort*)(a) = (ushort)val; }
cell fetchCell(cell a) { return *(cell*)(a); }
cell fetchWord(cell a) { return *(ushort*)(a); }
int lower(char c) { return btwi(c, 'A', 'Z') ? c + 32 : c; }
int strLen(const char *s) { int l = 0; while (s[l]) { l++; } return l; }

int strEqI(const char *s, const char *d) {
    while (lower(*s) == lower(*d)) { if (*s == 0) { return 1; } s++; d++; }
    return 0;
}

void strCpy(char *d, const char *s) {
    while (*s) { *(d++) = *(s++); }
    *(d) = 0;
}

void comma(x) { code[here++] = x; }
void commaCell(cell n) {
    storeCell((cell)&code[here], n);
    here += sizeof(cell) / 2;
}

int nextWord() {
    int l = 0;
    while (btwi(*toIn, 1, 32)) { ++toIn; }
    while (btwi(*toIn, 33, 126)) { wd[l++] = *(toIn++); }
    wd[l] = 0;
    return l;
}

DE_T *addWord(const char *w) {
    if (!w) { nextWord(); w = wd; }
    int ln = strLen(w);
    int sz = ln + 7;
    if (sz & 1) { ++sz; }
    ushort newLast = last - sz;
    DE_T *dp = (DE_T*)&dict[newLast];
    dp->sz = sz;
    dp->xt = here;
    dp->fl = 0;
    dp->lx = (byte)lex;
    dp->ln = ln;
    strCpy(dp->nm, w);
    last = newLast;
    // printf("\n-add:%d,[%s],%d (%d)-", last, dp->nm, here, dp->xt);
    return dp;
}

DE_T *findWord(const char *w) {
    if (!w) { nextWord(); w = wd; }
    int len = strLen(w);
    int cw = last;
    while (cw < DICT_SZ) {
        DE_T *dp = (DE_T*)&dict[cw];
        // printf("-%d,(%s)-", cw, dp->nm);
        if ((len == dp->ln) && strEqI(dp->nm, w)) { return dp; }
        cw += dp->sz;
    }
    return (DE_T*)0;
}

int findXT(int xt) {
    int cw = last;
    while (cw < DICT_SZ) {
        DE_T *dp = (DE_T*)&dict[cw];
        // printf("-%d,(%s)-", cw, dp->nm);
        if (dp->xt == xt) { return cw; }
        cw += dp->sz;
    }
    return 0;
}

int findPrevXT(int xt) {
    int prevXT = here;
    int cw = last;
    while (cw < DICT_SZ) {
        DE_T *dp = (DE_T*)&dict[cw];
        if (dp->xt == xt) { return prevXT; }
        prevXT = dp->xt;
        cw += dp->sz;
    }
    return here;
}

void doSee() {
    DE_T *dp = findWord(0);
    if (!dp) { printf("-nf:%s-", wd); return; }
    if (dp->xt < LASTPRIM) { printf("%s is a primitive (%hX).\n", wd, dp->xt); return; }
    cell x = (cell)dp-(cell)dict;
    int stop = findPrevXT(dp->xt)-1;
    int i = dp->xt;
    printf("\n%04lX: %s (%04hX to %04X)", x, dp->nm, dp->xt, stop);
    while (i <= stop) {
        int op = code[i++];
        x = code[i];
        printf("\n%04X: %04X\t", i-1, op);
        switch (op) {
            case  LIT1: printf("LIT1 %ld (%lX)", x, x); i++;
            BCASE LIT2: x = fetchCell((cell)&code[i]);
                printf("LIT2 %ld (%lX)", x, x);
                i += CELL_SZ / 2;
            BCASE JMP:   printf("JMP %04lX", x);   i++;
            BCASE JMPZ:  printf("JMPZ %04lX (IF)", x);     i++;
            BCASE JMPNZ: printf("JMPNZ %04lX (WHILE)", x); i++; break;
            default: x = findXT(op); 
                printf("%s", x ? ((DE_T*)&dict[(ushort)x])->nm : "??");
        }
    }
}

char *iToA(cell N, int b) {
    static char buf[65];
    ucell X = (ucell)N;
    int isNeg = 0;
    if (b == 0) { b = (int)base; }
    if ((b == 10) && (N < 0)) { isNeg = 1; X = -N; }
    char c, *cp = &buf[64];
    *(cp) = 0;
    do {
        c = (char)(X % b) + '0';
        X /= b;
        c = (c > '9') ? c+7 : c;
        *(--cp) = c;
    } while (X);
    if (isNeg) { *(--cp) = '-'; }
    return cp;
}

void quote() {
    comma(LIT2);
    commaCell((cell)&vars[vhere]);
    ushort start = vhere;
    vars[vhere++] = 0;
    if (*toIn) { ++toIn; }
    while (*toIn) {
        if (*toIn == '"') { ++toIn; break; }
        vars[vhere++] = *(toIn++);
        ++vars[start];
    }
}

void dotQuote() {
    quote(); comma(COUNT); comma(TYPE);
}

void doRand() {
    static cell sd = 0;
    if (!sd) { sd = (cell)code * clock(); }
    sd = (sd << 13) ^ sd;
    sd = (sd >> 17) ^ sd;
    sd = (sd <<  5) ^ sd;
    push(sd);
}

void Exec(int start) {
    cell t, n;
    ushort pc = start, wc;
    next:
    wc = code[pc++];
    switch(wc) {
        case  STOP:   return;
        NCASE LIT1:   push(code[pc++]);
        NCASE LIT2:   push(fetchCell((cell)&code[pc])); pc += CELL_SZ/2;
        NCASE EXIT:   if (0<rsp) { pc = (ushort)rpop(); } else { return; }
        NCASE DUP:    t=TOS; push(t);
        NCASE SWAP:   t=TOS; TOS=NOS; NOS=t;
        NCASE DROP:   pop();
        NCASE OVER:   t = NOS; push(t);
        NCASE DO:     lsp+=3; L2=pc; L0=pop(); L1=pop();
        NCASE INDEX:  push(L0);
        NCASE LOOP:   if (++L0<L1) { pc=(ushort)L2; } else { lsp=(lsp<3) ? 0 : lsp-3; }
        NCASE AND:    t = pop(); TOS &= t;
        NCASE OR:     t = pop(); TOS |= t;
        NCASE XOR:    t = pop(); TOS ^= t;
        NCASE COM:    TOS = ~TOS;
        NCASE ADD:    t = pop(); TOS += t;
        NCASE SUB:    t = pop(); TOS -= t;
        NCASE MUL:    t = pop(); TOS *= t;
        NCASE DIV:    t = pop(); TOS /= t;
        NCASE SLMOD:  t = TOS; n = NOS; TOS = n/t; NOS = n%t;
        NCASE INC:    ++TOS;
        NCASE DEC:    --TOS;
        NCASE LT:     t = pop(); TOS = (TOS < t);
        NCASE EQ:     t = pop(); TOS = (TOS == t);
        NCASE GT:     t = pop(); TOS = (TOS > t);
        NCASE EQ0:    TOS = (TOS == 0) ? 1 : 0;
        NCASE CLK:    push(clock());
        NCASE JMP:    pc=code[pc];
        NCASE JMPZ:   if (pop()==0) { pc=code[pc]; } else { ++pc; }
        NCASE JMPNZ:  if (pop()) { pc=code[pc]; } else { ++pc; }
        NCASE EMIT:   printf("%c", (char)pop());
        NCASE DOT:    t=pop(); printf("%s", iToA(t, base));
        NCASE FET:    TOS = fetchCell(TOS);
        NCASE CFET:   TOS = *(byte *)TOS;
        NCASE WFET:   TOS = fetchWord(TOS);
        NCASE STO:    t=pop(); n=pop(); storeCell(t, n);
        NCASE CSTO:   t=pop(); n=pop(); *(byte*)t=(byte)n;
        NCASE WSTO:   t=pop(); n=pop(); storeWord(t, n);
        NCASE TOR:    rpush(pop());
        NCASE RAT:    push(rstk[rsp]);
        NCASE RFROM:  push(rpop());
        NCASE COLON:  addWord(0); state = 1;
        NCASE SEMI:   comma(EXIT); state = 0;
        NCASE IMM:    { DE_T *dp = (DE_T*)&dict[last]; dp->fl=1; }
        NCASE COMMA:  comma((ushort)pop());
        NCASE CREATE: addWord(0); comma(LIT2); commaCell(vhere+(cell)vars);
        NCASE ANEW:   aStk[++aSp] = pop();
        NCASE ASET:   aStk[aSp] = pop();
        NCASE AGET:   push(aStk[aSp]);
        NCASE AFREE:  if (0 < aSp) --aSp;
        NCASE SEE:    doSee();
        NCASE COUNT:  t=pop(); push(t+1); push(*(byte *)t);
        NCASE TYPE:   t=pop(); n=pop(); for (int i = 0; i<t; i++) printf("%c", ((char *)n)[i]);
        NCASE QUOTE:  quote();
        NCASE DOTQT:  quote(); comma(COUNT); comma(TYPE);
        NCASE TOCODE:  TOS += (cell)code;
        NCASE TOVARS:  TOS += (cell)vars;
        NCASE TODICT:  TOS += (cell)dict;
        NCASE RAND:    doRand();
        NCASE BYE:     exit(0);
        default:      if (code[pc] != EXIT) { rpush(pc); } pc = wc;
            goto next;
    }
}

int isNum(const char *w, int b) {
    cell n=0, isNeg=0;
    if ((w[0]==39) && (w[2]==39) && (w[3]==0)) { push(w[1]); return 1; }
    if (w[0]=='%') { b= 2; ++w; }
    if (w[0]=='#') { b=10; ++w; }
    if (w[0]=='$') { b=16; ++w; }
    if ((b==10) && (w[0]=='-')) { isNeg=1; ++w; }
    if (w[0]==0) { return 0; }
    char c = lower(*(w++));
    while (c) {
        n = (n*b);
        if (btwi(c,'0','9') &&  btwi(c,'0','0'+b-1)) { n+=(c-'0'); }
        else if (btwi(c,'a','a'+b-11)) { n+=(c-'a'+10); }
        else return 0;
        c = lower(*(w++));
    }
    if (isNeg) { n = -n; }
    push(n);
    return 1;
}

int parseWord(char *w) {
    if (!w) { w = &wd[0]; }
    // printf("-pw:%s-",w);

    if (isNum(w, base)) {
        long n = pop();
        if (btwi(n, 0, 0xffff)) {
            comma(LIT1); comma((ushort)n);
        } else {
            comma(LIT2);
            commaCell(n);
        }
        return 1;
    }

    DE_T *de = findWord(w);
    if (de) {
        if (de->fl == 1) {   // IMMEDIATE
            int h = here+100;
            code[h]   = de->xt;
            code[h+1] = EXIT;
            Exec(h);
        } else {
            comma(de->xt);
        }
        return 1;
     }

    return 0;
}

int parseLine(const char *ln) {
    ushort cH=here, cL=last, cS=state, cV=vhere;
    toIn = (char *)ln;
    // printf("-pl:%s-",ln);
    while (nextWord()) {
        if (!parseWord(wd)) {
            printf("-%s?-", wd);
            here=cH;
            vhere=cV;
            last=cL;
            state=0;
            return 0;
        }
    }
    if ((cL==last) && (cH<here) && (cS==0) && (state==0 )) {
        comma(0);
        here=cH;
        vhere=cV;
        Exec(cH);
    }
    return 1;
}

void parseF(const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 128, fmt, args);
    va_end(args);
    parseLine(buf);
}

void baseSys() {
    for (int i = 0; prims[i].name; i++) {
        DE_T *w = addWord(prims[i].name);
        w->xt = prims[i].op;
        w->fl = prims[i].imm;
        if (prims[i].op == CREATE) { addWord("ADDWORD")->xt = prims[i].op; }
    }

    parseF(": (JMP)     #%d ;", JMP);
    parseF(": (JMPZ)    #%d ;", JMPZ);
    parseF(": (JMPNZ)   #%d ;", JMPNZ);
    parseF(": (LIT1)    #%d ;", LIT1);
    parseF(": (LIT2)    #%d ;", LIT2);
    parseF(": (EXIT)    #%d ;", EXIT);
    // parseF(": CREATE  #%d ;", CREATE);
    parseF(addrFmt, "CODE", &code[0]);
    parseF(addrFmt, "VARS", &vars[0]);
    parseF(addrFmt, "DICT", &dict[0]);
    parseF(addrFmt, ">IN",  &toIn);
    parseF(": CODE-SZ #%d ;", CODE_SZ);
    parseF(": VARS-SZ #%d ;", VARS_SZ);
    parseF(": DICT-SZ #%d ;", DICT_SZ);
    parseF(": CELL    #%d ;", CELL_SZ);
    sys_load();
}

void Init() {
    for (int t=0; t<CODE_SZ; t++) { code[t]=0; }
    sp = rsp = lsp = aSp = state = 0;
    last = DICT_SZ;
    base = 10;
    here = LASTPRIM+1;
    baseSys();
}

// REP - Read/Execute/Print (no Loop)
FILE *REP(FILE *fp) {
    if (!fp) { fp = stdin; }
    if ((fp == stdin) && (state==0)) { printf(" ok\n"); }
    if (fgets(tib, sizeof(tib), fp) == tib) {
        parseLine(tib);
        return fp;
    }
    if (fp == stdin) { exit(0); }
    fclose(fp);
    return NULL;
}

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    if (argc>1) { fp=fopen(argv[1], "rt"); }
    Init();
    while (1) { fp = REP(fp); }; // REPL
    return 0;
}
