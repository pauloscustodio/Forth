Forth
-----

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
FACILITY, FACILITY-EXT
FILE and FILE-EXT
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
  Missing:
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
    BYE STATE
  Missing:
    ;CODE AHEAD ASSEMBLER CODE CS-PICK CS-ROLL EDITOR FORGET N>R
    NAME>COMPILE NAME>INTERPRET NAME>STRING NR> SYNONYM TRAVERSE-WORDLIST
    [DEFINED] [ELSE] [IF] [THEN] [UNDEFINED]

XCHAR:
  Missing:
    X-SIZE XC!+ XC!+? XC, XC-SIZE XC@+ XCHAR+ XEMIT XKEY XKEY?

XCHAR EXT:
    CHAR PARSE [CHAR]
  Missing:
    +X/STRING -TRAILING-GARBAGE EKEY>XCHAR X-WIDTH XC-WIDTH XCHAR- XHOLD
    X\STRING-

NOT STANDARD:
    #IN #TIB -2ROT -FROT -ROT .FS .RS 0<= 0>= 2FIELD: <= >= CONVERT D0<=
    D0<> D0> D0>= D<= D<> D> D>= DPL DU<= DU> DU>= EXPECT F0<= F0<> F0>
    F0>= F<= F<> F= F> F>= FS-DIRECTORY FS-EXECUTABLE FS-EXISTS FS-READABLE
    FS-REGULAR FS-SYMLINK FS-WRITABLE INTERPRET LATEST NUMBER NUMBER? OFF
    ON PARSE-WORD QUERY R0 RDROP RSP! RSP@ S0 SP! SP@ SPAN TIB TRACE U<=
    U>=
```

Copyright (c) Paulo Custodio, 2020-2025

License: [GPL3](https://www.gnu.org/licenses/gpl-3.0.html) 
