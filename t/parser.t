#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

forth_ok(("0" x 1000)." .S", "( 0 )");

note "Test WORD";
note "Test COUNT";
note "Test TYPE";
forth_ok("BL WORD hello COUNT TYPE", "hello");
forth_ok(<<'END', "hello");
		'.' WORD hello
		COUNT TYPE
END

note "Test PARSE";
forth_ok(<<END, "( )");
		'!' PARSE
		TYPE .S
END
forth_ok("'!' PARSE ! TYPE .S", "( )");
forth_ok("'!' PARSE abc! TYPE '!' EMIT '!' PARSE def! TYPE .S", "abc!def( )");

note "Test PARSE-WORD";
forth_ok(<<END, "( )");
		PARSE-WORD
		TYPE .S
END
forth_ok("PARSE-WORD  !   TYPE .S", "!( )");
forth_ok("PARSE-WORD  abc TYPE '!' EMIT PARSE-WORD def TYPE .S", "abc!def( )");

note "Test PARSE-NAME";
forth_ok(<<END, "( )");
		PARSE-NAME
		TYPE .S
END
forth_ok("PARSE-NAME  !   TYPE .S", "!( )");
forth_ok("PARSE-NAME  abc TYPE '!' EMIT PARSE-NAME def TYPE .S", "abc!def( )");

note "Test CHAR";
forth_ok("CHAR ! .S", "( 33 )");

note "Test [CHAR]";
forth_ok(": x [CHAR] ! ; x .S", "( 33 )");

note "Test >NUMBER";
note "Test NUMBER";
note "Test NUMBER?";
note "Test CONVERT";
note "Test DPL";
note "Test BASE";
note "Test DECIMAL";
note "Test HEX";

forth_ok(("0" x 1000)." .S", "( 0 )");

# not a number
forth_ok(" BL WORD abc COUNT NUMBER? .S DROP DPL @ .S", "( 0 )( 0 )");
forth_nok("BL WORD abc COUNT NUMBER  .S DROP DPL @ .S", 
		  "\nError: Invalid number: abc\n");
forth_ok("0. 1000 0            >NUMBER      .S", "( 0 0 1000 0 )");
forth_ok("0. BL WORD abc COUNT >NUMBER TYPE .S", "abc( 0 0 )");
forth_ok("0. BL WORD abc CONVERT 3 TYPE .S", "abc( 0 0 )");

# wrong digit base 2
forth_ok("BL WORD 102 COUNT 2 BASE ! NUMBER? DECIMAL .S DROP DPL @ .S", 
		 "( 0 )( 0 )");
forth_nok("BL WORD 102 COUNT 2 BASE ! NUMBER  DECIMAL .S DROP DPL @ .S", 
		  "\nError: Invalid number: 102\n");
forth_ok("0. BL WORD 102 COUNT 2 BASE ! >NUMBER DECIMAL TYPE .S", "2( 2 0 )");
forth_ok("0. BL WORD 102 2 BASE ! CONVERT DECIMAL 1 TYPE .S", "2( 2 0 )");

# wrong digit base 10
forth_ok("BL WORD 10a COUNT NUMBER? .S DROP DPL @ .S", "( 0 )( 0 )");
forth_nok("BL WORD 10a COUNT NUMBER  .S DROP DPL @ .S", 
		  "\nError: Invalid number: 10a\n");
forth_ok("0. BL WORD 10a COUNT >NUMBER TYPE .S", "a( 10 0 )");
forth_ok("0. BL WORD 10a CONVERT 1 TYPE .S", "a( 10 0 )");

# wrong digit base 16
forth_ok("BL WORD 10g COUNT HEX NUMBER? DECIMAL .S DROP DPL @ .S", "( 0 )( 0 )");
forth_nok("BL WORD 10g COUNT HEX NUMBER  DECIMAL .S DROP DPL @ .S", 
		  "\nError: Invalid number: 10g\n");
forth_ok("0. BL WORD 10g COUNT HEX >NUMBER DECIMAL TYPE .S", "g( 16 0 )");
forth_ok("0. BL WORD 10g HEX CONVERT DECIMAL 1 TYPE .S", "g( 16 0 )");

