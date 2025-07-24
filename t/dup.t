#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DUP";
forth_ok("1 DUP .S", "( 1 1 ) ");
forth_nok("DUP", "\nError: stack underflow\n");

end_test;
