#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
    7660 (  6472) :  (LITERAL) 3 
    7668 (  4032) :  DUP 
    7672 (  3612) :  0> 
    7676 (  6852) :  0BRANCH 7704 
    7684 (  4032) :  DUP 
    7688 (  5812) :  . 
    7692 (  3072) :  1- 
    7696 (  6832) :  BRANCH 7668 
    7704 (  3992) :  DROP 
    7708 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
    7660 (  6472) :  (LITERAL) 3 
    7668 (  5152) :  (.") hello "
    7676 (  3072) :  1- 
    7680 (  4032) :  DUP 
    7684 (  3632) :  0<= 
    7688 (  6852) :  0BRANCH 7704 
    7696 (  3992) :  DROP 
    7700 (  6012) :  EXIT 
    7704 (  6832) :  BRANCH 7668 
    7712 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
    7660 (  6852) :  0BRANCH 7684 
    7668 (  6472) :  (LITERAL) 1 
    7676 (  6832) :  BRANCH 7692 
    7684 (  6472) :  (LITERAL) 2 
    7692 (  5812) :  . 
    7696 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
    7660 (  6472) :  (LITERAL) 123 
    7668 (  5812) :  . 
    7672 (  6512) :  (2LITERAL) 123 
    7684 (  5832) :  D. 
    7688 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
    7660 (  5212) :  (S") hello"
    7668 (  5572) :  TYPE 
    7672 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
    7660 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
    7660 (  5252) :  (C") hello"
    7668 (  5112) :  COUNT 
    7672 (  5572) :  TYPE 
    7676 (  6132) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
    7660 (  7472) :  (ABORT") hello"
    7668 (  6132) :  (SEMICOLON) 
;
END

end_test;
