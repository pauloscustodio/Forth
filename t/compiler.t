#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test QUIT";
note "Test REFILL";
note "Test INTERPRET";
forth_ok("1 .S QUIT 2 .S", "( 1 )");

note "Test BYE";
forth_ok("1 .S BYE 2 .S", "( 1 )");

note "Test EVALUATE";
forth_ok("SOURCE-ID . '!' WORD SOURCE-ID .! COUNT EVALUATE .S", 
		 "1 -1 ( )");

note "Test EXIT";
note "Test EXECUTE";
note "Test LITERAL";
note "Test [";
note "Test ]";
forth_ok(<<'END', "( 1 )");
		: x [
		1 LITERAL   		\ 1
		] EXIT [   			\ exit
		2 LITERAL   		\ 2
		] ;
		' x EXECUTE			\ and runs it
		.S
END

note "Test (";
forth_ok("1 ( 2 3)4 .S", "( 1 4 )");
forth_ok(<<'END', "( 1 4 )");
		1 ( 2 3
		4 ( 5 6)
		.S
END

forth_ok(": x 1 ( 2 3)4 ; x .S", "( 1 4 )");

note "Test \\";
forth_ok(<<'END', "( 1 4 )");
		1 \ 2 3
		4 \ 5 6
		.S
END

forth_ok(<<'END', "( 1 4 )");
		: x
		  1 \ 2 3
		  4 \ 5 6 
		;
		x .S
END

note "Test IF";
note "Test ELSE";
note "Test THEN";
forth_ok(": x IF 1        THEN ;  0 x .S", "( )");
forth_ok(": x IF 1        THEN ; -1 x .S", "( 1 )");
forth_ok(": x IF 1 ELSE 2 THEN ;  0 x .S", "( 2 )");
forth_ok(": x IF 1 ELSE 2 THEN ; -1 x .S", "( 1 )");

note "Test DO";
note "Test ?DO";
note "Test LOOP";
note "Test +LOOP";
note "Test I";
note "Test J";
note "Test LEAVE";
note "Test UNLOOP";

forth_ok(": x 5 0 DO I .     LOOP ; x", "0 1 2 3 4 ");
forth_ok("HEX : x -80000000 7FFFFFFF DO I . LOOP ; x", "7FFFFFFF ");
forth_ok(": x 0 5 DO I . -1 +LOOP ; x", "5 4 3 2 1 0 ");
forth_ok("HEX : x 7FFFFFFF -80000000 DO I . -1 +LOOP ; x", "-80000000 7FFFFFFF ");
forth_ok(": x 0 5 DO I . -2 +LOOP ; x", "5 3 1 ");

forth_ok(": x 0 0 ?DO I .     LOOP ; x", "");
forth_ok(": x 5 0 ?DO I .     LOOP ; x", "0 1 2 3 4 ");
forth_ok(": x 0 5 ?DO I . -1 +LOOP ; x", "5 4 3 2 1 0 ");
forth_ok(": x 0 5 ?DO I . -2 +LOOP ; x", "5 3 1 ");

forth_ok(": x 2 0 DO 4 2 DO J . I . LOOP LOOP ; x", "0 2 0 3 1 2 1 3 ");

forth_ok(": x 10 0 DO I . I 1 = IF LEAVE THEN I 5 = IF LEAVE THEN LOOP .S ; x",
		 "0 1 ( )");
forth_ok(": x 10 0 DO I . I 5 = IF LEAVE THEN I 1 = IF LEAVE THEN LOOP .S ; x",
		 "0 1 ( )");
		 

forth_ok(": x 10 0 DO I . I 5 = IF UNLOOP EXIT THEN LOOP ; x", "0 1 2 3 4 5 ");

# John Hayes failing test
forth_ok(<<'END', "( 1 0 -1 -2 -3 -4 6 )");
VARIABLE ITERS
VARIABLE INCRMNT

: QD6 ( limit start increment -- )
   INCRMNT !
   0 ITERS !
   ?DO
      1 ITERS +!
      I
      ITERS @  6 = IF LEAVE THEN
      INCRMNT @
   +LOOP ITERS @
;

4  1 -1 QD6 

.S
END

note "Test BEGIN";
note "Test AGAIN";
forth_ok(": x 0 BEGIN DUP . DUP 5 = IF DROP EXIT THEN 1+ AGAIN ; x", 
		 "0 1 2 3 4 5 ");

note "Test BEGIN";
note "Test UNTIL";
forth_ok(": x 0 BEGIN DUP . 1+ DUP 6 = UNTIL DROP ; x", 
		 "0 1 2 3 4 5 ");

note "Test BEGIN";
note "Test WHILE";
note "Test REPEAT";
forth_ok(": x 0 BEGIN DUP 6 <> WHILE DUP . 1+ REPEAT DROP ; x", 
		 "0 1 2 3 4 5 ");

note "Test CASE";
note "Test OF";
note "Test ENDOF";
note "Test ENDCASE";
my $func = ": x CASE 33 EMIT ENDCASE ;";
forth_ok("$func 1 x .S", "!( )");
forth_ok("$func 2 x .S", "!( )");

$func = <<END;
		: x CASE 1 OF 1 . ENDOF 
				 2 OF 2 . ENDOF 
				 3 OF 3 . ENDOF 
				  	  0 . ENDCASE ;
END
forth_ok("$func 1 x .S", "1 ( )");
forth_ok("$func 2 x .S", "2 ( )");
forth_ok("$func 3 x .S", "3 ( )");
forth_ok("$func 4 x .S", "0 ( )");

note "Test RECURSE";
forth_ok(": fact DUP 1 > IF DUP 1- RECURSE * THEN ; 1 fact 2 fact 3 fact 4 fact .S",
		 "( 1 2 6 24 )");		 

note "Test IMMEDIATE";
forth_ok(": x           33 EMIT ; IMMEDIATE : y x ;", "!");

note "Test DOES>";
forth_ok(": x CREATE , DOES> @ 2* ;  2 x a  a .S", "( 4 )");

note "Test VALUE";
note "Test TO";
forth_ok("10 VALUE ten .S ten .S", "( )( 10 )");
forth_ok("10 VALUE ten      20 TO ten     .S ten .S", "( )( 20 )");
forth_ok("10 VALUE ten  : x 20 TO ten ; x .S ten .S", "( )( 20 )");

note "Test 2VALUE";
note "Test TO";
forth_ok("HEX 123456712345678. 2VALUE xx .S xx .S", "( )( 12345678 1234567 )");
forth_ok("HEX 123456712345678. 2VALUE xx 1111111122222222. TO xx .S xx .S",
		"( )( 22222222 11111111 )");
forth_ok("HEX 123456712345678. 2VALUE xx : x 1111111122222222. TO xx ; x .S xx .S",
		"( )( 22222222 11111111 )");

# error
forth_nok("CREATE xx 20 TO xx", "\nError: invalid name argument\n");
forth_nok("CREATE xx : x 20 TO xx ; x", "\nError: invalid name argument\n");

end_test;
