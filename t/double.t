#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test 2LITERAL';
forth_ok(": xx [ 1. ] 2LITERAL ; .S xx .S",		"( )( 1 0 )");
forth_ok(": xx [ -1. ] 2LITERAL ; .S xx .S",	"( )( -1 -1 )");

note "Test DABS";
forth_ok(" 1. .S DABS .S", "( 1 0 )( 1 0 )");
forth_ok(" 0. .S DABS .S", "( 0 0 )( 0 0 )");
forth_ok("-1. .S DABS .S", "( -1 -1 )( 1 0 )");

end_test;
