#!/usr/bin/perl
# compant.pl
use Env;
use File::Basename;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }

ToolTrace("compant.pl called with @ARGV\n");

my $ANTCMD = $ANT_CALL;
$ANTCMD .= " ". (join " ", @ANT_FLAGS);
$ANTCMD .= " ". (join " ", @ARGV);

my $MAKESTATE = 1;

defined $ENV{NOQUIET} and print "$ANTCMD\n";

open (ANTMAKE, "$ANTCMD 2>&1 |") or die "Can't start ant command\n";
while (<ANTMAKE>)
{
   print "$_";
   (/BUILD SUCCESSFUL/i) and $MAKESTATE = 0;
   (/BUILD FAILED/i) and $MAKESTATE = 1;
}
exit ($MAKESTATE);




