// editor.cpp - A simple block editor

#include "c4.h"
#include <string.h>

#ifndef EDITOR
void editBlock(cell blk) { zType("-no edit-"); }
#else

#define NUM_LINES     32
#define NUM_COLS      100
#define BLOCK_SZ      (NUM_LINES*NUM_COLS)
#define MAX_LINE      (NUM_LINES-1)
#define MAX_COL       (NUM_COLS-1)
#define EDCH(r,c)     edBuf[((r)*NUM_COLS)+(c)]
#define DIRTY         isDirty=1; isShow=1
#define CLEAN(show)   isDirty=0; isShow=show
#define BCASE         break; case
#define RCASE         return; case
#define EOL_CHAR      13

#ifndef MAX
  #define MIN(a,b) ((a)<(b))?(a):(b)
  #define MAX(a,b) ((a)>(b))?(a):(b)
#endif

enum { NORMAL=1, INSERT, REPLACE, QUIT };
enum { Up=7240, Dn=7248, Rt=7245, Lt=7243, Home=7239, PgUp=7241, PgDn=7249,
    End=7247, Ins=7250, Del=7251, CHome=7287, CEnd=7285,
    STab=12333, F1=0xF01, F5=0xF05, F6=0xF06, F7=0xF07
};

static cell line, off, edMode, isDirty, isShow, block;
static char edBuf[BLOCK_SZ], yanked[NUM_COLS+1];

static void GotoXY(int x, int y) { zTypeF("\x1B[%d;%dH", y, x); }
static void CLS() { zType("\x1B[2J"); GotoXY(1, 1); }
static void ClearEOL() { zType("\x1B[K"); }
static void CursorBlock() { zType("\x1B[2 q"); }
static void CursorOn() { zType("\x1B[?25h"); }
static void CursorOff() { zType("\x1B[?25l"); }
static void showCursor() { GotoXY(off+2, line+2); CursorOn(); CursorBlock(); }
static void FG(int fg) { zTypeF("\x1B[38;5;%dm", fg); }
static void toFooter() { GotoXY(1, NUM_LINES+3); }
static void toCmd() { GotoXY(1, NUM_LINES+4); }
static void normalMode()  { edMode=NORMAL;  }
static void insertMode()  { edMode=INSERT;  }
static void replaceMode() { edMode=REPLACE; }
static void toggleInsert() { (edMode==INSERT) ? normalMode() : insertMode(); }
static void setBlock(wc_t blk) { block=MAX(MIN(blk,999),0); storeWC(BLKA, (wc_t)block); }
static void Green() { FG(40); }
static void Red() { FG(203); }
static void Yellow() { FG(226); }
static void White() { FG(231); }
static void Purple() { FG(213); }
static int  winKey() { return (224 << 5) ^ key(); }
static int  winFKey() { return 0xF00 + key() - 58; }

// VT key mapping, after <escape>, '['
#define NUM_VTK 16
static int vks[NUM_VTK][7] = {
        { 0, 49, 53, 126, 999, F5 },
        { 0, 49, 55, 126, 999, F6 },
        { 0, 49, 56, 126, 999, F7 },
        { 0, 49, 59, 53, 72, 999, CHome },
        { 0, 49, 59, 53, 70, 999, CEnd },
        { 0, 50, 126, 999, Ins },
        { 0, 51, 126, 999, Del },
        { 0, 53, 126, 999, PgUp },
        { 0, 54, 126, 999, PgDn },
        { 0, 65, 999, Up },
        { 0, 66, 999, Dn },
        { 0, 67, 999, Rt },
        { 0, 68, 999, Lt },
        { 0, 70, 999, End },
        { 0, 72, 999, Home },
        { 0, 90, 999, STab },
    };

