#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test RECURSE";
forth_ok(": fact DUP 1 > IF DUP 1- RECURSE * THEN ; 1 fact 2 fact 3 fact 4 fact .S",
		 "( 1 2 6 24 )");		 

end_test;
