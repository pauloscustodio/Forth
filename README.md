# Forth

[![Build on Ubuntu](https://github.com/pauloscustodio/Forth/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/pauloscustodio/Forth/actions/workflows/ubuntu.yml)
[![Build on MacOS](https://github.com/pauloscustodio/Forth/actions/workflows/macos.yml/badge.svg)](https://github.com/pauloscustodio/Forth/actions/workflows/macos.yml)
[![Build on Windows Msbuild](https://github.com/pauloscustodio/Forth/actions/workflows/msbuild.yml/badge.svg)](https://github.com/pauloscustodio/Forth/actions/workflows/msbuild.yml)
[![Build on Windows MSYS2](https://github.com/pauloscustodio/Forth/actions/workflows/msys2.yml/badge.svg)](https://github.com/pauloscustodio/Forth/actions/workflows/msys2.yml)
[![Build on Windows Cygwin](https://github.com/pauloscustodio/Forth/actions/workflows/cygwin.yml/badge.svg)](https://github.com/pauloscustodio/Forth/actions/workflows/cygwin.yml)

This is a C++ implementation of a ANS 2012 Forth interpreter. It implements all
the 
CORE, CORE-EXT, 
BLOCK, BLOCK-EXT, 
DOUBLE, DOUBLE-EXT, 
EXCEPTION, EXCEPTION-EXT,
FACILITY, FACILITY-EXT,
FILE, FILE-EXT,
FLOATING, FLOATING-EXT,
LOCAL, LOCAL-EXT 
and
MEMORY
words. 

It is intended to be be used as a script interpreter, 
i.e. calling ABORT exits the interpreter instead of restarting the QUIT loop.

It was inspired by the excelent [pforth](https://github.com/philburk/pforth).

See [ANS Forth Standard Introduction](https://forth-standard.org/standard/intro)

It should compile and run on any C++17 compiler on Windows and POSIX systems 
(Linux, macOS, Windows MSYS2, Cygwin and Msbuild).

Why another Forth interpreter? Just for fun!

Implemented WORDS:
```
BLOCK:
    BLK BLOCK BUFFER EVALUATE FLUSH LOAD SAVE-BUFFERS UPDATE

BLOCK EXT:
    EMPTY-BUFFERS LIST REFILL SCR THRU \

CORE:
    ! # #> #S ' ( * */ */MOD + +! +LOOP , - . ." / /MOD 0< 0= 1+ 1- 2! 2*
    2/ 2@ 2DROP 2DUP 2OVER 2SWAP : ; < <# = > >BODY >IN >NUMBER >R ?DUP @
    ABORT ABORT" ABS ACCEPT ALIGN ALIGNED ALLOT AND BASE BEGIN BL C! C, C@
    CELL+ CELLS CHAR CHAR+ CHARS CONSTANT COUNT CR CREATE DECIMAL DEPTH DO
    DOES> DROP DUP ELSE EMIT ENVIRONMENT? EVALUATE EXECUTE EXIT FILL FIND
    FM/MOD HERE HOLD I IF IMMEDIATE INVERT J KEY LEAVE LITERAL LOOP LSHIFT
    M* MAX MIN MOD MOVE NEGATE OR OVER POSTPONE QUIT R> R@ RECURSE REPEAT
    ROT RSHIFT S" S>D SIGN SM/REM SOURCE SPACE SPACES STATE SWAP THEN TYPE
    U. U< UM* UM/MOD UNLOOP UNTIL VARIABLE WHILE WORD XOR [ ['] [CHAR] ]

CORE EXT:
    .( .R 0<> 0> 2>R 2R> 2R@ :NONAME <> ?DO ACTION-OF AGAIN BUFFER: C" CASE
    COMPILE, DEFER DEFER! DEFER@ ENDCASE ENDOF ERASE FALSE HEX HOLDS IS
    MARKER NIP OF PAD PARSE PARSE-NAME PICK REFILL RESTORE-INPUT ROLL
    SAVE-INPUT SOURCE-ID S\" TO TRUE TUCK U.R U> UNUSED VALUE WITHIN
    [COMPILE] \

DOUBLE:
    2CONSTANT 2LITERAL 2VARIABLE D+ D- D. D.R D0< D0= D2* D2/ D< D= D>S
    DABS DMAX DMIN DNEGATE M*/ M+

DOUBLE EXT:
    2ROT 2VALUE DU<

EXCEPTION:
    CATCH THROW

EXCEPTION EXT:
    ABORT ABORT"

FACILITY:
    AT-XY KEY? PAGE

FACILITY EXT:
    +FIELD BEGIN-STRUCTURE CFIELD: EKEY EKEY>CHAR EKEY>FKEY EKEY? EMIT?
    END-STRUCTURE FIELD: K-ALT-MASK K-CTRL-MASK K-DELETE K-DOWN K-END K-F1
    K-F10 K-F11 K-F12 K-F2 K-F3 K-F4 K-F5 K-F6 K-F7 K-F8 K-F9 K-HOME
    K-INSERT K-LEFT K-NEXT K-PRIOR K-RIGHT K-SHIFT-MASK K-UP MS TIME&DATE

FILE:
    ( BIN CLOSE-FILE CREATE-FILE DELETE-FILE FILE-POSITION FILE-SIZE
    INCLUDE-FILE INCLUDED OPEN-FILE R/O R/W READ-FILE READ-LINE
    REPOSITION-FILE RESIZE-FILE S" SOURCE-ID W/O WRITE-FILE WRITE-LINE

FILE EXT:
    FILE-STATUS FLUSH-FILE INCLUDE REFILL RENAME-FILE REQUIRE REQUIRED S\"

FLOATING:
    >FLOAT D>F F! F* F+ F- F/ F0< F0= F< F>D F@ FALIGN FALIGNED FCONSTANT
    FDEPTH FDROP FDUP FLITERAL FLOAT+ FLOATS FLOOR FMAX FMIN FNEGATE FOVER
    FROT FROUND FSWAP FVARIABLE REPRESENT

FLOATING EXT:
    DF! DF@ DFALIGN DFALIGNED DFFIELD: DFLOAT+ DFLOATS F** F. F>S FABS
    FACOS FACOSH FALOG FASIN FASINH FATAN FATAN2 FATANH FCOS FCOSH FE. FEXP
    FEXPM1 FFIELD: FLN FLNP1 FLOG FS. FSIN FSINCOS FSINH FSQRT FTAN FTANH
    FTRUNC FVALUE F~ PRECISION S>F SET-PRECISION SF! SF@ SFALIGN SFALIGNED
    SFFIELD: SFLOAT+ SFLOATS

LOCAL:
    (LOCAL)

LOCAL EXT:
    LOCALS| {:

MEMORY:
    ALLOCATE FREE RESIZE

SEARCH:
    FIND
  Missing:
    DEFINITIONS FORTH-WORDLIST GET-CURRENT GET-ORDER SEARCH-WORDLIST
    SET-CURRENT SET-ORDER WORDLIST

SEARCH EXT:
  Missing:
    ALSO FORTH ONLY ORDER PREVIOUS

STRING:
    /STRING
  Missing:
    -TRAILING BLANK CMOVE CMOVE> COMPARE SEARCH SLITERAL

STRING EXT:
  Missing:
    REPLACES SUBSTITUTE UNESCAPE

TOOLS:
    .S ? DUMP SEE WORDS

TOOLS EXT:
    AHEAD BYE CS-PICK CS-ROLL FORGET N>R NAME>COMPILE NAME>INTERPRET
    NAME>STRING NR> STATE SYNONYM TRAVERSE-WORDLIST [DEFINED] [UNDEFINED]
  Missing:
    ;CODE ASSEMBLER CODE EDITOR [ELSE] [IF] [THEN]

XCHAR:
  Missing:
    X-SIZE XC!+ XC!+? XC, XC-SIZE XC@+ XCHAR+ XEMIT XKEY XKEY?

XCHAR EXT:
    CHAR PARSE [CHAR]
  Missing:
    +X/STRING -TRAILING-GARBAGE EKEY>XCHAR X-WIDTH XC-WIDTH XCHAR- XHOLD
    X\STRING-

NOT STANDARD:
    #IN #TIB -2ROT -FROT -ROT .FS .RS 0<= 0>= 2FIELD: <= >= >NAME CONVERT
    D0<= D0<> D0> D0>= D<= D<> D> D>= DPL DU<= DU> DU>= EXPECT F0<= F0<>
    F0> F0>= F<= F<> F= F> F>= FS-DIRECTORY FS-EXECUTABLE FS-EXISTS
    FS-READABLE FS-REGULAR FS-SYMLINK FS-WRITABLE INTERPRET LATEST NUMBER
    NUMBER? OFF ON PARSE-WORD QUERY R0 RDROP RSP! RSP@ S0 SP! SP@ SPAN TIB
    TRACE U<= U>=
```

# Documentation of not standard words

## #IN
( -- a-addr )

Returns the address of a cell containing the number of characters in the current input buffer.

## TIB
( -- c-addr )

Returns the address of the current input buffer.

## #TIB
( -- a-addr )

Returns the address of a cell containing the number of characters in the current input buffer.

## -ROT
( x1 x2 x3 -- x3 x1 x2 )

Rotate the top three stack entries bringing cell x3 to the bottom
of the stack, the opposite of ROT.

## -2ROT
( x1 x2 x3 x4 x5 x6 -- x5 x6 x1 x2 x3 x4 )

Rotate the top three cell pairs on the stack bringing cell pair x5 x6 to the 
bottom of the stack, the opposite of 2ROT.

## -FROT
(F: r1 r2 r3 -- r3 r1 r2 )

Rotate the top three floating-point stack entries bringing cell r3 to the bottom
of the stack, the opposite of FROT.

## .RS
( -- )

Copy and display the values currently on the return stack. The format of the display is implementation-dependent.

## .FS
( -- )

Copy and display the values currently on the floating point stack. The format of the display is implementation-dependent.

## 0<=
( n -- flag )

flag is true if and only if n is less or equal to zero.

## 0>=
( n -- flag )

flag is true if and only if n is greater or equal to zero.

## <=
( n1 n2 -- flag )

flag is true if and only if n1 is less or equal to n2.

## >=
( n1 n2 -- flag )

flag is true if and only if n1 is greater or equal to n2.

## D0<=
( d -- flag )

flag is true if and only if d is less or equal to zero.

## D0<>
( d -- flag )

flag is true if and only if d is not zero.

## D0>
( d -- flag )

flag is true if and only if d is greater than zero.

## D0>=
( d -- flag )

flag is true if and only if d is greater or equal to zero.

## D<=
( d1 d2 -- flag )

flag is true if and only if d1 is less or equal to d2.

## D<>
( d1 d2 -- flag )

flag is true if and only if d1 is different from d2.

## D>
( d1 d2 -- flag )

flag is true if and only if d1 is greater than d2.

## D>=
( d1 d2 -- flag )

flag is true if and only if d1 is greater or equal to d2.

## DU<=
( ud1 ud2 -- flag )

flag is true if and only if ud1 is less or equal to ud2.

## DU>
( ud1 ud2 -- flag )

flag is true if and only if ud1 greater than ud2.

## DU>=
( ud1 ud2 -- flag )

flag is true if and only if ud1 greater or equal to ud2.

## F0<=
( -- flag ) (F: r -- )

flag is true if and only if r is less or equal to zero.

## F0<>
( -- flag ) (F: r -- )

flag is true if and only if r is different from zero.

## F0>
( -- flag ) (F: r -- )

flag is true if and only if r is greater than zero.

## F0>=
( -- flag ) (F: r -- )

flag is true if and only if r is greater or equal to zero.

## F<=
( -- flag ) (F: r1 r2 -- )

flag is true if and only if r1 is less or equal than r2.

## F<>
( -- flag ) (F: r1 r2 -- )

flag is true if and only if r1 is different than r2.

## F=
( -- flag ) (F: r1 r2 -- )

flag is true if and only if r1 is equal to r2.

## F>
( -- flag ) (F: r1 r2 -- )

flag is true if and only if r1 is greater than r2.

## F>=
( -- flag ) (F: r1 r2 -- )

flag is true if and only if r1 is greater or equal to r2.

## U<=
( u1 u2 -- flag )

flag is true if and only if u1 is less or equal than u2.

## U>=
( u1 u2 -- flag )

flag is true if and only if u1 is greater or equal than u2.

## 2FIELD:
( n1 "<spaces>name" -- n2 )

Skip leading space delimiters. Parse name delimited by a space. Offset is the first cell aligned value greater than or equal to n1. n2 = offset + 2 cells.

Create a definition for name with the execution semantics given below.

name Execution:
( addr1 -- addr2 )

Add the offset calculated during the compile-time action to addr1 giving the address addr2.

## FILE-STATUS

Return the status of the file identified by the character string c-addr u. If the file exists, ior is zero; otherwise ior is the implementation-defined I/O result code. x contains implementation-defined information about the file.

## FS-DIRECTORY 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is a directory.

## FS-EXECUTABLE 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is executable.

## FS-EXISTS 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file exists.

## FS-READABLE 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is reaadable.

## FS-REGULAR 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is a regular file.

## FS-SYMLINK 
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is a symbolic link.

## FS-WRITABLE
( -- x )

Returns the bitmask returned by FILE-STATUS if the file is writeable.

## TRACE
( -- a-addr )

Returns the address of the TRACE flag. If the TRACE flag is TRUE, either by being 
set by the program, or by the forth interpreter being called with the -t option, then
before each word execution the name of the word is output, and after execution
the contents of the stack is output.

## ON
( a-addr -- )

a-addr is the address of a cell variable. ON sets the variable to TRUE (-1).

## OFF
( a-addr -- )

a-addr is the address of a cell variable. OFF sets the variable to FALSE (0).

## SP@
( -- n )

Return the current value of the stack pointer.

## SP!
( n -- )

Sets the value of the stack pointer.

## S0
( -- n )

Returns the value that when used by SP! clears the stack.

## RSP@
( -- n )

Return the current value of the return stack pointer.

## RSP!
( n -- )

Sets the value of the return stack pointer.

## R0
( -- n )

Returns the value that when used by RSP! clears the return stack.

## RDROP
(R: x -- )

Eliminate the top item of the return stack.

## LATEST
( -- a-addr )

Return the address of the header of the word being defined, or the last word defined.

## PARSE-WORD
( "<spaces>name<space>" -- c-addr u )

Skip leading space delimiters. Parse name delimited by a space.

c-addr is the address of the selected string within the input buffer and u is its length in characters. If the parse area is empty or contains only white space, the resulting string has length zero.

## CONVERT
( ud1 c-addr1 -- ud2 c-addr2 )

Take a double-cell unsigned number ud1 and a counted string at c-addr1, and 
attempt to convert the next character(s) of the string into a number using the 
current numeric base. It returns the updated number ud2 and the address of the 
first character that could not be converted c-addr2. It is used to incrementally 
parse a string of digits. If the next character is not a valid digit in the 
current base, conversion stops.

## NUMBER
( c-addr u -- d )

Converts the string at c-addr and length u into a double-cell number d, using 
the current numeric base. If the string is not a valid number, it throws an 
error.

## NUMBER?
( c-addr u -- d 2 )  	\ double number conversion
( c-addr u -- n 1 )  	\ single number conversion
( c-addr u -- n 0 )  	\ conversion failed

Attempts to interpret the string at c-addr u as a number using the current base. 
If successful, it returns the number and the number of cells of the result; 
otherwise, it returns zero.

## DPL
( -- a-addr )

Return the address of a cell containing the number of digits after the last 
punctuation character in a double number conversion.

## EXPECT
( c-addr +n -- )

Reads up to +n characters from the input source (typically the keyboard) and 
stores them at the address c-addr. The input ends when the user presses Enter 
or when +n characters have been read. The characters are stored starting at 
c-addr, and the actual number of characters read is stored in the system 
variable SPAN.

## SPAN
( -- a-addr )

Return the address of a cell containing the number of characters read by 
EXPECT.

## QUERY
( -- )

Read a line of input from the user (typically from the keyboard) and store it 
in the terminal input buffer (TIB). It also sets the system variable #TIB to 
the number of characters read.

## INTERPRET
( -- )

Processe the current input buffer (as defined by SOURCE) and attempt to 
interpret each word found. It uses the current search order and dictionary to 
resolve each word, executing it if found, or attempting to parse it as a number 
if not. If a word is not found and cannot be converted to a number, INTERPRET 
triggers an error.

## >NAME
( xt -- nt )

Convert an execution token into a name token, the inverse of NAME>INTERPRTET.

#

Copyright (c) Paulo Custodio, 2020-2025

License: [GPL3](https://www.gnu.org/licenses/gpl-3.0.html) 
