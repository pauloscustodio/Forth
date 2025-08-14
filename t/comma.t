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

end_test;
