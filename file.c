#include "c4.h"

// #define NO_FILE
// #define PC_FILE
// #define PICO_FILE
// #define TEENSY_FILE

cell inputFp, outputFp, fileStk[FSTK_SZ+1];
int fileSp;

void filePush(cell fh) { if (fileSp < FSTK_SZ) { fileStk[++fileSp] = fh; } }
cell filePop() { return (0<fileSp) ? fileStk[fileSp--]: 0; }

#ifdef NO_FILE
void fileInit() { fileSp = 0; }
cell fileOpen(cell name, cell mode) { return 0; }
void fileClose(cell fh) {}
cell fileRead(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
cell fileWrite(char *buf, int sz, cell fh) { return 0; }
int fileGets(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
void fileLoad(cell name) {}
void blockLoad(cell blk) {}
#endif

#ifdef PC_FILE
static char fn[32];
void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(char *name, char *mode) { return (cell)fopen(name+1, mode+1); }
void fileClose(cell fh) { fclose((FILE*)fh); }
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }

int  fileGets(char *buf, int sz, cell fh) {
    buf[0] = 0; // length
    buf[1] = 0; // NULL terminator
    if (fh == 0) { fh = (cell)stdin; }
    if (fgets(buf+1, sz, (FILE*)fh) != buf+1) return 0;
    buf[0] = strLen(buf+1);
    return 1;
}

void fileLoad(char *name) {
    cell fh = fileOpen(name, " rt");
    if (fh == 0) { printf("-not found-"); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}

static char *blockFn(int blk) { sprintf(fn, " block-%03d.c4", blk); return fn; }
void blockLoad(int blk) { fileLoad(blockFn(blk)); }

#endif

#ifdef PICO_FILE
void fileInit() { fileSp = 0; }
cell fileOpen(cell name, cell mode) { return 0; }
void fileClose(cell fh) {}
cell fileRead(char* buf, int sz, cell fh) { buf[0] = 0; return 0; }
cell fileWrite(char* buf, int sz, cell fh) { return 0; }
int fileGets(char* buf, int sz, cell fh) { buf[0] = 0; return 0; }
void fileLoad(cell name) {}
void blockLoad(cell blk) {}
#endif

#ifdef TEENSY_FILE
void fileInit() { fileSp = 0; }
cell fileOpen(cell name, cell mode) { return 0; }
void fileClose(cell fh) {}
cell fileRead(char* buf, int sz, cell fh) { buf[0] = 0; return 0; }
cell fileWrite(char* buf, int sz, cell fh) { return 0; }
int fileGets(char* buf, int sz, cell fh) { buf[0] = 0; return 0; }
void fileLoad(cell name) {}
void blockLoad(cell blk) {}
#endif
