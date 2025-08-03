#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test COUNT';
note 'Test TYPE';
forth_ok("2 PAD C! 'O' PAD 1 + C! 'k' PAD 2 + C! PAD COUNT TYPE", "Ok");
forth_ok("2 PAD C! 'O' PAD 1 + C! 'k' PAD 2 + C! PAD 1 + 2  TYPE", "Ok");

forth_ok(' .( '.('x' x 1019).')', 'x' x 1019);
forth_ok('.( '.('x' x 1021), 'x' x 1021);

note 'Test ."';
forth_ok('     ." "    ', "");
forth_ok(' : x ." " ; x', "");
forth_ok('     ." 123"    ', "123");
forth_ok(' : x ." 123" ; x', "123");
forth_ok(' : x ." '.('x' x 1000).'" ; x', 'x' x 1000);

note 'Test S"';
forth_ok(' : x S" "    ; x TYPE', "");
forth_ok(' : x S" 123" ; x TYPE', "123");
forth_ok(' : x S" '.('x' x 1000).'" ; x TYPE', 'x' x 1000);

note 'Test C"';
forth_ok('     C" "        COUNT TYPE', "");
forth_ok(' : x C" "    ; x COUNT TYPE', "");
forth_ok('     C" 123"     COUNT TYPE', "123");
forth_ok(' : x C" 123" ; x COUNT TYPE', "123");
forth_ok(' : x C" '.('x' x 255).'" ; x COUNT TYPE', 'x' x 255);
forth_nok(': x C" '.('x' x 256).'" ; x', "\nError: parsed string overflow: ".('x' x 256)."\n");

note 'Test .(';
forth_ok('     .( )    ', "");
forth_ok(' : x .( ) ;  ', "");
forth_ok(' : x .( ) ; x', "");

forth_ok('     .( 123)    ', "123");
forth_ok(' : x .( 123) ;  ', "123");
forth_ok(' : x .( 123) ; x', "123");

end_test;
