#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SOURCE";
forth_ok("SOURCE TYPE", "SOURCE TYPE");

note "Test SOURCE-ID";
capture_ok("echo SOURCE-ID .S | ./forth", "( 0 ) ok\n");
forth_ok("SOURCE-ID .S", "( 1 )");

end_test;
