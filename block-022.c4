: printable? ( c--c 1 | 0 ) dup 32 126 btwi if 1 else drop 0 then ;
: replace-char ( ch-- ) printable? if rc>pos c! dirty mv-rt then show! ;
: replace-one  ( -- )   cur-off red '?' emit key white replace-char ;
: insert-char  ( ch-- ) printable? if0 exit then
    >a rc>pos >r  row max-col >pos >t
    begin  t@ 1- c@  !t-  t@ r@ >  while
    a> r> c! tdrop mv-rt dirty ;
: delete-char ( -- ) rc>pos >t  row max-col >pos >r
    begin  t@ 1+ c@  !t+  t@ r@ < while
    32 r> 1- c! dirty tdrop ;
: eol-offset ( row--offset ) >t max-col >a
    begin t@ a@ >pos c@ 32 > if a@ 1+ atdrop exit then a@- while 0 atdrop  ;
: mv-eol ( -- ) row eol-offset >col ;
: clear-line ( r-- ) 0 >pos max-col for 32 over c! 1+ next 0 swap c! dirty ;
: clear-eol  ( r c-- ) max-col over - >t >pos t> for 32 over c! 1+ next drop dirty ;
: insert-line ( -- )  row max-row < if
        row 0 >pos >t  t@  t@ cols +  max-row 0 >pos t> - cmove>
    then  row clear-line  dirty ;
: delete-line ( -- )  row max-row < if
        row 0 >pos >t  t@ cols + >r  rows 0 >pos r@ - >a  r> t> a> cmove
    then  max-row clear-line  dirty ;
: insert-row ( r-- ) row swap  >row insert-line  >row ;
: delete-row ( r-- ) row swap  >row delete-line  >row ;
: join-lines ( -- ) row max-row >= if exit then
    p1 row 0 >pos s-cpy s-rtrim 32 s-catc
    row 1+ 0 >pos s-cat cols 1- + 0 swap c!
    row 0 >pos p1 s-cpy drop
    row 1+ delete-row ;




