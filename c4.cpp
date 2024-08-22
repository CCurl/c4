#include "c4.h"

#define LASTPRIM      BYE
#define NCASE         goto next; case
#define BCASE         break; case
#define dsp           code[DSPA]
#define rsp           code[RSPA]
#define lsp           code[LSPA]
#define tsp           code[TSPA]
#define here          code[HA]
#define last          code[LA]
#define base          code[BA]
#define state         code[SA]
#define vhere         code[VHA]
#define regBase       code[RBA]
#define frameSz       code[FSZ]
#define TOS           dstk[dsp]
#define NOS           dstk[dsp-1]
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]

enum { DSPA=0, RSPA, LSPA, TSPA, HA, LA, BA, SA, VHA };

cell dstk[STK_SZ+1], lstk[LSTK_SZ + 1], rstk[STK_SZ + 1];
cell code[MAX_CODE+1];
byte vars[MAX_VARS+1];
DE_T dict[MAX_DICT+1];
char source[MAX_SRC+1];
cell cV, cH, cL, cS;
cell inputFp, outputFp;
char wd[32], *toIn;
cell tstk[TSTK_SZ+1];
DE_T tmpWords[10];

#define PRIMS \
	X(EXIT,    "exit",      0, if (0<rsp) { pc = rpop(); } else { return; } ) \
	X(DUP,     "dup",       0, t=TOS; push(t); ) \
	X(SWAP,    "swap",      0, t=TOS; TOS=NOS; NOS=t; ) \
	X(DROP,    "drop",      0, pop(); ) \
	X(OVER,    "over",      0, t=NOS; push(t); ) \
	X(FET,     "@",         0, TOS = fetchCell(TOS); ) \
	X(CFET,    "c@",        0, TOS = *(byte *)TOS; ) \
	X(WFET,    "w@",        0, TOS = fetchWord(TOS); ) \
	X(STO,     "!",         0, t=pop(); n=pop(); storeCell(t, n); ) \
	X(CSTO,    "c!",        0, t=pop(); n=pop(); *(byte*)t=(byte)n; ) \
	X(WSTO,    "w!",        0, t=pop(); n=pop(); storeWord(t, n); ) \
	X(ADD,     "+",         0, t=pop(); TOS += t; ) \
	X(SUB,     "-",         0, t=pop(); TOS -= t; ) \
	X(MUL,     "*",         0, t=pop(); TOS *= t; ) \
	X(DIV,     "/",         0, t=pop(); TOS /= t; ) \
	X(SLMOD,   "/mod",      0, t=TOS; n = NOS; TOS = n/t; NOS = n%t; ) \
	X(INCR,    "1+",        0, ++TOS; ) \
	X(DECR,    "1-",        0, --TOS; ) \
	X(LT,      "<",         0, t=pop(); TOS = (TOS < t); ) \
	X(EQ,      "=",         0, t=pop(); TOS = (TOS == t); ) \
	X(GT,      ">",         0, t=pop(); TOS = (TOS > t); ) \
	X(EQ0,     "0=",        0, TOS = (TOS == 0) ? 1 : 0; ) \
	X(AND,     "and",       0, t=pop(); TOS &= t; ) \
	X(OR,      "or",        0, t=pop(); TOS |= t; ) \
	X(XOR,     "xor",       0, t=pop(); TOS ^= t; ) \
	X(COM,     "com",       0, TOS = ~TOS; ) \
	X(FOR,     "for",       0, lsp+=3; L2=pc; L0=0; L1=pop(); ) \
	X(NDX_I,   "i",         0, push(L0); ) \
	X(NDX_j,   "j",         0, t=(lsp>2) ? lsp-3 : 0; push(lstk[t]); ) \
	X(NDX_K,   "k",         0, t=(lsp>5) ? lsp-6 : 0; push(lstk[t]); ) \
	X(UNLOOP,  "unloop",    0, if (lsp>2) { lsp-=3; } ) \
	X(NEXT,    "next",      0, if (++L0<L1) { pc=L2; } else { lsp=(lsp<3) ? 0 : lsp-3; } ) \
	X(TOR,     ">r",        0, rpush(pop()); ) \
	X(RAT,     "r@",        0, push(rstk[rsp]); ) \
	X(RSTO,    "r!",        0, rstk[rsp] = pop(); ) \
	X(RFROM,   "r>",        0, push(rpop()); ) \
	X(RDROP,   "rdrop",     0, rpop(); ) \
	X(TTO,     ">t",        0, t=pop(); if (tsp < TSTK_SZ) { tstk[++tsp]=t; }; ) \
	X(TAT,     "t@",        0, push(tstk[tsp]); ) \
	X(TSTO,    "t!",        0, tstk[tsp] = pop(); ) \
	X(TFROM,   "t>",        0, push((0 < tsp) ? tstk[tsp--] : 0); ) \
	X(TDROP,   "tdrop",     0, if (0<tsp) { --tsp; } ) \
	X(EMIT,    "emit",      0, emit((char)pop()); ) \
	X(KEY,     "key",       0, push(key()); ) \
	X(QKEY,    "?key",      0, push(qKey()); ) \
	X(COLON,   ":",         1, execIt(); addWord(0); state = 1; ) \
	X(SEMI,    ";",         1, comma(EXIT); state=0; cH=here; cL=last; ) \
	X(IMMED,   "immediate", 1, dict[last].fl=1; ) \
	X(ADDWORD, "addword",   0, execIt(); addWord((char*)123); comma(LIT); comma(vhere); ) \
	X(CLK,     "timer",     0, push(timer()); ) \
	X(ZTYPE,   "ztype",     0, zType((char*)pop()); ) \
	X(SCPY,    "s-cpy",     0, t=pop(); strCpy((char*)TOS, (char*)t); ) \
	X(SEQ,     "s-eq",      0, t=pop(); TOS = strEq((char*)TOS, (char*)t); ) \
	X(SEQI,    "s-eqi",     0, t=pop(); TOS = strEqI((char*)TOS, (char*)t); ) \
	X(SZLEN,   "sz-len",    0, TOS = strLen((char*)TOS); ) \
	X(QUOTE,   "\"",        1, quote(0); ) \
	X(DOTQT,   ".\"",       1, quote(0); comma(ZTYPE); ) \
	X(FIND,    "find",      1, { DE_T *dp=findWord(0); push(dp?dp->xt:0); push((cell)dp); } ) \
    X(SYSTEM,  "system",    0, t=pop(); ttyMode(0); system((char*)t+1); ) \
	X(FLOPEN,  "fopen",     0, t=pop(); n=pop(); push(fileOpen((char*)n, (char*)t)); ) \
	X(FLCLOSE, "fclose",    0, t=pop(); fileClose(t); ) \
	X(FLDEL,   "fdelete",   0, t=pop(); fileDelete((char*)t); ) \
	X(FLREAD,  "fread",     0, t=pop(); n=pop(); TOS = fileRead((char*)TOS, (int)n, t); ) \
	X(FLWRITE, "fwrite",    0, t=pop(); n=pop(); TOS = fileWrite((char*)TOS, (int)n, t); ) \
    X(XDOT, "(.)", 0, printf("[%ld]",pop()); ) \
	X(BYE,     "bye",       0, doBye(); )

