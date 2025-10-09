#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BL";
forth_ok("BL .S", "( 32 )");

note "Test PAD";
forth_ok("PAD .S", "( 2048 )");

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

end_test;
