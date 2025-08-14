#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SOURCE-ID";
capture_ok("echo SOURCE-ID .S | ./forth", "( 0 ) ok\n");
forth_ok("SOURCE-ID .S", "( 1 )");
forth_ok("CHAR ! WORD SOURCE-ID! COUNT EVALUATE .S", "( -1 )");

end_test;
