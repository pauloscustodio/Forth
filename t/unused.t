#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test UNUSED";
forth_ok("UNUSED 1 , UNUSED - .S", "( 4 )");

end_test;
