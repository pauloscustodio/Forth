#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test VARIABLE";
forth_ok("VARIABLE x  123 x !  ' x >BODY @  .S", "( 123 )");

end_test;
