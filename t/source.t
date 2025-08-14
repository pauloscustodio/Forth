#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SOURCE";
forth_ok("SOURCE TYPE", "SOURCE TYPE");

note "Test TIB";
forth_ok("TIB 11 TYPE", "TIB 11 TYPE");
forth_ok("TIB .S", "( 1576 )");

note "Test #IN";
forth_ok("#IN @ .S", "( 8 )");

note "Test #TIB";
forth_ok("#TIB @ .S", "( 9 )");

note "Test >IN";
forth_ok(">IN @ .S", "( 6 )");
forth_ok("1 2 3 100 >IN ! .S", "");

# from John Hayes test suite
forth_ok(<<'END', "( -1 -1 )");
: GS1 S" SOURCE" 2DUP EVALUATE
       >R SWAP >R = R> R> = ;
GS1 .S
END

end_test;
