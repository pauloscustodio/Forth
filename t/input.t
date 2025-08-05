#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test >IN";
forth_ok(">IN @ .S", "( 6 )");
forth_ok("1 2 3 100 >IN ! .S", "");

note "Test SOURCE";
forth_ok("SOURCE TYPE", "SOURCE TYPE");

note "Test SOURCE-ID";
capture_ok("echo SOURCE-ID .S | ./forth", "( 0 ) ok\n");
forth_ok("SOURCE-ID .S", "( 1 )");
forth_ok("CHAR ! WORD SOURCE-ID! COUNT EVALUATE .S", "( -1 )");

note "Test ACCEPT";
capture_ok("echo hello | ./forth -e 'PAD 255 ACCEPT PAD SWAP TYPE'",
		   "hello");

note "Test KEY";
capture_ok("echo ! | ./forth -e 'KEY .S'", "( 33 )");

end_test;
