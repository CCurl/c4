vhere const ctrl-cases
  key-up   case  mv-up           key-down  case  mv-dn          127 case  mv-lt
  key-left case  mv-lt           key-right case  mv-rt          8   case  mv-lt
  key-end  case  mv-end          key-cend  case  mv-end!        9   case  mv-tab-r
  key-home case  mv-home         key-chome case  mv-home!       17  case  mv-tab-l
  key-pgup case  next-blk        key-pgdn  case  prev-blk       13  case  mv-cr
  key-ins  case  insert-toggle   key-del   case  delete-char    24  case  delete-prev
  5 case   exec-line            19        case  w             27  case  normal-mode!
  1 case!1 put-char ;  2 case!2 put-char ;  3 case!3 put-char ;  4 case!4 put-char ;
end-cases

vhere const ed-cases
  'k' case  mv-up          'h' case  mv-lt    'j' case mv-dn     'l' case  mv-rt
  32  case  mv-rt          '_' case  mv-home  'q' case mv-tab-r  'Q' case  mv-tab-l
  'R' case  replace-mode!  'r' case  replace-one                 '$' case  mv-end
  'i' case  insert-mode!   'b' case!32 insert-char mv-lt ;
  ':' case  do-cmd         '!' case!cmd-buf ed-exec ;    'D' case  yank/del
  'x' case  delete-char    'X' case  delete-prev
  'J' case  join-lines     'Y' case  yank-line          'E' case  exec-line
  'p' case  put-line       'P' case!mv-dn put-line ;  'A' case!mv-end insert-mode! ;
  '-' case  next-blk       '+' case  prev-blk           'C' case!row col clear-eol ;
  'o' case!1 open-line ; 'O' case!0 open-line ;     '#' case!cur-block cls show! ;
  '1' case!1 put-char ;  '2' case!2 put-char ;
  '3' case!3 put-char ;  '4' case!4 put-char ;
end-cases

:ed-key( ch-- )dup 32 126 btwiif0ctrl-cases switch exitthen
   insert-mode? ifinsert-char  exitthen
   replace-mode?ifreplace-char exitthen
   ed-cases switch ;


