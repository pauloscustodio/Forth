#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test PAD";
forth_ok("PAD .S", "( 0 )");
forth_ok("PAD C@ .S", "( 32 )");

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
forth_ok('9223372036854775807. PAD 2!  PAD 2@ .S', "( -1 2147483647 )");

note "Test FILL";
forth_ok("PAD 10 BL FILL PAD 5 'x' FILL PAD 10 TYPE", "xxxxx     ");

note "Test ERASE";
forth_ok(<<END, "00000000  00 00 00 00 00 20 20 20 20 20                   .....           \n");
		PAD 10 BL FILL 
		PAD 5 ERASE 
		PAD 10 DUMP
END

note "Test MOVE";
forth_ok("HERE 'h' C, 'e' C, 'l' C, 'l' C, 'o' C, PAD 5 MOVE PAD 5 TYPE", "hello");

end_test;
