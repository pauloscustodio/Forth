#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test AND";
forth_ok("0 0 AND 0 1 AND 1 0 AND 1 1 AND .S", "( 0 0 0 1 )");

note "Test OR";
forth_ok("0 0 OR 0 1 OR 1 0 OR 1 1 OR .S", "( 0 1 1 1 )");

note "Test XOR";
forth_ok("0 0 XOR 0 1 XOR 1 0 XOR 1 1 XOR .S", "( 0 1 1 0 )");

note "Test INVERT";
forth_ok("-1 INVERT .S", "( 0 )");

note "Test RSHIFT";
forth_ok("20 2 RSHIFT .S", "( 5 )");

note "Test LSHIFT";
forth_ok("5 2 LSHIFT .S", "( 20 )");

note "Test =";
note "Test <>";
note "Test <";
note "Test <=";
note "Test >";
note "Test >=";

note "Test 0=";
note "Test 0<>";
note "Test 0<";
note "Test 0<=";
note "Test 0>";
note "Test 0>=";

for my $op (qw( = <> < <= > >= )) {
	my $perl_op = $op eq '=' ? '==' : $op eq '<>' ? '!=' : $op;
	
	my $forth = "";
	my $expected = "";
	
	for my $a (-1..1) {
		for my $b (-1..1) {		
			my $result = eval("$a $perl_op $b") ? -1 : 0;
			$forth .= "$a $b $op ";
			$expected .= "$result ";
		}
		my $result = eval("$a $perl_op 0") ? -1 : 0;
		$forth .= "$a 0$op ";
		$expected .= "$result ";
	}
	forth_ok("$forth .S", "( $expected)");
}

note "Test U<";
note "Test U<=";
note "Test U>";
note "Test U>=";

for my $op (qw( < <= > >= )) {
	my $forth = "";
	my $expected = "";
	
	for my $a (-1..1) {
		for my $b (-1..1) {		
			my $result = eval("($a & 0xff) $op ($b & 0xff)") ? -1 : 0;
			$forth .= "$a $b U$op ";
			$expected .= "$result ";
		}
	}
	forth_ok("$forth .S", "( $expected)");
}

end_test;
