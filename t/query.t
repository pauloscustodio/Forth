#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test QUERY";
capture_ok("echo .S | ./forth -e '1 2 QUERY INTERPRET'", "( 1 2 )");

end_test;
