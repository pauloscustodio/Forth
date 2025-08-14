#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ALIGN";
forth_ok("HERE ALIGN HERE SWAP - .S", "( 0 )");
forth_ok("1 C, HERE ALIGN HERE SWAP - .S", "( 3 )");

note "Test ALIGNED";
forth_ok("0 ALIGNED .S", "( 0 )");
forth_ok("1 ALIGNED .S", "( 4 )");
forth_ok("2 ALIGNED .S", "( 4 )");
forth_ok("3 ALIGNED .S", "( 4 )");
forth_ok("4 ALIGNED .S", "( 4 )");
forth_ok("5 ALIGNED .S", "( 8 )");

end_test;
