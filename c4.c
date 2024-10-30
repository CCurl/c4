#include "c4.h"

#define NCASE         goto next; case
#define BCASE         break; case
#define dsp           code[DSPA]
#define rsp           code[RSPA]
#define lsp           code[LSPA]
#define tsp           code[TSPA]
#define asp           code[ASPA]
#define here          code[HA]
#define last          code[LA]
#define base          code[BA]
#define state         code[SA]
#define TOS           dstk[dsp]
#define NOS           dstk[dsp-1]
#define ATOS          astk[asp]
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]

enum { DSPA=0, RSPA, LSPA, TSPA, ASPA, HA, LA, BA, SA };

wc_t code[CODE_SZ+1];
byte vars[VARS_SZ+1];
cell lstk[LSTK_SZ+1], rstk[STK_SZ+1], dstk[STK_SZ+1];
cell tstk[TSTK_SZ+1], astk[TSTK_SZ+1];
cell vhere;
char wd[32], *toIn;
DE_T tmpWords[10];

#define PRIMS \
	X(EXIT,    "exit",      0, if (0<rsp) { pc = (wc_t)rpop(); } else { return; } ) \
	X(DUP,     "dup",       0, t=TOS; push(t); ) \
	X(SWAP,    "swap",      0, t=TOS; TOS=NOS; NOS=t; ) \
	X(DROP,    "drop",      0, pop(); ) \
	X(OVER,    "over",      0, t=NOS; push(t); ) \
	X(FET,     "@",         0, TOS = fetchCell(TOS); ) \
	X(FET1,    "c@",        0, TOS = *(byte *)TOS; ) \
	X(LFET32,  "d@",        0, TOS = fetch32(TOS); ) \
	X(FETC,    "wc@",       0, TOS = code[(wc_t)TOS]; ) \
	X(STO,     "!",         0, t=pop(); n=pop(); storeCell(t, n); ) \
	X(STO1,    "c!",        0, t=pop(); n=pop(); *(byte*)t=(byte)n; ) \
	X(STO32,   "d!",        0, t=pop(); n=pop(); store32(t, n); ) \
	X(STOC,    "wc!",       0, t=pop(); n=pop(); code[(wc_t)t] = (wc_t)n; ) \
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
	X(INDEX,   "i",         0, push(L0); ) \
	X(NEXT,    "next",      0, if (++L0<L1) { pc=(wc_t)L2; } else { lsp=(lsp<3) ? 0 : lsp-3; } ) \
	X(TOR,     ">r",        0, rpush(pop()); ) \
	X(RSTO,    "r!",        0, rstk[rsp] = pop(); ) \
	X(RAT,     "r@",        0, push(rstk[rsp]); ) \
	X(RATI,    "r@+",       0, push(rstk[rsp]++); ) \
	X(RATD,    "r@-",       0, push(rstk[rsp]--); ) \
	X(RFROM,   "r>",        0, push(rpop()); ) \
	X(RDROP,   "rdrop",     0, rpop(); ) \
	X(TTO,     ">t",        0, t=pop(); if (tsp < TSTK_SZ) { tstk[++tsp]=t; }; ) \
	X(TSTO,    "t!",        0, tstk[tsp] = pop(); ) \
	X(TAT,     "t@",        0, push(tstk[tsp]); ) \
	X(TATI,    "t@+",       0, push(tstk[tsp]++); ) \
	X(TATD,    "t@-",       0, push(tstk[tsp]--); ) \
	X(TFROM,   "t>",        0, push((0 < tsp) ? tstk[tsp--] : 0); ) \
	X(TDROP,   "tdrop",     0, if (0 < tsp) { tsp--; } ) \
	X(TOA,     ">a",        0, t=pop(); if (asp < TSTK_SZ) { astk[++asp] = t; } ) \
	X(ASET,    "a!",        0, ATOS=pop(); ) \
	X(AGET,    "a@",        0, push(ATOS); ) \
	X(AGETI,   "a@+",       0, push(ATOS++); ) \
	X(AGETD,   "a@-",       0, push(ATOS--); ) \
	X(AFROM,   "a>",        0, push((0 < asp) ? astk[asp--] : 0); ) \
	X(ADROP,   "adrop",     0, if (0 < asp) { asp--; } ) \
	X(EMIT,    "emit",      0, emit((char)pop()); ) \
	X(KEY,     "key",       0, push(key()); ) \
	X(QKEY,    "?key",      0, push(qKey()); ) \
	X(COLON,   ":",         0, addWord(0); state=1; ) \
	X(SEMI,    ";",         1, comma(EXIT); state=0; ) \
	X(LITC,    "lit,",      0, t=pop(); compileNum(t); ) \
	X(NEXTWD,  "next-wd",   0, push(nextWord()); ) \
	X(IMMED,   "immediate", 0, { DE_T *dp = (DE_T*)&vars[last]; dp->fl=_IMMED; } ) \
	X(INLINE,  "inline",    0, { DE_T *dp = (DE_T*)&vars[last]; dp->fl=_INLINE; } ) \
	X(OUTER,   "outer",     0, outer((char*)pop()); ) \
	X(ADDWORD, "addword",   0, addWord(0); ) \
	X(CLK,     "timer",     0, push(timer()); ) \
	X(SEE,     "see",       0, doSee(); ) \
	X(ZTYPE,   "ztype",     0, zType((char*)pop()); ) \
	X(FTYPE,   "ftype",     0, fType((char*)pop()); ) \
	X(SCPY,    "s-cpy",     0, t=pop(); strCpy((char*)TOS, (char*)t); ) \
	X(SEQ,     "s-eq",      0, t=pop(); TOS = strEq((char*)TOS, (char*)t); ) \
	X(SEQI,    "s-eqi",     0, t=pop(); TOS = strEqI((char*)TOS, (char*)t); ) \
	X(SLEN,    "s-len",     0, TOS = strLen((char*)TOS); ) \
	X(ZQUOTE,  "z\"",       1, quote(); ) \
	X(DOTQT,   ".\"",       1, quote(); state ? comma(FTYPE) : fType((char*)pop()); ) \
	X(FIND,    "find",      0, { DE_T *dp=findWord(0); push(dp?dp->xt:0); push((cell)dp); } ) \
	X(LOADED,  "loaded?",   0, t=pop(); pop(); if (t) { fileClose(inputFp); inputFp=filePop(); } ) \
	X(FLOPEN,  "fopen",     0, t=pop(); n=pop(); push(fileOpen((char*)n, (char*)t)); ) \
	X(FLCLOSE, "fclose",    0, t=pop(); fileClose(t); ) \
	X(FLDEL,   "fdelete",   0, t=pop(); fileDelete((char*)t); ) \
	X(FLREAD,  "fread",     0, t=pop(); n=pop(); TOS = fileRead((char*)TOS, (int)n, t); ) \
	X(FLWRITE, "fwrite",    0, t=pop(); n=pop(); TOS = fileWrite((char*)TOS, (int)n, t); ) \
	X(FLGETS,  "fgets",     0, t=pop(); n=pop(); TOS = fileGets((char*)TOS, (int)n, t); ) \
	X(INCL,    "include",   0, t=nextWord(); if (t) fileLoad(wd); ) \
	X(LOAD,    "load",      0, t=pop(); blockLoad((int)t); ) \
	X(NXTBLK,  "load-next", 0, t=pop(); blockLoadNext((int)t); ) \
	X(SYSTEM,  "system",    0, t=pop(); ttyMode(0); system((char*)t); ) \
	X(BYE,     "bye",       0, ttyMode(0); exit(0); )

