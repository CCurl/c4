vhere const ctrl-cases
  key-up   case mv-up          key-down  case mv-dn       127 case mv-lt
  key-left case mv-lt          key-right case mv-rt         8 case mv-lt
  key-end  case mv-end         key-cend  case mv-end!       9 case mv-tab-r
  key-home case mv-home        key-chome case mv-home!     17 case mv-tab-l
  key-pgup case next-blk       key-pgdn  case prev-blk     13 case mv-cr
  key-del  case insert-toggle  key-del   case delete-char  24 case delete-prev
  3   case normal-mode!
end-cases

vhere const ed-cases
  'k' case  mv-up          'h' case  mv-lt    'j' case mv-dn     'l' case  mv-rt
  32  case  mv-rt          '_' case  mv-home  'q' case mv-tab-r  'Q' case  mv-tab-l
  'R' case  replace-mode!  'r' case  replace-one                 '$' case  mv-end
  'i' case  insert-mode!   'b' case! 32 insert-char mv-lt ;      '#' case! cls show! ;
  ':' case  do-cmd         '!' case! ->cmd cmd-buf outer ;       'D' case  yank/del
  'x' case  delete-char    'X' case  delete-prev
  'J' case  join-lines     'Y' case  yank-line
  'p' case  put-line       'P' case! mv-dn put-line ;   'A' case! mv-end insert-mode! ;
  '-' case  next-blk       '+' case  prev-blk           'C' case! row col clear-eol ;
  'o' case! 1 open-line ;  'O' case! 0 open-line ;
end-cases

: ed-key ( ch-- ) >a a@ 32 126 btwi if0 a> ctrl-cases switch exit then
    insert-mode?  if a> insert-char   exit then
    replace-mode? if a> replace-char  exit then
    a> ed-cases switch ;





