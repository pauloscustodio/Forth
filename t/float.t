#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test D>F";
forth_ok("1234. D>F .S .FS", "( ) (F: 1234 )");

note "Test F>D";
forth_ok("1234e F>D .S .FS", "( 1234 0 ) (F: )");
forth_ok("1234.9e F>D .S .FS", "( 1234 0 ) (F: )");
forth_ok("-1234.9e F>D .S .FS", "( -1234 -1 ) (F: )");

note "Test S>F";
forth_ok("1234 S>F .S .FS", "( ) (F: 1234 )");

note "Test F>S";
forth_ok("1234e F>S .S .FS", "( 1234 ) (F: )");
forth_ok("1234.9e F>S .S .FS", "( 1234 ) (F: )");
forth_ok("-1234.9e F>S .S .FS", "( -1234 ) (F: )");

note "Test F@";
note "Test F!";
forth_ok("3.14e PAD F! .S .FS PAD F@ .S .FS", "( ) (F: ) ( ) (F: 3.14 )");

note "Test DF@";
note "Test DF!";
forth_ok("3.14e PAD DF! .S .FS PAD DF@ .S .FS", "( ) (F: ) ( ) (F: 3.14 )");

note "Test SF@";
note "Test SF!";
forth_ok("3.14e PAD SF! .S .FS PAD SF@ .S .FS", "( ) (F: ) ( ) (F: 3.14 )");

note "Test F+";
forth_ok("1e 2e F+ .FS", "(F: 3 )");

note "Test F*";
forth_ok("2e 4e F* .FS", "(F: 8 )");

note "Test F-";
forth_ok("2e 1e F- .FS", "(F: 1 )");

note "Test F/";
forth_ok("15e 2e F/ .FS", "(F: 7.5 )");
if (0) {	# test results differ from machine to machine
	forth_ok(" 0e 0e F/ .FS", "(F: nan)");
	forth_ok("10e 0e F/ .FS", "(F: inf)");
}

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

note "Test DFALIGN";
forth_ok("HERE DFALIGN HERE SWAP - .S", "( 0 )");
forth_ok("1 C, HERE DFALIGN HERE SWAP - .S", "( 3 )");

note "Test DFALIGNED";
forth_ok("0 DFALIGNED .S", "( 0 )");
forth_ok("1 DFALIGNED .S", "( 4 )");
forth_ok("2 DFALIGNED .S", "( 4 )");
forth_ok("3 DFALIGNED .S", "( 4 )");
forth_ok("4 DFALIGNED .S", "( 4 )");
forth_ok("5 DFALIGNED .S", "( 8 )");

note "Test SFALIGN";
forth_ok("HERE SFALIGN HERE SWAP - .S", "( 0 )");
forth_ok("1 C, HERE SFALIGN HERE SWAP - .S", "( 3 )");

note "Test SFALIGNED";
forth_ok("0 SFALIGNED .S", "( 0 )");
forth_ok("1 SFALIGNED .S", "( 4 )");
forth_ok("2 SFALIGNED .S", "( 4 )");
forth_ok("3 SFALIGNED .S", "( 4 )");
forth_ok("4 SFALIGNED .S", "( 4 )");
forth_ok("5 SFALIGNED .S", "( 8 )");

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

note "Test DFLOAT+";
forth_ok("0 DFLOAT+ .S", "( 8 )");
forth_ok("8 DFLOAT+ .S", "( 16 )");
forth_ok("16 DFLOAT+ .S", "( 24 )");

note "Test DFLOATS";
forth_ok("0 DFLOATS .S", "( 0 )");
forth_ok("1 DFLOATS .S", "( 8 )");
forth_ok("2 DFLOATS .S", "( 16 )");

note "Test SFLOAT+";
forth_ok("0 SFLOAT+ .S", "( 8 )");
forth_ok("8 SFLOAT+ .S", "( 16 )");
forth_ok("16 SFLOAT+ .S", "( 24 )");

note "Test SFLOATS";
forth_ok("0 SFLOATS .S", "( 0 )");
forth_ok("1 SFLOATS .S", "( 8 )");
forth_ok("2 SFLOATS .S", "( 16 )");

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

