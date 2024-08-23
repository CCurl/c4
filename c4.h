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

#define VERSION   20240822

#define MAX_CODE      0x00FFFF    // 0x80000000 and above are numbers
#define MAX_VARS      0x100000
#define MAX_DICT         999
#define MAX_BLOCK       1023
#define NUM_BLOCKS      1024
#define DISK_SZ     NUM_BLOCKS*(MAX_BLOCK+1)
#define STK_SZ            63
#define RSTK_SZ           63
#define LSTK_SZ           60
#define TSTK_SZ           63
#define btwi(n,l,h)   ((l<=n) && (n<=h))

#if INTPTR_MAX > INT32_MAX
    #define CELL_T    int64_t
    #define UCELL_T   uint64_t
    #define CELL_SZ   8
    #define FLT_T     double
    #define addrFmt   ": %s $%llx ;"
    #define VAL_MASK  0x4000000000000000
    #define VAL_MAX   0x3FFFFFFFFFFFFFFF
#else
    #define CELL_T    int32_t
    #define UCELL_T   uint32_t
    #define CELL_SZ   4
    #define FLT_T     float
    #define addrFmt   ": %s $%lx ;"
    #define VAL_MASK  0x08000000
    #define VAL_MAX   0x07FFFFFF
#endif

#define DE_SZ     32
#define NAME_LEN  (DE_SZ-(CELL_SZ+2))

typedef CELL_T cell;
typedef UCELL_T ucell;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef struct { cell xt; byte fl; byte ln; char nm[NAME_LEN]; } DE_T;
typedef struct { short op; const char* name; byte fl; } PRIM_T;

// These are defined by c4.cpp
extern cell inputFp, outputFp;
extern char disk[DISK_SZ];
extern void push(cell x);
extern cell pop();
extern void strCpy(char *d, const char *s);
extern int  strEq(const char *d, const char *s);
extern int  strEqI(const char *d, const char *s);
extern int  strLen(const char *s);
extern int  lower(const char c);
extern char *getTIB(int sz);
extern void zTypeF(const char *fmt, ...);
extern void inner(cell start);
extern int  outer(const char *src);
extern void outerF(const char *fmt, ...);
extern void Init();

// c4.cpp needs these to be defined
extern void zType(const char *str);
extern void emit(const char ch);
extern void ttyMode(int isRaw);
extern int  key();
extern int  qKey();
extern cell timer();
extern cell fileOpen(const char *name, const char *mode);
extern void fileClose(cell fh);
extern void fileDelete(const char *name);
extern cell fileRead(char *buf, cell sz, cell fh);
extern cell fileWrite(char *buf, cell sz, cell fh);

#endif //  __C4_H__
