#!/usr/bin/perl

# Tools to manage blocks.fb

use Modern::Perl;

my $BLOCKS = "blocks.fb";
my $BLOCKS_TEXT = "blocks.txt";
my $BLOCK_SZ = 1024;
my $ROWS = 16;
my $COLS = 64;

my $FH;
if (-f $BLOCKS) {
	open($FH, "+<:raw", $BLOCKS) or die "open $BLOCKS failed: $!\n";
}
else {
	open($FH, "+>:raw", $BLOCKS) or die "create $BLOCKS failed: $!\n";
}

my $cmd = shift // "";
if ($cmd eq 'spew') {
	my $blk = shift // die "expected block number\n";
	spew($blk);
}
elsif ($cmd eq 'slurp') {
	my $blk = shift // die "expected block number\n";
	slurp($blk);
}
elsif ($cmd eq 'list') {
	if (@ARGV==0) {
		list_all();
	}
	else {
		list(shift);
	}
}
elsif ($cmd eq 'edit') {
	my $blk = shift // die "expected block number\n";
	edit($blk);
}
else {
	die <<'END';
Usage: blocks.pl cmd args
cmd:
    list [blk]  - list the given block or whole file
    spew blk    - output the given block as text
    read blk    - read STDIN into block
    edit blk    - edit given block
END
}

sub num_blocks {
	seek($FH, 0, 2) or die "seek to end failed: $!";
	my $size = tell($FH);
	my $num_blocks = $size / $BLOCK_SZ;
	return $num_blocks;
}

sub read_block {
	my($blk) = @_;
	my $fpos = $blk * 1024;
	seek($FH, $fpos, 0) or die "seek $fpos failed: $!";
	my $buffer;
	read($FH, $buffer, $BLOCK_SZ) or die "failed to read $BLOCK_SZ bytes at $fpos\n";
	if (length($buffer) != $BLOCK_SZ) {
		die "failed to read $BLOCK_SZ bytes at $fpos\n";
	}
	return $buffer;
}

sub write_block {
	my($blk, $buffer) = @_;
	if (length($buffer) != $BLOCK_SZ) {
		die "block must be $BLOCK_SZ bytes\n";
	}
	my $fpos = $blk * 1024;
	seek($FH, $fpos, 0) or die "seek $fpos failed: $!";
	print $FH $buffer or die "failed to write $BLOCK_SZ bytes at $fpos\n";
}

sub format_text_as_block {
	my($text) = @_;
	my $buffer = "";
	for my $row (0..$ROWS-1) {
		my $line;
		if ($text =~ s/(.*)\n//) {
			$line = $1;
		}
		else {
			$line = substr($text, 0, $COLS);
			if (length($text) >= $COLS) {
				$text = substr($text, $COLS);
			}
			else {
				$text = "";
			}
		}
		
		$line =~ s/\s/ /g;
		$line =~ s/\s+$//;
		if (length($line) > $COLS) {
			die "line too long: $line\n";
		}
		$line .= " " x ($COLS - length($line));
		$buffer .= $line;
	}
	$text =~ s/\s+$//s;
	if ($text ne "") {
		die "text too long: $text\n";
	}
	return $buffer;
}

sub spew {
	my($blk) = @_;
	my $buffer = read_block($blk);
	for my $row (0..$ROWS-1) {
		my $line = substr($buffer, $row*$COLS, $COLS);
		$line =~ s/\s/ /g;
		$line =~ s/\s+$//;
		say $line;
	}
}

sub slurp {
	my($blk) = @_;
	my @text = <>;
	my $text = join("", @text);
	my $buffer = format_text_as_block($text);
	write_block($blk, $buffer);
}
	
sub list {
	my($blk) = @_;
	my $buffer = read_block($blk);
	display_block($blk, $buffer);
}

sub list_all {
	my $num_blocks = num_blocks();
	for (my $blk = 1; $blk < $num_blocks; $blk++) {
		list($blk);
	}
}
	
sub display_block {
	my($blk, $buffer) = @_;
	say "Block $blk:";
	for my $row (0..$ROWS-1) {
		printf("%2d  ", $row + 1);
		for my $col (0..$COLS-1) {
			my $c = substr($buffer, $row*$COLS+$col, 1);
			if (ord($c) >= 0x20 && ord($c) < 0x7f) {
				print $c;
			}
			else {
				print "?";
			}
		}
		print "\n";
	}
	print "\n";
}

sub edit {
	my($blk) = @_;
	
	my $num_blocks = num_blocks();
	
	open(my $save_stdout, '>&STDOUT') or die "dup STDOUT: $!";
	open(STDOUT, '>', $BLOCKS_TEXT) or die "open $BLOCKS_TEXT: $!";
	if ($blk < $num_blocks) {
		spew($blk);
	}
	else {
		say "";
	}
	open(STDOUT, '>&', $save_stdout) or die "restore STDOUT: $!";
	
	system $ENV{EDITOR}, $BLOCKS_TEXT;
	
	open(my $save_stdin, '<&STDIN') or die "dup STDIN: $!";
	open(STDIN, '<', $BLOCKS_TEXT) or die "open $BLOCKS_TEXT: $!";
	slurp($blk);
	open(STDIN, '<&', $save_stdout) or die "restore STDIN: $!";
	
	list $blk;
}