# 9
forth_ok("BL WORD 9 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 9 1 )( 0 )");
forth_ok("BL WORD 9 COUNT NUMBER  .S      DROP DPL @ .S", "( 9 )( 0 )");

# + 99
forth_ok("BL WORD +99 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 99 1 )( 0 )");
forth_ok("BL WORD +99 COUNT NUMBER  .S      DROP DPL @ .S", "( 99 )( 0 )");

# - 999
forth_ok("BL WORD -999 COUNT NUMBER? .S DROP DROP DPL @ .S", "( -999 1 )( 0 )");
forth_ok("BL WORD -999 COUNT NUMBER  .S      DROP DPL @ .S", "( -999 )( 0 )");

# double 9999
forth_ok("BL WORD 9999. COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( 9999 0 2 )( 0 )");
forth_ok("BL WORD 9999. COUNT NUMBER  .S      2DROP DPL @ .S", "( 9999 0 )( 0 )");

# double 0x7FFFFFFFFFFFFFFF
forth_ok("BL WORD 9.223.372.036.854.775.807 COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( -1 2147483647 2 )( 3 )");
forth_ok("BL WORD 9.223.372.036.854.775.807 COUNT NUMBER  .S      2DROP DPL @ .S", "( -1 2147483647 )( 3 )");

# double 0xFFFFFFFFFFFFFFFF
forth_ok("BL WORD -1. COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( -1 -1 2 )( 0 )");
forth_ok("BL WORD -1. COUNT NUMBER  .S      2DROP DPL @ .S", "( -1 -1 )( 0 )");

# quoted char
forth_ok("BL WORD '!' COUNT NUMBER? .S DROP DROP DPL @ .S", "( 33 1 )( 0 )");
forth_ok("BL WORD '!' COUNT NUMBER  .S      DROP DPL @ .S", "( 33 )( 0 )");

forth_ok("BL WORD -'!' COUNT NUMBER? .S DROP DROP DPL @ .S", "( -33 1 )( 0 )");
forth_ok("BL WORD -'!' COUNT NUMBER  .S      DROP DPL @ .S", "( -33 )( 0 )");

# decimal
forth_ok("BL WORD #255 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD #255 COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -#-255 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD -#-255 COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -#255 COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD -#255 COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");

forth_ok("BL WORD #-255 COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD #-255 COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");


# hex
forth_ok("BL WORD \$ff COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD \$ff COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -\$-ff COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD -\$-ff COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -\$ff COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD -\$ff COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");

forth_ok("BL WORD \$-ff COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD \$-ff COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");

# binary
forth_ok("BL WORD %11111111 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD %11111111 COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -%-11111111 COUNT NUMBER? .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD -%-11111111 COUNT NUMBER  .S      DROP DPL @ .S", "( 255 )( 0 )");

forth_ok("BL WORD -%11111111 COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD -%11111111 COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");

forth_ok("BL WORD %-11111111 COUNT NUMBER? .S DROP DROP DPL @ .S", "( -255 1 )( 0 )");
forth_ok("BL WORD %-11111111 COUNT NUMBER  .S      DROP DPL @ .S", "( -255 )( 0 )");

# base = 2
forth_ok("BL WORD 11111111 COUNT 2 BASE ! NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 255 1 )( 0 )");
forth_ok("BL WORD 11111111 COUNT 2 BASE ! NUMBER  DECIMAL .S      DROP DPL @ .S", "( 255 )( 0 )");

# base = 8
forth_ok("BL WORD 7777 COUNT 8 BASE ! NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 4095 1 )( 0 )");
forth_ok("BL WORD 7777 COUNT 8 BASE ! NUMBER  DECIMAL .S      DROP DPL @ .S", "( 4095 )( 0 )");

# base = 16
forth_ok("BL WORD FFF COUNT 16 BASE ! NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 4095 1 )( 0 )");
forth_ok("BL WORD FFF COUNT 16 BASE ! NUMBER  DECIMAL .S      DROP DPL @ .S", "( 4095 )( 0 )");

