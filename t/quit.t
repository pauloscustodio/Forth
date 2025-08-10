#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test QUIT";
note "Test REFILL";
note "Test INTERPRET";
forth_ok("1 .S QUIT 2 .S", "( 1 )");


end_test;