note "Test F**";
forth_ok("2e 5e F** .FS", "(F: 32 )");
forth_ok("64e 0.5e F** .FS", "(F: 8 )");

note "Test F.";
forth_ok("1.23e F.", "1.23 ");
forth_ok("1.23e3 F.", "1230. ");
forth_ok("1.23e9 F.", "1230000000. ");

note "Test FE.";
forth_ok("0e FE.", "0. ");                    # 0 ? "0. "
forth_ok("1e FE.", "1E0 ");                   # 1 ? "1e0 "
forth_ok("123.456e FE.", "123.456E0 ");       # 123.456 ? "123.456e0 "
forth_ok("0.00123e FE.", "1.23E-3 ");         # 0.00123 ? "1.23e-3 "
forth_ok("1000000e FE.", "1E6 ");             # 1,000,000 ? "1e6 "
forth_ok("-987654.321e FE.", "-987.654321E3 ");# -987654.321 ? "-987.654321e3 "
forth_ok("3.1415926535e FE.", "3.141593E0 "); # p ? "3.141593e0 "
forth_ok("2.718281828e FE.", "2.718282E0 ");  # e ? "2.718282e0 "
forth_ok("0.000000987e FE.", "987E-9 ");      # 9.87e-7 ? "987e-9 "
forth_ok("-0.000456e FE.", "-456E-6 ");       # -4.56e-4 ? "-456e-6 "

note "Test FS.";
forth_ok("0e FS.", "0E+0 ");                      # 0 ? "0.0E0 "
forth_ok("1e FS.", "1E+0 ");                      # 1 ? "1.0E0 "
forth_ok("123.456e FS.", "1.23456E+2 ");          # 123.456 ? "1.23456E2 "
forth_ok("0.00123e FS.", "1.23E-3 ");             # 0.00123 ? "1.23E-3 "
forth_ok("1000000e FS.", "1E+6 ");                # 1,000,000 ? "1.0E6 "
forth_ok("-987654.321e FS.", "-9.876543E+5 ");    # -987654.321 ? "-9.876543E5 "
forth_ok("3.1415926535e FS.", "3.141593E+0 ");    # p ? "3.141593E0 "
forth_ok("2.718281828e FS.", "2.718282E+0 ");     # e ? "2.718282E0 "
forth_ok("0.000000987e FS.", "9.87E-7 ");         # 9.87e-7 ? "9.87E-7 "
forth_ok("-0.000456e FS.", "-4.56E-4 ");          # -4.56e-4 ? "-4.56E-4 "

note "Test FSIN";
forth_ok("0e FSIN F.", "0.");               # sin(0) = 0
forth_ok("0.5235987756e FSIN F.", "0.5");   # sin(p/6) ˜ 0.5
forth_ok("0.7853981634e FSIN F.", "0.707107");  # sin(p/4) ˜ v2/2
forth_ok("1.5707963268e FSIN F.", "1.");     # sin(p/2) = 1
forth_ok("3.1415926536e FSIN F.", "0.");     # sin(p) = 0
forth_ok("4.7123889804e FSIN F.", "-1.");    # sin(3p/2) = -1

note "Test FCOS";
forth_ok("0e FCOS F.", "1.");               # cos(0) = 1
forth_ok("0.5235987756e FCOS F.", "0.866025");   # cos(p/6) ˜ v3/2
forth_ok("0.7853981634e FCOS F.", "0.707107");   # cos(p/4) ˜ v2/2
forth_ok("1.5707963268e FCOS F.", "0.");         # cos(p/2) = 0
forth_ok("3.1415926536e FCOS F.", "-1.");        # cos(p) = -1
forth_ok("4.7123889804e FCOS F.", "0.");         # cos(3p/2) = 0

note "Test FTAN";
forth_ok("0e FTAN F.", "0.");               # tan(0) = 0
forth_ok("0.5235987756e FTAN F.", "0.57735");   # tan(p/6) ˜ 1/v3
forth_ok("0.7853981634e FTAN F.", "1.");        # tan(p/4) = 1
forth_ok("3.1415926536e FTAN F.", "0.");        # tan(p) = 0

