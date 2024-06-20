# c4: a portable Forth system inspired by Tachyon

In C4, a program is a sequence of WORD-CODEs. <br/>
A `WORD-CODE` is a 16-bit unsigned number (0..65535). <br/>
Primitives are assigned numbers sequentially from 0 to `BYE`. <br/>
If a WORD-CODE is less than or equal to `BYE`, it is a primitive. <br/>
If the top 2 bits are set ($C000), it is a (14-bit unsigned) literal. <br/>
If it is greater than `BYE`, it is the code address of a word to execute. <br/>

## CELLs in C4
A `CELL` is either 32-bits or 64-bits, depending on the target system.
- Linux 32-bit (-m32): a CELL is 32-bits.
- Linux 64-bit (-m64): a CELL is 64-bits.
- Windows 32-bit (x86): a CELL is 32-bits.
- Windows 64-bit (x64): a CELL is 64-bits.
- Development boards: a CELL is 32-bits.

## C4 memory areas

C4 uses three memory areas:
- The code area can store up to 65536 WORD-CODEs, 16-bit index. (CODE-SZ).
- The dictionary area can be up to 65536 bytes, 16-bit index (DICT-SZ).
- The variables area can be up to CELL bytes, CELL index (VARS-SZ).
- Use `->code`, `->vars`, or `->dict` to turn a relative address into a real one.

Some 16-bit system variables are located in the code area.<br/>
The are set/retrieved using words `@c` and `!c`.
- here  `(here)`
- last  `(last)`
- base  `base`
- state `state`
- lex   `(lex)`
- sp    `(sp)`
- rsp   `(rsp)`

## C4 Strings

Strings in C4 are both counted and NULL terminated.

## MachineForth influences

C4 includes 3 built-in variables as primitives (`a`, `s`, `d`). <br/>
This is in the spirit of MachineForth, that has opcodes for the 'a' register. <br/>
C4 also includes third stack, same ops at the return stack. (`>t`, `t@`, `t>`). <br/>
This third stack can be used for any purpose. <br/>
I use it to save and restore `a`, `s`, and `d` (see `+a` for details). <br/>
's' is shorthand for 'source', but can be used for anything. <br/>
'd' is shorthand for 'destination', but can be used for anything. <br/>
- They all support set (`>a`, `>s`, `>d`)
- They all support get (`a`, `s`, `d`)
- They all support get with increment (`a+`, `s+`, `d+`)

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
| @         | (A--N)       | N: the CELL at absolute address A |
| c@        | (A--C)       | C: the CHAR at absolute address A |
| w@        | (A--W)       | W: the WORD at absolute address A |
| !         | (N A--)      | Store CELL N to absolute address A |
| c!        | (C A--)      | Store CHAR C to absolute address A |
| w!        | (W A--)      | Store WORD W to absolute address A |
| +         | (A B--C)     | C: A + B |
| -         | (A B--C)     | C: A - B |
| *         | (A B--C)     | C: A * B |
| /         | (A B--C)     | C: A / B (integer division) |
| /mod      | (A B--M Q)   | M: A modulo B, Q: quotient of A / B |
| 1+        | (A--B)       | Increment TOS |
| 1-        | (A--B)       | Decrement TOS |
| <         | (A B--F)     | F: 1 if (A < B), else 0 |
| =         | (A B--F)     | F: 1 if (A = B), else 0 |
| >         | (A B--F)     | F: 1 if (A > B), else 0 |
| 0=        | (N--F)       | F: 1 if (N=0), else 0 |
| and       | (A B--N)     | N: A AND B |
| or        | (A B--N)     | N: A OR  B |
| xor       | (A B--N)     | N: A XOR B |
| com       | (A--B)       | N: A with all bits flipped (complement) |
| for       | (N--)        | Begin FOR loop with bounds 0 and N |
| i         | (--I)        | I: Current DO LOOP index |
| next      | (--)         | Increment I, stop if I >= T |
| a         | (--N)        | Push a (a built-in variable ala MachineForth) |
| a+        | (--N)        | Push a, then increment it |
| >a        | (N--)        | Set a to N |
| s         | (--N)        | Push s (s: alias for "source") |
| s+        | (--N)        | Push s, then increment it |
| >s        | (N--)        | Set s to N |
| d         | (--N)        | Push d (d: alias for "destination") |
| d+        | (--N)        | Push d, then increment it |
| >d        | (N--)        | Set d to N |
| >r        | (N--R:N)     | Move TOS to the return stack |
| r@        | (--N)        | N: return stack TOS |
| r>        | (R:N--N)     | Move return TOS to the stack |
| >t        | (N--T:N)     | Move TOS to the third stack |
| t@        | (--N)        | N: third stack TOS |
| t>        | (T:N--N)     | Move third TOS to the stack |
| emit      | (C--)        | Output char C |
| :         | (--)         | Create a new word, set STATE=1 |
| ;         | (--)         | Compile EXIT, set STATE=0 |
| immediate | (--)         | Mark the last created word as IMMEDIATE |
| addword   | (--)         | -COMPILE: Add the next word to the dictionary |
|           | (--A)        | -RUN: push VHERE |
| timer     | (--N)        | N: Current time |
| see       | (--)         | Output the definition of a word |
| count     | (S--A N)     | A,N: address and count of chars in string S |
| type      | (A N--)      | Print N chars starting at A |
| ztype     | (A--)        | Print uncounted, NULL terminated string at A |
| "         | (--)         | -COMPILE: Copy chars up to " to VARS |
|           | (--A)        | -RUN: push address A of string |
| ."        | (--)         | -COMPILE: Copy chars to up " to VARS |
|           | (--)         | -RUN: COUNT/TYPE string |
| rand      | (--N)        | N: a pseudo-random number (uses XOR shift) |
| fopen     | (NM MD--H)   | NM: File Name, MD: Mode, H: File Handle |
| fclose    | (H--)        | H: File Handle (0 if error or not found) |
| fread     | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
| fwrite    | (B SZ H--N)  | B: Buffer, SZ: Size, H: File Handle |
| fgets     | (B SZ H--F)  | B: Buffer, SZ: Size, F: 0 if EOF/Error, else 1 |
| fload     | (NM--)       | NM: File Name to load |
| load      | (N--)        | N: Block number to load (block-NNN.c4) |
| loaded?   | (XT DE--)    | Stops a load if DE <> 0 |
| to-string | (N--A)       | Convert N to a string in the current BASE |
| .s        | (--)         | Display the stack |
| @c        | (A--N)       | Fetch unsiged 16-bit N from CODE address A |
| !c        | (N A--)      | Store unsiged 16-bit N to CODE address A |
| find      | (--XT DE)    | XT: word XT, DE: dict entry address (0 if not found) |
| system    | (A--)        | PC ONLY: A: String to send to `system()` |
| bye       | (--)         | PC ONLY: Exit C4 |
