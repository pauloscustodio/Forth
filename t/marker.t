#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test MARKER";
forth_ok(<<END, "( -1 -1 )");
		HERE
		MARKER mark
		: x 1 2 3 4 5 6 7 8 9 10 ;
		: y 1 2 3 4 5 6 7 8 9 10 ;
		: z 1 2 3 4 5 6 7 8 9 10 ;
		DUP HERE <> SWAP
		mark
		HERE =
		.S
END

end_test;
