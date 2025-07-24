#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test THROW";
forth_ok("0 THROW", "");
forth_nok("-3 THROW", "\nError: stack overflow\n");
forth_nok("3 THROW", "\nError: 3\n");
forth_nok("THROW", "\nError: stack underflow\n");
forth_nok("xxxx", "\nError: undefined word: xxxx\n");

end_test;
