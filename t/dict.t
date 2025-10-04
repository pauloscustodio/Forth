#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test ,';
note 'Test HERE';
forth_ok('HERE 123 , @ .S', "( 123 )");
forth_ok("HERE 123 , HERE SWAP - .S", "( 4 )");

note 'Test C,';
note 'Test HERE';
note 'Test COUNT';
note 'Test TYPE';
forth_ok("HERE 2 C, 'O' C, 'K' C, COUNT TYPE", "OK");

note "Test FIND";
note "Test COUNT";
note "Test TYPE";
forth_ok("BL WORD hello   FIND .S", "( 0 0 )");
forth_ok("BL WORD hello   FIND DROP COUNT TYPE", "hello");
forth_ok("BL WORD dup     FIND SWAP ' dup     = SWAP .S", "( -1 -1 )");
forth_ok("BL WORD literal FIND SWAP ' literal = SWAP .S", "( -1 1 )");

note "Test '";
note "Test EXECUTE";
forth_nok("'", "\nError: attempt to use zero-length string as a name\n");
forth_nok("' hello", "\nError: undefined word: hello\n");
forth_ok("1 2 3 ' .S EXECUTE", "( 1 2 3 )");

note "Test [']";
note "Test EXECUTE";
forth_nok(": x ['] hello ; x", "\nError: undefined word: hello\n");
forth_ok(": x ['] .S EXECUTE ; 1 2 3 x", "( 1 2 3 )");

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

note "Test COMPILE,";

# from John Hayes
forth_ok(<<'END', "( 246 )");
:NONAME DUP + ; CONSTANT DUP+
: Q DUP+ COMPILE, ; 
: AS1 [ Q ] ;
123 AS1 .S
END

note "Test [COMPILE]";
my $func = <<END;
	: ENDIF [COMPILE] THEN ; IMMEDIATE  
	: x IF 1 . ELSE 2 . ENDIF ;
END
forth_ok("$func  0 x", "2 ");
forth_ok("$func -1 x", "1 ");

note "Test DEFER";
note "Test DEFER@";
note "Test DEFER!";
forth_nok("DEFER hello hello .S", "");
forth_ok("DEFER hello ' * ' hello DEFER!  2 3 hello .S", "( 6 )");
forth_ok("DEFER hello ' + ' hello DEFER!  2 3 hello .S", "( 5 )");
forth_ok("DEFER hello ' + ' hello DEFER!  ' hello DEFER@  ' + =  .S", "( -1 )");

note "Test ACTION-OF";
my $action1 = "DEFER hello  : action-of-hello ACTION-OF hello ;  ' * ' hello DEFER!  2 3 hello .S";
my $action2 = "ACTION-OF hello  ' * = .S";
my $action3 = "action-of-hello  ' * = .S";
forth_ok("$action1", "( 6 )");
forth_ok("$action1 $action2", "( 6 )( 6 -1 )");
forth_ok("$action1 $action2 $action3", "( 6 )( 6 -1 )( 6 -1 -1 )");

note "Test IS";
forth_ok("DEFER hello : is-hello IS hello ; ' * IS hello 2 3 hello . ' + is-hello 2 3 hello . .S",
		"6 5 ( )");

