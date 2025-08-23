#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test KEY";
if ($ENV{HARNESS_ACTIVE}) {
    ok 1;
} else {
	path("$test.fs")->spew(<<'END');
		: test-key
			." Press keys and ESC at the end:"
			BEGIN
				KEY
				DUP 27 <> 
			WHILE
				EMIT
			REPEAT
			CR
			." Press any key" KEY DROP CR
		;
		test-key
END
	run_ok("./forth $test.fs");
}

note "Test KEY?";
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
				100000 0 DO LOOP
			REPEAT
			CR
			." You pressed:" KEY EMIT CR
			." Press any key" KEY DROP CR
		;
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

end_test;