note "Test FASIN";
forth_ok("0e FASIN F.", "0.");               # asin(0) = 0
forth_ok("0.5e FASIN F.", "0.523599");       # asin(0.5) ˜ p/6
forth_ok("0.7071067812e FASIN F.", "0.785398");  # asin(v2/2) ˜ p/4
forth_ok("1e FASIN F.", "1.570796");         # asin(1) = p/2
forth_ok("-1e FASIN F.", "-1.570796");       # asin(-1) = -p/2

note "Test FACOS";
forth_ok("1e FACOS F.", "0.");               # acos(1) = 0
forth_ok("0.8660254038e FACOS F.", "0.523599");   # acos(v3/2) ˜ p/6
forth_ok("0.7071067812e FACOS F.", "0.785398");   # acos(v2/2) ˜ p/4
forth_ok("0e FACOS F.", "1.570796");         # acos(0) = p/2
forth_ok("-1e FACOS F.", "3.141593");        # acos(-1) = p

note "Test FATAN";
forth_ok("0e FATAN F.", "0.");               # atan(0) = 0
forth_ok("0.5773502692e FATAN F.", "0.523599");   # atan(1/v3) ˜ p/6
forth_ok("1e FATAN F.", "0.785398");         # atan(1) = p/4
forth_ok("100e FATAN F.", "1.560797");       # atan(100) ˜ close to p/2
forth_ok("-1e FATAN F.", "-0.785398");       # atan(-1) = -p/4

note "Test FSINH";
forth_ok("0e FSINH F.", "0.");               # sinh(0) = 0
forth_ok("1e FSINH F.", "1.175201");         # sinh(1) ˜ 1.175201
forth_ok("-1e FSINH F.", "-1.175201");       # sinh(-1) ˜ -1.175201
forth_ok("2e FSINH F.", "3.62686");          # sinh(2) ˜ 3.626860

note "Test FCOSH";
forth_ok("0e FCOSH F.", "1.");               # cosh(0) = 1
forth_ok("1e FCOSH F.", "1.543081");         # cosh(1) ˜ 1.543081
forth_ok("2e FCOSH F.", "3.762196");         # cosh(2) ˜ 3.762196

note "Test FTANH";
forth_ok("0e FTANH F.", "0.");               # tanh(0) = 0
forth_ok("1e FTANH F.", "0.761594");         # tanh(1) ˜ 0.761594
forth_ok("-1e FTANH F.", "-0.761594");       # tanh(-1) ˜ -0.761594
forth_ok("2e FTANH F.", "0.964028");         # tanh(2) ˜ 0.964028

note "Test FASINH";
forth_ok("0e FASINH F.", "0.");               # asinh(0) = 0
forth_ok("1e FASINH F.", "0.881374");         # asinh(1) ˜ 0.881374
forth_ok("-1e FASINH F.", "-0.881374");       # asinh(-1) ˜ -0.881374
forth_ok("2e FASINH F.", "1.443635");         # asinh(2) ˜ 1.443635

note "Test FACOSH";
forth_ok("1e FACOSH F.", "0.");               # acosh(1) = 0
forth_ok("2e FACOSH F.", "1.316958");         # acosh(2) ˜ 1.316958
forth_ok("3e FACOSH F.", "1.762747");         # acosh(3) ˜ 1.762747

note "Test FATANH";
forth_ok("0e FATANH F.", "0.");               # atanh(0) = 0
forth_ok("0.5e FATANH F.", "0.549306");       # atanh(0.5) ˜ 0.549306
forth_ok("-0.5e FATANH F.", "-0.549306");     # atanh(-0.5) ˜ -0.549306
forth_ok("0.9e FATANH F.", "1.472219");       # atanh(0.9) ˜ 1.472219

