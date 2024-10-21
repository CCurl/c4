: \ 0 >in @ w! ; immediate
: ->code code + ;
: ->dict dict + ;
: here  (here)  @c ;
: last  (last)  @c ;
: base@ base    @c ;
: base! base    !c ;
: vhere (vhere) @ ;
: allot vhere + (vhere) ! ;
: 0sp  0 (dsp)  !c ;
: 0rsp 0 (rsp)  !c ;
: , here  dup 1+ (here) !c !c ;
: create addword ; immediate
: const  addword here cell wc-sz / - wc-sz * ->code ! (exit) , ; immediate
: does> (jmp) , r> , ;
: var    addword allot (exit) , ; immediate
: begin here ; immediate
: again (jmp)   , , ; immediate
: while (jmpnz) , , ; immediate
: until (jmpz)  , , ; immediate
: -while (njmpnz) , , ; immediate
: -until (njmpz)  , , ; immediate
: if  (jmpz)  , here 0 , ; immediate
: if0 (jmpnz) , here 0 , ; immediate
: -if (njmpz) , here 0 , ; immediate
: else (jmp) , here swap 0 , here swap !c ; immediate
: then here swap !c ; immediate
: ( begin
    >in @ c@
    dup  0= if drop exit then
    >in @ 1+ >in !
    ')' = if exit then
  again ; immediate
: hex     $10 base! ;
: binary  %10 base! ;
: decimal #10 base! ;
: ?dup -if dup then ;
: nip swap drop ;        : tuck swap over ;
: 2dup over over ;       : 2drop drop drop ;
: rot >r swap r> swap ;  : -rot swap >r swap r> ;
: 0< 0 < ;            : 0> 0 > ;
: <= > 0= ;           : >= < 0= ;      : <> = 0= ;
: 2+ 1+ 1+ ;          : 2* dup + ;     : 2/ 2 / ;
: cells cell * ;      : chars ;        : cell+ cell + ;
: min ( a b--c ) 2dup > if swap then drop ;
: max ( a b--c ) 2dup < if swap then drop ;
: btwi ( n l h--f ) >r over >  swap r> >  or 0= ;
: negate com 1+ ;
: abs  dup 0< if negate then ;
: -abs dup 0> if negate then ;
: mod /mod drop ;
: +! tuck  @ +  swap  ! ;
: @a  a@  c@ ;    : !a  a@  c! ;
: @a+ a@+ c@ ;    : !a+ a@+ c! ;
: @a- a@- c@ ;    : !a- a@- c! ;
: a+  a@+ drop ;  : a-  a@- drop ;
: @t  t@  c@ ;    : !t  t@  c! ;
: @t+ t@+ c@ ;    : !t+ t@+ c! ;
: @t- t@- c@ ;    : !t- t@- c! ;
: t+  t@+ drop ;  : t-  t@- drop ;
: atdrop adrop tdrop ;
: <#   ( n1--n2 )  dict 64 + >t 0 t@ c! dup 0 < >a abs ;
: #c   ( c-- )     t- t@ c! ;
: #.   ( -- )      '.' #c ;
: #n   ( n-- )     dup 9 > if 7 + then '0' + #c ;
: #    ( n1--n2 )  base@ /mod swap #n ;
: #s   ( n-- )     begin # -while drop ;
: #>   ( --str )   a> if '-' #c then t> ;
: bl 32 ; inline : space bl emit ; inline
: (.) <# #s #> ztype ;
: . (.) space ;
: .version version <# # # #. # # #. #s #> ztype ;
: .s '(' emit space (dsp) @c 1- ?dup
    if for i 1+ cells dstk + @ . next then ')' emit ;
: cr 13 emit 10 emit ;
: tab 9 emit ;
: ?  @ . ;
: ->xt     l@ ;
: ->flags  wc-sz + c@ ;
: ->len    wc-sz + 1+ c@ ;
: ->name   wc-sz + 2+ ;
: dict-end dict dict-sz + 1- ;
: words last ->dict >a 0 >t 0 >r
    begin
      a@ ->name ztype r@ 1+ r!
      a@ ->len  7 > if t+ then
      a@ ->len 12 > if t+ then
      t@+ 8 > if cr 0 t! else tab then
      a@ de-sz + a! a@ dict-end <
    while tdrop adrop r> ."  (%d words)" ;
: words-n ( n-- )  0 >a last ->dict swap for
          dup ->name ztype tab a@+ 9 > if cr 0 a! then de-sz +
      next drop adrop ;
cell var vh
: marker here 20 !c last 21 !c vhere vh ! ;
: forget 20 @c (here) !c 21 @c (last) !c vh @ (vhere) ! ;
: fopen-rt ( fn--fh )  z" rt" fopen ;
: fopen-rb ( fn--fh )  z" rb" fopen ;
: fopen-wb ( fn--fh )  z" wb" fopen ;
: thru ( f t-- ) begin dup load 1- over over > until drop drop ;
marker
