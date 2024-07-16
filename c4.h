#ifndef __C4_H__

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define IS_WINDOWS 1
#define IS_PC      1
#endif

#ifdef __linux__
#define IS_LINUX   1
#define IS_PC      1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#define VERSION       240716

#ifdef IS_PC
    #define CODE_SZ       0xDFFF    // 0xE000 and above are numbers
    #define VARS_SZ     0x100000
    #define DICT_SZ       0xFFFD
    #define STK_SZ            63
    #define RSTK_SZ           63
    #define LSTK_SZ           60
    #define TSTK_SZ           63
    #define FSTK_SZ           15
    #define REGS_SZ          255
    #define btwi(n,l,h)   ((l<=n) && (n<=h))
    #define PC_FILE
#endif // IS_PC

#ifndef IS_PC
    // Must be a dev board ...
    #define CODE_SZ       0xDFFF    // 0xE000 and above are numbers
    #define VARS_SZ      0x10000
    #define DICT_SZ       0x4000
    #define STK_SZ            63
    #define RSTK_SZ           63
    #define LSTK_SZ           60
    #define TSTK_SZ           63
    #define FSTK_SZ           15
    #define REGS_SZ          200
    #define btwi(n,l,h)   ((l<=n) && (n<=h))
    #define NO_FILE
#endif // IS_PC

#if INTPTR_MAX > INT32_MAX
    #define CELL_T    int64_t
    #define UCELL_T   uint64_t
    #define CELL_SZ   8
    #define FLT_T     double
    #define addrFmt ": %s $%llx ;"
#else
    #define CELL_T    int32_t
    #define UCELL_T   uint32_t
    #define CELL_SZ   4
    #define FLT_T     float
    #define addrFmt ": %s $%lx ;"
#endif

typedef CELL_T cell;
typedef UCELL_T ucell;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef union { FLT_T f; cell i; } SE_T;
typedef struct { ushort xt; byte sz, fl, lx, ln; char nm[32]; } DE_T;
typedef struct { short op; const char* name; byte fl; } PRIM_T;

// These are defined by c4.cpp
extern void push(cell x);
extern cell pop();
extern void strCpy(char *d, const char *s);
extern int  strEq(const char *d, const char *s);
extern int  strEqI(const char *d, const char *s);
extern int  strLen(const char *s);
extern int  lower(const char c);
extern char *iToA(cell N, int b);
extern void zTypeF(const char *fmt, ...);
extern void inner(ushort start);
extern int  outer(const char *src);
extern void outerF(const char *fmt, ...);
extern void Init();

// c4.cpp needs these to be defined
extern cell inputFp, outputFp;
extern void zType(const char *str);
extern void emit(const char ch);
extern void ttyMode(int isRaw);
extern int  key();
extern int  qKey();
extern cell timer();
extern void fileInit();
extern void filePush(cell fh);
extern cell filePop();
extern cell fileOpen(const char *name, const char *mode);
extern void fileClose(cell fh);
extern cell fileRead(char *buf, int sz, cell fh);
extern cell fileWrite(char *buf, int sz, cell fh);
extern int  fileGets(char *buf, int sz, cell fh);
extern void fileLoad(const char *name);
extern void blockLoad(int blk);

#endif //  __C4_H__
