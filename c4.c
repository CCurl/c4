#include "c4.h"

#define LASTPRIM      BYE
#define NCASE         goto next; case
#define BCASE         break; case
#define sp            code[SPA]
#define rsp           code[RSPA]
#define here          code[HA]
#define last          code[LA]
#define base          code[BA]
#define state         code[SA]
#define lsp           code[LSPA]
#define tsp           code[TSPA]
#define lex           code[LEXA]
#define TOS           stk[sp].i
#define NOS           stk[sp-1].i
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]

enum { SPA=0, RSPA, HA, LA, BA, SA, LSPA, TSPA, LEXA };

SE_T stk[STK_SZ+1];
ushort code[CODE_SZ+1], cH, cL, cS;
byte dict[DICT_SZ+1], vars[VARS_SZ+1];
cell vhere, A, S, D, cV, lstk[LSTK_SZ], rstk[STK_SZ+1];
char wd[32], *toIn;
cell tstk[TSTK_SZ+1];

#define PRIMS \
	X(EXIT,    "exit",      0, if (0<rsp) { pc = (ushort)rpop(); } else { return; } ) \
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
	X(INC,     "1+",        0, ++TOS; ) \
	X(DEC,     "1-",        0, --TOS; ) \
	X(LT,      "<",         0, t=pop(); TOS = (TOS < t); ) \
	X(EQ,      "=",         0, t=pop(); TOS = (TOS == t); ) \
	X(GT,      ">",         0, t=pop(); TOS = (TOS > t); ) \
	X(EQ0,     "0=",        0, TOS = (TOS == 0) ? 1 : 0; ) \
	X(AND,     "and",       0, t=pop(); TOS &= t; ) \
	X(OR,      "or",        0, t=pop(); TOS |= t; ) \
	X(XOR,     "xor",       0, t=pop(); TOS ^= t; ) \
	X(COM,     "com",       0, TOS = ~TOS; ) \
	X(FOR,     "for",       0, lsp+=3; L2=pc; L0=0; L1=pop(); ) \
	X(INDEX,   "i",         0, push(L0); ) \
	X(NEXT,    "next",      0, if (++L0<L1) { pc=(ushort)L2; } else { lsp=(lsp<3) ? 0 : lsp-3; } ) \
	X(AGET,    "a>",        0, push(A); ) \
	X(ASET,    ">a",        0, A=pop(); ) \
	X(AINC,    "a+",        0, push(A++); ) \
	X(SGET,    "s>",        0, push(S); ) \
	X(SSET,    ">s",        0, S=pop(); ) \
	X(SINC,    "s+",        0, push(S++); ) \
	X(DGET,    "d>",        0, push(D); ) \
	X(DSET,    ">d",        0, D=pop(); ) \
	X(DINC,    "d+",        0, push(D++); ) \
	X(TOR,     ">r",        0, rpush(pop()); ) \
	X(RAT,     "r@",        0, push(rstk[rsp]); ) \
	X(RFROM,   "r>",        0, push(rpop()); ) \
	X(TOT,     ">t",        0, tpush(pop()); ) \
	X(TAT,     "t@",        0, push(tstk[tsp]); ) \
	X(TFROM,   "t>",        0, push(tpop()); ) \
	X(EMIT,    "emit",      0, t=pop(); emit((char)t); ) \
	X(KEY,     "key",       0, push(key()); ) \
	X(QKEY,    "?key",      0, push(qKey()); ) \
	X(COLON,   ":",         1, execIt(); addWord(0); state = 1; ) \
	X(SEMI,    ";",         1, comma(EXIT); state = 0; cH=here; ) \
	X(IMMED,   "immediate", 1, { DE_T *dp = (DE_T*)&dict[last]; dp->fl=1; } ) \
	X(ADDWORD, "addword",   0, execIt(); addWord(0); comma(LIT2); commaCell(vhere+(cell)vars); ) \
	X(CLK,     "timer",     0, push(clock()); ) \
	X(SEE,     "see",       1, doSee(); ) \
	X(COUNT,   "count",     0, t=pop(); push(t+1); push(*(byte *)t); ) \
	X(TYPE,    "type",      0, t=pop(); char *y=(char*)pop(); for (int i = 0; i<t; i++) emit(y[i]); ) \
	X(ZTYPE,   "ztype",     0, zType((char*)pop()); ) \
	X(SCPY,    "s-cpy",     0, t=pop(); strCpy((char*)TOS, (char*)t); ) \
	X(SEQ,     "s-eq",      0, t=pop(); TOS = strEq((char*)TOS, (char*)t); ) \
	X(SEQI,    "s-eqi",     0, t=pop(); TOS = strEqI((char*)TOS, (char*)t); ) \
	X(SZLEN,   "sz-len",    0, TOS = strLen((char*)TOS); ) \
	X(ZQUOTE,  "z\"",       1, quote(0); ) \
	X(QUOTE,   "\"",        1, quote(1); ) \
	X(DOTQT,   ".\"",       1, quote(0); comma(ZTYPE); ) \
	X(RAND,    "rand",      0, doRand(); ) \
	X(FLOPEN,  "fopen",     0, t=pop(); n=pop(); push(fileOpen((char*)n, (char*)t)); ) \
	X(FLCLOSE, "fclose",    0, t=pop(); fileClose(t); ) \
	X(FLREAD,  "fread",     0, t=pop(); n=pop(); TOS = fileRead((char*)TOS, (int)n, t); ) \
	X(FLWRITE, "fwrite",    0, t=pop(); n=pop(); TOS = fileWrite((char*)TOS, (int)n, t); ) \
	X(FLGETS,  "fgets",     0, t=pop(); n=pop(); TOS = fileGets((char*)TOS, (int)n, t); ) \
	X(FLLOAD,  "fload",     0, t=pop(); fileLoad((char*)t); ) \
	X(LOAD,    "load",      0, t=pop(); blockLoad((int)t); ) \
	X(LOADED,  "loaded?",   0, t=pop(); pop(); if (t) { fileClose(inputFp); inputFp=filePop(); } ) \
	X(ITOA,    "to-string", 0, t=pop(); push((cell)iToA(t, base)); ) \
	X(DOTS,    ".s",        0, dotS(); ) \
	X(FETC,    "@c",        0, TOS = code[(ushort)TOS]; ) \
	X(STOC,    "!c",        0, t=pop(); n=pop(); code[(ushort)t] = (ushort)n; ) \
	X(FIND,    "find",      1, { DE_T *dp=findWord(0); push(dp?dp->xt:0); push((cell)dp); } ) \
	X(SYSTEM,  "system",    0, t=pop(); ttyMode(0); system((char*)t+1); ) \
	X(BYE,     "bye",       0, ttyMode(0); exit(0); )

