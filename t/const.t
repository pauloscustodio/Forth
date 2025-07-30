#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test PAD";
forth_ok("PAD .S", "( 3076 )");

note "Test BL";
forth_ok("BL .S", "( 32 )");

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

note "Test S0";
forth_ok("S0 .S", "( 4096 )");

note "Test R0";
forth_ok("R0 .S", "( 4096 )");

note "Test CS0";
forth_ok("CS0 .S", "( 4096 )");

note "Test TIB";
forth_ok("TIB 11 TYPE", "TIB 11 TYPE");
forth_ok("TIB .S", "( 2052 )");

end_test;
