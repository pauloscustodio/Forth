#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test EXIT";
note "Test EXECUTE";
note "Test LITERAL";
note "Test [";
note "Test ]";
forth_ok(<<'END', "( 1 )");
		: x [
		1 LITERAL   		\ 1
		] EXIT [   			\ exit
		2 LITERAL   		\ 2
		] ;
		' x EXECUTE			\ and runs it
		.S
END

end_test;
