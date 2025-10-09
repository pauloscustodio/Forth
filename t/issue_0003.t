#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

forth_ok(<<'END', "1 10 20 ( )");
	: aa { a b } 1 THROW ." not reached" ;
	: bb { a b } a b aa ;
	: cc { a b } a b bb ; 
	: dd 1 2 cc ;
	: test dd .S ;
	: x { a b } ['] test CATCH . a . b . ;
	10 20 x .S
END

end_test;
