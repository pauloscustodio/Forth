#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test VALUE";
note "Test TO";
forth_ok("10 VALUE ten .S ten .S", "( )( 10 )");
forth_ok("10 VALUE ten      20 TO ten     .S ten .S", "( )( 20 )");
forth_ok("10 VALUE ten  : x 20 TO ten ; x .S ten .S", "( )( 20 )");

end_test;
