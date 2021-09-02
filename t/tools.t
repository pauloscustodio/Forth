#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test .S";
forth_ok("     .S", "( )");
forth_ok("1 -1 .S", "( 1 -1 )");

note "Test ~~";
forth_ok("~~ : x 5 0 ~~ DO ~~ I . ~~ LOOP ~~ ; ~~", <<END);
( )
( )
( 5048 5052 )
( 5048 5052 )
( )
( )
END

note "Test .RS";
forth_ok(".RS", "(R: )");
forth_ok("1 >R 2 >R .RS BYE", "(R: 1 2 )");

end_test;
