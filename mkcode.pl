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
my @vars = parse_vars("vars.def");
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
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*Vars\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."int ".$var->{id}.";\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*VarsInit\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix.$var->{id}." = ".$var->{value}.";\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*VarsImplementation\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."void f".$var->{id}."() ".
						"{ push(vm.mem.addr(&vm.user->".$var->{id}.")); }\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsXtDeclaration\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."extern int xt".$var->{id}."; // ".$var->{name}."\n";
			}
			for my $word (@words) {
				push @out, $prefix."extern int xt".$word->{id}."; // ".$word->{name}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsXtDefinition\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."int xt".$var->{id}." = 0; // ".$var->{name}."\n";
			}
			for my $word (@words) {
				push @out, $prefix."int xt".$word->{id}." = 0; // ".$word->{name}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsDeclaration\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."void f".$var->{id}."(); // ".$var->{name}."\n";
			}
			for my $word (@words) {
				push @out, $prefix."void f".$word->{id}."(); // ".$word->{name}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsCreateDictionary\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."xt".$var->{id}." = create(".c_string($var->{name}).", 0, ".
							"id".$var->{id}.");\n";
			}
			for my $word (@words) {
				push @out, $prefix."xt".$word->{id}." = create(".c_string($word->{name}).", ".
							$word->{flags}.", id".$word->{id}.");\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsIdDeclaration\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."id".$var->{id}.", // ".$var->{name}."\n";
			}
			for my $word (@words) {
				push @out, $prefix."id".$word->{id}.", // ".$word->{name}."\n";
			}
			while (@in && $in[0] !~ /^\s*\/\/\@\@END/) {
				shift @in;
			}
		}
		elsif (/^(\s*)\/\/\@\@BEGIN:\s*WordsIdExecution\b/) {
			my $prefix = $1;
			push @out, $_;
			for my $var (@vars) {
				push @out, $prefix."case id".$var->{id}.": f".$var->{id}."(); break; // ".$var->{name}."\n";
			}
			for my $word (@words) {
				push @out, $prefix."case id".$word->{id}.": f".$word->{id}."(); break; // ".$word->{name}."\n";
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
# parse vars.def
#-------------------------------------------------------------------------------
sub parse_vars {
	my($filename) = @_;
	my @vars;
	
	my $csv = Text::CSV->new({ 	auto_diag => 1, 
								allow_whitespace => 1,
								skip_empty_rows => 1,
								});
	open my $fh, "<", $filename or die "$filename: $!";
	while (my $row = $csv->getline($fh)) {
		next if $row->[0] =~ /^\/\//;
		my ($name, $id, $value) = @$row;
		push @vars, {name=>$name, id=>$id, value=>$value};
	}
	close $fh;
	
	return @vars;
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
