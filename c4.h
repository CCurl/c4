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

#define NAME_LEN 16

#ifdef _WIN32
  #define __BOARD__     PC
  #define __TARGET__    WINDOWS
  #include <Windows.h>
  #include <conio.h>
  #define CODE_SZ      ( 64*1024)
  #define VARS_SZ      (256*1024)
  #define STK_SZ        64
  #define LSTK_SZ       32
  #define LOCALS_SZ    160
  #define DICT_SZ     2000
  #define FLT_SZ        10
  #define __FILES__
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
  #define DICT_SZ     2000
  #define FLT_SZ        10
  #define __FILES__
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
  #define DICT_SZ     1000
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
  #define DICT_SZ     1000
  #define FLT_SZ        10
  #define __PIN__
  #define __FILES__
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
  #define DICT_SZ      200
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
  #define DICT_SZ      100
  #define FLT_SZ         8
  #define __PIN__
  #define NEEDS_ALIGN
// #define __GAMEPAD__
#endif

#define TOS           stks[sp]
#define NOS           stks[sp-1]
#define AOS           (byte*)TOS
#define CTOS          (char*)TOS
#define CNOS          (char*)NOS
#define L0            lstk[lsp]
#define L1            lstk[lsp-1]
#define L2            lstk[lsp-2]
#define FTOS          fstk[fsp]
#define FNOS          fstk[fsp-1]
#define FDROP         fsp-=(0<fsp)?1:0
#define DROP1         sp--
#define DROP2         sp-=2
#define CA(l)         (code+l)
#define DP_AT(l)      ((DICT_T *)(&code[l]))
#define BTW(x, a, b)  ((a<=x)&&(x<=b))
#define BA(a)         ((byte *)a)

typedef unsigned char byte;
typedef unsigned short WORD;
typedef long CELL;
typedef unsigned long UCELL;
typedef unsigned short USHORT;

#define CELL_SZ   sizeof(CELL)
#define CSZ       CELL_SZ
#define MEM_SZ    (CELL_SZ*3) + CODE_SZ + VARS_SZ + (DICT_SZ*sizeof(DICT_E)) + 8

typedef struct {
    USHORT xt;
    byte flags;
    byte len;
    char name[NAME_LEN];
} DICT_E;

#define BIT_IMMEDIATE 0x80

extern CELL BASE, STATE, tHERE, tVHERE, tempWords[10], sp;
extern CELL stks[], &HERE, &VHERE, &LAST;
extern byte *code, *vars, mem[];
extern DICT_E *dict;

extern void vmReset();
extern void systemWords();
extern void push(CELL);
extern CELL pop();
extern void printString(const char*);
extern void printStringF(const char*, ...);
extern void printChar(char);
extern void printBase(CELL, CELL);
extern void run(WORD);
extern int doFind(const char *);
extern void doParse(const char *);
extern void doWords();
extern void doOK();
extern byte *doUser(CELL, byte *);
extern char *rTrim(char *);
extern void doEditor();
extern int charAvailable();
extern int getChar();
extern CELL doTimer();
extern void doSleep();

// FILEs
extern byte *doFile(CELL, byte *);
extern void fileInit();
extern void fOpen();
extern void fGetC();
extern void fGetS();
extern void fWrite();
extern void fClose();
extern void fDelete();
extern void fList();
extern void fSave();
extern void fLoad();
extern int doLoad();

#endif // __C4_INCLUDED__
