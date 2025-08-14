#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test :";
note "Test ;";
forth_nok(":", "\nError: Attempt to use zero length string as name\n");
forth_nok(": ".("x" x 64), "\nError: Name too long: ".("x" x 64)."\n");
forth_nok(": x [ :", "\nError: Compiler nesting\n");
forth_nok(";", "\nError: Compiler nesting\n");
forth_ok(": x DUP ; 1 x .S", "( 1 1 )");

note "Test :NONAME";
forth_ok(":NONAME 1 . ; EXECUTE .S", "1 ( )");

# do not create entries with empty names
forth_ok('               C" " FIND NIP 0= .S', "( -1 )");
forth_ok(':NONAME ; DROP C" " FIND NIP 0= .S', "( -1 )");

end_test;
