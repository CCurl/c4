#include "c4.h"

#ifndef _SYS_LOAD_
void sys_load() {
    fileLoad("block-000.fth");
}
#else
void sys_load() {
    outer(" \
( Comments are free/built-in ) ; \
: \\ 0 >in @ c! ; immediate \
: ->memory memory + ; \
: ->code dup + ->memory ; \
: here  (here)  wc@ ; \
: last  (last)  @ ; \
: base@ base  wc@ ; \
: base! base  wc! ; \
: block@ (block) wc@ ; \
: block! (block) wc! ; \
: vhere (vhere) @ ; \
: allot vhere + (vhere) ! ; \
: 0sp  0 (dsp)  wc! ; \
: 0rsp 0 (rsp)  wc! ; \
: , here  dup 1+ (here) wc! wc! ; \
: v, vhere dup cell + (vhere) ! ! ; \
: vc, vhere dup 1+ (vhere) ! c! ; \
: const  addword lit, (exit) , ; \
: var vhere const allot ; \
: val -1 const ; \
: (val) last ->memory w@ 1+ ->code const ; \
: create vhere addword vhere lit, ; \
: does> (jmp) , r> , ; \
: begin here ; immediate \
: again (jmp)   , , ; immediate \
: while (jmpnz) , , ; immediate \
: until (jmpz)  , , ; immediate \
: -while (njmpnz) , , ; immediate \
: -until (njmpz)  , , ; immediate \
: -if (njmpz) , here 0 , ; immediate \
: if  (jmpz)  , here 0 , ; immediate \
: if0 (jmpnz) , here 0 , ; immediate \
: else (jmp) , here swap 0 , here swap wc! ; immediate \
: then here swap wc! ; immediate \
: hex     $10 base! ; \
: binary  %10 base! ; \
: decimal #10 base! ; \
: ?dup -if dup then ; \
: nip swap drop ;        : tuck swap over ; \
: 2dup over over ;       : 2drop drop drop ; \
: rot >r swap r> swap ;  : -rot swap >r swap r> ; \
: 0< 0 < ;            : 0> 0 > ; \
: <= > 0= ;           : >= < 0= ;      : <> = 0= ; \
: 2+ 1+ 1+ ; inline \
: 2* dup + ; inline \
: 2/ 2 / ;   inline \
: cells cell * ; inline \
: cell+ cell + ; inline \
: min ( a b--c ) 2dup > if swap then drop ; \
: max ( a b--c ) 2dup < if swap then drop ; \
: btwi ( n l h--f ) >r over >  swap r> >  or 0= ; \
: negate com 1+ ; \
: abs  dup 0< if negate then ; \
: -abs dup 0> if negate then ; \
: mod /mod drop ; \
: +! tuck  @ +  swap  ! ; \
: execute ( a-- ) >r ; \
: atdrop adrop tdrop ; \
: a+  a@+ drop ;  inline  : a-  a@- drop ; inline \
: @a  a@  c@ ;    inline  : !a  a@  c! ;   inline \
: @a+ a@+ c@ ;    inline  : !a+ a@+ c! ;   inline \
: @a- a@- c@ ;    inline  : !a- a@- c! ;   inline \
: b+  b@+ drop ;  inline  : b-  b@- drop ; inline \
: @b  b@  c@ ;    inline  : !b  b@  c! ;   inline \
: @b+ b@+ c@ ;    inline  : !b+ b@+ c! ;   inline \
: @b- b@- c@ ;    inline  : !b- b@- c! ;   inline \
: t+  t@+ drop ;  inline  : t-  t@- drop ; inline \
: @t  t@  c@ ;    inline  : !t  t@  c! ;   inline \
: @t+ t@+ c@ ;    inline  : !t+ t@+ c! ;   inline \
: @t- t@- c@ ;    inline  : !t- t@- c! ;   inline \
100 var #buf \
: <#   ( n1--n2 )  #buf 99 + >t 0 t@ c! dup 0 < >a abs ; \
: #c   ( c-- )     t- t@ c! ; \
: #.   ( -- )      '.' #c ; \
: #n   ( n-- )     dup 9 > if 7 + then '0' + #c ; \
: #    ( n1--n2 )  base@ /mod swap #n ; \
: #s   ( n-- )     begin # -while ; \
: #>   ( --str )   drop a> if '-' #c then t> ; \
: (.) <# #s #> ztype ; \
: . (.) 32 emit ; \
: bl 32 ; : space 32 emit ; \
: cr 13 emit 10 emit ; \
: tab 9 emit ; \
: .version version <# # # #. # # #. #s #> ztype ; \
: ?  @ . ; \
: ed block@ edit ; : ed! block! ; inline \
: .s '(' emit space (dsp) wc@ 1- ?dup \
    if for i 1+ cells dstk + @ . next then ')' emit ; \
: [[ vhere >t here >t 1 state wc! ; \
: ]] (exit) , 0 state wc! t@ (here) wc! t> >r t> (vhere) ! ; immediate \
mem-sz 1- ->memory const dict-end \
: ->xt     w@ ; inline \
: ->flags  wc-sz + c@ ; \
: ->len    wc-sz + 1+ c@ ; \
: ->name   wc-sz + 2+ ; \
: words last ->memory >a 0 >t 0 >r \
    begin \
        a@ ->name ztype r@ 1+ r! \
        a@ ->len dup 7 > t@ + t! 14 > t@ + t! \
        t@+ 9 > if cr 0 t! else tab then \
        a@ de-sz + a! a@ dict-end < \
    while tdrop adrop r> .\"  (%d words)\" ; \
: words-n ( n-- )  0 >a last ->memory swap for \
            dup ->name ztype tab a@+ 9 > if cr 0 a! then de-sz + \
        next drop adrop ; \
cell var t0 cell var t1 \
: marker here 20 wc!  last t0 !  vhere t1 ! ; \
: forget 20 wc@ (here) wc! t0 @ (last) ! t1 @ (vhere) ! ; \
: fgl last dup de-sz + (last) ! ->memory ->xt (here) wc! ; \
: fopen-rt ( fn--fh|0 )  z\" rt\" fopen ; \
: fopen-rb ( fn--fh|0 )  z\" rb\" fopen ; \
: fopen-wb ( fn--fh|0 )  z\" wb\" fopen ; \
: ->file ( fn-- ) fopen-wb (output-fp) ! ; \
: ->stdout ( -- ) (output-fp) @ ?dup if fclose 0 (output-fp) ! then ; \
: thru ( f t-- ) begin dup load 1- over over > until drop drop ; \
marker \
\
");
}
#endif // _SYS_LOAD_
