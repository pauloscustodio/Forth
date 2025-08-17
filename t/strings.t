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

000f36e0  68 65 6c 6c 6f 20 07 20 08 20 1b 20 0c 20 0a 20   hello . . . . . 
000f36f0  0d 0a 20 0a 20 22 20 0d 20 09 20 0b 20 00 20 22   .. . " . . . . "
000f3700  20 ff 20 5c                                        . \            
END

note "Test FILL";
forth_ok("PAD 10 BL FILL PAD 5 'x' FILL PAD 10 TYPE", "xxxxx     ");

note "Test ERASE";
forth_ok(<<END, "0 0 0 0 0 32 32 32 32 32 ");
		: dump 10 0 DO PAD I + C@ . LOOP ;
		PAD 10 BL FILL 
		PAD 5 ERASE 
		dump
END

note "Test MOVE";
forth_ok("BL WORD hello COUNT PAD SWAP MOVE PAD 5 TYPE", "hello");

end_test;
