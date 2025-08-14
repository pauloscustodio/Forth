#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BL";
forth_ok("BL .S", "( 32 )");

end_test;
