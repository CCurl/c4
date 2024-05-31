# c4: a portable Forth system inspired by Tachyon

In C4, a program is a sequence of WORD-CODEs. <br/>
A `WORD-CODE` is a 16-bit unsigned number (0..65535). <br/>
Primitives are assigned numbers sequentially from 0 to `BYE`. <br/>
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
- The code area can store up to 65536 WORD-CODEs, 16-bit index. (CODE-SZ).
- The dictionary area can be up to 65536 bytes, 16-bit index (DICT-SZ).
- The variables area can be up to CELL bytes, CELL index (VARS-SZ).

## C4 Strings

Strings in C4 are both counted and NULL terminated.

## MachineForth influences

C4 includes 4 built-in variables as primitives (`a`, `b`, `s`, `d`). <br/>
This is in the spirit of MachineForth, with its opcodes for the 'a' register. <br/>
's' is shorthand for 'source', but can be used for anything. <br/>
'd' is shorthand for 'destination', but can be used for anything. <br/>
- They all support set (`>a`, `>b`, `>s`, `>d`)
- They all support get (`a`, `b`, `s`, `d`)
- They all support get with increment (`a+`, `b+`, `s+`, `d+`)

## C4 architecture

todo

## C4 WORD-CODE primitives

| NAME      | STACK        | DESCRIPTION |
|:--        |:--           |:-- |
| (lit1)    | (--N)        | N: WORD-CODE for LIT1 primitive |
| (lit2)    | (--N)        | N: WORD-CODE for LIT2 primitive |
| (jmp)     | (--N)        | N: WORD-CODE for JMP primitive |
| (jmpz)    | (--N)        | N: WORD-CODE for JMPZ primitive |
| (jmpnz)   | (--N)        | N: WORD-CODE for JMPNZ primitive |
| (exit)    | (--N)        | N: WORD-CODE for EXIT primitive |
| exit      | (--)         | EXIT word |
| dup       | (N--N N)     | Duplicate TOS (Top-Of-Stack) |
| swap      | (A B--B A)   | Swap TOS and NOS (Next-On-Stack) |
| drop      | (N--)        | Drop TOS |
| over      | (A B--A B A) | Push NOS |
| @         | (A--N)       | N: the CELL at A |
| c@        | (A--C)       | C: the CHAR at A |
| w@        | (A--W)       | W: the WORD at A |
| !         | (V A--)      | Store CELL N to A |
| c!        | (V A--)      | Store CHAR C to A |
| w!        | (V A--)      | Store WORD W to A |
| +         | (A B--C)     | C: A + B |
| -         | (A B--C)     | C: A - B |
| *         | (A B--C)     | C: A * B |
| /         | (A B--C)     | C: A / B (integer division) |
| /mod      | (A B--M Q)   | M: A modulo B, Q: quotient of A / B |
| 1+        | (A--B)       | Increment TOS |
| 1-        | (A--B)       | Decrement TOS |
| <         | (A B--F)     | F: 1 if (A<B>), else 0 |
| =         | (A B--F)     | F: 1 if (A=B>), else 0 |
| >         | (A B--F)     | F: 1 if (A>B>), else 0 |
| 0=        | (N--F)       | F: 1 if (N=0), else 0 |
| and       | (A B--N)     | N: A AND B |
| or        | (A B--N)     | N: A OR  B |
| xor       | (A B--N)     | N: A XOR B |
| com       | (A--B)       | N: A with all bits flipped |
| for       | (T F--)      | Begin DO loop with bounds F and T |
| i         | (--I)        | I: Current DO LOOP index |
| next      | (--)         | Increment I, stop if I >= T |
| a         | (--N)        | Push a (a built-in variable ala MachineForth) |
| a+        | (--N)        | Push a, then increment it |
| >a        | (N--)        | Set a to N |
| b         | (--N)        | Push b (a built-in variable ala MachineForth) |
| b+        | (--N)        | Push b, then increment it |
| >b        | (N--)        | Set b to N |
| s         | (--N)        | Push s (s => alias for "source") |
| s+        | (--N)        | Push s, then increment it |
| >s        | (N--)        | Set s to N |
| d         | (--N)        | Push d (d => alias for "destination") |
| d+        | (--N)        | Push d, then increment it |
| >d        | (N--)        | Set d to N |
| >r        | (N--R:N)     | Move TOS to the return stack |
| r@        | (--N)        | N: return stack TOS |
| r>        | (R:N--N)     | Move return TOS to the stack |
| emit      | (C--)        | Output char C |
| :         | (--)         | Create a new word, set STATE=1 |
| ;         | (--)         | Compile EXIT, set STATE=0 |
| immediate | (--)         | Mark the last created word as IMMEDIATE |
| addword   | (--)         | Add the next word to the dictionary |
| timer     | (--N)        | N: Current time |
| see       | (--)         | Output the definition of a word |
| count     | (S--A N)     | A,N: Address and Count of string S |
| type      | (A N--)      | Print N chars starting at A |
| "         | (--A)        | COMPILE: Copy chars to " to VARS |
|           |              | RUN: push address A of string |
| ."        | (--)         | COMPILE: Copy chars to " to VARS |
|           |              | RUN: COUNT/TYPE string |
| rand      | (--N)        | N: a pseudo-random number (uses 13/17/5 XOR shift) |
| fopen     | (NM MD--H)   | NM: File Name, MD: Mode, H: File Handle |
| fclose    | (H--)        | H: File Handle |
| fread     | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
| fwrite    | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
| fgets     | (B SZ H--F)  | B: Buffer, SZ: Size, F: 0 if EOF/Error, else 1 |
| fload     | (NM--)       | NM: File Name to load |
| load      | (N--)        | N: Block number to load (block-NNN.c4) |
| loaded?   | (XT DE--)    | Stops a load if DE <> 0 |
| to-string | (N--A)       | Convert N to a string in the current BASE |
| .s        | (--)         | Display the stack |
| @c        | (A--N)       | Fetch N from CODE address A |
| !c        | (N A--)      | Store N to CODE address A |
| find      | (--XT DE)    | XT: word XT, DE: dict entry address (0 if not found) |
| system    | (A--)        | PC ONLY: A: String to send to the system() |
| bye       | (--)         | PC ONLY: Exit C4 |
