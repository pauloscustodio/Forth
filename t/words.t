#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test WORDS";

forth_ok("words", <<'END');
QUIT INCLUDED DU>= DU> DU<= DU< M+ M*/ DNEGATE DMIN DMAX DABS D>S D>= D> D<= D< 
D<> D= D2/ D2* D0>= D0> D0<= D0< D0<> D0= D- D+ THRU LIST UPDATE LOAD FLUSH 
EMPTY-BUFFERS SAVE-BUFFERS BUFFER BLOCK SCR BLK ENVIRONMENT? WORDS .RS .S 
ABORT" ABORT BYE ENDCASE ENDOF OF CASE RECURSE REPEAT WHILE UNTIL AGAIN BEGIN 
UNLOOP LEAVE +LOOP LOOP ?DO DO THEN ELSE IF [COMPILE] COMPILE, IMMEDIATE 
POSTPONE ['] ' \ ( ] [ DOES> 2LITERAL LITERAL 2CONSTANT CONSTANT TO VALUE 
2VARIABLE VARIABLE CREATE ; :NONAME : EXIT EXECUTE EVALUATE INTERPRET U.R .R U. 
D.R D. . #> SIGN HOLD #S # <# SPACES SPACE CR EMIT TYPE RESTORE-INPUT 
SAVE-INPUT QUERY KEY EXPECT ACCEPT REFILL SOURCE-ID SOURCE SPAN >IN #TIB #IN 
TIB .( C" S" ." COUNT CONVERT >NUMBER NUMBER NUMBER? DPL [CHAR] CHAR WORD 
PARSE-WORD PARSE MARKER UNUSED ALLOT ALIGNED ALIGN >BODY FIND LATEST HERE C, , 
2R@ 2R> 2>R J I R@ RSP! RSP@ RDROP R> >R SP! SP@ -2ROT 2ROT 2OVER 2DUP 2SWAP 
2DROP TUCK ROLL PICK NIP DEPTH -ROT ROT OVER ?DUP DUP SWAP DROP MOVE ERASE FILL 
2@ 2! C@ C! +! @ ! 0>= 0> 0<= 0< 0<> 0= U>= U> U<= U< >= > <= < <> = RSHIFT 
LSHIFT INVERT XOR OR AND WITHIN CELLS CELL+ CHARS CHAR+ MIN MAX ABS UM* S>D 
NEGATE 2/ 2* 1- 1+ M* SM/REM UM/MOD FM/MOD */MOD */ /MOD MOD / - * + HEX 
DECIMAL BASE STATE TRUE FALSE PAD R0 S0 BL 
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
