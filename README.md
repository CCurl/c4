# c4: A portable Forth system inspired by ColorForth and Tachyon

## ColorForth's influence on c4
c4 supports control characters in the whitespace that change the state.<br/>
c4 has 4 states: INTERPRET, COMPILE, DEFINE, AND COMMENT,<br/>
c4 also supports the standard state-change words.<br/>
c4 has 'A' and 'T' stacks, inspired by ColorForth's 'a' register.<br/>

| Ascii | Word  | State | Description|
|:--    |:--    |:--    |:-- |
|  $01  |  ]    |   1   | Compile |
|  $02  |  :    |   2   | Define |
|  $03  |  [    |   3   | Interpret/execute/immediate |
|  $04  |       |   4   | Comment |
|       |  (    |   4   | Comment, save current state |
|       |  )    |       | End comment, restores saved state |

**NOTE**: In the DEFINE state, c4 changes the state to COMPILE after adding the next word.<br/>
**NOTE**: Unlike ColorForth, ';' compiles EXIT and then changes the state to INTERPRET.<br/>

## Tachyon's influence on c4
In c4, a program is a sequence of WORD-CODEs. <br/>
A WORD-CODE is a 32-bit unsigned number (a DWORD). <br/>
Primitives are assigned numbers sequentially from 0 to **BYE**. <br/>
If a WORD-CODE is less than or equal to **BYE**, it is a primitive. <br/>
If the top 3 bits are set, it is a 29-bit unsigned literal, 0-$1FFFFFFF. <br/>
If it is between **BYE**, and $E0000000, it is the code address of a word to execute. <br/>

## Building c4
### Windows
- There is a Visual Studio solution file, c4.sln (either 32- or 64-bit)
### Linux
- 32-bit: There is a makefile; use 'ARCH=32 make'
- 64-bit: There is a makefile; use 'make'

## c4 memory usage
c4 provides a single memory area with size 'mem-sz' (see c4.h, MEM_SZ).
- It is divided into 3 areas as follows **[CODE][VARS][Dictionary]**.
- The **CODE** area is an aray of WORD-CODEs starting at the beginning of the memory.
- The **VARS** area is defined to begin at address **&memory[CODE_SLOTS*WC_SZ]**.
- The **Dictionary** starts at the end and grows downward.
- The size of the CODE area is 'code-sz' (see c4.h, CODE_SLOTS).
- `here` is an offset into the **CODE** area, the next slot to be allocated.
- `last` is an also offset into the memory area.
- `vhere` is the absolute address of the first free byte the **VARS** area.
- Use `->memory` to turn an offset into an address into the memory area.
- **NOTE**: CODE slots 0-25 (`0 wc@` .. `25 wc@`) are reserved for c4 system values.
- **NOTE**: CODE slots 26-75 (`26 wc@` .. `75 wc@`) are unused by c4.
- **NOTE**: These are free for the application to use as desired.
- **NOTE**: Use `wc@` and `wc!` to get and set WORD-CODE values in the **CODE** area.

| WORD    | STACK | DESCRIPTION |
|:--      |:--    |:-- |
| memory  | (--A) | A: starting address of the c4 memory |
| vars    | (--A) | A: starting address of the VARS area |
| mem-sz  | (--N) | N: size in BYTEs of the c4 memory |
| dstk-sz | (--N) | N: size in CELLs of the DATA and RETURN stacks |
| tstk-sz | (--N) | N: size in CELLs of the A and T stacks |
| wc-sz   | (--N) | N: size in BYTEs of a WORD-CODE |
| de-sz   | (--N) | N: size in BYTEs of a dictionary entry |
| (dsp)   | (--N) | N: CODE slot for the data stack pointer |
| (rsp)   | (--N) | N: CODE slot for the return stack pointer |
| (lsp)   | (--N) | N: CODE slot for the loop stack pointer |
| (tsp)   | (--N) | N: CODE slot for the T stack pointer |
| (asp)   | (--N) | N: CODE slot for the A stack pointer |
| (here)  | (--N) | N: CODE slot for the HERE variable |
| (vhere) | (--A) | A: address of the VHERE variable |
| (last)  | (--N) | N: CODE slot for the LAST variable |
| base    | (--N) | N: CODE slot for the BASE variable |
| state   | (--N) | N: CODE slot for the STATE variable |

## c4 Strings
Strings in c4 are NULL-terminated with no count byte.<br/>

## Format specifiers in `ftype` and `."`
Similar to the printf() function in C, c4 supports formatted output using '%'. <br/>
For example `: ascii dup dup dup ." char %c, decimal #%d, binary: %%%b, hex: $%x%n" ;`.

| Format | Stack | Description |
|:--     |:--    |:-- |
| %b     | (N--) | Print TOS in base 2. |
| %c     | (N--) | EMIT TOS. |
| %d     | (N--) | Print TOS in base 10. |
| %e     | (--)  | EMIT `escape` (#27). |
| %i     | (N--) | Print TOS in the current base. |
| %n     | (--)  | Print CR/LF (13/10). |
| %q     | (--)  | EMIT `"` (#34). |
| %s     | (A--) | Print TOS as a string (formatted). |
| %S     | (A--) | Print TOS as a string (unformatted). |
| %x     | (N--) | Print TOS in base 16. |
| %[x]   | (--)  | EMIT [x]. |

## The A stack
c4 includes an A stack. <br/>
This is somewhat similar to MachineForth's operations for 'a', but in c4, it is a stack.<br/>
The size of the A stack is 'tstk-sz' (see c4.h, TSTK_SZ).<br/>

| WORD  | STACK | DESCRIPTION |
|:--    |:--    |:-- |
| >a    | (N--) | Push N onto the A stack. |
| a!    | (N--) | Set A-TOS to N. |
| a@    | (--N) | N: copy of A-TOS. |
| a@+   | (--N) | N: copy of A-TOS, then increment A-TOS. |
| a@-   | (--N) | N: copy of A-TOS, then decrement A-TOS. |
| a>    | (--N) | Pop N from the A stack. |
| adrop | (--)  | Drop A-TOS |

## The T Stack
c4 includes a T stack, with same ops as the A stack. <br/>
Note that there are also additional words for the return stack. <br/>

| WORD  | STACK | DESCRIPTION |
|:--    |:--    |:-- |
| >t    | (N--) | Push N onto the T stack. |
| t!    | (N--) | Set T-TOS to N. |
| t@    | (--N) | N: copy of T-TOS. |
| t@+   | (--N) | N: copy of T-TOS, then increment T-TOS. |
| t@-   | (--N) | N: copy of T-TOS, then decrement T-TOS. |
| t>    | (--N) | Pop N from the T stack. |
| tdrop | (--)  | Drop T-TOS |

## Inline words
In c4, an "INLINE" word is similar to a macro. When compiling a word that is INLINE, c4 copies the contents of the word (up to, but not including the first EXIT) to the target, as opposed to compiling a CALL to the word. This improves performance, but uses extra space.

**Note that if a word might have an embedded 7 (EXIT) in its implementation (eg - a byte in an address for example), then it should not be marked as INLINE.**

## c4 WORD-CODE primitives
Stack effect notation conventions:

| TERM     | DESCRIPTION |
|:--       |:-- |
| SZ/NM/MD | String, uncounted, NULL terminated |
| A        | Address |
| C        | Number, 8-bits |
| WC       | WORD-CODE, 32-bits |
| N/X/Y    | Number, CELL sized |
| F        | Flag: 0 means false, <>0 means true |
| FH       | File handle: 0 means no file |
| I        | For loop index counter |

The primitives:

| WORD      | STACK        | DESCRIPTION |
|:--        |:--           |:-- |
| (lit)     | (--WC)       | WC: WORD-CODE for LIT primitive |
| (jmp)     | (--WC)       | WC: WORD-CODE for JMP primitive |
| (jmpz)    | (--WC)       | WC: WORD-CODE for JMPZ primitive |
| (jmpnz)   | (--WC)       | WC: WORD-CODE for JMPNZ primitive |
| (njmpz)   | (--WC)       | WC: WORD-CODE for NJMPZ primitive |
| (njmpnz)  | (--WC)       | WC: WORD-CODE for NJMPNZ primitive |
| (exit)    | (--WC)       | WC: WORD-CODE for EXIT primitive |
| exit      | (--)         | EXIT word |
| dup       | (X--X X)     | Duplicate TOS (Top-Of-Stack) |
| swap      | (X Y--Y X)   | Swap TOS and NOS (Next-On-Stack) |
| drop      | (N--)        | Drop TOS |
| over      | (N X--N X N) | Push NOS |
| @         | (A--N)       | N: the CELL at absolute address A |
| c@        | (A--C)       | C: the CHAR at absolute address A |
| d@        | (A--D)       | D: the DWORD at absolute address A |
| wc@       | (N--WC)      | WC: the WORD-CODE in CODE slot N |
| !         | (N A--)      | Store CELL N to absolute address A |
| c!        | (C A--)      | Store CHAR C to absolute address A |
| d!        | (D A--)      | Store DWORD D to absolute address A |
| wc!       | (WC N--)     | Store WORD-CODE WC to CODE slot N |
| +         | (X Y--N)     | N: X + Y |
| -         | (X Y--N)     | N: X - Y |
| *         | (X Y--N)     | N: X * Y |
| /         | (X Y--N)     | N: X / Y (integer division) |
| /mod      | (X Y--M Q)   | M: X modulo Y, Q: quotient of X / Y |
| 1+        | (X--Y)       | Increment TOS |
| 1-        | (X--Y)       | Decrement TOS |
| <         | (X Y--F)     | F: 1 if (X < Y), else 0 |
| =         | (X Y--F)     | F: 1 if (X = Y), else 0 |
| >         | (X Y--F)     | F: 1 if (X > Y), else 0 |
| 0=        | (N--F)       | F: 1 if (N=0), else 0 |
| and       | (X Y--N)     | N: X AND Y |
| or        | (X Y--N)     | N: X OR  Y |
| xor       | (X Y--N)     | N: X XOR Y |
| com       | (X--Y)       | Y: X with all bits flipped (complement) |
| for       | (N--)        | Begin FOR loop with bounds 0 and N-1. |
| i         | (--I)        | N: Current FOR loop index. |
| next      | (--)         | Increment I. If I >= N, exit, else start loop again. |
| >r        | (N--)        | Push N onto the return stack |
| r!        | (N--)        | Set R-TOS to N |
| r@        | (--N)        | N: copy of R-TOS |
| r@+       | (--N)        | N: copy of R-TOS, then increment it |
| r@-       | (--N)        | N: copy of R-TOS, then decrement it |
| r>        | (--N)        | Pop N from the return stack |
| rdrop     | (--)         | Drop R-TOS |
| >t        | (N--)        | Push N onto the T stack |
| t!        | (N--)        | Set T-TOS to N |
| t@        | (--N)        | N: copy of T-TOS |
| t@+       | (--N)        | N: copy of T-TOS, then increment T-TOS |
| t@-       | (--N)        | N: copy of T-TOS, then decrement T-TOS |
| t>        | (--N)        | Pop N from the T stack |
| tdrop     | (--)         | Drop T-TOS |
| >a        | (N--)        | Push N onto the A stack |
| a!        | (N--)        | Set A-TOS to N |
| a@        | (--N)        | N: copy of A-TOS |
| a@+       | (--N)        | N: copy of A-TOS, then increment A-TOS |
| a@-       | (--N)        | N: copy of A-TOS, then decrement A-TOS |
| a>        | (--N)        | Pop N from the A stack |
| adrop     | (--)         | Drop A-TOS |
| emit      | (C--)        | Output char C |
| ;         | (--)         | Compile EXIT, set STATE=INTERPRET |
| lit,      | (N--)        | Compile a push of number N |
| next-wd   | (--A L)      | L: length of the next word (A) from the input stream |
|           |              | - If L=0, then A is an empty string (end of input) |
| immediate | (--)         | Mark the last created word as IMMEDIATE |
| inline    | (--)         | Mark the last created word as INLINE |
| outer     | (S--)        | Send string S to the c4 outer interpreter |
| addword   | (--)         | Add the next word to the dictionary |
| timer     | (--N)        | N: Current time |
| see X     | (--)         | Output the definition of word X |
| ztype     | (S--)        | Print string at S (unformatted) |
| ftype     | (S--)        | Print string at S (formatted) |
| s-cpy     | (D S--D)     | Copy string S to D |
| s-eq      | (D S--F)     | F: 1 if string S is equal to D (case sensitive) |
| s-eqi     | (D S--F)     | F: 1 if string S is equal to D (NOT case sensitive) |
| s-len     | (S--N)       | N: Length of string S |
| z" str"   | (--)         | - COMPILE: generate code to push address of `str` (vhere) |
|           | (--A)        | - RUN: A=address of `str` |
|           | (--A)        | - INTERPRET: A=address of `str` (only 1 string supported) |
| ." msg"   | (--)         | - COMPILE: execute `z"`, compile `ftype` |
|           | (--)         | - RUN: perform `ftype` on `msg` |
|           | (--)         | - INTERPRET: output `msg` using `ftype` |
| find      | (--XT A)     | XT: Execution Token, A: Dict Entry address (0 0 if not found) |
| loaded?   | (XT A--)     | Stops current load if A <> 0 (see `find`) |
| fopen     | (NM MD--FH)  | NM: File Name, MD: Mode, FH: File Handle (0 if error/not found) |
| fclose    | (FH--)       | FH: File Handle to close |
| fdelete   | (NM--)       | NM: File Name to delete |
| fread     | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars read |
| fwrite    | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars written |
| fgets     | (A N FH--X)  | A: Buffer, N: Size, X: num chars read (0 if EOF/Error) |
| include X | (--)         | Load file named X (X: next word) |
| load      | (N--)        | N: Block number to load (file named "block-NNN.fth") |
| load-next | (N--)        | Close the current block and load block N next |
| system    | (S--)        | PC ONLY: S: String to send to `system()` |
| bye       | (--)         | PC ONLY: Exit c4 |

## c4 default words
Default words are defined in function `sys_load()` in file sys-load.c.<br/>
For details, or to add or change the default words, modify that function.
