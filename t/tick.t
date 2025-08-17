#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test '";
note "Test EXECUTE";
forth_nok("'", "\nError: attempt to use zero-length string as a name\n");
forth_nok("' hello", "\nError: undefined word: hello\n");
forth_ok("1 2 3 ' .S EXECUTE", "( 1 2 3 )");

note "Test [']";
note "Test EXECUTE";
forth_nok(": x ['] hello ; x", "\nError: undefined word: hello\n");
forth_ok(": x ['] .S EXECUTE ; 1 2 3 x", "( 1 2 3 )");

end_test;
