#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CONSTANT";
forth_ok("123 CONSTANT x  x  .S", "( 123 )");

end_test;
