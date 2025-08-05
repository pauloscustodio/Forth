#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WORDS";

my $expected_words = path("README.md")->slurp;
$expected_words =~ s/.*?```\n//s;
$expected_words =~ s/\n```.*//s;

forth_ok("words", $expected_words);
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
	ok exists $words{$word}, "$word tested";
	delete $untested{$word};
}

ok %untested==0, "all words tested";
for (sort keys %untested) {
	diag "$_ not tested";
}

# show ANS words that are not implemented
# ,['BLOCK' => 0], ['BLOCK EXT' => 0],['DOUBLE' => 0], ['DOUBLE EXT' => 0],['FILE' => 0], ['FILE EXT' => 0]
for (['CORE' => 1], ['CORE EXT' => 1]) {
	my($wordset, $included) = @$_;
	my $env_query = $wordset =~ s/ /-/gr;
	
	my @pending;
	@ARGV = <t/Forth2012-words.txt>;
	while (<>) {
		next unless /[0-9.]+\s+(\S+).*?([A-Z]+.*)/;
		my($chapter, $word, $reading, $word_wordset) = split(/\t/, $_);
		for ($chapter, $word, $reading, $word_wordset) {
			s/^\s+//; s/\s+$//;
		}

		if ($word_wordset eq $wordset && !exists $words{$word}) {
			push(@pending, $word);
		}
	}
	
	if ($included) {
		forth_ok("BL WORD $env_query COUNT ENVIRONMENT? .S", "( -1 -1 )");
		ok 0==@pending, "$wordset not implemented: @pending";
	}
	else {
		forth_ok("BL WORD $env_query COUNT ENVIRONMENT? .S", "( 0 -1 )");
		note "$wordset not implemented: @pending";
	}
}

# show implemented/not-implemented words
my %wordsets;
my %not_standard_words = %words;
@ARGV = <t/Forth2012-words.txt>;
while (<>) {
	next unless /[0-9.]+\s+(\S+).*?([A-Z]+.*)/;
	my($chapter, $word, $reading, $word_wordset) = split(/\t/, $_);
	for ($chapter, $word, $reading, $wordset) {
		s/^\s+//; s/\s+$//;
	}
	$wordsets{$word_wordset}{$word} = 1;
	delete $not_standard_words{$word}
}

for my $wordset (sort keys %wordsets) {
	note $wordset;
	for my $word (sort keys %{$wordsets{$wordset}}) {
		my $defined = exists $words{$word} ? "T" : "-";
		note "$defined\t$word";
	}
}

note "Not standard words:";
for my $word (sort keys %not_standard_words) {
	note "\t$word";
}

end_test;
