#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# usage
capture_nok("./forth -h", "Usage: forth [-e forth] [source [args...]]\n");

note "Test THROW";
forth_ok("1 0 THROW .", "1 ");
forth_nok("-1 THROW not reached", "\nError: ABORT\n");
forth_nok("-2 THROW not reached", "\nError: ABORTQuote\n");
forth_nok("-3 THROW not reached", "\nError: stack overflow\n");
forth_nok("-4 THROW not reached", "\nError: stack underflow\n");
forth_nok("-5 THROW not reached", "\nError: return stack overflow\n");
forth_nok("-6 THROW not reached", "\nError: return stack underflow\n");
forth_nok("-7 THROW not reached", "\nError: do-loops nested too deeply during execution\n");
forth_nok("-8 THROW not reached", "\nError: dictionary overflow\n");
forth_nok("-9 THROW not reached", "\nError: invalid memory address\n");
forth_nok("-10 THROW not reached", "\nError: division by zero\n");
forth_nok("-11 THROW not reached", "\nError: result out of range\n");
forth_nok("-12 THROW not reached", "\nError: argument type mismatch\n");
forth_nok("-13 THROW not reached", "\nError: undefined word\n");
forth_nok("-14 THROW not reached", "\nError: interpreting a compile-only word\n");
forth_nok("-15 THROW not reached", "\nError: invalid FORGET\n");
forth_nok("-16 THROW not reached", "\nError: attempt to use zero-length string as a name\n");
forth_nok("-17 THROW not reached", "\nError: pictured numeric output string overflow\n");
forth_nok("-18 THROW not reached", "\nError: parsed string overflow\n");
forth_nok("-19 THROW not reached", "\nError: definition name too long\n");
forth_nok("-20 THROW not reached", "\nError: write to a read-only location\n");
forth_nok("-21 THROW not reached", "\nError: unsupported operation\n");
forth_nok("-22 THROW not reached", "\nError: control structure mismatch\n");
forth_nok("-23 THROW not reached", "\nError: address alignment exception\n");
forth_nok("-24 THROW not reached", "\nError: invalid numeric argument\n");
forth_nok("-25 THROW not reached", "\nError: return stack imbalance\n");
forth_nok("-26 THROW not reached", "\nError: loop parameters unavailable\n");
forth_nok("-27 THROW not reached", "\nError: invalid recursion\n");
forth_nok("-28 THROW not reached", "\nError: user interrupt\n");
forth_nok("-29 THROW not reached", "\nError: compiler nesting\n");
forth_nok("-30 THROW not reached", "\nError: obsolescent feature\n");
forth_nok("-31 THROW not reached", "\nError: >BODY used on non-CREATEd definition\n");
forth_nok("-32 THROW not reached", "\nError: invalid name argument (e.g., TO xxx)\n");
forth_nok("-33 THROW not reached", "\nError: block read exception\n");
forth_nok("-34 THROW not reached", "\nError: block write exception\n");
forth_nok("-35 THROW not reached", "\nError: invalid block number\n");
forth_nok("-36 THROW not reached", "\nError: invalid file position\n");
forth_nok("-37 THROW not reached", "\nError: file I/O exception\n");
forth_nok("-38 THROW not reached", "\nError: non-existent file\n");
forth_nok("-39 THROW not reached", "\nError: unexpected end of file\n");
forth_nok("-40 THROW not reached", "\nError: invalid BASE for floating point conversion\n");
forth_nok("-41 THROW not reached", "\nError: loss of precision\n");
forth_nok("-42 THROW not reached", "\nError: floating-point divide by zero\n");
forth_nok("-43 THROW not reached", "\nError: floating-point result out of range\n");
forth_nok("-44 THROW not reached", "\nError: floating-point stack overflow\n");
forth_nok("-45 THROW not reached", "\nError: floating-point stack underflow\n");
forth_nok("-46 THROW not reached", "\nError: floating-point invalid argument\n");
forth_nok("-47 THROW not reached", "\nError: compilation word list deleted\n");
forth_nok("-48 THROW not reached", "\nError: invalid POSTPONE\n");
forth_nok("-49 THROW not reached", "\nError: search-order overflow\n");
forth_nok("-50 THROW not reached", "\nError: search-order underflow\n");
forth_nok("-51 THROW not reached", "\nError: compilation word list changed\n");
forth_nok("-52 THROW not reached", "\nError: control-flow stack overflow\n");
forth_nok("-53 THROW not reached", "\nError: exception stack overflow\n");
forth_nok("-54 THROW not reached", "\nError: floating-point underflow\n");
forth_nok("-55 THROW not reached", "\nError: floating-point unidentified fault\n");
forth_nok("-56 THROW not reached", "\nError: QUIT\n");
forth_nok("-57 THROW not reached", "\nError: exception in sending or receiving a character\n");
forth_nok("-58 THROW not reached", "\nError: [IF], [ELSE], or [THEN] exception\n");
forth_nok("-256 THROW not reached", "\nError: input buffer overflow\n");
forth_nok("-257 THROW not reached", "\nError: invalid number\n");
forth_nok("-258 THROW not reached", "\nError: out of memory\n");
forth_nok("-259 THROW not reached", "\nError: string too long\n");
forth_nok("-260 THROW not reached", "\nError: control-flow stack underflow\n");
forth_nok("-261 THROW not reached", "\nError: too many open files\n");
forth_nok("-262 THROW not reached", "\nError: invalid Forth code\n");
forth_nok("-263 THROW not reached", "\nError: memory overflow\n");

end_test;
