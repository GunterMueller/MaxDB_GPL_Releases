#!/usr/bin/perl
# pass call to Python script, vmake supports only Perl for execscript
#
system ("python " . $ENV{'TOOL'} . '/bin/extractDTD.py ' . "@ARGV");

