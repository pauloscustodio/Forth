#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ENVIRONMENT?";

forth_ok('S" /COUNTED-STRING" 	ENVIRONMENT? .S', "( 255 -1 )");
forth_ok('S" /counted-string" 	ENVIRONMENT? .S', "( 255 -1 )");

forth_ok('S" /HOLD" 			ENVIRONMENT? .S', "( 128 -1 )");
forth_ok('S" /PAD" 				ENVIRONMENT? .S', "( 256 -1 )");
forth_ok('S" ADDRESS-UNIT-BITS"	ENVIRONMENT? .S', "( 8 -1 )");
forth_ok('S" CORE" 				ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" CORE-EXT" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FLOORED"  			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" MAX-CHAR" 			ENVIRONMENT? .S', "( 255 -1 )");
forth_ok('S" MAX-D" 			ENVIRONMENT? .S', "( -1 2147483647 -1 )");
forth_ok('S" MAX-N" 			ENVIRONMENT? .S', "( 2147483647 -1 )");
forth_ok('S" MAX-U" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" MAX-UD" 			ENVIRONMENT? .S', "( -1 -1 -1 )");
forth_ok('S" RETURN-STACK-CELLS"ENVIRONMENT? .S', "( 4096 -1 )");
forth_ok('S" STACK-CELLS" 		ENVIRONMENT? .S', "( 4096 -1 )");
forth_ok('S" BLOCK" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" BLOCK-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" DOUBLE" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" DOUBLE-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" EXCEPTION" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" EXCEPTION-EXT" 	ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FACILITY" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FACILITY-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FILE" 				ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FILE-EXT" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FLOATING" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FLOATING-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" FLOATING-STACK" 	ENVIRONMENT? .S', "( 0 )");
forth_ok('S" MAX_FLOAT" 		ENVIRONMENT? .S', "( 0 )");
forth_ok('S" LOCALS" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" LOCALS-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" #LOCALS" 			ENVIRONMENT? .S', "( 0 )");
forth_ok('S" MEMORY-ALLOC" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" MEMORY-ALLOC-EXT" 	ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" TOOLS" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" TOOLS-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" SEARCH-ORDER" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" SEARCH-ORDER-EXT" 	ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" WORDLISTS" 		ENVIRONMENT? .S', "( 0 )");
forth_ok('S" STRING" 	 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" STRING-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");

end_test;
