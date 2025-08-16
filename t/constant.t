#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CONSTANT";
forth_ok("123 CONSTANT x  x  .S", "( 123 )");

note 'Test 2CONSTANT';
forth_ok("1. .S 2CONSTANT xx .S xx .S",		"( 1 0 )( )( 1 0 )");
forth_ok("-1. .S 2CONSTANT xx .S xx .S",	"( -1 -1 )( )( -1 -1 )");

note "Test BL";
forth_ok("BL .S", "( 32 )");

note "Test PAD";
forth_ok("PAD .S", "( 1028 )");

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

note "Test S0";
forth_ok("S0 .S", "( 4096 )");

note "Test R0";
forth_ok("R0 .S", "( 4096 )");

end_test;
