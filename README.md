# c4: a portable Forth system inspired by MachineForth and Tachyon

In C4, a program is a sequence of WORD-CODEs. <br/>
A `WORD-CODE` is a 16-bit unsigned number (0..65535). <br/>
Primitives are assigned numbers sequentially from 0 to `BYE`. <br/>
If a WORD-CODE is less than or equal to `BYE`, it is a primitive. <br/>
If the top 3 bits are set ($E000), it is a (13-bit unsigned) literal (0-8191). <br/>
If it is greater than `BYE`, it is the code address of a word to execute. <br/>

## CELLs in C4
A `CELL` is either 32-bits or 64-bits, depending on the target system.
- Linux 32-bit (-m32): a CELL is 32-bits.
- Linux 64-bit (-m64): a CELL is 64-bits.
- Windows 32-bit (x86): a CELL is 32-bits.
- Windows 64-bit (x64): a CELL is 64-bits.
- Development boards: a CELL is 32-bits.

## C4 memory areas

C4 provides three memory areas:
- The code area can store up to 65536 16-bit WORD-CODEs, 16-bit index. (see `code-sz`).
- NOTE: In the CODE area, slots 25-75 (`25 @c` .. `75 @c`) are unused by C4.
- NOTE: These are free for the application to use as desired.
- The variables area can store up to CELL bytes, CELL index (see `vars-sz`).
- The dictionary area can store up to 65536 bytes, 16-bit index (see `dict-sz`).
- Use `->code`, `->vars`, or `->dict` to turn an offset into an address.

16-bit system variables are located in the code area.<br/>
They are set/retrieved using `!c` and `@c` (e.g. `: hex #16 !c ;`).<br/>

| WORD       | STACK   | DESCRIPTION |
|:--         |:--      |:--          |
| (sp)       | (--N)   | Offset of the parameter stack pointer |
| (rsp)      | (--N)   | Offset of the return stack pointer |
| (here)     | (--N)   | Offset of the HERE variable |
| (last)     | (--N)   | Offset of the LAST variable |
| base       | (--N)   | Offset of the BASE variable |
| state      | (--N)   | Offset of the STATE variable |
| (lex)      | (--N)   | Offset of the LEX (the current lexicon) |
| (lsp)      | (--N)   | Offset of the loop stack pointer |
| (tsp)      | (--N)   | Offset of the third stack pointer |
| (reg-base) | (--N)   | Offset of the register base |
| (frame-sz) | (--N)   | Offset of the frame-sz value |

## C4 Strings

Strings in C4 are both counted and NULL terminated.<br/>
C4 also supports NULL-terminated strings with no count byte (ztype).<br/>

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

## Registers

C4 includes an array of "registers" (pre-defined cells). <br/>
The number of registers is configurable (see `reg-sz`). Default is 255 on PCs.<br/>
There is a `reg-base` that can be used to provide a "stack frame" if desired.<br/>
You can create words to reference them `frame-sz` at a time in a pseudo "stack frame".<br/>
For example: `123 42 reg-s`. <br/>
**NOTE** - you can leave `reg-base` at 0, and reference them individually.<br/>
Use `frame-sz` to control stack frame size. Default is 5.<br/>
The default bootstrap file creates 5 "registers" for stack frame use (a, s, d, x, y).<br/>
C4 provides 8 words to manage these registers. They are:<br/>

| WORD     | STACK   | DESCRIPTION |
|:--       |:--      |:-- |
| `+regs`  | (--)    | Create new frame; add `frame-sz` to `reg-base`. |
| `-regs`  | (--)    | Destroy frame; subtract `frame-sz` from `reg-base`. |
| `reg-r`  | (R--N)  | Push register (R + `reg-base`). |
| `reg-s`  | (N R--) | Set register (R + `reg-base`). |
| `reg-i`  | (R--)   | Increment register (R + `reg-base`). |
| `reg-d`  | (R--)   | Decrement register (R + `reg-base`). |
| `reg-ri` | (R--N)  | Push register (R + `reg-base`), then increment it. |
| `reg-rd` | (R--N)  | Push register (R + `reg-base`), then decrement it. |

## The Third Stack
C4 includes a third stack, with same ops as the return stack. (`>t`, `t@`, `t>`). <br/>
The size of the third stack is configurable (see `tstk-sz`).<br/>
This third stack can be used for any purpose. Words are:<br/>

