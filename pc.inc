// Support for the PC
// NOTE: this is a .inc file because the Arduino IDE only knows about .CPP and .H files
// If it were a .CPP file, it would need a #if around the entire thing.
// Making it a .INC file avoids that need.

FILE *input_fp = NULL;
FILE *fpStk[10];
byte fpSP = 0, isBye = 0;

CELL doTimer() { return clock(); }
#if __TARGET__ == LINUX

void ttyMode(int mode)
{
    static struct termios oldt;
    static struct termios rawt;
    static int curMode = -1;
    if (curMode == -1) {
        curMode = 0;
        tcgetattr( STDIN_FILENO, &oldt);
        cfmakeraw(&rawt);
    }

    if (mode != curMode) {
        if (mode) {
            tcsetattr( STDIN_FILENO, TCSANOW, &rawt);
        } else {
            tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
        }
        curMode = mode;
    }
}

int charAvailable(void)
{
    struct timeval tv;
    fd_set rdfs;

    ttyMode(1);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);

    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    int x = FD_ISSET(STDIN_FILENO, &rdfs);
    return x;
}
int getChar(void) {
    ttyMode(1);
    return getchar();
}
void doSleep() { usleep(pop()); }
void readLine(char *buf, int sz) {
    ttyMode(0);
    fgets(buf, sz, stdin);
    ttyMode(1);
}

#else // WINDOWS

int charAvailable(void) { return _kbhit(); }
int getChar(void) { return _getch(); }
void doSleep() { Sleep(pop()); }
#define ttyMode(x)
#define readLine(buf, sz) fgets(buf, sz, stdin)

#endif

// ************************** FILES **************************

void fpPush(FILE *v) { if (fpSP < 9) { fpStk[++fpSP] = v; } }
FILE *fpPop() { return (fpSP) ? fpStk[fpSP--] : 0 ; }

FILE *myFopen(const char *fn, const char *md) {
    FILE *fp;
#ifdef _WIN32
    fopen_s(&fp, fn, md);
#else
    fp = fopen(fn, md);
#endif
    return fp;
}

void fOpen() {
    CELL mode = pop();
    char *fn = (char *)pop();
    FILE *fp = myFopen(fn, mode ? "wb" : "rb");
    push((CELL)fp);
}

void fClose() {
    CELL fp = pop();
    if (fp) { fclose((FILE *)fp); }
}

void fGetC() {
    FILE *fp = (FILE *)pop();
    push(0); push(0);
    if (fp && (!feof(fp))) {
        NOS = fgetc(fp);
        TOS = 1;
    }
}

void fGetS() {
    FILE *fp = (FILE *)pop();
    CELL sz = pop();
    char *cp = (char *)TOS;
    TOS = 0;
    *cp = 0;
    if (fp && !feof(fp)) {
        fgets(cp, sz, fp);
        TOS = 1;
    }
}

void fWrite() {
    FILE *fp = (FILE *)pop();
    CELL val = pop();
    if (fp) { fputc(val, fp); }
}

void fDelete() {}

void fList() {}

void fSave() {
    FILE *fp = myFopen("system.c4", "wb");
    if (fp) {
        fwrite(&mem[0], MEM_SZ, 1, fp);
        fclose(fp);
        printString("-saved-");
    } else { printString("-error-"); }
}

void fLoad() {
    FILE *fp = myFopen("system.c4", "rb");
    if (fp) {
        vmReset();
        fread(&mem[0], MEM_SZ, 1, fp);
        fclose(fp);
        printString("-loaded-");
    } else { printString("-error-"); }
}

int doLoad() {
    if (getWord(word) == 0) { printString("-fn?-"); return 0; }
    FILE *fp = myFopen(word, "rt");
    if (fp) {
        if (input_fp) { fpPush(input_fp); }
        input_fp = fp;
    }
    return fp ? 1 : 0;
}

byte *doUser(CELL ir, byte *pc) {
    switch (ir) {
    case 'E': doEditor();                break;
    case 'L': doLoad();                  break;
    case 'Q': isBye=1; pc=0;             break;
    default: printString("-unk ext-");
    }
    return pc;
}

void printString(const char *cp) { fputs(cp, stdout); }
void printChar(char c) { putc(c, stdout); }

void doHistory(const char *txt) {
    FILE* fp = myFopen("history.txt", "at");
    if (fp) {
        fputs(txt, fp);
        fclose(fp);
    }
}

void loop() {
    char *tib = (char *)CA(tHERE+32);
    FILE* fp = (input_fp) ? input_fp : stdin;
    if (input_fp) {
        if (fgets(tib, 128, fp) == tib) {
            doParse(rTrim(tib));
        } else {
            fclose(input_fp);
            input_fp = fpPop();
        }
    } else {
        doOK();
        readLine(tib, 128);
        doHistory(tib);
        doParse(rTrim(tib));
    }
}

int main(int argc, char *argv[]) {
    if (sizeof(&HERE) > CELL_SZ) {
        printf("ERROR: CELL cannot support a pointer!");
        exit(1);
    }
    vmReset();
    ttyMode(1);
    if (argc > 1) { input_fp = myFopen(argv[1], "rt"); }
    while (!isBye) { loop(); }
    ttyMode(0);
    return 0;
}
