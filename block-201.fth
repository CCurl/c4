( Mandelbrot set )

7 load( registers )

val k    (val) t0  k!t0 ! ;
val x    (val) t0  x!t0 ! ;
val y    (val) t0  y!t0 ! ;
val z    (val) t0  z!t0 ! ;

sqr/ dup * z / ; inline
calc r1 sqr/ s3  r2 sqr/ s4
   r3 r4 + k >if1 exitthen
   r1 r2 * 100 / y + s2
   r3 r4 - x + s1
   r5 1+ s5  r5 z > ;

l-loop 0 s1 0 s2 0 s5begincalcuntil;
m-loop l-loop r5 40 + dup '~' >ifdrop blthenemit ;
x-loop -490 x! 95 for m-loop x  8 + x! next cr ;
y-loop -340 y! 35 for x-loop y 20 + y! next ;
mbrot  cls ." The Mandelbrot set%n" 1000000 k!  200 z!  y-loop ;











