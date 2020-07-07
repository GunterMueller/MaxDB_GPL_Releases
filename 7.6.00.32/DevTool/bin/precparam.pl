#!/usr/bin/perl
# @(#)precparam		6.2.8   1998-12-15
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

if (!getopts('bhd:') || $opt_h) {
    print <DATA>;
    exit (1);
}


$serverdb = $opt_d ? $opt_d : $SERVERDB;
if ( $serverdb eq "" ) {
  print <DATA>;
  exit(1);
}

print "Anlegen des param-Files fuer die PREC\n";
print "------------------------------------\n...";
sleep 1;

$GETPARAM = "$DBROOT/pgm/getparam $serverdb";
$PUTPARAM = "$DBROOT/pgm/putparam $serverdb";
$XPARAM   = "x_param -d $serverdb";
system("$XPARAM -u superdba,colduser BINIT"); print ".";

if ($RELVER eq "R61"){
	system("$PUTPARAM SYSDEVSPACE $OWN/dev/SYSDEV1"); print ".";
	system("$PUTPARAM TRANSACTION_LOG $OWN/dev/LOGDEV1"); print ".";
	system("$PUTPARAM MAXUSER 4"); print ".";
	# system("$PUTPARAM MAXDEVSPACES 3"); print ".";
	# system("$PUTPARAM MAXDATADEVSPACES 1"); print ".";
	system("$PUTPARAM MAXDATAPAGES 6000"); print ".";
	system("$PUTPARAM MAXBACKUPDEVS 1"); print ".";
	# system("$PUTPARAM MAXSERVERDB 1"); print ".";
	# system("$PUTPARAM MAXCPU 1"); print ".";
	system("$PUTPARAM DATA_CACHE_PAGES 400"); print ".";
	# system("$PUTPARAM CONV_CACHE_PAGES 23"); print ".";
	system("$PUTPARAM PROC_DATA_PAGES 40"); print ".";
	system("$PUTPARAM RUNDIRECTORY $OWN/test/pc/wrk"); print ".";
	system("$PUTPARAM KERNELTRACESIZE 1000"); print ".";
	system("$PUTPARAM LOG_QUEUE_PAGES 20"); print ".";
}
else{
	system("$PUTPARAM _MAXDEVSPACES 4"); print ".";
	system("$PUTPARAM _MAXSYSDEVSPACES 1"); print ".";
	system("$PUTPARAM MAXARCHIVELOGS 1"); print ".";
	system("$PUTPARAM MAXDATADEVSPACES 1"); print ".";
	system("$PUTPARAM LOG_MODE DUAL"); print ".";
	system("$PUTPARAM LOG_SEGMENT_SIZE 600"); print ".";
	system("$PUTPARAM LOG_IO_QUEUE 24"); print ".";
    system("$PUTPARAM _MAXSERVERDB 2"); print ".";
	system("$PUTPARAM MAXBACKUPDEVS 4"); print ".";
	system("$PUTPARAM _MAXAUTOBACKUPDEVS 1"); print ".";
	system("$PUTPARAM MAXSERVERTASKS 12"); print ".";
	system("$PUTPARAM MAXUSERTASKS 4"); print ".";
	system("$PUTPARAM MAXDATAPAGES 6000"); print ".";
	system("$PUTPARAM PACKET_SIZE 32768"); print ".";
	system("$PUTPARAM DATA_CACHE 500"); print ".";
	system("$PUTPARAM CAT_CACHE_SUPPLY 96"); print ".";
	system("$PUTPARAM CONVERTER_CACHE 23"); print ".";
	system("$PUTPARAM MAXLOCKS 2500"); print ".";
	system("$PUTPARAM DEADLOCK_DETECTION 0"); print ".";
	system("$PUTPARAM RUNDIRECTORY $OWN/test/pc/wrk"); print ".";
	system("$PUTPARAM KERNELTRACESIZE 1000"); print ".";

	system("$PUTPARAM SYSDEV_001 $OWN/dev/SYSDEV");
	print ".";
	system("$PUTPARAM ARCHIVE_LOG_001 $OWN/dev/ARCHLOG");
	print ".";
	system("$PUTPARAM ARCHIVE_SIZE_001 2000");
	print ".";
	system("$PUTPARAM M_ARCHIVE_LOG_001 $OWN/dev/M_ARCHLOG");
	print ".";
	system("$PUTPARAM DATADEV_0001 $OWN/dev/DATADEV");
	print ".";
	system("$PUTPARAM DATA_SIZE_0001 4000");
        print ".";
}

system("$XPARAM -u superdba,colduser BCHECK"); print ".";

if ( $opt_b != 1 ) {

	print "\nJetzt folgt gleich ein x_param-Aufruf\n";
	print "Alle Parameter sollten noch einmal kontrolliert werden\n\n";
	print "Weiter mit einer Taste ...\n";
	<STDIN>;

	if ($RELVER eq "R61"){
		system("$XPARAM -u superdba,colduser");
	}
	else{
		system("$XPARAM -u superdba,colduser SHOWALL");
	}
}



sub check_dev
{
  $RUNDIR = `$GETPARAM RUNDIRECTORY`; chomp $RUNDIR;
  foreach $i (@_) {
    if ( $i=~  /\w:/ ) { $DEV_NAME = $i }
    else { $DEV_NAME = "$RUNDIR/$i" }
    if ( ! -f $DEV_NAME ) {
      system("$TOOL/Posix/touch $DEV_NAME");
      print "$DEV_NAME touched.\n";
    }
  }
}

if ($RELVER eq "R61") {
	$SYSDEV = `$GETPARAM SYSDEVSPACE`;		chomp $SYSDEV;
	$LOGDEV = `$GETPARAM TRANSACTION_LOG`;	chomp $LOGDEV;
	$DATDEV = "$OWN/dev/DATADEV1";
	check_dev ($SYSDEV, $LOGDEV, $DATADEV);
} else {
	$SYSDEV= `$GETPARAM SYSDEV_001`;		chomp $SYSDEV;
	$ALOG  = `$GETPARAM ARCHIVE_LOG_001`;	chomp $ALOG;
	$MLOG  = `$GETPARAM M_ARCHIVE_LOG_001`; chomp $MLOG;
	$DATA  = `$GETPARAM DATADEV_0001`;		chomp $DATA;
	check_dev ($SYSDEV,$ALOG,$MLOG,$DATA);
}


if ($RELVER ne "R61"){
	if ( $^O =~ /mswin32/i) {
		system("xservice -d $serverdb -u -s");
		system("xservice -d $serverdb -u -q");
		system("xservice -d $serverdb -u");
	}
}


__DATA__


 USAGE: sutparam [-h] [-d <serverdb>]

        Creates a database for the single user tests

 OPTIONS:
       -h provides this help information
       -d specifies serverdb to create




