#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BASE";
note "Test DECIMAL";
note "Test HEX";
forth_ok("        BASE @  DECIMAL .S", "( 10 )");
forth_ok("DECIMAL BASE @  DECIMAL .S", "( 10 )");
forth_ok("HEX     BASE @  DECIMAL .S", "( 16 )");
forth_ok("  2 BASE ! 1010 DECIMAL .S", "( 10 )");
forth_ok(" 16 BASE !    A DECIMAL .S", "( 10 )");
forth_ok("HEX           A DECIMAL .S", "( 10 )");

end_test;
