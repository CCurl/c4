( benchmarks / tests )

find bm1 loaded?
: test-if 'n' swap if drop 'y' then emit '.' emit ;
: if-else if z" yes" else z" no" then ztype '.' emit ;
cr 0 test-if 1 test-if    see test-if cr
cr 0 if-else 1 if-else    see if-else cr
: .. dup . ;
[[ cr 5 for i . ." [[ " 4 for i . next ." ]] " next ]]
[[ cr 10 begin .. 1- dup      while drop ]]
[[ cr  0 begin .. 1+ dup 10 = until drop ]]
: elapsed timer swap - ."  (%d usec)" ;   : mil 1000 dup * * ;
: t0 1 drop ;                             : t1 1 drop t0 ;
: fib 1- dup 2 < if drop 1 exit then dup fib swap 1- fib + ;
: bm1 cr dup ." bm1: empty loop: (%d)"     timer swap for next elapsed ;
: bm2 cr dup ." bm2: decrement loop: (%d)" timer swap begin 1- -while drop elapsed ;
: bm3 cr dup ." bm3: call loop: (%d)"      timer swap for t0 next elapsed ;
: bm4 cr dup ." bm4: 2 call loop: (%d)"    timer swap for t1 next elapsed ;
: bm5 cr dup ." bm5: fib (%d) ... " timer swap fib . elapsed ;
: bmk 1000 mil bm1 ;
: go  250 mil dup dup bmk bm2 bm3 bm4  38 bm5 cr ;
go cr
cell var xxx
see xxx cr cr
12345 xxx !
xxx @ ." -xxx created, (%d)-%n"
xxx @ const yyy
yyy ." -yyy created, (%d)-" cr
see yyy cr
[[ xxx @ yyy = ." const/var: " if ." PASS" exit then  ." FAIL!" ]]


