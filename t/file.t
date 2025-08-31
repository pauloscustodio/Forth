#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test CREATE-FILE";
note "Test CLOSE-FILE";
note "Test W/O";
note "Test FILE-POSITION";

# create file
rmdir "$test.dat";
unlink "$test.dat";
forth_ok(<<END, "0 2 ( )");
S" $test.dat" W/O CREATE-FILE THROW CONSTANT file_id
file_id FILE-POSITION THROW D.
file_id CLOSE-FILE THROW
file_id .
.S
END
ok -f "$test.dat", "file created";
is -s "$test.dat", 0, "file empty";

# truncate file
path("$test.dat")->spew("Hello world");
forth_ok(<<END, "0 2 ( )");
S" $test.dat" W/O CREATE-FILE THROW CONSTANT file_id
file_id FILE-POSITION THROW D.
file_id CLOSE-FILE THROW
file_id .
.S
END
ok -f "$test.dat", "file created";
is -s "$test.dat", 0, "file empty";

# fail creation
unlink "$test.dat";
mkdir "$test.dat";
forth_nok(<<END, "\nError: CREATE-FILE exception\n");
S" $test.dat" W/O CREATE-FILE THROW .S
END
rmdir "$test.dat";

note "Test OPEN-FILE";
note "Test READ-FILE";
note "Test R/O";
note "Test FILE-POSITION";

path("$test.dat")->spew("Hello world");
forth_ok(<<END, "0 Hello!5  worl!10 d!11 !11 ( )");
5 CONSTANT buffer_size
buffer_size BUFFER: buffer
S" $test.dat" R/O OPEN-FILE THROW CONSTANT file_id

file_id FILE-POSITION THROW D.

buffer buffer_size file_id READ-FILE THROW VALUE num_read
buffer num_read TYPE 33 EMIT

file_id FILE-POSITION THROW D.

buffer buffer_size file_id READ-FILE THROW TO num_read
buffer num_read TYPE 33 EMIT

file_id FILE-POSITION THROW D.

buffer buffer_size file_id READ-FILE THROW TO num_read
buffer num_read TYPE 33 EMIT

file_id FILE-POSITION THROW D.

buffer buffer_size file_id READ-FILE THROW TO num_read
buffer num_read TYPE 33 EMIT

file_id FILE-POSITION THROW D.

file_id CLOSE-FILE THROW
.S
END

note "Test OPEN-FILE";
note "Test WRITE-FILE";
note "Test W/O";
note "Test FILE-POSITION";
unlink "$test.dat";
forth_ok(<<END, "0 6 11 ( )");
S" $test.dat" W/O OPEN-FILE THROW CONSTANT file_id

file_id FILE-POSITION THROW D.
S" hello " file_id WRITE-FILE THROW 
file_id FILE-POSITION THROW D.
S" world"  file_id WRITE-FILE THROW 
file_id FILE-POSITION THROW D.

file_id CLOSE-FILE THROW
.S
END
is path("$test.dat")->slurp, "hello world", "write ok";

# test write on read-only file
note "Test OPEN-FILE";
note "Test WRITE-FILE";
note "Test R/O";
path("$test.dat")->spew("Hello world");
forth_nok(<<END, "\nError: WRITE-FILE exception\n");
S" $test.dat" R/O OPEN-FILE THROW CONSTANT file_id

S" HELLO " file_id WRITE-FILE THROW
.S
END

note "Test OPEN-FILE";
note "Test READ-FILE";
note "Test WRITE-FILE";
note "Test R/W";
note "Test FILE-POSITION";
path("$test.dat")->spew("Hello world");
forth_ok(<<END, "0 6 world!11 ( )");
5 CONSTANT buffer_size
buffer_size BUFFER: buffer
S" $test.dat" R/W OPEN-FILE THROW CONSTANT file_id

file_id FILE-POSITION THROW D.
S" HELLO " file_id WRITE-FILE THROW
file_id FILE-POSITION THROW D.

buffer buffer_size file_id READ-FILE THROW VALUE num_read
buffer num_read TYPE 33 EMIT
file_id FILE-POSITION THROW D.

file_id CLOSE-FILE THROW
.S
END
is path("$test.dat")->slurp, "HELLO world", "write ok";

note "Test OPEN-FILE";
note "Test READ-FILE";
note "Test WRITE-FILE";
note "Test R/W";
note "Test FILE-POSITION";
path("$test.dat")->spew("Hello world");
forth_ok(<<END, "0 Hello !6 ( )");
6 CONSTANT buffer_size
buffer_size BUFFER: buffer
S" $test.dat" R/W OPEN-FILE THROW CONSTANT file_id

file_id FILE-POSITION THROW D.
buffer buffer_size file_id READ-FILE THROW VALUE num_read
buffer num_read TYPE 33 EMIT
file_id FILE-POSITION THROW D.

S" WORLD" file_id WRITE-FILE THROW

file_id CLOSE-FILE THROW
.S
END
is path("$test.dat")->slurp, "Hello WORLD", "write ok";

