#!/usr/bin/perl
#
# saveLGP.pl Script
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

$| = 1;

use Env;
use Getopt::Long;

#--------------------------------------------------------------------------
# Modify these if needed.... They are not automatically maintained!!!!!!!
#
$DEVCORRECTIONLEVEL = sprintf "%02d", $ENV{CORRECTION_LEVEL};
$DEVBUILD = sprintf "%02d", $ENV{BUILDPRAEFIX};
#--------------------------------------------------------------------------


if ( -r "/rel/TOOL/tool/bin/remuserEnv.pl" ) {
    $ENV{'TOOL'}="/rel/TOOL/tool";
    $IgmeVersion="7.4.${DEVCORRECTIONLEVEL}.develop";
    do "/rel/TOOL/tool/bin/remuserEnv.pl" || die "Cant execute local Env 'remuserEnv.pl'\n";
	if ( ($No32Bit == 1)
	  || ($No64Bit == 1) ) {
		$OWN="/rel/74";
	}
	else
	{
		$OWN="/rel/74_64";
	}
}
else
{
	$OWN="D:\\";
}

do "$ENV{'TOOL'}\\bin\\remuser_mach$ENV{'TOOLEXT'}";

# Subdirectory where history is stored
$HISTORY="packageHistory";
$PACKAGE="SAPDB74${DEVCORRECTIONLEVEL}_${DEVBUILD}.SAR";
$LINE_SEPARATOR="------------------------------------------------------------------------\n";

#------------------------------------------------------------------------------
sub usage()
{
    print "usage: saveLGP.pl [<platform>] [-maxsave nnn] [-os <platform>] [-host hostname] [-c additional quoted comment]\n";
    print "\nUsed to help platform responsible to save last good package of 74 development\n";
    print " - save a last good package into a packageHistory folder\n";
    print " - trigger moving on mirror host from lastPackage to LC_OK\n";
    print " - adding a comment line for the last change list number in log file\n";
    print " - informs LC_APPS responsibles by mail\n";
    print "Example: saveLGP.pl AIX -c \"everything fine\"\n";
    print "Valid platforms are AIX,DEC,HP,NT,SUN and SOLARIS\n";
    exit 1;
}

#------------------------------------------------------------------------------
sub ShowHistoryFolders {

    print "History folder content:\n";
    system("ls -o $OWN/$HISTORY/$PACKAGE.*");
    print $LINE_SEPARATOR;
    if ( $MIRRORNEEDED eq "YES" )
    {
        print "Mirror on $MIRRORHOST folder content:\n";
        system("$RSH $MIRRORHOST -l $MIRRORUSER ls -o $MIRRORPATH/*");
        print $LINE_SEPARATOR;
    }
}

#------------------------------------------------------------------------------
sub BuildHistoryDirectory()
{
    if ( ! -d "$OWN/$HISTORY" ) {
        mkdir("$OWN/$HISTORY",0770);
    }

    if ( ! -d "$OWN/$HISTORY" ) {
        print "FATAL ERROR creation of history directory failed\n".$LINE_SEPARATOR;
        exit 1;
    }
}

#------------------------------------------------------------------------------
sub MoveLastToLastGoodPackage
{
    if ( -f "$OWN/lastPackage/$PACKAGE" )
    {
        unlink ( "$OWN/lastGoodPackage/$PACKAGE" );
        link( "$OWN/lastPackage/$PACKAGE", "$OWN/lastGoodPackage/$PACKAGE" );
        unlink ( "$OWN/lastPackage/$PACKAGE" );
        print "Moved last package to last good package\n".$LINE_SEPARATOR;
    }
}

#------------------------------------------------------------------------------
sub CheckForLastGoodPackage
{
    if ( ! -f "$OWN/lastGoodPackage/$PACKAGE" )
    {

        print "No last package $PACKAGE found\n";
        print "Nothing to do....\n".$LINE_SEPARATOR;
        ShowHistoryFolders();
        exit 0;
    }
}

