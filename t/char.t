#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CHAR";
forth_ok("CHAR ! .S", "( 33 )");

note "Test [CHAR]";
forth_ok(": x [CHAR] ! ; x .S", "( 33 )");

end_test;
