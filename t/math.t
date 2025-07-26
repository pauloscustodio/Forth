#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test +";
forth_ok("1 2 + .S", "( 3 )");
forth_nok("3 +", "\nError: stack underflow\n");

note "Test *";
forth_ok("2 4 * .S", "( 8 )");
forth_nok("3 *", "\nError: stack underflow\n");

note "Test -";
forth_ok("2 1 - .S", "( 1 )");
forth_nok("3 -", "\nError: stack underflow\n");

note "Test /";
forth_ok("14 3 / .S", "( 4 )");
forth_nok("10 0 /", "\nError: division by zero\n");
forth_nok("3 /", "\nError: stack underflow\n");

note "Test MOD";
forth_ok(" 14  3 MOD .S", "( 2 )");
forth_ok("-14  3 MOD .S", "( 1 )");
forth_ok(" 14 -3 MOD .S", "( -1 )");
forth_nok("10  0 MOD", "\nError: division by zero\n");
forth_nok("3     MOD", "\nError: stack underflow\n");

note "Test /MOD";
forth_ok(" 14 3  /MOD .S", "( 2 4 )");
forth_ok("-14 3  /MOD .S", "( 1 -5 )");
forth_ok(" 14 -3 /MOD .S", "( -1 -5 )");
forth_nok("10 0  /MOD", "\nError: division by zero\n");
forth_nok("3     /MOD", "\nError: stack underflow\n");

note "Test */";
forth_ok(" 1525 10 100 */ .S", "( 152 )");
forth_nok("1525 10   0 */", "\nError: division by zero\n");
forth_nok("1525 10     */", "\nError: stack underflow\n");

note "Test */MOD";
forth_ok("1525  14  3 */MOD .S", "( 2 7116 )");
forth_ok("1525 -14  3 */MOD .S", "( 1 -7117 )");
forth_ok("1525  14 -3 */MOD .S", "( -1 -7117 )");
forth_nok("100  10  0 */MOD", "\nError: division by zero\n");
forth_nok("1525 10    */MOD", "\nError: stack underflow\n");

note "Test FM/MOD";
forth_ok(" 1525.  14 FM/MOD .S", "( 13 108 )");
forth_ok("-1525. -14 FM/MOD .S", "( -13 108 )");
forth_ok(" 1525. -14 FM/MOD .S", "( -1 -109 )");
forth_ok("-1525.  14 FM/MOD .S", "( 1 -109 )");
forth_nok(" 100.   0 FM/MOD", "\nError: division by zero\n");
forth_nok("1525.     FM/MOD", "\nError: stack underflow\n");

note "Test UM/MOD";
forth_ok(" 1525.  14 UM/MOD .S", "( 13 108 )");
forth_nok(" 100.   0 UM/MOD", "\nError: division by zero\n");
forth_nok(" 100.     UM/MOD", "\nError: stack underflow\n");

note "Test SM/REM";
forth_ok(" 1525.  14 SM/REM .S", "( 13 108 )");
forth_ok("-1525. -14 SM/REM .S", "( -13 108 )");
forth_ok(" 1525. -14 SM/REM .S", "( 13 -108 )");
forth_ok("-1525.  14 SM/REM .S", "( -13 -108 )");
forth_nok(" 100.   0 SM/REM", "\nError: division by zero\n");
forth_nok(" 100.     SM/REM", "\nError: stack underflow\n");

note "Test M*";
forth_ok("2  4 M* .S", "( 8 0 )");
forth_ok("2 -4 M* .S", "( -8 -1 )");
forth_nok("2 M*", "\nError: stack underflow\n");

note "Test 1+";
forth_ok("2 1+ .S", "( 3 )");
forth_nok("1+", "\nError: stack underflow\n");

note "Test 1-";
forth_ok("2 1- .S", "( 1 )");
forth_nok("1-", "\nError: stack underflow\n");

note "Test 2*";
forth_ok(" 2 2* .S", "( 4 )");
forth_ok("-2 2* .S", "( -4 )");
forth_nok("2*", "\nError: stack underflow\n");

note "Test 2/";
forth_ok(" 5 2/ .S", "( 2 )");
forth_ok("-5 2/ .S", "( -3 )");
forth_nok("2/", "\nError: stack underflow\n");

note "Test NEGATE";
forth_ok(" 5 NEGATE .S", "( -5 )");
forth_ok(" 0 NEGATE .S", "( 0 )");
forth_ok("-5 NEGATE .S", "( 5 )");
forth_nok("NEGATE", "\nError: stack underflow\n");

note "Test S>D";
forth_ok(" 5 S>D .S", "( 5 0 )");
forth_ok(" 0 S>D .S", "( 0 0 )");
forth_ok("-5 S>D .S", "( -5 -1 )");
forth_nok("S>D", "\nError: stack underflow\n");

note "Test UM*";
forth_ok("-1 -1 UM* .S", "( 1 -2 )");
forth_nok("-1 UM*", "\nError: stack underflow\n");

note "Test ABS";
forth_ok(" 1 .S ABS .S", "( 1 )( 1 )");
forth_ok(" 0 .S ABS .S", "( 0 )( 0 )");
forth_ok("-1 .S ABS .S", "( -1 )( 1 )");
forth_nok("ABS", "\nError: stack underflow\n");

note "Test MAX";
forth_ok("1 2 MAX .S", "( 2 )");
forth_nok("1 MAX", "\nError: stack underflow\n");

note "Test MIN";
forth_ok("1 2 MIN .S", "( 1 )");
forth_nok("1 MIN", "\nError: stack underflow\n");

note "Test CHAR+";
forth_ok("1 CHAR+ .S", "( 2 )");
forth_ok("2 CHAR+ .S", "( 3 )");
forth_nok("CHAR+", "\nError: stack underflow\n");

note "Test CHARS";
forth_ok("1 CHARS .S", "( 1 )");
forth_ok("2 CHARS .S", "( 2 )");
forth_nok("CHARS", "\nError: stack underflow\n");

note "Test CELL+";
forth_ok("0 CELL+ .S", "( 4 )");
forth_ok("4 CELL+ .S", "( 8 )");
forth_ok("8 CELL+ .S", "( 12 )");
forth_nok("CELL+", "\nError: stack underflow\n");

note "Test CELLS";
forth_ok("0 CELLS .S", "( 0 )");
forth_ok("1 CELLS .S", "( 4 )");
forth_ok("2 CELLS .S", "( 8 )");
forth_nok("CELLS", "\nError: stack underflow\n");

note "Test WITHIN";
forth_ok("1  2 4 WITHIN .S", "( 0 )");
forth_ok("2  2 4 WITHIN .S", "( -1 )");
forth_ok("3  2 4 WITHIN .S", "( -1 )");
forth_ok("4  2 4 WITHIN .S", "( 0 )");
forth_ok("1  4 2 WITHIN .S", "( 0 )");
forth_ok("2  4 2 WITHIN .S", "( -1 )");
forth_ok("3  4 2 WITHIN .S", "( -1 )");
forth_ok("4  4 2 WITHIN .S", "( 0 )");
forth_nok("  4 2 WITHIN", "\nError: stack underflow\n");

end_test;
