#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

$ENV{FORTH} = 'TRACE ON  ." Forth ready" CR';
forth_ok("1 2 + . .S", <<'END');
 ( ) 
>> ." Forth ready ( ) 
>> CR 
 ( ) 
 ( ) 
>> QUIT 
> 1 2 + . .S
>> 1  ( 1 ) 
>> 2  ( 1 2 ) 
>> +  ( 3 ) 
>> . 3  ( ) 
>> .S ( ) ( ) 

> 
END

delete $ENV{FORTH};
forth_ok("1 2 + . .S", "3 ( ) ");
end_test;
