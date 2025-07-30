#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test COUNT';
note 'Test TYPE';
forth_ok("2 PAD C! 'O' PAD 1 + C! 'k' PAD 2 + C! PAD COUNT TYPE", "Ok");
forth_ok("2 PAD C! 'O' PAD 1 + C! 'k' PAD 2 + C! PAD 1 + 2  TYPE", "Ok");

forth_ok(' .( '.('x' x 1019).')', 'x' x 1019);
forth_ok('.( '.('x' x 1021), 'x' x 1021);

end_test;
