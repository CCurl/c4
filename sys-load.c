extern int parseLine(const char *ln);

void sys_load() {
    parseLine("\
: @C DUP + >CODE W@ ; \
: !C DUP + >CODE W! ; \
: (HERE)  0 ; : HERE  (HERE)  @C ; \
: (LAST)  1 ; : LAST  (LAST)  @C ; \
: (VHERE) 2 ; : VHERE (VHERE) @C ; \
: (LEX)   5 ; : LEX   (LEX)   @C ; \
: BASE    3 ; : STATE 4 ; \
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
: ALLOT VHERE + (VHERE) !C ; \
: ,V VHERE >VARS ! CELL ALLOT ; \
: C@V >VARS C@ ; \
: C!V >VARS C! ; \
: CELLS CELL * ; \
: NIP SWAP DROP ; \
: TUCK SWAP OVER ; \
: ?DUP DUP IF DUP THEN ; \
: NEGATE COM 1+ ; \
: ABS DUP 0 < IF NEGATE THEN ; \
: MOD /MOD DROP ; \
: +! TUCK @ + SWAP ! ; \
: SPACE 32 EMIT ; : . (.) SPACE ; \
: CR 13 EMIT 10 EMIT ; : TAB 9 EMIT ; \
: !A A C! ;   : !A+ A+ C! ; \
: @A A C@ ;   : @A+ A+ C@ ; \
: >XT     W@ ; \
: >SIZE   2 + C@ ; \
: >FLAGS  3 + C@ ; \
: >LEX    4 + C@ ; \
: >NAME   5 + ; \
: lex-match? ( a--f ) >LEX LEX = LEX 0= OR ; \
: WORDS A >R  0 DUP >A >R  LAST >DICT BEGIN \
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
}
