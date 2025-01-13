(Block 52 - Block cache)

(Cache Operations)
ca-dump(--)cache-sz for i . i ent ent-dump next ;
ca-flush(--)cache-sz for i ent dup ent-write? ent-clr next ;
ca-reuse(ent--ent)ent-write? ent-clr ;
ca-find-free(--ent)cache-sz for i ent ent-blkif0exitthennext 0 ;
t0(blk ent--blk f)ent dup ent-blk =if0 then0 ;

Looking for the block. If found, exit.
Find a free block. If found use it.
Find the entry with the lowest seqno. Use that entry.

find-blk(blk--ent)>a cache-sz for i ent ent-blk a@ =ifadrop unloop exitthennext ;
ca-find(blk--addr)cache-sz >at5dup i ent t0 t5
    t@ ent-blk a@ =ifunloop t> adrop exitthent@ ent-sz t!
  next 0 atdrop ;

blk-addr(blk--addr)dup ca-findifexitthen;

53 load-next