#define X(op, name, imm, cod) op,

enum _PRIM  {
	STOP, LIT1, LIT2, JMP, JMPZ, NJMPZ, JMPNZ, NJMPNZ,
	PRIMS
};

#undef X
#define X(op, name, imm, code) { op, name, imm },

PRIM_T prims[] = { PRIMS {0, 0, 0} };

void sys_load();
void push(cell x) { if (sp < STK_SZ) { stk[++sp].i = x; } }
cell pop() { return (0<sp) ? stk[sp--].i : 0; }
void rpush(cell x) { if (rsp < RSTK_SZ) { rstk[++rsp] = x; } }
cell rpop() { return (0<rsp) ? rstk[rsp--] : 0; }
void tpush(cell x) { if (tsp < TSTK_SZ) { tstk[++tsp] = x; } }
cell tpop() { return (0<tsp) ? tstk[tsp--] : 0; }
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

int strEq(const char *s, const char *d) {
	while (*s == *d) { if (*s == 0) { return 1; } s++; d++; }
	return 0;
}

void strCpy(char *d, const char *s) {
	while (*s) { *(d++) = *(s++); }
	*(d) = 0;
}

void comma(ushort x) { code[here++] = x; }
void commaCell(cell n) {
	storeCell((cell)&code[here], n);
	here += CELL_SZ / 2;
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
	int sz = ln + 7;          // xt + sz + fl + lx + ln + null
	if (sz & 1) { ++sz; }
	ushort newLast=last - sz;
	DE_T *dp = (DE_T*)&dict[newLast];
	dp->sz = sz;
	dp->xt = here;
	dp->fl = 0;
	dp->lx = (byte)lex;
	dp->ln = ln;
	strCpy(dp->nm, w);
	last=newLast;
	// printF("-add:%d,[%s],%d (%d)-\n", newLast, dp->nm, dp->lx, dp->xt);
	return dp;
}

