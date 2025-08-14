#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test LATEST";
forth_ok(<<END, "x( -1 )");
		HERE
		: x 1 2 3 4 5 6 7 8 9 10 ;
		LATEST = 
		LATEST CELL+ @ COUNT TYPE
		.S
END

end_test;
