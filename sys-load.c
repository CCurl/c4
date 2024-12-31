#include "c4.h"

#ifndef _SYS_LOAD_
void sys_load() {
    fileLoad("block-000.fth");
}
#else
void sys_load() {
    outer("( Comments are free/built-in )");
    outer(": \\ 0 >in @ c! ; immediate");
    outer(": ->memory memory + ;");
    outer(": here  (here)  wc@ ;");
    outer(": last  (last)  @ ;");
    outer(": base@ base    wc@ ;");
    outer(": base! base    wc! ;");
    outer(": vhere (vhere) @ ;");
    outer(": allot vhere + (vhere) ! ;");
    outer(": 0sp  0 (dsp)  wc! ;");
    outer(": 0rsp 0 (rsp)  wc! ;");
    outer(": , here  dup 1+ (here) wc! wc! ;");
    outer(": v, vhere dup cell + (vhere) ! ! ;");
    outer(": vc, vhere dup 1+ (vhere) ! c! ;");

    outer(": const  addword lit, (exit) , ;");
    outer(": var    vhere const allot ;");
    outer(": create vhere addword vhere lit, ;");
    outer(": does> (jmp) , r> , ;");

    outer(": begin here ; immediate");
    outer(": again (jmp)   , , ; immediate");
    outer(": while (jmpnz) , , ; immediate");
    outer(": until (jmpz)  , , ; immediate");
    outer(": -while (njmpnz) , , ; immediate");
    outer(": -until (njmpz)  , , ; immediate");
    outer(": -if (njmpz) , here 0 , ; immediate");
    outer(": if  (jmpz)  , here 0 , ; immediate");
    outer(": if0 (jmpnz) , here 0 , ; immediate");
    outer(": else (jmp) , here swap 0 , here swap wc! ; immediate");
    outer(": then here swap wc! ; immediate");

    outer(": hex     $10 base! ;");
    outer(": binary  %10 base! ;");
    outer(": decimal #10 base! ;");
    outer(": ?dup -if dup then ;");
    outer(": nip swap drop ;        : tuck swap over ;");
    outer(": 2dup over over ;       : 2drop drop drop ;");
    outer(": rot >r swap r> swap ;  : -rot swap >r swap r> ;");
    outer(": 0< 0 < ;            : 0> 0 > ;");
    outer(": <= > 0= ;           : >= < 0= ;      : <> = 0= ;");
    outer(": 2+ 1+ 1+ ;          : 2* dup + ;     : 2/ 2 / ;");
    outer(": cells cell * ;      : chars ;        : cell+ cell + ;");
    outer(": min ( a b--c ) 2dup > if swap then drop ;");
    outer(": max ( a b--c ) 2dup < if swap then drop ;");
    outer(": btwi ( n l h--f ) >r over >  swap r> >  or 0= ;");
    outer(": negate com 1+ ;");
    outer(": abs  dup 0< if negate then ;");
    outer(": -abs dup 0> if negate then ;");
    outer(": mod /mod drop ;");
    outer(": +! tuck  @ +  swap  ! ;");
    outer(": execute ( a-- ) >r ;");

    outer(": @a  a@  c@ ;    : !a  a@  c! ;");
    outer(": @a+ a@+ c@ ;    : !a+ a@+ c! ;");
    outer(": @a- a@- c@ ;    : !a- a@- c! ;");
    outer(": a+  a@+ drop ;  : a-  a@- drop ;");
    outer(": atdrop adrop tdrop ;");
    outer(": @t  t@  c@ ;    : !t  t@  c! ;");
    outer(": @t+ t@+ c@ ;    : !t+ t@+ c! ;");
    outer(": @t- t@- c@ ;    : !t- t@- c! ;");
    outer(": t+  t@+ drop ;  : t-  t@- drop ;");

    outer("100 var #buf");
    outer(": <#   ( n1--n2 )  #buf 99 + >t 0 t@ c! dup 0 < >a abs ;");
    outer(": #c   ( c-- )     t- t@ c! ;");
    outer(": #.   ( -- )      '.' #c ;");
    outer(": #n   ( n-- )     dup 9 > if 7 + then '0' + #c ;");
    outer(": #    ( n1--n2 )  base@ /mod swap #n ;");
    outer(": #s   ( n-- )     begin # -while ;");
    outer(": #>   ( --str )   drop a> if '-' #c then t> ;");
    outer(": (.) <# #s #> ztype ;");
    outer(": . (.) 32 emit ;");

    outer(": bl 32 ; : space 32 emit ;");
    outer(": cr 13 emit 10 emit ;");
    outer(": tab 9 emit ;");
    outer(": .version version <# # # #. # # #. #s #> ztype ;");
    outer(": ?  @ . ;");

    outer(": .s '(' emit space (dsp) wc@ 1- ?dup");
    outer("    if for i 1+ cells dstk + @ . next then ')' emit ;");

    outer(": [[ vhere >t here >t 1 state wc! ;");
    outer(": ]] (exit) , 0 state wc! t@ (here) wc! t> >r t> (vhere) ! ; immediate");

    outer("mem-sz 1- ->memory const dict-end");
    outer(": ->xt     w@ ; inline");
    outer(": ->flags  wc-sz + c@ ;");
    outer(": ->len    wc-sz + 1+ c@ ;");
    outer(": ->name   wc-sz + 2+ ;");

    outer(": words last ->memory >a 0 >t 0 >r");
    outer("    begin");
    outer("      a@ ->name ztype r@ 1+ r!");
    outer("      a@ ->len dup 7 > t@ + t! 14 > t@ + t!");
    outer("      t@+ 9 > if cr 0 t! else tab then");
    outer("      a@ de-sz + a! a@ dict-end <");
    outer("    while tdrop adrop r> .\"  (%d words)\" ;");
    outer(": words-n ( n-- )  0 >a last ->memory swap for");
    outer("          dup ->name ztype tab a@+ 9 > if cr 0 a! then de-sz +");
    outer("      next drop adrop ;");

    outer("cell var t0 cell var t1");
    outer(": marker here 20 wc!  last t0 !  vhere t1 ! ;");
    outer(": forget 20 wc@ (here) wc! t0 @ (last) ! t1 @ (vhere) ! ;");
    outer(": fgl last dup de-sz + (last) ! ->memory ->xt (here) wc! ;");
    outer(": fopen-rt ( fn--fh )  z\" rt\" fopen ;");
    outer(": fopen-rb ( fn--fh )  z\" rb\" fopen ;");
    outer(": fopen-wb ( fn--fh )  z\" wb\" fopen ;");
    outer(": thru ( f t-- ) begin dup load 1- over over > until drop drop ;");
    outer("marker");
}
#endif // _SYS_LOAD_
