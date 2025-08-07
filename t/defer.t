#!/usr/bin/perl

BEGIN { use lib 't'; require 'testlib.pl'; }

note "Test DEFER";
note "Test DEFER@";
note "Test DEFER!";
forth_nok("DEFER hello hello .S", "");
forth_ok("DEFER hello ' * ' hello DEFER!  2 3 hello .S", "( 6 )");
forth_ok("DEFER hello ' + ' hello DEFER!  2 3 hello .S", "( 5 )");
forth_ok("DEFER hello ' + ' hello DEFER!  ' hello DEFER@  ' + =  .S", "( -1 )");

note "Test ACTION-OF";
my $action1 = "DEFER hello  : action-of-hello ACTION-OF hello ;  ' * ' hello DEFER!  2 3 hello .S";
my $action2 = "ACTION-OF hello  ' * = .S";
my $action3 = "action-of-hello  ' * = .S";
forth_ok("$action1", "( 6 )");
forth_ok("$action1 $action2", "( 6 )( 6 -1 )");
forth_ok("$action1 $action2 $action3", "( 6 )( 6 -1 )( 6 -1 -1 )");

note "Test IS";
forth_ok("DEFER hello : is-hello IS hello ; ' * IS hello 2 3 hello . ' + is-hello 2 3 hello . .S",
		"6 5 ( )");

end_test;
