#include "c4.h"

cell inputFp, outputFp, fileStk[FSTK_SZ+1];
int fileSp;

void filePush(cell fh) { if (fileSp < FSTK_SZ) { fileStk[++fileSp] = fh; } }
cell filePop() { return (0<fileSp) ? fileStk[fileSp--]: 0; }
static char *blockFn(char *fn, int blk) { sprintf(fn, "block-%03d.c4", blk); return fn; }

#ifdef NO_FILE  // -----------------------------------------------------
void fileInit() { fileSp = 0; inputFp = outputFp = 0; }
void fileClose(cell fh) {}
#endif

#ifdef PC_FILE  // -----------------------------------------------------
static char fn[32];
void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(const char *name, const char *mode) { return (cell)fopen(name, mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }
void fileDelete(const char *name) { remove(name); }
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }
void blockLoad(int blk) { fileLoad(blockFn(fn, blk)); }

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

#endif

#ifdef PICO_FILE  // -----------------------------------------------------
// Support for LittleFS on the Pico
#include <LittleFS.h>

#define myFS LittleFS

#define NFILES 16
File files[NFILES+1];

void fileInit() {
	zType("\r\nFileInit: begin ...");
	myFS.begin();
	zType(" LittleFS initialized");
    FSInfo64 fsinfo;
    LittleFS.info64(fsinfo);
	zTypeF("\r\nBytes - total: %llu, used: %llu", fsinfo.totalBytes, fsinfo.usedBytes);
	fileSp = 0; inputFp = outputFp = 0;
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

void fileClose(cell fh) {
	if (btwi(fh, 1, NFILES) &&((bool)files[fh])) {
		files[fh].close();
	}
}

void fileDelete(const char *name) {
    myFS.remove((char*)name);
}

cell fileRead(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].read((uint8_t*)buf, sz); }
	return  0;
}

cell fileWrite(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].write((uint8_t*)buf, sz); }
	return  0;
}

int fileGets(char *buf, int sz, cell fh) {
	int n = 0;
	if (btwi(fh, 1, NFILES) && (0 < files[fh].available())) {
		n = files[fh].readBytesUntil(10, buf, sz);
	}
    buf[n] = 0;
	return n;
}

void fileLoad(const char *name) {
    cell fh = fileOpen(name, "r");
    if (fh == 0) { zTypeF("-[%s] not found-", name); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}

static char fn[32];
void blockLoad(int blk) { fileLoad(blockFn(fn, blk)); }

#endif // PICO_FILE

#ifdef TEENSY_FILE  // -----------------------------------------------------
// Support for LittleFS on the Teensy 4.x
#include <LittleFS.h>

LittleFS_Program myFS;

#define NFILES 16
File files[NFILES+1];

void fileInit() {
	zType("\r\nFileInit: begin ...");
	myFS.begin(1024*1024);
	zType(" LittleFS initialized");
	zTypeF("\r\nBytes - total: %llu, used: %llu", myFS.totalSize(), myFS.usedSize());
	fileSp = 0; inputFp = outputFp = 0;
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
		files[fh] = myFS.open((char*)fn, mode[0]=='w' ? FILE_WRITE_BEGIN : FILE_READ);
        if (files[fh].name() == nullptr) { return 0; }
        if (mode[0] == 'w') { files[fh].truncate(0); }
	}
	return fh;
}

void fileClose(cell fh) {
	if (btwi(fh, 1, NFILES) &&((bool)files[fh])) {
		files[fh].close();
	}
}

void fileDelete(const char *name) {
    myFS.remove((char*)name);
}

cell fileRead(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].read((uint8_t*)buf, sz); }
	return  0;
}

cell fileWrite(char *buf, int sz, cell fh) {
	if (btwi(fh, 1, NFILES)) { return files[fh].write((uint8_t*)buf, sz); }
	return  0;
}

int fileGets(char *buf, int sz, cell fh) {
	int n = 0;
	if (btwi(fh, 1, NFILES) && (0 < files[fh].available())) {
		n = files[fh].readBytesUntil(10, buf, sz);
	}
    buf[n] = 0;
	return n;
}

void fileLoad(const char *name) {
    cell fh = fileOpen(name, "r");
    if (fh == 0) { zTypeF("-[%s] not found-", name); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}

static char fn[32];
void blockLoad(int blk) { fileLoad(blockFn(fn, blk)); }
#endif // TEENSY_FILE
