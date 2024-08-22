#include "c4.h"

#ifdef IS_WINDOWS

#include <conio.h>
int qKey() { return _kbhit(); }
int key() { return _getch(); }
void ttyMode(int isRaw) {}

#endif

#ifdef IS_LINUX // Support for Linux

#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

void ttyMode(int isRaw) {
    static struct termios origt, rawt;
    static int curMode = -1;
    if (curMode == -1) {
        curMode = 0;
        tcgetattr( STDIN_FILENO, &origt);
        cfmakeraw(&rawt);
    }
    if (isRaw != curMode) {
        if (isRaw) {
            tcsetattr( STDIN_FILENO, TCSANOW, &rawt);
        } else {
            tcsetattr( STDIN_FILENO, TCSANOW, &origt);
        }
        curMode = isRaw;
    }
}
int qKey() {
    struct timeval tv;
    fd_set rdfs;
    ttyMode(1);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    int x = FD_ISSET(STDIN_FILENO, &rdfs);
    // ttyMode(0);
    return x;
}
int key() {
    ttyMode(1);
    int x = fgetc(stdin);
    // ttyMode(0);
    return x;
}

#endif // IS_LINUX

cell fileOpen(const char *name, const char *mode) { return (cell)fopen(name, mode); }
void fileClose(cell fh) { fclose((FILE*)fh); }
void fileDelete(const char *name) { remove(name); }
cell fileRead(char *buf, int sz, cell fh) { return fread(buf, 1, sz, (FILE*)fh); }
cell fileWrite(char *buf, int sz, cell fh) { return fwrite(buf, 1, sz, (FILE*)fh); }
cell timer() { return (cell)clock(); }
void zType(const char *str) { fputs(str, outputFp ? (FILE*)outputFp : stdout); }
void emit(const char ch) { fputc(ch, outputFp ? (FILE*)outputFp : stdout); }

// REP - Read/Execute/Print (no Loop)
void REP() {
    char tib[256];
	ttyMode(0);
	zType(" ok\r\n");
	if (fgets(tib, sizeof(tib), stdin) == tib) {
		outer(tib);
		return;
	} else { exit(0); }
}

void loadArgument(const char *arg) {
    char fn[32];
    strCpy(fn, arg);
    cell fh = fileOpen(fn, "rb");
    if (fh) {
        for (int i = 0; i <= MAX_SRC; i++) { source[i] = 0; }
        fileRead(source, MAX_SRC, fh);
        fileClose(fh);
        outer(source);
        printf("\nback!");
    }
}

int main(int argc, char *argv[]) {
	Init();
    if (argc > 1) { loadArgument(argv[1]); }
    else { loadArgument("source.c4"); }
    while (1) { REP(); };
	return 0;
}
