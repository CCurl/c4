( Block 50 - Blocks )
find blocks loaded?
3 load( strings )
block-sz2048 ;inline  num-blocks500 ;inline
block-sz num-blocks * var blocks   32 var fn
num-blocks var t0(dirty/clean flags)
blk-norm(n1--n2)0 max num-blocks 1- min ;
blk-dirty!(n--)blk-norm 1 swap t0 + c! ;
blk-clean!(n--)blk-norm 0 swap t0 + c! ;
blk-dirty?(n--)blk-norm t0 + c@ ;
blk-buf(n--buf)blk-norm block-sz * blocks + ;
blk-fn(n--fn)>r fn z" block-" s-cpy r>
    <# # # #s #> s-cat z" .fth" s-cat ;
blk-clear(buf--)block-sz 0 fill ;
blk-clear-allnum-blocks for i blk-buf blk-clear next ;
blk-read(n--)blk-norm dup blk-buf >t blk-fn fopen-rb
   ?dupif0t> blk-clear exitthenfn ." %n-%S-"
   >a t> dup .hex space block-sz a@ fread . a> fclose ;
blk-write(n-)blk-norm >a a@ blk-dirty?if0adrop exitthen
    a@ blk-buf >t a@ blk-fn fopen-wb ?dup p1 ." -%s-%n"
   if>r t@ block-sz r@ fwrite . r> fclosethenatdrop ;
blk-rm(n--)blk-fn fdelete ;
blk-cp(from to--)swap blk-read blk-write ;
blk-mv(from to--)>t dup t> blk-cp blk-rm ;
blk-ins(n to--)1+ >t >a
   begint@- t@ swap blk-mv t@ a@ >whiletdrop a> blk-rm ;
blk-init(--)t0 num-blocks 0 fill blk-clear-all ;
blk-write-allnum-blocks for i blk-write next ;
blk-read-allblk-init num-blocks for i blk-read next ;
blk-read-all


