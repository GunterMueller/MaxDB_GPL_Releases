#!/usr/bin/perl
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


# Some often used programs in unusual places...
use Env;
use Cwd;
require "hostname.pl";
require "$TOOL/bin/sutvars$TOOLEXT";

# Okay, its not that remote, but...
$RemoteHost= hostname().".bea.sap-ag.de";

if ( cwd() ne $SUT ) {
	print "cd $SUT";
	chdir($SUT) || die "Can't change to directory $SUT: $!";
}

#if ( $DST_VERBOSE ) { ECHO=echo ; else ECHO=: ; fi

if ( $DBREMOTE ) {
  $DBREMOTE=$SERVERDB."r"; $ENV{DBREMOTE} = $DBREMOTE;
  print "Set DBREMOTE to $DBREMOTE\n" if $DST_VERBOSE;
}

system("x_server");

print "Now creating the remote database $DBREMOTE\n" if $DST_VERBOSE;

sutparam_remote();
# Since it is nesseccary, that both database servers are running with the
# same param version, here x_param is called with the local server, too...
system("x_param -d $DBNAME -u superdba,colduser BCHECK");
if ($^O !~ /mswin32/i ) { system("x_clear $DBNAME") } else { system("x_clear $DBNAME") }
system("x_start $SUTOPTION $DBNAME   > $NULL");
if ($^O !~ /mswin32/i ) { system("x_clear $DBREMOTE") } else { system("x_clear $DBREMOTE") }
system("x_start $SUTOPTION $DBREMOTE > $NULL");

diagnose($DBNAME, "init config");
diagnose($DBNAME, "activate serverdb sysdba dummy password dummy");
diagnose($DBNAME, "create serverdb '$DBREMOTE' on '$RemoteHost' sysdba sut password sut");

diagnose($DBREMOTE, "init config");
diagnose($DBREMOTE, "activate serverdb using '$DBNAME' on '$RemoteHost'");


ci($DBREMOTE, "XINSTALL.cmd", "sut sut");
ci($DBNAME,   "XINSTALL.cmd", "dummy dummy");
ci($DBNAME,   "UCDOMAIN.cmd");
ci($DBREMOTE, "MESSAGES.cmd", "sut   sut");
ci($DBNAME,   "MESSAGES.cmd", "dummy dummy");

diagnose($DBNAME,   "autosave end");
diagnose($DBNAME,   "shutdown");
diagnose($DBREMOTE, "autosave end");
diagnose($DBREMOTE, "shutdown");


sub diagnose {
  $server = @_[0]; shift;
  @command = @_;
  print "$server: @command\n" if $DST_VERBOSE;
  if ($RELVER eq "R61"){
	 system("$X_UTIL -d $server -u superdba,colduser @command");
  }
  else{
	 system("$X_DIAGNOSE -d $server -u superdba,colduser @command");
  }
}

sub ci {
  $server = @_[0]; shift;
  $file = @_[0];   shift;
  print "$server: $file" if $DST_VERBOSE;
  $tmpName = "$TMP/ci$$.cmd";
  if  ( @ARGV != 0 )  { print " ( @ARGV\n" if $DST_VERBOSE } else { print "\n" if $DST_VERBOSE; }

  # Note, that it is nessessary to write VDNEXIT at the end,
  # since otherwise slowci calls sqlabort...
  # Due to a bug in ish we can't use the quote character here; instead
  # we have to hide the quote in a variable.
  $quote="'";
  open(FILE_OUT, ">$tmpName");
  print FILE_OUT "*************************\n";
  print FILE_OUT "FILE '$file' @_!\n";
  print FILE_OUT "VDNEXIT!\n";
  print FILE_OUT "*************************\n";
  close(FILE_OUT);
  system("xci -d $server -b $tmpName");
  unlink $tmpName
}

sub sutparam_remote {

  $WrkDir = "$DBROOT/wrk/$DBREMOTE";
  system("x_param -d $DBREMOTE -u superdba,colduser BINIT");
  system("$DBROOT/pgm/putparam  $DBREMOTE _MAXDEVSPACES 4");
  system("$DBROOT/pgm/putparam  $DBREMOTE _MAXSYSDEVSPACES 1");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXARCHIVELOGS 1");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXDATADEVSPACES 1");
  system("$DBROOT/pgm/putparam  $DBREMOTE LOG_MODE DUAL");
  system("$DBROOT/pgm/putparam  $DBREMOTE LOG_SEGMENT_SIZE 400");
  system("$DBROOT/pgm/putparam  $DBREMOTE LOG_IO_QUEUE 16");
  system("$DBROOT/pgm/putparam  $DBREMOTE _MAXSERVERDB 2");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXBACKUPDEVS 4");
  system("$DBROOT/pgm/putparam  $DBREMOTE _MAXAUTOBACKUPDEVS 1");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXSERVERTASKS 12");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXUSERTASKS 2");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXDATAPAGES 3000");
  system("$DBROOT/pgm/putparam  $DBREMOTE PACKET_SIZE 32768");
  system("$DBROOT/pgm/putparam  $DBREMOTE DATA_CACHE 500");
  system("$DBROOT/pgm/putparam  $DBREMOTE DBPROC_DATA_CACHE 24");
  system("$DBROOT/pgm/putparam  $DBREMOTE DBPROC_CODE_CACHE 80");
  system("$DBROOT/pgm/putparam  $DBREMOTE CAT_CACHE_SUPPLY 96");
  system("$DBROOT/pgm/putparam  $DBREMOTE CONVERTER_CACHE 23");
  system("$DBROOT/pgm/putparam  $DBREMOTE MAXLOCKS 2500");
  system("$DBROOT/pgm/putparam  $DBREMOTE DEADLOCK_DETECTION 0");
  system("$DBROOT/pgm/putparam  $DBREMOTE RUNDIRECTORY $WrkDir");
  system("$DBROOT/pgm/putparam  $DBREMOTE OPMSG1 $NULL");
  system("$DBROOT/pgm/putparam  $DBREMOTE KERNELTRACESIZE 1000");
  system("$DBROOT/pgm/putparam  $DBREMOTE SYSDEV_001 SYSDEV-R");
  system("$DBROOT/pgm/putparam  $DBREMOTE ARCHIVE_LOG_001 ARCHLOG-R");
  system("$DBROOT/pgm/putparam  $DBREMOTE ARCHIVE_SIZE_001 900");
  system("$DBROOT/pgm/putparam  $DBREMOTE M_ARCHIVE_LOG_001 M_ARCHLOG-R");
  system("$DBROOT/pgm/putparam  $DBREMOTE DATADEV_0001 DATADEV-R");
  system("$DBROOT/pgm/putparam  $DBREMOTE DATA_SIZE_0001 2000");
  system("x_param -d $DBREMOTE -u superdba,colduser BCHECK");

  mkdir($WrkDir, 0) unless -d $WrkDir;
  system("$TOOL/Posix/touch $WrkDir/SYSDEV-R");
  system("$TOOL/Posix/touch $WrkDir/ARCHLOG-R");
  system("$TOOL/Posix/touch $WrkDir/M_ARCHLOG-R");
  system("$TOOL/Posix/touch $WrkDir/DATADEV-R");
}
