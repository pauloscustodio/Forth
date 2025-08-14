#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test COMPILE,";

# from John Hayes
forth_ok(<<'END', "( 246 )");
:NONAME DUP + ; CONSTANT DUP+
: Q DUP+ COMPILE, ; 
: AS1 [ Q ] ;
123 AS1 .S
END

end_test;
