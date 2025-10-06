#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test (LOCAL)";
forth_ok(<<'END', "1 2 3 ( )");
	: x ( a b c -- )
		[ S" c" (LOCAL) ]
		[ S" b" (LOCAL) ]
		[ S" a" (LOCAL) ]
		[ S" " (LOCAL) ]
		a . b . c .
	;

	1 2 3 x .S
END

forth_nok(<<'END', "\nError: interpreting a compile-only word: a\n");
	: x [ S" a" (LOCAL) ] [ a ] LITERAL ;
END

forth_ok(<<'END', "( 4 )");
	: x 
		CREATE , 
	DOES>
		[ S" body" (LOCAL) ]
		[ S" " (LOCAL) ]
		body @ 2* 
	;
	2 x a
	a .S
END

forth_ok(<<'END', "2 4 6 ( )");
	: x ( a b c -- )
		[ S" c" (LOCAL) ]
		[ S" b" (LOCAL) ]
		[ S" a" (LOCAL) ]
		[ S" " (LOCAL) ]
		a 2* TO a
		b 2* TO b
		c 2* TO c
		a . b . c .
	;

	1 2 3 x .S
END

forth_nok(<<'END', "\nError: interpreting a compile-only word: a\n");
	: x [ S" a" (LOCAL) ] [ 3 TO a ] ;
END

forth_nok(<<'END', "\nError: duplicate definition: a\n");
	: x [ S" a" (LOCAL) ] [ S" a" (LOCAL) ] ;
END

note "Test LOCALS|";
forth_ok(<<'END', "1 2 3 ( )");
	: x ( a b c -- )
		LOCALS| c b a | a . b . c .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 ( )");
	: x ( a b c -- )
		LOCALS| c b a
		a . b . c .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 ( )");
	: x ( a b c -- )
		LOCALS| |
		ROT . SWAP . .
	;
	1 2 3 x .S
END

note "Test {:";
forth_ok(<<'END', "1 2 3 0 0 0 ( )");
	: x {: a b c | d e f -- g h i :}
		a . b . c .
		d . e . f .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 0 0 0 ( )");
	: x {: a b c | d e f -- g h i
		a . b . c .
		d . e . f .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 0 0 0 ( )");
	: x {: a b c | d e f 
		a . b . c .
		d . e . f .
	;
	1 2 3 x .S
END

forth_nok(<<'END', "\nError: undefined word: g");
	: x {: a b c | d e f -- g h i :}
		a . b . c .
		d . e . f .
		g . h . i .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 ( )");
	: x {: a b c -- g h i :}
		a . b . c .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 0 0 0 ( )");
	: x {: a b c | d e f :}
		a . b . c .
		d . e . f .
	;
	1 2 3 x .S
END

forth_ok(<<'END', "1 2 3 ( )");
	: x {: a b c :}
		a . b . c .
	;
	1 2 3 x .S
END

# Gforth locals
note "Test {";
forth_ok(<<'END', "1 2 3 0 0 0 45 ( )");
	: x { a b c | d e f -- x y z }
		a . b . c . d . e . f .
		10 0 DO
			I d + to d
		LOOP
		d .
	;
	1 2 3 x .S
END

# different types
forth_ok(<<'END', "1 2 3 4 5. 6 ( )");
	: x { w1 W: w2 D: d1 w3 F: f1 w4 }
		w1 . w2 . d1 D. w3 . f1 F. w4 .
	;
	1 2 3. 4 5e 6 x .S
END

forth_ok(<<'END', "1 2 3 4 5. 6 10 20 30 40 50. 60 ( )");
	: x { w1 W: w2 D: d1 w3 F: f1 w4 }
		w1 . w2 . d1 D. w3 . f1 F. w4 .
		10 TO w1
		20 TO w2
		30. TO d1
		40 TO w3
		50e TO f1
		60 TO w4
		w1 . w2 . d1 D. w3 . f1 F. w4 .
	;
	1 2 3. 4 5e 6 x .S
END

# nesting
forth_ok(<<'END', "10 ( )");
	: x { a b -- x }
		a b +
	;
	: y { a b -- x }
		a b x
	;
	: z { -- x }
		1 2 y ( 3 )
		3 4 y ( 7 )
		+ ( 10 ) .
	;
	z .S
END

end_test;
