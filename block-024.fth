: ->file ( fh-- ) (output-fp) ! ;  : ->stdout 0 ->file ;
: write-row  p1 i 0 >pos s-cpy s-rtrim ztype 10 emit ;
: write-block rows for write-row next ;
: save-block  dirty? if0 exit then
    block-fn fopen-wb ?dup
    if >t t@ ->file write-block t> fclose clean ->stdout then ;
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
: ?quit dirty? if ." (use q! to quit without saving)" exit then quit! ;
: do-cmd ->cmd ':' emit clr-eol cur-on
    p1 accept ->cmd clr-eol
    p1 z" q"  s-eq  if  ?quit exit  then
    p1 z" q!" s-eq  if  quit! exit  then
    p1 z" wq" s-eq  if  save-block quit!  exit   then
    p1 z" w"  s-eq  if  save-block clean  exit   then
    p1 c@ '!' = if p1 1+ outer exit then
    p1 z" rl" s-eq  if  ed-load then ;
: yank-line  p2 row 0 >pos  s-cpy drop ;
: put-line   insert-line  row 0 >pos p2 s-cpy drop dirty ;
: next-blk   save-block  blk 1- 0 max >blk  ed-load ;
: prev-blk   save-block  blk 1+       >blk  ed-load ;

