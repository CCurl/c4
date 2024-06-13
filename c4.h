#ifndef __C4_H__

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define IS_WINDOWS 1
#define IS_PC      1
#define PC_FILE
#endif

#ifdef IS_LINUX
#define IS_PC      1
#define PC_FILE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#define VERSION       240601
#define CODE_SZ       0xFFFF
#define VARS_SZ       0xFFFF
#define DICT_SZ       0xFFFF
#define STK_SZ            63
#define RSTK_SZ           63
#define LSTK_SZ           60
#define FSTK_SZ           10
#define btwi(n,l,h)   ((l<=n) && (n<=h))

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

// These are defined by c4.c

extern int strLen(const char *s);
extern void printF(const char *fmt, ...);
extern void Init();

// c4.c needs these
extern cell inputFp, outputFp;
extern void fileInit();
extern void filePush(cell fh);
extern cell filePop();
extern cell fileOpen(char *name, char *mode);
extern void fileClose(cell fh);
extern cell fileRead(char *buf, int sz, cell fh);
extern cell fileWrite(char *buf, int sz, cell fh);
extern int  fileGets(char *buf, int sz, cell fh);
extern void fileLoad(char *name);
extern void blockLoad(int blk);

extern void zType(const char *str);
extern void emit(const char ch);
extern void ttyMode(int isRaw);
extern int  key();
extern int  qKey();

#endif //  __C4_H__
