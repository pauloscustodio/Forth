#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

forth_nok(".", "\nError: stack underflow\n");
forth_ok(("0 " x 1024).".", "0 ");
forth_nok(("0 " x 1025).".", "\nError: stack overflow\n");

note "Test DUP";
forth_ok("1 DUP .S", "( 1 1 )");
forth_nok("DUP", "\nError: stack underflow\n");

note "Test DROP";
forth_ok("1 2 DROP .S", "( 1 )");
forth_nok("DROP", "\nError: stack underflow\n");

note "Test PICK";
forth_ok("1 2 3  0 PICK .S", "( 1 2 3 3 )");
forth_ok("1 2 3  1 PICK .S", "( 1 2 3 2 )");
forth_ok("1 2 3  2 PICK .S", "( 1 2 3 1 )");
forth_nok("1 2 3  3 PICK .S", "\nError: stack underflow\n");
forth_nok("1 2 3  -1 PICK .S", "\nError: invalid memory address\n");

end_test;
