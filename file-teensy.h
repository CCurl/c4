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
    for (int i = 1; i <= NF; i++)
    if (files[i] == 0) { return i; }
    return 0;
}

void fOpen() {               // (name mode--fh)
    CELL mode = pop();
    char *fn = (char*)TOS;
    TOS = 0;
    int i = openSlot();
    if (i) {
        files[i] = myFS.open(fn, (mode) ? FILE_WRITE : FILE_READ);
        if (files[i]) { TOS = i; }
    }
}

void fGetC() {               // (fh--c n)
    byte c;
    CELL fh = TOS; 
    push(0);
    if (VALIDF(fh)) {
        TOS = files[fh].read(&c, 1);
        NOS = (CELL)c;
    }
}

void fGetS() {               // (a sz fh--f)
    CELL fh = pop();
    CELL sz = pop();
    char *a = (char *)TOS;
    TOS = 0;
    *a = 0;
    if (VALIDF(fh) && (files[fh].available())) {
        files[fh].readBytesUntil('\n', a, sz);
        TOS = 1;
    }
}

void fWrite() {              // (c fh--)
    CELL fh = pop();
    byte c = (byte)pop();
    if (VALIDF(fh)) { files[fh].write(&c, 1); }
}

void fClose() {              // (fh--)
    CELL fh = pop();
    if (VALIDF(fh)) { 
        files[fh].close();
        files[fh] = 0;
    }
}

void fSave() {
    myFS.remove("/system.ccc");
    File fp = myFS.open("/system.ccc", FILE_WRITE);
    if (fp) {
        fp.write(&mem[0], MEM_SZ);
        fp.close();
        printString("-saved-");
    } else { printString("-error-"); }
}

void fLoad() {
    File fp = myFS.open("/system.ccc", FILE_READ);
    if (fp) {
        vmReset();
        fp.read(&mem[0], MEM_SZ);
        fp.close();
        printString("-loaded-");
    } else { printString("-error-"); }
}

int doLoad() {
      printString("-load-");
      return 0;
}

void fDelete() {
    char* fn = (char*)pop();
    if (myFS.remove(fn)) { printString("-deleted-"); }
    else { printString("-noFile-"); }
}

void fList() {
    File dir = myFS.open("/");
    while(true) {
         File entry = dir.openNextFile();
         if (!entry) { break; }
         printString(entry.name());
         // files have sizes, directories do not
         if (entry.isDirectory()) { printStringF(" (dir)\r\n"); }
         else { printStringF(" (%llu)\r\n", entry.size()); }
         //char *x = entry.size();
         // fprintStringF("%d", (int)x);
      entry.close();
    }
    dir.close();
}