#------------------------------------------------------------------------------
sub ShiftHistoryFiles {

    chdir("$OWN/$HISTORY");

    $LASTPACKAGE = undef;
    $TESTPACKAGE = 0;
    $MAXPACKAGE  = shift(@_);

    while ( -f "$PACKAGE.$TESTPACKAGE" )
    {
        $LASTPACKAGE  = $TESTPACKAGE;
        $TESTPACKAGE += 1;
    }

    if ( !defined $LASTPACKAGE )
    {
        print "Found no last package in $OWN/$HISTORY\n".$LINE_SEPARATOR;
    }
    else
    {
        if ( $LASTPACKAGE == $MAXPACKAGE )
        {
            print "WARNING removing last entry in history folder\n";
            system("ls -o $OWN/$HISTORY/$PACKAGE.$MAXPACKAGE");
            print $LINE_SEPARATOR;
            unlink("$PACKAGE.$MAXPACKAGE");
            $LASTPACKAGE -= 1;
        }

        $TESTPACKAGE = $LASTPACKAGE + 1;

        while ( $LASTPACKAGE >= 0 )
        {
            rename("$PACKAGE.$LASTPACKAGE", "$PACKAGE.$TESTPACKAGE");
            $TESTPACKAGE  = $LASTPACKAGE;
            $LASTPACKAGE -= 1;
        }
    }

}

#------------------------------------------------------------------------------
sub CheckForAlreadySavedAndShift
{
    if ( -f "$OWN/$HISTORY/$PACKAGE.0" )
    {
        system("cmp -s $OWN/lastGoodPackage/$PACKAGE $OWN/$HISTORY/$PACKAGE.0" );
        $exitcode = $? >> 8;

        if ( $exitcode == 0 )
        {
            print "Last good package $PACKAGE already in history folder\n";
            print "Nothing to do....\n".$LINE_SEPARATOR;
            ShowHistoryFolders();
            exit 0;
        }

        ShiftHistoryFiles( "$maxsave" );
    }
}

#------------------------------------------------------------------------------
sub LinkTopOfHistory
{
    link("$OWN/lastGoodPackage/$PACKAGE", "$OWN/$HISTORY/$PACKAGE.0");
}

#------------------------------------------------------------------------------
sub MoveOnRemoteMachine
{
    if ( $MIRRORNEEDED eq "YES" )
    {
        print "INFO: Moving last good package on $MIRRORHOST into $MIRROROS/LC_OK\n";
        system("$RSH $MIRRORHOST -l $MIRRORUSER mv $MIRRORPATH/lastPackage/$PACKAGE $MIRRORPATH/LC_OK/$PACKAGE");
        print "Mirror folder on $MIRRORHOST content:\n";
        system("$RSH $MIRRORHOST -l $MIRRORUSER ls -oR $MIRRORPATH");
        print "INFO: Moving last good package on $MIRRORHOST completed\n";
    }
    else
    {
        print "INFO: Remote mirror not wanted\n".$LINE_SEPARATOR;
    }
}

#------------------------------------------------------------------------------
sub SendInfoMail
{
    use Mail::Sendmail;

    my $msg = "New last good package copied for $MIRROROS\n\nDon't reply to this mail because it's generated automatically\n";

    my $receiver = 'ivan.schreter@sap.com,peter.goertz@sap.com,markus.sinnwell@sap.com';
    my $cc       = 'gerald.arnold@sap.com,el.houssine.touh@sap.com,carl.seghers@sap.com,holm.kirstein@sap.com,thorsten.duda@sap.com,jan.wedel@sap.com,hans-georg.reusch@sap.com';

    my %mail = ( To      => "$receiver",
                 From    => 'automail.saveLGP@sap.com',
                 Cc      => "$cc",
                 Message => "$msg",
                 Smtp    => 'mail.sap-ag.de',
                 Subject => "New last good package for $MIRROROS");

    if ( sendmail(%mail) ) {
        print "INFO: send mail successfully\n".$LINE_SEPARATOR;
    } else {
        print "FATAL: sendmail error $Mail::Sendmail::error\n".$LINE_SEPARATOR;
    }

}

