#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test IMMEDIATE";
forth_ok(": x           33 EMIT ; IMMEDIATE : y x ;", "!");

end_test;
