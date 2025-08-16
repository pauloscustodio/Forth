#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
   24484 ( 23036) :  (LITERAL) 3 
   24492 ( 20576) :  DUP 
   24496 ( 20276) :  0> 
   24500 ( 23416) :  0BRANCH 24528 
   24508 ( 20576) :  DUP 
   24512 ( 22356) :  . 
   24516 ( 19616) :  1- 
   24520 ( 23396) :  BRANCH 24492 
   24528 ( 20536) :  DROP 
   24532 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
   24484 ( 23036) :  (LITERAL) 3 
   24492 ( 21696) :  (.") hello "
   24500 ( 19616) :  1- 
   24504 ( 20576) :  DUP 
   24508 ( 20296) :  0<= 
   24512 ( 23416) :  0BRANCH 24528 
   24520 ( 20536) :  DROP 
   24524 ( 22576) :  EXIT 
   24528 ( 23396) :  BRANCH 24492 
   24536 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
   24484 ( 23416) :  0BRANCH 24508 
   24492 ( 23036) :  (LITERAL) 1 
   24500 ( 23396) :  BRANCH 24516 
   24508 ( 23036) :  (LITERAL) 2 
   24516 ( 22356) :  . 
   24520 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
   24484 ( 23036) :  (LITERAL) 123 
   24492 ( 22356) :  . 
   24496 ( 23076) :  (2LITERAL) 123 
   24508 ( 22396) :  D. 
   24512 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
   24484 ( 21756) :  (S") hello"
   24492 ( 22116) :  TYPE 
   24496 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
   24484 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
   24484 ( 21796) :  (C") hello"
   24492 ( 21656) :  COUNT 
   24496 ( 22116) :  TYPE 
   24500 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
   24484 ( 24036) :  (ABORT") hello"
   24492 ( 22696) :  (SEMICOLON) 
;
END

end_test;
