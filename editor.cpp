// editor.cpp - A simple block editor
//
// NOTE: A huge thanks to Alain Theroux. This editor was inspired by
//       his editor and is a shameful reverse-engineering of it. :D

#include "c4.h"

#ifndef __EDITOR__

void doEditor() {}

#else

#include <stdio.h>

#define LLEN        96
#define NUM_LINES   32
#define BLOCK_SZ    (NUM_LINES)*(LLEN)
#define MAX_CUR     (BLOCK_SZ-1)
#define SETC(c)     edLines[line][off]=c
#define FAKE_NL     190
int line, off, blkNum;
int cur, isDirty = 0;
char theBlock[BLOCK_SZ];
const char* msg = NULL;
char edLines[NUM_LINES][LLEN];

extern char *sprintF(char *dst, const char *fmt, ...);
extern int strLen(const char *cp);

void GotoXY(int x, int y) { printStringF("\x1B[%d;%dH", y, x); }
void CLS() { printString("\x1B[2J"); GotoXY(1, 1); }
void ClearEOL() { printString("\x1B[K"); }
void CursorOn() { printString("\x1B[?25h"); }
void CursorOff() { printString("\x1B[?25l"); }
void Color(int c, int bg) {
    printStringF("%c[%d;%dm", 27, (30 + c), bg ? bg : 40);
}

void NormLO() {
    if (line < 0) { line = 0; }
    if (NUM_LINES <= line) { line = NUM_LINES - 1; }
    if (off < 0) { off = 0; }
    int len = strLen(edLines[line]);
    if (len < off) { off = len; }
}

char edChar(int l, int o) {
    char c = edLines[l][o];
    return c ? c : ' ';
}

void showLine(int ln) {
    // CursorOff();
    GotoXY(1, ln + 1);
    printString(edLines[ln]);
    ClearEOL();
    // CursorOn();
}

void showCursor() {
    char c = edChar(line, off);
    GotoXY(off + 1, line + 1);
    Color(0, 47);
    printChar(c ? c : 'X');
    Color(7, 0);
}

void mv(int l, int o) {
    showLine(line);
    line += l;
    off += o;
    NormLO();
    showLine(line);
    showCursor();
}

void edSetCh(byte c) {
    SETC(c);
    mv(0, 1);
    isDirty = 1;
}

int edGetChar() {
    CursorOn();
    int c = getChar();
    CursorOff();
    // in PuTTY, cursor keys are <esc>, '[', [A..D]
    // other keys are <esc>, '[', [1..6] , '~'
    if (c == 27) {
        c = getChar();
        if (c == '[') {
            c = getChar();
            if (c == 'D') { return 'h'; } // left
            if (c == 'B') { return 'j'; } // down
            if (c == 'A') { return 'k'; } // up
            if (c == 'C') { return 'l'; } // right
            if (c == '1') { if (getChar() == '~') { return '_'; } } // home
            if (c == '4') { if (getChar() == '~') { return '$'; } } // end
            if (c == '5') { if (getChar() == '~') { return 'g'; } } // top (pgup)
            if (c == '6') { if (getChar() == '~') { return 'G'; } } // last (pgdn)
            if (c == '3') { if (getChar() == '~') { return 'x'; } } // del
        }
        return c;
    }
    else {
        // in Windows, cursor keys are 224, [HPMK]
        // other keys are 224, [GOIQS]
        if (c == 224) {
            c = getChar();
            if (c == 'K') { return 'h'; } // left
            if (c == 'P') { return 'j'; } // down
            if (c == 'H') { return 'k'; } // up
            if (c == 'M') { return 'l'; } // right
            if (c == 'G') { return '_'; } // home
            if (c == 'O') { return '$'; } // end
            if (c == 'I') { return 'g'; } // top pgup (pgup)
            if (c == 'Q') { return 'G'; } // last (pgdn)
            if (c == 'S') { return 'x'; } // del
            return c;
        }
    }
    return c;
}

void clearBlock() {
    for (int i = 0; i < BLOCK_SZ; i++) {
        theBlock[i] = 0;
        // if ((i % 50) == 0) { theBlock[i] = 10; }
    }
}

int toBlock() {
    clearBlock();
    int o = 0;
    for (int ln = 0; ln < NUM_LINES; ln++) {
        for (int x = 0; x < LLEN; x++) {
            byte c = edLines[ln][x];
            if (c) { theBlock[o++] = c; }
            else { break; }
        }
        theBlock[o++] = 10;
    }
    return o;
}

void toLines() {
    int o = 0, x = 0, ln = 0;
    while ((o < BLOCK_SZ) && theBlock[o]) {
        byte c = theBlock[o++];
        if ((c == 10) || (c == FAKE_NL)) {
            edLines[ln++][x] = 0;
            if (NUM_LINES <= ln) { return; }
            x = 0;
            continue;
        }
        if (x < LLEN) {
            edLines[ln][x++] = (c < 32) ? ' ' : c;
        }
    }
    edLines[ln++][x] = 0;
    while (ln < NUM_LINES) { edLines[ln++][0] = 0; }
}

