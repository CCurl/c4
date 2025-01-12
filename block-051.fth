( Block 51 - Blocks )

cache-sz(--n)32 ;inline
ent-sz(--n)block-sz 4 + lit,;inline
vhere align (vhere) !
ent-sz cache-sz * var cache
(Cache entry operations)
ent (ent#--ent)ent-sz * cache + ;
ent-clr   (ent--)ent-sz for 0 over c! 1+ next drop ;
ent-blk   (ent--blk)w@ ;inline
ent-blk!  (blk ent--)w! ;inline
ent-dirty (ent--)1 swap 2+ c! ;
ent-dirty?(ent--)2+ c@ ;
ent-clean (ent--)0 swap 2+ c! ;
ent-data  (ent--data)4 + ;
ent-free? (ent--f)ent-blk 0= ;
ent-read  (ent--)>a a@ ent-clean a@ ent-blk a> ent-data blk-read  drop ;
ent-write (ent--)>a a@ ent-clean a@ ent-blk a> ent-data blk-write drop ;
ent-write?(ent--)dup ent-dirty?ifent-write exitthendrop ;
(Cache Operations)
ca-flush(--)cache-sz for i ent dup ent-write? ent-clr next ;
ca-find(blk#--addr)>a 0 ent >t cache-sz for
    t@ ent-blk a@ =ifunloop t> adrop exitthent@ ent-sz t!
  next 0 atdrop ;

52 load-next






