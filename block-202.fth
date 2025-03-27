( Conway's Game Of Life )

 ( An XOR shift random number generator )
val seed   (val) (seed)
new-seedtimer (seed) ! ; new-seed
x17%00010000000000000000 ; x13%0001000000000000 ; x5 %00100000 ;
rand(--n)seed dup x13 * xor dup x17 / xor dup x5 * xor dup (seed) ! ;
rand-mod(max--n)rand abs swap mod ;

rows(--n)75 ;  cols(--n)150 ;
grid-sz rows cols *     const grid-sz
grid    grid-sz         var   grid
work    grid-sz         var   work
start   grid cols + 1+  const start
end     work cols - 2 - const end
states  10              var   states     states z"   ?*      " s-cpy drop
nb(addr--addr+1)dup c@ 1+ over c! 1+ ;
neighbors(addr--)cols - 1- nb nb nb cols + 3 - nb 1+ nb cols + 3 - nb nb nb drop ;
?alive(--)@a '*' =ifa@ grid - work + neighborsthen;
live?(--c)@b states + c@ dup '?' =ifdrop @athen;
prep(a--a1 c)100 + >t t@ @t 0 !t tdrop ;
.row(r--)cols * grid + 10 + dup prep >t >t ztype t> t> swap c! ;
disp(--)1 1 ->cr  35 for  i 10 + .row cr  next ;
grid->work(--)start a!begin ?alive  a@+ end < while;
work->grid(--)work b! grid a!  grid-sz for  live? !a+  0 !b+ next ;
one-gen(--)grid->work work->grid disp ;
gens(gens--)for one-gen  ?keyifkey drop unloop exitthennext ;
init(--)grid a!  grid-sz for  100 rand-mod 70 > 10 * 32 + !a+  next ;32+10=42/'*'
life(gens--)cur-off dupif0drop 500thencls init gens cur-on ;
lifes(n--)for 1000 life.s key dropnext ;


