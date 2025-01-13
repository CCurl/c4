(Block 51 - Blocks)

(an entry looks like this: [blk:2] [seq:2] [flgs:2] [data:block-sz])

cache-sz(--n)08 ;inline
ent-sz(--n)6 block-sz + lit,;inline

vhere align (vhere) !
ent-sz cache-sz * var cache

(Cache entry operations)
ent       (ent#--ent)ent-sz * cache + ;
ent-dump  (ent--)dup 4 + w@ swap dup 2+ w@ swap w@ ." blk:%d, seq:%d, flg:%d%n" ;
ent-clr   (ent--ent)dup ent-sz for 0 over c! 1+ next drop ;
ent-blk   (ent--ent blk)dup w@ ;
ent-blk!  (ent blk--ent)over w! ;
ent-seq   (ent--ent seq)dup 2+ w@ ;
ent-seq!  (ent n--ent)over 2+ w! ;
ent-seq+  (ent--ent)ent-seq 1+ ent-seq! ;
ent-flg!  (ent n--ent)over 4 + w! ;
ent-dirty?(ent--ent f)dup 4 + w@ ;
ent-dirty!(ent--ent)1 ent-flg! ;
ent-clean!(ent--ent)0 ent-flg! ;
ent-data  (ent--data)6 + ;
ent-free? (ent--ent f)dup ent-blk 0= ;
ent-read  (ent--ent)dup ent-clean! ent-blk swap ent-data blk-read ;
ent-write (ent--ent)dup ent-clean! ent-blk swap ent-data blk-write ;
ent-write?(ent--ent)ent-dirty?ifent-writethen;

52 load-next


