#!/usr/bin/env perl

#------------------------------------------------------------------------------
# C++ implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2025
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

use Modern::Perl;
use Path::Tiny;
use Text::Wrap; $Text::Wrap::columns = 76;

# get implemented words
my $words = `./forth -e words`;
my @words = sort split ' ', $words;
my %words; $words{$_} = 1 for @words;

# get standard wordsets and words
my %wordsets;
my %not_standard_words = %words;
@ARGV = <t/Forth2012-words.txt>;
while (<>) {
	next unless /[0-9.]+\s+(\S+).*?([A-Z]+.*)/;
	my($chapter, $word, $reading, $wordset) = split(/\t/, $_);
	for ($chapter, $word, $reading, $wordset) {
		s/^\s+//; s/\s+$//;
	}
	$wordsets{$wordset}{$word} = 1;
	delete $not_standard_words{$word};
}

# show implemented and missing words per wordset
my $out = "";
for my $wordset (sort keys %wordsets) {
	$out .= $wordset . ":\n";
	my @exist;
	my @missing;
	for my $word (sort keys %{$wordsets{$wordset}}) {
		if (exists $words{$word}) {
			push @exist, $word;
		}
		else {
			push @missing, $word;
		}
	}
	$out .= "  Implemented:\n".wrap("    ", "    ", "@exist")."\n" if @exist;
	$out .= "  Missing:\n".wrap("    ", "    ", "@missing")."\n" if @missing;
	$out .= "\n";
}
$out .= "NOT STANDARD:\n  Implemented:\n".
		wrap("    ", "    ", join(" ", sort keys %not_standard_words))."\n";

# patch README
my $readme = path("README.md")->slurp;
$readme =~ s/```.*```/```\n$out```/s;
path("README.md")->spew($readme);
