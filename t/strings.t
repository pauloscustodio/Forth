#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test COUNT';
note 'Test TYPE';
forth_ok("HERE 2 C, 'O' C, 'k' C,  COUNT TYPE", "Ok");

end_test;
