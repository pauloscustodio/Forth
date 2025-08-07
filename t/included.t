#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test INCLUDED";
for my $i (1..9) {
	my $j = $i-1;
	path("$test.$i.fs")->spew(<<END);
		$i 
		.
		S" $test.$j.fs" INCLUDED $i
		.
END
}
path("$test.0.fs")->spew("0 .");
capture_ok("./forth $test.9.fs", "9 8 7 6 5 4 3 2 1 0 1 2 3 4 5 6 7 8 9 ");

unlink <$test.*.fs>;
end_test;
