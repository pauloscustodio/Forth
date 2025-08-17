#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CREATE";
forth_nok("CREATE", "\nError: attempt to use zero-length string as a name\n");
forth_ok("CREATE ".("x" x 63), "");
forth_nok("CREATE ".("x" x 64), "\nError: definition name too long: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
forth_ok("CREATE x 123 , x @ .S", "( 123 )");

end_test;
