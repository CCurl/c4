#include "c4.h"

void zType(const char *str) {}
void emit(const char ch) {}
void ttyMode(int isRaw) {}
int  key() { return 0; }
int  qKey() { return 0; }
cell timer() { return 0; }

void setup() {
    Init();
}

void loop() {
    outer("a>+ . a> 64 = if cr then");
}
