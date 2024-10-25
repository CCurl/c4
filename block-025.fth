: ed-ctrl-key ( -- )
    a@ key-up    = if mv-up    exit then   a@ key-left  = if mv-lt  exit then
    a@ key-down  = if mv-dn    exit then   a@ key-right = if mv-rt  exit then
    a@ key-end   = if mv-eol   exit then   a@ key-home  = if 0 >col exit then
    a@ key-pgup  = if next-blk exit then   a@ key-pgdn  = if prev-blk    then
    a@ 13        = if 1 -99 mv exit then   a@ key-chome = if 0 0 >row/col  exit then
    a@  9        = if 0   8 mv exit then   a@ key-ins   = if insert-toggle exit then
    a@ 17        = if 0  -8 mv exit then   a@ key-del   = if delete-char   exit then
    a@  8        = if mv-lt    exit then   a@ 127       = if mv-lt exit then
    a@  3 =  if normal-mode!   exit then   a@ key-cend  = if max-row 0 >row/col exit then
    a@ 24 =  if mv-lt delete-char  exit then ;

: ed-key ( a@ )   a@ 32 126 btwi 0= if ed-ctrl-key  exit then
    insert-mode?  if a@ insert-char   exit then
    replace-mode? if a@ replace-char  exit then
    a@ 'k' = if mv-up exit then            a@ 'h' = if mv-lt  exit then
    a@ 'j' = if mv-dn exit then            a@ 'l' = if mv-rt  exit then
    a@  32 = if mv-rt exit then            a@ '_' = if 0 >col exit then
    a@ 'q' = if 0  8 mv exit then          a@ 'R' = if replace-mode! exit then
    a@ 'Q' = if 0 -8 mv exit then          a@ 'i' = if insert-mode!  exit then
    a@ ':' = if do-cmd  exit then          a@ '#' = if cls show!     exit then
    a@ 'r' = if replace-one exit then      a@ 'C' = if row col clear-eol exit then
    a@ 'x' = if delete-char exit then      a@ 'X' = if mv-lt delete-char exit then
    a@ 'J' = if join-lines  exit then      a@ 'Y' = if yank-line       exit then
    a@ 'p' = if put-line    exit then      a@ 'P' = if mv-dn put-line  exit then
    a@ '$' = if mv-eol exit then           a@ 'A' = if mv-eol insert-mode! exit then
    a@ '-' = if next-blk exit then         a@ '+' = if prev-blk exit then
    a@ 'D' = if yank-line delete-line exit then   a@ '!' = if ->cmd p1 outer exit then
    a@ 'b' = if 32 insert-char mv-lt  exit then
    a@ 'o' = if mv-dn insert-line replace-mode!  exit then
    a@ 'O' = if       insert-line replace-mode!  exit then ;

