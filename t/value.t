#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test VALUE";
note "Test TO";
forth_ok("10 VALUE ten .S ten .S", "( )( 10 )");
forth_ok("10 VALUE ten      20 TO ten     .S ten .S", "( )( 20 )");
forth_ok("10 VALUE ten  : x 20 TO ten ; x .S ten .S", "( )( 20 )");

note "Test 2VALUE";
note "Test TO";
forth_ok("HEX 123456712345678. 2VALUE xx .S xx .S", "( )( 12345678 1234567 )");
forth_ok("HEX 123456712345678. 2VALUE xx 1111111122222222. TO xx .S xx .S",
		"( )( 22222222 11111111 )");
forth_ok("HEX 123456712345678. 2VALUE xx : x 1111111122222222. TO xx ; x .S xx .S",
		"( )( 22222222 11111111 )");

# error
forth_nok("CREATE xx 20 TO xx", "\nError: Invalid destination to TO\n");
forth_nok("CREATE xx : x 20 TO xx ; x", "\nError: Invalid destination to TO\n");

end_test;
