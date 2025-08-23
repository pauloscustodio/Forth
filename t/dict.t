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
		LATEST CELL+ @ COUNT TYPE
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

end_test;
