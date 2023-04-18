#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test +";
forth_ok("1 2 + .S", "( 3 )");

note "Test -";
forth_ok("2 1 - .S", "( 1 )");

note "Test 1+";
forth_ok("2 1+ .S", "( 3 )");

note "Test 1-";
forth_ok("2 1- .S", "( 1 )");

end_test;
