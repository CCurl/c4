#include <stdio.h>
#include "c4.h"

#define PC_FILE

cell inputFp, outputFp, fileStk[20];
int fileSp;

void filePush(cell val) { ; }
cell filePop() { return (0<fileSp) ? fileStk[fileSp--]: 0; }

#ifdef NO_FILE
void fileInit() { fileSp = 0; }
cell fileOpen(cell name, cell mode) { return 0; }
void fileClose(cell fh) {}
int fileGets(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
#endif

#ifdef PC_FILE
void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(cell name, cell mode) { return (cell)fopen((char*)name, (char*)mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }

int fileGets(char *buf, int sz, cell fh) {
    buf[0] = 0;
    if (fh == 0) { fh = (cell)stdin; }
    return (fgets(buf, sz, (FILE*)fh) == buf) ? 1 : 0;
}
#endif

#ifdef PICO_FILE
#endif

#ifdef TEENSY_FILE
#endif
