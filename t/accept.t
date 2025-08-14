#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ACCEPT";
capture_ok("echo hello | ./forth -e 'PAD 255 ACCEPT PAD SWAP TYPE'",
		   "hello");

end_test;
