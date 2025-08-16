#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test >R";
note "Test R>";
forth_ok("1 >R -1 >R .S .RS R> R> .S .RS", "( )(R: 1 -1 )( -1 1 )(R: )");

note "Test 2>R";
note "Test 2R>";
note "Test 2R@";
forth_ok("1 2 2>R .RS  2R@ .S .RS  2R> .S .RS", 
		 "(R: 1 2 )( 1 2 )(R: 1 2 )( 1 2 1 2 )(R: )");

note "Test R@";
note "Test I";
note "Test J";
forth_ok("1 >R 2 >R 3 >R  R@ . I . J . .RS BYE", "3 3 1 (R: 1 2 3 )");


note "Test RDROP";
forth_ok("1 >R -1 >R .S .RS RDROP RDROP .S .RS", "( )(R: 1 -1 )( )(R: )");

note "Test RSP@";
note "Test RSP!";
note "Test R0";
forth_ok("R0 RSP@ - .S", "( 0 )");
forth_ok("1 >R RSP@ -1 >R RSP! .S .RS RDROP", "( )(R: 1 )");

end_test;
