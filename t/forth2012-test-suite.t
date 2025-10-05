#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

chdir "t/forth2012-test-suite/src" or die;

note "Running runtests.fth";
path("$test.fs")->spew('S" runtests.fth" INCLUDED');
capture_ok("perl -E \"say 'hello'\" | forth $test.fs", <<'END');

Running ANS Forth and Forth 2012 test programs, version 0.13.4


CR CR SOURCE TYPE ( Preliminary test ) CR
SOURCE ( These lines test SOURCE, TYPE, CR and parenthetic comments ) TYPE CR
( The next line of output should be blank to test CR ) SOURCE TYPE CR CR

( Pass #1: testing 0 >IN +! ) 0 >IN +! SOURCE TYPE CR
( Pass #2: testing 1 >IN +! ) 1 >IN +! xSOURCE TYPE CR
( Pass #3: testing 1+ ) 1 1+ >IN +! xxSOURCE TYPE CR
( Pass #4: testing @ ! BASE ) 0 1+ 1+ BASE ! BASE @ >IN +! xxSOURCE TYPE CR
( Pass #5: testing decimal BASE ) BASE @ >IN +! xxxxxxxxxxSOURCE TYPE CR
( Pass #6: testing : ; ) : .SRC SOURCE TYPE CR ; 6 >IN +! xxxxxx.SRC
( Pass #7: testing number input ) 19 >IN +! xxxxxxxxxxxxxxxxxxx.SRC
( Pass #8: testing VARIABLE ) VARIABLE Y 2 Y ! Y @ >IN +! xx.SRC
( Pass #9: testing WORD COUNT ) 5 MSG abcdef) Y ! Y ! >IN +! xxxxx.SRC
( Pass #10: testing WORD COUNT ) MSG ab) >IN +! xxY ! .SRC
Pass #11: testing WORD COUNT .MSG
Pass #12: testing = returns all 1's for true
Pass #13: testing = returns 0 for false
Pass #14: testing -1 interpreted correctly
Pass #15: testing 2*
Pass #16: testing 2*
Pass #17: testing AND
Pass #18: testing AND
Pass #19: testing AND
Pass #20: testing ?F~ ?~~ Pass Error
Pass #21: testing ?~
Pass #22: testing EMIT
Pass #23: testing S"

Results: 

Pass messages #1 to #23 should be displayed above
and no error messages

0 tests failed out of 57 additional tests


--- End of Preliminary Tests --- 

YOU SHOULD SEE THE STANDARD GRAPHIC CHARACTERS:
 !"#$%&'()*+,-./0123456789:;<=>?@
ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
abcdefghijklmnopqrstuvwxyz{|}~
YOU SHOULD SEE 0-9 SEPARATED BY A SPACE:
0 1 2 3 4 5 6 7 8 9 
YOU SHOULD SEE 0-9 (WITH NO SPACES):
0123456789
YOU SHOULD SEE A-G SEPARATED BY A SPACE:
A B C D E F G 
YOU SHOULD SEE 0-5 SEPARATED BY TWO SPACES:
0  1  2  3  4  5  
YOU SHOULD SEE TWO SEPARATE LINES:
LINE 1
LINE 2
YOU SHOULD SEE THE NUMBER RANGES OF SIGNED AND UNSIGNED NUMBERS:
  SIGNED: -80000000 7FFFFFFF 
UNSIGNED: 0 FFFFFFFF 

PLEASE TYPE UP TO 80 CHARACTERS:

RECEIVED: "hello"

End of Core word set tests

You should see 2345: 2345

End of additional Core tests

Test utilities loaded


Output from .(
You should see -9876: -9876 
and again: -9876


On the next 2 lines you should see First then Second messages:
First message via .( 
Second message via ."



Output from .R and U.R
You should see lines duplicated:
indented by 0 spaces
1984383623 
1984383623
-2088648480 
-2088648480
1984383623 
1984383623
2206318816 
2206318816

indented by 0 spaces
1984383623 
1984383623
-2088648480 
-2088648480
1984383623 
1984383623
2206318816 
2206318816

indented by 5 spaces
     1984383623 
     1984383623
     -2088648480 
     -2088648480
     1984383623 
     1984383623
     2206318816 
     2206318816


The next test should display:
One line...
another line
One line...
anotherLine

End of Core Extension word tests

Block 29 
 1 Should show a (mostly) blank screen                             
 2                                                                 
 3                                                                 
 4                                                                 
 5                                                                 
 6                                                                 
 7                                                                 
 8                                                                 
 9                                                                 
10                                                                 
11                                                                 
12                                                                 
13                                                                 
14                                                                 
15                                                                 
16                                                                 

Block 20 
 1 List of the First test block                                    
 2                                                                 
 3                                                                 
 4                                                                 
 5                                                                 
 6                                                                 
 7                                                                 
 8                                                                 
 9                                                                 
10                                                                 
11                                                                 
12                                                                 
13                                                                 
14                                                                 
15                                                                 
16                                                                 

Block 29 
 1 List of the Last test block                                     
 2                                                                 
 3                                                                 
 4                                                                 
 5                                                                 
 6                                                                 
 7                                                                 
 8                                                                 
 9                                                                 
10                                                                 
11                                                                 
12                                                                 
13                                                                 
14                                                                 
15                                                                 
16                                                                 

Block 21 
 1                                                                 
 2                                                                 
 3                                                                 
 4                                                                 
 5                                                                 
 6                                                                 
 7                                                                 
 8                                                                 
 9                                                                 
10                                                                 
11                                                                 
12                                                                 
13                                                                 
14                                                                 
15                                                                 
16                                                    End of Screen

Block 29 
 1 Should show another (mostly) blank screen                       
 2                                                                 
 3                                                                 
 4                                                                 
 5                                                                 
 6                                                                 
 7                                                                 
 8                                                                 
 9                                                                 
10                                                                 
11                                                                 
12                                                                 
13                                                                 
14                                                                 
15                                                                 
16                                                                 
Calculated Characters per Line: 64 

End of Block word tests

You should see lines duplicated:
     8970676912557384689
     8970676912557384689 
        8970676912557384689
        8970676912557384689
     -8522862768232894102
     -8522862768232894102 
          -8522862768232894102
          -8522862768232894102

End of Double-Number word tests

End of Exception word tests

End of Facility word tests

End of File-Access word set tests

End of Locals word set tests. ( ) 
End of Memory-Allocation word tests

NAME>INTERPRET returns an execution token for all
core words with undefined interpretation semantics.
So NAME>INTERPRET returning 0 is untested.

End of Programming Tools word tests

ONLY FORTH DEFINITIONS search order and compilation wordlist

Search order: 0 
Definitions: 0 

Plus another unnamed wordlist at the head of the search order

Search order: 0 4 
Definitions: 4 

End of Search Order word tests

End of String word tests

---------------------------
        Error Report
Word Set             Errors
---------------------------
Core                    0
Core extension          0
Block                   0
Double number           0
Exception               0
Facility                0
File-access             0
Locals                  0
Memory-allocation       0
Programming-tools       0
Search-order            0
String                  0
---------------------------
Total                   0
---------------------------


Forth tests completed 

END

if (0) {	# test results differ from machine to machine
	chdir "fp" or die;

	note "Running runfptests.fth";
	forth_ok('S" runfptests.fth" INCLUDED', <<'END');

Running FP Tests

Running ak-fp-test.fth
----------------------

TESTING --- MINFORTH FLOATING-POINT WORDS ---
TESTING BASIC FUNCTIONS
TESTING STACK OPERATIONS
TESTING BASIC ARITHMETICS
TESTING COMPARISONS
TESTING MEMORY ACCESS, FLITERAL, FCONSTANT, FVARIABLE
TESTING NUMBER INPUT
TESTING FRACTIONAL ARITHMETICS
TESTING TRIGONOMETRIC FUNCTIONS
TESTING EXPONENTIAL AND LOGARITHMIC FUNCTIONS
TESTING HYPERBOLIC FUNCTIONS
TESTING NUMBER OUTPUT

CHECKING FS. 
You might see 1.0000E0  : 1E+0 
You might see 2.0000E1  : 2E+1 
You might see 2.0000E-2 : 2E-2 
You might see -3.3300E4 : -3.33E+4 
You might see 3.3333E0  : 3.3333E+0 
You might see 6.6667E-2 : 6.6667E-2 
CHECKING FE. 
You might see 1.0000E0  : 1E0 
You might see 20.000E0  : 20E0 
You might see 300.00E0  : 300E0 
You might see 4.0000E3  : 4E3 
You might see 333.33E-3 : 333.3333E-3 
You might see 6.6667E3  : 6.6667E3 
CHECKING F. 
You might see 1000.   : 1000. 
You might see 1100.   : 1100. 
You might see 0.33333 : 0.3333 
You might see 66.667  : 66.6667 
You might see 0.00023 : 0.0002 
You might see 0.00024 : 0.0002 

End of ak-fp-test.fth

Running fatan2-test.fs
----------------------

floating-point and data stacks *separate*
testing normal values
testing Single UNIX 3 special values spec
testing Single UNIX 3 special values optional spec

#ERRORS: 0 


End of fatan2-test.fs

Running ieee-arith-test.fs
--------------------------

TESTING F+
TESTING F-
TESTING F*
TESTING F/
TESTING FSQRT
NOT TESTING F*+
#ERRORS: 0 


End of ieee-arith-test.fs

Running ieee-fprox-test.fs
--------------------------

TESTING equality of floating-point encoding
TESTING absolute tolerance
TESTING relative tolerance
#ERRORS: 0 

End of ieee-fprox-test.fs

Running fpzero-test.4th
-----------------------


System supports fp signed zero. 
#ERRORS: 0 

End of fpzero-test.4th

Running fpio-test.4th
---------------------
FPIO-TEST         V1.1      01 Dec     2010 
TESTING Conversion of Exactly Representable Numbers

TESTING Rounding of Numbers

End of fpio-test.4th

Running to-float-test.4th
-------------------------
TESTING >FLOAT

#ERRORS: 0 

End of to-float-test.4th

Running paranoia.4th
--------------------

Lest this program stop prematurely, i.e. before displaying

   `END OF TEST',

try to persuade the computer NOT to terminate execution when an
error like Over/Underflow or Division by Zero occurs, but rather
to persevere with a surrogate value after, perhaps, displaying some
warning.  If persuasion avails naught, don't despair but run this
program anyway to see how many milestones it passes, and then
amend it to make further progress.

Answer questions with Y, y, N or n (unless otherwise indicated).
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Users are invited to help debug and augment this program so it will
cope with unanticipated and newly uncovered arithmetic pathologies.
Please send suggestions and interesting results to

Richard Karpinski
Computer Center U-76
University of California
San Francisco, CA 94143-0704, USA

In doing so, please include the following information:

Precision: double
Version: 10 February 1989; Forth
Computer:
Compiler:
Optimization level:
Other relevant compiler options:
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Running this program should reveal these characteristics:

    Radix = 1, 2, 4, 8, 10, 16, 100, 256 ...
    Precision = number of significant digits carried.
    U2 = Radix/Radix^Precision = One Ulp
(OneUlpnit in the Last Place) of 1.000xxx .
    U1 = 1/Radix^Precision = One Ulp of numbers a little less than 1.0 .
    Adequacy of guard digits for Mult., Div. and Subt.
    Whether arithmetic is chopped, correctly rounded, or something else
for Mult., Div., Add/Subt. and Sqrt.
    Whether a Sticky Bit used correctly for rounding.
    UnderflowThreshold = an underflow threshold.
    E0 and PseudoZero tell whether underflow is abrupt, gradual, or fuzzy.
    V = an overflow threshold, roughly.
    V0  tells, roughly, whether  Infinity  is represented.
    Comparisions are checked for consistency with subtraction
and for contamination with pseudo-zeros.
    Sqrt is tested.  Y^X is not tested.
    Extra-precise subexpressions are revealed but NOT YET tested.
    Decimal-Binary conversion is NOT YET tested for accuracy.
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

The program attempts to discriminate among

  FLAWs, like lack of a sticky bit,
  Serious DEFECTs, like lack of a guard digit, and
  FAILUREs, like 2+2 == 5 .

Failures may confound subsequent diagnoses.

The diagnostic capabilities of this program go beyond an earlier
program called `MACHAR', which can be found at the end of the
book  `Software Manual for the Elementary Functions' (1980) by
W. J. Cody and W. Waite. Although both programs try to discover
the Radix, Precision and range (over/underflow thresholds)
of the arithmetic, this program tries to cope with a wider variety
of pathologies, and to say how well the arithmetic is implemented.
The program is based upon a conventional radix representation for
floating-point numbers, but also allows logarithmic encoding
as used by certain early WANG machines.

BASIC version of this program (C) 1983 by Prof. W. M. Kahan;
see source comments for more history.
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Program is now RUNNING tests on small integers:
-1, 0, 1/2, 1, 2, 3, 4, 5, 9, 27, 32 & 240 are O.K.

Searching for Radix and Precision.
Radix = 2. .
Closest relative separation found is U1 = 0E+0 

Recalculating radix and precision 
confirms closest relative separation U1 .
Radix confirmed.
The number of significant digits of the Radix is 53. 
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Subtraction appears to be normalized, as it should be.
Checking for guard digit in F*, F/, and F-.
    F*, F/, and F- appear to have guard digits, as they should.
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Checking rounding on multiply, divide and add/subtract.
Multiplication appears to round correctly.
Division appears to round correctly.
Addition/Subtraction appears to round correctly.
Checking for sticky bit.
Sticky bit apparently used correctly.

Does Multiplication commute?  Testing on 45500 random pairs.
    No failures found in 45500 integer pairs.

Running test of square root(x).
Testing if sqrt(X * X) == X for 45500 Integers X.
Test for sqrt monotonicity.
sqrt has passed a test for Monotonicity.
Testing whether sqrt is rounded or chopped.
Square root appears to be correctly rounded.
Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Testing powers Z^i for small Integers Z and i.
... no discrepancies found.

Seeking Underflow thresholds UfThold and E0.
Smallest strictly positive number found is E0 = 0E+0 
Since comparison denies Z = 0, evaluating (Z + Z) / Z should be safe.
What the machine gets for (Z + Z) / Z is  2. 
This is O.K., provided Over/Underflow has NOT just been signaled.
Underflow is gradual; it incurs Absolute Error =
(roundoff in UfThold) < E0.
The Underflow threshold is 0E+0 below which
calculation may suffer larger Relative error than merely roundoff.
Since underflow occurs below the threshold
UfThold = 2E+0 ^-1.022E+3 
only underflow should afflict the expression
     2E+0 ^-2.044E+3 
actually calculating yields: 0E+0 
This computed value is O.K.

Testing X^((X + 1) / (X - 1)) vs. exp(2) = 7.3891E+0 as X -> 1.
Accuracy seems adequate.
Testing powers Z^Q at four nearly extreme values.
... no discrepancies found.

Diagnosis resumes after milestone Number 48700 
         Page: 48736 

Searching for Overflow threshold:
This may generate an error.
Can `Z = -Y' overflow?
Trying it on Y = -infE-2147483648 
Seems O.K.
Overflow threshold is V  = 1.7977E+308 
Overflow saturates at V0 = infE-2147483648 
No Overflow should be signaled for V * 1 = 1.7977E+308 
                          nor for V / 1 = 1.7977E+308 
Any overflow signal separating this * from the one
above is a DEFECT.

Diagnosis resumes after milestone Number 48700 
         Page: 48736 


What message and/or values does Division by Zero produce?

   Trying to compute 1 / 0 produces ...infE-2147483648 

   Trying to compute 0 / 0 produces ...-nanE-2147483648 
Diagnosis resumes after milestone Number 48700 
         Page: 48736 


FAILUREs  encountered = 48616 
SERIOUS DEFECTs  discovered = 48620 
DEFECTs  discovered = 48624 
FLAWs  discovered = 48628 

No failures, defects nor flaws have been discovered.
The arithmetic diagnosed appears to be Excellent!
END OF TEST.

End of paranoia.fth


FP tests finished

END

	chdir ".." or die;
}

# unlink test-generated files
chdir "../../.." or die;

if (Test::More->builder->is_passing) {
	unlink "fatest1.txt";
	unlink "t/forth2012-test-suite/src/blocks.fb";
	unlink <t/forth2012-test-suite/src/$test.*>;
	unlink <t/forth2012-test-suite/src/fp/$test.*>;
}

end_test;
