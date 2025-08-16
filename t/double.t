#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test 2LITERAL';
forth_ok(": xx [ 1. ] 2LITERAL ; .S xx .S",		"( )( 1 0 )");
forth_ok(": xx [ -1. ] 2LITERAL ; .S xx .S",	"( )( -1 -1 )");

note 'Test D+';
forth_ok("-1 0  1 0  D+ .S", 	"( 0 1 )");

note 'Test D-';
forth_ok("0 1  1 0  D- .S", 	"( -1 0 )");

note 'Test D2*';
forth_ok("1. D2* D.",			"2 ");
forth_ok("2. D2* D.",			"4 ");
forth_ok("2147483648. D2* D.",	"4294967296 ");

note 'Test D2/';
forth_ok("4294967296. D2/ D.",	"2147483648 ");
forth_ok("4. D2/ D.",			"2 ");
forth_ok("2. D2/ D.",			"1 ");
forth_ok("1. D2/ D.",			"0 ");
forth_ok("-4. D2/ D.",			"-2 ");
forth_ok("-2. D2/ D.",			"-1 ");
forth_ok("-1. D2/ D.",			"-1 ");

note 'Test M*/';
forth_ok("2. 6 3 M*/ .S", "( 4 0 )");
forth_ok("5. -7 11 M*/ .S", "( -4 -1 )");
forth_nok("2. 6 0 M*/", "\nError: Division by zero\n");

note 'Test M+';
forth_ok("1. 2 M+ .S", "( 3 0 )");

note 'Test D=';
forth_ok("1. 2. D= .S",			"( 0 )");
forth_ok("2. 2. D= .S",			"( -1 )");
forth_ok("3. 2. D= .S",			"( 0 )");

note 'Test D<>';
forth_ok("1. 2. D<> .S",		"( -1 )");
forth_ok("2. 2. D<> .S",		"( 0 )");
forth_ok("3. 2. D<> .S",		"( -1 )");

note 'Test D<';
forth_ok("1. 2. D< .S",			"( -1 )");
forth_ok("2. 2. D< .S",			"( 0 )");
forth_ok("3. 2. D< .S",			"( 0 )");

note 'Test D<=';
forth_ok("1. 2. D<= .S",		"( -1 )");
forth_ok("2. 2. D<= .S",		"( -1 )");
forth_ok("3. 2. D<= .S",		"( 0 )");

note 'Test D>';
forth_ok("1. 2. D> .S",			"( 0 )");
forth_ok("2. 2. D> .S",			"( 0 )");
forth_ok("3. 2. D> .S",			"( -1 )");

note 'Test D>=';
forth_ok("1. 2. D>= .S",		"( 0 )");
forth_ok("2. 2. D>= .S",		"( -1 )");
forth_ok("3. 2. D>= .S",		"( -1 )");

note "Test DU<";
forth_ok("-1. -1. DU< .S",		"( 0 )");
forth_ok("-1.  1. DU< .S",		"( 0 )");
forth_ok(" 1. -1. DU< .S",		"( -1 )");
forth_ok(" 1.  1. DU< .S",		"( 0 )");

note "Test DU<=";
forth_ok("-1. -1. DU<= .S",		"( -1 )");
forth_ok("-1.  1. DU<= .S",		"( 0 )");
forth_ok(" 1. -1. DU<= .S",		"( -1 )");
forth_ok(" 1.  1. DU<= .S",		"( -1 )");

note "Test DU>";
forth_ok("-1. -1. DU> .S",		"( 0 )");
forth_ok("-1.  1. DU> .S",		"( -1 )");
forth_ok(" 1. -1. DU> .S",		"( 0 )");
forth_ok(" 1.  1. DU> .S",		"( 0 )");

note "Test DU>=";
forth_ok("-1. -1. DU>= .S",		"( -1 )");
forth_ok("-1.  1. DU>= .S",		"( -1 )");
forth_ok(" 1. -1. DU>= .S",		"( 0 )");
forth_ok(" 1.  1. DU>= .S",		"( -1 )");

note 'Test D0=';
forth_ok("-1. D0= .S",		"( 0 )");
forth_ok(" 0. D0= .S",		"( -1 )");
forth_ok(" 1. D0= .S",		"( 0 )");

note 'Test D0<>';
forth_ok("-1. D0<> .S",		"( -1 )");
forth_ok(" 0. D0<> .S",		"( 0 )");
forth_ok(" 1. D0<> .S",		"( -1 )");

note 'Test D0<';
forth_ok("-1. D0< .S",		"( -1 )");
forth_ok(" 0. D0< .S",		"( 0 )");
forth_ok(" 1. D0< .S",		"( 0 )");

note 'Test D0<=';
forth_ok("-1. D0<= .S",		"( -1 )");
forth_ok(" 0. D0<= .S",		"( -1 )");
forth_ok(" 1. D0<= .S",		"( 0 )");

note 'Test D0>';
forth_ok("-1. D0> .S",		"( 0 )");
forth_ok(" 0. D0> .S",		"( 0 )");
forth_ok(" 1. D0> .S",		"( -1 )");

note 'Test D0>=';
forth_ok("-1. D0>= .S",		"( 0 )");
forth_ok(" 0. D0>= .S",		"( -1 )");
forth_ok(" 1. D0>= .S",		"( -1 )");

note 'Test D>S';
forth_ok("1. D>S .S",			"( 1 )");
forth_ok("0. D>S .S",			"( 0 )");
forth_ok("-1. D>S .S",			"( -1 )");

note "Test DABS";
forth_ok(" 1. .S DABS .S", "( 1 0 )( 1 0 )");
forth_ok(" 0. .S DABS .S", "( 0 0 )( 0 0 )");
forth_ok("-1. .S DABS .S", "( -1 -1 )( 1 0 )");

note "Test DMAX";
forth_ok("1. 2. DMAX .S", "( 2 0 )");

note "Test DMIN";
forth_ok("1. 2. DMIN .S", "( 1 0 )");

note "Test DNEGATE";
forth_ok(" 5. DNEGATE .S", "( -5 -1 )");
forth_ok(" 0. DNEGATE .S", "( 0 0 )");
forth_ok("-5. DNEGATE .S", "( 5 0 )");

end_test;
