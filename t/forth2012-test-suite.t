#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

chdir "t/forth2012-test-suite/src" or die;

note "runtests.fth";
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

*********************YOU SHOULD SEE THE STANDARD GRAPHIC CHARACTERS:
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
*
PLEASE TYPE UP TO 80 CHARACTERS:

RECEIVED: "hello"
*
End of Core word set tests
*********
You should see 2345: 2345
******
End of additional Core tests

Test utilities loaded
********************

Output from .(
You should see -9876: -9876 
and again: -9876


On the next 2 lines you should see First then Second messages:
First message via .( 
Second message via ."

*

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

*******
The next test should display:
One line...
another line
One line...
anotherLine

End of Core Extension word tests
*********
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
***Calculated Characters per Line: 64 
*
End of Block word tests
*****************
You should see lines duplicated:
     8970676912557384689
     8970676912557384689 
        8970676912557384689
        8970676912557384689
     -8522862768232894102
     -8522862768232894102 
          -8522862768232894102
          -8522862768232894102
**
End of Double-Number word tests
***
End of Exception word tests
****
End of Facility word tests
*******************
End of File-Access word set tests
*************
End of Locals word set tests. ( ) ****
End of Memory-Allocation word tests
**********
ONLY FORTH DEFINITIONS search order and compilation wordlist

Search order: 0 
Definitions: 0 

Plus another unnamed wordlist at the head of the search order

Search order: 0 3 
Definitions: 3 

End of Search Order word tests

Forth tests completed 

END

# floating point tests
chdir "fp" or die;

note "runfptests.fth";
forth_ok('S" runfptests.fth" INCLUDED', <<'END');

Running FP Tests


FP tests finished

END

# unlink test-generated files
unlink "../../../../fatest1.txt" 	if Test::More->builder->is_passing;
unlink "../blocks.fb" 				if Test::More->builder->is_passing;
unlink <../$test.*>					if Test::More->builder->is_passing;
unlink <$test.*>					if Test::More->builder->is_passing;
end_test;
