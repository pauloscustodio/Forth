#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DUMP";
forth_ok('S" The quick brown fox jumps over the lazy dog." DUMP', <<'END');

00000620                                   54 68 65 20 71              The q
00000630  75 69 63 6b 20 62 72 6f 77 6e 20 66 6f 78 20 6a   uick brown fox j
00000640  75 6d 70 73 20 6f 76 65 72 20 74 68 65 20 6c 61   umps over the la
00000650  7a 79 20 64 6f 67 2e                              zy dog.         
END

forth_ok('S" hello world" PAD 1+ SWAP MOVE PAD 1+ 11 DUMP', <<'END');

00000400                 68 65 6c 6c 6f 20 77 6f 72 6c 64        hello world
END

forth_ok('256 BUFFER: buf : init 256 0 DO I buf I + C! LOOP ; init buf 256 DUMP', <<'END');

000061b0                                      00 01 02 03               ....
000061c0  04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13   ................
000061d0  14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23   ............ !"#
000061e0  24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33   $%&'()*+,-./0123
000061f0  34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 40 41 42 43   456789:;<=>?@ABC
00006200  44 45 46 47 48 49 4a 4b 4c 4d 4e 4f 50 51 52 53   DEFGHIJKLMNOPQRS
00006210  54 55 56 57 58 59 5a 5b 5c 5d 5e 5f 60 61 62 63   TUVWXYZ[\]^_`abc
00006220  64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70 71 72 73   defghijklmnopqrs
00006230  74 75 76 77 78 79 7a 7b 7c 7d 7e 7f 80 81 82 83   tuvwxyz{|}~.....
00006240  84 85 86 87 88 89 8a 8b 8c 8d 8e 8f 90 91 92 93   ................
00006250  94 95 96 97 98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3   ................
00006260  a4 a5 a6 a7 a8 a9 aa ab ac ad ae af b0 b1 b2 b3   ................
00006270  b4 b5 b6 b7 b8 b9 ba bb bc bd be bf c0 c1 c2 c3   ................
00006280  c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf d0 d1 d2 d3   ................
00006290  d4 d5 d6 d7 d8 d9 da db dc dd de df e0 e1 e2 e3   ................
000062a0  e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef f0 f1 f2 f3   ................
000062b0  f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff               ............    
END

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
   25024 ( 22996) :  (LITERAL) 3 
   25032 ( 20576) :  DUP 
   25036 ( 20276) :  0> 
   25040 ( 23336) :  0BRANCH 25068 
   25048 ( 20576) :  DUP 
   25052 ( 22356) :  . 
   25056 ( 19616) :  1- 
   25060 ( 23316) :  BRANCH 25032 
   25068 ( 20536) :  DROP 
   25072 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
   25024 ( 22996) :  (LITERAL) 3 
   25032 ( 21696) :  (.") hello "
   25040 ( 19616) :  1- 
   25044 ( 20576) :  DUP 
   25048 ( 20296) :  0<= 
   25052 ( 23336) :  0BRANCH 25068 
   25060 ( 20536) :  DROP 
   25064 ( 22576) :  EXIT 
   25068 ( 23316) :  BRANCH 25032 
   25076 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
   25024 ( 23336) :  0BRANCH 25048 
   25032 ( 22996) :  (LITERAL) 1 
   25040 ( 23316) :  BRANCH 25056 
   25048 ( 22996) :  (LITERAL) 2 
   25056 ( 22356) :  . 
   25060 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
   25024 ( 22996) :  (LITERAL) 123 
   25032 ( 22356) :  . 
   25036 ( 24436) :  (2LITERAL) 123 
   25048 ( 22396) :  D. 
   25052 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
   25024 ( 21756) :  (S") hello"
   25032 ( 22116) :  TYPE 
   25036 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
   25024 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
   25024 ( 21796) :  (C") hello"
   25032 ( 21656) :  COUNT 
   25036 ( 22116) :  TYPE 
   25040 ( 22696) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
   25024 ( 23956) :  (ABORT") hello"
   25032 ( 22696) :  (SEMICOLON) 
;
END

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
