#!/usr/bin/perl
#
use Getopt::Long;
use SDB::Remote::WebMake;

GetOptions ('pause!');

my $host = shift;
my $cmd = shift;

SDB::Remote::WebMake::run_make($host,$cmd);

if (defined $opt_pause)
{
	print "Press any key to continue . . .";
	<STDIN>;
}
