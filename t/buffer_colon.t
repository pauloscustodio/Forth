#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test BUFFER:";
forth_ok("127 BUFFER: buf1  127 BUFFER: buf2  buf1 buf2 - ABS 127 > .S", "( -1 )");

end_test;
