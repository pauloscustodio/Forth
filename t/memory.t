#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test !';
note 'Test @';
forth_ok('10 PAD !  PAD @ .S  11 PAD ! PAD @ .S', "( 10 )( 10 11 )");

note 'Test +!';
forth_ok('10 PAD !  2 PAD +!  PAD @ .S', "( 12 )");

note 'Test C!';
note 'Test C@';
forth_ok('10 PAD C!  PAD C@ .S  11 PAD C! PAD C@ .S', "( 10 )( 10 11 )");

note 'Test 2!';
note 'Test 2@';
forth_ok('1234567890123. PAD 2!  PAD 2@ .S', "( 1912276171 287 )");

end_test;
