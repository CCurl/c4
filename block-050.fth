( Block 50 - Blocks )

find blk-mv loaded?
3 load( strings )

block-sz(--n)3072 ;inline
blk-fn(blk--fn)>t p1 z" block-" s-cpy t> <# # # #s #> s-cat z" .fth" s-cat ;
blk-clear(buf--)block-sz 0 fill ;
t0(buf fh--)?dupif>r block-sz r@ fread drop r> fclose exitthenblk-clear ;
blk-read(blkNum buf--)swap blk-fn fopen-rb t0 ;
t1(buf fh--)?dupif>r block-sz r@ fwrite drop r> fclose exitthendrop ;
blk-write(blkNum buf--)swap blk-fn fopen-wb t1 ;
blk-rm(blkNum--)blk-fn fdelete ;
blk-cp(from to--sz)swap p2 blk-read  p2 blk-write ;
blk-mv(from to--)over >r blk-cp r> blk-rm ;
blk-ins(blkNum stop--)1+ >t >a
   begint@- t@ swap blk-mv t@ a@ >whiletdrop a> blk-rm ;

51 load-next













