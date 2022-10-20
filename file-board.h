#ifndef __FILES__

void noFile() { printString("-noFile-"); }
void fileInit() { }
void fOpen() { noFile(); }
void fGetC() { noFile(); }
void fGetS() { noFile(); }
void fWrite() { noFile(); }
void fClose() { noFile(); }
void fDelete() { noFile(); }
void fList() { noFile(); }
void fSave() { noFile(); }
void fLoad() { noFile(); }
int doLoad() { noFile(); return 0; }

#else

#if __BOARD__ == TEENSY4
    #include "file-teensy.h"
#else
    #include "file-generic.h"
#endif // TEENSY4

#endif // __FILES__
