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
forth_ok(": x 0 5 DO I . -1 +LOOP ; x", "5 4 3 2 1 0 ");

forth_ok(": x 0 0 ?DO I .     LOOP ; x", "");
forth_ok(": x 5 0 ?DO I .     LOOP ; x", "0 1 2 3 4 ");
forth_ok(": x 0 5 ?DO I . -1 +LOOP ; x", "5 4 3 2 1 0 ");

forth_ok(": x 2 0 DO 4 2 DO J . I . LOOP LOOP ; x", "0 2 0 3 1 2 1 3 ");

forth_ok(": x 10 0 DO I . I 1 = IF LEAVE THEN I 5 = IF LEAVE THEN LOOP .S ; x",
		 "0 1 ( )");
forth_ok(": x 10 0 DO I . I 5 = IF LEAVE THEN I 1 = IF LEAVE THEN LOOP .S ; x",
		 "0 1 ( )");
		 

forth_ok(": x 10 0 DO I . I 5 = IF UNLOOP EXIT THEN LOOP ; x", "0 1 2 3 4 5 ");

end_test;
