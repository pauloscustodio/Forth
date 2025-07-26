#!/usr/bin/env perl

#------------------------------------------------------------------------------
# C++ implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2025
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

use Modern::Perl;
use Path::Tiny;

my $words = `./forth -e words`;
my $readme = path("README.md")->slurp;
$readme =~ s/```.*```/```\n$words\n```/s;
path("README.md")->spew($readme);
