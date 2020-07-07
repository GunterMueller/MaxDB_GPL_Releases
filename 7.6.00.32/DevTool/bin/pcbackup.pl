#!/usr/bin/perl
# pcbackup
#
# @(#)pcclear  6.2  1998-01-09
#
# Copys $SRC and $DESC to the backup Server.
# For perforce releases copys all open files to the backup server
#
# Burkhard Diesing
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

package pcbackup;
use Env;
use Getopt::Std;
use ICopy;
sub date;
sub _mkdir;
sub _system;
sub EchoOff;
sub EchoOn;


#  Main

if (!getopts ('hvn') || $opt_h )
{
    print <DATA>;
    exit;
}

$useperf=0;
$relver = $ENV{RELVER};
if ($relver eq "") {
  print "Environment variable RELVER not set.\n";
  exit;
} elsif ( $relver ge "R74" ) {
  $useperf = 1;;
}

$BACKUPSRV="//dber010/HOME/$USERNAME";
$DATE=date();
if ($useperf) {
    open(P1, "p4 opened |" ) || die "Can't open pipe 'p4 opened' : $!\n";
    while(<P1>) {
	s/#[0-9]+ - .*$//g;
        open(P2, "p4 where $` |" ) || die "Can't open pipe 'p4 where $`' : $!\n";
        while(<P2>) {
            m/(.*) (.*) (.*)/;
            $source=$3;
            $OWN=$3;$OWN =~ s/://g;$OWN =~ s/\//\\/g;
            $OWN =~ s/(.*)\\.*$/$1/g;
            $BACKUPDIR="$BACKUPSRV/$DATE/$OWN"; $BACKUPDIR =~ s/\//\\/g;
            if (! -d "$BACKUPDIR" ) {
                _system("cmd /C mkdir $BACKUPDIR");
            }
            print "backup of $source\n" if ( $opt_v);
            _system("copy $source $BACKUPDIR");
        }
        close(P2);
    }
    close(P1);
}
else {
$OWN=@ENV{OWN};$OWN =~ s/://g;$OWN =~ s/\//\\/g;
$BACKUPDIR="$BACKUPSRV/$DATE/$OWN"; $BACKUPDIR =~ s/\//\\/g;
@dirs = ("$SRC", "$DESC");
foreach $dir (@dirs) {
    if ( -d "$dir" ) {
	_mkdir("$BACKUPDIR");
	print "backup of $dir\n" if ( $opt_v);
	_system("xcopy $dir $BACKUPDIR /S");
    }
}
}
sub _mkdir
{
    if (! -d $_[0] ) {
	mkdir ($_[0], 0)  unless ( $opt_n );
    }
}

sub _system
{
    print "$_[0]\n" if ( $opt_v);
    EchoOff if ( $opt_v );
    system("$_[0]") unless ( $opt_n );
    EchoOn if ( $opt_v );
}

sub EchoOff
{
    open (SAVEOUT, ">&STDOUT");
    close STDOUT;
    open (STDOUT, ">NUL");
}

sub EchoOn
{
    close STDOUT;
    open(STDOUT, ">&SAVEOUT");
    close SAVEOUT;
}

sub date
{
    $Year_index = \5;	#constant
    $Month_index = \4;	#constant
    $Day_index = \3;	#constant
    $Hour_index = \2;	#constant
    $Min_index = \1;	#constant
    $Sec_index = \0;	#constant
    @localtime = localtime();
    $localtime[$$Month_index]++;
    $localtime[$$Year_index]+=1900;
    return sprintf("%04d.%02d.%02d", $localtime[$$Year_index], $localtime[$$Month_index], $localtime[$$Day_index]);
}

__DATA__

 USAGE: pcbackup [-vhn]

           Copys $SRC and $DESC to the backup Server.

 OPTIONS:    -h        prints this help.
             -v        verbose
             -n        no execution