#define X(op, name, imm, cod) op,

enum _PRIM  {
	STOP, LIT, JMP, JMPZ, NJMPZ, JMPNZ, NJMPNZ, PRIMS
};

#undef X
#define X(op, name, imm, code) { op, name, imm },

PRIM_T prims[] = { PRIMS {0, 0, 0} };

void push(cell x) { if (dsp < STK_SZ) { dstk[++dsp] = x; } }
cell pop() { return (0<dsp) ? dstk[dsp--] : 0; }
void rpush(cell x) { if (rsp < RSTK_SZ) { rstk[++rsp] = x; } }
cell rpop() { return (0<rsp) ? rstk[rsp--] : 0; }
int lower(const char c) { return btwi(c, 'A', 'Z') ? c + 32 : c; }
int strLen(const char *s) { int l = 0; while (s[l]) { l++; } return l; }
void store32(cell a, cell v) { *(uint32_t*)(a) = (uint32_t)v; }
cell fetch32(cell a) { return *(uint32_t*)(a); }
void storeCell(cell a, cell v) { *(cell*)(a) = v; }
cell fetchCell(cell a) { return *(cell*)(a); }
void comma(cell x) { code[here++] = (wc_t)x; }
void commaCell(cell n) { storeCell((cell)&code[here], n); here += CELL_SZ / WC_SZ; }

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

int nextWord() {
	int len = 0;
	while (btwi(*toIn, 1, 32)) { ++toIn; }
	while (btwi(*toIn, 33, 126)) { wd[len++] = *(toIn++); }
	wd[len] = 0;
	return len;
}

int isTemp(const char *w) {
	return ((w[0]=='t') && btwi(w[1],'0','9') && (w[2]==0)) ? 1 : 0;
}