#define X(op, name, imm, cod) op,

enum _PRIM  {
	STOP, LIT, JMP, JMPZ, NJMPZ, JMPNZ, NJMPNZ,
	PRIMS
};

#undef X
#define X(op, name, imm, code) { op, name, imm },

PRIM_T prims[] = { PRIMS {0, 0, 0} };

void sys_load();
void push(cell x) { if (dsp < STK_SZ) { dstk[++dsp] = x; } }
cell pop() { return (0<dsp) ? dstk[dsp--] : 0; }
void rpush(cell x) { if (rsp < RSTK_SZ) { rstk[++rsp] = x; } }
cell rpop() { return (0<rsp) ? rstk[rsp--] : 0; }
int lower(const char c) { return btwi(c, 'A', 'Z') ? c + 32 : c; }
int strLen(const char *s) { int l = 0; while (s[l]) { l++; } return l; }

void storeWord(cell a, cell v) { *(ushort*)(a) = (ushort)v; }
cell fetchWord(cell a) { return *(ushort*)(a); }
void storeCell(cell a, cell v) { *(cell*)(a) = v; }
cell fetchCell(cell a) { return *(cell*)(a); }
void doBye() { ttyMode(0); exit(0); }

int strEqI(const char *s, const char *d) {
	while (lower(*s) == lower(*d)) { if (*s == 0) { return 1; } s++; d++; }
	return 0;
}

