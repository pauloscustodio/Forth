#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# from John Hayes tests
forth_ok(<<'END', "( 11111 0 22222 )");
	VARIABLE SIV TRUE SIV !

	: NEVEREXECUTED
	   CR ." This should never be executed" CR
	;

	11111 SAVE-INPUT

	SIV @

	[IF]
		0 SIV !
		RESTORE-INPUT
		NEVEREXECUTED
		33333
	[ELSE]
		22222
	[THEN] [THEN] \ -[IF]- is passed twice, need two -[THEN]-s
	.S
END

end_test;
