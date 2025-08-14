#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SAVE-INPUT";
note "Test RESTORE-INPUT";
forth_ok(': x SAVE-INPUT S" .S" EVALUATE RESTORE-INPUT ; x .S', 
		 "( )( 0 )");
forth_ok('RESTORE-INPUT .S', 
		 "( -1 )");

end_test;
