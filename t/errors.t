#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# usage
capture_nok("./forth", "usage: forth file.fs args...\n");

note "Test THROW";
forth_ok("1 0 THROW .", "1 ");
forth_nok("-3 THROW not reached", "\nError: stack overflow\n");
forth_nok("-4 THROW not reached", "\nError: stack underflow\n");
forth_nok("-5 THROW not reached", "\nError: return stack overflow\n");
forth_nok("-6 THROW not reached", "\nError: return stack underflow\n");
forth_nok("-8 THROW not reached", "\nError: dictionary overflow\n");
forth_nok("-9 THROW not reached", "\nError: invalid memory address\n");
forth_nok("-13 THROW not reached", "\nError: undefined word\n");
forth_nok("-23 THROW not reached", "\nError: address alignment exception\n");
forth_nok("-38 THROW not reached", "\nError: non-existent file\n");

end_test;
