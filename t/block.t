#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test LOAD';
note 'Test BLK';
unlink <scr*.blk>;
path("scr00001.blk")->spew(<<'END');
1 2  BLK @  2 LOAD
END
path("scr00002.blk")->spew(<<'END');
3 4  BLK @
END
forth_ok("1 LOAD .S", "( 1 2 1 3 4 2 )");

note 'Test REFILL';
unlink <scr*.blk>;
path("scr00001.blk")->spew(<<'END');
1 2  REFILL
END
path("scr00002.blk")->spew(<<'END');
3 4  REFILL
END
forth_ok(<<'END', "( 1 2 -1 3 4 -1 5 6 )");
	1 LOAD
	5 6
	.S
END

note 'Test THRU';
unlink <scr*.blk>;
path("scr00001.blk")->spew(<<'END');
1 2  
END
path("scr00002.blk")->spew(<<'END');
3 4 
END
path("scr00003.blk")->spew(<<'END');
5 6
END
forth_ok("1 2 THRU .S", "( 1 2 3 4 )");

note 'Test \\';
unlink <scr*.blk>;
path("scr00001.blk")->spew(
	#1234567890123456789012345678901234567890123456789012345678901234
	"1 2 \\ 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ".
	"3 4 \\ 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 ".
	"5 6                                                           \\ ".
	"7 8                                                             ");
forth_ok("1 LOAD .S", "( 1 2 3 4 5 6 7 8 )");

note 'Test BLOCK';
note 'Test BUFFER';
unlink <scr*.blk>;
path("scr00001.blk")->spew(<<'END');
1 2  
END
path("scr00002.blk")->spew(<<'END');
3 4 
END
path("scr00003.blk")->spew(<<'END');
5 6
END
forth_ok("1 BLOCK 3 TYPE", "1 2");
forth_ok("2 BLOCK 3 TYPE", "3 4");
forth_ok("3 BLOCK 3 TYPE", "5 6");
forth_ok("4 BLOCK 3 TYPE", "   ");
forth_ok("1 BUFFER 3 TYPE", "   ");

note 'Test SAVE-BUFFERS';
note 'Test EMPTY-BUFFERS';
note 'Test FLUSH';
note 'Test UPDATE';
unlink <scr*.blk>;
forth_ok('S" block1" 1 BUFFER SWAP MOVE UPDATE SAVE-BUFFERS', "");
is path("scr00001.blk")->slurp, "block1".(" " x (1024-6)), "buffer saved";

unlink <scr*.blk>;
forth_ok('S" block1" 1 BUFFER SWAP MOVE        SAVE-BUFFERS', "");
ok ! -f "scr00001.blk", "buffer not saved";

unlink <scr*.blk>;
forth_ok('S" block1" 1 BUFFER SWAP MOVE UPDATE FLUSH', "");
is path("scr00001.blk")->slurp, "block1".(" " x (1024-6)), "buffer saved";

unlink <scr*.blk>;
forth_ok('S" block1" 1 BUFFER SWAP MOVE        FLUSH', "");
ok ! -f "scr00001.blk", "buffer not saved";

unlink <scr*.blk>;
forth_ok('S" block1" 1 BUFFER SWAP MOVE UPDATE EMPTY-BUFFERS', "");
ok ! -f "scr00001.blk", "buffer not saved";

note 'Test LIST';
note 'Test SCR';
unlink <scr*.blk>;
my $text = "";
my $i = 0;
$text .= ++$i . " " while length($text) < 1024;
$text = substr($text, 0, 1024);
path("scr00001.blk")->spew($text);
forth_ok("SCR @  1 LIST  SCR @  .S CR", <<'END');
 1  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 2
 2  5 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46
 3   47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 
 4  68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 8
 5  9 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 
 6  108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 
 7  124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 
 8  140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 
 9  156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 
10  172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 
11  188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 
12  204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 
13  220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 
14  236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 
15  252 253 254 255 256 257 258 259 260 261 262 263 264 265 266 267 
16  268 269 270 271 272 273 274 275 276 277 278 279 280 281 282 283 
( 0 1 )
END

unlink <scr*.blk>;
end_test;
