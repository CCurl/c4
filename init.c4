reset

: hex $10 base ! ;
: decimal #10 base ! ;
: binary %10 base ! ;

: vhere (vhere) @ vb + ;
: here (here) @ cb + ;
: last (last) @ 1- 20 * db + ;

: num-words (last) @ ;
: used here cb - ;

: T1 ( a-- ) DUP 4 + SWAP W@ ." %n%i: %s" ;
: words-l db num-words 0 DO DUP T1 20 + LOOP DROP ;

( n a b -betw- f )
: betw +tmps s3 s2 s1  r2 r1 <=  r1 r3 <=  and -tmps ;
: min ( a b--x ) OVER OVER > .if SWAP .then DROP ;
: max ( a b--x ) OVER OVER < .if SWAP .then DROP ;

: T0 ( c-- ) dup BL $7E betw .if emit exit .then ." (%d)" ;
: code here cb  DO I C@ T0 LOOP ;
: vars vhere cb DO I C@ T0 LOOP ;
: fill ( c f t-- ) SWAP DO dup i c! LOOP drop ;
: fill-n ( c a n-- ) over + fill ;
: ON 1 ; : OFF 0 ;

// Screen stuff
: ->XY ." %e[%d;%dH" ;
: CLS ." %e[2J" 1 dup ->XY ;
: COLOR ( bg fg -- ) ." %e[%d;%dm" ;
: FG ( fg -- ) 40 swap COLOR ;
: CURSOR-ON  ." %e[?25h" ;
: CURSOR-OFF ." %e[?25l" ;
: CURSOR ( f-- ) IF CURSOR-ON EXIT THEN CURSOR-OFF ;
