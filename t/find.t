#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test FIND";
note "Test COUNT";
note "Test TYPE";
forth_ok("BL WORD hello   FIND .S", "( 0 0 )");
forth_ok("BL WORD hello   FIND DROP COUNT TYPE", "hello");
forth_ok("BL WORD dup     FIND SWAP ' dup     = SWAP .S", "( -1 -1 )");
forth_ok("BL WORD literal FIND SWAP ' literal = SWAP .S", "( -1 1 )");

end_test;
