#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test COUNT';
note 'Test TYPE';
forth_ok("HERE 2 C, 'O' C, 'k' C,  COUNT TYPE", "Ok");

note 'Test ."';
forth_ok('     ." "    ', "");
forth_ok(' : x ." " ; x', "");
forth_ok('     ." 123"    ', "123");
forth_ok(' : x ." 123" ; x', "123");
forth_ok(' : x ." '.('x' x 1000).'" ; x', 'x' x 1000);

note 'Test S"';
forth_ok('     S" "        TYPE', "");
forth_ok(' : x S" "    ; x TYPE', "");
forth_ok('     S" 123"     TYPE', "123");
forth_ok(' : x S" 123" ; x TYPE', "123");
forth_ok(' : x S" '.('x' x 1000).'" ; x TYPE', 'x' x 1000);
forth_ok(' S" one" S" two" S" three" S" four" TYPE TYPE TYPE TYPE .S',
		 "fourthreetwoone( )");

note 'Test C"';
forth_ok('     C" "        COUNT TYPE', "");
forth_ok(' : x C" "    ; x COUNT TYPE', "");
forth_ok('     C" 123"     COUNT TYPE', "123");
forth_ok(' : x C" 123" ; x COUNT TYPE', "123");
forth_ok(' : x C" '.('x' x 255).'" ; x COUNT TYPE', 'x' x 255);
forth_nok(': x C" '.('x' x 256).'" ; x', 
		  "\nError: parsed string overflow: ".('x' x 256)."\n");

note 'Test .(';
forth_ok('     .( )    ', "");
forth_ok(' : x .( ) ;  ', "");
forth_ok(' : x .( ) ; x', "");

forth_ok('     .( 123)    ', "123");
forth_ok(' : x .( 123) ;  ', "123");
forth_ok(' : x .( 123) ; x', "123");

forth_ok(' .( '.('x' x 1000).')', 'x' x 1000);

note 'Test S\\"';
forth_ok(<<'END', <<'END');
S\" hello \a \b \e \f \l \m \n \q \r \t \v \z \" \xff \\" DUMP
END

00000000              68 65 6c 6c 6f 20 07 20 08 20 1b 20       hello . . . 
00000010  0c 20 0a 20 0d 0a 20 0a 20 22 20 0d 20 09 20 0b   . . .. . " . . .
00000020  20 00 20 22 20 ff 20 5c                            . " . \        
END

forth_ok(<<'END', <<'END');
: x S\" hello \a \b \e \f \l \m \n \q \r \t \v \z \" \xff \\" ; 
x 2DUP PAD SWAP MOVE
SWAP DROP PAD SWAP DUMP
END

00000800  68 65 6c 6c 6f 20 07 20 08 20 1b 20 0c 20 0a 20   hello . . . . . 
00000810  0d 0a 20 0a 20 22 20 0d 20 09 20 0b 20 00 20 22   .. . " . . . . "
00000820  20 ff 20 5c                                        . \            
END

note "Test FILL";
forth_ok("PAD 10 BL FILL PAD  5 'x' FILL PAD 10 TYPE", "xxxxx     ");
forth_ok("PAD 10 BL FILL PAD  0 'x' FILL PAD 10 TYPE", "          ");
forth_ok("PAD 10 BL FILL PAD -1 'x' FILL PAD 10 TYPE", "          ");

note "Test ERASE";
forth_ok(<<END, "0 0 0 0 0 32 32 32 32 32 ");
		: dump 10 0 DO PAD I + C@ . LOOP ;
		PAD 10 BL FILL 
		PAD 5 ERASE 
		dump
END

forth_ok(<<END, "32 32 32 32 32 32 32 32 32 32 ");
		: dump 10 0 DO PAD I + C@ . LOOP ;
		PAD 10 BL FILL 
		PAD 0 ERASE 
		dump
END

forth_ok(<<END, "32 32 32 32 32 32 32 32 32 32 ");
		: dump 10 0 DO PAD I + C@ . LOOP ;
		PAD 10 BL FILL 
		PAD -1 ERASE 
		dump
END

