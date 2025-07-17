#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test C!';
note 'Test C@';
forth_ok('10 PAD C!  PAD C@ .S  11 PAD C! PAD C@ .S', "( 10 )( 10 11 )");

end_test;
