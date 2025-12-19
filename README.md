# c4: A portable Forth system inspired by ColorForth and Tachyon

## ColorForth's influence on c4
c4 supports control characters in the whitespace that change the state.<br/>
c4 has 'A', 'B' and 'T' stacks, inspired by ColorForth's 'a' register.<br/>
c4 has 4 states: INTERPRET, COMPILE, DEFINE, AND COMMENT,<br/>
c4 also supports the standard state-change words.<br/>

| Ascii | Word  | State | Description|
|:--    |:--    |:--    |:-- |
|  $01  |  ]    |   1   | Compile |
|  $02  |  :    |   2   | Define |
|  $03  |  [    |   3   | Interpret/execute/immediate |
|  $04  |       |   4   | Comment |

**NOTE**: In the DEFINE state, c4 changes the state to COMPILE after adding the next word.<br/>
**NOTE**: Unlike ColorForth, ';' compiles EXIT and then changes the state to INTERPRET.<br/>
**NOTE**: The `(` word simply skips to the next `)` word, no nesting.<br/>

## Tachyon's influence on c4
In c4, a program is a sequence of OPCODEs. <br/>
An OPCODE is an unsigned CELL, 32 or 64 bits. <br/>
Primitives are assigned numbers sequentially from 0 to **BYE**. <br/>
If the OPCODE is less than or equal to **BYE**, it is a primitive. <br/>
Else if the top 3 bits are set, it is an unsigned literal. <br/>
Else, it is the offset in the CODE area (XT) of a word to execute. <br/>

## c4's built-in block editor
c4 has a built-in editor. See [Editor.md](Editor.md) for details. <br/>
The editor can be excluded from c4 by undefining **EDITOR** in c4.h. <br/>
It is built-in so that the editor is available when running c4 from any folder. <br/>
In c4, the size if a block is 3072 bytes (3x1024). <br/>
The editor is 32 lines, 96 columns, and has a VI like feel. <br/>

## Building c4
### Windows
- There is a Visual Studio solution file, **c4.sln** (either 32- or 64-bit)
### Linux and other similar systems
- There is a makefile.
- The default architecture is 32-bits. That is a little faster on my systems.
- 32-bit: use 'make'
- 64-bit: use 'ARCH=64 make'

## c4 memory usage
c4 provides a single memory area with size 'mem-sz' (see c4.h, MEM_SZ).
- It is divided into 3 areas as follows **[CODE][VARS][Dictionary]**.
- The **CODE** area is an array of OPCODEs starting at the beginning of the memory.
- The **VARS** area is defined to begin at address **&memory[CODE_SLOTS*WC_SZ]**.
- The **Dictionary** starts at the end and grows downward.
- The size of the CODE area is 'code-sz' (see c4.h, CODE_SLOTS).
- `here` is an offset into the **CODE** area, the next slot to be allocated.
- `last` is the address of the most recently created word.
- `vhere` is the address of the first free byte the **VARS** area.
- Use `->memory` to turn an offset into an address into the memory area.
- Use `->code` to turn an offset into an address of an OPCODE.
- **NOTE**: CODE slots 0-25 (`0 wc@` .. `25 wc@`) are reserved for c4 system values.
- **NOTE**: CODE slots 26-BYE (`26 wc@` .. `<bye> wc@`) are unused by c4.
- **NOTE**: These are free for the application to use as desired.
- **NOTE**: Use `wc@` and `wc!` to get and set OPCODE values in the **CODE** area.

| WORD    | STACK | DESCRIPTION |
|:--      |:--    |:-- |
| memory  | (--A) | A: starting address of the c4 memory |
| vars    | (--A) | A: starting address of the VARS area |
| mem-sz  | (--N) | N: size in BYTEs of the c4 memory |
| code-sz | (--N) | N: size in CODE entries of the c4 CODE area |
| dstk-sz | (--N) | N: size in CELLs of the DATA and RETURN stacks |
| tstk-sz | (--N) | N: size in CELLs of the A and T stacks |
| wc-sz   | (--N) | N: size in BYTEs of an OPCODE |
| de-sz   | (--N) | N: size in BYTEs of a dictionary entry |
| (dsp)   | (--N) | N: CODE slot for the data stack pointer |
| (rsp)   | (--N) | N: CODE slot for the return stack pointer |
| (lsp)   | (--N) | N: CODE slot for the loop stack pointer |
| (tsp)   | (--N) | N: CODE slot for the T stack pointer |
| (asp)   | (--N) | N: CODE slot for the A stack pointer |
| (bsp)   | (--N) | N: CODE slot for the B stack pointer |
| (here)  | (--N) | N: CODE slot for the HERE variable |
| (vhere) | (--A) | A: address of the VHERE variable |
| (last)  | (--A) | A: address of the LAST variable |
| base    | (--N) | N: CODE slot for the BASE variable |
| state   | (--N) | N: CODE slot for the STATE variable |
| (block) | (--N) | N: CODE slot for the BLOCK variable |

## c4 Strings
Strings in c4 are NULL-terminated with no count byte.<br/>

## Format specifiers in `ftype` and `."`
Similar to the printf() function in C, c4 supports formatted output using '%'. <br/>
For example `: ascii ( n-- ) dup dup dup ." char %c, decimal #%d, binary: %%%b, hex: $%x%n" ;`.

| Format | Stack | Description |
|:--     |:--    |:-- |
| %b     | (N--) | Print TOS in base 2. |
| %c     | (N--) | EMIT TOS. |
| %d     | (N--) | Print TOS in base 10. |
| %e     | (--)  | EMIT `escape` (#27). |
| %i     | (N--) | Print TOS in the current base. |
| %n     | (--)  | Print CR/LF (13/10). |
| %q     | (--)  | EMIT a `double-quote` (#34). |
| %s     | (A--) | Print TOS as a string (formatted). |
| %S     | (A--) | Print TOS as a string (unformatted). |
| %x     | (N--) | Print TOS in base 16. |
| %B     | (--)  | Change foreground to Blue |
| %G     | (--)  | Change foreground to Green |
| %P     | (--)  | Change foreground to Purple |
| %R     | (--)  | Change foreground to Reg |
| %W     | (--)  | Change foreground to White |
| %Y     | (--)  | Change foreground to Yellow |
| %[x]   | (--)  | EMIT [x]. |

## The A, B and T stacks
c4 includes A, B and T stacks. <br/>
These are similar to ColorForth's operations for 'a', but in c4, they are stacks.<br/>
The size of the stacks is 'tstk-sz' (see c4.h, TSTK_SZ).<br/>
Note that there are also additional words for the return stack. <br/>

| WORD  | STACK | DESCRIPTION |
|:--    |:--    |:-- |
| >a    | (N--) | Push N onto the A stack. |
| a!    | (N--) | Set A-TOS to N. |
| a@    | (--N) | N: copy of A-TOS. |
| a@+   | (--N) | N: copy of A-TOS, then increment A-TOS. |
| a@-   | (--N) | N: copy of A-TOS, then decrement A-TOS. |
| a>    | (--N) | Pop N from the A stack. |
| adrop | (--)  | Drop A-TOS. |

## Inline words
In c4, an "INLINE" word is similar to a macro. When compiling a word that is INLINE, c4 copies the contents of the word (up to, but not including the first EXIT) to the target, as opposed to compiling a CALL to the word. This improves performance, but uses extra space.

**Note that if a word might have an embedded 7 (EXIT) in its implementation (eg - a word in an address for example), then it should not be marked as INLINE.**

## Temporary words
c4 provides 10 temporary words, 't0' thru 't9'.
- Defining a temporary word does not add an entry to the dictionary.
- Temporary words are intended to be helpful in factoring code.
- Their names are case-sensitive (t0-t9, not T0-T9).
- A temporary word can be redefined as often as desired.
- When redefined, code references to the previous definition are unchanged.
- t0 thru t5 are 'normal', t6 thru t9 are 'inline'.

## c4 OPCODE primitives
The primitives:

| WORD      | STACK        | DESCRIPTION |
|:--        |:--           |:-- |
| :         | (--)  ST=2   | Set STATE to DEFINE (RED) |
| ;         | (--)  ST=3   | Compile EXIT and set STATE to INTERPRET (YELLOW) |
| [         | (--)  ST=3   | Set STATE to INTERPRET (YELLOW) |
| ]         | (--)  ST=1   | Set STATE to COMPILE (GREEN) |
| (         | (--)  ST=4   | Remember STATE. Set STATE to COMMENT (WHITE) |
| )         | (--)  ST=?   | Set STATE to previous state |
| (lit)     | (--WC)       | WC: OPCODE for the LIT primitive |
| (jmp)     | (--WC)       | WC: OPCODE for the JMP primitive |
| (jmpz)    | (--WC)       | WC: OPCODE for the JMPZ primitive |
| (jmpnz)   | (--WC)       | WC: OPCODE for the JMPNZ primitive |
| (njmpz)   | (--WC)       | WC: OPCODE for the NJMPZ primitive |
| (njmpnz)  | (--WC)       | WC: OPCODE for the NJMPNZ primitive |
| (exit)    | (--WC)       | WC: OPCODE for the EXIT primitive |
| exit      | (--)         | EXIT word |
| dup       | (N--N N)     | Duplicate TOS (Top-Of-Stack) |
| swap      | (X Y--Y X)   | Swap TOS and NOS (Next-On-Stack) |
| drop      | (N--)        | Drop TOS |
| over      | (N X--N X N) | Push NOS |
| @         | (A--N)       | N: the CELL at absolute address A |
| c@        | (A--C)       | C: the CHAR at absolute address A |
| w@        | (A--W)       | W: the 16-bit WORD at absolute address A |
| d@        | (A--D)       | D: the 32-bit DWORD at absolute address A |
| wc@       | (N--WC)      | WC: the CODE entry in CODE slot N |
| !         | (N A--)      | Store CELL N to absolute address A |
| c!        | (C A--)      | Store CHAR C to absolute address A |
| w!        | (W A--)      | Store 16-bit WORD W to absolute address A |
| d!        | (D A--)      | Store 32-bit DWORD D to absolute address A |
| wc!       | (WC N--)     | Store WC to CODE slot N |
| +         | (X Y--N)     | N: X + Y |
| -         | (X Y--N)     | N: X - Y |
| *         | (X Y--N)     | N: X * Y |
| /         | (X Y--N)     | N: X / Y (integer division) |
| /mod      | (X Y--M Q)   | M: X modulo Y, Q: quotient of X / Y |
| 1+        | (X--Y)       | Increment TOS |
| 1-        | (X--Y)       | Decrement TOS |
| +!        | (N A--)      | Add N to the CELL at A |
| <         | (X Y--F)     | F: 1 if (X < Y), else 0 |
| =         | (X Y--F)     | F: 1 if (X = Y), else 0 |
| >         | (X Y--F)     | F: 1 if (X > Y), else 0 |
| 0=        | (N--F)       | F: 1 if (N=0), else 0 |
| and       | (X Y--N)     | N: X AND Y |
| or        | (X Y--N)     | N: X OR  Y |
| xor       | (X Y--N)     | N: X XOR Y |
| com       | (X--Y)       | Y: X with all bits flipped (complement) |
| for       | (CNT--)      | Begin FOR loop with bounds 0 and CNT-1. |
| i         | (--I)        | I: Current FOR loop index. |
| next      | (--)         | Increment I. If I >= CNT, exit, else start loop again. |
| unloop    | (--)         | Unwind the loop stack. **DOES NOT EXIT THE LOOP**. |
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
| >b        | (N--)        | Push N onto the B stack |
| b!        | (N--)        | Set B-TOS to N |
| b@        | (--N)        | N: copy of B-TOS |
| b@+       | (--N)        | N: copy of B-TOS, then increment B-TOS |
| b@-       | (--N)        | N: copy of B-TOS, then decrement B-TOS |
| b>        | (--N)        | Pop N from the B stack |
| bdrop     | (--)         | Drop B-TOS |
| emit      | (C--)        | Output char C |
| key       | (--C)        | Read char C from stdin, block if needed |
| key?      | (--F)        | F: 1 if key was pressed, else 0 |
| lit,      | (N--)        | Compile a push of number N |
| next-wd   | (--A L)      | L: length of the next word (A) from the input stream |
|           |              | - If L=0, then A is an empty string (end of input) |
| immediate | (--)         | Mark the last created word as IMMEDIATE |
| inline    | (--)         | Mark the last created word as INLINE |
| outer     | (S--)        | Send string S to the c4 outer interpreter |
| addword   | (--)         | Add the next word to the dictionary |
| timer     | (--N)        | N: Current time |
| ms        | (MS--)       | MS: Number of milliseconds to sleep |
| see X     | (--)         | Output the definition of word X |
| ztype     | (S--)        | Print string at S (unformatted) |
| ftype     | (S--)        | Print string at S (formatted) |
| s-cpy     | (D S--D)     | Copy string S to D |
| s-find    | (B L S--N)   | Find string L in B starting at char S. N = -1 if not found |
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
If _SYS_LOAD_ is not defined in file c4.h, load block-000.fth.
Else, load the words in function `sys_load()` in file sys-load.c.<br/>
For details, or to add or change the default words, modify that function.
