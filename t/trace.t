#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test TRACE";
forth_ok("TRUE TRACE ! : x DUP + ; 1 x .", <<END);
 ( ) 
>> : (C: 0 0 )  ( ) 
>> ; (C: )  ( ) 
>> 1  ( 1 ) 
>> x  ( 1 ) 
>>>> DUP  ( 1 1 ) 
>>>> +  ( 2 ) 
>>>> (SEMICOLON)  ( 2 ) 
>> . 2  ( ) 
END

end_test;
