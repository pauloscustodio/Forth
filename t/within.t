#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WITHIN";
forth_ok("1  2 4 WITHIN .S", "( 0 )");
forth_ok("2  2 4 WITHIN .S", "( -1 )");
forth_ok("3  2 4 WITHIN .S", "( -1 )");
forth_ok("4  2 4 WITHIN .S", "( 0 )");
forth_ok("1  4 2 WITHIN .S", "( -1 )");
forth_ok("2  4 2 WITHIN .S", "( 0 )");
forth_ok("3  4 2 WITHIN .S", "( 0 )");
forth_ok("4  4 2 WITHIN .S", "( -1 )");

end_test;
