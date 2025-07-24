#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DROP";
forth_ok("1 2 3 DROP .S", "( 1 2 ) ");
forth_ok("1 2 3 DROP DROP DROP .S", "( ) ");
forth_nok("DROP", "\nError: stack underflow\n");

end_test;
