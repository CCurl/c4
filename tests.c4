// Tests

12345 constant xxx
xxx .

: T1 IF 'Y' EMIT EXIT THEN 'N' EMIT ;
: T2 cr 1 2 3 + + . cr 42 19 do i . loop cr 19 42 do i . -1 +loop ;
: T3 cr 20 s1 begin r1 . d1 r1 while ;
: T4 cr +tmps 123 445 s2 s1 r1 . r2 . -tmps ;
: T5 cr +tmps 666 777 s2 s1 r1 . r2 . T4 r1 . r2 . -tmps ;
: T6 s6 DO I . i r6 = .IF ." -out6" UNLOOP EXIT .THEN LOOP ." error!" ;

cr 1 T1 0 T1 
T2 T3 T4 T5
80 52 72 T6
: prime? ( n--f ) 3 s2 s1 begin 
		r1 r2 /mod 0= if 0= unloop exit then 
		drop r2 dup * r1 > if unloop 1 exit then i2 i2 
	again ;
: primes ( n1--n2 ) +tmps 4 s5 11 DO I prime? IF i5 THEN 1 +LOOP r5 -tmps ;
: .primes ( n1-- )  1 . 2 . 3 DO I prime? IF I . THEN 1 +LOOP ;
: elapsed timer swap - ;
: bm timer SWAP primes . elapsed ." primes. (%d ms)" ;
: mil 1000 DUP * * ;
cr see prime? cr
71 prime? ."  71 prime? (%d)%n"
cr cr 1 mil 4 / bm cr
cr 100 .primes ." (primes in 100)%n"

: T1 ." %nx1-" 9 BEGIN DUP . 1+ DUP 34 < WHILE ." -out1" DROP ;
: T2 ." %nx2-" 8 BEGIN DUP . 1+ DUP 27 = UNTIL ." -out2" DROP ;
: T3 ." %nx3-" 8 s1 BEGIN r1 . i1 r1 30 > .IF ." -out3" UNLOOP EXIT .THEN AGAIN ." -error!" ;
T1 T2 T3

CR words
