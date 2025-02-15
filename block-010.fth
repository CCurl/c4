(Block 10 - Variables x/y/z)

find x! loaded?

val x@   (val) T0   x! T0 ! ;
x+ x@ 1+ x! ;      x- x@ 1- x! ;
x@+x@ dup 1+ x! ;  x@-x@ dup 1- x! ;
!x x@ c! ;         @x x@ c@ ;
!x+x+ c! ;         !x-x@- c! ;
x>tx@ >t ;         t>xt> x! ;

val y@   (val) T0   y! T0 ! ;
y+ y@ 1+ y! ;      y- y@ 1- x! ;
y@+y@ dup 1+ y! ;  y@-y@ dup 1- y! ;
!y y@ c! ;         @y y@ c@ ;
!y+y@+ c! ;        !y-y@- c! ;
y>ty@ >t ;         t>yt> y! ;

val z@   (val) T0   z! T0 ! ;
z+ z@ 1+ z! ;      z- z@ 1- z! ;
z@+z@ dup 1+ z! ;  z@-z@ dup 1- z! ;
!z z@ c! ;         @z z@ c@ ;
!z+z@+ c! ;        !z-z@- c! ;
z>tz@ >t ;         t>zt> z! ;








