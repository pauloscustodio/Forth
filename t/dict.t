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

note "Test CREATE";
forth_nok("CREATE", "\nError: Attempt to use zero length string as name\n");
forth_ok("CREATE ".("x" x 63), "");
forth_nok("CREATE ".("x" x 64), "\nError: Name too long: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
forth_ok("CREATE x 123 , x @ .S", "( 123 )");

note "Test CONSTANT";
forth_ok("123 CONSTANT x  x  .S", "( 123 )");

note "Test VARIABLE";
forth_ok("VARIABLE x  123 x !  ' x >BODY @  .S", "( 123 )");

note "Test >BODY";
forth_ok("CREATE x 123 , ' x >BODY @ .S", "( 123 )");

note "Test :";
note "Test ;";
forth_nok(":", "\nError: Attempt to use zero length string as name\n");
forth_nok(": ".("x" x 64), "\nError: Name too long: ".("x" x 64)."\n");
forth_nok(": x [ :", "\nError: Compiler nesting\n");
forth_nok(";", "\nError: Compiler nesting\n");
forth_ok(": x DUP ; 1 x .S", "( 1 1 )");

note "Test :NONAME";
forth_ok(":NONAME 1 . ; EXECUTE .S", "1 ( )");

note "Test '";
note "Test EXECUTE";
forth_nok("'", "\nError: Attempt to use zero length string as name\n");
forth_nok("' hello", "\nError: Undefined word: hello\n");
forth_ok("1 2 3 ' .S EXECUTE", "( 1 2 3 )");

note "Test [']";
note "Test EXECUTE";
forth_nok(": x ['] hello ; x", "\nError: Undefined word: hello\n");
forth_ok(": x ['] .S EXECUTE ; 1 2 3 x", "( 1 2 3 )");

note "Test POSTPONE";
forth_ok(": x 1 . ; IMMEDIATE  : y POSTPONE x ;  ", "");
forth_ok(": x 1 . ; IMMEDIATE  : y POSTPONE x ; y", "1 ");

note "Test COMPILE,";
forth_ok(": x [ ' DUP ] COMPILE, ;  1 x .S", "( 1 1 )");

note "Test [COMPILE]";
my $func = <<END;
	: ENDIF [COMPILE] THEN ; IMMEDIATE  
	: x IF 1 . ELSE 2 . ENDIF ;
END
forth_ok("$func  0 x", "2 ");
forth_ok("$func -1 x", "1 ");

note "Test CHAR";
forth_ok("CHAR ! .S", "( 33 )");

note "Test [CHAR]";
forth_ok(": x [CHAR] ! ; x .S", "( 33 )");

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
forth_ok("UNUSED 1 , UNUSED - .S", "( 1 )");

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

note "Test LATEST";
forth_ok(<<END, "x( -1 )");
		HERE
		: x 1 2 3 4 5 6 7 8 9 10 ;
		LATEST = 
		LATEST CELL+ @ COUNT TYPE
		.S
END

end_test;
