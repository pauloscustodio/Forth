#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test EXPECT";
note "Test SPAN";
capture_ok("echo hello | ./forth -e 'PAD 255 EXPECT SPAN @ . .S PAD SPAN @ TYPE'",
		   "5 ( )hello");

end_test;
