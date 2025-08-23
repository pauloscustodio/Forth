#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BEGIN-STRUCTURE";
note "Test +FIELD";
note "Test END-STRUCTURE";
forth_ok(<<'END', "( ) ( 8 1000 1004 ) ");
	BEGIN-STRUCTURE point
		1 CELLS +FIELD p.x
		1 CELLS +FIELD p.y
	END-STRUCTURE
	.S
	point 
	1000 p.x
	1000 p.y
	.S
END

end_test;
