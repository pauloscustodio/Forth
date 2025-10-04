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
forth_ok(<<'END', "( ) ( 10 20 30 40 4 )");
	: x
		10 20 30 40 4 N>R .S
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

note "Test >NAME";
forth_ok(<<'END', "-1 -1 ( )");
	0 VALUE ntX
	HERE TO ntX
	: X ;
	LATEST ntX = .
	' X >NAME ntX = .
	.S
END

note "Test NAME>COMPILE";
forth_ok(<<'END', "( 2 )");
	: X 2 ; IMMEDIATE
	LATEST NAME>COMPILE EXECUTE
	.S
END

forth_ok(<<'END', "( 2 )");
	: X 2 ;
	LATEST NAME>COMPILE 
	: Y [ EXECUTE ] ;
	Y .S
END

note "Test NAME>STRING";
forth_ok(": x ; LATEST NAME>STRING TYPE .S", "x( )");

note "Test NAME>INTERPRET";
forth_ok(": x ; LATEST NAME>INTERPRET ' x = . .S", "-1 ( )");

note "Test SYNONYM";
forth_ok(<<'END', "1 ( )");
	SYNONYM ENDIF     THEN 
	SYNONYM PRINT_NUM . 
	: x ?DUP IF PRINT_NUM ENDIF ;
	0 x
	1 x
	.S
END

note "Test TRAVERSE-WORDLIST";
my @words = split(' ', `forth -e WORDS`);
forth_ok(<<'END', join(" ", @words)." ( )");
	:NONAME ( nt - )
		NAME>STRING TYPE SPACE
		TRUE
	;
	0 ( xt wid ) TRAVERSE-WORDLIST .S
END

forth_ok(<<'END', join(" ", @words[0..9])." ( )");
	0 PAD !
	:NONAME ( nt - )
		NAME>STRING TYPE SPACE
		1 PAD +!
		PAD @ 10 <=
	;
	0 ( xt wid ) TRAVERSE-WORDLIST .S
END

note "Test [DEFINED]";
forth_ok(<<'END', "-1 0 ( )");
	: x ;
	: y [DEFINED] x LITERAL ;
	: z [DEFINED] z LITERAL ;
	y . z . .S
END

note "Test [UNDEFINED]";
forth_ok(<<'END', "0 -1 ( )");
	: x ;
	: y [UNDEFINED] x LITERAL ;
	: z [UNDEFINED] z LITERAL ;
	y . z . .S
END

note "Test [IF]";
note "Test [ELSE]";
note "Test [THEN]";
forth_ok(<<'END', "111 ( )");
	: x [ TRUE ] [IF] 111 [ELSE] 222 [THEN] ;
	x . .S
END

forth_ok(<<'END', "222 ( )");
	: x [ FALSE ] [IF] 111 [ELSE] 222 [THEN] ;
	x . .S
END

forth_ok(<<'END', "333 ( )");
	: x [ FALSE ] [IF] 
			[ TRUE ] [IF] 111 [ELSE] 222 [THEN] 
		[ELSE]
			[ TRUE ] [IF] 333 [ELSE] 444 [THEN] 
		[THEN] ;
	x . .S
END

forth_nok(<<'END', "\nError: unmatched conditional compilation\n");
	: x [ FALSE ] [IF] 
			[ TRUE ] [IF] 111 [ELSE] 222 [THEN] 
		[ELSE]
			[ TRUE ] [IF] 333 [ELSE] 444 [THEN] 
	;
END

forth_ok("TRUE  [IF] 1 TRUE  [IF] 2 [ELSE] 3 [THEN] [ELSE] 4 [THEN] .S", "( 1 2 )");

forth_ok("1 [ELSE] 2 [THEN] 3 .S", "( 1 3 )");

end_test;
