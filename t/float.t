#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test D>F";
forth_ok("1234. D>F .S .FS", "( ) (F: 1234 )");

note "Test F>D";
forth_ok("1234e F>D .S .FS", "( 1234 0 ) (F: )");

note "Test F@";
note "Test F!";
forth_ok("3.14e PAD F! .S .FS PAD F@ .S .FS", "( ) (F: ) ( ) (F: 3.14 )");

note "Test F+";
forth_ok("1e 2e F+ .FS", "(F: 3 )");

note "Test F*";
forth_ok("2e 4e F* .FS", "(F: 8 )");

note "Test F-";
forth_ok("2e 1e F- .FS", "(F: 1 )");

note "Test F/";
forth_ok("15e 2e F/ .FS", "(F: 7.5 )");
forth_nok("10e 0e F/", "\nError: division by zero\n");

note "Test F=";
note "Test F<>";
note "Test F<";
note "Test F<=";
note "Test F>";
note "Test F>=";

note "Test F0=";
note "Test F0<>";
note "Test F0<";
note "Test F0<=";
note "Test F0>";
note "Test F0>=";

for my $op (qw( = <> < <= > >= )) {
	my $perl_op = $op eq '=' ? '==' : $op eq '<>' ? '!=' : $op;
	
	my $forth = "";
	my $expected = "";
	
	for my $a (-1..1) {
		for my $b (-1..1) {		
			my $result = eval("$a $perl_op $b") ? -1 : 0;
			$forth .= "${a}e ${b}e F$op ";
			$expected .= "$result ";
		}
		my $result = eval("$a $perl_op 0") ? -1 : 0;
		$forth .= "${a}e F0$op ";
		$expected .= "$result ";
	}
	forth_ok("$forth .S", "( $expected)");
}

note "Test FALIGN";
forth_ok("HERE FALIGN HERE SWAP - .S", "( 0 )");
forth_ok("1 C, HERE FALIGN HERE SWAP - .S", "( 3 )");

note "Test FALIGNED";
forth_ok("0 FALIGNED .S", "( 0 )");
forth_ok("1 FALIGNED .S", "( 4 )");
forth_ok("2 FALIGNED .S", "( 4 )");
forth_ok("3 FALIGNED .S", "( 4 )");
forth_ok("4 FALIGNED .S", "( 4 )");
forth_ok("5 FALIGNED .S", "( 8 )");

note 'Test FCONSTANT';
forth_ok("1e .FS FCONSTANT xx .FS xx .FS",	"(F: 1 ) (F: ) (F: 1 )");
forth_ok("-1e .FS FCONSTANT xx .FS xx .FS",	"(F: -1 ) (F: ) (F: -1 )");

note "Test FDROP";
forth_ok("1e FDROP .FS  1e 2e FDROP .FS", "(F: )(F: 1 )");
forth_nok("FDROP", "\nError: floating point stack underflow\n");

note "Test FSWAP";
forth_ok("1e 2e FSWAP .FS  3e FSWAP .FS", "(F: 2 1 )(F: 2 3 1 )");
forth_nok("1e FSWAP", "\nError: floating point stack underflow\n");

note "Test FDUP";
forth_ok("1e FDUP .FS  2e FDUP .FS", "(F: 1 1 )(F: 1 1 2 2 )");
forth_nok("FDUP", "\nError: floating point stack underflow\n");

note "Test FOVER";
forth_ok("1e 2e FOVER .FS", "(F: 1 2 1 )");
forth_nok("1e FOVER", "\nError: floating point stack underflow\n");

note "Test FROT";
forth_ok("1e 2e 3e FROT .FS", "(F: 2 3 1 )");
forth_nok("1e 2e FROT", "\nError: floating point stack underflow\n");

note "Test -FROT";
forth_ok("1e 2e 3e -FROT .FS", "(F: 3 1 2 )");
forth_nok("1e 2e -FROT", "\nError: floating point stack underflow\n");

note "Test FDEPTH";
forth_ok("FDEPTH . 1e FDEPTH . 2e FDEPTH . .S .FS", "0 1 2 ( ) (F: 1 2 )");

note "Test FLITERAL";
forth_ok(": x [ 1e FLITERAL ] [ 2e ] FLITERAL ; x .FS", "(F: 1 2 )");

note "Test FLOAT+";
forth_ok("0 FLOAT+ .S", "( 8 )");
forth_ok("8 FLOAT+ .S", "( 16 )");
forth_ok("16 FLOAT+ .S", "( 24 )");

note "Test FLOATS";
forth_ok("0 FLOATS .S", "( 0 )");
forth_ok("1 FLOATS .S", "( 8 )");
forth_ok("2 FLOATS .S", "( 16 )");

note "Test FLOOR";
forth_ok("1.1e FLOOR 1.5e FLOOR 1.9e FLOOR .FS", "(F: 1 1 1 )");
forth_ok("-1.1e FLOOR -1.5e FLOOR -1.9e FLOOR .FS", "(F: -2 -2 -2 )");

note "Test FMAX";
forth_ok("1.1e 1.2e FMAX .FS", "(F: 1.2 )");

note "Test FMIN";
forth_ok("1.1e 1.2e FMIN .FS", "(F: 1.1 )");

note "Test FNEGATE";
forth_ok(" 5.1e FNEGATE .FS", "(F: -5.1 )");
forth_ok(" 0e   FNEGATE .FS", "(F: 0 )");
forth_ok("-5.1e FNEGATE .FS", "(F: 5.1 )");

note "Test FROUND";
forth_ok("1.1e FROUND .FS", "(F: 1 )");
forth_ok("1.4e FROUND .FS", "(F: 1 )");
forth_ok("1.5e FROUND .FS", "(F: 2 )");
forth_ok("1.9e FROUND .FS", "(F: 2 )");

forth_ok("-1.1e FROUND .FS", "(F: -1 )");
forth_ok("-1.4e FROUND .FS", "(F: -1 )");
forth_ok("-1.5e FROUND .FS", "(F: -2 )");
forth_ok("-1.9e FROUND .FS", "(F: -2 )");

note "Test FABS";
forth_ok(" 1.1e .FS FABS .FS", "(F: 1.1 )  (F: 1.1 )");
forth_ok(" 0e   .FS FABS .FS", "(F: 0 )    (F: 0 )");
forth_ok("-1.1e .FS FABS .FS", "(F: -1.1 ) (F: 1.1 )");

note 'Test FVARIABLE';
forth_ok("FVARIABLE xx xx F@ .FS",			"(F: 0 )");
forth_ok("FVARIABLE xx 1e xx F! xx F@ .FS",	"(F: 1 )");
forth_ok("FVARIABLE xx -1e xx F! xx F@ .FS","(F: -1 )");

note "Test REPRESENT";
forth_ok("1e PAD 5 REPRESENT PAD 5 TYPE .S", "10000( 1 0 -1 )");
forth_ok("-1e PAD 5 REPRESENT PAD 5 TYPE .S", "10000( 1 -1 -1 )");
forth_ok("100e 3e F/ PAD 5 REPRESENT PAD 5 TYPE .S", "33333( 2 0 -1 )");
forth_ok("0.02e 3e F/ PAD 5 REPRESENT PAD 5 TYPE .S", "66667( -2 0 -1 )");

end_test;
