#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test 2CONSTANT';
forth_ok("1. .S 2CONSTANT xx .S xx .S",		"( 1 0 )( )( 1 0 )");
forth_ok("-1. .S 2CONSTANT xx .S xx .S",	"( -1 -1 )( )( -1 -1 )");

note 'Test 2LITERAL';
forth_ok(": xx [ 1. ] 2LITERAL ; .S xx .S",		"( )( 1 0 )");
forth_ok(": xx [ -1. ] 2LITERAL ; .S xx .S",	"( )( -1 -1 )");

note 'Test 2VARIABLE';
forth_ok("2VARIABLE xx xx 2@ .S",			"( 0 0 )");
forth_ok("2VARIABLE xx 1. xx 2! xx 2@ .S",	"( 1 0 )");
forth_ok("2VARIABLE xx -1. xx 2! xx 2@ .S",	"( -1 -1 )");

note "Test DABS";
forth_ok(" 1. .S DABS .S", "( 1 0 )( 1 0 )");
forth_ok(" 0. .S DABS .S", "( 0 0 )( 0 0 )");
forth_ok("-1. .S DABS .S", "( -1 -1 )( 1 0 )");

end_test;
