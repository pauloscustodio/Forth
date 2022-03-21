#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

chdir "t/forth2012-test-suite/src" or die;

note "Test INCLUDED";

note "prelimtest.fth";
path("$test.fs")->spew('S" prelimtest.fth" INCLUDED');
run_ok("../../../forth $test.fs > $test.out");
$out = path("$test.out")->slurp;
#diag $out;
for (1..23) {
	like $out, qr/Pass #$_:/, "Pass $_";
}
like $out, qr/0 tests failed/, "0 tests failed";

unlink "$test.fs", "$test.out";
done_testing;