#------------------------------------------------------------------------------
sub PutTestLog
{
    use LWP::Simple;

    my ($platform, $comment) = @_;

    return -1 if ( ! defined $platform || ! defined $comment );

    my %PlatformStrings;
    $PlatformStrings{"NT/i386"} = "NT,%2032%20Bit";
    $PlatformStrings{"NT/ia64"} = "NT,%2064%20Bit";
    $PlatformStrings{"UNIX/SUNOS_64"}  = "Solaris";
    $PlatformStrings{"UNIX/DEC_64"}  = "True64";
    $PlatformStrings{"UNIX/AIX5_64"} = "AIX%205.1";
    $PlatformStrings{"UNIX/AIX4_64"} = "AIX%204.3";
    $PlatformStrings{"UNIX/HP11_64"}   = "HP-UX";
    $PlatformStrings{"ANY"}  = "Allgemeiner%20Eintrag";

    my $platformstr = $PlatformStrings{ uc $platform };
    return -1 if ( ! defined $platformstr );

    my $url = "http://ls0106.wdf.sap-ag.de:1111/note.py?platform=$platformstr&status=OK&comment=$comment";
    my $inhalt = get($url);

    # normaly we have to check for errors but this doesn't work for noty.py :-(
    # debug necessary
    # return -2 unless ( defined $inhalt);
    # return -3 unless ($inhalt =~ /Die Daten wurden erfolgreich geschrieben/)

    return 0;
}

#----------------------------------------------------------------------------------
sub PutComment {

#
# Format in changelist input file:
# <maxchangenr>22962</maxchangenr>
#
    my ( $AddComment ) = @_;
    $ChangeList=undef;
    open(SYNCINFO, "<$OWN/syncinfo.dat") or print "FATAL: Cannot open syncinfo.dat -> no comment added".$LINE_SEPARATOR;
    while ( <SYNCINFO> ) {
        if (/<maxchangenr>(.*)<\/maxchangenr>/)
        {
            $ChangeList = $1;
        }
    }
    close(SYNCINFO);

    if ( defined $ChangeList )
    {
#       print "INFO: Dont forget to use http://ls0106.wdf.sap-ag.de:1111/note.py\n";
#       print "INFO: Add comment line with change list number: 'cl $ChangeList'\n";
#
#       if ( defined $AddComment )
#       {
#           print "INFO: and additional comment '$AddComment'\n".$LINE_SEPARATOR;
#       }
#       else
#       {
#           print $LINE_SEPARATOR;
#       }
#
        my $rc = PutTestLog( "$MIRROROS", "cl $ChangeList  $AddComment");
        if ($rc < 0) {
            if ($rc == -1) {
                print "FATAL: Adding comment wrong parameter\n".$LINE_SEPARATOR;
            } elsif ( $rc == -2 ) {
                print "FATAL: Adding comment can't get url\n".$LINE_SEPARATOR;
            } elsif ( $rc == -3 ) {
                print "FATAL: Adding comment wrong parameter for url\n".$LINE_SEPARATOR;
            } else {
                print "FATAL: Adding comment unknown return code: $rc\n".$LINE_SEPARATOR;
            }
        }
        else
        {
            print "Added comment successfully\n".$LINE_SEPARATOR;
        }
    }
    else
    {
        print "FATAL: Found no change list number in syncinfo.dat!\n".$LINE_SEPARATOR;
    }
}


