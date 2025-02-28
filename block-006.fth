(Block 6 - Some utility words)

find dump loaded?

.nw(num width--)>r <# r> 1- for # next #s #> ztype ;
.22 .nw ;  .33 .nw ;  .44 .nw ;  .88 .nw ;
.NWB(num width base--)base@ >t base! .nw t> base! ;
.hex(num--)#2 #16 .NWB ;    .hex4 (num--)#4 #16 .NWB ;
.bin(num--)#8  #2 .NWB ;    .bin16(num--)#16 #2 .NWB ;
spaces(n--)for space next ;
a-emit(ch--)dup 32 < over 126 > orifdrop '.'thenemit ;
a-dump(addr--)$10 for dup c@ a-emit 1+ next drop ;
dump(addr num--)swap >a 0 >t
  for t@+if0a@ ." %n%x: "then@a+ .hex space
      t@ $10 =if3 spaces 0 t! a@ $10 - a-dumpthen
  next atdrop ;
lshift(n1 count--n2)for 2* next ;
rshift(n1 count--n2)for 2/ next ;
align(a1--a2)4 over 3 and - 3 and + ;













