# c4: a portable Forth system inspired by Tachyon

In C4, a program is a sequence of WORD-CODEs. <br/>
A `WORD-CODE` is a 16-bit unsigned number (0..65535). <br/>
Primitives are assigned numbers sequentially from 0 to `BYE`.
If a WORD-CODE is less than or equal to `BYE`, it is a primitive. <br/>
If it is greater than `BYE`, it is the code address of a word to execute. <br/>

## CELLs in C4
A `CELL` is either 32-bits or 64-bits, depending on the target system.
- Linux 32-bit (-m32): CELLs are 32-bit
- Linux 64-bit (-m64): CELLs are 64-bit
- Windows 32-bit (x86): CELLs are 32-bit
- Windows 32-bit (x64): This configration does not work
- Development boards: CELLs are 32-bit

## C4 memory areas

C4 uses three memory areas:
- The code area can store up to 65536 WORD-CODEs, 16-bit index. (CODE-SZ). <br/>
- The dictionary area can be up to 65536 bytes, 16-bit index (DICT-SZ). <br/>
- The variables area can be up to CELL bytes, CELL index (VARS-SZ).

## C4 Strings

Strings in C4 are both counted and NULL terminated.

## C4 architecture

todo

## C4 WORD-CODE primitives

| CODE | NAME      | STACK        | DESCRIPTION |
|:--   |:--        |:--           |:-- |
|   0  | stop      | (--)         | Stop C4 execution, return to REPL |
|   1  | lit1      | (--N)        | N: next WORD-CODE |
|   2  | lit2      | (--N)        | N: next CELL |
|   3  | jmp       | (N--)        | Set PC to the next WORD-CODE |
|   4  | jmpz      | (N--)        | Set PC to the next WORD-CODE if N == 0 |
|   5  | jmpnz     | (--)         | Set PC to the next WORD-CODE if N != 0 |
|   6  | exit      | (--)         | EXIT word |
|   7  | dup       | (N--N N)     | Duplicate TOS (Top-Of-Stack) |
|   8  | swap      | (A B--B A)   | Swap TOS and NOS (Next-On-Stack) |
|   9  | drop      | (N--)        | Drop TOS |
|  10  | over      | (A B--A B A) | Push NOS |
|  11  | @         | (A--N)       | N: the CELL at A |
|  12  | c@        | (A--C)       | C: the CHAR at A |
|  13  | w@        | (A--W)       | W: the WORD at A |
|  14  | !         | (V A--)      | Store CELL N to A |
|  15  | c!        | (V A--)      | Store CHAR C to A |
|  16  | w!        | (V A--)      | Store WORD W to A |
|  17  | +         | (A B--C)     | C: A + B |
|  18  | -         | (A B--C)     | C: A - B |
|  19  | *         | (A B--C)     | C: A * B |
|  20  | /         | (A B--C)     | C: A / B (integer division) |
|  21  | /mod      | (A B--M Q)   | M: A MODULO B, Q: quotient of A / B |
|  22  | 1+        | (A--B)       | Increment TOS |
|  23  | 1-        | (A--B)       | Decrement TOS |
|  24  | <         | (A B--F)     | F: 1 if (A<B>), else 0 |
|  25  | =         | (A B--F)     | F: 1 if (A=B>), else 0 |
|  26  | >         | (A B--F)     | F: 1 if (A>B>), else 0 |
|  27  | 0=        | (N--F)       | F: 1 if (N=0), else 0 |
|  28  | and       | (A B--N)     | N: A AND B |
|  29  | or        | (A B--N)     | N: A OR  B |
|  30  | xor       | (A B--N)     | N: A XOR B |
|  31  | com       | (A--B)       | N: A with all bits flipped |
|  32  | do        | (T F--)      | Begin DO loop with bounds F and T |
|  33  | i         | (--I)        | I: Current DO LOOP index |
|  34  | loop      | (--)         | Increment I, stop if I >= T |
|  35  | >a        | (N--)        | Set A to N |
|  36  | a         | (--N)        | N: the current value of A |
|  37  | a+        | (--N)        | Push, then increment A |
|  38  | a-        | (--N)        | Push, then decrement A |
|  39  | >r        | (N--R:N)     | Move TOS to the return stack |
|  40  | r@        | (--N)        | N: return stack TOS |
|  41  | r>        | (R:N--N)     | Move return TOS to the stack |
|  42  | emit      | (C--)        | Output char C |
|  43  | (.)       | (N--)        | Print N in the current BASE, no trailing SPACE |
|  44  | :         | (--)         | Create a new word, set STATE=1 |
|  45  | ;         | (--)         | Compile EXIT, set STATE=0 |
|  46  | immediate | (--)         | Mark the last created word as IMMEDIATE |
|  47  | addword   | (--)         | Add the next word to the dictionary |
|  48  | timer     | (--N)        | N: Current time |
|  50  | see       | (--)         | Output the definition of a word |
|  50  | count     | (S--A N)     | A,N: Address and Count of string S |
|  51  | type      | (A N--)      | Print N chars starting at A |
|  52  | "         | CT: (--)     | Compile time: Copy chars to " to VARS |
|      |           | RT: ()       | Run time: push address A of string |
|  54  | ."        | CT: (--)     | Compile time: Copy chars to " to VARS |
|      |           | RT: (--)     | Run time: COUNT/TYPE string |
|  54  | rand      | (--N)        | N: a pseudo-random number (13/17/5 XOR shift) |
|  55  | fopen     | (NM MD--H)   | NM: File Name, MD: Mode, H: File Handle |
|  56  | fclose    | (H--)        | H: File Handle |
|  57  | fread     | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
|  58  | fwrite    | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
|  59  | fgets     | (B SZ H--F)  | B: Buffer, SZ: Size, F: 0 if EOF/Error, else 1 |
|  60  | fload     | (NM--)       | NM: File Name to load |
|  61  | load      | (N--)        | N: Block number to load (block-NNN.c4) |
|  62  | system    | (A--)        | PC ONLY: A: String to send to the system() |
|  63  | bye       | (--)         | PC ONLY: Exit C4 |