note "Test FILE-SIZE";
unlink "$test.1.dat";
path("$test.2.dat")->spew("");
path("$test.3.dat")->spew(" " x 1024);
forth_nok(<<END, "\nError: FILE-SIZE exception\n");
12345 FILE-SIZE THROW .
.S
END
forth_nok(<<END, "\nError: OPEN-FILE exception\n");
S" $test.1.dat" R/O OPEN-FILE THROW CONSTANT file_id
file_id FILE-SIZE THROW .
file_id CLOSE-FILE THROW
.S
END
forth_ok(<<END, "0 ( )");
S" $test.2.dat" R/O OPEN-FILE THROW CONSTANT file_id
file_id FILE-SIZE THROW .
file_id CLOSE-FILE THROW
.S
END
forth_ok(<<END, "1024 ( )");
S" $test.3.dat" R/O OPEN-FILE THROW CONSTANT file_id
file_id FILE-SIZE THROW .
file_id CLOSE-FILE THROW
.S
END
unlink "$test.1.dat";
unlink "$test.2.dat";
unlink "$test.3.dat";

note "Test BIN";
unlink "$test.dat";
forth_ok(<<END, "( )");
S" $test.dat" W/O BIN CREATE-FILE THROW CONSTANT file_id
S\\" \\n" file_id WRITE-FILE THROW
file_id CLOSE-FILE THROW
.S
END
if ($^O eq 'MSWin32') {
	is path("$test.dat")->slurp_raw, "\r\n", "read end of line";
	forth_ok(<<END, <<END);
	S" $test.dat" R/O BIN OPEN-FILE THROW CONSTANT file_id
	PAD 256 file_id READ-FILE THROW 
	PAD SWAP DUMP
	file_id CLOSE-FILE THROW
	.S
END

00000800              0d 0a                                     ..          
( ) 
END
}
else {
	is path("$test.dat")->slurp_raw, "\n", "read end of line";
	forth_ok(<<END, <<END);
	S" $test.dat" R/O BIN OPEN-FILE THROW CONSTANT file_id
	PAD 256 file_id READ-FILE THROW 
	PAD SWAP DUMP
	file_id CLOSE-FILE THROW
	.S
END

00000800              0a                                        .           
( ) 
END
}
unlink "$test.dat";

note "Test READ-LINE";
note "Test FILE-POSITION";
#                            01 2 34 5 67 8 9 ab
path("$test.dat")->spew_raw("A\n\rB\r\rC\r\n\rD");
forth_ok(<<END, "0 A!2 !3 B!5 !6 C!9 !10 D!11 ( ) ");
5 CONSTANT buffer_size
buffer_size BUFFER: buffer
0 VALUE file_id
: x
  S" $test.dat" R/O BIN OPEN-FILE THROW TO file_id
  BEGIN
    file_id FILE-POSITION THROW D.
    buffer buffer_size file_id READ-LINE THROW
  WHILE ( eof flag )
    buffer SWAP TYPE 33 EMIT
  REPEAT
  DROP ( num_read )
  file_id CLOSE-FILE THROW
;
x .S
END

note "Test READ-LINE";
path("$test.dat")->spew_raw("hello");
forth_ok(<<END, "0 ( 0 -1 ) 0 ( 5 -1 ) 5 ( 0 0 ) 5 ( )");
5 CONSTANT buffer_size
buffer_size BUFFER: buffer
0 VALUE file_id
S" $test.dat" R/O BIN OPEN-FILE THROW TO file_id
file_id FILE-POSITION THROW D.
buffer 0 file_id READ-LINE THROW .S 2DROP
file_id FILE-POSITION THROW D.
buffer buffer_size file_id READ-LINE THROW .S 2DROP
file_id FILE-POSITION THROW D.
buffer buffer_size file_id READ-LINE THROW .S 2DROP
file_id FILE-POSITION THROW D.
file_id CLOSE-FILE THROW
.S
END

note "Test WRITE-LINE";
note "Test FILE-POSITION";
unlink "$test.dat";
if ($^O eq 'MSWin32') {
	forth_ok(<<END, "0 7 9 ( )");
		S" $test.dat" W/O BIN OPEN-FILE THROW CONSTANT file_id
		file_id FILE-POSITION THROW D.
		S" hello" file_id WRITE-LINE THROW
		file_id FILE-POSITION THROW D.
		PAD 0 file_id WRITE-LINE THROW
		file_id FILE-POSITION THROW D.
		file_id CLOSE-FILE THROW
		.S
END
}
else {
	forth_ok(<<END, "0 6 7 ( )");
		S" $test.dat" W/O BIN OPEN-FILE THROW CONSTANT file_id
		file_id FILE-POSITION THROW D.
		S" hello" file_id WRITE-LINE THROW
		file_id FILE-POSITION THROW D.
		PAD 0 file_id WRITE-LINE THROW
		file_id FILE-POSITION THROW D.
		file_id CLOSE-FILE THROW
		.S
END
}

