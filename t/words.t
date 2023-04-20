#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WORDS";

# check that all words are tested
forth_ok("words", <<'END');
BL FALSE TRUE PAD STATE BASE DPL DECIMAL HEX + * - / MOD /MOD */ */MOD FM/MOD
UM/MOD SM/REM M* 1+ 1- 2* 2/ NEGATE S>D UM* ABS MAX MIN CHAR+ CHARS CELL+ CELLS
WITHIN AND OR XOR INVERT LSHIFT RSHIFT = <> < <= > >= U< U<= U> U>= 0= 0<> 0<
0<= 0> 0>= ! @ +! C! C@ 2! 2@ FILL ERASE MOVE DROP SWAP DUP ?DUP OVER ROT -ROT
NIP PICK ROLL TUCK 2DROP 2SWAP 2DUP 2OVER 2ROT -2ROT S0 SP@ SP! DEPTH >R R>
RDROP R@ I J 2>R 2R> 2R@ R0 RSP@ RSP! , C, HERE LATEST COUNT BYE THROW .S .RS
WORDS DUMP TYPE .
END
my %words; 
$words{$_}=1 for sort split(' ', slurp("${test}.out"));

my %untested = %words;
@ARGV = <t/*.t>;
while (<>) {
	next unless /^note ['"]Test (.*)['"];/;
	my $word = $1;
	$word = '\\' if $word eq '\\\\';
	ok exists $words{$word}, "$word exists";
	delete $untested{$word};
}

ok %untested==0, "all words tested";
for (sort keys %untested) {
	diag "$_ not tested";
}

# show ANS words not implemented
for (['CORE' => 1], ['CORE EXT' => 1],
	 ['BLOCK' => 1], ['BLOCK EXT' => 1],
	 ['DOUBLE' => 1], ['DOUBLE EXT' => 1],
	 ['FILE' => 0], ['FILE EXT' => 0]) {
	my($wordset, $included) = @$_;
	my $env_query = $wordset =~ s/ /-/gr;
	
	my @pending;
	@ARGV = <t/DPANS94.txt>;
	while (<>) {
		next unless /^\s+[0-9.]+\s+(\S+).*?([A-Z]+.*)/;
		my($word, $word_wordset) = ($1, $2);
		$word =~ s/environment-query$//;
		if ($word_wordset eq $wordset && !exists $words{$word}) {
			push(@pending, $word);
		}
	}
	
	if ($included) {
		forth_ok(qq(S" $env_query" ENVIRONMENT? .S), "( -1 -1 )");
		ok 0==@pending, "$wordset not implemented: @pending";
	}
	else {
		forth_ok(qq(S" $env_query" ENVIRONMENT? .S), "( 0 -1 )");
		note "$wordset not implemented: @pending";
	}
}

end_test;
