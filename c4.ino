#include "c4.h"

void ttyMode(int isRaw) {}
cell timer() { return millis(); }

#define mySerial Serial // Teensy and Pico

#ifdef mySerial
    void serialInit() { while (!mySerial) ; }
    void emit(char c) { mySerial.print(c); }
    void zType(const char *s) { mySerial.print(s); }
    int qKey() { return mySerial.available(); }
    int key() {
        while (!qKey()) {}
        return mySerial.read();
    }
#else
    void serialInit() {}
    void emit(char c) {}
    void zType(const char *s) {}
    int qKey() { return 0; }
    int key() { return 0; }
#endif

char tib[128],*in=0,isInit=0;

void fill(char *d, int x, int num) {
    for (int i=0; i<num; i++) { d[i] = x; }
}

void setup() {
    isInit = 0;
    in = 0;
    serialInit();
    fill(tib, 0, sizeof(tib));
    // zType("hello!\r\n");
}

void idle() {}


void loop() {
  // if (isInit==0) { zType("Init ... "); Init(); isInit=1; zType("back\r\n ok."); }
  // ++isInit;
  // zType("more ...");
  // return;

  if (!isInit) {
    Init();
    isInit=1;
  }
  // outer("a>+ . a> 64 = if cr then");
  if (qKey() == 0) { idle(); return; }
  int c = key();
  if (!in) {
      in = tib;
      fill(tib, 0, sizeof(tib));
  }

  if (c==13) {
      *(in) = 0;
      emit(32);
      outer(tib);
      zType(" ok\r\n");
      in = 0;
  } else if ((c==8) || (c==127)) {
      if ((--in) < tib) { in = tib; }
      else { emit(8); emit(32); emit(8); }
  } else {
      if (c==9) { c = 32; }
      if (btwi(c,32,126)) { *(in++) = c; emit(c); }
  }
}
