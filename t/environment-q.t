#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ENVIRONMENT?";
forth_ok('S" CORE" ENVIRONMENT? .S', "( -1 -1 ) ");
forth_ok('S" ????" ENVIRONMENT? .S', "( 0 ) ");

end_test;
