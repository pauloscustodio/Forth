#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test S"';
note 'Test TYPE';
forth_ok('S" hello world" .S', "( 465 11 ) ");
forth_ok('     S" "        TYPE', "");
forth_ok('     S" 123"     TYPE', "123");

end_test;
