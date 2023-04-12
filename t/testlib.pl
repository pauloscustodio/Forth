#------------------------------------------------------------------------------
# C implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2022
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
	
	spew("$test.fs", $fth);
	capture_ok("./forth $test.fs", $exp_out);
	check_die();
}

sub forth_nok {
	my($fth, $exp_err) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	spew("$test.fs", $fth);
	capture_nok("./forth $test.fs", $exp_err);
	check_die();
}

sub capture_ok {
	my($cmd, $exp_out) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	spew("$test.exp", $exp_out);
	run_ok("$cmd > $test.out");
	run_ok("dos2unix -q $test.exp $test.out");
	run_ok("diff $test.exp $test.out");
	check_die();
}

sub capture_nok {
	my($cmd, $exp_err) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1; 
	
	spew("$test.exp", $exp_err);
	run_nok("$cmd 2> $test.err");
	run_ok("dos2unix -q $test.exp $test.err");
	run_ok("diff $test.exp $test.err");
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

#------------------------------------------------------------------------------
# path()->spew fails sometimes on Windows (race condition?) with 
# Error rename on 'test_t_ALIGN.asm37032647357911' -> 'test_t_ALIGN.asm': Permission denied
# replace by a simpler spew without renames
sub spew {
	my($file, @data) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1;

	my $open_ok = open(my $fh, ">:raw", $file);
	ok $open_ok, "write $file"; 
	
	if ($open_ok) {
		print $fh join('', @data);
	}
}

#------------------------------------------------------------------------------
# and for simetry
sub slurp {
	my($file) = @_;
	local $Test::Builder::Level = $Test::Builder::Level + 1;

	my $open_ok = open(my $fh, "<:raw", $file);
	ok $open_ok, "read $file";
	
	if ($open_ok) {
		read($fh, my $data, -s $file);
		return $data;
	}
	else {
		return "";
	}
}


1;
