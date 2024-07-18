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
#define regBase       code[RBA]
#define frameSz       code[FSZ]
#define TOS           stk[sp].i
#define NOS           stk[sp-1].i
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]

enum { SPA=0, RSPA, HA, LA, BA, SA, LSPA, TSPA, LEXA, RBA, FSZ };

SE_T stk[STK_SZ+1];
ushort code[CODE_SZ+1], cH, cL, cS;
byte dict[DICT_SZ+2], vars[VARS_SZ+1];
cell vhere, cV, lstk[LSTK_SZ+1], rstk[STK_SZ+1];
char wd[32], *toIn;
cell tstk[TSTK_SZ+1], regs[REGS_SZ+1];

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
	X(UNLOOP,  "unloop",    0, if (lsp>2) { lsp-=3; } ) \
	X(NEXT,    "next",      0, if (++L0<L1) { pc=(ushort)L2; } else { lsp=(lsp<3) ? 0 : lsp-3; } ) \
	X(REGA,    "+regs",     0, if ((regBase+frameSz) < REGS_SZ) { regBase+=frameSz; } ) \
	X(REGM,    "-regs",     0, if (regBase>=frameSz) { regBase-=frameSz; } ) \
	X(REGR,    "reg-r",     0, t=pop()+regBase; push(btwi(t,0,REGS_SZ) ? regs[t] : 0); ) \
	X(REGS,    "reg-s",     0, t=pop()+regBase; n=pop(); if (btwi(t,0,REGS_SZ)) { regs[t]=n; } ) \
	X(REGI,    "reg-i",     0, t=pop()+regBase; if (btwi(t,0,REGS_SZ)) { regs[t]++; } ) \
	X(REGD,    "reg-d",     0, t=pop()+regBase; if (btwi(t,0,REGS_SZ)) { regs[t]--; } ) \
	X(REG_RI,  "reg-ri",    0, t=pop()+regBase; push(btwi(t,0,REGS_SZ) ? regs[t]++ : 0); ) \
	X(REG_RD,  "reg-rd",    0, t=pop()+regBase; push(btwi(t,0,REGS_SZ) ? regs[t]-- : 0); ) \
	X(TOR,     ">r",        0, rpush(pop()); ) \
	X(RAT,     "r@",        0, push(rstk[rsp]); ) \
	X(RFROM,   "r>",        0, push(rpop()); ) \
	X(RDROP,   "rdrop",     0, rpop(); ) \
	X(TTO,     ">t",        0, t=pop(); if (tsp < TSTK_SZ) { tstk[++tsp]=t; }; ) \
	X(TAT,     "t@",        0, push(tstk[tsp]); ) \
	X(TSTO,    "t!",        0, tstk[tsp] = pop(); ) \
	X(TFROM,   "t>",        0, push((0 < tsp) ? tstk[tsp--] : 0); ) \
	X(EMIT,    "emit",      0, emit((char)pop()); ) \
	X(KEY,     "key",       0, push(key()); ) \
	X(QKEY,    "?key",      0, push(qKey()); ) \
	X(COLON,   ":",         1, execIt(); addWord(0); state = 1; ) \
	X(SEMI,    ";",         1, comma(EXIT); state = 0; cH=here; ) \
	X(IMMED,   "immediate", 1, { DE_T *dp = (DE_T*)&dict[last]; dp->fl=1; } ) \
	X(ADDWORD, "addword",   0, execIt(); addWord(0); comma(LIT2); commaCell(vhere+(cell)vars); ) \
	X(CLK,     "timer",     0, push(timer()); ) \
	X(SEE,     "see",       1, doSee(); ) \
	X(COUNT,   "count",     0, t=pop(); push(t+1); push(*(byte *)t); ) \
	X(TYPE,    "type",      0, t=pop(); y=(char*)pop(); for (int i = 0; i<t; i++) emit(y[i]); ) \
	X(ZTYPE,   "ztype",     0, zType((char*)pop()); ) \
	X(FTYPE,   "ftype",     0, fType((char*)pop()); ) \
	X(SCPY,    "s-cpy",     0, t=pop(); strCpy((char*)TOS, (char*)t); ) \
	X(SEQ,     "s-eq",      0, t=pop(); TOS = strEq((char*)TOS, (char*)t); ) \
	X(SEQI,    "s-eqi",     0, t=pop(); TOS = strEqI((char*)TOS, (char*)t); ) \
	X(SZLEN,   "sz-len",    0, TOS = strLen((char*)TOS); ) \
	X(ZQUOTE,  "z\"",       1, quote(0); ) \
	X(QUOTE,   "s\"",       1, quote(1); ) \
	X(DOTQT,   ".\"",       1, quote(0); comma(FTYPE); ) \
	X(RAND,    "rand",      0, doRand(); ) \
	X(FLOPEN,  "fopen",     0, t=pop(); n=pop(); push(fileOpen((char*)n, (char*)t)); ) \
	X(FLCLOSE, "fclose",    0, t=pop(); fileClose(t); ) \
	X(FLDEL,   "fdelete",   0, t=pop(); fileDelete((char*)t); ) \
	X(FLREAD,  "fread",     0, t=pop(); n=pop(); TOS = fileRead((char*)TOS, (int)n, t); ) \
	X(FLWRITE, "fwrite",    0, t=pop(); n=pop(); TOS = fileWrite((char*)TOS, (int)n, t); ) \
	X(FLGETS,  "fgets",     0, t=pop(); n=pop(); TOS = fileGets((char*)TOS, (int)n, t); ) \
	X(INCL,    "include",   1, t=nextWord(); if (t) fileLoad(wd); ) \
	X(LOAD,    "load",      0, t=pop(); blockLoad((int)t); ) \
	X(LOADED,  "loaded?",   0, t=pop(); pop(); if (t) { fileClose(inputFp); inputFp=filePop(); } ) \
	X(ITOA,    "to-string", 0, t=pop(); push((cell)iToA(t, base)); ) \
	X(DOTS,    ".s",        0, dotS(); ) \
	X(FETC,    "@c",        0, TOS = code[(ushort)TOS]; ) \
	X(STOC,    "!c",        0, t=pop(); n=pop(); code[(ushort)t] = (ushort)n; ) \
	X(FIND,    "find",      1, { DE_T *dp=findWord(0); push(dp?dp->xt:0); push((cell)dp); } ) \
	X(SYSTEM,  "system",    0, t=pop(); ttyMode(0); system((char*)t+1); ) \
	X(BYE,     "bye",       0, doBye(); )

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
int lower(const char c) { return btwi(c, 'A', 'Z') ? c + 32 : c; }
int strLen(const char *s) { int l = 0; while (s[l]) { l++; } return l; }

