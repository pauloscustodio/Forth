#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test !';
note 'Test @';
forth_ok('10 PAD !  PAD @ .S  11 PAD ! PAD @ .S', "( 10 )( 10 11 )");

end_test;
