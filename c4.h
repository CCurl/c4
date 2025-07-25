#ifndef __C4_H__

#define __C4_H__

#define VERSION   20250725

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_WARNINGS
  #define IS_WINDOWS 1
#else
  #define IS_LINUX   1
#endif

#define MEM_SZ          16*1024*1024
#define CODE_SLOTS    0xE000
#define STK_SZ          63  // Both data and return stacks
#define LSTK_SZ         60  // 3 cells per entry
#define TSTK_SZ         63  // A, B and T stacks
#define FSTK_SZ         15  // File stack
#define NAME_LEN        19  // DE-SZ = 2+1+1+LEN+1
#define _SYS_LOAD_
#define EDITOR

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#define btwi(n,l,h)   ((l<=n) && (n<=h))
#define _IMMED           1
#define _INLINE          2

#define WC_T          uint16_t
#define WC_SZ            2
#define NUM_BITS       0xE000
#define NUM_MASK       0x1FFF

#if INTPTR_MAX > INT32_MAX
    #define CELL_T        int64_t
    #define CELL_SZ       8
#else
    #define CELL_T        int32_t
    #define CELL_SZ       4
#endif

enum { COMPILE=1, DEFINE=2, INTERP=3, COMMENT=4 };
enum { DSPA=0, RSPA, LSPA, TSPA, ASPA, BSPA, HA, BA, SA, BLKA };

typedef CELL_T cell;
typedef WC_T wc_t;
typedef uint8_t byte;
typedef struct { wc_t xt; byte fl, ln; char nm[NAME_LEN+1]; } DE_T;
typedef struct { const char *name; wc_t op; byte fl; byte pad; } PRIM_T;

// These are defined by c4.c
extern void push(cell x);
extern cell pop();
extern void storeWC(wc_t a, wc_t v);
extern wc_t fetchWC(wc_t a);
extern void strCpy(char *d, const char *s);
extern int  strFind(const char *buf, const char *lookFor, int start);
extern int  strEq(const char *d, const char *s);
extern int  strEqI(const char *d, const char *s);
extern int  strLen(const char *s);
extern int  lower(const char c);
extern void zTypeF(const char *fmt, ...);
extern int  changeState(int x);
extern void inner(wc_t start);
extern void outer(const char *src);
extern void outerF(const char *fmt, ...);
extern void c4Init();
extern void ok();

// These are in the editor
extern void FG(int fg);
extern void Blue();
extern void Green();
extern void Purple();
extern void Red();
extern void White();
extern void Yellow();

// c4.c needs these to be defined
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
extern char *blockFn(int blk);
extern void blockLoad(int blk);
extern void blockLoadNext(int blk);
extern void sys_load();
extern void editBlock(cell blk);

#endif //  __C4_H__
