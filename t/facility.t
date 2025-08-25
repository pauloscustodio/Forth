#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test EMIT?";
forth_ok("EMIT? .S", "( -1 )");

end_test;