forth_ok("BL WORD FFF COUNT HEX NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 4095 1 )( 0 )");
forth_ok("BL WORD FFF COUNT HEX NUMBER  DECIMAL .S      DROP DPL @ .S", "( 4095 )( 0 )");

# base = 36
forth_ok("BL WORD z COUNT 36 BASE ! NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 35 1 )( 0 )");
forth_ok("BL WORD z COUNT 36 BASE ! NUMBER  DECIMAL .S      DROP DPL @ .S", "( 35 )( 0 )");

forth_ok("BL WORD 100 COUNT 36 BASE ! NUMBER? DECIMAL .S DROP DROP DPL @ .S", "( 1296 1 )( 0 )");
forth_ok("BL WORD 100 COUNT 36 BASE ! NUMBER  DECIMAL .S      DROP DPL @ .S", "( 1296 )( 0 )");

# all punctuation chars
for my $p (split(" ", ", . + - / :")) {
	forth_ok("BL WORD 10${p} COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( 10 0 2 )( 0 )");
	forth_ok("BL WORD 10${p} COUNT NUMBER  .S      2DROP DPL @ .S", "( 10 0 )( 0 )");

	forth_ok("BL WORD 1${p}0 COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( 10 0 2 )( 1 )");
	forth_ok("BL WORD 1${p}0 COUNT NUMBER  .S      2DROP DPL @ .S", "( 10 0 )( 1 )");

	forth_ok("BL WORD 1${p}0${p}0 COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( 100 0 2 )( 1 )");
	forth_ok("BL WORD 1${p}0${p}0 COUNT NUMBER  .S      2DROP DPL @ .S", "( 100 0 )( 1 )");

	forth_ok("BL WORD 1${p}00 COUNT NUMBER? .S DROP 2DROP DPL @ .S", "( 100 0 2 )( 2 )");
	forth_ok("BL WORD 1${p}00 COUNT NUMBER  .S      2DROP DPL @ .S", "( 100 0 )( 2 )");

	# no digits
	forth_ok("BL WORD ${p}${p}${p} COUNT NUMBER? .S DROP DPL @ .S", "( 0 )( 0 )");
	forth_nok("BL WORD ${p}${p}${p} COUNT NUMBER  .S DROP DPL @ .S", 
			  "\nError: Invalid number: ${p}${p}${p}\n");
}

# no sign, negative and positive double number
for my $s ("", "+", "-") {
	my $s1 = ($s eq "-") ? "-" : "";
	my $s2 = ($s eq "-") ? "-1" : "0";

	forth_ok("BL WORD ${s}4294967296. COUNT NUMBER? .S", "( 0 ${s1}1 2 )");
	forth_ok("BL WORD ${s}4294967296. COUNT NUMBER  .S", "( 0 ${s1}1 )");

	forth_ok("HEX BL WORD ${s}100000000. COUNT NUMBER? .S", "( 0 ${s1}1 2 )");
	forth_ok("HEX BL WORD ${s}100000000. COUNT NUMBER  .S", "( 0 ${s1}1 )");

	forth_ok("36 BASE ! BL WORD ${s}1z141z4. COUNT NUMBER? .S", "( 0 ${s1}1 2 )");
	forth_ok("36 BASE ! BL WORD ${s}1z141z4. COUNT NUMBER  .S", "( 0 ${s1}1 )");

	forth_ok("36 BASE ! BL WORD ${s}1Z141Z4. COUNT NUMBER? .S", "( 0 ${s1}1 2 )");
	forth_ok("36 BASE ! BL WORD ${s}1Z141Z4. COUNT NUMBER  .S", "( 0 ${s1}1 )");
}

# compile single precision number
forth_ok(": x 123 ; x .S", "( 123 )");

# compile double precision number
forth_ok(": x 1.23 ; x .S", "( 123 0 )");

note "Test DPL";
forth_ok("123. DPL @ .S", "( 123 0 0 )");
forth_ok("12.3 DPL @ .S", "( 123 0 1 )");
forth_ok("1.23 DPL @ .S", "( 123 0 2 )");
forth_ok(".123 DPL @ .S", "( 123 0 3 )");

end_test;
