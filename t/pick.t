#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test PICK";
forth_ok("1 2 3 0 PICK .S", "( 1 2 3 3 )");
forth_ok("1 2 3 1 PICK .S", "( 1 2 3 2 )");
forth_ok("1 2 3 2 PICK .S", "( 1 2 3 1 )");
forth_nok("3 2 1 4 PICK .S", "\nError: stack underflow\n");
forth_nok("3 2 1 -1 PICK .S", "\nError: invalid memory address\n");

end_test;
