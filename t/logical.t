#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

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
