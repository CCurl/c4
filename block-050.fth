( Block 50 - Blocks )

find blocks loaded?

3 load(strings)

341 blocks of 3k is close to 1 meg
block-sz  3072 ;inline
num-blocks 341 ;inline

block-sz num-blocks * var blocks
num-blocks var fl(clean/dirty flags)

blk-norm(n--blk)0 maxnum-blocks 1- lit,min ;
blk-flg! (blk flg--)swap blk-norm fl + c! ;
blk-dirty!(blk--)1 blk-flg! ;
blk-clean!(blk--)0 blk-flg! ;
blk-dirty?(blk--flg)blk-norm fl + c@ ;
blk-buf(blk--buf)blk-norm block-sz * blocks + ;

32 var fn(file-name)
blk-fn(blk--fn)>r fn z" block-" s-cpy r>
    <# # # #s #> s-cat z" .fth" s-cat ;

blk-clear(blk--)dup blk-clean! blk-buf block-sz 0 fill ;
blk-clear-all(--)num-blocks for i blk-clear next ;

51 load-next




