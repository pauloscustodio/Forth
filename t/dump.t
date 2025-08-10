#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DUMP";
forth_ok('S" The quick brown fox jumps over the lazy dog." DUMP', <<'END');

00000610                                               54                  T
00000620  68 65 20 71 75 69 63 6b 20 62 72 6f 77 6e 20 66   he quick brown f
00000630  6f 78 20 6a 75 6d 70 73 20 6f 76 65 72 20 74 68   ox jumps over th
00000640  65 20 6c 61 7a 79 20 64 6f 67 2e                  e lazy dog.     
END

forth_ok('S" hello world" PAD 1+ SWAP MOVE PAD 1+ 11 DUMP', <<'END');

00000400                 68 65 6c 6c 6f 20 77 6f 72 6c 64        hello world
END

forth_ok('256 BUFFER: buf : init 256 0 DO I buf I + C! LOOP ; init buf 256 DUMP', <<'END');

00001de0                          00 01 02 03 04 05 06 07           ........
00001df0  08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17   ................
00001e00  18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27   ........ !"#$%&'
00001e10  28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 36 37   ()*+,-./01234567
00001e20  38 39 3a 3b 3c 3d 3e 3f 40 41 42 43 44 45 46 47   89:;<=>?@ABCDEFG
00001e30  48 49 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 56 57   HIJKLMNOPQRSTUVW
00001e40  58 59 5a 5b 5c 5d 5e 5f 60 61 62 63 64 65 66 67   XYZ[\]^_`abcdefg
00001e50  68 69 6a 6b 6c 6d 6e 6f 70 71 72 73 74 75 76 77   hijklmnopqrstuvw
00001e60  78 79 7a 7b 7c 7d 7e 7f 80 81 82 83 84 85 86 87   xyz{|}~.........
00001e70  88 89 8a 8b 8c 8d 8e 8f 90 91 92 93 94 95 96 97   ................
00001e80  98 99 9a 9b 9c 9d 9e 9f a0 a1 a2 a3 a4 a5 a6 a7   ................
00001e90  a8 a9 aa ab ac ad ae af b0 b1 b2 b3 b4 b5 b6 b7   ................
00001ea0  b8 b9 ba bb bc bd be bf c0 c1 c2 c3 c4 c5 c6 c7   ................
00001eb0  c8 c9 ca cb cc cd ce cf d0 d1 d2 d3 d4 d5 d6 d7   ................
00001ec0  d8 d9 da db dc dd de df e0 e1 e2 e3 e4 e5 e6 e7   ................
00001ed0  e8 e9 ea eb ec ed ee ef f0 f1 f2 f3 f4 f5 f6 f7   ................
00001ee0  f8 f9 fa fb fc fd fe ff                           ........        
END

end_test;
