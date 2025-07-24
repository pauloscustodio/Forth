#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test S"';
note 'Test TYPE';
forth_ok('S" hello world" .S', "( 5 11 ) ");
forth_ok('S" hello world" TYPE', "hello world");

end_test;