note "Test CREATE";
forth_nok("CREATE", "\nError: attempt to use zero-length string as a name\n");
forth_ok("CREATE ".("x" x 63), "");
forth_nok("CREATE ".("x" x 64), "\nError: definition name too long: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
forth_ok("CREATE x 123 , x @ .S", "( 123 )");

note "Test CONSTANT";
forth_ok("123 CONSTANT x  x  .S", "( 123 )");

note 'Test 2CONSTANT';
forth_ok("1. .S 2CONSTANT xx .S xx .S",		"( 1 0 )( )( 1 0 )");
forth_ok("-1. .S 2CONSTANT xx .S xx .S",	"( -1 -1 )( )( -1 -1 )");

note "Test VARIABLE";
forth_ok("VARIABLE x  123 x !  ' x >BODY @  .S", "( 123 )");

note 'Test 2VARIABLE';
forth_ok("2VARIABLE xx xx 2@ .S",			"( 0 0 )");
forth_ok("2VARIABLE xx 1. xx 2! xx 2@ .S",	"( 1 0 )");
forth_ok("2VARIABLE xx -1. xx 2! xx 2@ .S",	"( -1 -1 )");

note "Test ON";
note "Test OFF";
forth_ok("VARIABLE X  123 X !  X @  X ON   X @ .S", "( 123 -1 )");
forth_ok("VARIABLE X  123 X !  X @  X OFF  X @ .S", "( 123 0 )");

note "Test >BODY";
forth_ok("CREATE x 123 , ' x >BODY @ .S", "( 123 )");

note "Test :";
note "Test ;";
forth_nok(":", "\nError: attempt to use zero-length string as a name\n");
forth_ok(": ".("x" x 63)." ;", "");
forth_nok(": ".("x" x 64), "\nError: definition name too long: ".("x" x 64)."\n");
forth_nok(": x [ :", "\nError: compiler nesting\n");
forth_nok(";", "\nError: compiler nesting\n");
forth_ok(": x DUP ; 1 x .S", "( 1 1 )");

note "Test :NONAME";
forth_ok(":NONAME 1 . ; EXECUTE .S", "1 ( )");

# do not create entries with empty names
forth_ok('               C" " FIND NIP 0= .S', "( -1 )");
forth_ok(':NONAME ; DROP C" " FIND NIP 0= .S', "( -1 )");

note "Test ALIGN";
forth_ok("HERE ALIGN HERE SWAP - .S", "( 0 )");
forth_ok("1 C, HERE ALIGN HERE SWAP - .S", "( 3 )");

note "Test ALIGNED";
forth_ok("0 ALIGNED .S", "( 0 )");
forth_ok("1 ALIGNED .S", "( 4 )");
forth_ok("2 ALIGNED .S", "( 4 )");
forth_ok("3 ALIGNED .S", "( 4 )");
forth_ok("4 ALIGNED .S", "( 4 )");
forth_ok("5 ALIGNED .S", "( 8 )");

note "Test ALLOT";
forth_ok("HERE  1 ALLOT HERE SWAP - .S", "( 1 )");
forth_ok("HERE -1 ALLOT HERE SWAP - .S", "( -1 )");

note "Test UNUSED";
forth_ok("UNUSED 1 , UNUSED - .S", "( 4 )");

note "Test LATEST";
forth_ok(<<END, "x( -1 )");
		HERE
		: x 1 2 3 4 5 6 7 8 9 10 ;
		LATEST = 
		LATEST 2 CELLS + @ COUNT TYPE
		.S
END

note "Test MARKER";
forth_ok(<<END, "( -1 -1 )");
		HERE
		MARKER mark
		: x 1 2 3 4 5 6 7 8 9 10 ;
		: y 1 2 3 4 5 6 7 8 9 10 ;
		: z 1 2 3 4 5 6 7 8 9 10 ;
		DUP HERE <> SWAP
		mark
		HERE =
		.S
END

note "Test BUFFER:";
forth_ok("127 BUFFER: buf1  127 BUFFER: buf2  buf1 buf2 - ABS 127 > .S", "( -1 )");

note "Test ORDER";
my $forth = <<'END';
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0
Definitions: 0
END

note "Test ALSO";
my $forth = <<'END';
	ALSO ORDER
END
forth_ok($forth, <<'END');

Search order: 0 0 
Definitions: 0
END

note "Test WORDLIST";
note "Test GET-ORDER";
note "Test SET-ORDER";
$forth = <<'END';
	WORDLIST CONSTANT wid
	GET-ORDER .S SET-ORDER
	ALSO GET-ORDER .S 
	NIP wid SWAP .S SET-ORDER
	ORDER
END
forth_ok($forth, <<'END');
( 0 1 ) ( 0 0 2 ) ( 0 1 2 )
Search order: 0 1 
Definitions: 0
END

note "Test DEFINITIONS";
$forth = <<'END';
	WORDLIST CONSTANT wid
	ALSO GET-ORDER NIP wid SWAP SET-ORDER
	DEFINITIONS
	: x ;
	WORDS
END
forth_ok($forth, <<'END');
x
END

note "Test ONLY";
$forth = <<'END';
	WORDLIST CONSTANT wid
	0 1 2 SET-ORDER
	ORDER
	ONLY
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0 1 
Definitions: 0

Search order: 0 
Definitions: 0
END

note "Test SET-ORDER";
$forth = <<'END';
	WORDLIST CONSTANT wid1
	WORDLIST CONSTANT wid2
	WORDLIST CONSTANT wid3
	wid3 wid2 wid1 0 4 SET-ORDER
	ORDER
END
forth_ok($forth, <<'END');

Search order: 3 2 1 0
Definitions: 0
END

$forth = <<'END';
	WORDLIST CONSTANT wid1
	WORDLIST CONSTANT wid2
	WORDLIST CONSTANT wid3
	wid3 wid2 wid1 0 4 SET-ORDER
	0 SET-ORDER
	ORDER
END
forth_ok($forth, <<'END');

Search order: 
Definitions: 0
END

$forth = <<'END';
	WORDLIST CONSTANT wid1
	WORDLIST CONSTANT wid2
	WORDLIST CONSTANT wid3
	wid3 wid2 wid1 0 4 SET-ORDER
	-1 SET-ORDER
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0
Definitions: 0
END

note "Test FORTH-WORDLIST";
forth_ok("FORTH-WORDLIST .S", "( 0 )");

note "Test FORTH";
$forth = <<'END';
	0 SET-ORDER
	FORTH
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0
Definitions: 0
END

$forth = <<'END';
	0 SET-ORDER
	FORTH
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0
Definitions: 0
END

$forth = <<'END';
	FORTH
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0
Definitions: 0
END

$forth = <<'END';
	WORDLIST CONSTANT wid1
	0 wid1 2 SET-ORDER
	FORTH
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0 0
Definitions: 0
END

note "Test PREVIOUS";
$forth = <<'END';
	WORDLIST CONSTANT wid1
	0 wid1 2 SET-ORDER
	PREVIOUS
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0 
Definitions: 0
END

note "Test SEARCH-WORDLIST";
$forth = <<'END';
	0 VALUE xtX
	WORDLIST CONSTANT wid
	ALSO GET-ORDER NIP wid SWAP SET-ORDER
	DEFINITIONS
	: x ;
	' X TO xtX
	ONLY FORTH
	S" x" wid SEARCH-WORDLIST 
	SWAP xtX = SWAP .S
END
forth_ok($forth, <<'END');
( -1 -1 )
END

$forth = <<'END';
	WORDLIST CONSTANT wid
	ALSO GET-ORDER NIP wid SWAP SET-ORDER
	DEFINITIONS
	: x ;
	ONLY FORTH
	S" x" 0 SEARCH-WORDLIST 
	.S
END
forth_ok($forth, <<'END');
( 0 )
END

note "Test GET-CURRENT";
forth_ok("GET-CURRENT .S", "( 0 )");

$forth = <<'END';
	WORDLIST CONSTANT wid
	ALSO GET-ORDER NIP wid SWAP SET-ORDER
	DEFINITIONS
	GET-CURRENT .S
END
forth_ok($forth, <<'END');
( 1 )
END

note "Test SET-CURRENT";
$forth = <<'END';
	WORDLIST CONSTANT wid
	0 SET-CURRENT : x ;
	ORDER
	1 SET-CURRENT : y ;
	ORDER
END
forth_ok($forth, <<'END');

Search order: 0 
Definitions: 0

Search order: 0 
Definitions: 1
END

note "Test SET-CURRENT";
$forth = <<'END';
	0 VALUE xtX
	WORDLIST CONSTANT wid
	0 SET-CURRENT : x ;
	LATEST NAME>INTERPRET TO xtX
	S" x" 0 SEARCH-WORDLIST
	SWAP xtX = SWAP .S
END
forth_ok($forth, <<'END');
( -1 -1 )
END

$forth = <<'END';
	WORDLIST CONSTANT wid
	0 SET-CURRENT : x ;
	S" x" 1 SEARCH-WORDLIST
	.S
END
forth_ok($forth, <<'END');
( 0 )
END

$forth = <<'END';
	WORDLIST CONSTANT wid
	1 SET-CURRENT : x ;
	S" x" 0 SEARCH-WORDLIST
	.S
END
forth_ok($forth, <<'END');
( 0 )
END

$forth = <<'END';
	0 VALUE xtX
	WORDLIST CONSTANT wid
	1 SET-CURRENT : x ;
	LATEST NAME>INTERPRET TO xtX
	S" x" 1 SEARCH-WORDLIST
	SWAP xtX = SWAP .S
END
forth_ok($forth, <<'END');
( -1 -1 )
END



end_test;
