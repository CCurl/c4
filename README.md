# c4: a Forth system inspired by MachineForth and Tachyon

In C4, a program is a sequence of WORD-CODEs. <br/>
A `WORD-CODE` is a 32-bit unsigned number. <br/>
Primitives are assigned numbers sequentially from 0 to `BYE`. <br/>
If a WORD-CODE is less than or equal to `BYE`, it is a primitive. <br/>
If the top 3 bits are set ($Exxxxxxx), it is a 29-bit unsigned literal. <br/>
If it is between `BYE`, and $E0000000, it is the code address of a word to execute. <br/>

## CELLs in C4
A `CELL` is either 32-bits or 64-bits, depending on the target system.
- Linux 32-bit (-m32): a CELL is 32-bits.
- Linux 64-bit (-m64): a CELL is 64-bits.
- Windows 32-bit (x86): a CELL is 32-bits.
- Windows 64-bit (x64): a CELL is 64-bits.

## C4 memory areas
C4 provides two memory areas:
- The `code` area can store up to $1FFFFFFF 32-bit WORD-CODEs. (see `code-sz`).
  - **NOTE**: CODE slots 0-25 (`0 wc@ .. 25 wc@`) are reserved for C4 system values.
  - **NOTE**: CODE slots 25-75 (`25 wc@` .. `75 wc@`) are unused by C4.
  - **NOTE**: These are free for the application to use as desired.
  - **NOTE**: Use `wc@` and `wc!` to get and set WORD-CODE values in the code area.
  - `here` is an offset into the code area.
- The `vars` area can store up to CELL bytes (see `vars-sz`).
  - `vhere` is the address of the first free byte the vars area.
  - `last` is an offset into the vars area.
- Use `->code` and `->vars` to turn an offset into an address.

| WORD       | STACK   | DESCRIPTION |
|:--         |:--      |:--          |
| (dsp)      | (--N)   | CODE slot for the data stack pointer |
| (rsp)      | (--N)   | CODE slot for the return stack pointer |
| (lsp)      | (--N)   | CODE slot for the loop stack pointer |
| (tsp)      | (--N)   | CODE slot for the T stack pointer |
| (asp)      | (--N)   | CODE slot for the A stack pointer |
| (here)     | (--N)   | CODE slot for the HERE variable |
| (last)     | (--N)   | CODE slot for the LAST variable |
| base       | (--N)   | CODE slot for the BASE variable |
| state      | (--N)   | CODE slot for the STATE variable |

## C4 Strings
Strings in C4 are NULL-terminated strings with no count byte (ztype).<br/>

## Format specifiers in `ftype` and `."`
Similar to the printf() function in C, C4 supports formatted output using '%'. <br/>
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
| %s     | (A--) | Print TOS as a string (uncounted, formatted). |
| %S     | (A--) | Print TOS as a string (uncounted, unformatted). |
| %x     | (N--) | Print TOS in base 16. |
| %[x]   | (--)  | EMIT [x]. |

## The A stack
C4 includes an A stack. <br/>
This is somewhat similar to MachineForth's operations for 'a', but in C4, it is a stack.<br/>
The size of the A stack is configurable (see `tstk-sz`).<br/>

| WORD  | STACK  | DESCRIPTION |
|:--    |:--     |:-- |
| `>a`  | (N--)  | Push N onto the A stack. |
| `a!`  | (N--)  | Set A-TOS to N. |
| `a@`  | (--N)  | N: copy of A-TOS. |
| `a@+` | (--N)  | N: copy of A-TOS, then increment A-TOS. |
| `a@-` | (--N)  | N: copy of A-TOS, then decrement A-TOS. |
| `a>`  | (--N)  | Pop N from the A stack. |

## The T Stack
C4 includes a T stack, with same ops as the T stack. <br/>
Note that there are also additional words for the return stack. <br/>

| WORD  | STACK  | DESCRIPTION |
|:--    |:--     |:-- |
| `>t`  | (N--)  | Push N onto the T stack. |
| `t!`  | (N--)  | Set T-TOS to N. |
| `t@`  | (--N)  | N: copy of T-TOS. |
| `t@+` | (--N)  | N: copy of T-TOS, then increment T-TOS. |
| `t@-` | (--N)  | N: copy of T-TOS, then decrement T-TOS. |
| `t>`  | (--N)  | Pop N from the T stack. |

## C4 WORD-CODE primitives
Stack effect notation conventions:

