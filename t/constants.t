#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BL";
forth_ok("BL .S", "( 32 )");

note "Test S0";
forth_ok("S0 SP@ - .S", "( 4 )");
forth_ok("1 2 S0 SP@ - .S", "( 1 2 12 )");

note "Test R0";
forth_ok("R0 RSP@ - .S", "( 0 )");

note "Test PAD";
forth_ok("PAD .S", "( 0 )");

note "Test TIB";
forth_ok("TIB 11 TYPE", "TIB 11 TYPE");

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

end_test;