static int vtKey() {
    if (key() != '[') { return 27; }
    int ndx = 0, k, m;
    for (int i=0; i<NUM_VTK; i++) { vks[i][0] = 1; }
    while (++ndx < 5) {
        m = 0;
        k = key();
        for (int i=0; i<NUM_VTK; i++) {
            if ((vks[i][0] == ndx) && (vks[i][ndx] == k)) {
                if (vks[i][ndx+1] == 999) { return vks[i][ndx+2]; }
                vks[i][0] = ndx+1;
                m++;
            }
        }
        if (m == 0) { return 27; }
    }
    return 27;
}

static int edKey() {
    int x = key();
    if (x ==   0) { return winFKey(); }  // Windows: Function key
    if (x ==  27) { return vtKey(); }    // Possible VT control sequence
    if (x == 224) { return winKey(); }   // Windows: start char
    return x;
}

static void mv(int r, int c) {
    line += r;
    off += c;
    if (line < 0) { line = 0; }
    if (MAX_LINE < line) { line = MAX_LINE; }
    if (off < 0) { off=0; }
    if (MAX_COL < off) { off = MAX_COL;}
}

static void mvLeft() { mv(0,-1); }
static void mvRight() { mv(0,1); }
static void mvUp() { mv(-1,0); }
static void mvDown() { mv(1,0); }
static void mvNextLine() { mv(1,-99); }
static void mvPrevLine() { mv(-1,-99); }

static void moveWord(int isRight) {
    if (isRight) {
        while ((off < MAX_COL) & (EDCH(line,off) > 32)) { ++off; }
        while ((off < MAX_COL) & (EDCH(line,off) < 33)) { ++off; }
    } else {
        while ((0 < off) & (EDCH(line,off-1) < 33)) { --off; }
        while ((0 < off) & (EDCH(line,off-1) > 32)) { --off; }
    }
}

static void showState(char ch) {
    static int lastState = INTERP;
    if (ch == -1) { lastState = INTERP; return; }
    int cols[4] = { 40, 203, 226, 231 };
    if (ch == 0) { ch = (lastState) ? lastState : INTERP; }
    if (btwi(ch,1,4)) { FG(cols[ch-1]); lastState = ch; }
}

static void gotoEOL() {
    off = MAX_COL;
    if (EDCH(line, off) > 32) { return; }
    while (off && (EDCH(line, off-1) < 33)) { --off; }
}

static void copyLine(char *from, char *to, int nullTerm) {
    for (int c=0; c<NUM_COLS; c++) { *(to++) = *(from++); }
    if (nullTerm) { *to = 0; }
}

static void yankLine(int lineNum, char *to) {
    copyLine(&EDCH(lineNum, 0), to, 1);
}

static void edRdBlk() {
    for (int i=0; i<BLOCK_SZ; i++) { edBuf[i]=32; }
    toCmd();
    cell fh = fileOpen(blockFn(block), "rb");
    if (fh) {
        char x[BLOCK_SZ+1];
        int r=0, c=0, n=fileRead(x, BLOCK_SZ, fh);
        zTypeF("%d chars", n);
        fileClose(fh);
        for (int i=0; i<n; i++) {
            char ch=x[i];
            if (ch==13) { continue; }
            if (ch==9) { ch=32; }
            if (ch==10) {
                if (r == MAX_LINE) { break; }
                r++; c=0;
            } else { EDCH(r,c)=ch; c++; }
        }
    } else { zTypeF("-can't open [%s]!-", blockFn(block)); }
    ClearEOL();
    CLEAN(1);
}

static void edSvBlk(int force) {
    if (!isDirty && !force) { return; }
    char x[NUM_COLS+1];
    cell fh = fileOpen(blockFn(block), "wb");
    if (fh) {
        outputFp = fh;
        for (int r=0; r<NUM_LINES; r++) {
            yankLine(r, x);
            if (x[MAX_COL]==0) { x[MAX_COL] = 32; }
            for (int r=MAX_COL; 0 <= r; r--) {
                if (x[r]==32) { x[r] = 0; }
                else { break; }
            }
            zType(x);
            emit(10);
        }
        outputFp = 0;
        fileClose(fh);
        toCmd(); zType("-saved-"); ClearEOL();
    }
    CLEAN(0);
}

