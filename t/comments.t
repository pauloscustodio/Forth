#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

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

end_test;
