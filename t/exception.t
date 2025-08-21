#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ABORT";
forth_nok("ABORT", "");
forth_nok("1 -1 THROW .S", "");

note 'Test ABORT"';
forth_ok('1 2 0 ABORT" error" ." ok" .S', "ok ( 1 2 )");
forth_ok(': x ABORT" error" ." ok" ;  1 2 0 x .S', "ok ( 1 2 )");

forth_nok('-1 ABORT" error" ." ok"', "\nAborted: error\n");
forth_nok(': x ABORT" error" ." ok" ; -1 x', "\nAborted: error\n");

note "Test THROW";
forth_ok("1 0 THROW .S", "( 1 )");
forth_nok("1 -1 THROW .S", "");
forth_nok("1 -2 THROW .S", "\nAborted: \n");
forth_nok("1 -3 THROW .S", "\nError: stack overflow\n");
forth_nok("1 -4 THROW .S", "\nError: stack underflow\n");
forth_nok("1 -4095 THROW .S", "\nError: -4095\n");

note "Test CATCH";
my $code = <<'END';
	VARIABLE error
	: aa 1 error @ THROW 2 ;
	: bb 3 aa 4 ;
	: cc 5 bb 6 ;
	: dd 7 cc 8 ;
	: test dd .S ;
END
forth_ok("$code  0 error ! test", "( 7 5 3 1 2 4 6 8 )");
forth_nok("$code 1 error ! test", "\nError: 1\n");

forth_ok("$code : x ['] test CATCH ; 0 0 error ! x .S", 
		"( 0 7 5 3 1 2 4 6 8 ) ( 0 7 5 3 1 2 4 6 8 0 ) "); 

forth_ok("$code : x ['] test CATCH ; 0 1 error ! x .S", 
		"( 0 1 ) "); 

end_test;
