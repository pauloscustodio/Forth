#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test !';
note 'Test @';
forth_ok('10 PAD !  PAD @ .S  11 PAD ! PAD @ .S', "( 10 )( 10 11 )");

note 'Test +!';
forth_ok('10 PAD !  2 PAD +!  PAD @ .S', "( 12 )");

note 'Test C!';
note 'Test C@';
forth_ok('10 PAD C!  PAD C@ .S  11 PAD C! PAD C@ .S', "( 10 )( 10 11 )");

note 'Test 2!';
note 'Test 2@';
forth_ok('1234567890123. PAD 2!  PAD 2@ .S', "( 1912276171 287 )");

note "Test FILL";
forth_ok("PAD 10 BL FILL PAD 5 'x' FILL PAD 10 TYPE", "xxxxx     ");

note "Test ERASE";
forth_ok(<<END, "0 0 0 0 0 32 32 32 32 32 ");
		PAD 10 BL FILL 
		PAD 5 ERASE 
		PAD 0 + C@ .
		PAD 1 + C@ .
		PAD 2 + C@ .
		PAD 3 + C@ .
		PAD 4 + C@ .
		PAD 5 + C@ .
		PAD 6 + C@ .
		PAD 7 + C@ .
		PAD 8 + C@ .
		PAD 9 + C@ .
END

note "Test MOVE";
forth_ok("BL WORD hello COUNT PAD SWAP MOVE PAD 5 TYPE", "hello");

end_test;
