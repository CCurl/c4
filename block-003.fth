( Block 3 - Strings )

find fill loaded?

p1vhere $100 + ;   p2p1 $100 + ;

fill  (dst cnt ch--) swap ?dupif>t swap t> for over over c! 1+    nextthen2drop ;
fill-c(dst cnt n--)  swap ?dupif>t swap t> for over over ! cell + nextthen2drop ;

cmove(src dst n--)>r >t >a
   r> ?dupiffor @a+ !t+ nextthen
   atdrop ;
cmove>(src dst n--)>r  r@ + >t  r@ + >a
   r> ?dupif1+ for @a- !t- nextthen
   atdrop ;

s-trunc(dst--dst)    0 over c! ;
s-end  (str-end)     dup s-len + ;
s-cat  (dst src--dst)>t >r r@ s-end t> s-cpy drop r> ;
s-catc (dst ch--dst) over s-end >t t@+ c! 0 t> c! ;
s-catn (dst n--dst)  <# #s #> s-cat ;
s-scat (src dst--dst)swap s-cat ;
s-scatc(ch dst--dst) swap s-catc ;
s-scatn(n dst--dst)  swap s-catn ;
s-rtrim(str--str)    >r r@ s-end 1- >t
  begint@ r@ <  @t 32 >  oriftdrop r> exitthen0 !t-again;






