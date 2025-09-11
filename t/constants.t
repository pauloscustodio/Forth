#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BL";
forth_ok("BL .S", "( 32 )");

note "Test S0";
forth_ok("S0 .S", "( 4096 )");
forth_ok("S0 SP@ - .S", "( 1 )");
forth_ok("1 2 S0 SP@ - .S", "( 1 2 3 )");

note "Test R0";
forth_ok("R0 RSP@ - .S", "( 0 )");
forth_ok("R0 .S", "( 4096 )");

note "Test PAD";
forth_ok("PAD .S", "( 2048 )");

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

end_test;
