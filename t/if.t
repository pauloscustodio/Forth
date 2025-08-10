#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test IF";
note "Test ELSE";
note "Test THEN";
forth_ok(": x IF 1        THEN ;  0 x .S", "( )");
forth_ok(": x IF 1        THEN ; -1 x .S", "( 1 )");
forth_ok(": x IF 1 ELSE 2 THEN ;  0 x .S", "( 2 )");
forth_ok(": x IF 1 ELSE 2 THEN ; -1 x .S", "( 1 )");

end_test;
