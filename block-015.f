( Block 15 - Blocks )

find blk-mv loaded?
3 load ( strings )

: blk-fn ( blk--fn ) >t p1 z" block-" s-cpy t> <# # # #s #> s-cat z" .f" s-cat ;
: blk-read ( blkNum buf bufSize--numRead ) >t >a  a@ 0 t@ fill
    blk-fn fopen-rb ?dup if >r a@ t@ r@ fread  r> fclose else 0 then atdrop ;
: blk-write ( blkNum buf bufSize--numWritten ) >t >a
    blk-fn fopen-wb ?dup if >r a@ t@ r@ fwrite r> fclose else 0 then atdrop ;
: blk-rm ( blkNum-- ) blk-fn fdelete ;
: blk-cp ( from to--sz ) >t p2 25000 blk-read t> swap p2 swap blk-write ;
: blk-mv ( from to-- ) over swap blk-cp drop blk-rm ;
: blk-ins ( blkNum stop-- ) 1+ >t >a
   begin t@- t@ swap blk-mv t@ a@ > while tdrop a> blk-rm ;

















