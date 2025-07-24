#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test +";
forth_ok("1 2 + .S", "( 3 ) ");
forth_nok("3 +", "\nError: stack underflow\n");

end_test;
