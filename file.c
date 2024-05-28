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
cell fileRead(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
cell fileWrite(char *buf, int sz, cell fh) { return 0; }
int fileGets(char *buf, int sz, cell fh) { buf[0]=0; return 0; }
#endif

#ifdef PC_FILE
void fileInit() { fileSp = 0; inputFp = 0; }
cell fileOpen(cell name, cell mode) { return (cell)fopen((char*)name+1, (char*)mode+1); }
void fileClose(cell fh) { fclose((FILE*)fh); }
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }

int  fileGets(char *buf, int sz, cell fh) {
    buf[0] = 0; // length
    buf[1] = 0; // NULL terminator
    if (fh == 0) { fh = (cell)stdin; }
    if (fgets(buf+1, sz, (FILE*)fh) != buf+1) return 0;
    buf[0] = strLen(buf+1);
    return 1;
}

void fileLoad(cell name) {
    cell fh = fileOpen(name, " rt");
    if (fh == 0) { printf("-not found-"); return; }
    if (inputFp) { filePush(inputFp); }
    inputFp = fh;
}

#endif

#ifdef PICO_FILE
#endif

#ifdef TEENSY_FILE
#endif