DE_T *addWord(const char *w) {
	if (!w) {
		nextWord();
		if (NAME_LEN < strLen(wd)) { wd[NAME_LEN]=0; }
		w = wd;
	}
	if (isTemp(w)) {
		tmpWords[w[1]-'0'].xt = here;
		return &tmpWords[w[1]-'0'];
	}
	last -= sizeof(DE_T);
	int ln = strLen(w);
	DE_T *dp = (DE_T*)&vars[last];
	dp->xt = here;
	dp->fl = 0;
	dp->ln = ln;
	strCpy(dp->nm, w);
	// zTypeF("-add:%d,[%s],%d (%d)-\n", newLast, dp->nm, dp->lx, dp->xt);
	return dp;
}

DE_T *findWord(const char *w) {
	if (!w) { nextWord(); w = wd; }
	if (isTemp(w)) { return &tmpWords[w[1]-'0']; }
	int len = strLen(w);
	int cw = last;
	while (cw < VARS_SZ) {
		DE_T *dp = (DE_T*)&vars[cw];
		if ((len == dp->ln) && strEqI(dp->nm, w)) { return dp; }
		cw += sizeof(DE_T);
	}
	return (DE_T*)0;
}

int findXT(int xt) {
	int cw = last;
	while (cw < VARS_SZ) {
		DE_T *dp = (DE_T*)&vars[cw];
		if (dp->xt == xt) { return cw; }
		cw += sizeof(DE_T);
	}
	return 0;
}

void doSee() {
	DE_T *dp = findWord(0), *lastWord = (DE_T*)&vars[last];
	if (!dp) { zTypeF("-nf:%s-", wd); return; }
	if (dp->xt <= BYE) { zTypeF("%s is a primitive (#%ld/$%lX).\r\n", wd, dp->xt, dp->xt); return; }
	cell x = (cell)dp-(cell)vars;
	int i = dp->xt, stop = (lastWord < dp) ? (dp-1)->xt : here;
	zTypeF("\r\n%04lX: %s (%04lX to %04lX)", (long)x, dp->nm, (long)dp->xt, (long)stop-1);
	while (i < stop) {
		long op = code[i++];
		zTypeF("\r\n%04X: %04X\t", i-1, op);
		if (op & NUM_BITS) { op &= NUM_MASK; zTypeF("num #%ld ($%lx)", op, op); continue; }
		x = code[i];
		switch (op) {
			case  STOP: zType("stop"); i++;
			BCASE LIT: x = fetchCell((cell)&code[i]);
				zTypeF("lit #%zd ($%zX)", (size_t)x, (size_t)x);
				i += (CELL_SZ/WC_SZ);
			BCASE JMP:    zTypeF("jmp $%04lX", (long)x);             i++;
			BCASE JMPZ:   zTypeF("jmpz $%04lX (IF)", (long)x);       i++;
			BCASE NJMPZ:  zTypeF("njmpz $%04lX (-IF)", (long)x);     i++;
			BCASE JMPNZ:  zTypeF("jmpnz $%04lX (WHILE)", (long)x);   i++; break;
			BCASE NJMPNZ: zTypeF("njmpnz $%04lX (-WHILE)", (long)x); i++; break;
			default: x = findXT(op); 
				zType(x ? ((DE_T*)&vars[x])->nm : "??");
		}
	}
}

void iToA(cell n, cell b) {
	if (n<0) { emit('-'); n = -n; }
	if (b<=n) { iToA(n/b, b); }
	n %= b; if (9<n) { n += 7; }
	emit('0'+(char)n);
}

void fType(const char *s) {
	while (*s) {
		char c = *(s++);
		if (c=='%') {
			c = *(s++);
			switch (c) {
				case  'b': iToA(pop(),2);
				BCASE 'c': emit((char)pop());
				BCASE 'd': iToA(pop(),10);
				BCASE 'e': emit(27);
				BCASE 'i': iToA(pop(),base);
				BCASE 'n': emit(13); emit(10);
				BCASE 'q': emit('"');
				BCASE 's': fType((char*)pop());
				BCASE 'S': zType((char*)pop());
				BCASE 'x': iToA(pop(),16); break;
				default: emit(c); break;
			}
		} else { emit(c); }
	}
}

void compileNum(cell num) {
	if (btwi(num, 0, NUM_MASK)) { comma((wc_t)(num | NUM_BITS)); }
	else { comma(LIT); commaCell(num); }
}

void quote() {
	char *vh=(char*)vhere;
	if (*toIn) { ++toIn; }
	while (*toIn) {
		if (*toIn == '"') { ++toIn; break; }
		*(vh++) = *(toIn++);
	}
	*(vh++) = 0; // NULL terminator
	if (state) {
		compileNum(vhere);
		vhere = (cell)vh;
	} else {
		push(vhere);
	}
}

#undef X
#define X(op, name, imm, code) NCASE op: code

