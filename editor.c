// editor.cpp - A simple block editor

#include "c4.h"
#include <string.h>

#define __EDITOR__

#ifndef __EDITOR__
void editBlock(cell Blk) { zType("-noEdit-"); }
#else

#define NUM_LINES     16
#define NUM_COLS      64
#define MAX_LINE      (NUM_LINES-1)
#define MAX_COL       (NUM_COLS-1)
#define BLOCK_SZ      (NUM_LINES*NUM_COLS)
#define EDCH(r,c)     edBuf[((r)*NUM_COLS)+(c)]
#define DIRTY         isDirty=1; isShow=1
#define BCASE         break; case

#ifndef MAX
  #define MIN(a,b) ((a)<(b))?(a):(b)
  #define MAX(a,b) ((a)>(b))?(a):(b)
#endif

enum { NORMAL=1, INSERT, REPLACE, QUIT };
enum { Up=7240, Dn=7248, Rt=7245, Lt=7243, Home=7239, PgUp=7241, PgDn=7249,
    End=7247, Ins=7250, Del=7251 };

static int line, off, blkNum, edMode, isDirty, isShow;
static char edBuf[BLOCK_SZ], yanked[NUM_COLS];

static void GotoXY(int x, int y) { zTypeF("\x1B[%d;%dH", y, x); }
static void CLS() { zType("\x1B[2J"); GotoXY(1, 1); }
static void ClearEOL() { zType("\x1B[K"); }
static void CursorOn() { zType("\x1B[?25h"); zType("\x1B[2 q"); }
static void CursorOff() { zType("\x1B[?25l"); }
static void Color(int fg, int bg) { zTypeF("\x1B[%d;%dm", (30+fg), bg?bg:40); }
static void FG(int fg) { zTypeF("\x1B[38;5;%dm", fg); }
static void toFooter() { GotoXY(1, NUM_LINES+3); }
static void toCmd() { GotoXY(1, NUM_LINES+4); }
static void normalMode()  { edMode=NORMAL;  }
static void insertMode()  { edMode=INSERT;  }
static void replaceMode() { edMode=REPLACE; }
static void toggleInsert() { (edMode==INSERT) ? normalMode() : insertMode(); }
static int winKey() { return (245 << 5) ^ key(); }

static int vtKey() {
    int y = key();
    if (y != '[') { return 27; }
    y = key();
    if (btwi(y, 'A', 'T')) {
        switch (y) {
            case 'A': return Up;
            case 'B': return Dn;
            case 'C': return Rt;
            case 'D': return Lt;
            case 'F': return End;
            case 'H': return Home;
            case 'S': return PgUp;
            case 'T': return PgDn;
            default: return 27;
        }
    }
    if (btwi(y, '2', '8')) {
        int z = key();
        if (z!='~') { return 27; }
        switch (y) {
        case '2': return Ins;
        case '3': return Del;
        case '5': return PgUp;
        case '6': return PgDn;
        case '7': return Home;
        case '8': return End;
        default: return 27;
        }
    }
    return 27;
}

static int edKey() {
    int x = key();
    // if (x ==   3) { return 27; }         // ctrl-c => escape
    if (x ==  27) { return vtKey(); }    // Possible VT control sequence
    if (x == 224) { return winKey(); }   // Windows: start char
    return x;
}

static void mv(int r, int c) {
    line += r;
    off += c;
    if (line < 0) { line = 0; }
    if (NUM_LINES <= line) { line = MAX_LINE; }
    if (off < 0) { off=0; }
    if (NUM_COLS <= off) { off = MAX_COL;}
}

static void showState(char ch) {
    static int lastState = INTERP;
    int cols[4] = { 40, 203, 226, 255 };
    if (ch == 0) { ch = lastState; }
    else { lastState = ch; }
    if (ch<5) { FG(cols[ch-1]); }
    char *x = "%c[38;5;%dm]";
}

static void showCursor() {
    GotoXY(off+2, line+2);
    CursorOn();
}

void showStatus() {
    char *x[3] = { "-normal-","-insert-","-replace-" };
    char ch = EDCH(line,off);
    toFooter(); FG(255);
    zTypeF("Block# %03d %s", blkNum, isDirty ? "*" : " ");
    if (edMode != NORMAL) { FG(203); }
    zTypeF(" %s", x[edMode-1]);
    if (edMode != NORMAL) { FG(255); }
    zTypeF(" (%d:%d - #%d/$%02x)", line+1, off+1, ch, ch);
    ClearEOL();
}

static void gotoEOL() {
    off = MAX_COL;
    if (EDCH(line, off) > 32) { return; }
    while (off && (EDCH(line, off-1) < 33)) { --off; }
}

static char *blockFn(int blk) {
    static char fn[32];
    sprintf(fn, "block-%03d.fth", blk); return fn;
}

