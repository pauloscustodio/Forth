#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DOES>";
forth_ok(": x CREATE , DOES> @ 2* ;  2 x a  a .S", "( 4 )");

end_test;
