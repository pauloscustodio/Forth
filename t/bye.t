#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BYE";
forth_ok("1 .S BYE 2 .S", "( 1 )");

end_test;
