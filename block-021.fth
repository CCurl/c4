block-fn( --fn )p2 z" block-" s-cpy blk <# # # #s #> s-cat z" .fth" s-cat ;
t1( ch-- )dup 10 =ifdrop row++ 0 >col rc>pos t! exitthendup 4 >if32 maxthenc!t+ ;
t2atdrop nt-lines ;
work->blockwork >a  block >t  0 dup >row/col
   begin  c@a+ ?dupif0t2 exitthent1 row rows < while t2 ;
clear-block( addr-- )block-sz 32 fill ;
rl( reload block )work clear-block  block clear-block
   block-fn fopen-rb ?dupif
       >t work block-sz t@ fread drop t> fclose
  thenwork->block clean show! 0 dup >row/col ;
ed!( blk-- )>blk rl ;
normal-mode 0 ;   insert-mode 1 ;    replace-mode 2 ;   quit-mode99 ;
normal-mode? normal-mode  ed-mode = ;  normal-mode! normal-mode  >ed-mode ;
insert-mode? insert-mode  ed-mode = ;  insert-mode! insert-mode  >ed-mode ;
replace-mode?replace-mode ed-mode = ;  replace-mode!replace-mode >ed-mode ;
quit?        quit-mode    ed-mode = ;  q!           quit-mode    >ed-mode ;
.modespace normal-mode?if." -normal-" exitthen
   red insert-mode?if." -insert-"then
   replace-mode?if." -replace-"then
   white ;
insert-toggle
   normal-mode?ifinsert-mode!  exitthen
   insert-mode?ifreplace-mode! exitthen
   normal-mode! ;
ks( -- )yellow ." %n(press a key ...)" white key drop cls show! ;







