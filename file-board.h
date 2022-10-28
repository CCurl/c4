#ifndef __FILES__

void fileInit() { }
void fOpen()    { DROP2; push(0); }           // (fn md--fh) -- md: 0=READ, else WRITE
void fClose()   { DROP1; }                    // (fh--)
void fRead()    { DROP3; push(0); }           // (a sz fh--n)
void fWrite()   { DROP3; push(0); }           // (a sz fh--n)
void fGetC()    { DROP1; push(0); push(0); }  // (fh--c n)
void fPutC()    { DROP2; }                    // (c fh--)
void fGetS()    { DROP3; push(0); }           // (a sz fh--n)
void fDelete()  { DROP1; }                    // (fn--)
void fSeek()    { DROP3; }                    // ( offset whence fh-- )
void fTell()    { DROP1; push(0); }           // (fh--n)
void fList()    { }                           // (--)
void fSaveSys() { }                           // (--)
int fLoadSys()  { return 0; }                 // (--)
void fLoad(const char *fn) { }                // (--)

#else

#if __BOARD__ == TEENSY4
    #include "file-teensy.h"
#elif __BOARD__ == PICO
    #include "file-pico.h"
#else
    #include "file-generic.h"
#endif // __BOARD__

#endif // __FILES__
