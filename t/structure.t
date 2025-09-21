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

note "Test CFIELD:";
note "Test FIELD:";
note "Test 2FIELD:";
note "Test FFIELD:";
note "Test SFFIELD:";
note "Test DFFIELD:";
forth_ok(<<'END', "( ) ( 44 1000 1004 1008 1012 1020 1028 1036 ) ");
	BEGIN-STRUCTURE s
		CFIELD:  s.char1
		FIELD:   s.cell
		CFIELD:  s.char2
		2FIELD:  s.dcell
		FFIELD:  s.fcell
		SFFIELD: s.sfcell
		DFFIELD: s.dfcell
	END-STRUCTURE
	.S
	s 
	1000 s.char1
	1000 s.cell
	1000 s.char2
	1000 s.dcell
	1000 s.fcell
	1000 s.sfcell
	1000 s.dfcell
	.S
END

end_test;
