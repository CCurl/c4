( Blocks 20:26 - A simple block editor )

 3 load ( strings/fill/cmove/cmove> )
 5 load ( screen )
10 load ( vars )
15 load ( blocks )

32 const rows            100 const cols          rows cols * const block-sz
block-sz var block       block-sz var work
rows 1- const max-row    cols 1- const max-col

: blk      40 wc@ ;   : >blk          40 wc! ;
: row      43 wc@ ;   : >row          43 wc! ;  : row++ row 1+ >row ;
: col      44 wc@ ;   : >col          44 wc! ;  : col++ col 1+ >col ;
: ed-mode  45 wc@ ;   : >ed-mode      45 wc! ;
: show?    46 wc@ ;   : show!       1 46 wc! ;  : shown   0 46 wc! ;
: dirty?   47 wc@ ;   : dirty show! 1 47 wc! ;  : clean   0 47 wc! ;
: >row/col ( r c-- ) >col >row ;
: >pos ( r c--a ) swap cols * + block + ;
: rc>pos ( --a ) row col >pos ;
: nt-line  ( r--)  0 swap max-col >pos c! ;
: nt-lines rows for i nt-line next ;
: ->cur   col 2+ row 2+ ->cr cur-on ;
: ->foot  1 rows 3 + ->cr ;
: ->cmd   ->foot cr clr-eol ;
: norm-row ( x-- )  row + 0 max max-row    min >row ;
: norm-col ( x-- )  col + 0 max max-col 1- min >col ;
: mv      ( r c-- ) norm-col norm-row row nt-line ;
: mv-lt 0 -1 mv ;  : mv-rt 0 1 mv ;  : mv-up -1 0 mv ;  : mv-dn 1 0 mv ;

21 26 thru

