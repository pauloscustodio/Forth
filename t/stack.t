#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DROP";
forth_ok("1 DROP .S  1 2 DROP .S", "( )( 1 )");
forth_nok("DROP", "\nError: stack underflow\n");

note "Test SWAP";
forth_ok("1 2 SWAP .S  3 SWAP .S", "( 2 1 )( 2 3 1 )");
forth_nok("1 SWAP", "\nError: stack underflow\n");

note "Test DUP";
forth_ok("1 DUP .S  2 DUP .S", "( 1 1 )( 1 1 2 2 )");
forth_nok("DUP", "\nError: stack underflow\n");

note "Test ?DUP";
forth_ok("0 ?DUP .S", "( 0 )");
forth_ok("1 ?DUP .S", "( 1 1 )");

note "Test OVER";
forth_ok("1 2 OVER .S", "( 1 2 1 )");
forth_nok("1 OVER", "\nError: stack underflow\n");

note "Test ROT";
forth_ok("1 2 3 ROT .S", "( 2 3 1 )");
forth_nok("1 2 ROT", "\nError: stack underflow\n");

note "Test -ROT";
forth_ok("1 2 3 -ROT .S", "( 3 1 2 )");
forth_nok("1 2 -ROT", "\nError: stack underflow\n");

note "Test NIP";
forth_ok("1 2 NIP .S", "( 2 )");
forth_nok("1 NIP", "\nError: stack underflow\n");

note "Test PICK";
forth_ok("1 2 3 0 PICK .S", "( 1 2 3 3 )");
forth_ok("1 2 3 1 PICK .S", "( 1 2 3 2 )");
forth_ok("1 2 3 2 PICK .S", "( 1 2 3 1 )");
forth_nok("1 2 3 3 PICK", "\nError: stack underflow\n");
forth_nok("3 2 1 -1 PICK .S", "\nError: invalid memory address\n");

note "Test ROLL";
forth_ok(" 1 2 3 4  0 ROLL .S", "( 1 2 3 4 )");
forth_ok(" 1 2 3 4  1 ROLL .S", "( 1 2 4 3 )");
forth_ok(" 1 2 3 4  2 ROLL .S", "( 1 3 4 2 )");
forth_ok(" 1 2 3 4  3 ROLL .S", "( 2 3 4 1 )");
forth_nok("1 2 3 4  4 ROLL", "\nError: stack underflow\n");
forth_nok("1 2 3 4  -1 ROLL", "\nError: invalid memory address\n");

note "Test TUCK";
forth_ok(" 1 2 TUCK .S", "( 2 1 2 )");
forth_nok("1   TUCK", "\nError: stack underflow\n");

note "Test 2DROP";
forth_ok("1 2 3 2DROP .S", "( 1 )");
forth_nok("1 2DROP", "\nError: stack underflow\n");

note "Test 2SWAP";
forth_ok("1 2 3 4 2SWAP .S", "( 3 4 1 2 )");
forth_nok("1 2 3 2SWAP", "\nError: stack underflow\n");

note "Test 2DUP";
forth_ok("1 2 2DUP .S", "( 1 2 1 2 )");
forth_nok("1 2DUP", "\nError: stack underflow\n");

note "Test 2OVER";
forth_ok("1 2 3 4 2OVER .S", "( 1 2 3 4 1 2 )");
forth_nok("1 2 3 2OVER", "\nError: stack underflow\n");

note "Test 2ROT";
forth_ok("1 2 3 4 5 6 2ROT .S", "( 3 4 5 6 1 2 )");
forth_nok("1 2 3 4 5 2ROT", "\nError: stack underflow\n");

note "Test -2ROT";
forth_ok("1 2 3 4 5 6 -2ROT .S", "( 5 6 1 2 3 4 )");
forth_nok("1 2 3 4 5 -2ROT", "\nError: stack underflow\n");

note "Test SP@";
note "Test SP!";
note "Test S0";
forth_ok("1 SP@ 2 SWAP SP! .S", "( 1 )");
forth_ok("1 2 3 4 5 6 S0 SP! .S", "( )");
forth_ok("S0 SP@ - .S", "( 1 )");
forth_ok("1 2 S0 SP@ - .S", "( 1 2 3 )");

note "Test DEPTH";
forth_ok("DEPTH . 1 DEPTH . 2 DEPTH . .S", "0 1 2 ( 1 2 )");

end_test;
