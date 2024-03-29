#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SOURCE";
forth_ok("SOURCE TYPE", "SOURCE TYPE");

note "Test SOURCE-ID";
capture_ok("echo SOURCE-ID .S | ./forth", "( 0 ) ok\n");
forth_ok("SOURCE-ID .S", "( 1 )");

note "Test ACCEPT";
capture_ok("echo hello | ./forth -e 'PAD 255 ACCEPT PAD SWAP TYPE'",
		   "hello");

note "Test EXPECT";
note "Test SPAN";
capture_ok("echo hello | ./forth -e 'PAD 255 EXPECT SPAN @ . .S PAD SPAN @ TYPE'",
		   "5 ( )hello");

note "Test KEY";
capture_ok("echo ! | ./forth -e 'KEY .S'", "( 33 )");

note "Test #IN";
forth_ok("#IN @ .S", "( 8 )");

note "Test #TIB";
forth_ok("#TIB @ .S", "( 9 )");

note "Test >IN";
forth_ok(">IN @ .S", "( 6 )");
forth_ok("1 2 3 100 >IN ! .S", "");

note "Test QUERY";
capture_ok("echo .S | ./forth -e '1 2 QUERY INTERPRET'", "( 1 2 )");

note "Test SAVE-INPUT";
note "Test RESTORE-INPUT";
forth_ok(': x SAVE-INPUT S" .S" EVALUATE RESTORE-INPUT ; x .S', 
		 "( 51 49 1 )( -1 )");

end_test;
