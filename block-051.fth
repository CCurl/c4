 Block 51 - More Blocks)

find blk-init loaded?

blk-read(n--)blk-norm >t t@ blk-fn fopen-rb
   ?dupif0t> blk-clear exitthenfn ." %n-r/%S-"
   >a t> blk-buf block-sz a@ fread . a> fclose ;
blk-read-allnum-blocks for i blk-read next ;

blk-write(n--)blk-norm >a a@ blk-dirty?if0adrop exitthen
   a@ blk-buf >t a@ blk-fn fopen-wb ?dup fn ." %n-w/%S-"
   if>r t@ block-sz r@ fwrite . r> fclosethenatdrop ;
blk-flush(--)num-blocks for i blk-write next ;

blk-rm(n--)dup blk-fn fdelete blk-clear ;
blk-cp-buf(buf n--)dup blk-dirty! blk-buf block-sz cmove ;
blk-cp(from to--)>t blk-buf t> blk-cp-buf ;
blk-mv(from to--)over swap blk-cp blk-rm ;
blk-ins(n to--)1+ >t >a
   begint@- t@ swap blk-mv t@ a@ >whiletdrop a> blk-rm ;

blk-load(n--)blk-buf outer ;
blk-init(--)blk-clear-all blk-read-all ;

blk-init







