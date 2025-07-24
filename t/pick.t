#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test PICK";
forth_ok("3 2 1 0 PICK .S", "( 3 2 1 1 ) ");
forth_ok("3 2 1 1 PICK .S", "( 3 2 1 2 ) ");
forth_ok("3 2 1 2 PICK .S", "( 3 2 1 3 ) ");
forth_nok("3 2 1 4 PICK .S", "\nError: stack underflow\n");
forth_nok("3 2 1 -1 PICK .S", "\nError: invalid memory address\n");

end_test;
