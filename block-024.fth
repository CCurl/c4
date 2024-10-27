: ->file ( fh-- ) (output-fp) ! ;  : ->stdout 0 ->file ;
: write-row  p1 i 0 >pos s-cpy s-rtrim ztype 10 emit ;
: write-block rows for write-row next ;
: w  dirty? if block-fn fopen-wb ?dup
    if >t t@ ->file write-block t> fclose clean ->stdout then then ;
: bs 8 emit ;
: del-ch ( -- ) y@ x@ < if x- 0 !x bs space bs then ;
: app-ch ( ch-- ) !x+ 0 !x emit ;
: clr-buf y@ x! 0 !x ;
: t5 0 >a x>t y>t ;  : t6 t>y t>x adrop ;
: accept ( str-- ) t5 y! clr-buf
  begin  key a!
    a@ 13 =  if t6 exit then
    a@ 27 =  a@ 3 =    or  if clr-buf t6 exit then
    a@ 8 =   a@ 127 =  or  if del-ch then
    a@ printable? if a@ app-ch then
  again ;
: q dirty? if ." (use q! to quit without saving)" exit then q! ;
: wq w q! ;
cols var cmd-buf
cols var yank-buf
: do-cmd ->cmd ':' emit cur-on cmd-buf accept ->cmd cmd-buf outer ;
: yank-line  yank-buf row 0 >pos  s-cpy drop ;
: put-line   insert-line  row 0 >pos yank-buf s-cpy drop dirty ;
: next-blk   w  blk 1- 0 max ed! ;
: prev-blk   w  blk 1+       ed! ;






