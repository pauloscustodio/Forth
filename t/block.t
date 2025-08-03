#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

my $block_file = "forth.blk";

note 'Test LOAD';
note 'Test BLK';
unlink $block_file;
write_block(1, <<END);
1 2  BLK @  2 LOAD
END
write_block(2, <<END);
3 4  BLK @
END
forth_ok("1 LOAD .S", "( 1 2 1 3 4 2 )");


unlink $block_file;
end_test;

sub write_block {
	my($blk, $text) = @_;
	$text =~ s/\s/ /g;
	die "$text too long" if length($text) > 1024;
	$text .= " " x (1024-length($text));
	
	my $offset = $blk * 1024;
	
	my $fh;
	if (-f $block_file) {
		open($fh, "+<", $block_file) or die "open $block_file: $!";
	}
	else {
		open($fh, "+>", $block_file) or die "create $block_file: $!";
	}
	
	seek($fh, $offset, 0) or die "Seek failed: $!";
	my $pos = tell($fh);
	print $fh  $text or die "print failed: $!";
	close $fh;
}
