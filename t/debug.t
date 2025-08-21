#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DUMP";
forth_ok('S" The quick brown fox jumps over the lazy dog." DUMP', <<'END');

00000000              54 68 65 20 71 75 69 63 6b 20 62 72       The quick br
00000010  6f 77 6e 20 66 6f 78 20 6a 75 6d 70 73 20 6f 76   own fox jumps ov
00000020  65 72 20 74 68 65 20 6c 61 7a 79 20 64 6f 67 2e   er the lazy dog.
END

forth_ok('S" hello world" PAD 1+ SWAP MOVE PAD 1+ 11 DUMP', <<'END');

00000800                 68 65 6c 6c 6f 20 77 6f 72 6c 64        hello world
END

forth_ok('256 BUFFER: buf : init 256 0 DO I buf I + C! LOOP ; init buf 256 DUMP', <<'END');

000065e0              00 01 02 03 04 05 06 07 08 09 0a 0b       ............
000065f0  0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b   ................
00006600  1c 1d 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b   .... !"#$%&'()*+
00006610  2c 2d 2e 2f 30 31 32 33 34 35 36 37 38 39 3a 3b   ,-./0123456789:;
00006620  3c 3d 3e 3f 40 41 42 43 44 45 46 47 48 49 4a 4b   <=>?@ABCDEFGHIJK
00006630  4c 4d 4e 4f 50 51 52 53 54 55 56 57 58 59 5a 5b   LMNOPQRSTUVWXYZ[
00006640  5c 5d 5e 5f 60 61 62 63 64 65 66 67 68 69 6a 6b   \]^_`abcdefghijk
00006650  6c 6d 6e 6f 70 71 72 73 74 75 76 77 78 79 7a 7b   lmnopqrstuvwxyz{
00006660  7c 7d 7e 7f 80 81 82 83 84 85 86 87 88 89 8a 8b   |}~.............
00006670  8c 8d 8e 8f 90 91 92 93 94 95 96 97 98 99 9a 9b   ................
00006680  9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab   ................
00006690  ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb   ................
000066a0  bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb   ................
000066b0  cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db   ................
000066c0  dc dd de df e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb   ................
000066d0  ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb   ................
000066e0  fc fd fe ff                                       ....            
END

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
   26088 ( 24020) :  (LITERAL) 3 
   26096 ( 21600) :  DUP 
   26100 ( 21300) :  0> 
   26104 ( 24360) :  0BRANCH 26132 
   26112 ( 21600) :  DUP 
   26116 ( 23380) :  . 
   26120 ( 20640) :  1- 
   26124 ( 24340) :  BRANCH 26096 
   26132 ( 21560) :  DROP 
   26136 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
   26088 ( 24020) :  (LITERAL) 3 
   26096 ( 22720) :  (.") hello "
   26104 ( 20640) :  1- 
   26108 ( 21600) :  DUP 
   26112 ( 21320) :  0<= 
   26116 ( 24360) :  0BRANCH 26132 
   26124 ( 21560) :  DROP 
   26128 ( 23600) :  EXIT 
   26132 ( 24340) :  BRANCH 26096 
   26140 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
   26088 ( 24360) :  0BRANCH 26112 
   26096 ( 24020) :  (LITERAL) 1 
   26104 ( 24340) :  BRANCH 26120 
   26112 ( 24020) :  (LITERAL) 2 
   26120 ( 23380) :  . 
   26124 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
   26088 ( 24020) :  (LITERAL) 123 
   26096 ( 23380) :  . 
   26100 ( 25400) :  (2LITERAL) 123 
   26112 ( 23420) :  D. 
   26116 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
   26088 ( 22780) :  (S") hello"
   26096 ( 23140) :  TYPE 
   26100 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
   26088 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
   26088 ( 22820) :  (C") hello"
   26096 ( 22680) :  COUNT 
   26100 ( 23140) :  TYPE 
   26104 ( 23720) :  (SEMICOLON) 
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
   26088 ( 26060) :  (ABORT") hello"
   26096 ( 23720) :  (SEMICOLON) 
;
END

note "Test TRACE";
forth_ok("TRACE ON : x DUP + ; 1 x .", <<END);
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
