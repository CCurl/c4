: ks ( -- ) yellow ." %n(press a key ...)" white key drop cls show! ;
: t1 ( -- ) green cols 1+ for '-' emit next cr white ;
: t2 ( -- ) green '|' emit white ;
: footer ( -- ) ->foot blk ."  -Block %d- "
    bl dirty? if drop '*' then emit
    col 1+ row 1+ ."  (%d,%d) " .mode
    cmd-buf ."   (cmd: %S)" clr-eol ;
: show ( -- ) cur-off 1 1 ->rc t1 rows for i 0 >pos t2 ztype t2 cr next t1 shown ;
: ?show ( -- ) show? if show then footer ;
: ed-init ( -- ) 0 dup >row/col normal-mode! clean cur-block ;
: ed-loop ( -- ) begin
    ?show ->cur vkey cur-off ed-key quit?
  until  ->cmd cur-on ;
: ed ( -- ) ed-init rl cls ed-loop ;
: edit ( blk-- ) >blk ed ;

















