#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
    7728 (  6500) :  (LITERAL) 3 
    7736 (  4040) :  DUP 
    7740 (  3620) :  0> 
    7744 (  6880) :  0BRANCH 7772 
    7752 (  4040) :  DUP 
    7756 (  5820) :  . 
    7760 (  3080) :  1- 
    7764 (  6860) :  BRANCH 7736 
    7772 (  4000) :  DROP 
    7776 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
    7728 (  6500) :  (LITERAL) 3 
    7736 (  5160) :  (.") hello "
    7744 (  3080) :  1- 
    7748 (  4040) :  DUP 
    7752 (  3640) :  0<= 
    7756 (  6880) :  0BRANCH 7772 
    7764 (  4000) :  DROP 
    7768 (  6040) :  EXIT 
    7772 (  6860) :  BRANCH 7736 
    7780 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
    7728 (  6880) :  0BRANCH 7752 
    7736 (  6500) :  (LITERAL) 1 
    7744 (  6860) :  BRANCH 7760 
    7752 (  6500) :  (LITERAL) 2 
    7760 (  5820) :  . 
    7764 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
    7728 (  6500) :  (LITERAL) 123 
    7736 (  5820) :  . 
    7740 (  6540) :  (2LITERAL) 123 
    7752 (  5860) :  D. 
    7756 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
    7728 (  5220) :  (S") hello"
    7736 (  5580) :  TYPE 
    7740 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
    7728 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
    7728 (  5260) :  (C") hello"
    7736 (  5120) :  COUNT 
    7740 (  5580) :  TYPE 
    7744 (  6160) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
    7728 (  7500) :  (ABORT") hello"
    7736 (  6160) :  (SEMICOLON) 
;
END

end_test;
