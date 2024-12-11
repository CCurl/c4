cols var cmd-buf cols var yank-buf
->file(fh--)(output-fp) ! ;  ->stdout0 ->file ;
write-row p1 i 0 >pos s-cpy s-rtrim ztype 10 emit ;
write-blockrows for write-row next ;
w(write the block)dirty?ifblock-fn fopen-wb ?dup
   if>t t@ ->file write-block t> fclose clean ->stdoutthen then;
bs8 emit ;
del-ch( -- )y@ x@ <ifx- 0 !x bs space bsthen;
app-ch( ch-- )!x+ 0 !x emit ;
clr-bufy@ x! 0 !x ;
t50 >a x>t y>t ;  t6t>y t>x adrop ;
accept( str-- )t5 y! clr-buf
 beginkey a!
   a@ 13 = ift6 exitthen
   a@ 27 =  a@ 3 =    or ifclr-buf t6 exitthen
   a@ 8 =   a@ 127 =  or ifdel-chthen
   a@ printable?ifa@ app-chthen
again;
q(quit)dirty?if." (use q! to quit without saving)" exitthenq! ;
wq(write and quit)w q! ;
ed-exec(A--)->cmd 3 state wc! outer ;
do-cmd    ->cmd ':' emit cur-on cmd-buf accept cmd-buf ed-exec ;
yank-line yank-buf row 0 >pos  s-cpy drop ;
put-line  insert-line  row 0 >pos yank-buf s-cpy drop dirty ;
next-blk  w blk 1- 0 max ed! ;   prev-blk w blk 1+ ed! ;
mv-tab-l  0 -8 mv ;              mv-tab-r 0 8 mv ;
mv-cr     1 -99 mv ;             mv-end!  max-row 0 >row/col ;
yank/del  yank-line delete-line ;
exec-line row 0 >pos ed-exec ;