void edRdBlk() {
    clearBlock();
    char buf[24];
    sprintF(buf, "./block-%03d.4th", blkNum);
    push((CELL)buf);
    push(1);
    fOpen();
    msg = "-noFile-";
    CELL fh = pop();
    if (fh) {
        push((CELL) theBlock);
        push(BLOCK_SZ);
        push(fh);
        fRead();
        pop();
        msg = "-loaded-";
        push(fh);
        fClose();
    }
    toLines();
    cur = isDirty = 0;
}

void edSvBlk() {
    int sz = toBlock();
    char buf[24];
    sprintF(buf, "./block-%03d.4th", blkNum);
    push((CELL)buf);
    push(2);
    fOpen();
    msg = "-err:open-";
    CELL fh = pop();
    if (fh) {
        push((CELL)theBlock);
        push(sz);
        push(fh);
        fWrite();
        if (pop() != sz) { msg = "-err:sz-"; }
        else {
            msg = "-saved-";
            cur = isDirty = 0;
        }
        push(fh);
        fClose();
    }
}

void showFooter() {
    GotoXY(1, NUM_LINES);
    printString("- Block Editor v0.1 - ");
    printStringF("Block# %03d %c", blkNum, isDirty ? '*' : ' ');
    printStringF(" %s -\r\n", msg ? msg : "");
    printString("\r\n  (q)home (w)up (e)end (a)left (s)down (d)right (t)op (l)ast");
    printString("\r\n  (x)del char (r)eplace (i)nsert");
    printString("\r\n  (W)rite (L)reLoad (+)next (-)prev (Q)uit");
    printString("\r\n  (D)efine (C)ompile (I)nterp (A)sm (M)Comment");
    printString("\r\n->");
    ClearEOL();
}

void showEditor() {
    CursorOff();
    msg = NULL;
    for (int i = 0; i < NUM_LINES; i++) {
        showLine(i);
    }
    showCursor();
    GotoXY(1, NUM_LINES);
}

void lineEdit() {
    for (int i = cur; i < MAX_CUR; i++) { theBlock[i] = theBlock[i + 1]; }
}

void deleteChar() {
    for (int o = off; o < (LLEN - 2); o++) {
        edLines[line][o] = edLines[line][o + 1];
    }
    isDirty = 1;
    showLine(line);
    showCursor();
}

void insertChar(char c, int refresh) {
    for (int o = LLEN - 1; o > off; o--) {
        edLines[line][o] = edLines[line][o - 1];
    }
    SETC(c);
    mv(0, 1);
    isDirty = 1;
    if (refresh) {
        showLine(line);
        showCursor();
    }
}

void edType(int isInsert) {
    CursorOff();
    if (isInsert) { printString(" -insert-"); }
    else { printString(" -replace-"); }
    while (1) {
        byte c = getChar();
        if (c == 27) { return; }
        int isBS = ((c == 127) || (c == 8));
        if (isBS) {
            if (off) {
                --off;
                if (isInsert) { deleteChar(); }
                else { SETC(' '); }
            }
        } else {
            char ch = (c < ' ') ? ' ' : c;
            if (c == 13) { ch = FAKE_NL; }
            if (isInsert) { 
                insertChar(ch, 0);
                if (c == 13) {
                    toBlock();
                    toLines();
                    mv(1, -99);
                    showEditor();
                }
            } else {
                edSetCh(ch);
                showLine(line);
            }
        }
        showCursor();
        isDirty = 1;
    }
}

int processEditorChar(char c) {
    printChar(c);
    cur = (line * LLEN) + off;
    switch (c) {
    case 'Q': toBlock();  CursorOn();           return 0; // Quit
    case 9: mv(0, 8);                           break;    // simple <TAB>
    case 'h': mv(0, -1);                        break;    // left
    case 'j': mv(1, 0);                         break;    // up
    case 'k': mv(-1, 0);                        break;    // down
    case 'l': mv(0, 1);                         break;    // right
    case '_': mv(0, -99);                       break;    // home
    case '$': mv(0, 99);                        break;    // end
    case 'g': mv(-99, -99);                     break;    // top
    case 'G': mv(99, -99);                      break;    // bottom
    case 'i': edType(1);                        break;    // insert mode
    case 'r': edType(0);                        break;    // replace mode
    case 'x': deleteChar();                     break;    // delete char
    case 'L': edRdBlk(); showEditor();          break;    // ReLoad block
    case 'W': edSvBlk();                        break;    // Write block
    case 'n': edSetCh(0); mv(1, -99);           break;    // insert Newline
    case '+': if (isDirty) { edSvBlk(); }                 // Next block
            ++blkNum;
            edRdBlk();
            break;
    case '-': if (isDirty) { edSvBlk(); }                 // Previous block
            blkNum -= (blkNum) ? 1 : 0;
            edRdBlk();
            break;
    }
    return 1;
}

void doEditor() {
    line = 0;
    off = 0;
    blkNum = pop();
    if (0 <= blkNum) { edRdBlk(); }
    blkNum = (0 <= blkNum) ? blkNum : 0;
    CLS();
    showEditor();
    showFooter();
    while (processEditorChar(edGetChar())) {
        NormLO();
        showFooter();
    }
}
#endif