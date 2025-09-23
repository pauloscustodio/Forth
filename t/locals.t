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

end_test;
