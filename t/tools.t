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
forth_ok(<<'END', "( ) ( 1 2 3 4 )");
	: x
		1 2 3 4  4 N>R .S
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
forth_ok(<<'END', "-1 -1 -1 -1 -1 ()");
	0 VALUE xtX
	0 VALUE xtEXECUTE
	0 VALUE ntX
	HERE TO ntX
	: X ; IMMEDIATE
	BL WORD X       FIND  1 = . TO xtX
	BL WORD EXECUTE FIND -1 = . TO xtEXECUTE
	LATEST ntX = .
	LATEST NAME>COMPILE
	xtEXECUTE = .
	xtX = .
	.S
END

forth_ok(<<'END', "-1 -1 -1 -1 -1 ()");
	0 VALUE xtX
	0 VALUE xtCOMPILE_COMMA
	0 VALUE ntX
	HERE TO ntX
	: X ;
	BL WORD X        FIND -1 = . TO xtX
	BL WORD COMPILE, FIND  1 = . TO xtCOMPILE_COMMA
	LATEST ntX = .
	LATEST NAME>COMPILE
	xtCOMPILE_COMMA = .
	xtX = .
	.S
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

end_test;