DE_T *findWord(const char *w) {
	if (!w) { nextWord(); w = wd; }
	int len = strLen(w);
	int cw = last;
	while (cw < DICT_SZ) {
		DE_T *dp = (DE_T*)&dict[cw];
		// printF("-%d,(%s)-", cw, dp->nm);
		if ((len == dp->ln) && strEqI(dp->nm, w)) { return dp; }
		cw += dp->sz;
	}
	return (DE_T*)0;
}

int findXT(int xt) {
	int cw = last;
	while (cw < DICT_SZ) {
		DE_T *dp = (DE_T*)&dict[cw];
		// printF("-%d,(%s)-", cw, dp->nm);
		if (dp->xt == xt) { return cw; }
		cw += dp->sz;
	}
	return 0;
}

int findPrevXT(int xt) {
	int prevXT=here;
	int cw = last;
	while (cw < DICT_SZ) {
		DE_T *dp = (DE_T*)&dict[cw];
		if (dp->xt == xt) { return prevXT; }
		prevXT=dp->xt;
		cw += dp->sz;
	}
	return here;
}

void doSee() {
	DE_T *dp = findWord(0);
	if (!dp) { printF("-nf:%s-", wd); return; }
	if (dp->xt <= LASTPRIM) { printF("%s is a> primitive (%hX).\n", wd, dp->xt); return; }
	cell x = (cell)dp-(cell)dict;
	int stop = findPrevXT(dp->xt)-1;
	int i = dp->xt;
	printF("\n%04hX: %s (%04hX to %04X)", (ushort)x, dp->nm, dp->xt, stop);
	while (i <= stop) {
		int op = code[i++];
		x = code[i];
		printF("\n%04X: %04X\t", i-1, op);
		if (op & 0xC000) { printF("lit %d", (int)(op & 0x3FFF)); continue; }
		switch (op) {
			case  STOP: zType("stop"); i++;
			BCASE LIT1: printF("lit1 %hu (%hX)", (ushort)x, (ushort)x); i++;
			BCASE LIT2: x = fetchCell((cell)&code[i]);
				printF("lit2 %zd (%zX)", (size_t)x, (size_t)x);
				i += CELL_SZ / 2;
			BCASE JMP:    printF("jmp %04hX", (ushort)x);             i++;
			BCASE JMPZ:   printF("jmpz %04hX (IF)", (ushort)x);       i++;
			BCASE NJMPZ:  printF("njmpz %04hX (-IF)", (ushort)x);     i++;
			BCASE JMPNZ:  printF("jmpnz %04hX (WHILE)", (ushort)x);   i++; break;
			BCASE NJMPNZ: printF("njmpnz %04hX (-WHILE)", (ushort)x); i++; break;
			default: x = findXT(op); 
				zType(x ? ((DE_T*)&dict[(ushort)x])->nm : "??");
		}
	}
}

char *iToA(cell N, int b) {
	static char buf[65];
	ucell X = (ucell)N;
	int isNeg = 0, len = 0;
	if (b == 0) { b = (int)base; }
	if ((b == 10) && (N < 0)) { isNeg = 1; X = -N; }
	char c, *cp = &buf[64];
	*(cp) = 0;
	do {
		c = (char)(X % b) + '0';
		X /= b;
		c = (c > '9') ? c+7 : c;
		*(--cp) = c;
		++len;
	} while (X);
	if (isNeg) { *(--cp) = '-'; ++len; }
	*(--cp) = len;
	return cp;
}

void dotS() {
    zType("( ");
    for (int i = 1; i <= sp; i++) { printF("%s ", iToA(stk[i].i, base)+1); }
    zType(")");
}

void quote(int counted) {
	comma(LIT2);
	commaCell((cell)&vars[vhere]);
	cell vh=vhere;
	if (counted) { vars[vhere++] = 0; } // Length byte
	if (*toIn) { ++toIn; }
	while (*toIn) {
		if (*toIn == '"') { ++toIn; break; }
		vars[vhere++] = *(toIn++);
		if (counted) { ++vars[vh]; }
	}
	vars[vhere++] = 0; // NULL terminator
}

void doRand() {
	static cell sd = 0;
	if (!sd) { sd = (cell)code  *clock(); }
	sd = (sd << 13) ^ sd;
	sd = (sd >> 17) ^ sd;
	sd = (sd <<  5) ^ sd;
	push(sd);
}

void execIt() {
	// printF("-cH:%d,here:%d-\n",cH, here);
	if (cH < here) {
		comma(0);
		here=cH;
		vhere=cV;
		inner(cH);
	}
}

