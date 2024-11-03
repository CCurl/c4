: ks ( -- ) yellow ." %n(press a key ...)" white key drop cls show! ;
: t1 ( -- ) green cols 1+ for '-' emit next cr white ;
: t2 ( -- ) green '|' emit white ;
: footer ( -- ) ->foot blk ."  -Block %d- "
    bl dirty? if drop '*' then emit
    col 1+ row 1+ ."  (%d,%d) " .mode rc>pos c@ ."  (%d)"
    cmd-buf ."   (cmd: %S)" clr-eol ;
: ed-color ( ch-- ) dup 1 = if drop green exit then ( compile )
    dup 2 = if drop red exit then ( define )
    dup 3 = if drop blue exit then ( interp )
    4 = if grey then ; ( comment )
: ed-emitc ( ch-- ) dup 1 < over 4 > or if drop exit then ed-color space ;
: ed-emit ( ch-- ) dup 31 > if emit exit then ed-emitc ;
: ed-type ( a-- ) >a cols for @a+ ed-emit next adrop ;
: show ( -- ) cur-off 1 1 ->rc t1 rows for t2 i 0 >pos ed-type t2 cr next t1 shown ;
: ?show ( -- ) show? if show then footer ;
: ed-init ( -- ) 0 dup >row/col normal-mode! clean cur-block ;
: ed-loop ( -- ) begin
    ?show ->cur vkey cur-off ed-key quit?
  until  ->cmd cur-on ;
: ed ( -- ) ed-init rl cls ed-loop ;
: edit ( blk-- ) >blk ed ;










