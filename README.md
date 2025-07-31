Forth
-----

This is a C implementation of a ANS Forth interpreter. It implements all the 
CORE and CORE-EXT words. It is intended to be be used as a script interpreter, 
i.e. calling ABORT exits the interpreter instead of restarting the QUIT loop.

Why another Forth interpreter? Just for fun!

Output of WORDS:
```
QUIT LITERAL BYE DABS WORDS ENVIRONMENT? THROW BLK .RS RDEPTH .S CS_DEPTH
RDEPTH EXIT EXECUTE EVALUATE INTERPRET U.R .R U. D.R D. . #> SIGN HOLD #S # <#
SPACES SPACE CR EMIT TYPE RESTORE-INPUT SAVE-INPUT QUERY KEY EXPECT ACCEPT
REFILL SOURCE-ID SOURCE >IN #TIB #IN TIB .( C" S" ." COUNT CONVERT >NUMBER
NUMBER NUMBER? [CHAR] CHAR WORD PARSE-WORD PARSE MARKER UNUSED ALLOT ALIGNED
ALIGN >BODY FIND LATEST HERE C, , 2R@ 2R> 2>R J I R@ RSP! RSP@ RDROP R> >R SP!
SP@ -2ROT 2ROT 2OVER 2DUP 2SWAP 2DROP TUCK ROLL PICK NIP DEPTH -ROT ROT OVER
?DUP DUP SWAP DROP MOVE ERASE FILL 2@ 2! C@ C! +! @ ! 0>= 0> 0<= 0< 0<> 0= U>=
U> U<= U< >= > <= < <> = RSHIFT LSHIFT INVERT XOR OR AND WITHIN CELLS CELL+
CHARS CHAR+ MIN MAX ABS UM* S>D NEGATE 2/ 2* 1- 1+ M* SM/REM UM/MOD FM/MOD
*/MOD */ /MOD MOD / - * + HEX DECIMAL PAD BASE TRACE DPL STATE SPAN FALSE TRUE
CS0 R0 S0 BL

```

Copyright (c) Paulo Custodio, 2020-2025

License: [GPL3](https://www.gnu.org/licenses/gpl-3.0.html) 
