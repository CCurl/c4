// Support for development boards

#include "c4.h"

#define mySerial Serial // Teensy and Pico

// #include "file-teensy.ipp"
// #include "file-pico.ipp"

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
    void serialInit() { }
    void emit(char c) {}
    void zType(char *s) {}
    int qKey() { return 0; }
    int key() { return 0; }
#endif

cell timer() { return micros(); }

// Cells are always 32-bit on dev boards (no 64-bit)
#define S1(x, y) (*(x)=((y)&0xFF))
void Store(char *l, cell v) {
    if (((cell)l & 0x03) == 0) { *(cell*)l = v; }
    else {
        S1(l,v); S1(l+1,v>>8); S1(l+2,v>>16); S1(l+3,v>>24);
    }
}

#define G1(x, y) (*(x)<<y)
cell Fetch(const char *l) {
    if (((cell)l & 0x03) == 0) { return *(cell*)l; }
    return (*l) | G1(l+1,8) | G1(l+2,16) | G1(l+3,24);
}

char *in, tib[128];
void setup() {
  serialInit();
  // zType("Hello.");
  fileInit();
  c4Init();
  ok();
  in = (char*)0;
}

void idle() {
  // Fill this in as desired
}

void loop() {
  if (qKey() == 0) { idle(); return; }
  int c = key();
  if (!in) {
      in = tib;
      for (int i=0; i<128; i++) { tib[i] = 0; }
  }

  if (c==9) { c = 32; }
  if (c==13) {
      *(in) = 0;
      outer(tib);
      ok();
      in = 0;
  } else if ((c==8) || (c==127)) {
      if ((--in) < tib) { in = tib; }
      else { emit(8); emit(32); emit(8); }
  } else {
      if (btwi(c,32,126)) { *(in++) = c; emit(c); }
  }
}
