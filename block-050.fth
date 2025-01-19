(Block 50 - Blocks)

find blocks loaded?

3 load(strings)

block-sz 2048 ;inline
num-blocks500 ;inline

block-sz num-blocks * var blocks
num-blocks var fl(clean/dirty flags)

blk-norm(n1--n2)0 maxnum-blocks 1- lit,min ;
blk-dirty!(n--)blk-norm 1 swap fl + c! ;
blk-clean!(n--)blk-norm 0 swap fl + c! ;
blk-dirty?(n--)blk-norm fl + c@ ;
blk-buf(n--buf)blk-norm block-sz * blocks + ;

32 var fn(file-name)
blk-fn(n--fn)>r fn z" block-" s-cpy r>
    <# # # #s #> s-cat z" .fth" s-cat ;

blk-clear(n--)dup blk-clean! blk-buf block-sz 0 fill ;
blk-clear-all(--)num-blocks for i blk-clear next ;

51 load-next






