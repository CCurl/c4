#ifndef __C4_INCLUDED__
#define __C4_INCLUDED__

#define PC           1
#define TEENSY4      2
#define XIAO         3
#define ESP32_DEV    4
#define ESP8266      5
#define APPLE_MAC    6
#define PICO         7
#define LEO          8
#define LINUX      100
#define WINDOWS    101

#define _FLT_       float

#ifdef _WIN32
    #define __BOARD__     PC
    #define __TARGET__    WINDOWS
    #include <Windows.h>
    #include <conio.h>
    #define CODE_SZ      ( 64*1024)
    #define VARS_SZ      (256*1024)
    #define STK_SZ        32
    #define LSTK_SZ       32
    #define LOCALS_SZ    160
    #define FLT_SZ        10
    #define __FILES__
    #define __EDITOR__
    // #define USE_ACCEPT
#endif

#ifdef _LINUX
    #define __BOARD__     PC
    #define __TARGET__    LINUX
    #include <unistd.h>
    #include <termios.h>
    #define CODE_SZ      ( 64*1024)
    #define VARS_SZ      (256*1024)
    #define STK_SZ        64
    #define LSTK_SZ       32
    #define LOCALS_SZ    160
    #define FLT_SZ        10
    #define __FILES__
    #if __LONG_MAX__ > __INT32_MAX__
        #undef  _FLT_
        #define _FLT_       double
    #endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifndef __BOARD__
    #define __BOARD__    TEENSY4
#endif

#if __BOARD__ == TEENSY4
    #define CODE_SZ      (48*1024)
    #define VARS_SZ      (96*1024)
    #define STK_SZ        64
    #define LSTK_SZ       32
    #define LOCALS_SZ    160
    #define FLT_SZ        10
    #define __PIN__
    #define __FILES__
    // #define __EDITOR__
    #define NEEDS_ALIGN
#elif __BOARD__ == PICO
    #define CODE_SZ      (48*1024)
    #define VARS_SZ      (96*1024)
    #define STK_SZ        64
    #define LSTK_SZ       32
    #define LOCALS_SZ    160
    #define FLT_SZ        10
    #define __PIN__
    // #define __FILES__
    // #define __EDITOR__
    #define NEEDS_ALIGN
#elif __BOARD__ == XIAO
    #undef NAME_LEN
    #define NAME_LEN      12
    #define CODE_SZ      (10*1024)
    #define VARS_SZ      (10*1024)
    #define STK_SZ        32
    #define LSTK_SZ       16
    #define LOCALS_SZ     80
    #define FLT_SZ         8
    #define __PIN__
    #define NEEDS_ALIGN
    // #define __GAMEPAD__
#elif __BOARD__ == ESP8266
    #undef NAME_LEN
    #define NAME_LEN      12
    #define CODE_SZ      (14*1024)
    #define VARS_SZ      (12*1024)
    #define STK_SZ        32
    #define LSTK_SZ       16
    #define LOCALS_SZ     80
    #define FLT_SZ         8
    #define __PIN__
    #define NEEDS_ALIGN
    // #define __GAMEPAD__
#endif

#define TOS           stk.i[sp]
#define NOS           stk.i[sp-1]
#define BTOS          (byte*)TOS
#define CTOS          (char*)TOS
#define CNOS          (char*)NOS
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]
#define FTOS          stk.f[sp]
#define FNOS          stk.f[sp-1]
#define DROP1         sp--
#define DROP2         sp-=2
#define DROP3         sp-=3
#define CA(l)         (code+l)
#define BTW(x, a, b)  ((a<=x)&&(x<=b))
#define BA(a)         ((byte *)a)
#define HERE          st.cells[0]
#define VHERE         st.cells[1]
#define LAST          st.cells[2]
#define MEM           st.bytes
#define BIT_IMMEDIATE 0x80

typedef unsigned char byte;
typedef unsigned short WORD;
typedef long CELL;
typedef unsigned long UCELL;
typedef unsigned short USHORT;
typedef _FLT_ FLT_T;

#define CELL_SZ   sizeof(CELL)
#define CSZ       CELL_SZ
#define MEM_SZ    CODE_SZ + VARS_SZ

// NB: we want sizeof(DICT_E) to be a mutiple of 4
// If not, it can be a problem on AVR dev-boards
// PCs don't care.
#define NAME_LEN    15

typedef struct {
    USHORT xt;
    byte flags;
    byte lexicon;
    byte len;
    char name[NAME_LEN];
} DICT_E;

#define DICT_SZ    sizeof(DICT_E)

typedef union { CELL cells[MEM_SZ / CELL_SZ]; byte bytes[MEM_SZ+8]; } ST_T;
typedef union { FLT_T f[STK_SZ + 1]; CELL i[STK_SZ + 1]; } STK_T;

extern CELL BASE, STATE, tHERE, tVHERE, tempWords[10], sp;
extern byte *code, *vars;
extern DICT_E *dict;
extern ST_T st;
extern STK_T stk;

extern void vmReset();
extern void push(CELL v);
extern CELL pop();
extern void systemWords();
extern void printString(const char*);
extern void printStringF(const char*, ...);
extern void printChar(char);
extern void printBase(CELL, CELL);
extern void run(WORD);
extern int  doFind(const char *);
extern void doParse(const char *);
extern void doWords();
extern void doOK();
extern byte *doUser(CELL, byte *);
extern char *rTrim(char *);
extern void doEditor();
extern int  charAvailable();
extern int  isBackSpace(int c);
extern int  getChar();
extern CELL doTimer();
extern void doSleep();

// FILEs - the c4 FILE API
extern void fileInit();
extern void fOpen();     // (fn md--fh) -- md: 0=READ, else WRITE
extern void fClose();    // (fh--)
extern void fRead();     // (a sz fh--n)
extern void fWrite();    // (a sz fh--n)
extern void fGetC();     // (fh--c n)
extern void fPutC();     // (c fh--)
extern void fGetS();     // (a sz fh--n)
extern void fDelete();   // (fn--)
extern void fSeek();     // (offset whence fh--)
extern void fTell();     // (fh--n)
extern void fList();     // (--)
extern void fSaveSys();  // (--)
extern int  fLoadSys();  // (--)
extern void fLoad(const char *fn);

#endif // __C4_INCLUDED__
