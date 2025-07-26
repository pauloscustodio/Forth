#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CR";
forth_ok("CR", "\n");

end_test;
