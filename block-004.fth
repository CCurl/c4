( Block 4 - case / case!/ switch  )

[find switch loaded?

:case ( N-- )v, find drop v, ;
:case!( N-- )v, here v, 1 state wc! ;
:end-cases0 v, 0 v, ;
:switch( N TBL-- )>t >abegin
      a@ t@ @ =ift@ cell+ @ >r atdrop exitthen
      t@ 2 cells + t!
      t@ @if0atdrop exitthen
  again;

[find switch loaded?( Example usage ... )
( NOTE: you can't use z" or ," when using 'case!' )

:case-a." case a" ;
:case-b." , case b " ;

[vhere const cases
 'a' case    case-a
 'b' case!  123 . case-a case-b 789 . ;
end-cases

'a' cases switch cr
'b' cases switch cr






