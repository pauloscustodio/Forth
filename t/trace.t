#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test TRACE";
forth_ok("TRUE TRACE ! : x DUP + ; 1 x .", <<END);
 ( ) 
>> :  ( ) 
>> ;  ( ) 
>> 1  ( 1 ) 
>> x  ( 1 ) 
>>>> DUP  ( 1 1 ) 
>>>> +  ( 2 ) 
>>>> EXIT  ( 2 ) 
>> . 2  ( ) 
END

end_test;
