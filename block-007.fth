( Locals: example ... )
( : mx+b [[ m x b--n]] +locs  s3 s2 s1  r1 r2 * r3 +  -locs ; )

45 cells var ls-stk
vhere const ls-basemax
5 cells allot
cell var ls-base
: t0   ( index--addr ) [ cell lit, ] * ls-base @ + ;
: ls!  ( num index-- ) t0 ! ;
: ls@  ( index--num )  t0 @ ;
: ls@+ ( index--num )  t0 dup >r @ dup 1+ r> ! ;
: ls@- ( index--num )  t0 dup >r @ dup 1- r> ! ;
: ls-reset ls-stk ls-base ! ;
: +locs ( -- ) ls-base @ [ 5 cells lit, ] + ls-basemax min ls-base ! ;
: -locs ( -- ) ls-base @ [ 5 cells lit, ] - ls-stk     max ls-base ! ;

: s1 0 ls! ;  : r1 0 ls@ ;  : r1+ 0 ls@+ ;  : r1- 0 ls@- ;
: s2 1 ls! ;  : r2 1 ls@ ;  : r2+ 1 ls@+ ;  : r2- 1 ls@- ;
: s3 2 ls! ;  : r3 2 ls@ ;  : r3+ 2 ls@+ ;  : r3- 2 ls@- ;
: s4 3 ls! ;  : r4 3 ls@ ;  : r4+ 3 ls@+ ;  : r4- 3 ls@- ;
: s5 4 ls! ;  : r5 4 ls@ ;  : r5+ 4 ls@+ ;  : r5- 4 ls@- ;

ls-reset









