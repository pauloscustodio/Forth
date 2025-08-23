#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WORDS";

forth_ok("words", <<'END');
INCLUDED PAGE KEY? KEY AT-XY ABORT" ABORT CATCH THROW DNEGATE DMIN DMAX DABS
D>S D0>= D0> D0<= D0< D0<> D0= DU>= DU> DU<= DU< D>= D> D<= D< D<> D= M+ M*/
D2/ D2* D- D+ 2LITERAL 2VARIABLE 2CONSTANT OFF ON SEE DUMP ENVIRONMENT? WORDS
.RS .S THRU LIST UPDATE LOAD FLUSH EMPTY-BUFFERS SAVE-BUFFERS BUFFER BLOCK SCR
BLK BYE QUIT ENDCASE ENDOF OF CASE RECURSE REPEAT WHILE UNTIL AGAIN BEGIN
UNLOOP LEAVE +LOOP LOOP ?DO DO THEN ELSE IF \ ( IS ACTION-OF DEFER! DEFER@
DEFER [COMPILE] COMPILE, IMMEDIATE POSTPONE DOES> LITERAL CONSTANT TO 2VALUE
VALUE BUFFER: VARIABLE CREATE ['] ' ] [ ; :NONAME : STATE EXIT EXECUTE EVALUATE
INTERPRET TRACE U.R .R U. D.R D. ? . #> SIGN HOLDS HOLD #S # <# SPACES SPACE CR
EMIT TYPE RESTORE-INPUT SAVE-INPUT QUERY EXPECT SPAN ACCEPT REFILL SOURCE-ID
#TIB TIB SOURCE #IN >IN .( C" S\" S" ." COUNT CONVERT >NUMBER NUMBER NUMBER?
DPL [CHAR] CHAR PARSE-NAME PARSE-WORD PARSE WORD MARKER UNUSED ALLOT ALIGNED
ALIGN >BODY FIND LATEST HERE C, , RSP! RSP@ R0 RDROP 2R@ 2R> 2>R J I R@ R> >R
SP! SP@ S0 -2ROT 2ROT 2OVER 2DUP 2SWAP 2DROP TUCK ROLL PICK NIP DEPTH -ROT ROT
OVER ?DUP DUP SWAP DROP MOVE ERASE FILL 2@ 2! C@ C! +! @ ! 0>= 0<= 0> 0< 0<> 0=
U>= U<= U> U< >= <= > < <> = RSHIFT LSHIFT INVERT XOR OR AND WITHIN CELLS CELL+
CHARS CHAR+ MIN MAX ABS UM* S>D NEGATE 2/ 2* 1- 1+ M* SM/REM UM/MOD FM/MOD
*/MOD */ /MOD MOD / - * + HEX DECIMAL BASE TRUE FALSE PAD BL
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
	$word =~ s/\\\\/\\/;
	ok exists $words{$word}, "$word tested";
	delete $untested{$word};
}

ok %untested==0, "all words tested";
for (sort keys %untested) {
	diag "$_ not tested";
}

# show ANS words that are not implemented
# ,['FILE' => 0], ['FILE EXT' => 0]
for (['CORE' => 1], ['CORE EXT' => 1], 
	 ['BLOCK' => 1], ['BLOCK EXT' => 1],
     ['DOUBLE' => 1], ['DOUBLE EXT' => 1], 
	 ['EXCEPTION' => 1], ['EXCEPTION EXT' => 1],
	 ['FACILITY' => 1]) {
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

end_test;