| TERM     | DESCRIPTION |
|:--       |:-- |
| SZ/NM/MD | String, uncounted, NULL terminated |
| A        | Address |
| C        | Number, 8-bits |
| WC       | WORD-CODE, 32-bits |
| N/X/Y    | Number, CELL sized |
| F        | Flag: 0 mean0 false, <>0 means true |
| R        | Register number |
| FH       | File handle: 0 means no file |
| I        | For loop index counter |

The primitives:

| WORD      | STACK        | DESCRIPTION |
|:--        |:--           |:-- |
| (lit)     | (--W)        | W: WORD-CODE for LIT primitive |
| (jmp)     | (--W)        | W: WORD-CODE for JMP primitive |
| (jmpz)    | (--W)        | W: WORD-CODE for JMPZ primitive |
| (jmpnz)   | (--W)        | W: WORD-CODE for JMPNZ primitive |
| (njmpz)   | (--W)        | W: WORD-CODE for NJMPZ primitive |
| (njmpnz)  | (--W)        | W: WORD-CODE for NJMPNZ primitive |
| (exit)    | (--W)        | W: WORD-CODE for EXIT primitive |
| exit      | (--)         | EXIT word |
| dup       | (X--X X)     | Duplicate TOS (Top-Of-Stack) |
| swap      | (X Y--Y X)   | Swap TOS and NOS (Next-On-Stack) |
| drop      | (N--)        | Drop TOS |
| over      | (N X--N X N) | Push NOS |
| @         | (A--N)       | N: the CELL at absolute address A |
| c@        | (A--C)       | C: the CHAR at absolute address A |
| d@        | (A--D)       | D: the DWORD at absolute address A |
| wc@       | (N--WC)      | Fetch word-code WC from CODE slot N |
| !         | (N A--)      | Store CELL N to absolute address A |
| c!        | (C A--)      | Store CHAR C to absolute address A |
| d!        | (D A--)      | Store DWORD D to absolute address A |
| wc!       | (WC N--)     | Store word-code WC to CODE slot N |
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
| for       | (N--)        | Begin FOR loop with bounds 0 and N. |
| i         | (--I)        | N: Current FOR loop index. |
| next      | (--)         | Increment I. If I < N, start loop again, else exit. |
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
| :         | (--)         | Create a new word, set STATE=1 |
| ;         | (--)         | Compile EXIT, set STATE=0 |
| immediate | (--)         | Mark the last created word as IMMEDIATE |
| inline    | (--)         | Mark the last created word as INLINE |
| outer     | (S--)        | Send string S to the C4 outer interpreter |
| addword   | (--)         | Add the next word to the dictionary |
| timer     | (--N)        | N: Current time |
| see X     | (--)         | Output the definition of word X |
| ztype     | (SZ--)       | Print string at SZ (uncounted, unformatted) |
| ftype     | (SZ--)       | Print string at SZ (uncounted, formatted) |
| s-cpy     | (D S--D)     | Copy string S to D, counted |
| s-eq      | (D S--F)     | F: 1 if string S is equal to D (case sensitive) |
| s-eqi     | (D S--F)     | F: 1 if string S is equal to D (NOT case sensitive) |
| s-len     | (S--N)       | N: Length of string S |
| z"        | (--)         | -COMPILE: Create uncounted string SZ to next `"` |
|           | (--S)        | -RUN: push address S of string |
| ."        | (--)         | -COMPILE: execute `z"`, compile `ftype` |
|           | (--)         | -RUN: `ftype` on string |
| find      | (--XT A)     | XT: Execution Token, A: Dict Entry address (0 0 if not found) |
| loaded?   | (XT A--)     | Stops current load if A <> 0 (see `find`) |
| fopen     | (NM MD--FH)  | NM: File Name, MD: Mode, FH: File Handle (0 if error/not found) |
|           |              |     NOTE: NM and MD are uncounted, use `z"` |
| fclose    | (FH--)       | FH: File Handle to close |
| fdelete   | (NM--)       | NM: File Name to delere |
| fread     | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars read |
| fwrite    | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars written |
| fgets     | (A N FH--X)  | A: Buffer, N: Size, X: num chars read (0 if EOF/Error) |
| include X | (--)         | Load file named X (X: next word) |
| load      | (N--)        | N: Block number to load (file named "block-NNN.fth") |
| load-next | (N--)        | Close the current block and load block N next |
| system    | (S--)        | PC ONLY: S: String to send to `system()` |
| bye       | (--)         | PC ONLY: Exit C4 |

## C4 default words
Default words are defined in function `sys_load()` in file sys-load.c.<br/>
For details, or to add or change the default words, modify that function.
