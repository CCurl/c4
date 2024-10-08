#include "c4.h"

cell inputFp, outputFp, fileStk[FSTK_SZ+1];
int fileSp;

void filePush(cell fh) { if (fileSp < FSTK_SZ) { fileStk[++fileSp] = fh; } }
cell filePop() { return (0<fileSp) ? fileStk[fileSp--]: 0; }
static char *blockFn(char *fn, int blk) { sprintf(fn, "block-%03d.f", blk); return fn; }
static char fn[32];

void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(const char *name, const char *mode) { return (cell)fopen(name, mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }
void fileDelete(const char *name) { remove(name); }
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }
void blockLoad(int blk) { fileLoad(blockFn(fn, blk)); }

void blockLoadNext(int blk) {
    if (inputFp) { fileClose(inputFp); inputFp = 0; }
    fileLoad(blockFn(fn, blk));
    if (inputFp == 0) { inputFp = filePop(); }
}

int fileGets(char *buf, int sz, cell fh) {
    buf[0] = 0;
    if (fh == 0) { fh = (cell)stdin; }
    if (fgets(buf, sz, (FILE*)fh) != buf) return 0;
    return strLen(buf);
}

void fileLoad(const char *name) {
    cell fh = fileOpen(name, "rt");
    if (fh == 0) { zTypeF("-[%s] not found-", name); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}
