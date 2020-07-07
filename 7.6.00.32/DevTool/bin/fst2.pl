#!/usr/bin/perl

use Env ;
use imake ;
use File::Path ;
use Getopt::Long;
use POSIX;
use File::Copy;

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

my %ENV_SAV = ("1" => "eins",
               "2" => "zwei");
# my @ENV_SAV_LIST = %ENV ;
# my %ENV_SAV = @ENV_SAV_LIST ;
%ENV_SAV = %ENV ;

print "\n\n--------------------------------------\n" ;
print "---   print \%ENV       --------------\n" ;
print "--------------------------------------\n" ;
while (( $schluessel , $wert ) = each %ENV )
{
   print "$schluessel = $wert\n" ;
}

print "\n\n--------------------------------------\n" ;
print "---   print \%ENV_SAV   --------------\n" ;
print "--------------------------------------\n" ;
while (( $s , $w ) = each %ENV_SAV )
{
   print "$s = $w\n" ;
}

print "\n\n--------------------------------------\n" ;
print "---   delete \%ENV      --------------\n" ;
print "--------------------------------------\n" ;
while (( $schluessel , $wert ) = each %ENV )
{
   delete ( $ENV { $schluessel }) ;
}

print "\n\n--------------------------------------\n" ;
print "---   new    \%ENV      --------------\n" ;
print "--------------------------------------\n" ;
$ENV { "FST________________1" } = "FST\+\+\+\+\+\+\+\+\+1" ;
$ENV { "FST________________2" } = "FST\+\+\+\+\+\+\+\+\+2" ;
$ENV { "FST________________3" } = "FST\+\+\+\+\+\+\+\+\+3" ;
$ENV { "SHELL" }                = "FST\+\+\+\+\+\+\+\+\+4" ;

print "\n\n--------------------------------------\n" ;
print "---   print \%ENV       --------------\n" ;
print "--------------------------------------\n" ;
while (( $schluessel , $wert ) = each %ENV )
{
   print "$schluessel = $wert\n" ;
}
print "\n\n--------------------------------------\n" ;
print "---   print \%ENV_SAV again   --------\n" ;
print "--------------------------------------\n" ;
while (( $schluessel , $wert ) = each %ENV_SAV )
{
   print "$schluessel = $wert\n" ;
}

%ENV = %ENV_SAV ;

print "\n\n--------------------------------------\n" ;
print "---   print \%ENV again   ------------\n" ;
print "--------------------------------------\n" ;
while (( $schluessel , $wert ) = each %ENV )
{
   print "$schluessel = $wert\n" ;
}

