#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CASE";
note "Test OF";
note "Test ENDOF";
note "Test ENDCASE";
my $func = ": x CASE 33 EMIT ENDCASE ;";
forth_ok("$func 1 x .S", "!( )");
forth_ok("$func 2 x .S", "!( )");

$func = <<END;
		: x CASE 1 OF 1 . ENDOF 
				 2 OF 2 . ENDOF 
				 3 OF 3 . ENDOF 
				  	  0 . ENDCASE ;
END
forth_ok("$func 1 x .S", "1 ( )");
forth_ok("$func 2 x .S", "2 ( )");
forth_ok("$func 3 x .S", "3 ( )");
forth_ok("$func 4 x .S", "0 ( )");

end_test;
