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

#define VERSION   240831

// For 32-bit word-codes, use these
#define WC_T           uint32_t
#define WC_SZ             4
#define NUM_BITS        0xE0000000
#define NUM_MASK        0x1FFFFFFF
#define CODE_SZ         0x00020000
#define DICT_SZ        (10000*sizeof(DE_T))

// For 16-bit word-codes, use these
// #define WC_T           uint16_t
// #define WC_SZ           2
// #define NUM_BITS       0xE000
// #define NUM_MASK       0x1FFF
// #define CODE_SZ        0xDFFF
// #define DICT_SZ       (2500*sizeof(DE_T))

#define VARS_SZ         4*1024*1024
#define STK_SZ            63
#define RSTK_SZ           63
#define LSTK_SZ           60
#define TSTK_SZ           63
#define FSTK_SZ           15
#define NAME_LEN      (32-(WC_SZ+2))
#define btwi(n,l,h)   ((l<=n) && (n<=h))

#if INTPTR_MAX > INT32_MAX
    #define CELL_T    int64_t
    #define UCELL_T   uint64_t
    #define CELL_SZ   8
    #define addrFmt ": %s $%llx ;"
#else
    #define CELL_T    int32_t
    #define UCELL_T   uint32_t
    #define CELL_SZ   4
    #define addrFmt ": %s $%lx ;"
#endif

typedef CELL_T cell;
typedef UCELL_T ucell;
typedef WC_T wc_t;
typedef unsigned char byte;
typedef struct { wc_t xt; byte fl, ln; char nm[NAME_LEN]; } DE_T;
typedef struct { wc_t op; const char *name; byte fl; } PRIM_T;

// These are defined by c4.cpp
extern void push(cell x);
extern cell pop();
extern void strCpy(char *d, const char *s);
extern int  strEq(const char *d, const char *s);
extern int  strEqI(const char *d, const char *s);
extern int  strLen(const char *s);
extern int  lower(const char c);
extern void zTypeF(const char *fmt, ...);
extern void inner(wc_t start);
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
extern void fileDelete(const char *name);
extern cell fileRead(char *buf, int sz, cell fh);
extern cell fileWrite(char *buf, int sz, cell fh);
extern int  fileGets(char *buf, int sz, cell fh);
extern void fileLoad(const char *name);
extern void blockLoad(int blk);
extern void sys_load();

#endif //  __C4_H__
