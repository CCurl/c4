#include "c4.h"

// Support for files
cell inputFp, outputFp, fileStk[FSTK_SZ + 1];
static char fn[32];
int fileSp;

void filePush(cell fh) { if (fileSp < FSTK_SZ) { fileStk[++fileSp] = fh; } }
cell filePop() { return (0 < fileSp) ? fileStk[fileSp--] : 0; }
char *blockFn(cell blk) { sprintf(fn, "block-%03d.fth", (int)blk); return fn; }

void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(const char *name, const char *mode) { return (cell)fopen(name, mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }
void fileDelete(const char *name) { remove(name); }
cell fileRead(char *buf, cell sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, cell sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }
void blockLoad(cell blk) { fileLoad(blockFn(blk)); }

void blockLoadNext(cell blk) {
    if (inputFp) { fileClose(inputFp); inputFp = 0; }
    fileLoad(blockFn(blk));
    if (inputFp == 0) { inputFp = filePop(); }
}

int fileGets(char *buf, cell sz, cell fh) {
    buf[0] = 0;
    if (fh == 0) { fh = (cell)stdin; }
    if (fgets(buf, (int)sz, (FILE*)fh) != buf) return 0;
    return strLen(buf);
}

void fileLoad(const char *name) {
    cell fh = fileOpen(name, "rt");
    if (fh == 0) { zTypeF("-[%s] not found-", name); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}
