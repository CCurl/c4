 Block 51 - More Blocks)

find blk-init loaded?

blk-init(--)t0 num-blocks 0 fill blk-clear-all ;

blk-read(n--)blk-norm >t t@ blk-fn fopen-rb
   ?dupif0t> blk-clear exitthenfn ." %n-%S-"
   >a t> blk-buf block-sz a@ fread . a> fclose ;
blk-read-allblk-init num-blocks for i blk-read next ;

blk-write(n--)blk-norm >a a@ blk-dirty?if0adrop exitthen
   a@ blk-buf >t a@ blk-fn fopen-wb ?dup p1 ." -%s-%n"
   if>r t@ block-sz r@ fwrite . r> fclosethenatdrop ;
blk-write-allnum-blocks for i blk-write next ;

blk-rm(n--)blk-fn fdelete ;
blk-cp(from to--)swap blk-read blk-write ;
blk-mv(from to--)>t dup t> blk-cp blk-rm ;
blk-ins(n to--)1+ >t >a
   begint@- t@ swap blk-mv t@ a@ >whiletdrop a> blk-rm ;


blk-read-all








