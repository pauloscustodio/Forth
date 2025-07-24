#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test .";
forth_ok("-1 .", "-1 ");
forth_ok("0 .", "0 ");
forth_nok(".", "\nError: stack underflow\n");

end_test;
