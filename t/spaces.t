#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SPACE";
forth_ok("SPACE", " ");

note "Test SPACES";
forth_ok("-1 SPACES", "");
forth_ok(" 0 SPACES", "");
forth_ok(" 1 SPACES", " ");
forth_ok(" 2 SPACES", "  ");
forth_ok(" 3 SPACES", "   ");

end_test;
