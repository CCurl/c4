# c4: a portable Forth system inspired by MachineForth and Tachyon

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
- The dictionary area can store up to 65536 bytes, 16-bit index (DICT-SZ).
- The variables area can store up to CELL bytes, CELL index (VARS-SZ).
- Use `->code`, `->vars`, or `->dict` to turn an offset into an address.

16-bit system variables are located in the code area.<br/>
They are set/retrieved using `@c` and `!c` (e.g. `: here (here) @c ;`).<br/>


| WORD   | STACK   | DESCRIPTION |
|:--     |:--      |:--          |
| (sp)   | (--N)   | Offset of the parameter stack pointer |
| (rsp)  | (--N)   | Offset of the return stack pointer |
| (here) | (--N)   | Offset of the HERE variable |
| (last) | (--N)   | Offset of the LAST variable |
| base   | (--N)   | Offset of the BASE variable |
| state  | (--N)   | Offset of the STATE variable |
| (lex)  | (--N)   | Offset of the LEX (the current lexicon) |
| (lsp)  | (--N)   | Offset of the loop stack pointer |
| (tsp)  | (--N)   | Offset of the third stack pointer |

## C4 Strings

Strings in C4 are both counted and NULL terminated.

## MachineForth influences

C4 includes opcodes for 3 built-in cells (`a`, `s`, `d`). <br/>
This is in the spirit of MachineForth, that has opcodes for an 'a' register. <br/>
's' is shorthand for 'source', but can be used for anything. <br/>
'd' is shorthand for 'destination', but can be used for anything. <br/>
- They all support set (`>a`, `>s`, `>d`)
- They all support get (`a>`, `s>`, `d>`)
- They all support get with increment (`a+`, `s+`, `d+`)

C4 also includes third stack, with same ops as the return stack. (`>t`, `t@`, `t>`). <br/>
This third stack can be used for any purpose. <br/>
I use it to save and restore `a`, `s`, and `d` (see `+a/-a` for details). <br/>

## C4 WORD-CODE primitives

| WORD      | STACK        | DESCRIPTION |
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
| i         | (--I)        | I: Current FOR LOOP index |
| next      | (--)         | Increment I, stop if I >= T |
| a>        | (--N)        | Push a (a built-in variable ala MachineForth) |
| a+        | (--N)        | Push a, then increment it |
| >a        | (N--)        | Set a to N |
| s>        | (--N)        | Push s (s is an alias for "source") |
| s+        | (--N)        | Push s, then increment it |
| >s        | (N--)        | Set s to N |
| d>        | (--N)        | Push d (d is an alias for "destination") |
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
| s-cpy     | (D S--D)     | Copy string S to D |
| s-eq      | (D S--F)     | F: 1 if string S is equal to D (case sensitive) |
| s-eqi     | (D S--F)     | F: 1 if string S is equal to D (NOT case sensitive) |
| z"        | (--)         | -COMPILE: Copy chars up to next " to VARS (NOT counted) |
|           | (--A)        | -RUN: push address A of string |
| s"        | (--)         | -COMPILE: Copy chars up to next " to VARS (counted) |
|           | (--A)        | -RUN: push address A of string |
| ."        | (--)         | -COMPILE: Copy chars up to next " to VARS |
|           | (--)         | -RUN: COUNT/TYPE on string |
| rand      | (--N)        | N: a pseudo-random number (uses XOR shift) |
| fopen     | (NM MD--FH)  | NM: File Name, MD: Mode, FH: File Handle (0 if error/not found) |
|           |              |     NOTE: NM and MD are NOT counted, use `z"` |
| fclose    | (FH--)       | FH: File Handle |
| fread     | (B SZ FH--N) | B: Buffer, SZ: Size, FH: File Handle |
| fwrite    | (B SZ FH--N) | B: Buffer, SZ: Size, FH: File Handle |
| fgets     | (B SZ FH--F) | B: Buffer, SZ: Size, F: 0 if EOF/Error, else 1 |
| fload     | (SZ--)       | SZ: File Name to load (NOT counted, use `z"`) |
| load      | (N--)        | N: Block number to load (file named "block-NNN.c4") |
| loaded?   | (XT DE--)    | Stops a load if DE <> 0 (see `find`) |
| to-string | (N--A)       | Convert N to string A in the current BASE |
| .s        | (--)         | Display the stack |
| @c        | (N--WC)      | Fetch unsigned 16-bit WC from CODE address N |
| !c        | (WC N--)     | Store unsigned 16-bit WC to CODE address N |
| find      | (--XT DE)    | XT: Execution Token, DE: Dict Entry address (0 0 if not found) |
| system    | (A--)        | PC ONLY: A: String to send to `system()` |
| bye       | (--)         | PC ONLY: Exit C4 |
