#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test SOURCE";
forth_ok("SOURCE TYPE", "SOURCE TYPE");

# from John Hayes test suite
forth_ok(<<'END', "( -1 -1 )");
: GS1 S" SOURCE" 2DUP EVALUATE
       >R SWAP >R = R> R> = ;
GS1 .S
END

end_test;
