#!/usr/bin/env perl

#------------------------------------------------------------------------------
# C++ implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2025
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

use Modern::Perl;
use Array::Compare;
use Path::Tiny;
use File::Copy;
use Text::CSV;
use Data::Dump 'dump';

#-------------------------------------------------------------------------------
# main
#-------------------------------------------------------------------------------

@ARGV or die "Usage: ",path($0)->basename," files.cpp...\n";
my(@source_files) = @ARGV;

my @errors = parse_errors("errors.def");
my @words = parse_words("words.def");

for my $file (@source_files) {
	patch_file($file);
}

#-------------------------------------------------------------------------------
# patch the file with grammar
#-------------------------------------------------------------------------------
sub patch_file {
	my($file) = @_;
	my @in = path($file)->lines;
	my @in0 = @in;
	my @out;
	
	while (@in) {
		$_ = shift(@in);
		if (/^(\s*)\/\/\@\@BEGIN:\s*Error\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $error (@errors) {
				push @out, $prefix.$error->{id}." = ".$error->{code}.
						", // ".$error->{text}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*ErrorMessage\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $error (@errors) {
				push @out, $prefix."case Error::".$error->{id}.": return ".
						c_string($error->{text})."; // ".$error->{code}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*Words\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $word (reverse @words) {
				push @out, $prefix.c_string($word->{name}).",\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsDeclarations\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $word (@words) {
				push @out, $prefix."void ".$word->{id}."(); // ".$word->{name}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsImplementation\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $word (reverse @words) {
				push @out, $prefix."else if (case_insensitive_equal(word, ".
					c_string($word->{name}).")) { ".$word->{id}."(); }\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		else {
			push @out, $_;
		}
	}
	
	# change template if needed
	my $ac = Array::Compare->new;
	if (!$ac->compare(\@in0, \@out)) {
		copy($file, "$file.bak") or die "copy to $file.bak failed\n";
		path($file)->spew(@out);
		system("dos2unix", $file)==0 or die "dos2unix failed: $!";
	}
}

sub c_string {
	my($text) = @_;
	$text =~ s/([\\"])/\\$1/g;
	$text =~ s/\n/\\n/g;
	return '"'.$text.'"';
}

#-------------------------------------------------------------------------------
# parse errors.def
#-------------------------------------------------------------------------------
sub parse_errors {
	my($filename) = @_;
	my @errors;
	
	my $csv = Text::CSV->new({ 	auto_diag => 1, 
								allow_whitespace => 1,
								skip_empty_rows => 1,
								});
	open my $fh, "<", $filename or die "$filename: $!";
	while (my $row = $csv->getline($fh)) {
		next if $row->[0] =~ /^\/\//;
		my ($code, $id, $text) = @$row;
		push @errors, {code=>$code, id=>$id, text=>$text};
	}
	close $fh;
	
	return @errors;
}

#-------------------------------------------------------------------------------
# parse words.def
#-------------------------------------------------------------------------------
sub parse_words {
	my($filename) = @_;
	my @words;
	
	my $csv = Text::CSV->new({ 	auto_diag => 1, 
								allow_whitespace => 1,
								skip_empty_rows => 1,
								});
	open my $fh, "<", $filename or die "$filename: $!";
	while (my $row = $csv->getline($fh)) {
		next if $row->[0] =~ /^\/\//;
		my ($name, $id, $flags) = @$row;
		push @words, {name=>$name, id=>$id, flags=>$flags};
	}
	close $fh;
	
	return @words;
}
