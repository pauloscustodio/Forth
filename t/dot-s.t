#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test .S";
forth_ok("     .S", "( )");
forth_ok("1 -1 .S", "( 1 -1 )");

end_test;