note "Test FATAN2";
forth_ok("0e 1e FATAN2 F.", "0.");               # atan2(0, 1) = 0
forth_ok("1e 0e FATAN2 F.", "1.570796");         # atan2(1, 0) = p/2
forth_ok("0e -1e FATAN2 F.", "3.141593");        # atan2(0, -1) = p
forth_ok("-1e 0e FATAN2 F.", "-1.570796");       # atan2(-1, 0) = -p/2
forth_ok("1e 1e FATAN2 F.", "0.785398");         # atan2(1, 1) = p/4
forth_ok("1e -1e FATAN2 F.", "2.356194");        # atan2(1, -1) = 3p/4
forth_ok("-1e -1e FATAN2 F.", "-2.356194");      # atan2(-1, -1) = -3p/4
forth_ok("-1e 1e FATAN2 F.", "-0.785398");       # atan2(-1, 1) = -p/4

note "Test FSINCOS";
forth_ok("0e FSINCOS .FS", "(F: 0 1 )");               # sin(0) = 0, cos(0) = 1
forth_ok("0.5235987756e FSINCOS .FS", "(F: 0.5 0.866025 )");   # sin(p/6), cos(p/6)
forth_ok("0.7853981634e FSINCOS .FS", "(F: 0.707107 0.707107 )"); # sin(p/4), cos(p/4)
forth_ok("1.5707963268e FSINCOS .FS", "(F: 1 0 )");            # sin(p/2), cos(p/2)
forth_ok("3.1415926536e FSINCOS .FS", "(F: 0 -1 )");           # sin(p), cos(p)
forth_ok("4.7123889804e FSINCOS .FS", "(F: -1 0 )");           # sin(3p/2), cos(3p/2)

note "Test FLOG";
forth_ok("1e FLOG F.", "0.");                  # log10(1) = 0
forth_ok("10e FLOG F.", "1.");                 # log10(10) = 1
forth_ok("100e FLOG F.", "2.");                # log10(100) = 2
forth_ok("1000e FLOG F.", "3.");               # log10(1000) = 3
forth_ok("0.1e FLOG F.", "-1.");               # log10(0.1) = -1
forth_ok("0.01e FLOG F.", "-2.");              # log10(0.01) = -2
forth_ok("2e FLOG F.", "0.30103");             # log10(2) ˜ 0.301030
forth_ok("0.5e FLOG F.", "-0.30103");          # log10(0.5) ˜ -0.301030

note "Test FALOG";
forth_ok("-2e FALOG F.", "0.01");
forth_ok("-1e FALOG F.", "0.1");
forth_ok("0e FALOG F.", "1.");
forth_ok("1e FALOG F.", "10.");
forth_ok("2e FALOG F.", "100.");

note "Test FEXP";
forth_ok("0e FEXP F.", "1.");                  # e^0 = 1
forth_ok("1e FEXP F.", "2.718282");            # e^1 ˜ 2.718282
forth_ok("2e FEXP F.", "7.389056");            # e^2 ˜ 7.389056
forth_ok("-1e FEXP F.", "0.367879");           # e^-1 ˜ 0.367879
forth_ok("3e FEXP F.", "20.085537");           # e^3 ˜ 20.085537

note "Test FLN";
forth_ok("1e FLN F.", "0.");                   # ln(1) = 0
forth_ok("2.718281828e FLN F.", "1.");         # ln(e) = 1
forth_ok("7.389056e FLN F.", "2.");            # ln(e^2) = 2
forth_ok("0.367879e FLN F.", "-1.000001");     # ln(e^-1) = -1
forth_ok("20.085537e FLN F.", "3.");           # ln(e^3) = 3

note "Test FEXPM1";
forth_ok("0e FEXPM1 F.", "0.");                  # e^0 - 1 = 0
forth_ok("1e FEXPM1 F.", "1.718282");            # e^1 - 1 ˜ 1.718282
forth_ok("2e FEXPM1 F.", "6.389056");            # e^2 - 1 ˜ 6.389056
forth_ok("-1e FEXPM1 F.", "-0.632121");          # e^-1 - 1 ˜ -0.632121
forth_ok("0.000001e FEXPM1 F.", "0.000001");     # small x: e^x - 1 ˜ x
forth_ok("-0.000001e FEXPM1 F.", "-0.000001");   # small negative x: e^x - 1 ˜ x