note "Test MOVE";
forth_ok("BL WORD hello COUNT PAD SWAP MOVE PAD 5 TYPE", "hello");

note "Test -TRAILING";
forth_ok('S" " -TRAILING NIP .S', "( 0 )");
forth_ok('S"  " -TRAILING NIP .S', "( 0 )");
forth_ok('S"   " -TRAILING NIP .S', "( 0 )");
forth_ok('S" a " -TRAILING NIP .S', "( 1 )");
forth_ok('S"  a " -TRAILING NIP .S', "( 2 )");
forth_ok('S"   a " -TRAILING NIP .S', "( 3 )");
forth_ok('S" a" -TRAILING TYPE .S', "a( )");
forth_ok('S"  a" -TRAILING TYPE .S', " a( )");
forth_ok('S"   a" -TRAILING TYPE .S', "  a( )");
forth_ok('S"   a " -TRAILING TYPE .S', "  a( )");
forth_ok('S"   a  " -TRAILING TYPE .S', "  a( )");
forth_ok('S"   a   " -TRAILING TYPE .S', "  a( )");

note "Test /STRING";
forth_ok('S" ABC" 2 /STRING TYPE .S', "C ( )");
forth_ok('S" ABC" 2 /STRING -1 /STRING TYPE .S', "BC ( )");

note "Test BLANK";
forth_ok("PAD 10 'x' FILL PAD  5 BLANK PAD 10 TYPE", "     xxxxx");
forth_ok("PAD 10 'x' FILL PAD  1 BLANK PAD 10 TYPE", " xxxxxxxxx");
forth_ok("PAD 10 'x' FILL PAD  0 BLANK PAD 10 TYPE", "xxxxxxxxxx");
forth_ok("PAD 10 'x' FILL PAD -1 BLANK PAD 10 TYPE", "xxxxxxxxxx");

note "Test CMOVE";
forth_ok('PAD 10 BLANK S" hello" DROP PAD  5 CMOVE PAD 10 TYPE', "hello     ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD  3 CMOVE PAD 10 TYPE', "hel       ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD  0 CMOVE PAD 10 TYPE', "          ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD -1 CMOVE PAD 10 TYPE', "          ");

forth_ok('S" ABCD" PAD SWAP CMOVE PAD PAD 1+ 3 CMOVE PAD 4 TYPE', "AAAA");