#ifdef IS_PC
void storeWord(cell a, cell v) { *(ushort*)(a) = (ushort)v; }
cell fetchWord(cell a) { return *(ushort*)(a); }
void storeCell(cell a, cell v) { *(cell*)(a) = v; }
cell fetchCell(cell a) { return *(cell*)(a); }
void doBye() { ttyMode(0); exit(0); }
#else
void doBye() { zType("-bye?-"); }
void storeWord(cell a, cell v) {
    // zTypeF("-sw:%lx=%lx-",a,v);
	if ((a & 0x01) == 0) { *(ushort*)(a) = (ushort)v; }
	else {
		byte *y=(byte*)a;
		*(y++) = (v & 0xFF);
		*(y) = (v>>8) & 0xFF;
	}
}
cell fetchWord(cell a) {
    // zTypeF("-fw:%lx-",a);
	if ((a & 0x01) == 0) { return *(ushort*)(a); }
	else {
        cell x;
		byte *y = (byte*)a;
        x = *(y++); x |= (*(y) << 8);
		return x;
    }
}
void storeCell(cell a, cell v) {
	storeWord(a, v & 0xFFFF);
	storeWord(a+2, v >> 16);
}
cell fetchCell(cell a) {
	return fetchWord(a) | (fetchWord(a+2) << 16);
}
#endif

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

void comma(ushort x) { code[here++] = x; }
void commaCell(cell n) {
	storeCell((cell)&code[here], n);
	here += CELL_SZ / 2;
}

int nextWord() {
	int len = 0;
	while (btwi(*toIn, 1, 32)) { ++toIn; }
	while (btwi(*toIn, 33, 126)) { wd[len++] = *(toIn++); }
	wd[len] = 0;
	return len;
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
	// zTypeF("-add:%d,[%s],%d (%d)-\n", newLast, dp->nm, dp->lx, dp->xt);
	return dp;
}

DE_T *findWord(const char *w) {
	if (!w) { nextWord(); w = wd; }
	int len = strLen(w);
	int cw = last;
	while (cw < DICT_SZ) {
		DE_T *dp = (DE_T*)&dict[cw];
		// zTypeF("-%d,(%s)-", cw, dp->nm);
		if ((len == dp->ln) && strEqI(dp->nm, w)) { return dp; }
		cw += dp->sz;
	}
	return (DE_T*)0;
}

