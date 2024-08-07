#include "c4.h"

void sys_load() {
    outer(": \\ 0 >in @ w! ; immediate");
    outer(": ->code code + ;");
    outer(": ->vars vars + ;");
    outer(": ->dict dict + ;");
    outer(": here  (here)  @c ;");
    outer(": last  (last)  @c ;");
    outer(": base@ base    @c ;");
    outer(": >base base    !c ;");
    outer(": vhere (vhere) @ ;");
    outer(": lex   (lex)   @c ;");
    outer(": >lex  (lex)   !c ;");
    outer(": 0sp  0 (sp)   !c ;");
    outer(": 0rsp 0 (rsp)  !c ;");
    outer(": , here  dup 1+ (here) !c !c ;");
    outer(": -exit #39 , (exit) , ; immediate"); // NOTE: #39 is -REGS
    outer(": begin here ; immediate");
    outer(": again (jmp)   , , ; immediate");
    outer(": while (jmpnz) , , ; immediate");
    outer(": until (jmpz)  , , ; immediate");
    outer(": -while (njmpnz) , , ; immediate");
    outer(": -until (njmpz)  , , ; immediate");
    outer(": -if (njmpz) , here 0 , ; immediate");
    outer(": if (jmpz) , here 0 , ; immediate");
    outer(": else (jmp) , here swap 0 , here swap !c ; immediate");
    outer(": then here swap !c ; immediate");
    outer(": ( begin");
    outer("    >in @ c@");
    outer("    dup  0= if drop exit then");
    outer("    >in @ 1+ >in !");
    outer("    ')' = if exit then");
    outer("  again ; immediate");
    outer(": allot vhere + (vhere) ! ;");
    outer(": ,v  vhere ->vars ! cell allot ;");
    outer(": hex     $10 >base ;");
    outer(": binary  %10 >base ;");
    outer(": decimal #10 >base ;");
    outer(": ?dup -if dup then ;");
    outer(": nip swap drop ;        : tuck swap over ;");
    outer(": 2dup over over ;       : 2drop drop drop ;");
    outer(": rot >r swap r> swap ;  : -rot swap >r swap r> ;");
    outer(": tdrop t> drop ;");
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
    outer(": +! tuck @ + swap ! ;     : c+! tuck c@ + swap c! ;");
    outer(": c++ dup c@ 1+ swap c! ;  : c-- dup c@ 1- swap c! ;");
    outer(": reg-base (reg-base) @c ; : >reg-base (reg-base) !c ;");
    outer(": frame-sz (frame-sz) @c ; : >frame-sz (frame-sz) !c ;");
    outer(": a>  0 reg-r ;  : >a  0 reg-s ;");
    outer(": a>+ 0 reg-ri ;  : a+ 0 reg-i ;");
    outer(": a>- 0 reg-rd ;  : a- 0 reg-d ;");
    outer(": s>  1 reg-r ;  : >s  1 reg-s ;");
    outer(": s>+ 1 reg-ri ;  : s+ 1 reg-i ;");
    outer(": s>- 1 reg-rd ;  : s- 1 reg-d ;");
    outer(": d>  2 reg-r ;  : >d  2 reg-s ;");
    outer(": d>+ 2 reg-ri ;  : d+ 2 reg-i ;");
    outer(": d>- 2 reg-rd ;  : d- 2 reg-d ;");
    outer(": x>  3 reg-r ;  : >x  3 reg-s ;");
    outer(": x>+ 3 reg-ri ;  : x+ 3 reg-i ;");
    outer(": x>- 3 reg-rd ;  : x- 3 reg-d ;");
    outer(": y>  4 reg-r ;  : >y  4 reg-s ;");
    outer(": y>+ 4 reg-ri ;  : y+ 4 reg-i ;");
    outer(": y>- 4 reg-rd ;  : y- 4 reg-d ;");
    outer(": bl 32 ; : space bl emit ;");
    outer(": (.) to-string count type ;");
    outer(": . (.) space ;");
    outer(": cr 13 emit 10 emit ;");
    outer(": tab 9 emit ;");
    outer(": ?  @ . ;");
    outer(": ->xt     w@ ;");
    outer(": ->size   2+  c@ ;");
    outer(": ->flags  3 + c@ ;");
    outer(": ->lex    4 + w@ ;");
    outer(": ->len    6 + ;");
    outer(": ->name   7 + ;");
    outer(": lex-match? ( a--f )  ->lex lex =  lex 0=  or ;");
    outer(": words +regs 0 >a 0 >d  last ->dict >s  dict-sz 1- ->dict >x");
    outer("    begin");
    outer("      s> lex-match? if");
    outer("        s> ->len count type d+");
    outer("        s> ->len c@ 7 > if a+ then");
    outer("        a>+ 8 > if cr 0 >a else tab then");
    outer("      then");
    outer("      s> dup ->size + dup >s  x> <");
    outer("    while d> .\"  (%d words)\" -regs ;");
    outer(": does> (jmp) , r> , ;");
    outer(": create addword ; immediate");
    outer(": const  addword here cell 2/ - 2* ->code ! (exit) , ; immediate");
    outer(": var    addword allot (exit) , ; immediate");
    outer("cell var vv");
    outer(": marker here 20 !c last 21 !c vhere vv ! ;");
    outer(": forget 20 @c (here) !c 21 @c (last) !c vv @ (vhere) ! 0 >lex ;");
#if defined(PC_FILE)
    outer(": fopen-rt ( fn--fh )  z\" rt\" fopen ;");
    outer(": fopen-rb ( fn--fh )  z\" rb\" fopen ;");
    outer(": fopen-wb ( fn--fh )  z\" wb\" fopen ;");
    outer(": thru ( f t-- ) begin dup load 1- over over > until drop drop ;");
#elif defined(TEENSY_FILE)
    outer(": fopen-r ( fn--fh )  z\" r\" fopen ;");
    outer(": fopen-w ( fn--fh )  z\" w\" fopen ;");
    outer(": thru ( f t-- ) begin dup load 1- over over > until drop drop ;");
#elif defined(PICO_FILE)
    outer(": fopen-r ( fn--fh )  z\" r\" fopen ;");
    outer(": fopen-w ( fn--fh )  z\" w\" fopen ;");
    outer(": thru ( f t-- ) begin dup load 1- over over > until drop drop ;");
#endif
    outer("marker");
}
