( Conway's Game Of Life )

rows(--n)100 ;  cols(--n)200 ;  rows cols * const grid-sz
 grid-sz  var grid   grid-sz var work
 10 var states  states z"   ?*      "
nb(addr--addr1)dup c@ 1+ over c! 1+ ;
neighbors(addr--)cols - 1- nb nb nb cols + 2 - nb 1+ nb cols + 2 - nb nb nb drop ;
alive?(c n--c1)swap t! states + c@ dup '?' =ifdrop t@then;
?alive(--)a@ grid + c@ '*' =ifa@ work + neighborsthen;
prep(a--a1 c)120 + >t t@ t@ c@ 0 t> c! ;
.row(r--)cols * grid + 2+  dup prep >t >t ztype t> t> c! ;
disp(--) 1 1 ->cr  32 for i .row next ;
grid->work(--) cols 1+a!  grid-sz cols 2* - for  ?alive  next ;
work->grid(--) work b! grid a! grid-sz for  a@ @b+ alive?  !a+  next ;
one-gen(--k)grid->work work->grid ?key ;
gens(gens--)for one-genifunloop exitthennext ;
init(--)grid >a memory >b  grid-sz for  bl @b+if drop '*' then !a+  next ;
life(gens--)cls init gens ;














