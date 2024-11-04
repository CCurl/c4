( Block 6 - Utility words )

[ find dump loaded?

: .X ( num width-- ) >r <# r> 1- for # next #s #> ztype ;
: .2 2 .X ;   : .3 3 .X ;   : .4 4 .X ;
: .hex base@ >t hex .2 t> base! ;
: .bin base@ >t binary 8 .X t> base! ;
: spaces for space next ;
: a-emit ( ch-- ) dup 32 < over 126 > or if drop '.' then emit ;
: a-dump ( addr-- ) $10 for dup c@ a-emit 1+ next drop ;
: dump ( addr n-- ) swap >a 0 >t
   for 0 t@ = if a@ ." %n%x: " then
       @a+ .hex space t@+ $0f = if 3 spaces 0 t! a@ $10 - a-dump then
   next atdrop ;
: lshift ( n1 count--n2 ) for 2* next ;
: rshift ( n1 count--n2 ) for 2/ next ;















