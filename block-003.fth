( Block 3 - Strings )

find fill loaded?

p1vhere $100 + ;   p2p1 $100 + ;

fill  (dst cnt ch--)>t swap >a for t@ !a+ nextatdrop ;
move  (src dst n--) >r >t >a r> for a@ @ t@ ! a@ cell+ a! t@ cell+ t! nextatdrop ;
cmove (src dst n--) >r >t >a r> for @a+ !t+ nextatdrop ;
cmove>(src dst n--) >r  r@ + 1- >t  r@ + 1- >a r>  for @a- !t- nextatdrop ;

s-trunc(dst--dst)    0 over c! ;
s-end  (str-end)     dup s-len + ;
s-cat  (dst src--dst)over s-end swap s-cpy drop ;
s-catc (dst ch--dst) over s-end w! ;
s-catn (dst n--dst)  <# #s #> s-cat ;
s-scpy (src dst--dst)swap s-cpy ;
s-scat (src dst--dst)swap s-cat ;
s-scatc(ch dst--dst) swap s-catc ;
s-scatn(n dst--dst)  swap s-catn ;
s-rtrim(str--str)    >r r@ s-end 1- >t
  begint@ r@ <  @t 32 >  oriftdrop r> exitthen
   0 !t-again;









