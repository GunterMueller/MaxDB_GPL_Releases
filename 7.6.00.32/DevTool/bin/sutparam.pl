#!/usr/bin/perl
# @(#)sutparm       6.2.0   1996-02-09
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
use Carp;
use DBTools;
use FileHandle;

require "$TOOL/bin/sutvars$TOOLEXT";

# forces a flush after every write or print
$| = 1;

# printed to show activity
my $OneChar = ".";

if (!getopts('hd:o:n:R:') || $opt_h) {
    print <DATA>;
    exit (1);
}


$serverdb = $opt_d ? $opt_d : $SERVERDB;
if ( $serverdb eq "" ) {
  print <DATA>;
  exit(1);
}

if ($opt_o) {
    $osuser = $opt_o;
}

my $Node       = $opt_n;
my $RemoteRoot = $opt_R;

if ( $Node ) {

    if ($RELVER lt "R72") {
        print "no remote sut for release < 7.2\n";
        exit (1);
    }
    if ( !$osuser ) {
        print "for remote sut os user must be given\n";
        exit (1);
    }

}


unlink ($DbmcliProt);
unlink ($DbmcliOutput);


if ($RemoteRoot) {

    $RUNDIR = "$RemoteRoot/wrk/$SERVERDB";

    $RUNDIR =~ tr [/][\\] if ( $^O =~ /mswin32/i);

    $SYSDEV = "SYSDEV";
    $ALOG   = "ARCHLOG";
    $MLOG   = "M_ARCHLOG";
    $DATA   = "DATADEV";

} else {

    $RUNDIR = "$TEST/kern/sut";

    $RUNDIR =~ tr [/][\\] if ( $^O =~ /mswin32/i);

    if ($RELVER eq "R61") {
        $SYSDEV = "SYSDEV1";
        $LOG    = "LOGDEV1";
        $DATA   = "DATADEV1";
    } else {
        $SYSDEV = "SYSDEV";
        $ALOG   = "ARCHLOG";
        $MLOG   = "M_ARCHLOG";
        $DATA   = "DATADEV";
    }
}

print "Anlegen des param-Files fuer den SUT\n";

DBTools::stop_db($serverdb, $StdColdUser, $Node, $RemoteRoot);
print $OneChar;

DBTools::BInit($serverdb, $StdColdUser, $osuser, "OLTP", $Node, $RemoteRoot);
print $OneChar;

#=====================
# SIZE OF DATA CACHE =
#=====================

if ($RELVER eq "R61")
{
    DBTools::PutParam($serverdb, "DATA_CACHE_PAGES", "200", undef, $Node, $RemoteRoot);
    print $OneChar;
}
elsif ($RELVER lt "R72")
{
    DBTools::PutParam($serverdb, "DATA_CACHE", "500", undef, $Node, $RemoteRoot);
    print $OneChar;
}
elsif ($RELVER lt "R74")
{
    DBTools::PutParam($serverdb, "DATA_CACHE", "800", undef, $Node, $RemoteRoot);
    print $OneChar;
}

#==========================
# SIZE OF CONVERTER CACHE =
#==========================

if (($RELVER ge "R62") && ($RELVER lt "R74"))
{
    DBTools::PutParam($serverdb, "CONVERTER_CACHE", "23", undef, $Node, $RemoteRoot);
    print $OneChar;
}

#========================
# SIZE OF FRAME CONTROL =
#========================

if ($RELVER ge "R74")
{
    DBTools::PutParam($serverdb, "CACHE_SIZE", "810", undef, $Node, $RemoteRoot);          print $OneChar;
}

#===============================
# MAXIMUM NUMBER OF DATA PAGES =
#===============================

if ($RELVER eq "R61")
{
    DBTools::PutParam($serverdb, "MAXDATAPAGES", "3000", undef, $Node, $RemoteRoot);       print $OneChar;
}
elsif ($RELVER lt "R74")
{
    DBTools::PutParam($serverdb, "MAXDATAPAGES", "3500", undef, $Node, $RemoteRoot);       print $OneChar; # 2000-12-27 UH, set to 3500
}

#==================================
# CONFIGURATION OF SYSTEMDEVSPACE =
#==================================

