( Block 6 - Utility words )

find dump loaded?

: .2 <# # #s #> ztype ;
: .hex base@ >t hex .2 t> base! ;
: .bin base@ >t binary (.)  t> base! ;
: t1 ( ch-- ) dup 32 < over 126 > or if drop '.' then emit ;
: t2 ( a-- ) $10 for dup c@ t1 1+ next drop ;
: dump ( a n-- ) swap >a 0 >t
   for 0 t@ = if a@ ." %n%x: " then
       @a+ .hex space t@+ $0f = if ."    " 0 t! a@ $10 - t2 then
   next atdrop ;



















