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

| VALUE | STACK     | DESCRIPTION |
|:--    |:--        |:-- |
| 0     | ( -- )    | `STOP`: Stop C4 execution, return to REPL |
| 1     | ( -- )    | `XXX`: todo |
