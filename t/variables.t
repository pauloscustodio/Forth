#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test STATE";
forth_ok("STATE @ .S", "( 0 )");
forth_ok(": x STATE @ ; IMMEDIATE : y x ; .S", "( 1 )");

end_test;
