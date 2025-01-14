50 load
rlforget 49 load ;
cell var cp
((1 33 wc! ."  ( " a@ 1+ c@ '(' =ifa+then;
))0 33 wc! ."  ) " ;
cm?33 wc@ ;
?cmcm?if))then;
cc(t)t@ 10 =if?cm crthen
   t@ 1 = if ?cm ."  " then
   t@ 2 = if ?cm ." : " then
   t@ 3 = if ?cm ."  " then
   t@ 4 = if (( then
  ;
wk(c--)>t t@ 31 >ift> emit exitthencc tdrop ;
lp(--)@a-ifwk a+ lp exitthendrop ;
ini(blk--)blk-buf a! ;
doit(blk)ini lp ;
go(--)num-blocks for i doit next ;