int findXT(int xt) {
	int cw = last;
	while (cw < DICT_SZ) {
		DE_T *dp = (DE_T*)&dict[cw];
		// zTypeF("-%d,(%s)-", cw, dp->nm);
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
	if (!dp) { zTypeF("-nf:%s-", wd); return; }
	if (dp->xt <= LASTPRIM) { zTypeF("%s is a primitive (%hX).\r\n", wd, dp->xt); return; }
	cell x = (cell)dp-(cell)dict;
	int stop = findPrevXT(dp->xt)-1;
	int i = dp->xt;
	zTypeF("\r\n%04hX: %s (%04hX to %04X)", (ushort)x, dp->nm, dp->xt, stop);
	while (i <= stop) {
		int op = code[i++];
		x = code[i];
		zTypeF("\r\n%04X: %04X\t", i-1, op);
		if (op & 0xE000) { zTypeF("lit %d", (int)(op & 0x1FFF)); continue; }
		switch (op) {
			case  STOP: zType("stop"); i++;
			BCASE LIT1: zTypeF("lit1 %hu (%hX)", (ushort)x, (ushort)x); i++;
			BCASE LIT2: x = fetchCell((cell)&code[i]);
				zTypeF("lit2 %zd (%zX)", (size_t)x, (size_t)x);
				i += CELL_SZ / 2;
			BCASE JMP:    zTypeF("jmp %04hX", (ushort)x);             i++;
			BCASE JMPZ:   zTypeF("jmpz %04hX (IF)", (ushort)x);       i++;
			BCASE NJMPZ:  zTypeF("njmpz %04hX (-IF)", (ushort)x);     i++;
			BCASE JMPNZ:  zTypeF("jmpnz %04hX (WHILE)", (ushort)x);   i++; break;
			BCASE NJMPNZ: zTypeF("njmpnz %04hX (-WHILE)", (ushort)x); i++; break;
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

void fType(const char *s) {
	// s is a NULL terminated string (NOT counted)
	while (*s) {
		char c = *(s++);
		if (c=='%') {
			c = *(s++);
			switch (c) {
				case  'b': zType(iToA(pop(),2)+1);
				BCASE 'c': emit((char)pop());
				BCASE 'd': zType(iToA(pop(),10)+1);
				BCASE 'e': emit(27);
				BCASE 'i': zType(iToA(pop(),base)+1);
				BCASE 'n': emit(13); emit(10);
				BCASE 'q': emit('"');
				BCASE 's': fType((char*)pop());
				BCASE 'S': zType((char*)pop());
				BCASE 'x': zType(iToA(pop(),16)+1); break;
				default: emit(c); break;
			}
		}
		else { emit(c); }
	}
}

void dotS() {
	zType("( ");
	for (int i = 1; i <= sp; i++) { zTypeF("%s ", iToA(stk[i].i, base)+1); }
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
	if (!sd) { sd = (cell)code; }
	sd = (sd << 13) ^ sd;
	sd = (sd >> 17) ^ sd;
	sd = (sd <<  5) ^ sd;
	push(sd);
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

void inner(ushort start) {
	cell t, n;
	char *y;
	ushort pc = start, wc;
	next:
	wc = code[pc++];
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
		default:
			if (wc & 0xE000) { push(wc & 0x1FFF); goto next; }
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
		if (btwi(n, 0, 0x1fff)) {
			comma((ushort)(n | 0xE000));
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
	// zTypeF("-outer:%s-\n",ln);
	cH=here, cL=last, cS=state, cV=vhere;
	toIn = (char *)ln;
	while (nextWord()) {
		if (!parseWord(wd)) {
			zTypeF("-%s?-", wd);
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
	outerF(": (lit1)    #%d ;", LIT1);
	outerF(": (lit2)    #%d ;", LIT2);
	outerF(": (exit)    #%d ;", EXIT);

	outerF(": (sp)       #%d ;", SPA);
	outerF(": (rsp)      #%d ;", RSPA);
	outerF(": (here)     #%d ;", HA);
	outerF(": (last)     #%d ;", LA);
	outerF(": base       #%d ;", BA);
	outerF(": state      #%d ;", SA);
	outerF(": (lsp)      #%d ;", LSPA);
	outerF(": (tsp)      #%d ;", TSPA);
	outerF(": (lex)      #%d ;", LEXA);
	outerF(": (reg-base) #%d ;", RBA);
	outerF(": (frame-sz) #%d ;", FSZ);

	outerF(addrFmt, "code", &code[0]);
	outerF(addrFmt, "vars", &vars[0]);
	outerF(addrFmt, "dict", &dict[0]);
	outerF(addrFmt, ">in",  &toIn);
	outerF(addrFmt, "(vhere)", &vhere);
	outerF(addrFmt, "(output-fp)", &outputFp);
	outerF(addrFmt, "stk",  &stk[0]);
	outerF(addrFmt, "rstk", &rstk[0]);
	outerF(addrFmt, "tstk", &tstk[0]);
	outerF(addrFmt, "regs", &regs[0]);

	outerF(": code-sz #%d ;", CODE_SZ+1);
	outerF(": vars-sz #%d ;", VARS_SZ);
	outerF(": dict-sz #%d ;", DICT_SZ+1);
	outerF(": stk-sz  #%d ;", STK_SZ+1);
	outerF(": regs-sz #%d ;", REGS_SZ+1);
	outerF(": tstk-sz #%d ;", TSTK_SZ+1);
	outerF(": cell    #%d ;", CELL_SZ);
	sys_load();
}

void Init() {
	for (int t=0; t<CODE_SZ; t++) { code[t]=0; }
	for (int t=0; t<VARS_SZ; t++) { vars[t]=0; }
	for (int t=0; t<DICT_SZ; t++) { dict[t]=0; }
	here = LASTPRIM+1;
	last = DICT_SZ;
	if ((cell)&dict[last] & 0x01) { ++last; }
	base = 10;
	vhere = 0;
	frameSz = 5;
	fileInit();
	baseSys();
}
