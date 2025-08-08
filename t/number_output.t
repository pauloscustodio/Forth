#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test <#";
note "Test #";
note "Test #S";
note "Test #>";
forth_ok(" 1234567890123. .S SWAP OVER DABS <# #S ROT SIGN #> TYPE", 
		  "( 1912276171 287 )1234567890123");
forth_ok("255. HEX <# #S #> TYPE", "FF");
forth_ok(<<'END', "( 1912276171 287 )1,234,567,890,123");
		   1234567890123. .S 
		   <# 
		   # # # ',' HOLD 
		   # # # ',' HOLD 
		   # # # ',' HOLD 
		   # # # ',' HOLD 
		   #S #> TYPE
END
forth_ok("-1234567890123. SWAP OVER DABS .S <# #S ROT SIGN #> TYPE", 
		 "( -288 1912276171 287 )-1234567890123");
forth_ok("0. <# ".("#\n" x 256)." #> TYPE", "0" x 256);
forth_nok("0. <# ".("#\n" x 257)." #> TYPE", 
		  "\nError: Number output overflow\n");

note "Test SIGN";
forth_ok('<#  127. TUCK DABS #S ROT SIGN #> TYPE .S', '127( )');
forth_ok('<# -127. TUCK DABS #S ROT SIGN #> TYPE .S', '-127( )');

note "Test HOLD";
forth_ok('<# 12700 0 # # CHAR , HOLD #S CHAR $ HOLD #> TYPE .S', '$127,00( )');

note "Test HOLDS";
forth_ok('<# 127 0 #S S" Number:" HOLDS #> TYPE .S', "Number:127( )");

note "Test .";
forth_ok("0 .", "0 ");
forth_ok("-12345 .", "-12345 ");
forth_ok("12345 .", "12345 ");
forth_ok("255 HEX .", "FF ");

note "Test U.";
forth_ok("    0 U.", "0 ");
forth_ok("   -1 U.", "4294967295 ");
forth_ok("12345 U.", "12345 ");

note "Test .R";
forth_ok("-123 0 .R", "-123");
forth_ok("-123 1 .R", "-123");
forth_ok("-123 2 .R", "-123");
forth_ok("-123 3 .R", "-123");
forth_ok("-123 4 .R", "-123");
forth_ok("-123 5 .R", " -123");
forth_ok("123 0 .R", "123");
forth_ok("123 1 .R", "123");
forth_ok("123 2 .R", "123");
forth_ok("123 3 .R", "123");
forth_ok("123 4 .R", " 123");
forth_ok("123 5 .R", "  123");

note "Test U.R";
forth_ok("123 0 U.R", "123");
forth_ok("123 1 U.R", "123");
forth_ok("123 2 U.R", "123");
forth_ok("123 3 U.R", "123");
forth_ok("123 4 U.R", " 123");
forth_ok("123 5 U.R", "  123");

note "Test U.R";
forth_ok("-1 7 U.R", "4294967295");
forth_ok("-1 8 U.R", "4294967295");
forth_ok("-1 9 U.R", "4294967295");
forth_ok("-1 10 U.R", "4294967295");
forth_ok("-1 11 U.R", " 4294967295");
forth_ok("-1 12 U.R", "  4294967295");

note 'Test D.';
forth_ok("1. D. .S",	"1 ( )");
forth_ok("-1. D. .S",	"-1 ( )");

note 'Test D.R';
forth_ok("1. 0 D.R",	"1");
forth_ok("1. 1 D.R",	"1");
forth_ok("1. 2 D.R",	" 1");
forth_ok("1. 3 D.R",	"  1");
forth_ok("1. 4 D.R",	"   1");

forth_ok("-1. 0 D.R",	"-1");
forth_ok("-1. 1 D.R",	"-1");
forth_ok("-1. 2 D.R",	"-1");
forth_ok("-1. 3 D.R",	" -1");
forth_ok("-1. 4 D.R",	"  -1");


end_test;
