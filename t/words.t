#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WORDS";

forth_ok("words", <<'END');
. THROW DROP DUP PICK + .S WORDS S" TYPE ENVIRONMENT?
END
die if !Test::More->builder->is_passing;

# check that all words are tested
run_ok("./forth -e WORDS > ${test}.out");
my %words; $words{$_}=1 for sort split(' ', path("${test}.out")->slurp);

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

# show ANS words that are not implemented
for (['CORE' => 1], ['CORE EXT' => 0],
	 ['BLOCK' => 0], ['BLOCK EXT' => 0],
	 ['DOUBLE' => 0], ['DOUBLE EXT' => 0],
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
