extern int outer(const char *ln);

// comment this out to back the system in
#define _BOOTFILE_

#ifdef _BOOTFILE_

void sys_load() { outer("1 LOAD"); }

#else

void sys_load() {
    outer("\
: C@V >VARS C@ ; \
: C!V >VARS C! ; \
: @C DUP + >CODE W@ ; \
: !C DUP + >CODE W! ; \
: HEX     $10 BASE !C ; \
: BINARY  %10 BASE !C ; \
: DECIMAL #10 BASE !C ; \
: HERE  (HERE)  @C ; \
: LAST  (LAST)  @C ; \
: VHERE (VHERE) @ ; \
: LEX   (LEX)   @C ; \
: BEGIN HERE ; IMMEDIATE \
: AGAIN (JMP)   , , ; IMMEDIATE \
: WHILE (JMPNZ) , , ; IMMEDIATE \
: UNTIL (JMPZ)  , , ; IMMEDIATE \
: IF (JMPZ) , HERE 0 , ; IMMEDIATE \
: ELSE (JMP) , HERE SWAP 0 , HERE SWAP !C ; IMMEDIATE \
: THEN HERE SWAP !C ; IMMEDIATE \
: ( BEGIN \
    >IN @ C@  \
    DUP  0= IF DROP EXIT THEN \
    >IN @ 1+ >IN ! \
    ')' = IF EXIT THEN \
  AGAIN ; IMMEDIATE \
: ALLOT VHERE + (VHERE) ! ; \
: ,V VHERE >VARS ! CELL ALLOT ; \
: CELLS CELL * ; \
: NIP SWAP DROP ; \
: TUCK SWAP OVER ; \
: ?DUP DUP IF DUP THEN ; \
: NEGATE COM 1+ ; \
: ABS DUP 0 < IF NEGATE THEN ; \
: MOD /MOD DROP ; \
: +! TUCK @ + SWAP ! ; \
: SPACE 32 EMIT ; \
: . (.) SPACE ; \
: CR 13 EMIT 10 EMIT ; \
: TAB 9 EMIT ; \
: ?  @ . ; \
: A+C a> DUP CELL + >A ; \
: !A   a> ! ;   : !A+   A+C ! ; \
: @A   a> @ ;   : @A+   A+C @ ; \
: C!A  a> C! ;  : C!A+  A+  C! ; \
: C@A  a> C@ ;  : C@A+  A+  C@ ; \
: >XT     W@ ; \
: >SIZE   2 + C@ ; \
: >FLAGS  3 + C@ ; \
: >LEX    4 + C@ ; \
: >NAME   5 + ; \
: lex-match? ( a--f ) >LEX LEX = LEX 0= OR ; \
: WORDS a> >R  0 DUP >A >R  LAST >DICT BEGIN \
      DUP lex-match? IF \
        DUP >NAME COUNT TYPE  R> 1+ >R \
        A+ 8 > IF CR 0 >A ELSE TAB THEN \
      THEN \
      DUP >SIZE + DUP DICT-SZ >DICT < \
    WHILE DROP '(' EMIT R> . .\" words)\" R> >A ; \
: DOES> (JMP) , R> , ; \
: VAR   ADDWORD CELL ALLOT (EXIT) , ; IMMEDIATE \
: CONST ADDWORD ,V DOES> @ ; IMMEDIATE \
");
  parseLine("VAR hh");
  parseLine("VAR ll");
  parseLine("VAR vv");
  parseLine(": MARKER HERE hh ! LAST ll ! VHERE vv ! ;");
  parseLine(": FORGET hh @ (HERE) !C ll @ (LAST) !C vv @ (VHERE) !C ;");
  parseLine("MARKER");
}
#endif // _BOOTFILE_
