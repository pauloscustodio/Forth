#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test FALSE";
forth_ok("FALSE .S", "( 0 )");

note "Test TRUE";
forth_ok("TRUE .S", "( -1 )");

end_test;