static void edRdBlk(int force) {
    for (int i = 0; i < BLOCK_SZ; i++) { edBuf[i] = 32; }
    cell fh = fileOpen(blockFn(blkNum), "rb");
    if (fh) {
        fileRead(edBuf, BLOCK_SZ, fh);
        fileClose(fh);
    }
    isDirty = 0;
    isShow = 1;
}

static void edSvBlk(int force) {
    if (isDirty || force) {
        cell fh = fileOpen(blockFn(blkNum), "wb");
        if (fh) {
            fileWrite(edBuf, BLOCK_SZ, fh);
            fileClose(fh);
        }
        isDirty = 0;
    }
}

static void deleteChar() {
    int o = off;
    while (o < MAX_COL) { EDCH(line,o) = EDCH(line, o+1); o++; }
    EDCH(line, MAX_COL) = 32;
    DIRTY;
}

static void deleteWord() {
    while (EDCH(line,off) > 32) { deleteChar(); }
    for (int i=0; i<20; i++) { if (EDCH(line,off)<33) { deleteChar(); } }
}

static void clrToEOL(int l, int o) {
    while (o<NUM_COLS) { EDCH(l,o)=32; o++; }
}

static void deleteLine() {
    if (line < (MAX_LINE)) {
        // TODO
    }
    clrToEOL(MAX_LINE, 0);
    DIRTY;
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

static void insertLine() {
    if (line < (MAX_LINE)) {
        // TODO
    }
    clrToEOL(line, 0);
    DIRTY;
}

static void joinLines() {
    gotoEOL();
    DIRTY;
}

static void replaceChar(char c, int force, int mov) {
    if (btwi(c,32,126) || (force)) {
        EDCH(line, off)=c;
        DIRTY;
        if (mov) { mv(0, 1); }
    }
}

static int doInsertReplace(char c) {
    if (c==13) {
        mv(1, -NUM_COLS);
        if (edMode == INSERT) { insertLine(); }
        return 1;
    }
    if (!btwi(c,1,5) && !btwi(c,32,126)) { return 1; }
    if (edMode == INSERT) { insertSpace(0); }
    replaceChar(c, 1, 1);
    return 1;
}

static void edDelX(int c) {
    if (c==0) { c = key(); }
    if (c=='d') { deleteLine(); }
    else if (c=='w') { deleteWord(); }
    else if (c=='.') { deleteChar(); }
    else if (c=='X') { if (0<off) { --off; deleteChar(); } }
    else if (c=='$') {
        clrToEOL(line, off);
        DIRTY;
    }
}

static int edReadLine(char *buf, int sz) {
    int len = 0;
    CursorOn();
    while (len<(sz-1)) {
        char c = key();
        if (c ==  3) { len=0; break; }
        if (c == 27) { len=0; break; }
        if (c == 13) { break; }
        if ((c==127) || ((c==8) && (len))) { --len; zTypeF("%c %c",8,8); }
        if (btwi(c,32,126)) { buf[len++]=c; emit(c); }
    }
    CursorOff();
    buf[len]=0;
    return len;
}

static void edCommand() {
    char buf[32];
    toCmd(); ClearEOL();
    emit(':');
    edReadLine(buf, sizeof(buf));
    toCmd(); ClearEOL();
    if (strEq(buf,"w")) { edSvBlk(0); }
    else if (strEq(buf,"w!")) { edSvBlk(1); }
    else if (strEq(buf,"wq")) { edSvBlk(0); edMode=QUIT; }
    else if (strEq(buf,"rl")) { edRdBlk(1); }
    else if (strEq(buf,"q!")) { edMode=QUIT; }
    else if (strEq(buf,"q")) {
        if (isDirty) { zType("(use 'q!' to quit without saving)"); }
        else { edMode=QUIT; }
    }
}

static void PageUp() { edSvBlk(0); blkNum = MAX(0, blkNum-1); edRdBlk(0); line=off=0; }
static void PageDn() { edSvBlk(0); ++blkNum; edRdBlk(0); line=off=0; }

static void doCTL(int c) {
    if (((c == 8) || (c == 127)) && (0 < off)) {      // <backspace>
        mv(0, -1); if (edMode == INSERT) { deleteChar(); }
        return;
    }
    if (c == 13) {      // <CR>
        mv(1, -NUM_COLS); if (edMode == INSERT) { insertLine(); }
        return;
    }
    switch (c) {
        case   1:   doInsertReplace(c);     // COMPLE
        BCASE  2:   doInsertReplace(c);     // DEFINE
        BCASE  3:   doInsertReplace(c);     // INTERP
        BCASE  4:   doInsertReplace(c);     // COMMENT
        BCASE  9:   mv(0, 8);               // <tab>
        BCASE 10:   mv(1, 0);               // <ctrl-j>
        BCASE 11:   mv(-1, 0);              // <ctrl-k>
        BCASE 12:   mv(0, 1);               // <ctrl-l>
        BCASE 24:   edDelX('X');            // <ctrl-x>
        BCASE 26:   edDelX('.');            // <ctrl-z>
        BCASE 27:   normalMode();           // <ctrl-z>
        BCASE Up:   mv(-1, 0);              // Up
        BCASE Lt:   mv(0, -1);              // Left
        BCASE Rt:   mv(0, 1);               // Right
        BCASE Dn:   mv(1, 0);               // Down
        BCASE Home: mv(0, -NUM_COLS);       // Home
        BCASE End:  gotoEOL();              // End
        BCASE PgUp: PageUp();               // PgUp
        BCASE PgDn: PageDn();               // PgDn
        BCASE 7251: edDelX('.');            // Delete
        BCASE 7250: toggleInsert();         // Insert
        BCASE 7287: mv(-NUM_LINES, -NUM_COLS);  // <ctrl>-Home
        // BCASE 7309: scroll(-1);             // <ctrl>-Up
        // BCASE 7313: scroll( 1);             // <ctrl>-Dn
    }
}

static int processEditorChar(int c) {
    CursorOff();
    if (!btwi(c,32,126)) { doCTL(c); return 1; }
    if (btwi(edMode,INSERT,REPLACE)) { return doInsertReplace((char)c); }

    switch (c) {
        BCASE ' ': mv(0, 1);
        BCASE 'h': mv(0,-1);
        BCASE 'l': mv(0, 1);
        BCASE 'j': mv(1, 0);
        BCASE 'k': mv(-1,0);
        BCASE '_': mv(0,-NUM_COLS);
        BCASE 'a': mv(0, 1); insertMode();
        BCASE 'A': gotoEOL(); insertMode();
        BCASE 'b': insertSpace(0);
        BCASE 'B': insertSpace(1);
        BCASE 'J': joinLines();
        BCASE '$': gotoEOL();
        BCASE 'g': mv(-NUM_LINES,-NUM_COLS);
        BCASE 'G': mv(NUM_LINES,-NUM_COLS);
        BCASE 'i': insertMode();
        BCASE 'I': mv(0, -NUM_COLS); insertMode();
        BCASE 'o': mv(1, -NUM_COLS); insertLine(); insertMode();
        BCASE 'O': mv(0, -NUM_COLS); insertLine(); insertMode();
        BCASE 'r': replaceChar(edKey(), 0, 1);
        BCASE 'R': replaceMode();
        BCASE 'c': edDelX('.'); insertMode();
        BCASE 'C': edDelX('$'); insertMode();
        BCASE 'd': edDelX(0);
        BCASE 'D': edDelX('$');
        BCASE 'x': edDelX('.');
        BCASE 'X': edDelX('X');
        BCASE '1': replaceChar(1,1,0); // COMPILE
        BCASE '2': replaceChar(2,1,0); // DEFINE
        BCASE '3': replaceChar(3,1,0); // INTERP
        BCASE '4': replaceChar(4,1,0); // COMMENT
        BCASE 'Y': strCpy(yanked, &EDCH(line, 0));
        BCASE 'p': mv(1,-NUM_COLS); insertLine(); strCpy(&EDCH(line,0), yanked);
        BCASE 'P': mv(0,-NUM_COLS); insertLine(); strCpy(&EDCH(line,0), yanked);
        BCASE '+': edSvBlk(0); ++blkNum; edRdBlk(0); line=off=0;
        BCASE '-': edSvBlk(0); blkNum = MAX(0, blkNum-1); edRdBlk(0); line=off=0;
        BCASE ':': edCommand();
    }
    return 1;
}

static void showEditor() {
    if (!isShow) { return; }
    FG(40); GotoXY(1,1);
    for (int i=0; i <= NUM_COLS; i++) { emit('-'); } zType("\r\n");
    for (int r=0; r < NUM_LINES; r++) {
        zType("|"); showState(0);
        for (int c = 0; c < NUM_COLS; c++) {
            char ch = EDCH(r,c);
            if (btwi(ch,1,4)) { showState(ch); }
            emit(MAX(ch,32));
        }
        FG(40); zType("|\r\n"); 
    }
    for (int i=0; i <= NUM_COLS; i++) { emit('-'); }
    isShow = 0;
}

void editBlock(cell Blk) {
    blkNum = MAX((int)Blk, 0);
    line = off = 0;
    CLS();
    edRdBlk(0);
    isShow = 1;
    normalMode();
    while (edMode != QUIT) {
        showEditor();
        showStatus();
        showCursor();
        processEditorChar(edKey());
    }
    toCmd();
    CursorOn(); FG(255);
}

#endif //  __EDITOR__
