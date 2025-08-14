#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test KEY";
capture_ok("echo ! | ./forth -e 'KEY .S'", "( 33 )");

end_test;
