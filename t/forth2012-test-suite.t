#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

chdir "t/forth2012-test-suite/src" or die;

note "prelimtest.fth";
path("$test.fs")->spew('S" prelimtest.fth" INCLUDED');
run_ok("../../../forth $test.fs > $test.out");
$out = path("$test.out")->slurp;
# diag $out;
for (1..23) {
	like $out, qr/Pass #$_:/, "Pass $_";
}
like $out, qr/0 tests failed/, "0 tests failed";

note "runtests.fth";
path("$test.fs")->spew('S" runtests.fth" INCLUDED');
run_ok("echo hello | ../../../forth $test.fs > $test.out");
$out = path("$test.out")->slurp;
#diag $out;
unlike $out, qr/INCORRECT RESULT/;
unlike $out, qr/WRONG NUMBER OF RESULTS/;
like $out, qr/YOU SHOULD SEE THE STANDARD GRAPHIC CHARACTERS:\s+!"#\$\%&'\(\)\*\+,-\.\/0123456789:;<=>\?\@\s+ABCDEFGHIJKLMNOPQRSTUVWXYZ\[\\\]\^_`\s+abcdefghijklmnopqrstuvwxyz\{\|\}~/;
like $out, qr/YOU SHOULD SEE 0-9 SEPARATED BY A SPACE:\s+0 1 2 3 4 5 6 7 8 9 /;
like $out, qr/YOU SHOULD SEE 0-9 \(WITH NO SPACES\):\s+0123456789/;
like $out, qr/YOU SHOULD SEE A-G SEPARATED BY A SPACE:\s+A B C D E F G /;
like $out, qr/YOU SHOULD SEE 0-5 SEPARATED BY TWO SPACES:\s+0  1  2  3  4  5  /;
like $out, qr/YOU SHOULD SEE TWO SEPARATE LINES:\s+LINE 1\s+LINE 2/;
like $out, qr/YOU SHOULD SEE THE NUMBER RANGES OF SIGNED AND UNSIGNED NUMBERS:\s+  SIGNED: -80000000 7FFFFFFF \s+UNSIGNED: 0 FFFFFFFF /;
like $out, qr/PLEASE TYPE UP TO 80 CHARACTERS:\s+RECEIVED: "hello"/;

end_test;