static void deleteChar(int toEnd) {
    char *f = &EDCH(line, off);
    char *t = toEnd ? &EDCH(MAX_LINE, MAX_COL) : &EDCH(line, MAX_COL);
    while (f < t) { *(f) = *(f+1); f++; }
    *f = 32;
    DIRTY;
}

static void deleteWord() {
    while (EDCH(line,off) > 32) { deleteChar(0); }
    for (int i=0; i<20; i++) { if (EDCH(line,off)<33) { deleteChar(0); } }
}

static void clrToEOL(int l, int o) {
    while (o<NUM_COLS) { EDCH(l,o)=32; o++; }
    DIRTY;
}

static void putLine(int l) {
    copyLine(yanked, &EDCH(l, 0), 0);
}

static void deleteLine(int l) {
    for (int r=l; r<MAX_LINE; r++) { copyLine(&EDCH(r+1, 0), &EDCH(r, 0), 0); }
    clrToEOL(MAX_LINE, 0);
}

static void insertSpace(int toEnd) {
    if (toEnd) {
        char *f = &EDCH(line,off);
        char *t = &EDCH(MAX_LINE,MAX_COL);
        while (f<t) { *(t) = *(t-1); t--; }
    } else {
        for (int o=MAX_COL; off<o; o--) {
            EDCH(line,o) = EDCH(line, o-1);
        }
    }
    EDCH(line, off)=32;
    DIRTY;
}

static void insertLine(int l, int o) {
    for (int r=MAX_LINE; r>l; r--) {
        char *f = &EDCH(r-1, 0);
        char *t = &EDCH(r, 0);
        for (int c=0; c<NUM_COLS; c++) { *(t++) = *(f++); }
    }
    clrToEOL(l, 0);
    if (0 <= o) {
        for (int i=o; i<NUM_COLS; i++) { EDCH(l,i-o) = EDCH(l-1,i); }
        clrToEOL(l-1, o);
    }
}

static void joinLines() {
    if (line == MAX_LINE) { return; }
    gotoEOL();
    int o = off+1;
    char *t = &EDCH(line, o);
    char *f = &EDCH(line+1, 0);
    while ((o++) < NUM_COLS) { *(t++) = *(f++); }
    deleteLine(line+1);
}

static void execLine(int ln) {
    char *x = &EDCH(ln,0);
    char c = x[MAX_COL];
    x[MAX_COL] = 0;
    changeState(INTERP); ttyMode(0);
    toCmd(); outer(x);
    x[MAX_COL] = c;
}

static void replaceChar(char c, int force, int mov) {
    if (btwi(c,32,126) || (force)) {
        EDCH(line, off)=c;
        DIRTY;
        if (mov) { mvRight(); }
    }
}

static void replace1() {
    Purple(); zType("?\x08"); CursorOn();
    int ch = key(); CursorOff();
    replaceChar(ch, 0, 1);
    isShow = 1;
}

static int doInsertReplace(char c) {
    if (c==EOL_CHAR) {
        if (edMode == INSERT) { insertLine(line+1, off); }
        mvNextLine();
        return 1;
    }
    if (!btwi(c,1,5) && !btwi(c,32,126)) { return 1; }
    if (edMode == INSERT) { insertSpace(0); }
    replaceChar(c, 1, 1);
    return 1;
}

static void edDelX(int c) {
    if (c==0) { c = key(); }
    if (c=='d') { yankLine(line, yanked); deleteLine(line); }
    else if (c=='w') { deleteWord(); }
    else if (c=='.') { deleteChar(0); }
    else if (c=='x') { deleteChar(0); }
    else if (c=='X') { if (0<off) { --off; deleteChar(0); } }
    else if (c=='$') { clrToEOL(line, off); }
    else if (c=='Z') { deleteChar(1); }
}

