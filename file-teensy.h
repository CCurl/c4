// file-teensy.h
// File ops on the Teensy are different
#include <LittleFS.h>

LittleFS_Program myFS;

#define NF 10
#define VALIDF(x) BTW(x,1,NF) && (files[x])

static File files[NF+1];
static int isInit = 0;

void fileInit() {
    myFS.begin(1 * 1024 * 1024);
    printString("\r\nLittleFS: initialized");
    printStringF("\r\nTotal Size: %llu bytes, Used: %llu", myFS.totalSize(), myFS.usedSize());
    for (int i = 0; i <= NF; i++) { files[i] = 0; }
    isInit = 1;
}

int openSlot() {
    if (!isInit) { fileInit(); }
    for (int i = 1; i <= NF; i++) {
        if (files[i] == 0) { return i; }
    }
    return 0;
}

// (name mode--fh)
void fOpen() {
    CELL mode = pop();
    char *fn = (char*)pop();
    push(0);
    int i = openSlot();
    if (i) {
        files[i] = myFS.open(fn, (mode) ? FILE_WRITE : FILE_READ);
        if (files[i]) {
          pop();
          push(i);
        }
    }
}

// (fh--)
void fClose() {
    CELL fh = pop();
    if (VALIDF(fh)) { 
        files[fh].close();
        files[fh] = 0;
    }
}

// (a sz fh--n)
void fRead() {
    CELL fh = pop();
    CELL sz = pop();
    char *a = (char*)pop();
    CELL n = 0;
    if (VALIDF(fh)) { n=files[fh].read(a, sz); }
    push(n);
}

// (a sz fh--n)
void fWrite() {
    CELL fh = pop();
    CELL sz = pop();
    char *a = (char*)pop();
    CELL n = 0;
    if (VALIDF(fh)) { n = files[fh].write(a, sz); }
    push(n);
}

// (fh--c n)
void fGetC() {
    CELL c = 0, n = 0, fh = pop(); 
    if (VALIDF(fh)) {
      n = files[fh].read((char*)&c, 1);
    }
    push(c);
    push(n);
}

// (c fh--)
void fPutC() {
    CELL fh = pop();
    byte c = (byte)pop();
    if (VALIDF(fh)) { files[fh].write(&c, 1); }
}

// (a sz fh--f)
void fGetS() {
    CELL fh = pop();
    CELL sz = pop();
    char *a = (char *)pop();
    *a = 0;
    if (VALIDF(fh) && (files[fh].available())) {
        files[fh].readBytesUntil('\n', a, sz);
        push(1);
        return;
    }
    push(0);
}

// ( offset whence fh-- )
void fSeek() {
    CELL fh = pop();
    CELL whence = pop();
    CELL offset = pop();
    // TODO: Fill this in
    printString("-todo:fSeek-");
    if (VALIDF(fh)) { /* files[fh].write(&c, 1); */ }
}

// (fh--n)
void fTell() {
    CELL fh = pop();
    // TODO: Fill this in
    printString("-todo:fTell-");
    if (VALIDF(fh)) { /* files[fh].write(&c, 1); */ }
    push(0);
}

// (fn--)
void fDelete() {
    char* fn = (char*)pop();
    if (myFS.remove(fn)) { printString("-deleted-"); }
    else { printString("-noFile-"); }
}

// (--)
void fList() {
    File dir = myFS.open("/");
    while(true) {
         File entry = dir.openNextFile();
         if (!entry) { break; }
         printString(entry.name());
         // files have sizes, directories do not
         if (entry.isDirectory()) { printStringF(" (dir)\r\n"); }
         else { printStringF(" (%llu)\r\n", entry.size()); }
        entry.close();
    }
    dir.close();
}

// (--)
void fSaveSys() {
    myFS.remove("/system.c4");
    File fp = myFS.open("/system.c4", FILE_WRITE);
    if (fp) {
        fp.write(&mem[0], MEM_SZ);
        fp.close();
        printString("-saved-");
    } else { printString("-error-"); }
}

// (--)
int fLoadSys() {
    File fp = myFS.open("/system.c4", FILE_READ);
    if (fp) {
        vmReset();
        fp.read(&mem[0], MEM_SZ);
        fp.close();
        printString("-loaded-");
    } else { printString("-error-"); }
    return (fp) ? 1 : 0;
}

// (--)
void fLoad(const char *fn) {
    // TODO: Fill this in
    printString("-todo:load-");
}
