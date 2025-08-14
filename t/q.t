#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

forth_ok("1 ? 2 ? 3 ? .S", "1 2 3 ( 1 2 3 )");

end_test;
