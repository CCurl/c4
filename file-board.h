#ifndef __FILES__

void fileInit() { }
void fOpen() { DROP2; push(0); }
void fClose() { DROP1; }
void fRead() { DROP3; push(0); }
void fWrite() { DROP3; push(0); }
void fGetC() { push(0); }
void fPutC() { DROP2; }
void fGetS() { DROP3; push(0); }
void fDelete() { DROP1; }
void fList() { }
void fSaveSys() { }
int fLoadSys() { return 0; }
void fLoad(const char *fn) { }

#else

#if __BOARD__ == TEENSY4
    #include "file-teensy.h"
#elif __BOARD__ == PICO
    #include "file-pico.h"
#else
    #include "file-generic.h"
#endif // __BOARD__

#endif // __FILES__
