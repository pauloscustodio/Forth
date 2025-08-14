#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test [COMPILE]";
my $func = <<END;
	: ENDIF [COMPILE] THEN ; IMMEDIATE  
	: x IF 1 . ELSE 2 . ENDIF ;
END
forth_ok("$func  0 x", "2 ");
forth_ok("$func -1 x", "1 ");

end_test;