static int edReadLine(char *buf, int sz) {
    int len = 0;
    CursorOn();
    while (len<(sz-1)) {
        char c = key();
        if (c == EOL_CHAR) { break; }
        if (c ==  3) { len=0; break; }
        if (c == 27) { len=0; break; }
        if (((c==127) || (c==8)) && (0<len)) { --len; zType("\x08 \x08"); }
        if (btwi(c,32,126)) { buf[len++]=c; emit(c); }
    }
    CursorOff();
    buf[len]=0;
    return len;
}

static void edCommand() {
    char buf[32];
    toCmd(); emit(':'); ClearEOL();
    edReadLine(buf, sizeof(buf));
    toCmd(); ClearEOL();
    if (buf[0]=='!') {
        wc_t curBlock = fetchWC(BLKA);
        ttyMode(0); changeState(INTERP); outer(&buf[1]);
        wc_t nowBlock = fetchWC(BLKA);
        if (curBlock != nowBlock) { block=nowBlock; strCpy(buf, "rl"); }
    }
    if (strEqI(buf,"rl")) { edRdBlk(); }
    else if (strEqI(buf,"w")) { edSvBlk(0); }
    else if (strEqI(buf,"w!")) { edSvBlk(1); }
    else if (strEqI(buf,"wq")) { edSvBlk(0); edMode=QUIT; }
    else if (strEqI(buf,"q!")) { edMode=QUIT; }
    else if (strEqI(buf,"q")) {
        if (isDirty) { zType("(use 'q!' to quit without saving)"); }
        else { edMode=QUIT; }
    }
}

static void gotoBlock(int blk) {
        edSvBlk(0); setBlock(blk);
        edRdBlk(); line = off = 0;
}

static void doCTL(int c) {
    if (c == EOL_CHAR) { doInsertReplace(c); return; }
    if (((c == 8) || (c == 127)) && (0 < off)) {      // <backspace>
        mvLeft(); if (edMode == INSERT) { deleteChar(0); }
        return;
    }
    switch (c) {
        case   1:    doInsertReplace(c);         // COMPLE
        RCASE  2:    doInsertReplace(c);         // DEFINE
        RCASE  3:    doInsertReplace(c);         // INTERP
        RCASE  4:    doInsertReplace(c);         // COMMENT
        RCASE  5:    execLine(line);             // Execute current line
        RCASE  9:    mv(0, 8);                   // <tab>
        RCASE 10:    mvDown();                   // <ctrl-j>
        RCASE 11:    mvUp();                     // <ctrl-k>
        RCASE 12:    mvRight();                  // <ctrl-l>
        RCASE 17:    mv(0, -8);                  // <ctrl-q>
        RCASE 24:    edDelX('.');                // <ctrl-x>
        RCASE 20:    edSvBlk(0);                 // <ctrl-s>
        RCASE 27:    normalMode();               // <escape>
        RCASE Up:    mvUp();                     // Up
        RCASE Lt:    mvLeft();                   // Left
        RCASE Rt:    mvRight();                  // Right
        RCASE Dn:    mvDown();                   // Down
        RCASE Home:  mv(0, -NUM_COLS);           // Home
        RCASE End:   gotoEOL();                  // End
        RCASE PgUp:  gotoBlock(block-1);         // PgUp
        RCASE PgDn:  gotoBlock(block+1);         // PgDn
        RCASE Del:   edDelX('.');                // Delete
        RCASE Ins:   toggleInsert();             // Insert
        RCASE CHome: mv(-NUM_LINES, -NUM_COLS);  // <ctrl>-Home
        RCASE CEnd:  mv(NUM_LINES, -NUM_COLS);   // <ctrl>-End
        RCASE STab:  mv(0, -8);                  // <shift-tab>
        RCASE F1: toCmd(); zType("-See Editor.md-"); // F1
    }
}

