#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DROP";
forth_ok("1 DROP .S  1 2 DROP .S", "( )( 1 )");
forth_nok("DROP", "\nError: stack underflow\n");

end_test;
