#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test VARIABLE";
forth_ok("VARIABLE x  123 x !  ' x >BODY @  .S", "( 123 )");

note 'Test 2VARIABLE';
forth_ok("2VARIABLE xx xx 2@ .S",			"( 0 0 )");
forth_ok("2VARIABLE xx 1. xx 2! xx 2@ .S",	"( 1 0 )");
forth_ok("2VARIABLE xx -1. xx 2! xx 2@ .S",	"( -1 -1 )");

note "Test ON";
note "Test OFF";
forth_ok("VARIABLE X  123 X !  X @  X ON   X @ .S", "( 123 -1 )");
forth_ok("VARIABLE X  123 X !  X @  X OFF  X @ .S", "( 123 0 )");

end_test;
