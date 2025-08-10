#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test ABORT";
forth_nok("ABORT", "");

note 'Test ABORT"';
forth_ok(' : x ABORT" error" ." ok" ;  0 x', "ok");
forth_nok(': x ABORT" error" ." ok" ; -1 x', "\nAborted: error\n");

end_test;
