#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test EVALUATE";
forth_ok("SOURCE-ID . '!' WORD SOURCE-ID .! COUNT EVALUATE .S", 
		 "1 -1 ( )");

end_test;
