#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test AND";
forth_ok("0 0 AND 0 1 AND 1 0 AND 1 1 AND .S", "( 0 0 0 1 )");

note "Test OR";
forth_ok("0 0 OR 0 1 OR 1 0 OR 1 1 OR .S", "( 0 1 1 1 )");

note "Test XOR";
forth_ok("0 0 XOR 0 1 XOR 1 0 XOR 1 1 XOR .S", "( 0 1 1 0 )");

note "Test INVERT";
forth_ok("-1 INVERT .S", "( 0 )");

note "Test RSHIFT";
forth_ok("20 2 RSHIFT .S", "( 5 )");
forth_ok("-1 1 RSHIFT HEX .S", "( 7FFFFFFF )"); # must be unsigned shift

note "Test LSHIFT";
forth_ok("5 2 LSHIFT .S", "( 20 )");

end_test;
