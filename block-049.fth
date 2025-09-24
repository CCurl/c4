( Block 49 )

50 load

cell var cp

rlforget 49 load ;

?spc@32 >ifspacethen;
((1 33 wc! a@ 1+ c@ '(' =ifa+then'(' emit a@ ?sp ;
))0 33 wc! ." ) ";
cm?33 wc@ ;
?cmcm?if))then;
.\'\' emit ;
cc(t)space t@ 10 =if?cm .\ cr exitthenspace
  t@ 2 =if?cm ." : "then
  t@ 1 =if?cmthen
  t@ 3 =if?cmthen
  t@ 4 =if((then
  ;
.qt? (c--)dup '\' =if.\then;
.bsl?(c--)dup '"' =if.\then;
.ch  (c-- ).qt? .bsl? emit ;
wk   (c--)>t t@ 31 >ift> .ch exitthencc tdrop ;
lp   (--)c@a-ifwk a+ lp exitthendrop ;
ini  (blk--)blk-buf a! ;
doit (blk--)ini lp ;
go(--)num-blocks for i doit next ;

cr cr 49 ini lp


