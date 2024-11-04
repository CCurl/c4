( Block 10 - Variables x/y/z )

find x! loaded?

cell var (x) inline
: x!  (x) ! ;         : x@  (x) @ ;
: x+  x@ 1+ x! ;      : x-  x@ 1- x! ;
: x@+ x@ dup 1+ x! ;  : x@- x@ dup 1- x! ;
: !x  x@ c! ;         : @x  x@ c@ ;
: !x+ x@+ c! ;        : !x- x@- c! ;
: x>t x@ >t ;         : t>x t> x! ;

cell var (y) inline
: y!  (y) ! ;         : y@  (y) @ ;
: y+  y@ 1+ y! ;      : y-  y@ 1- x! ;
: y@+ y@ dup 1+ y! ;  : y@- y@ dup 1- y! ;
: !y  y@ c! ;         : @y  y@ c@ ;
: !y+ y@+ c! ;        : !y- y@- c! ;
: y>t y@ >t ;         : t>y t> y! ;

cell var (z) inline
: z!  (z) ! ;         : z@  (z) @ ;
: z+  z@ 1+ z! ;      : z-  z@ 1- z! ;
: z@+ z@ dup 1+ z! ;  : z@- z@ dup 1- z! ;
: !z  z@ c! ;         : @z  z@ c@ ;
: !z+ z@+ c! ;        : !z- z@- c! ;
: z>t z@ >t ;         : t>z t> z! ;





