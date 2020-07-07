
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
sub EchoOff;
sub EchoOn;
sub PrintTemp;
sub CheckErr;

$TEMPFILE=".temp";

if ($ENV{RELVER} eq "R71")
{
    $DBMOPT="-uDBM ";
    $SHOWSTATE="show state";
    $DBMEDIA="backup_media_put autosave FILE autosave 0 YES";
}
else {
    if ($ENV{RELVER} ge "R72")
    {
	$DBMOPT="-u ";
	$SHOWSTATE="db_state";
	$DBMEDIA="backup_media_put autosave autosave FILE AUTO 0 8 YES"
	}
    else {
	if ($ENV{RELVER} ne "R62")
	{
	    print "Unsupported release ($ENV{RELVER})";
	    exit -1;
	}
    }
}

if (!getopts ('hd:') || $opt_h )
{
    print <DATA>;
    exit;
}

if ($opt_d)
{
    $SERVERDB=$opt_d;
}

if ($ENV{RELVER} eq "R62")
{
    $CMD="xcontrol.exe -d $SERVERDB -u control,control -b OFFLINE";
    system ($CMD);
    $CMD="xcontrol.exe -d $SERVERDB -u control,control -b WARM";
    system ($CMD);
}
else {
    print "Starting instance $ENV{SERVERDB}.\n";
    EchoOff;
    $CMD="| $ENV{INSTROOT}/pgm/dbmcli.exe -s -d $SERVERDB $DBMOPT control,control -o $TEMPFILE";
    open (DBMCLI, $CMD);
    print DBMCLI "util_connect\n";
    print DBMCLI "autosave_off\n";
    print DBMCLI "$DBMEDIA\n";
    print DBMCLI "dboffline\n";
    print DBMCLI "dbwarm\n";
    print DBMCLI "util_connect\n";
    print DBMCLI "autosave_on autosave\n";
    close DBMCLI;
    EchoOn;

    PrintTemp;
    CheckErr;

    EchoOff;
    open (DBMCLI, "| $ENV{INSTROOT}/pgm/dbmcli.exe -s -d $SERVERDB $DBMOPT control,control -o $TEMPFILE");
    print DBMCLI " $SHOWSTATE\n";
    close DBMCLI;
    EchoOn;

    PrintTemp;
    CheckErr;
}

sub CheckErr
{
    open (TEMP, "<$TEMPFILE") || die "Can't open $TEMPFILE (input): $!";
    while(<TEMP>) {
	if ( /^ERR.*$/ ) {
	    die $_;
	}
    }
    close(TEMP);
}

sub PrintTemp
{
    open (TEMP, "<$TEMPFILE") || die "Can't open $TEMPFILE (input): $!";
    while(<TEMP>) {
	if ( /^>.*$/ || /^OK.*$/ || /^---.*$/ || /^\n$/) {
	}
	else {
	    print $_;
	}
    }
    close(TEMP);
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

__DATA__

 USAGE: pcstartdb [-h | -d dbname]

           Starts a database instance .

 OPTIONS:    -h        (prints this help)
             -d dbname (Default $SERVERDB)