| WORD  | STACK  | DESCRIPTION |
|:--    |:--     |:-- |
| `>t`  | (N--)  | Move N to the third stack. |
| `t@`  | (--N)  | Copy TOS from the third stack. |
| `t>`  | (--N)  | Move N from the third stack. |

## C4 WORD-CODE primitives

Stack effect notation conventions:

| TERM     | DESCRIPTION |
|:--       |:-- |
| SZ/NM/MD | String, uncounted, NULL terminated |
| SC/D/S   | String, counted, NULL terminated |
| A        | Address |
| C        | Number, 8-bits |
| W        | Number, 16-bits |
| N/X/Y    | Number, CELL sized |
| F        | Flag: 0 mean0 false, <>0 means true |
| R        | Register number |
| FH       | File handle: 0 means no file |
| I        | For loop index counter |

The primitives:

| WORD      | STACK        | DESCRIPTION |
|:--        |:--           |:-- |
| (lit1)    | (--W)        | W: WORD-CODE for LIT1 primitive |
| (lit2)    | (--W)        | W: WORD-CODE for LIT2 primitive |
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
| w@        | (A--W)       | W: the WORD at absolute address A |
| !         | (N A--)      | Store CELL N to absolute address A |
| c!        | (C A--)      | Store CHAR C to absolute address A |
| w!        | (W A--)      | Store WORD W to absolute address A |
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
| unloop    | (--)         | Unwind the loop stack. NOTE: this does NOT exit the loop. |
| +regs     | (--)         | Increment `reg-base` by `frame-sz` |
| -regs     | (--)         | Decrement `reg-base` by `frame-sz` |
| reg-r     | (R--N)       | Push register (`reg-base`+R) |
| reg-s     | (N R--)      | Set register (`reg-base`+R) to N |
| reg-i     | (R--)        | Increment register (`reg-base`+R) |
| reg-d     | (R--)        | Decrement register (`reg-base`+R) |
| reg-ri    | (R--N)       | Push register (`reg-base`+R), then increment it |
| reg-rd    | (R--N)       | Push register (`reg-base`+R), then decrement it |
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
|           | (--A)        | -RUN: A: current VHERE address |
| timer     | (--N)        | N: Current time |
| see       | (--)         | Output the definition of a word |
| count     | (SC--A N)    | A,N: address and count of chars in string S |
| type      | (A N--)      | Print string at A (counted, unformatted) |
| ztype     | (SZ--)       | Print string at A (uncounted, unformatted) |
| ftype     | (SZ--)       | Print string at A (uncounted, formatted) |
| s-cpy     | (D S--D)     | Copy string S to D, counted |
| s-eq      | (D S--F)     | F: 1 if string S is equal to D (case sensitive) |
| s-eqi     | (D S--F)     | F: 1 if string S is equal to D (NOT case sensitive) |
| z"        | (--)         | -COMPILE: Create uncounted string SZ to next `"` |
|           | (--SZ)       | -RUN: push address SZ of string |
| s"        | (--)         | -COMPILE: Create counted string SC to next `"` |
|           | (--SC)       | -RUN: push address SC of string |
| ."        | (--)         | -COMPILE: execute `z"`, compile `ftype` |
|           | (--)         | -RUN: `ftype` on string |
| rand      | (--N)        | N: a pseudo-random number (uses XOR shift) |
| fopen     | (NM MD--FH)  | NM: File Name, MD: Mode, FH: File Handle (0 if error/not found) |
|           |              |     NOTE: NM and MD are uncounted, use `z"` |
| fclose    | (FH--)       | FH: File Handle |
| fread     | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars read |
| fwrite    | (A N FH--X)  | A: Buffer, N: Size, FH: File Handle, X: num chars written |
| fgets     | (A N FH--N)  | A: Buffer, N: Size, N: num chars read (0 if EOF/Error) |
| include X | (--)         | Load file named X (X: next word) |
| load      | (N--)        | N: Block number to load (file named "block-NNN.c4") |
| loaded?   | (W A--)      | Stops a load if DE <> 0 (see `find`) |
| to-string | (N--SC)      | Convert N to string SC in the current BASE |
| .s        | (--)         | Display the stack |
| @c        | (N--W)       | Fetch unsigned 16-bit W from CODE address N |
| !c        | (W N--)      | Store unsigned 16-bit W to CODE address N |
| find      | (--W A)      | W: Execution Token, A: Dict Entry address (0 0 if not found) |
| system    | (SC--)       | PC ONLY: SC: String to send to `system()` |
| bye       | (--)         | PC ONLY: Exit C4 |
