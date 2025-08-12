#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

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

end_test;