static int processEditorChar(int c) {
    CursorOff();
    if (!btwi(c,32,126)) { doCTL(c); return 1; }
    if (btwi(edMode,INSERT,REPLACE)) { return doInsertReplace((char)c); }

    switch (c) {
        BCASE ' ': mvRight();
        BCASE '#': CLS(); isShow=1;
        BCASE '$': gotoEOL();
        BCASE '_': mv(0,-NUM_COLS);
        BCASE '1': replaceChar(1,1,0);  // COMPILE
        BCASE '2': replaceChar(2,1,0);  // DEFINE
        BCASE '3': replaceChar(3,1,0);  // INTERP
        BCASE '4': replaceChar(4,1,0);  // COMMENT
        BCASE '+': gotoBlock(block+1);  // Next block
        BCASE '-': gotoBlock(block-1);  // Prev block
        BCASE ':': edCommand();
        BCASE 'a': mvRight(); insertMode();
        BCASE 'A': gotoEOL(); insertMode();
        BCASE 'b': insertSpace(0);
        BCASE 'B': insertSpace(1);
        BCASE 'c': edDelX('.'); insertMode();
        BCASE 'C': edDelX('$'); insertMode();
        BCASE 'd': edDelX(0);
        BCASE 'D': edDelX('$');
        BCASE 'g': mv(-NUM_LINES,-NUM_COLS);
        BCASE 'G': mv(NUM_LINES,-NUM_COLS);
        BCASE 'h': mvLeft();
        BCASE 'H': gotoBlock((block & 0x01) ? block+1 : block-1);
        BCASE 'i': insertMode();
        BCASE 'I': mv(0, -NUM_COLS); insertMode();
        BCASE 'j': mvDown();
        BCASE 'J': joinLines();
        BCASE 'k': mvUp();
        BCASE 'l': mvRight();
        BCASE 'M': line = (NUM_LINES/2)-1; off = 0;
        BCASE 'o': mvNextLine(); insertLine(line, -1); insertMode();
        BCASE 'O': insertLine(line, -1); insertMode();
        BCASE 'p': mvNextLine(); insertLine(line, -1); putLine(line);
        BCASE 'P': insertLine(line, -1); putLine(line);
        BCASE 'q': mv(0,8);
        BCASE 'Q': mv(0,-8);
        BCASE 'r': replace1();
        BCASE 'R': replaceMode();
        BCASE 'w': moveWord(1);
        BCASE 'W': moveWord(0);
        BCASE 'x': edDelX(c);
        BCASE 'X': edDelX(c);
        BCASE 'Y': yankLine(line, yanked);
        BCASE 'Z': edDelX(c);
    }
    return 1;
}

static void showFooter() {
    const char *x[3] = { "-normal-","-insert-","-replace-" };
    char ch = EDCH(line,off);
    toFooter(); White();
    zTypeF("Block# %03d%s", block, isDirty ? " *" : "");
    if (edMode != NORMAL) { Red(); }
    zTypeF(" %s", x[edMode-1]);
    if (edMode != NORMAL) { White(); }
    zTypeF(" (%d:%d - #%d/$%02x)", line+1, off+1, ch, ch);
    ClearEOL();
}

static void showEditor() {
    if (!isShow) { return; }
    Green(); GotoXY(1,1); showState(-1);
    for (int i=-2; i<NUM_COLS; i++) { emit('-'); } zType("\r\n");
    for (int r=0; r<NUM_LINES; r++) {
        zType("|"); showState(0);
        for (int c=0; c<NUM_COLS; c++) {
            char ch = EDCH(r,c);
            if (btwi(ch,1,4)) { showState(ch); }
            emit(MAX(ch,32));
        }
        Green(); zType("|\r\n"); 
    }
    for (int i=-2; i<NUM_COLS; i++) { emit('-'); }
    isShow = 0;
}

void editBlock(cell blk) {
    setBlock(blk);
    line = off = 0;
    CLS();
    edRdBlk();
    isShow = 1;
    normalMode();
    while (edMode != QUIT) {
        showEditor();
        showFooter();
        showCursor();
        edBuf[BLOCK_SZ-1]=0;
        processEditorChar(edKey());
    }
    toCmd(); ClearEOL();
    CursorOn(); White();
}

#endif //  EDITOR
