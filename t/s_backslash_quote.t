#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note 'Test S\\"';
forth_ok(<<'END', <<'END');
S\" hello \a \b \e \f \l \m \n \q \r \t \v \z \" \xff \\" DUMP
END

000f3800                                      68 65 6c 6c               hell
000f3810  6f 20 07 20 08 20 1b 20 0c 20 0a 20 0d 0a 20 0a   o . . . . . .. .
000f3820  20 22 20 0d 20 09 20 0b 20 00 20 22 20 ff 20 5c    " . . . . " . \
END

end_test;
