#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test .S";
forth_ok("     .S", "( )");
forth_ok("1 -1 .S", "( 1 -1 )");

note "Test .FS";
forth_ok("     .FS", "(F: )");
forth_ok("1.1E -1.1E .FS", "(F: 1.1 -1.1 )");

note "Test .RS";
forth_ok(".RS", "(R: )");
forth_ok("1 >R 2 >R .RS BYE", "(R: 1 2 )");

note "Test N>R";
note "Test NR>";
forth_ok(<<'END', "( ) ( 1 2 3 4 )");
	: x
		1 2 3 4  4 N>R .S
		NR> .S
	;
	x
END

note "Test FORGET";
forth_ok(<<'END', "( 1 2 )");
	: x1 1 ;
	: x2 2 ;
	: x3 3 ;
	FORGET x3
	x1 x2 .S
END

forth_nok(<<'END', "\nError: undefined word: x3\n");
	: x1 1 ;
	: x2 2 ;
	: x3 3 ;
	FORGET x3
	x1 x2 x3 .S
END

note "Test NAME>STRING";
forth_ok(": x ; LATEST NAME>STRING TYPE .S", "x( )");

end_test;
