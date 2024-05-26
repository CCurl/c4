# c4: a portable Forth system inspired by Tachyon

In c4, a program is a sequence of word-codes. <br/>
A word-code is an unsigned 16-bit unsigned value (0..65535). <br/>
A `CELL` is either 32-bits or 64-bits, depending on the target system. <br/>

## WORD-CODEs

If a word-code is less than or equal to `BYE`, it is a code primitive. <br/>
If it is greater than `BYE`, it is the address of a word to execute. <br/>

## C4 memory areas

C4 uses three memory areas:
- The code area can store up to 65536 word-codes. (CODE-SZ) <br/>
- The dictionary area can be up to 65536 bytes (DICT-SZ). <br/>
- The variables area can be up to CELL kkkkkkkk

## C4 architecture

todo

## C4 WORD-CODE primitives

| CODE | NAME  | STACK     | DESCRIPTION |
|:--   |:--    |:--        |:-- |
|  0 | STOP   | ( -- )     | Stop C4 execution, return to REPL |
|  1 | LIT1   | ( --N )    | N: next WORD-CODE |
|  2 | LIT2   | ( --N )    | N: next CELL |
|  3 | JMP    | (--)       | DESC |
|  4 | JMPZ   | (--)       | DESC |
|  5 | JMPNZ  | (--)       | DESC |
|  6 | EXIT   | (--)       | DESC |
|  7 | DUP    | (--)       | DESC |
|  8 | SWAP   | (--)       | DESC |
|  9 | DROP   | (--)       | DESC |
| 10 | OVER   | (--)       | DESC |
| 11 | FET    | (--)       | DESC |
| 12 | CFET   | (--)       | DESC |
| 13 | WFET   | (--)       | DESC |
| 14 | STO    | (--)       | DESC |
| 15 | CSTO   | (--)       | DESC |
| 16 | WSTO   | (--)       | DESC |
| 17 | ADD    | (--)       | DESC |
| 18 | SUB    | (--)       | DESC |
| 19 | MUL    | (--)       | DESC |
| 20 | DIV    | (--)       | DESC |
| 21 | SLMOD  | (--)       | DESC |
| 22 | INC    | (--)       | DESC |
| 23 | DEC    | (--)       | DESC |
| 24 | LT     | (--)       | DESC |
| 25 | EQ     | (--)       | DESC |
| 26 | GT     | (--)       | DESC |
| 27 | EQ0    | (--)       | DESC |
| 28 | AND    | (--)       | DESC |
| 29 | OR     | (--)       | DESC |
| 30 | XOR    | (--)       | DESC |
| 31 | COM    | (--)       | DESC |
| 32 | DO     | (--)       | DESC |
| 33 | INDEX  | (--)       | DESC |
| 34 | LOOP   | (--)       | DESC |
| 35 | ANEW   | (--)       | DESC |
| 36 | AGET   | (--)       | DESC |
| 37 | ASET   | (--)       | DESC |
| 38 | AFREE  | (--)       | DESC |
| 39 | TOR    | (--)       | DESC |
| 40 | RAT    | (--)       | DESC |
| 41 | RFROM  | (--)       | DESC |
| 42 | EMIT   | (--)       | DESC |
| 43 | DOT    | (--)       | DESC |
| 44 | COLON  | (--)       | DESC |
| 45 | SEMI   | (--)       | DESC |
| 46 | IMM    | (--)       | DESC |
| 47 | CREATE | (--)       | DESC |
| 48 | COMMA  | (--)       | DESC |
| 49 | CLK    | (--)       | DESC |
| 50 | SEE    | (--)       | DESC |
| 51 | COUNT  | (--)       | DESC |
| 52 | TYPE   | (--)       | DESC |
| 53 | QUOTE  | (--)       | DESC |
| 54 | DOTQT  | (--)       | DESC |
| 55 | TOCODE | (--)       | DESC |
| 56 | TOVARS | (--)       | DESC |
| 57 | TODICT | (--)       | DESC |
| 58 | RAND   | (--)       | DESC |
| 59 | BYE    | (--)       | Exit C4 (PC only) |
