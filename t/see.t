#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
    7668 (  6480) :  (LITERAL) 3 
    7676 (  4040) :  DUP 
    7680 (  3620) :  0> 
    7684 (  6860) :  0BRANCH 7712 
    7692 (  4040) :  DUP 
    7696 (  5820) :  . 
    7700 (  3080) :  1- 
    7704 (  6840) :  BRANCH 7676 
    7712 (  4000) :  DROP 
    7716 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
    7668 (  6480) :  (LITERAL) 3 
    7676 (  5160) :  (.") hello "
    7684 (  3080) :  1- 
    7688 (  4040) :  DUP 
    7692 (  3640) :  0<= 
    7696 (  6860) :  0BRANCH 7712 
    7704 (  4000) :  DROP 
    7708 (  6020) :  EXIT 
    7712 (  6840) :  BRANCH 7676 
    7720 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
    7668 (  6860) :  0BRANCH 7692 
    7676 (  6480) :  (LITERAL) 1 
    7684 (  6840) :  BRANCH 7700 
    7692 (  6480) :  (LITERAL) 2 
    7700 (  5820) :  . 
    7704 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
    7668 (  6480) :  (LITERAL) 123 
    7676 (  5820) :  . 
    7680 (  6520) :  (2LITERAL) 123 
    7692 (  5840) :  D. 
    7696 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
    7668 (  5220) :  (S") hello"
    7676 (  5580) :  TYPE 
    7680 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
    7668 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
    7668 (  5260) :  (C") hello"
    7676 (  5120) :  COUNT 
    7680 (  5580) :  TYPE 
    7684 (  6140) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
    7668 (  7480) :  (ABORT") hello"
    7676 (  6140) :  (SEMICOLON) 
;
END

end_test;
