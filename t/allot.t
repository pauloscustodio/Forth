#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ALLOT";
forth_ok("HERE  1 ALLOT HERE SWAP - .S", "( 1 )");
forth_ok("HERE -1 ALLOT HERE SWAP - .S", "( -1 )");

end_test;
