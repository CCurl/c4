#include "c4.h"

cell inputFp, outputFp, fileStk[FSTK_SZ+1];
int fileSp;

void filePush(cell fh) { if (fileSp < FSTK_SZ) { fileStk[++fileSp] = fh; } }
cell filePop() { return (0<fileSp) ? fileStk[fileSp--]: 0; }

#ifdef NO_FILE  // -----------------------------------------------------
void fileInit() { fileSp = 0; inputFp = outputFp = 0; }
cell fileOpen(const char *name, const char *mode) { return 0; }
void fileClose(cell fh) {}
void fileDelete(const char *name) {}
cell fileRead(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
cell fileWrite(char *buf, int sz, cell fh) { return 0; }
int fileGets(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
void fileLoad(const char *name) {}
void blockLoad(int blk) {}
#endif

#ifdef PC_FILE  // -----------------------------------------------------
void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(const char *name, const char *mode) { return (cell)fopen(name, mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }
void fileDelete(const char *name) {}
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }

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

static char fn[32];
static char *blockFn(int blk) { sprintf(fn, "block-%03d.c4", blk); return fn; }
void blockLoad(int blk) { fileLoad(blockFn(blk)); }

#endif

#ifdef PICO_FILE  // -----------------------------------------------------
// Support for LittleFS on the Pico
#include <LittleFS.h>

#define myFS LittleFS

#define NFILES 16
File files[NFILES+1];

void fileInit() {
	zType("\r\nLittleFS: begin ...");
	myFS.begin();
	zType("done.");
    FSInfo64 fsinfo;
    LittleFS.info64(fsinfo);
	zType("\r\nLittleFS: initialized");
	zTypeF("\r\nBytes total: %llu, used: %llu", fsinfo.totalBytes, fsinfo.usedBytes);
	fileSp = 0; inputFp = 0;
	for (int i = 0; i <= NFILES; i++) { files[i] = File(); }
}

int freeFile() {
	for (int i = 1; i <= NFILES; i++) {
		if ((bool)files[i] == false) { return i; }
	}
	return 0;
}

cell fileOpen(const char *fn, const char *mode) {
	int fh = freeFile();
	if (0 < fh) {
		files[fh] = myFS.open((char*)fn, mode);
        if (files[fh].name() == nullptr) { return 0; }
        if (mode[0] == 'w') { files[fh].truncate(0); }
	}
	return fh;
}

// fileClose - (fh--)
void fileClose(cell fh) {
	if (btwi(fh, 1, NFILES) &&((bool)files[fh])) {
		files[fh].close();
	}
}

// fileDelete - (name--)
void fileDelete(const char *name) {
    myFS.remove((char*)name);
}

// fileRead - (buf sz fh--n)
// buf: buffer, sz: number of bytes to read, fh: file handle, n: num chars read
// n=0: End of file or file error
cell fileRead(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].read((uint8_t*)buf, sz); }
	return  0;
}

// fileWrite - (buf sz fh--n)
// buf: buffer, sz: number of bytes to write, fh: file handle, n: num chars written
// n=0: File not open or error
cell fileWrite(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].write((uint8_t*)buf, sz); }
	return  0;
}

// fileGets - (buf sz fh--n)
// buf: buffer, sz: number of bytes to read, fh: file handle, n: num chars read
// n=0: File not open or error
// n=1: empty line (a single LineFeed char)
int fileGets(char *buf, int sz, cell fh) {
	int n = 0;
	if (btwi(fh, 1, NFILES) && (0 < files[fh].available())) {
		n = files[fh].readBytesUntil(10, buf, sz);
	}
    buf[n] = 0;
	return n;
}

// fileLoad - (name--)
// fh: File handle, sz: number of bytes to write, n: num chars written
// n=0: File not open or error
void fileLoad(const char *name) {
    cell fh = fileOpen(name, "r");
    if (fh == 0) { zTypeF("-[%s] not found-", name); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}

// blockLoad - (blk--)
// blk: block number
static char fn[32];
static char *blockFn(int blk) { sprintf(fn, "block-%03d.c4", blk); return fn; }
void blockLoad(int blk) { fileLoad(blockFn(blk)); }

#endif // PICO_FILE

#ifdef TEENSY_FILE  // -----------------------------------------------------
void fileInit() { fileSp = 0; }
cell fileOpen(char *name, char *mode) { return 0; }
void fileClose(cell fh) {}
void fileDelete(const char *name) {}
cell fileRead(char *buf, int sz, cell fh) { buf[0] = 0; return 0; }
cell fileWrite(char *buf, int sz, cell fh) { return 0; }
int fileGets(char *buf, int sz, cell fh) { buf[0] = 0; return 0; }
void fileLoad(char *name) {}
void blockLoad(int blk) {}
#endif // TEENSY_FILE
