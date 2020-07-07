#!/usr/bin/perl
# pcvt
#
# @(#)pcvt  7.1  1998-12-15
#
# Switch kernel vtrace on.
#
# Burkhard Diesing
#
# last change: 2000-08-11 Marco Paskamp - clear enhancement
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

use Env;
use Getopt::Std;

sub PrintErrorProt;
$DBMUSR=$ENV{DBMUID};
$DBMPWD=$ENV{DBMPWD};
$DBMCLI_PROG="dbmcli";
$DBMCLI_PROT="dbmcli.prt";
$DBKERNPROT="xkernprot";
$DBKERNSWITCH="akbxm";

if (!getopts ('hd:u:s:f:v') || $opt_h ) {
   print <DATA>;
   exit;
}

if ($ENV{RELVER} eq "R62" || $ENV{RELVER} eq "R71")
{
    $DBMOPT="-uDBM ";
}
else {
    if ($ENV{RELVER} ge "R72")
    {
	$DBMOPT="-uUTL ";
    }
    else
    {
	print "Unsupported release ($ENV{RELVER})\n";
	exit -1;
    }
}

if ($opt_d) {
   $SERVERDB=$opt_d;
}

if ($opt_u =~ /(.*),(.*)/) {
    $DBMUSR = $1;
    $DBMPWD = $2;
}
if ($opt_s){
    $sessionid = $opt_s;
    if ($opt_f){
       $filename = $opt_f;
    }
    else {
       $filename = $SERVERDB.".prt";
    }
}

if ((@ARGV < 1 && ! defined($sessionid)) || @ARGV > 2) {
   print <DATA>;
   exit;
}

my($SWITCH)=$ARGV[0];
if ( ! ($SWITCH=~/^on$/)
    && ! ($SWITCH=~/^off$/)
    && ! ($SWITCH=~/^flush$/)
    && ! ($SWITCH=~/^clear$/)
    && ! ($SWITCH=~/^create$/)
    && ! (defined($sessionid))
    ) {
   print <DATA>;
   exit;
}

if ($opt_s){
  filterSession($filename, $sessionid);
  print "filtering tracefile: $filename for session T$sessionid\n";
  print "OUTPUTFILE: T".$sessionid."_".$filename."\n";
  exit;
}

if ( $SWITCH=~/^create$/ && @ARGV > 1)  {
    $DBKERNSWITCH=$ARGV[1];
}

my($CMD)="$DBMCLI_PROG -u $DBMUSR,$DBMPWD -d $SERVERDB -o $DBMCLI_PROT ";
print $CMD if ($opt_v);
if ( $SWITCH=~/^clear$/ ) {
	$CMD .= "-uUTL $DBMUSR,$DBMPWD util_execute diagnose vtrace clear";
    print "$CMD\n" if ($opt_v);
    my($RC)=system($CMD); print "RC=$RC\n" if ($opt_v);
    if ($RC != 0)
    {
	PrintErrorProt;
	}
	else
	{
		print "Kernel vtrace for $SERVERDB has been cleared.";
	}
}
else
{
if ( $SWITCH=~/^create$/ ) {
    $CMD .= "-uSQL $DBMUSR,$DBMPWD sql_execute vtrace";
    print "$CMD\n" if ($opt_v);
    my($RC)=system($CMD); print "RC=$RC\n" if ($opt_v);
    if ($RC != 0)
    {
	PrintErrorProt;
    }
    else
    {
	$CMD = "$DBKERNPROT -d $SERVERDB $DBKERNSWITCH";
	print "$CMD\n" if ($opt_v);
	my($RC)=system($CMD); print "RC=$RC\n" if ($opt_v);
    }
}
else {
    if ( $SWITCH=~/^flush$/ ) {
	$CMD .= "-uSQL $DBMUSR,$DBMPWD sql_execute vtrace";
	print "$CMD\n" if ($opt_v);
    }
    else {
	$CMD .= "-uUTL $DBMUSR,$DBMPWD util_execute diagnose vtrace default $SWITCH";
	print "$CMD\n" if ($opt_v);
    }
    my($RC)=system($CMD); print "RC=$RC\n" if ($opt_v);
    if ($RC != 0)
    {
	PrintErrorProt;
    }
    else {
	if ( $SWITCH=~/^flush$/ ) {
	    print "Kernel vtrace for $SERVERDB has been flushed.";
	}
	else {
	    print "Kernel vtrace for $SERVERDB has been switched $SWITCH.";
	}
    }
}
}
sub PrintErrorProt
{
    open(FILE_IN, "<$DBMCLI_PROT") || die "Can't open $DBMCLI_PROT: $!";
    while(<FILE_IN>) {
	print STDERR $_;
    }
    close(FILE_IN);
}

sub filterSession {
  my ($filename,$Sess ) = @_;
  my $TempFile="T".$Sess."_".$filename;
  my $writeflag=1;
    open(FILE_IN, "<$filename")  || die "Can't open $filename (input): $!";
    open(FILE_OUT, ">$TempFile") || die "Can't open $TempFile (output): $!";
    while(<FILE_IN>) {
      if(/^===== T.*/) {
        $writeflag = 0;
      }
      if(/^===== T$Sess.*/) {
        $writeflag = 1;
      }
      if ($writeflag) {
        s/^===== T.*//i;
	print FILE_OUT $_;
      }
    }
    close(FILE_OUT); close(FILE_IN);
}
__DATA__

 USAGE: pcvt [-h | [-d dbname] [-u uid,pwd] [-v] <cmd> [<switch>]
                 | -s session [-f protfile] ]

           Switch kernel vtrace on or off or filter the protfile.

 OPTIONS:    -h           (prints this help)
             -d dbname    (default $SERVERDB)
             -u uid,pwd   (default control,control)
             -v           verbose
             -s session   session number in vtrace (T23 -> 23)
             -f protfile  (default ./$SERVERDB.prt)

 <cmd>:
	     on | off | flush | clear | create

 <switch>    Only with create. <switch> is passed to $DBKERNPROT.
             Default for <switch> is "$DBKERNSWITCH".
