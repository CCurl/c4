( Editor main loop )

( ANSI colors ...        COMPILE  DEFINE   INTERPRET  COMMENT )
[vhere const ed-colors  40 vc,   203 vc,  226 vc,    250 vc,

:ed-col@( state--color )-if1- ed-colors + c@then;
:ed-col!( color state-- )1- ed-colors + c! ;
:t1( -- )green cols 1+ for '-' emit next cr white ;
:t2( -- )green '|' emit white ;
:footer( -- )->foot blk ."  -Block %d- "
   bl dirty?ifdrop '*'thenemit
   col 1+ row 1+ ."  (%d,%d) " .mode rc>pos c@ ."  (%d)"
   cmd-buf ."   (cmd: %S)" clr-eol ;
:ed-emit( ch-- )dup 5 <ifed-col@ fg 1then32 max emit ;
:ed-type( a-- )>a cols for @a+ ed-emit next adrop ;
:show( -- )cur-off 1 1 ->rc t1 rows for t2 i 0 >pos ed-type t2 cr next t1 shown ;
:?show( -- )show?ifshowthenfooter ;
:ed-init( -- )0 dup >row/col normal-mode! clean cur-block ;
:ed-loop( -- )begin?show ->cur vkey cur-off ed-key quit?until->cmd cur-on ;
:ed( -- )ed-init rl cls ed-loop ;
:edit( blk-- )>blk ed ;











