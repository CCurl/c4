#include "c4.h"

// void sys_load() {
//     fileLoad("bootstrap.c4");
// }

void sys_load() {
    outer(": \\ 0 >in @ w! ; immediate");
    outer(": ->code code + ;");
    outer(": ->dict dict + ;");
    outer(": here  (here)  @c ;");
    outer(": last  (last)  @c ;");
    outer(": base@ base    @c ;");
    outer(": base! base    !c ;");
    outer(": vhere (vhere) @ ;");
    outer(": allot vhere + (vhere) ! ;");
    outer(": 0sp  0 (sp)   !c ;");
    outer(": 0rsp 0 (rsp)  !c ;");
    outer(": , here  dup 1+ (here) !c !c ;");
    outer(": begin here ; immediate");
    outer(": again (jmp)   , , ; immediate");
    outer(": while (jmpnz) , , ; immediate");
    outer(": until (jmpz)  , , ; immediate");
    outer(": -while (njmpnz) , , ; immediate");
    outer(": -until (njmpz)  , , ; immediate");
    outer(": -if (njmpz) , here 0 , ; immediate");
    outer(": if  (jmpz)  , here 0 , ; immediate");
    outer(": if0 (jmpnz) , here 0 , ; immediate");
    outer(": else (jmp) , here swap 0 , here swap !c ; immediate");
    outer(": then here swap !c ; immediate");
    outer(": ( begin");
    outer("    >in @ c@");
    outer("    dup  0= if drop exit then");
    outer("    >in @ 1+ >in !");
    outer("    ')' = if exit then");
    outer("  again ; immediate");
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
    outer(": +! tuck  @ +  swap  ! ;  : c+! tuck c@ +  swap c! ;");
    outer(": ++  dup  @ 1+ swap  ! ;  : --  dup   @ 1- swap  ! ;");
    outer(": c++ dup c@ 1+ swap c! ;  : c-- dup  c@ 1- swap c! ;");

    outer(": @a  a@  c@ ;    : !a  a@  c! ;");
    outer(": @a+ a@+ c@ ;    : !a+ a@+ c! ;");
    outer(": @a- a@- c@ ;    : !a- a@- c! ;");
    outer(": a+  a@+ drop ;  : a-  a@- drop ;");

    outer(": adrop a@ drop ; : atdrop adrop tdrop ;");

    outer(": @t  t@  c@ ;    : !t  t@  c! ;");
    outer(": @t+ t@+ c@ ;    : !t+ t@+ c! ;");
    outer(": @t- t@- c@ ;    : !t- t@- c! ;");
    outer(": t+  t@+ drop ;  : t-  t@- drop ;");

    outer(": #neg ( n1--n2 )   0 >a dup 0 < if 1 a! negate then ;");
    outer(": <# ( n--n )   dict 64 + >t 0 t@ c! #neg ;");
    outer(": #c ( c-- )    t@ 1- dup t! c! ;");
    outer(": #n ( n-- )    dup 9 > if 7 + then '0' + #c ;");
    outer(": #  ( n1--n2 ) base@ /mod swap #n ;");
    outer(": #s ( n-- )    begin # -while drop a> if '-' #c then ;");
    outer(": #> ( --str )  t> ;");
    outer(": space 32 emit ;");

    outer(": bl 32 ; inline : space bl emit ; inline");
    outer(": (.) <# #s #> ztype ;");
    outer(": . (.) space ;");

    outer(": .s '(' emit space (sp) @c 1- ?dup");
    outer("    if for i 1+ cells stk + @ . next then ')' emit ;");

    outer(": cr 13 emit 10 emit ;");
    outer(": tab 9 emit ;");
    outer(": ?  @ . ;");
    outer(": ->xt     w@ ;");
    outer(": ->flags  2 + c@ ;");
    outer(": ->len    3 + c@ ;");
    outer(": ->name   4 + ;");
    outer(": dict-end dict dict-sz + 1- ;");
    outer(": words last ->dict >a 0 >t 0 >r");
    outer("    begin");
    outer("      a@ ->name ztype r@ 1+ r!");
    outer("      a@ ->len  7 > if t+ then");
    outer("      a@ ->len 12 > if t+ then");
    outer("      t@+ 8 > if cr 0 t! else tab then");
    outer("      a@ de-sz + a! a@ dict-end <");
    outer("    while tdrop adrop r> .\"  (%d words)\" ;");
    outer(": words-n ( n-- )  0 >a last ->dict swap for");
    outer("          dup ->name ztype tab a@+ 9 > if cr 0 a! then de-sz +");
    outer("      next drop adrop ;");
    outer(": does> (jmp) , r> , ;");
    outer(": create addword ; immediate");
    outer(": const  addword here cell 2/ - 2* ->code ! (exit) , ; immediate");
    outer(": var    addword allot (exit) , ; immediate");
    outer("cell var vh");
    outer(": marker here 20 !c last 21 !c vhere vh ! ;");
    outer(": forget 20 @c (here) !c 21 @c (last) !c vh @ (vhere) ! ;");
    outer(": fopen-rt ( fn--fh )  z\" rt\" fopen ;");
    outer(": fopen-rb ( fn--fh )  z\" rb\" fopen ;");
    outer(": fopen-wb ( fn--fh )  z\" wb\" fopen ;");
    outer(": thru ( f t-- ) begin dup load 1- over over > until drop drop ;");
    outer("marker");
}
