#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test TRACE";
forth_ok("TRUE TRACE ! 1 2 + .", <<'END');
( ) 
>> 1  ( 1 ) 
>> 2  ( 1 2 ) 
>> + ( 3 ) 
>> . 3 ( ) 
END

end_test;
