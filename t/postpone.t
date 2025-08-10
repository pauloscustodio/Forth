#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test POSTPONE";
forth_ok(<<'END', <<'END');
	: x 123 ;						.S CR
	: y POSTPONE x ; IMMEDIATE		.S CR
	: z y ;							.S CR
	z . 							.S CR
END
( ) 
( ) 
( ) 
123 ( ) 
END

forth_ok(<<'END', "0 ( )");
	: skip POSTPONE \ ;
	skip : x 123 ;
	BL WORD x FIND NIP . \ 0
	.S
END

end_test;