int strEq(const char *s, const char *d) {
	while (*s == *d) { if (*s == 0) { return 1; } s++; d++; }
	return 0;
}

void strCpy(char *d, const char *s) {
	*(d+1) = 0;  // NULL terminator for empty counted strings
	while (*s) { *(d++) = *(s++); }
	*(d) = 0;
}

void comma(cell x) { code[here++] = x; }

int nextWord() {
	int len = 0;
	while (btwi(*toIn, 1, 32)) { ++toIn; }
	while (btwi(*toIn, 33, 126)) { wd[len++] = *(toIn++); }
	wd[len] = 0;
	return len;
}

DE_T *tempWord(const char *w) {
	if ((w[0]=='t') && btwi(w[1],'0','9') && (w[2]==0)) {
		return &tmpWords[w[1]-'0'];
	}
	return 0;
}

DE_T *addWord(const char *w) {
	if (TOS == 3456) {
		printf("brk");
	}
	if ((w==0) || (w==(char*)123)) { nextWord(); w = wd; }
	DE_T *dp = tempWord(w);
	if (dp) { dp->xt = here; return dp; }
	int ln = strLen(w);
	dp = (DE_T*)&dict[--last];
	dp->xt = here;
	dp->fl = 0;
	dp->ln = ln;
	strCpy(dp->nm, w);
	// printf("\n-add:%d,[%s],%d (%d)-\n", last, dp->nm, dp->xt);
	return dp;
}

DE_T *findWord(const char *w) {
	if (!w) { nextWord(); w = wd; }
	DE_T *dp = tempWord(w);
	if (dp) { return dp; }
	int len = strLen(w);
	cell cw = last;
	while (cw <= MAX_DICT) {
		dp = (DE_T*)&dict[cw];
		// zTypeF("-%d,(%s)-", cw, dp->nm);
		if ((len == dp->ln) && strEqI(dp->nm, w)) { return dp; }
		++cw;
	}
	return (DE_T*)0;
}

void quote(int counted) {
	comma(LIT);
	comma(vhere);
	char *vh = (char*)vhere;
	if (*toIn) { ++toIn; }
	while (*toIn) {
		if (*toIn == '"') { ++toIn; break; }
		*(vh++) = *(toIn++);
	}
	*(vh++) = 0; // NULL terminator
	vhere = (cell)vh;
}

void execIt() {
	// zTypeF("-cH:%d,here:%d-\n",cH, here);
	if (cH < here) {
		comma(0);
		here=cH;
		vhere=cV;
		inner(cH);
	}
}

#undef X
#define X(op, name, imm, code) NCASE op: code

