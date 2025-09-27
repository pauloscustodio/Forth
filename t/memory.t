#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test !';
note 'Test @';
forth_ok('10 PAD !  PAD @ .S  11 PAD ! PAD @ .S', "( 10 )( 10 11 )");

note 'Test +!';
forth_ok('10 PAD !  2 PAD +!  PAD @ .S', "( 12 )");

note 'Test C!';
note 'Test C@';
forth_ok('10 PAD C!  PAD C@ .S  11 PAD C! PAD C@ .S', "( 10 )( 10 11 )");

note 'Test 2!';
note 'Test 2@';
forth_ok('1234567890123. PAD 2!  PAD 2@ .S', "( 1912276171 287 )");

note "Test ALLOCATE";
note "Test RESIZE";
note "Test FREE";
forth_ok(<<'END', "1058592 Hello 1058704 Hello 1058916 Hello 1058592 Hello 1059228 Hello 1059740 Hello 1058704 ( )");
	100 ALLOCATE THROW VALUE mem1
	mem1 100 BL FILL
	mem1 .
	S" Hello" mem1 SWAP MOVE
	mem1 5 TYPE SPACE
	
	200 ALLOCATE THROW VALUE mem2
	mem2 200 BL FILL
	mem2 .
	S" Hello" mem2 SWAP MOVE
	mem2 5 TYPE SPACE
	
	300 ALLOCATE THROW VALUE mem3
	mem3 300 BL FILL
	mem3 .
	S" Hello" mem3 SWAP MOVE
	mem3 5 TYPE SPACE
	
	mem1 40 RESIZE THROW TO mem1
	mem1 .
	mem1 5 TYPE SPACE
	
	mem2 500 RESIZE THROW TO mem2
	mem2 .
	mem2 5 TYPE SPACE
	
	mem3 600 RESIZE THROW TO mem3
	mem3 .
	mem3 5 TYPE SPACE
	
	0 200 RESIZE THROW VALUE mem4
	mem4 .
	
	mem1 FREE THROW
	mem2 FREE THROW
	mem3 FREE THROW
	.S
END

forth_nok("-1 ALLOCATE THROW", "\nError: ALLOCATE exception");
forth_nok("0      FREE THROW", "\nError: FREE exception");

forth_ok(<<'END', "1058592 Hello 1058592 Hello ( 0 )");
	100 ALLOCATE THROW VALUE mem1
	mem1 100 BL FILL
	mem1 .
	S" Hello" mem1 SWAP MOVE
	mem1 5 TYPE SPACE
	
	mem1 -1 RESIZE 0= SWAP TO mem1
	mem1 .
	mem1 5 TYPE SPACE
	.S
END

end_test;
