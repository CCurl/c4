(Block 50 - Blocks)

find blocks loaded?

3 load(strings)

block-sz 2048 ;inline
num-blocks500 ;inline

block-sz num-blocks * var blocks
32 var fn
num-blocks var t0(dirty/clean flags)

blk-norm(n1--n2)0 max num-blocks 1- min ;
blk-dirty!(n--)blk-norm 1 swap t0 + c! ;
blk-clean!(n--)blk-norm 0 swap t0 + c! ;
blk-dirty?(n--)blk-norm t0 + c@ ;
blk-buf(n--buf)blk-norm block-sz * blocks + ;

blk-fn(n--fn)>r fn z" block-" s-cpy r>
    <# # # #s #> s-cat z" .fth" s-cat ;

blk-clear(n--)blk-buf block-sz 0 fill ;
blk-clear-allnum-blocks for i blk-clear next ;

51 load-next