#undef X
#define X(op, name, imm, code) NCASE op: code

void inner(ushort start) {
	cell t, n;
	ushort pc = start, wc;
	next:
	wc = code[pc++];
	if (wc & 0xC000) { push(wc & 0x3FFF); goto next; }
	switch(wc) {
		case  STOP:   return;
		NCASE LIT1:   push(code[pc++]);
		NCASE LIT2:   push(fetchCell((cell)&code[pc])); pc += CELL_SZ/2;
		NCASE JMP:    pc=code[pc];
		NCASE JMPZ:   if (pop()==0) { pc=code[pc]; } else { ++pc; }
		NCASE NJMPZ:  if (TOS==0) { pc=code[pc]; } else { ++pc; }
		NCASE JMPNZ:  if (pop()) { pc=code[pc]; } else { ++pc; }
		NCASE NJMPNZ: if (TOS) { pc=code[pc]; } else { ++pc; }
		PRIMS
		default: if (code[pc] != EXIT) { rpush(pc); }
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
	// printF("-pw:%s-",w);

	if (isNum(w, base)) {
		cell n = pop();
		if (btwi(n, 0, 0x3fff)) {
			comma((ushort)(n | 0xC000));
		} else if (btwi(n, 0, 0xffff)) {
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
			inner(h);
		} else {
			comma(de->xt);
		}
		return 1;
	 }

	return 0;
}

int outer(const char *ln) {
    cH=here, cL=last, cS=state, cV=vhere;
	toIn = (char *)ln;
	// printF("-outer:%s-\n",ln);
	while (nextWord()) {
		if (!parseWord(wd)) {
			printF("-%s?-", wd);
			here=cH;
			vhere=cV;
			last=cL;
			state=0;
			while (inputFp) { fileClose(inputFp); inputFp=filePop(); }
			return 0;
		}
	}
	if ((cL==last) && (cS==0) && (state==0)) execIt();
	return 1;
}

void parseF(const char *fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
	outer(buf);
}

void printF(const char *fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
	zType(buf);
}

void baseSys() {
	for (int i = 0; prims[i].name; i++) {
		DE_T *w = addWord(prims[i].name);
		w->xt = prims[i].op;
		w->fl = prims[i].fl;
	}

	parseF(": version   #%d ;", VERSION);
	parseF(": (jmp)     #%d ;", JMP);
	parseF(": (jmpz)    #%d ;", JMPZ);
	parseF(": (njmpz)   #%d ;", NJMPZ);
	parseF(": (jmpnz)   #%d ;", JMPNZ);
	parseF(": (njmpnz)  #%d ;", NJMPNZ);
	parseF(": (lit1)    #%d ;", LIT1);
	parseF(": (lit2)    #%d ;", LIT2);
	parseF(": (exit)    #%d ;", EXIT);

	parseF(": (sp)      #%d ;", SPA);
	parseF(": (rsp)     #%d ;", RSPA);
	parseF(": (here)    #%d ;", HA);
	parseF(": (last)    #%d ;", LA);
	parseF(": base      #%d ;", BA);
	parseF(": state     #%d ;", SA);
	parseF(": (lsp)     #%d ;", LSPA);
	parseF(": (tsp)     #%d ;", TSPA);
	parseF(": (lex)     #%d ;", LEXA);

	parseF(addrFmt, "code", &code[0]);
	parseF(addrFmt, "vars", &vars[0]);
	parseF(addrFmt, "dict", &dict[0]);
	parseF(addrFmt, ">in",  &toIn);
	parseF(addrFmt, "(vhere)", &vhere);
	parseF(addrFmt, "(output-fp)", &outputFp);

	parseF(": code-sz #%d ;", CODE_SZ);
	parseF(": vars-sz #%d ;", VARS_SZ);
	parseF(": dict-sz #%d ;", DICT_SZ);
	parseF(": cell    #%d ;", CELL_SZ);
	sys_load();
}

void Init() {
	for (int t=0; t<CODE_SZ; t++) { code[t]=0; }
	for (int t=0; t<VARS_SZ; t++) { vars[t]=0; }
	for (int t=0; t<DICT_SZ; t++) { dict[t]=0; }
	vhere = sp = rsp = lsp = tsp = state = 0;
	last = DICT_SZ;
	base = 10;
	here = LASTPRIM+1;
	fileInit();
	baseSys();
}
