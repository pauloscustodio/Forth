#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test ,';
note 'Test HERE';
forth_ok('HERE 123 , @ .S', "( 123 )");
forth_ok("HERE 123 , HERE SWAP - .S", "( 4 )");

note 'Test C,';
note 'Test HERE';
note 'Test COUNT';
note 'Test TYPE';
forth_ok("HERE 2 C, 'O' C, 'K' C, COUNT TYPE", "OK");

note "Test CHAR";
forth_ok("CHAR ! .S", "( 33 )");

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

note "Test ALLOT";
forth_ok("HERE  1 ALLOT HERE SWAP - .S", "( 1 )");
forth_ok("HERE -1 ALLOT HERE SWAP - .S", "( -1 )");

note "Test UNUSED";
forth_ok("UNUSED 1 , UNUSED - .S", "( 1 )");

end_test;