if ($RELVER eq "R61")
{
    DBTools::PutParam($serverdb, "SYSDEVSPACE", "$SYSDEV", undef, $Node, $RemoteRoot);     print $OneChar;
}
elsif ($RELVER lt "R74")
{
    DBTools::PutParam($serverdb, "_MAXSYSDEVSPACES", "1", undef, $Node, $RemoteRoot);      print $OneChar;
    DBTools::PutParam($serverdb, "SYSDEV_001", "$SYSDEV", undef, $Node, $RemoteRoot);      print $OneChar;
}

#================================
# CONFIGURATION OF ARCHIVE LOGS =
#================================

if ($RELVER eq "R61")
{
# no operation
}
elsif ($RELVER lt "R74")
{
    DBTools::PutParam($serverdb, "LOG_MODE",           "DUAL",  undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "MAXARCHIVELOGS",     "1",     undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_SIZE_001",   "900",   undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_LOG_001",    "$ALOG", undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_TYPE_001",   "F",     undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "M_ARCHIVE_LOG_001",  "$MLOG", undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "M_ARCHIVE_TYPE_001", "F",     undef, $Node, $RemoteRoot); print $OneChar;
}
elsif (($RELVER eq "R74") && ($CORRECTION_LEVEL lt "4"))
{
    DBTools::PutParam($serverdb, "MAXARCHIVELOGS",   "1",      undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_SIZE_001", "900",    undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_LOG_001",  "$ALOG",  undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "ARCHIVE_TYPE_001", "F",      undef, $Node, $RemoteRoot); print $OneChar;
}
else
{
    DBTools::PutParam($serverdb, "MAXLOGVOLUMES",   "1",      undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "LOG_VOLUME_SIZE_001", "900",    undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "LOG_VOLUME_NAME_001",  "$ALOG",  undef, $Node, $RemoteRoot); print $OneChar;
    DBTools::PutParam($serverdb, "LOG_VOLUME_TYPE_001", "F",      undef, $Node, $RemoteRoot); print $OneChar;
}

#================================
# CONFIGURATION OF DATA VOLUMES
#================================

if ($RELVER eq "R61")
{
}
elsif (($RELVER lt "R74") || (($RELVER eq "R74") && ($CORRECTION_LEVEL lt "4")))
{
    DBTools::PutParam($serverdb, "_MAXDEVSPACES", "4", undef, $Node, $RemoteRoot);             print $OneChar;
    DBTools::PutParam($serverdb, "MAXDATADEVSPACES", "1", undef, $Node, $RemoteRoot);          print $OneChar;
    DBTools::PutParam($serverdb, "DATADEV_0001", "$DATA", undef, $Node, $RemoteRoot);          print $OneChar;
    DBTools::PutParam($serverdb, "DATA_SIZE_0001", "3000", undef, $Node, $RemoteRoot);         print $OneChar; # 2000-12-27 UH, set to 3000
    DBTools::PutParam($serverdb, "DATA_TYPE_0001", "F", undef, $Node, $RemoteRoot);         print $OneChar; # 2003-01-30 UH, added type
}
else
{
    DBTools::PutParam($serverdb, "MAXVOLUMES", "4", undef, $Node, $RemoteRoot);             print $OneChar;
    DBTools::PutParam($serverdb, "MAXDATAVOLUMES", "1", undef, $Node, $RemoteRoot);          print $OneChar;
    DBTools::PutParam($serverdb, "DATA_VOLUME_NAME_0001", "$DATA", undef, $Node, $RemoteRoot);          print $OneChar;
    DBTools::PutParam($serverdb, "DATA_VOLUME_SIZE_0001", "3000", undef, $Node, $RemoteRoot);         print $OneChar; # 2000-12-27 UH, set to 3000
    DBTools::PutParam($serverdb, "DATA_VOLUME_TYPE_0001", "F", undef, $Node, $RemoteRoot);         print $OneChar; # 2003-01-30 UH, added type
}

#=========================================
# NUMBER OF MERGE FILES FOR CREATE INDEX =
#=========================================

if (($RELVER eq "R62") || ($RELVER ge "R72"))
{
	DBTools::PutParam($serverdb, "_IDXFILE_LIST_SIZE", "128", undef, $Node, $RemoteRoot);   print $OneChar;
}

#=========================================================
#=========================================================

if ($RELVER eq "R61")
{
    DBTools::PutParam($serverdb, "TRANSACTION_LOG", "$LOG", undef, $Node, $RemoteRoot);        print $OneChar;
    DBTools::PutParam($serverdb, "MAXUSER", "4", undef, $Node, $RemoteRoot);                   print $OneChar;
    DBTools::PutParam($serverdb, "MAXBACKUPDEVS", "1", undef, $Node, $RemoteRoot);             print $OneChar;
    DBTools::PutParam($serverdb, "PROC_DATA_PAGES", "20", undef, $Node, $RemoteRoot);          print $OneChar;
    DBTools::PutParam($serverdb, "RUNDIRECTORY", "$RUNDIR", undef, $Node, $RemoteRoot);        print $OneChar;
    DBTools::PutParam($serverdb, "LOG_QUEUE_PAGES"," 10", undef, $Node, $RemoteRoot);          print $OneChar;
}
else
{
    DBTools::PutParam($serverdb, "LOG_SEGMENT_SIZE", "400", undef, $Node, $RemoteRoot);        print $OneChar;
    DBTools::PutParam($serverdb, "LOG_IO_QUEUE", "16", undef, $Node, $RemoteRoot);             print $OneChar;
    DBTools::PutParam($serverdb, "MAXBACKUPDEVS", "4", undef, $Node, $RemoteRoot);             print $OneChar;
    if (
       ($RELVER lt "R74")
       ||
       (($RELVER eq "R74") && ($CORRECTION_LEVEL eq "1"))
       )
    {
        DBTools::PutParam($serverdb, "_MAXAUTOBACKUPDEVS", "1", undef, $Node, $RemoteRoot);    print $OneChar;
    }
    DBTools::PutParam($serverdb, "MAXSERVERTASKS", "12", undef, $Node, $RemoteRoot);           print $OneChar;
    if ($RELVER ge "R75") {
        DBTools::PutParam($serverdb, "MAXUSERTASKS", "4", undef, $Node, $RemoteRoot);              print $OneChar;
    }
    else {
        DBTools::PutParam($serverdb, "MAXUSERTASKS", "2", undef, $Node, $RemoteRoot);              print $OneChar;
    }
    DBTools::PutParam($serverdb, "CAT_CACHE_SUPPLY", "96", undef, $Node, $RemoteRoot);         print $OneChar;
    DBTools::PutParam($serverdb, "MAXLOCKS", "2500", undef, $Node, $RemoteRoot);               print $OneChar;
    DBTools::PutParam($serverdb, "DEADLOCK_DETECTION", "0", undef, $Node, $RemoteRoot);        print $OneChar;
    DBTools::PutParam($serverdb, "RUNDIRECTORY", "$RUNDIR", undef, $Node, $RemoteRoot);        print $OneChar;

    if ($RELVER ge "R72")
    {
       DBTools::PutParam($serverdb, "_CAT_CACHE_MINSIZE", "131072", undef, $Node, $RemoteRoot); print $OneChar;
       DBTools::PutParam($serverdb, "REQUEST_TIMEOUT", "180", undef, $Node, $RemoteRoot);      print $OneChar;
       DBTools::PutParam($serverdb, "LOG_IO_QUEUE", "17", undef, $Node, $RemoteRoot);          print $OneChar;
    }
    if (
       (($RELVER eq "R72") && ($CORRECTION_LEVEL ge "5"))
       ||
       ($RELVER eq "R73")
       ||
       ($RELVER ge "R74")
       )
    {
       DBTools::PutParam($serverdb, "DIAG_HISTORY_PATH", "$RUNDIR/DIAGHISTORY", undef, $Node, $RemoteRoot); print $OneChar;
    }

    if (($RELVER eq "R75") && ($BUILDPRAEFIX ge "33"))
    {
    	DBTools::PutParam($serverdb, "CHECK_TABLE_WIDTH", "YES", undef, $Node, $RemoteRoot); print $OneChar;
    }

    if ( ($RELVER ge "R75" ) && ( $^O =~ /hpux/ ))
    {
    	DBTools::PutParam($serverdb, "_MAXTASK_STACK", "500", undef, $Node, $RemoteRoot); print $OneChar;
    }

    if (
       (($RELVER eq "R72") && ($CORRECTION_LEVEL ge "5"))
       ||
       ($RELVER gt "R72")
       )
    {
       DBTools::PutParam($serverdb, "TRACE_PAGES_US", "450", undef, $Node, $RemoteRoot);       print $OneChar;
    }
    if (($RELVER eq "R72") && ($CORRECTION_LEVEL lt "5"))
    {
       DBTools::PutParam($serverdb, "KERNELTRACESIZE", "1000", undef, $Node, $RemoteRoot);     print $OneChar;
    }
    if ($RELVER lt "R72")
    {
      DBTools::PutParam($serverdb, "KERNELTRACESIZE", "1000", undef, $Node, $RemoteRoot);      print $OneChar;
      DBTools::PutParam($serverdb, "PACKET_SIZE", "32768", undef, $Node, $RemoteRoot);         print $OneChar;
      if ($RELVER eq "R61")
      {
        DBTools::PutParam($serverdb, "_MAXSERVERDB", "2", undef, $Node, $RemoteRoot);          print $OneChar;
      }
      else
      {
        DBTools::PutParam($serverdb, "_MAXSERVERDB", "1", undef, $Node, $RemoteRoot);          print $OneChar;
      }
    }
    else
    {
        if ($ENV{page32KB} =~ /yes|true/i)
        {
            DBTools::PutParam($serverdb, "_PACKET_SIZE", "49152", undef, $Node, $RemoteRoot);  print $OneChar;
            DBTools::PutParam($serverdb, "_CAT_CACHE_MINSIZE", "262144", undef, $Node, $RemoteRoot); print $OneChar;
        }
        else
        {
            if ($ENV{vers80} =~ /yes|true/i)
            {
                DBTools::PutParam($serverdb, "_PACKET_SIZE", "49152", undef, $Node, $RemoteRoot);  print $OneChar;
                DBTools::PutParam($serverdb, "_CAT_CACHE_MINSIZE", "262144", undef, $Node, $RemoteRoot); print $OneChar;
            }
            else
            {
                DBTools::PutParam($serverdb, "_PACKET_SIZE", "32768", undef, $Node, $RemoteRoot);  print $OneChar;
            }
        }
    }
    if ($RELVER ge "R77") {
        DBTools::PutParam($serverdb, "EXTERNAL_DUMP_REQUEST", "YES", undef, $Node, $RemoteRoot);  print $OneChar;
    }
}
DBTools::PutParam($serverdb, "SESSION_TIMEOUT", "420", undef, $Node, $RemoteRoot);  print $OneChar;


DBTools::BCheck($serverdb, $StdColdUser, $Node, $RemoteRoot); print $OneChar;

print "\n";


sub check_dev
{
  foreach $i (@_) {
    if ( $i=~  /\w:/ ) { $DEV_NAME = $i }
    else { $DEV_NAME = "$RUNDIR/$i" }
    if ( ! -f $DEV_NAME ) {
      system("$TOOL/Posix/touch $DEV_NAME");
      system("$TOOL/Posix/chmod a+rw $DEV_NAME");
      print "$DEV_NAME touched.\n";
    }
  }
}

if ($RELVER eq "R61") {
   check_dev ($SYSDEV, $LOGDEV, $DATADEV);
} else {
    if ($RELVER ge "R74"){
       check_dev ($ALOG,$MLOG,$DATA);
    } else {
       check_dev ($SYSDEV,$ALOG,$MLOG,$DATA);
    }
}


__DATA__


 USAGE: sutparam [-h] [-n node -R remoteroot] [-d <serverdb>] [-o osuser,password]

        Creates a database for the single user tests

 OPTIONS:
       -h provides this help information
       -d specifies serverdb to create
       -n specifies the remote host serverdb to create on
       -R specifies the dbroot directory on remote host
       -o specifies the operating system user and password (Unix only)





