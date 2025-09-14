#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test .S";
forth_ok("     .S", "( )");
forth_ok("1 -1 .S", "( 1 -1 )");

note "Test .FS";
forth_ok("     .FS", "(F: )");
forth_ok("1.1E -1.1E .FS", "(F: 1.1 -1.1 )");

note "Test .RS";
forth_ok(".RS", "(R: )");
forth_ok("1 >R 2 >R .RS BYE", "(R: 1 2 )");

end_test;
