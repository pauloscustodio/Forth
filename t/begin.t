#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

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

end_test;
