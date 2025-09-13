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

00007620                          00 01 02 03 04 05 06 07           ........
00007630  08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17   ................
00007640  18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27   ........ !"#$%&'
00007650  28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37   ()*+,-./01234567
00007660  38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47   89:;<=>?@ABCDEFG
00007670  48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57   HIJKLMNOPQRSTUVW
00007680  58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67   XYZ[\]^_`abcdefg
00007690  68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77   hijklmnopqrstuvw
000076a0  78 79 7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87   xyz{|}~.........
000076b0  88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97   ................
000076c0  98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7   ................
000076d0  a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7   ................
000076e0  b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7   ................
000076f0  c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7   ................
00007700  d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7   ................
00007710  e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7   ................
00007720  f8 f9 fa fb fc fd fe ff                           ........        
END

note "Test SEE";
forth_ok(": x 3 BEGIN DUP 0> WHILE DUP . 1- REPEAT DROP ; x SEE x", <<END);
3 2 1 
: x
    3 
L2:
    DUP
    0>
    0BRANCH  L1
    DUP
    .
    1-
    BRANCH  L2
L1:
    DROP
    EXIT
;
END

forth_ok(": x 3 BEGIN .\" hello \" 1- DUP 0<= IF DROP EXIT THEN AGAIN ; x SEE x", <<END);
hello hello hello 
: x
    3 
L2:
    ." hello "
    1-
    DUP
    0<=
    0BRANCH  L1
    DROP
    EXIT
L1:
    BRANCH  L2
    EXIT
;
END

forth_ok(": x IF 1 ELSE 2 THEN . ; 1 x SEE x", <<END);
1 
: x
    0BRANCH  L1
    1 
    BRANCH  L2
L1:
    2 
L2:
    .
    EXIT
;
END

forth_ok(": x 123 . 123. D. ; x SEE x", <<END);
123 123 
: x
    123 
    .
    123. 
    D.
    EXIT
;
END

forth_ok(": x S\" hello\" TYPE ; x SEE x", <<END);
hello
: x
    S" hello"
    TYPE
    EXIT
;
END

forth_ok(": x .( hello) ; x SEE x", <<END);
hello
: x
    EXIT
;
END

forth_ok(": x C\" hello\" COUNT TYPE ; x SEE x", <<END);
hello
: x
    C" hello"
    COUNT
    TYPE
    EXIT
;
END

forth_ok(": x ABORT\" hello\" ; SEE x", <<END);

: x
    ABORT" hello"
    EXIT
;
END

forth_ok(<<'END', <<'END');
: const CREATE ,
  DOES> @ ;
10 const ten

SEE const
SEE ten
END

: const
    CREATE
    ,
    DOES> L1
    EXIT
L1:
    @
    EXIT
;

const ten
00007660              0a 00 00 00                               ....        
END

forth_ok(<<'END', <<'END');
: x 10 0 DO I . I 5 = IF LEAVE THEN LOOP ;
x
SEE x
END
0 1 2 3 4 5 
: x
    10 
    0 
    DO L1
L3:
      I
      .
      I
      5 
      =
      0BRANCH L2
      LEAVE L1
L2:
    LOOP L3
L1:
    EXIT
;
END

forth_ok(<<'END', <<'END');
: x 10 0 ?DO I . I 5 = IF LEAVE THEN LOOP ;
x
SEE x
END
0 1 2 3 4 5 
: x
    10 
    0 
    ?DO L1
L3:
      I
      .
      I
      5 
      =
      0BRANCH L2
      LEAVE L1
L2:
    LOOP L3
L1:
    EXIT
;
END

forth_ok(<<'END', <<'END');
: x 0 10 DO I . I 5 = IF LEAVE THEN -1 +LOOP ;
x
SEE x
END
10 9 8 7 6 5 
: x
    0 
    10 
    DO L1
L3:
      I
      .
      I
      5 
      =
      0BRANCH L2
      LEAVE L1
L2:
      -1 
    +LOOP L3
L1:
    EXIT
;
END

forth_ok(<<'END', <<'END');
: x 0 10 ?DO I . I 5 = IF LEAVE THEN -1 +LOOP ;
x
SEE x
END
10 9 8 7 6 5 
: x
    0 
    10 
    ?DO L1
L3:
      I
      .
      I
      5 
      =
      0BRANCH L2
      LEAVE L1
L2:
      -1 
    +LOOP L3
L1:
    EXIT
;
END

forth_ok(<<'END', <<'END');
		: x CASE 1 OF 1 . ENDOF 
				 2 OF 2 . ENDOF 
				 3 OF 3 . ENDOF 
				  	  0 . ENDCASE ;
1 x
SEE x
END
1 
: x
    1 
    OF L1
    1 
    .
    BRANCH L2
L1:
    2 
    OF L3
    2 
    .
    BRANCH L2
L3:
    3 
    OF L4
    3 
    .
    BRANCH L2
L4:
    0 
    .
    DROP
L2:
    EXIT
;
END

forth_ok("DEFER hello  SEE hello", <<'END');

DEFER hello ACTION OF ABORT
END

forth_ok("123 CONSTANT x  SEE x", <<'END');

123 CONSTANT x
END

forth_ok("123 CONSTANT x 16 ALLOT SEE x", <<'END');

123 CONSTANT x

00007620                                      00 00 00 00               ....
00007630  00 00 00 00 00 00 00 00 00 00 00 00               ............    
END

forth_ok("123. 2CONSTANT xx SEE xx", <<'END');

123. 2CONSTANT xx
END

forth_ok("123. 2CONSTANT xx 16 ALLOT SEE xx", <<'END');

123. 2CONSTANT xx

00007630  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
END

forth_ok("VARIABLE x  123 x !  SEE x", <<'END');

VARIABLE x 123 x !
END

forth_ok("2VARIABLE x  123. x 2!  SEE x", <<'END');

2VARIABLE x 123. x 2!
END

forth_ok("2VARIABLE x  123. x 2! 16 ALLOT  SEE x", <<'END');

CREATE x 
00007620                          00 00 00 00 7b 00 00 00           ....{...
00007630  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
END

forth_ok(<<'END', <<'END');
	BEGIN-STRUCTURE point
		1 CELLS +FIELD p.x
		1 CELLS +FIELD p.y
	END-STRUCTURE
SEE point
SEE p.x
SEE p.y
END

8 CONSTANT point

FIELD p.x OFFSET 0

FIELD p.y OFFSET 4
END

forth_ok("MARKER x SEE x UNUSED 1024 / . 'k' EMIT CR", <<'END');

MARKER x
Latest: 30192 
Here:   30220 
Names:  979908 
927 k
END

note "Test TRACE";
forth_ok("TRACE ON : x DUP + ; 1 x .", <<END);
( ) 
>> : (C: 0 0 ) ( ) 
>> ; (C: ) ( ) 
>> 1  ( 1 ) 
>> x ( 1 ) 
>>>> DUP ( 1 1 ) 
>>>> + ( 2 ) 
>>>> EXIT ( 2 ) 
>> . 2 ( ) 
END

end_test;