note "Test CMOVE>";
forth_ok('PAD 10 BLANK S" hello" DROP PAD  5 CMOVE> PAD 10 TYPE', "hello     ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD  3 CMOVE> PAD 10 TYPE', "hel       ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD  0 CMOVE> PAD 10 TYPE', "          ");
forth_ok('PAD 10 BLANK S" hello" DROP PAD -1 CMOVE> PAD 10 TYPE', "          ");

forth_ok('S" ABCD" PAD SWAP CMOVE> PAD 1+ PAD 3 CMOVE> PAD 4 TYPE', "DDDD");

note "Test COMPARE";
forth_ok('S" ABC"  S" ABC"  COMPARE  0 = .S', '( -1 )');
forth_ok('S" 123"  S" 123"  COMPARE  0 = .S', '( -1 )');

forth_ok('S" ABC"  S" ABD"  COMPARE -1 = .S', '( -1 )');
forth_ok('S" ABD"  S" ABC"  COMPARE  1 = .S', '( -1 )');
forth_ok('S" ABC"  S" AB"   COMPARE  1 = .S', '( -1 )');
forth_ok('S" AB"   S" ABC"  COMPARE -1 = .S', '( -1 )');

forth_ok('S" "     S" "     COMPARE  0 = .S', '( -1 )');
forth_ok('S" ABC"  S" "     COMPARE  1 = .S', '( -1 )');
forth_ok('S" "     S" ABC"  COMPARE -1 = .S', '( -1 )');

forth_ok('S" ABC " S" ABC"  COMPARE  1 = .S', '( -1 )');
forth_ok('S" ABC"  S" ABC " COMPARE -1 = .S', '( -1 )');
forth_ok('S" A@C"  S" ABC"  COMPARE -1 = .S', '( -1 )');

note "Test SEARCH";
# Basic Match Cases
forth_ok('S" ABCDEF" S" ABC" SEARCH [IF] TYPE [THEN]', 'ABCDEF');
forth_ok('S" ABCDEF" S" CDE" SEARCH [IF] TYPE [THEN]', 'CDEF');
forth_ok('S" ABCDEF" S" DEF" SEARCH [IF] TYPE [THEN]', 'DEF');
forth_ok('S" ABCDEF" S" BCD" SEARCH [IF] TYPE [THEN]', 'BCDEF');

# No Match Cases
forth_ok('S" ABCDEF" S" XYZ" SEARCH 0= [IF] TYPE [THEN]', 'ABCDEF');
forth_ok('S" " S" ABC" SEARCH 0= [IF] TYPE [THEN]', '');

# Edge cases with empty strings
forth_ok('S" " S" " SEARCH [IF] TYPE [THEN]', '');
forth_ok('S" ABCDEF" S" " SEARCH [IF] TYPE [THEN]', 'ABCDEF');

forth_ok('S" A" S" A" SEARCH [IF] TYPE [THEN]', 'A');
forth_ok('S" A" S" " SEARCH [IF] TYPE [THEN]', 'A');

# Overlapping and repeated patterns
forth_ok('S" ABABAB" S" BAB" SEARCH [IF] TYPE [THEN]', 'BABAB');
forth_ok('S" ABABAB" S" ABA" SEARCH [IF] TYPE [THEN]', 'ABABAB');
forth_ok('S" ABABAB" S" ABAB" SEARCH [IF] TYPE [THEN]', 'ABABAB');

# Whitespace and special characters
forth_ok('S" ABC " S" ABC" SEARCH [IF] TYPE [THEN]', 'ABC ');
forth_ok('S" ABC" S" ABC " SEARCH 0= [IF] TYPE [THEN]', 'ABC');
forth_ok('S" A@C" S" @C" SEARCH [IF] TYPE [THEN]', '@C');
forth_ok('S" A B C" S" B" SEARCH [IF] TYPE [THEN]', 'B C');

note "Test SLITERAL";
forth_ok(': x [ S" hello" ] SLITERAL ; x TYPE', 'hello');

note "Test REPLACES";
note "Test SUBSTITUTE";
forth_ok(<<'END', "2 Paulo has a score of 100 %( )");
	100 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" Paulo" S" NAME"  REPLACES
	S" 100"   S" SCORE" REPLACES
	
	S" %NAME% has a score of %score% %%" BUFFER BUFFER_SZ SUBSTITUTE
	. TYPE .S
END

forth_ok(<<'END', "2 Paulo has a score of 100 %( )");
	26 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" Paulo" S" NAME"  REPLACES
	S" 100"   S" SCORE" REPLACES
	
	S" %NAME% has a score of %score% %%" BUFFER BUFFER_SZ SUBSTITUTE
	. TYPE .S
END

forth_ok(<<'END', "-78 ( )");
	25 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" Paulo" S" NAME"  REPLACES
	S" 100"   S" SCORE" REPLACES
	
	S" %NAME% has a score of %SCORE% %%" BUFFER BUFFER_SZ SUBSTITUTE
	NIP NIP . .S
END

forth_ok(<<'END', "2 Paulo %SURNAME% Paulo %SURNAME%( )");
	100 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" Paulo" S" NAME"  REPLACES
	
	S" %NAME% %SURNAME% %NAME% %SURNAME%" BUFFER BUFFER_SZ SUBSTITUTE
	. TYPE .S
END

forth_ok(<<'END', "2  %SURNAME%  %SURNAME%( )");
	100 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" " S" NAME"  REPLACES
	
	S" %NAME% %SURNAME% %NAME% %SURNAME%" BUFFER BUFFER_SZ SUBSTITUTE
	. TYPE .S
END

note "Test UNESCAPE";
forth_ok(<<'END', "%%hello%%%%world%% %%hello( )");
	100 CONSTANT BUFFER_SZ
	BUFFER_SZ BUFFER: BUFFER
	
	S" %hello%%world%" BUFFER UNESCAPE 
	TYPE SPACE 
	BUFFER 7 TYPE 
	.S
END


end_test;
