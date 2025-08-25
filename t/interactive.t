#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test KEY?";
note "Test MS";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		: test-key
			." Press any key to stop:"
			BEGIN
				KEY? 0= 
			WHILE
				'.' EMIT
				1000 MS
			REPEAT
			CR
			." You pressed:" KEY EMIT CR
			." Press any key" KEY DROP CR
		;
		test-key
END
	run_ok("./forth $test.fs");
}

note "Test KEY";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		: printable DUP 32 >= OVER 126 <= AND ;
		: test-key
			." Press keys and ESC at the end:" CR
			BEGIN
				KEY
				DUP 0 <# BL HOLD # #S #> TYPE
				DUP printable IF EMIT ELSE DROP THEN CR
				27 = 
			UNTIL
			CR
			." Press any key" KEY DROP CR
		;
		HEX 
		test-key
END
	run_ok("./forth $test.fs");
}

note "Test EKEY?";
note "Test MS";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		: test-key
			." Press any key to stop:"
			BEGIN
				EKEY? 0= 
			WHILE
				'.' EMIT
				1000 MS
			REPEAT
			CR
			." You pressed:" EKEY . CR
			." Press any key" KEY DROP CR
		;
		HEX
		test-key
END
	run_ok("./forth $test.fs");
}

note "Test EKEY";
note "Test EKEY>CHAR";
note "Test EKEY>FKEY";
note "Test K-ALT-MASK";
note "Test K-CTRL-MASK";
note "Test K-SHIFT-MASK";
note "Test K-UP";
note "Test K-DOWN";
note "Test K-LEFT";
note "Test K-RIGHT";
note "Test K-HOME";
note "Test K-END";
note "Test K-INSERT";
note "Test K-DELETE";
note "Test K-PRIOR";
note "Test K-NEXT";
note "Test K-F1";
note "Test K-F2";
note "Test K-F3";
note "Test K-F4";
note "Test K-F5";
note "Test K-F6";
note "Test K-F7";
note "Test K-F8";
note "Test K-F9";
note "Test K-F10";
note "Test K-F11";
note "Test K-F12";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		: printable DUP 32 >= OVER 126 <= AND ;
		: out_number 0 <# BL HOLD # #S #> TYPE ;
		: test-key
			." Press keys and ESC at the end:" CR
			BEGIN
				EKEY
				DUP out_number
				DUP printable IF EMIT ELSE DROP SPACE THEN SPACE
				DUP EKEY>CHAR IF EMIT ELSE DROP SPACE THEN SPACE
				DUP EKEY>FKEY IF out_number ELSE DROP THEN SPACE
				
				DUP EKEY>FKEY IF K-ALT-MASK AND 
				IF ." ALT" K-ALT-MASK INVERT AND THEN 
				ELSE DROP THEN SPACE
				
				DUP EKEY>FKEY IF K-CTRL-MASK AND 
				IF ." CTRL" K-CTRL-MASK INVERT AND THEN 
				ELSE DROP THEN SPACE
				
				DUP EKEY>FKEY IF K-SHIFT-MASK AND 
				IF ." SHIFT" K-SHIFT-MASK AND THEN 
				ELSE DROP THEN SPACE
				
				DUP EKEY>FKEY IF K-UP = IF ." UP" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-DOWN = IF ." DOWN" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-LEFT = IF ." LEFT" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-RIGHT = IF ." RIGHT" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-HOME = IF ." HOME" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-END = IF ." END" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-INSERT = IF ." INSERT" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-DELETE = IF ." DELETE" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-PRIOR = IF ." PRIOR" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-NEXT = IF ." NEXT" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F1 = IF ." F1" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F2 = IF ." F2" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F3 = IF ." F3" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F4 = IF ." F4" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F5 = IF ." F5" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F6 = IF ." F6" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F7 = IF ." F7" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F8 = IF ." F8" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F9 = IF ." F9" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F10 = IF ." F10" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F11 = IF ." F11" THEN ELSE DROP THEN SPACE
				DUP EKEY>FKEY IF K-F12 = IF ." F12" THEN ELSE DROP THEN SPACE
				
				CR
				27 = 
			UNTIL
			CR
			." Press any key" EKEY DROP CR
		;
		HEX 
		test-key
END
	run_ok("./forth $test.fs");
}

note "Test AT-XY";
note "Test PAGE";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		20 CONSTANT size
		: clear 
			." Press any key to clear screen" KEY DROP
			PAGE
		;
		: star '*' EMIT ;
		: star-xy AT-XY star ;
		: draw
			clear
			size 1+ 0 DO
				I I star-xy
				I size I - star-xy
				0 I star-xy
				size I star-xy
				I 0 star-xy
				I size star-xy
			LOOP
			CR
			." Press any key" KEY DROP CR
		;
		draw
END
	run_ok("./forth $test.fs");
}

note "Test TIME&DATE";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		." Date-time: "
		TIME&DATE . . . ." - " . . . CR
		.S
		CR
		." Press any key" KEY DROP CR
END
	run_ok("./forth $test.fs");
}

end_test;
