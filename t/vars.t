#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test STATE";
forth_ok("STATE @ .S", "( 0 )");
forth_ok(": x STATE @ ; IMMEDIATE : y x ; .S", "( 1 )");

note "Test BASE";
note "Test DECIMAL";
note "Test HEX";
forth_ok("        BASE @  DECIMAL .S", "( 10 )");
forth_ok("DECIMAL BASE @  DECIMAL .S", "( 10 )");
forth_ok("HEX     BASE @  DECIMAL .S", "( 16 )");
forth_ok("  2 BASE ! 1010 DECIMAL .S", "( 10 )");
forth_ok(" 16 BASE !    A DECIMAL .S", "( 10 )");
forth_ok("HEX           A DECIMAL .S", "( 10 )");

note "Test DPL";
forth_ok("123. DPL @ .S", "( 123 0 0 )");
forth_ok("12.3 DPL @ .S", "( 123 0 1 )");
forth_ok("1.23 DPL @ .S", "( 123 0 2 )");
forth_ok(".123 DPL @ .S", "( 123 0 3 )");

end_test;