note "Test FLNP1";
forth_ok("0e FLNP1 F.", "0.");                  # ln(1 + 0) = ln(1) = 0
forth_ok("1e FLNP1 F.", "0.693147");            # ln(1 + 1) = ln(2) ˜ 0.693147
forth_ok("2e FLNP1 F.", "1.098612");            # ln(3) ˜ 1.098612
forth_ok("-0.5e FLNP1 F.", "-0.693147");        # ln(0.5) ˜ -0.693147
forth_ok("0.000001e FLNP1 F.", "0.000001");     # small x: ln(1 + x) ˜ x
forth_ok("-0.000001e FLNP1 F.", "-0.000001");   # small negative x: ln(1 + x) ˜ x

note "Test FSQRT";
forth_ok("0e FSQRT .FS", "(F: 0 )");               # v0 = 0
forth_ok("1e FSQRT .FS", "(F: 1 )");               # v1 = 1
forth_ok("4e FSQRT .FS", "(F: 2 )");               # v4 = 2
forth_ok("9e FSQRT .FS", "(F: 3 )");               # v9 = 3
forth_ok("2e FSQRT .FS", "(F: 1.41421 )");         # v2 ˜ 1.414214
forth_ok("0.25e FSQRT .FS", "(F: 0.5 )");          # v0.25 = 0.5
forth_ok("10000e FSQRT .FS", "(F: 100 )");         # v10000 = 100

note "Test FTRUNC";
forth_ok("0e FTRUNC .FS", "(F: 0 )");               # trunc(0) = 0
forth_ok("1.9999e FTRUNC .FS", "(F: 1 )");          # trunc(1.9999) = 1
forth_ok("-1.9999e FTRUNC .FS", "(F: -1 )");        # trunc(-1.9999) = -1
forth_ok("123.456e FTRUNC .FS", "(F: 123 )");       # trunc(123.456) = 123
forth_ok("-123.456e FTRUNC .FS", "(F: -123 )");     # trunc(-123.456) = -123
forth_ok("2e FTRUNC .FS", "(F: 2 )");               # trunc(2) = 2
forth_ok("-2e FTRUNC .FS", "(F: -2 )");             # trunc(-2) = -2

note "Test F~";

# Positive tolerance: |r1 - r2| < r3
forth_ok("1.00001e  1.0e 0.00001e F~ .", "0");     # difference too large
forth_ok("1.000001e 1.0e 0.0001e  F~ .", "-1");    # within tolerance

# Zero tolerance: exact encoding match
forth_ok("0e 0e 0e F~ .", "-1");                   # +0 == +0
forth_ok("0e -0e 0e F~ .", "0");                   # +0 ? -0 (distinct encodings)
forth_ok("1e 1e 0e F~ .", "-1");                   # exact match
forth_ok("1e 1.0000000001e 0e F~ .", "0");         # not exactly equal

# Negative tolerance: relative comparison
forth_ok("1.000001e 1.0e -0.00001e F~ .", "-1");    # relative difference OK
forth_ok("1.000001e 1.0e -0.0000001e F~ .", "0");   # relative difference too large
forth_ok("100e 99e -0.1e F~ .", "-1");              # small relative tolerance OK
forth_ok("0e 1e -0.0001e F~ .", "0");               # large relative difference

note "Test PRECISION";
note "Test SET-PRECISION";
forth_ok("PRECISION .S", "( 7 )");
forth_ok("15 SET-PRECISION PRECISION .S", "( 15 )");
forth_ok("6  SET-PRECISION 10e 3e F/       F.", "3.33333");
forth_ok("12 SET-PRECISION 10e 3e F/       F.", "3.33333333333");
forth_ok("6  SET-PRECISION 10e 3e F/ 2e F* F.", "6.66667");
forth_ok("12 SET-PRECISION 10e 3e F/ 2e F* F.", "6.66666666667");

end_test;
