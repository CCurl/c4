( block 004 - a simple memory manager )

find mb loaded?

3 load

( mem-buf-slot-info: )
(   0: free, >0: size/free, <0: -size/in-use )

( : rl forget 4 load ; )

: mb-sz #20000 ;  ( memory buffer size )
: mb-ssz  #500 ;  ( memory buffer slots size )
: mb-ruv   #25 ;  ( round-up value )

mb-sz        var mb     ( memory-buffer )
cell         var (mbh)  ( mb-here )
mb-ssz cells var mb-sa  ( slot-address )
mb-ssz cells var mb-si  ( slot-info )

: mb-here (mbh) @ ;  : >mb-here (mbh) ! ;
: mb-slot-a ( n--a ) cells mb-sa + ;
: mb-slot-i ( n--a ) cells mb-si + ;

: mb-init ( -- )
    mb >mb-here
    mb mb-sz 0 fill
    mb-sa 0 mb-ssz fill-c
    mb-si 0 mb-ssz fill-c ;

: mb-end ( --a )     mb mb-sz + ;
: mb-ru  ( sz--sz2 ) mb-ruv /mod swap if 1+ then mb-ruv * ;
: mb-new ( sz--h )   mb-here dup >t + >mb-here
    mb-here mb-end > if t> >mb-here 0 >t then
    t> ;

: mb-find ( a--ndx [0 means not found] )
    +regs a! 1 >d begin
      d> mb-slot-a @ a@ =
      if d> -exit then
      d>+ mb-ssz <
    while -1 -regs ;

: mb-size ( a--sz [0 means not found] )
    mb-find dup if mb-slot-i @ abs then ;

: mb-free? ( n sz--f )
    +regs >s  mb-slot-i @ >x
    x> 0= if 1 then          ( not allocated )
    x> 0< if 0 then          ( allocated, in-use )
    x> 0> if s> x> <= then   ( allocated-free )
    -regs ;

: alloc ( sz--h [0 means out-of-space] )
    +regs mb-ru >s  1 >x
    begin x> s> mb-free?
      if x> mb-slot-a @ a!
        a@ 0= if s> mb-new dup a! x> mb-slot-a ! then
        x> mb-slot-i dup >t @ s> max negate t> !
        a@ -exit
      then
      x+ x> mb-ssz <
    while
    -regs 0 ." -out of space!-" ;

: free ( h-- )
    mb-find ?dup if
      dup >t mb-slot-i @ abs t> mb-slot-i !
    then ;

: realloc ( h1 sz--h2 [0 means out-of-space] )
    +regs >s a!  0 >y
    a@ mb-find dup >x
    if x> mb-slot-i @ abs >y  ( already big enough? )
       s> y> <= if a@ -exit then
       a@ free
    then
    s> alloc >x
    x> if a@ x> y> cmove then
    x> -regs ;
  
: mb-dmp ( --)
    a@ >t cr mb-ssz 1 a! for
      i mb-slot-i @ ?dup if 0 a! i . . i mb-slot-a @ . cr then
    next a@ if ." (empty)" then t> a! ;

mb-init

( some tests )
: t1  50 alloc a! a@ . mb-dmp ;
: t2  99 alloc >d d> . mb-dmp ;
: t3 151 alloc >s s> . mb-dmp ;
: t4 s>  free mb-dmp ;
: t5 d>  free mb-dmp ;
: t6 a@  free mb-dmp ;
: tst t1 t2 t3 t4 t5 t6 ;
