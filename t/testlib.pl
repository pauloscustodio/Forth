#------------------------------------------------------------------------------
# C implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2025
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

use Modern::Perl;
use Path::Tiny;
use Test::More;

use vars qw($test);
$test = 'test_'.(path($0)->basename) =~ s/\.\w+$//r;

sub forth_ok {
	my($fth, $exp_out) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	path("$test.fs")->spew($fth);
	capture_ok("./forth $test.fs", $exp_out);
	check_die();
}

sub forth_nok {
	my($fth, $exp_err) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	path("$test.fs")->spew($fth);
	capture_nok("./forth $test.fs", $exp_err);
	check_die();
}

sub capture_ok {
	my($cmd, $exp_out) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	path("$test.exp")->spew($exp_out);
	run_ok("$cmd > $test.out");
	run_ok("diff -w $test.exp $test.out");
	check_die();
}

sub capture_nok {
	my($cmd, $exp_err) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	path("$test.exp")->spew($exp_err);
	run_nok("$cmd 2> $test.err");
	run_ok("diff -w $test.exp $test.err");
	check_die();
}

sub run_ok {
	my($cmd) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	ok 0==system($cmd), $cmd;
	check_die();
}

sub run_nok {
	my($cmd) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	ok 0!=system($cmd), $cmd;
	check_die();
}

sub end_test {
	if (Test::More->builder->is_passing) {
		for my $ext (qw( .fs .exp .out .err )) {
			unlink($test.$ext);
		}
	}
	done_testing();
}

sub check_die {
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	die if (!Test::More->builder->is_passing && $ENV{DEBUG});
}

1;
