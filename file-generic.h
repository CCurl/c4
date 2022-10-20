// File support for generic boards
// File ops on the Teensy are different

#include <FS.h>
#include <LittleFS.h>

#define myFS LittleFS

#define NF 10
#define VALIDF(x) BTW(x,1,NF) && (files[x])

static File *files[NF+1];
static int isInit = 0;

void fileInit() {
    myFS.begin();
    FSInfo fs_info;
    myFS.info(fs_info);
    printString("\r\nLittleFS: initialized");
    printStringF("\r\nTotal Size: %ld bytes, Used: %ld", fs_info.totalBytes, fs_info.usedBytes);
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
    char* fn = (char*)TOS;
    TOS = 0;
    int i = openSlot();
    if (i) {
        File f = myFS.open(fn, (mode) ? "w" : "r");
        if (f) {
            files[i] = &f;
            TOS = i;
        }
    }
}

void fGetC() {               // (fh--c f)
    byte c;
    CELL fh = TOS;
    push(0);
    if (VALIDF(fh)) {
        TOS = files[fh]->read(&c, 1);
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
        int l = files[fh].readBytesUntil('\n', a, sz);
        TOS = 1;
    }
}

void fWrite() {              // (c fh--)
    CELL fh = pop();
    byte c = (byte)pop();
    if (VALIDF(fh)) { files[fh]->write(&c, 1); }
}

void fClose() {              // (fh--)
    CELL fh = pop();
    if (VALIDF(fh)) {
        files[fh]->close();
        files[fh] = 0;
    }
}

void fSave() {
    File fp = myFS.open("system.ccc", "w");
    if (fp) {
        fp.write((byte*)&mem[0], MEM_SZ);
        fp.close();
        printString("-saved-");
    } else { printString("-error-"); }
}

void fLoad() {
    File fp = myFS.open("system.ccc", "r");
    if (fp) {
        vmReset();
        fp.read((byte*)&mem[0], MEM_SZ);
        fp.close();
        printString("-loaded-");
    } else { printString("-error-"); }
}

int doLoad() {
      printString("-load-");
      return 0;
}

void fDelete() {
    char *fn = (char*)pop();
    if (myFS.remove(fn)) { printString("-deleted-"); }
    else { printString("-noFile-"); }
}

void fList() {}
