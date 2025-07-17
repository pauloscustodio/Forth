#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test PAD";
forth_ok("PAD .S", "( 0 )");

end_test;
