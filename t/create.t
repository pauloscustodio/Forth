#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CREATE";
forth_nok("CREATE", "\nError: Attempt to use zero length string as name\n");
forth_ok("CREATE ".("x" x 63), "");
forth_nok("CREATE ".("x" x 64), "\nError: Name too long: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
forth_ok("CREATE x 123 , x @ .S", "( 123 )");

end_test;
