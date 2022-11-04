# c4 - a minimal and extendable Forth-like VM / OS

c4 is intended to be a starting point for a programming environment that can grow to fit the user's needs.

The main goals for this project are as follows:

- To be easy to modify and add/extend the primitives.
- To be able to run on any system that has a C compiler.
- To have an implementation that is minimal and "intuitively obvious upon casual inspection".
- To be be frugal with its usage of memory.
- To be able to fit on systems with small amounts of memory.
- To be deployable to as many different kinds of development boards as possible, via the Arduino IDE.
- To have a VML (virtual machine language) that is as human-reabable as possible.

To these ends, I have wandered off the beaten path in the following ways:

- This is NOT an ANSI-standard Forth system.
- This is a byte-coded implementation.
- Many primitives (core words) are built into the compiler, and are not included in the dictionary.
- These primitves ARE NOT case sensitive (DUP = dup = Dup).
- User-defined words ARE case sensitive.
- The dictionary is separated from the CODE.
- There is no ELSE, only IF/THEN. This is consistent with ColorForth.
- A dictionary entry looks like this: (xt,flags,word-len,word,null terminator).
- The maximum length of a word is configurable. (#define NAME_LEN xx)
- The number of available dictionary entries is configurable. (#define DICT_SZ xxx)
- To save space, code addresses are 2 bytes, so code space is limited to 16 bits (64kb).
- All CODE addresses are offsets into the CODE space, not absolute addresses.
- HERE is also an offset into the CODE space, not an absolute address.
- The VARIABLE space is separated from the CODE space, and can be larger than 64kb.
- VHERE is a 32-bit offset to the first available byte in he VARIABLE space.
- There are 10 temporary words (T0..T9) that can be re-defined without any dictionary overhead.
- There are 10 temporary variables (r0..r9) that can be allocated/destroyed.

## Temporary words:
- A temporary word is named T0 .. T9. (e.g. - ": T4 ... ;" will define word T4, but T4 is not added to the dictionary)
- I think of temporary words as named transient words that don't incur any dictionary overhead.
- Their purpose is to support code reuse and factoring without the overhead of a dictionary entriy.
- You cannot create an IMMEDIATE temporary word.
- When defined, they refer to the current value of HERE.
- All previous references to T4 still refer to the previous definition. New references to T4 refer to the new definition.
- Here is a simple example:
```
: T1 dup $20 < .if drop '.' .then ;
: dumpC for i c@ T1 emit next ;
: .code cb dup here + 1- for i c@ T1 emit next ;
```

## Temporary variables (registers):
- Temporary variables are allocated in sets of 10.
- They are completely under the control of the programmer.
- They are not built into the call sequence, so they can be accessed across words.
- They are not set or retrieved using @ and !.
- There are 5 special operations for temp variables: read, set, increment, decrement, and increment-cell
- The words to manage temps are: +tmps, r0..r9, s0..s9, c0..c9, i0..i9, d0..d9 and -tmps
  - +tmps: allocate 10 new temps
  - rX: push the value of temp #X onto the stack (read)
  - sX: pop the new value for temp #X off the stack (set)
  - cX: add CELL-SZ to temp #X
  - iX: increment temp #X
  - dX: decrement temp #X
  - -tmps: destroy the most recently allocated temps
- Here are some simple examples:
```
: betw ( n min max--f ) +tmps s3 s2 s1  r1 r2 >=  r1 r3 <=  and -tmps ;
: .c ( n-- ) s9  r9 #32 $7e betw if r9 emit else r9 ." (%d)" then ;
: dumpX ( a n-- ) over + for I c@ .c next ;
```

## Building c4
```
- Windows:
  - I use Visual Studio 19, either the community or paid version.
  - Open the c4.sln file.
  - Use the 'x86' configuration.
  - It detects the _WIN32 #define and builds c4 appropriately.

- Linux:
  - I use clang on Mint. GCC works as well.
  - There is a makefile to build c4.

- Development Boards:
  - I use the Arduino IDE.
  - Edit the "shared.h" file. Set the __BOARD__  appropriately.
  - If the board is not defined yet, create a new section for it. 
    - CODE_SZ is the size of the code area (64K max).
    - VARS_SZ is the size of the variables area (any size).
    - STK_SZ is the size of the combined DATA and RETURN stacks.
    - LSTK_SZ is the size of the loop stack.
    - LOCALS_SZ is the size of the locals buffer.
    - DICT_SZ is the size (number of entries) of the dictionary.
    - FLT_SZ is the size of the float stack.
    - The _PIN__ define indicates that the board supports digitalWrite(), et al.
    - The __FILES__ define indicates that the board supports LittleFS.
```
## c4 Primitives
```
NOTES: (1) These primitives are built into c4.
       (2) They are NOT case-sensitive.
       (3) They cannot be overridden.

*** MATH ***
+        (a b--c)          Addition
-        (a b--c)          Subtraction
/        (a b--c)          Division
*        (a b--c)          Multiplication
LSHIFT   (a b--c)          c: a left-shifted b bits
RSHIFT   (a b--c)          c: a right-shifted b bits
ABS      (a--b)            Absolute value
MAX      (a b--c)          c: max(a,b)
MIN      (a b--c)          c: min(a,b)
/MOD     (a b--q r)        q: quotient(a,b), r: modulus(a,b)
MOD      (a b--c)          modulus(a,b)
NEGATE   (a--b)            b: -a

*** STACK ***
1+       (a--b)            Increment TOS
1-       (a--b)            Decrement TOS
2*       (a--b)            b: a*2
2/       (a--b)            b: a/2
DROP     (a b--a)          Drop TOS
DUP      (a--a a)          Duplicate TOS
2DROP    (a b--)           Drop NOS and TOS
2DUP     (a b--a b a b)    Duplicate NOS and TOS
NIP      (a b--b)          Drop NOS
OVER     (a b--a b a)      Copy NOS
SWAP     (a b--b a)        Swap TOS and NOS
TUCK     (a b--b a b)      Push TOS under NOS
>R       (n--)             Move n to return stack
R>       (--n)             Move n from return stack
R@       (--n)             Copy n from return stack
ROT      (a b c--b c a)    Rotate a to TOS
-ROT     (a b c--c a b)    Rotate c before a

*** BITWISE ***
AND      (a b--c)          Bitwise AND
OR       (a b--c)          Bitwise OR
XOR      (a b--c)          Bitwise XOR
INVERT   (a--b)            Bitwise COMPLEMENT

*** FLOAT ***
I>F      (n--)             Integer to Float
F>I      (--n)             Float to Integer
F+       (a b--c)          Float Add
F-       (a b--c)          Float Sub
F*       (a b--c)          Float Mult
F/       (a b--c)          Float Div
F<       (a b--c)          Float LT
F>       (a b--c)          Float GT
F.       (--)              Float PRINT
FDUP     (a--a a)          Float DUP
FOVER    (a b--a b a)      Float OVER
FSWAP    (a b--b a)        Float SWAP
FDROP    (a--)             Float DROP

*** INPUT/OUTPUT ***
(.)      (n--)             Output n in the current BASE (no SPACE).
.        (n--)             Output n in the current BASE (trailing SPACE).
."       (?--?)            Output a (possibly formatted) string. See (1).
QTYPE    (A--)             Quick string output, no formatting, A is NULL-terminated.
TYPE     (A N--)           Output string at A (standard Forth TYPE).
ZTYPE    (A--)             Output the (possibly formatted) string at A. See (1).
CR       (--)              Output a newline (#10,#13).
EMIT     (c--)             Output c as a character.
COUNT    (s--s n)          n: length of string at s (s must be NULL-terminated).
KEY      (--c)             c: Next keyboard char, wait if no char available.
KEY?     (--f)             f: FALSE if no char available, else TRUE.
SPACE    (--)              Output a single SPACE (32 EMIT).
SPACES   (n--)             Output n SPACEs.

(1) Notes on ." and ZTYPE:
- ." is NOT ansi-standard
- %b: output TOS as a binary number
- %c: output TOS as a character
- %d: output TOS as an integer (base 10)
- %e: output an ESCAPE (27)
- %f: output FTOS as a floating point number
- %g: output FTOS as a scientific number
- %i: output TOS as an integer (current base)
- %n: output a CR/LF (13,10)
- %q: output the quote (") character
- %s: output TOS as a string (null terminated, no count byte)
- %t: output a TAB (9)
- %x: output TOS as a hex number

example: : ascii '~' $20 DO I I I I ." %n%d: (%c) %x %b" LOOP ;

*** FILE ***
FOPEN-R  (NM--fh)          Open file NM in read-only mode (rb).
FOPEN-W  (NM--fh)          Open file NM in write-only mode (wb). Truncates the file if it exists.
FOPEN-RW (NM--fh)          Open file NM in read-write mode (rb+).
FCLOSE   (fh--)            fh: file handle to close.
FREAD    (a n fh--r)       Reads next n bytes from file fh to address a. r: number of bytes read.
FWRITE   (a n fh--r)       Writes n bytes from address a to file fh. r: number of bytes written.
FGETC    (fh--c f)         c: next char from file fh, f: 0 if EOF/error, else 1.
FPUTC    (c fh--)          c: char to write to file fh.
FGETS    (a n fh--f)       Read next line from file fh to address a, size n, f: 0 if EOF/error, else 1.
FSEEK    (o w fh--)        Set file fh position - o: offset, w: (0:SEEK_SET, 1: SEEK_CUR, 2:SEEK_END).
FTELL    (fh--n)           n: current file fh position.
FDELETE  (fn--)            fn: The name of the file to be deleted.
FLIST    (--)              Print the list of files in the current directory (or on the dev board).

*** LOGICAL ***
TRUE     (--f)             f: -1
FALSE    (--f)             f: 0
=        (a b--f)          Equality.
<        (a b--f)          Less-than.
>        (a b--f)          Greater-than.
<=       (a b--f)          Less-than or equal-to.
>=       (a b--f)          Greater-than or equal-to.
<>       (a b--f)          Not equal-to.
0=       (n--f)            Logical NOT.
NOT      (n--f)            Logical NOT.

*** MEMORY ***
@        (a--n)            n: CELL at a.
C@       (a--b)            b: BYTE at a.
W@       (a--w)            w: WORD at a.
!        (n a--)           Store CELL n at a.
C!       (b a--)           Store BYTE b at a.
W!       (w a--)           Store WORD w at a.
+!       (n a--)           Add n to CELL at a.

*** FLOW CONTROL ***
IF       (f--)             Standard IF (NOTE: in c4, there is no ELSE)
THEN     (--)              Standard THEN
.IF      (f--)             Simple IF (shorter, more human-readable)
.THEN    (--)              Simple THEN
DO       (T F--)           Begin DO/LOOP loop
LOOP     (--)              Increment I, jump to DO if I < T
+LOOP    (N--)             Add N to I, break out if I reaches or crosses T
I        (--n)             n: Current DO/LOOP index
J        (--n)             n: Index of next-most outer loop
UNLOOP   (--)              Drop top 3 entries from the loop stack (unwind loop).
                     NOTE: Use IF UNLOOP EXIT THEN to break out a loop (DO or BEGIN) prematurely.
BEGIN    (f--)             Start BEGIN/WHILE/UNTIL/AGAIN loop.
WHILE    (f--)             If f==0, jump to BEGIN, else DROP f and continue.
UNTIL    (f--)             If f<>0, jump to BEGIN, else DROP f and continue.
AGAIN    (--)              Jump to BEGIN. Use <condition> IF UNLOOP EXIT THEN to break out.
EXIT     (--)              Exit the word immediately (don't forget to UNLOOP first if in a loop)

*** STRINGS ***
"          (--a)           a: address of a null-terminated string.
S"         (--a n)         a: address of a string, n: length of string.
STR-CAT    ( src dst-- )   Concatenate src to dst.
STR-CATC   ( c dst-- )     Concatenate char c to dst.
STR-CPY    ( src dst-- )   Copy src to dst.
STR-END    ( s1--s2 )      s2: the end of string s1.
STR-EQ     ( s1 s2--f )    f: 1 if s1 and s2 are equivalent, else 0 (case-sensitive).
STR-EQI    ( s1 s2--f )    f: 1 if s1 and s2 are equivalent, else 0 (case-insensitive).
STR-LEN    ( str--n )      n: length of null-terminated string str.
STR-RTRIM  ( str--str )    Trim rightmost chars from str whose ASCII value <= 32.
STR-TRUNC  ( str--str )    Truncate str to 0 length.

NOTE: Strings in c4 are NULL-TERMINATED, not COUNTED

*** TEMPORARY VARIABLES ***
+TMPS    (--)              Allocate 10 temp variables, r0 .. r9.
rX       (--n)             n: read value of temp var X (X:[0..9]).
sX       (n--)             Set value of temp var X to n.
iX       (--n)             Increment temp var X.
dX       (--n)             Decrement temp var X.
-TMPS    (--)              Destroy current temp variables.

**** SYSTEM/OTHER ***
.S       (--)              Output the stack.
BL       (--c)             c: 32.
BYE      (--)              Exit c4 (PC).
CONSTANT (--)              Define a constant.
CELL     (--n)             n: The size of a CELL.
CELLS    (n--x)            x: The number of bytes in n CELLs.
EDIT     (n--)             Edit block n.
EXECUTE  (a--)             Execute CODE at address a.
LOAD fn  (--)              Load file FN. The rest of the line is ignored.
LOAD-SYS (--)              Loads the last saved "./system.c4" file, if it exists.
SAVE-SYS (--)              Saves the system to file "./system.c4".
' wd      (--f | xt i f)   Lookup wd. If not found, f=0. Else f=1, i: immediate, and xt: offset.
NOP      (--)              Do nothing.
RAND     (--n)             n: a RANDOM 31-bit number (0..$7FFFFFFF).
RESET    (--)              Re-initialize c4.
SYSTEM   (a--)             a: string to send to system() ... eg: " dir" system (PC).
TIMER    (--n)             n: Time in MS.
MS       (n--)             n: MS to sleep.
VARIABLE (--)              Define a variable.
WORDS    (--)              Output the dictionary and primitives.
```

## Built-in words
```
mem      (--a)   a: Start address for MEMORY area.
cb       (--a)   a: Start address for CODE area.
vb       (--a)   a: Start address for VARS area.
mem-sz   (--n)   n: Size of system to be persisted on FSAVE.
code-sz  (--n)   n: Size of CODE area.
vars-sz  (--n)   n: Size of VARS area.
(here)   (--a)   a: Address of HERE.
(last)   (--a)   a: Address of LAST.
(vars)   (--a)   a: Address of VHERE.
base     (--a)   a: Address of BASE.
CELL     (--n)   n: size in bytes of a CELL.
```

## Extending c4
In the C code, TOS is the "top-of-stack", and NOS is "next-on-stack". There is also push(x) and pop(), which manage the stack.

In the beginning of the c4.cpp file, there is a section where prims[] is defined. This enumerates all the primitives that c4 knows about. The first member of an entry is the Forth name, the second part is the VML code.

You will see there are sections that add additional primitives if a symbol is #defined, for example:
```
#ifdef __PIN__
    // Extension: PIN operations ... for dev boards
    , { "PIN-IN","uPI" }   // open input
    , { "PIN-OUT","uPO" }  // open output
    , { "PIN-UP","uPU" }   // open input-pullup
    , { "PINA@","uPRA" }   // Pin read: analog
    , { "PIN@","uPRD" }    // Pin read: digital
    , { "PINA!","uPWA" }   // Pin write: analog
    , { "PIN!","uPWD" }    // Pin write: digital
#endif
```
You will notice that the VML code for these operations all begin with 'u'. The byte 'u' is the trigger to the VM that the command is implemented in doUser(ir, pc). Here is the definition of that function for development boards (in c4.ino):
```
byte *doUser(CELL ir, byte *pc) {
    CELL pin;
    switch (ir) {
    case 'G': pc = doGamePad(ir, pc);           break;  // zG<x>
    case 'N': push(micros());                   break;  // zN (--n)
    case 'P': pin = pop(); ir = *(pc++);                // Pin operations
        switch (ir) {
        case 'I': pinMode(pin, INPUT);                           break;  // zPI (p--)
        case 'O': pinMode(pin, OUTPUT);                          break;  // zPO (p--)
        case 'U': pinMode(pin, INPUT_PULLUP);                    break;  // zPU (p--)
        case 'R': ir = *(pc++);
            if (ir == 'A') { push(analogRead(pin));  }                   // zPRA (p--n)
            if (ir == 'D') { push(digitalRead(pin)); }           break;  // zPRD (p--n)
        case 'W': ir = *(pc++);
            if (ir == 'A') { analogWrite(pin,  (int)pop()); }            // zPWA (n p--)
            if (ir == 'D') { digitalWrite(pin, (int)pop()); }    break;  // zPWD (n p--)
        default:
            isError = 1;
            printString("-notPin-");
        }                                       break;
    case 'W': delay(pop());                     break;  // zW (n--)
    default:
        isError = 1;
        printString("-notExt-");
    }
    return pc;
}
```
In this context, 'pc' points to the next byte in the command stream, so "ir = *(pc++);" sets ir to the next byte and moves pc to point to the next byte after that. A VML command "zN" will execute the 'N' case in doExt(), which in this case, executes "push(micros())", pushing the return value from the Arduino library function "micros()" onto the stack. 

So, to add a primitive, add the Forth word to the prims[] array (either behind a #define or not), make sure it starts with 'z', then add a handler for that new case in doUser() that does whatever you want.
