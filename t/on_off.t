#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

forth_ok("VARIABLE X  123 X !  X @  X ON   X @ .S", "( 123 -1 )");
forth_ok("VARIABLE X  123 X !  X @  X OFF  X @ .S", "( 123 0 )");

end_test;