sub createLC_OK {

#
# Format in changelist input file:
# <maxchangenr>22962</maxchangenr>
#
    my ($platform) = $MIRROROS;
    $ChangeList=undef;
    open(SYNCINFO, "<$OWN/syncinfo.dat") or print "FATAL: Cannot open syncinfo.dat -> no comment added".$LINE_SEPARATOR;
    while ( <SYNCINFO> ) {
        if (/<maxchangenr>(.*)<\/maxchangenr>/)
        {
            $ChangeList = $1;
        }
    }
    close(SYNCINFO);

    if ( defined $ChangeList )
    {
    #	$WdfPlatforms{"NT/i386"} = "NT,%2032%20Bit";
	#   $WdfPlatforms{"NT/ia64"} = "NT,%2064%20Bit";
	    $WdfPlatforms{"UNIX/SUNOS_64"}  = "sun_64";
	    $WdfPlatforms{"UNIX/DEC_64"}  = "alphaosf";
	    $WdfPlatforms{"UNIX/AIX5_64"} = "rs6000_64";
	#   $WdfPlatforms{"UNIX/AIX4_64"} = "rs6000_64";
	    $WdfPlatforms{"UNIX/HP11_64"}   = "hp_64";

	    my $WdfPlatform = $WdfPlatforms{ uc $platform };
	    return -1 if ( ! defined $WdfPlatform );

	    system("$RSH $MIRRORHOST -l $MIRRORUSER createLCPoolLink.pl -platform $WdfPlatform -cl $ChangeList LC_OK");
    }
    else
    {
        print "FATAL: Found no change list number in syncinfo.dat!\n".$LINE_SEPARATOR;
    }
}



#------------------------------------------------------------------------------
#
$os = $host = $maxsave = $comment = undef;
$opt_os = $opt_hostname = $opt_maxsave = $opt_comment = undef;

GetOptions ('os:s', 'host:s', 'maxsave:s', 'comment:s', 'help|?');

usage () if ( defined $opt_help );

$os          = uc($opt_os)      if ( defined $opt_os );
$host        = $opt_host        if ( defined $opt_host );
$maxsave     = $opt_maxsave     if ( defined $opt_maxsave );
$comment     = $opt_comment     if ( defined $opt_comment );

if ( (!defined $os)
  && (!defined $host) ) {
    if ( @ARGV < 1 ) {
	    chomp( $host = `hostname` );
	} else {
		$os = lc($ARGV[0]);
		shift;
    }
}

if ( (defined $os)
  && (defined $REMUSER_ALIAS{$os}) ) {
    @platforms = split(/ /,$REMUSER_ALIAS{$os});
    $count = @platforms + 0;
    if ( $count > 1 )
    {
        print "Only a single platform is allowed\n";
        usage();
    }
    $os = lc($platforms[0]);
}

if ( defined($os) && defined($host) ) {
    print "Only a single platform is allowed\n";
    usage();
}

if ( ! defined($host) ) {
    $dev74os=$os."_7.4";
    if ( defined $REMUSER_HOST{$dev74os} ) {
        print "Using ".$dev74os." : ".$REMUSER_HOST{$dev74os}."\n";
        $host = $REMUSER_HOST{$dev74os};
    } else {
        print "Using ".$os." : ".$REMUSER_HOST{$os}."\n";
        $host = $REMUSER_HOST{$os};
    }
    if ( ! defined $host )
    {
        print "No host for platform ".$os." given\n";
        usage();
    }
}

chomp ( $Hostname=`hostname` );

if ( $Hostname ne $host )
{
    $remote_cmd="$RSH $host -l remuser /rel/TOOL/tool/bin/saveLGP.pl";
    if ( defined $maxsave ) {
        $remote_cmd .= " -maxsave ".$maxsave;
    }
    if ( defined $comment ) {
        $remote_cmd .= " -c \"".$comment."\"";
    }
    print "saving last good package on host $host\n";
    system($remote_cmd);
    exit 0
}

$maxsave=10 if ( !defined $maxsave );

print $LINE_SEPARATOR .
      "Trying to save last good package on local host $host (maxsave $maxsave)\n" .
      $LINE_SEPARATOR;

if ( ! -d "/rel" ) {

    print "FATAL: Sorry saving last good package on NT is not supported (yet)\n".$LINE_SEPARATOR;
    exit 1;
}

createLC_OK ();

BuildHistoryDirectory();

MoveLastToLastGoodPackage();

CheckForLastGoodPackage();

CheckForAlreadySavedAndShift();

LinkTopOfHistory();

MoveOnRemoteMachine();

PutComment( $comment );

SendInfoMail();

ShowHistoryFolders();

exit 0
