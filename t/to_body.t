#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test >BODY";
forth_ok("CREATE x 123 , ' x >BODY @ .S", "( 123 )");

end_test;