void inner(cell start) {
	cell t, n;
	cell pc = start, wc;
next:
	if (TOS == 334455) {
		printf("break!");
	}
	wc = code[pc++];
	// printf("-wc:%ld-",wc);
	switch(wc) {
		case  STOP:   return;
		NCASE LIT:    push(code[pc++]);
		NCASE JMP:    pc=code[pc];
		NCASE JMPZ : if (pop() == 0) { pc = code[pc]; }
		else { ++pc; }
		NCASE NJMPZ:  if (TOS==0) { pc=code[pc]; } else { ++pc; }
		NCASE JMPNZ:  if (pop()) { pc=code[pc]; } else { ++pc; }
		NCASE NJMPNZ: if (TOS) { pc=code[pc]; } else { ++pc; }
		PRIMS
		default:
			if (wc & VAL_MASK) { push(wc&(VAL_MASK-1)); /*printf("-wc:%ld/%lx/%lx-", dsp, wc, wc & (VAL_MASK - 1));*/  goto next; }
			if (code[pc] != EXIT) { rpush(pc); }
			pc = wc;
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
	// zTypeF("-pw:%s-",w);

	if (isNum(w, base)) {
		cell n = pop();
		if (btwi(n, 0, VAL_MAX)) {
			comma(n | VAL_MASK);
		} else {
			comma(LIT);
			comma(n);
		}
		return 1;
	}

	DE_T *de = findWord(w);
	if (de) {
		if ((de->fl & 0x01) == 0x01) {   // IMMEDIATE
			code[here+100] = de->xt;
			code[here+101] = EXIT;
			inner(here+100);
		} else {
			comma(de->xt);
		}
		return 1;
	}

	return 0;
}

int outer(const char *ln) {
	// zTypeF("-outer:%s-\n",ln);
	cH=here, cL=last, cS=state, cV=vhere;
	toIn = (char *)ln;
	while (nextWord()) {
		if (!parseWord(wd)) {
			zTypeF("-%s?-", wd);
			if (inputFp) { zTypeF(" at\r\n\t%s", ln); }
			here=cH;
			vhere=cV;
			last=cL;
			state=0;
			return 0;
		}
	}
	if ((cL==last) && (cS==0) && (state==0)) execIt();
	return 1;
}

void outerF(const char *fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
    // zType(buf); zType("\r\n");
	outer(buf);
}

void zTypeF(const char *fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
	zType(buf);
}

void baseSys() {
	for (int i = 0; prims[i].name; i++) {
        // zTypeF("[%s]",prims[i].name); if (i%10==0) { zType("\r\n"); }
		DE_T *w = addWord(prims[i].name);
		w->xt = prims[i].op;
		w->fl = prims[i].fl;
	}

	outerF(": version   #%d ;", VERSION);
	outerF(": (jmp)     #%d ;", JMP);
	outerF(": (jmpz)    #%d ;", JMPZ);
	outerF(": (jmpnz)   #%d ;", JMPNZ);
	outerF(": (njmpz)   #%d ;", NJMPZ);
	outerF(": (njmpnz)  #%d ;", NJMPNZ);
	outerF(": (lit)     #%d ;", LIT);
	outerF(": (exit)    #%d ;", EXIT);

	outerF(addrFmt, "(ha)",   &here);
	outerF(addrFmt, "(la)",   &last);
	outerF(addrFmt, "(vha)",  &vhere);
	outerF(addrFmt, "base",   &base);
	outerF(addrFmt, "state ", &state);

	outerF(addrFmt, "(dsp)", &dsp);
	outerF(addrFmt, "(rsp)", &rsp);
	outerF(addrFmt, "(lsp)", &lsp);
	outerF(addrFmt, "(tsp)", &tsp);

	outerF(addrFmt, "code",  &code[0]);
	outerF(addrFmt, "vars",  &vars[0]);
	outerF(addrFmt, "dict",  &dict[0]);
	outerF(addrFmt, ">in",   &toIn);
	outerF(addrFmt, "(output-fp)", &outputFp);
	outerF(addrFmt, "stk",  &dstk[0]);
	outerF(addrFmt, "rstk", &rstk[0]);
	outerF(addrFmt, "tstk", &tstk[0]);

	outerF(": code-sz #%d ;", MAX_CODE+1);
	outerF(": vars-sz #%d ;", MAX_VARS+1);
	outerF(": dict-sz #%d ;", MAX_DICT+1);
	outerF(": stk-sz  #%d ;", STK_SZ+1);
	outerF(": tstk-sz #%d ;", TSTK_SZ+1);
	outerF(": cell    #%d ;", CELL_SZ);
	// sys_load();
}

void Init() {
	for (int t=0; t<=MAX_CODE; t++) { code[t]=0; }
	for (int t=0; t<=MAX_VARS; t++) { vars[t]=0; }
	for (int t=0; t<10; t++) { DE_T *x=&tmpWords[t]; x->fl=0, sprintf(x->nm,"t%d",t); }
	here  = LASTPRIM+1;
	last  = MAX_DICT+1;
	vhere = (cell)&vars[0];
	base = 10;
	baseSys();
}
