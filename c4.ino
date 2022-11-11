#include <Arduino.h>
#include "c4.h"

#define AR(x) analogRead(x)
#define DR(x) digitalRead(x)
#define mySerial Serial

extern char tib[];
extern void handleInput(char c, int echo);

CELL doTimer() { return millis(); }
void doSleep() { delay(pop()); }
CELL getSeed() { return millis(); }

#ifdef mySerial
    int charAvailable() { return mySerial.available(); }
    int getChar() { 
        while (!charAvailable()) {}
        return mySerial.read();
    }
    void printSerial(const char* str) { mySerial.print(str); }
#else
    int charAvailable() { return 0; }
    int getChar() { return 0; }
    void printSerial(const char* str) { }
#endif

#ifdef __GAMEPAD__
int buttonVal[GP_BUTTONS];
#if __BOARD__ == TEENSY4
  #include "gamePad-Teensy.h"
#else
  #include "gamePad.h"
#endif
void gamePadInit() {
    for (int i = 0; i < GP_BUTTONS; i++) {
        buttonVal[i] = 0;
    }
    gpInit();
}
#else
void gamePadInit() {}
byte *doGamePad(byte ir, byte *pc) { printString("-noGamepad-"); return pc; }
#endif

int isOTA = 0;

void printString(const char* str) { 
    if (isOTA) { } //printWifi(str); }
    else { printSerial(str); }
}

void printChar(char ch) { 
    char b[2] = { ch, 0 };
    printString(b);
}

#include "file-board.h"

byte *doUser(CELL ir, byte *pc) {
    CELL pin;
    switch (ir) {
    case 'G': pc = doGamePad(ir, pc);           break;  // zG<x>
    case 'N': push(micros());                   break;  // zN (--n)
    case 'P': pin = pop(); ir = *(pc++);                // Pin operations
        switch (ir) {
        case 'I': pinMode(pin, INPUT);                           break;  // zPI (p--)
        case 'O': pinMode(pin, OUTPUT);                          break;  // zPO (p--)
        case 'U': pinMode(pin, INPUT_PULLUP);                    break;  // zPU (p--)
        case 'R': ir = *(pc++);
            if (ir == 'A') { push(analogRead(pin));  }                   // zPRA (p--n)
            if (ir == 'D') { push(digitalRead(pin)); }           break;  // zPRD (p--n)
        case 'W': ir = *(pc++);
            if (ir == 'A') { analogWrite(pin,  (int)pop()); }            // zPWA (n p--)
            if (ir == 'D') { digitalWrite(pin, (int)pop()); }    break;  // zPWD (n p--)
        default:
            pc = 0;
            printString("-notPin-");
        }                                       break;
    default:
        pc = 0;
        printString("-notExt-");
    }
    return pc;
}

// ********************************************
// * HERE is where you load your default code *
// ********************************************
void loadCode() {
    doParse(".\" c4 v 0.0.1\"");
}

// NB: tweak this depending on what your terminal window sends for [Backspace]
// E.G. - PuTTY sends a 127 for Backspace
int isBackSpace(int c) { 
  // printStringF("(%d)",c);
  return (c == 127) ? 1 : 0; 
}

void setup() {
#ifdef mySerial
    mySerial.begin(19200);
    while (!mySerial) {}
    delay(500);
    // while (mySerial.available()) { char c = mySerial.read(); }
#endif
    vmReset();
    loadCode();
    printString("c4 v0.0.1 - Chris Curl\r\n");
    doOK();
    gamePadInit();
//    wifiStart();
    fileInit();
}

void boardIdle() {
    if (doFind("idle")) {
      pop();
      run((WORD) pop());
    }
}

void loop() {
    static int iLed = 0;
    static long nextBlink = 0;
    static int ledState = LOW;
    long curTm = millis();

    if (iLed == 0) {
        doOK();
        in = tib;
        iLed = LED_BUILTIN;
        pinMode(iLed, OUTPUT);
    }
    
    if (nextBlink < curTm) {
        ledState = (ledState == LOW) ? HIGH : LOW;
        digitalWrite(iLed, ledState);
        nextBlink = curTm + 1000;
    }

    while (charAvailable()) { 
        if (handleInput(getChar(), 1)) {
            doParse(rTrim(tib));
            doOK();
            in = tib;
        }

    }
    //while (wifiCharAvailable()) { 
    //    handleInput(wifiGetChar(), 0); 
    //}
    boardIdle();
}
