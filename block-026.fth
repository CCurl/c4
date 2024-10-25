: t1 green cols 1+ for '-' emit next cr white ;
: t2 green '|' emit white ;
: footer ->foot blk ."  -Block %d- "
    bl dirty? if drop '*' then emit
    col 1+ row 1+ ."  (%d,%d) " .mode
    p1 ."   (cmd: %S)" clr-eol ;
: show cur-off 1 1 ->rc t1 rows for i 0 >pos t2 ztype t2 cr next t1 shown ;
: ?show show? if show then footer ;
: ed-init 0 >row 0 >col normal-mode! clean cur-block ;
: ed-loop begin ?show ->cur vkey cur-off a! ed-key quit? until  ->cmd cur-on ;






















