#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DPL";
forth_ok("123. DPL @ .S", "( 123 0 0 )");
forth_ok("12.3 DPL @ .S", "( 123 0 1 )");
forth_ok("1.23 DPL @ .S", "( 123 0 2 )");
forth_ok(".123 DPL @ .S", "( 123 0 3 )");

end_test;
