#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

$ENV{FORTH} = 'TRACE ON  ." Forth ready" CR';
forth_ok("1 2 + . .S", <<'END');
 ( ) 
>> ." Forth ready ( ) 
>> CR 
 ( ) 
 ( ) 
>> QUIT 
> 1 2 + . .S
>> 1  ( 1 ) 
>> 2  ( 1 2 ) 
>> +  ( 3 ) 
>> . 3  ( ) 
>> .S ( )  ( ) 
END

delete $ENV{FORTH};
forth_ok("1 2 + . .S", "3 ( ) ");

note "Test ENVIRONMENT?";

# test case sensitivity and unknown query
forth_ok('S" /COUNTED-STRING" 	ENVIRONMENT? .S', "( 255 -1 )");
forth_ok('S" /counted-string" 	ENVIRONMENT? .S', "( 255 -1 )");
forth_ok('S" ????" ENVIRONMENT? .S', "( 0 ) ");

# test standard queries
forth_ok('S" /COUNTED-STRING" 	ENVIRONMENT? .S', "( 255 -1 )");
forth_ok('S" /HOLD" 			ENVIRONMENT? .S', "( 256 -1 )");
forth_ok('S" /PAD" 				ENVIRONMENT? .S', "( 256 -1 )");
forth_ok('S" ADDRESS-UNIT-BITS"	ENVIRONMENT? .S', "( 8 -1 )");
forth_ok('S" FLOORED"  			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" MAX-CHAR" 			ENVIRONMENT? .S', "( 127 -1 )");
forth_ok('S" MAX-D" 			ENVIRONMENT? .S', "( -1 2147483647 -1 )");
forth_ok('S" MAX-N" 			ENVIRONMENT? .S', "( 2147483647 -1 )");
forth_ok('S" MAX-U" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" MAX-UD" 			ENVIRONMENT? .S', "( -1 -1 -1 )");
forth_ok('S" RETURN-STACK-CELLS"ENVIRONMENT? .S', "( 65536 -1 )");
forth_ok('S" STACK-CELLS" 		ENVIRONMENT? .S', "( 65536 -1 )");

# deprecated queries
forth_ok('S" CORE" 				ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" CORE-EXT" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" BLOCK" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" BLOCK-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" DOUBLE" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" DOUBLE-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" EXCEPTION" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" EXCEPTION-EXT" 	ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FACILITY" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FACILITY-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FILE" 				ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FILE-EXT" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FLOATING" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FLOATING-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" FLOATING-STACK" 	ENVIRONMENT? .S', "( 65536 -1 )");
forth_ok('S" MAX-FLOAT" 		ENVIRONMENT? .S .FS', "( -1 ) (F: 1.79769e+308 )");
forth_ok('S" LOCAL" 			ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" LOCAL-EXT" 		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" #LOCALS" 			ENVIRONMENT? .S', "( 65536 -1 )");
forth_ok('S" MEMORY"      		ENVIRONMENT? .S', "( -1 -1 )");
forth_ok('S" TOOLS" 			ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" TOOLS-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" SEARCH-ORDER" 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" SEARCH-ORDER-EXT" 	ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" WORDLISTS" 		ENVIRONMENT? .S', "( 0 )");
forth_ok('S" STRING" 	 		ENVIRONMENT? .S', "( 0 -1 )");
forth_ok('S" STRING-EXT" 		ENVIRONMENT? .S', "( 0 -1 )");

end_test;