note "Test REPOSITION-FILE";
path("$test.dat")->spew("hello world");
forth_ok(<<END, "world!");
S" $test.dat" R/O OPEN-FILE THROW CONSTANT file_id
6 0 file_id REPOSITION-FILE THROW
PAD 5 file_id READ-FILE THROW PAD SWAP TYPE 33 EMIT
file_id CLOSE-FILE THROW
END

note "Test REPOSITION-FILE";
unlink "$test.dat";
forth_ok(<<END, "");
S" $test.dat" W/O CREATE-FILE THROW CONSTANT file_id
6 0 file_id REPOSITION-FILE THROW
S" world" file_id WRITE-FILE THROW
0 0 file_id REPOSITION-FILE THROW
S" hello " file_id WRITE-FILE THROW
file_id CLOSE-FILE THROW
END
is path("$test.dat")->slurp, "hello world", "write file ok";

note "Test RESIZE-FILE";
unlink "$test.dat";
forth_ok(<<END, "");
S" $test.dat" W/O CREATE-FILE THROW CONSTANT file_id
S" hello world" file_id WRITE-FILE THROW
5 file_id RESIZE-FILE THROW
file_id CLOSE-FILE THROW
END
is path("$test.dat")->slurp, "hello", "write file ok";

note "Test DELETE-FILE";
path("$test.dat")->spew("hello world");
forth_ok(<<END, "");
S" $test.dat" DELETE-FILE THROW
END
ok !-f "$test.dat", "file deleted";

note "Test DELETE-FILE";
unlink "$test.dat";
forth_ok(<<END, "");
S" $test.dat" DELETE-FILE THROW
END
ok !-f "$test.dat", "file deleted";

note "Test RENAME-FILE";
path("$test.dat")->spew("hello world");
forth_ok(<<END, "");
S" $test.dat" S" $test.dat2" RENAME-FILE THROW
END
ok !-f "$test.dat", "file deleted";
ok -f "$test.dat2", "file created";
unlink "$test.dat2";

note "Test FLUSH-FILE";
unlink "$test.dat";
forth_ok(<<END, "");
S" $test.dat" W/O CREATE-FILE THROW CONSTANT file_id
s" Hello world" file_id WRITE-FILE THROW
file_id FLUSH-FILE THROW
END
is path("$test.dat")->slurp, "Hello world", "write file ok";

note "Test INCLUDE-FILE";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
S" $test.inc" R/O OPEN-FILE THROW VALUE file_id
file_id INCLUDE-FILE .S
END

note "Test INCLUDED";
for my $i (1..9) {
	my $j = $i-1;
	path("$test.$i.fs")->spew(<<END);
		$i 
		.
		S" $test.$j.fs" INCLUDED $i
		.
END
}
path("$test.0.fs")->spew("0 .");
capture_ok("./forth $test.9.fs", "9 8 7 6 5 4 3 2 1 0 1 2 3 4 5 6 7 8 9 ");

note "Test INCLUDE";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
INCLUDE $test.inc .S
END

note "Test REQUIRE";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
REQUIRE $test.inc
REQUIRE $test.inc
.S
END

note "Test REQUIRE";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
INCLUDE $test.inc
REQUIRE $test.inc
.S
END

note "Test REQUIRE";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 3 ( )");
INCLUDE $test.inc
INCLUDE $test.inc
.S
END

note "Test REQUIRED";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
S" $test.inc" REQUIRED
S" $test.inc" REQUIRED
.S
END

note "Test REQUIRED";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 ( )");
S" $test.inc" INCLUDED
S" $test.inc" REQUIRED
.S
END

note "Test REQUIRED";
path("$test.inc")->spew("1 2 + .");
forth_ok(<<END, "3 3 ( )");
S" $test.inc" INCLUDED
S" $test.inc" INCLUDED
.S
END

note "Test FILE-STATUS";
note "Test FS-EXISTS";
note "Test FS-REGULAR";
note "Test FS-DIRECTORY";
note "Test FS-SYMLINK";			# not tested
note "Test FS-READABLE";		# not tested
note "Test FS-WRITABLE";		# not tested
note "Test FS-EXECUTABLE";		# not tested

path("$test.dat")->spew("hello");
forth_ok(<<END, "-1 -1 0 ( ) ");
S" $test.dat" FILE-STATUS THROW VALUE status
status FS-EXISTS    AND 0= 0= .
status FS-REGULAR   AND 0= 0= .
status FS-DIRECTORY AND 0= 0= .
.S
END

unlink "$test.dat";
mkdir "$test.dat";
forth_ok(<<END, "-1 0 -1 ( ) ");
S" $test.dat" FILE-STATUS THROW VALUE status
status FS-EXISTS    AND 0= 0= .
status FS-REGULAR   AND 0= 0= .
status FS-DIRECTORY AND 0= 0= .
.S
END

rmdir "$test.dat";
forth_nok(<<END, "\nError: FILE-STATUS exception\n");
S" $test.dat" FILE-STATUS THROW
END

unlink "$test.dat", "$test.inc";
unlink <$test.*.fs>;
unlink <$test.*.dat>;
rmdir <$test.*.dat>;
end_test;