void inner(wc_t start) {
	cell t, n;
	wc_t pc = start, wc;
	next:
	wc = code[pc++];
	switch(wc) {
		case  STOP:   return;
		NCASE LIT:   push(fetchCell((cell)&code[pc])); pc += CELL_SZ/WC_SZ;
		NCASE JMP:    pc=code[pc];
		NCASE JMPZ:   if (pop()==0) { pc=code[pc]; } else { ++pc; }
		NCASE NJMPZ:  if (TOS==0) { pc=code[pc]; } else { ++pc; }
		NCASE JMPNZ:  if (pop()) { pc=code[pc]; } else { ++pc; }
		NCASE NJMPNZ: if (TOS) { pc=code[pc]; } else { ++pc; }
		PRIMS
		default:
			if (wc & NUM_BITS) { push(wc & NUM_MASK); goto next; }
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

void executeWord(DE_T *de) {
	int h = here+100;
	code[h]   = de->xt;
	code[h+1] = STOP;
	inner(h);
}

void compileWord(DE_T *de) {
	if (de->fl & _IMMED) { executeWord(de); }
	else if (de->fl & _INLINE) {
		wc_t x = de->xt;
		do { comma(code[x++]); } while (code[x]!=EXIT);
	} else { comma(de->xt); }
}

int parseWord(char *w) {
	// printf("-%s-",w);
	if (isNum(w, base)) {
		if (state) { compileNum(pop()); }
		return 1;
	}

	DE_T *de = findWord(w);
	if (de) {
		if (state) { compileWord(de); }
		else { executeWord(de); }
		return 1;
	}

	return 0;
}

int outer(const char *ln) {
	// zTypeF("-outer:%s-\n",ln);
	char *curIn = toIn;
	toIn = (char*)ln;
	while (nextWord()) {
		if (!parseWord(wd)) {
			zTypeF("-%s?-", wd);
			if (inputFp) { zTypeF(" at\r\n\t%s", ln); }
			state=0;
			while (inputFp) { fileClose(inputFp); inputFp=filePop(); }
			toIn = curIn;
			return 0;
		}
	}
	toIn = curIn;
	return 1;
}

void outerF(const char *fmt, ...) {
	char buf[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
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
		DE_T *w = addWord(prims[i].name);
		w->xt = prims[i].op;
		w->fl = prims[i].fl;
	}

	outerF(addrFmt, "code-sz", CODE_SZ);
	outerF(addrFmt, "vars-sz", VARS_SZ);
	outerF(addrFmt, "de-sz  ", sizeof(DE_T));
	outerF(addrFmt, "dstk-sz", STK_SZ+1);
	outerF(addrFmt, "tstk-sz", TSTK_SZ+1);
	outerF(addrFmt, "wc-sz  ", WC_SZ);
	outerF(addrFmt, "(dsp)  ", DSPA);
	outerF(addrFmt, "(rsp)  ", RSPA);
	outerF(addrFmt, "(lsp)  ", LSPA);
	outerF(addrFmt, "(tsp)  ", TSPA);
	outerF(addrFmt, "(asp)  ", ASPA);

	outerF(addrFmt, "dstk", &dstk[0]);
	outerF(addrFmt, "rstk", &rstk[0]);
	outerF(addrFmt, "tstk", &tstk[0]);
	outerF(addrFmt, "astk", &astk[0]);
	outerF(addrFmt, "code", &code[0]);
	outerF(addrFmt, "vars", &vars[0]);
	outerF(addrFmt, ">in ", &toIn);
	outerF(addrFmt, "wd  ", &wd[0]);
	outerF(addrFmt, "(vhere)", &vhere);
	outerF(addrFmt, "(output-fp)", &outputFp);

	outerF(addrFmt, "version ", VERSION);
	outerF(addrFmt, "(lit)   ", LIT);
	outerF(addrFmt, "(jmp)   ", JMP);
	outerF(addrFmt, "(jmpz)  ", JMPZ);
	outerF(addrFmt, "(njmpz) ", NJMPZ);
	outerF(addrFmt, "(jmpnz) ", JMPNZ);
	outerF(addrFmt, "(njmpnz)", NJMPNZ);
	outerF(addrFmt, "(exit)  ", EXIT);
	outerF(addrFmt, "(here)  ", HA);
	outerF(addrFmt, "(last)  ", LA);
	outerF(addrFmt, "base    ", BA);
	outerF(addrFmt, "state   ", SA);
	outerF(addrFmt, "cell    ", CELL_SZ);
	sys_load();
}

void Init() {
	for (int t=0; t<CODE_SZ; t++) { code[t]=0; }
	for (int t=0; t<VARS_SZ; t++) { vars[t]=0; }
	here = BYE+1;
	last = VARS_SZ;
	base = 10;
	vhere = (cell)&vars[0];
	fileInit();
	baseSys();
}
