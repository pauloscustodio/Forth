#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test EMIT";
forth_ok("33 EMIT", "!");

end_test;
