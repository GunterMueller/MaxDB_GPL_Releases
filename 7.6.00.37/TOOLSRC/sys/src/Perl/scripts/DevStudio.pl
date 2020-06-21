#!/usr/bin/perl
#
# @(#)DevStudio     1999-09-09
#
# G. Groﬂmann, G. Arnold SAP AG
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

package DevStudio;

unshift @INC, "$ENV{TOOL}/bin";
unshift @INC, "$ENV{TOOL}/lib/Perl";
unshift @INC, "$ENV{TOOL}/lib/perl5";

require 5.000;
use Getopt::Std;
#use SDBM_File;
use File::Path;
use Win32::NetResource;
use Win32::Shortcut;
use Env;
require PerfSet;
import PerfSet;
require PerforceEnv;
import PerforceEnv;

$SAPDB_PROFILE_HOST="";
unless ($ENV{TOOL})
{
    print "%TOOL% is not set in the environment !\n";
    <STDIN>;
}
else
{
    do "$ENV{TOOL}/bin/SAPDBHosts.pm";
}

$ProfileDB = "\\\\$SAPDB_PROFILE_HOST\\ToolSrc\\profiles";
$ReleaseDB = "$TOOL\\config\\releases";
$ShortcutDB= "$TOOL\\config\\shortcuts";
$ShareDB = "$TOOL\\config\\shares";
$CalledOWN=$ENV{OWN};

unless (-d "$TOOL/data")
{
    mkdir ("$TOOL/data", 0777);
}
unless (-d "$TOOL/config")
{
    mkdir ("$TOOL/config", 0777);
}


if (!getopts('fLlhcdipxsoU:C:P:t:X:') || $opt_h ) { Usage(); }

if ($opt_L)
{
    dbmopen(%ReleaseDB, $ReleaseDB, 0777) || warn "Can't open Release-Database!: $!\n";
    @ReleaseKeys = keys %ReleaseDB;
    @ReleaseOutput = ();
    foreach $RelOWN (sort @ReleaseKeys)
    {
        unless (-d $RelOWN)
        {
            %ReleaseDB->{$RelOWN} = undef;
            next;
        }
        push @ReleaseOutput, (pack "A10",$1)."$RelOWN $3 $2"
            if (%ReleaseDB->{$RelOWN} =~ /^(\S*)\s?(\(.*\))?\s?(\[.*\])?/);
    }
    foreach (sort @ReleaseOutput) {
      print "$_\n";
    }
    dbmclose(%ReleaseDB);
    exit;
}

$ENV{P4USER} = $opt_U if ($opt_U);
$ENV{P4CLIENT} = $opt_C if ($opt_C);
$ENV{P4PORT} = $opt_P if ($opt_P);

$OWN = shift;
$Release = shift;

if (!$Release && ($OWN =~/^\d\.\d/))
{
    ($OWN,$Release) = ($Release,$OWN);
}

if (!$OWN)
{
    if ($opt_i)
    {
        $OWN = $CalledOWN;
    }
    elsif ($opt_p)
    {
        Usage() if (!$Release);
        $OWN = PerfSet::SetOwn($CalledOWN, $Release);
    }
    else
    {
        Usage();
    }
}

if ($opt_p)
{
    unless ($opt_U)
    {
        $ENV{P4USER} = PerfSet::SetPerforceUser();
    }
    unless ($opt_C)
    {
        $ENV{P4CLIENT} = PerfSet::SetClientView($p4user);
    }
    unless ($opt_P)
    {
        $ENV{P4PORT} = PerfSet::GetPort();
    }
    PerfSet::SetViewReleaseEntry($ENV{P4CLIENT},$Release,$OWN);
}

# $OWN to lower
$OWN_for_hash = $OWN;
$OWN_for_hash =~ tr/A-Z/a-z/;

#if ( !$OWN || (!$Release && !opt_i) ) { Usage(); }

while ($_ = $ARGV[0])
{
    shift;
    last if /^--$/;
    $userargs .= " $_" ;
}


# Release not as parameter
# look in ReleaseDb und in Release.dat
if (!$Release)
{
    if ( dbmopen(%ReleaseDB, $ReleaseDB, 0) )
    {
        if (%ReleaseDB->{$OWN_for_hash} =~ /^(\S*)\s?\((.*)\)/)
        {
            $Release = $1;
        }
        elsif ( -f "$OWN/Release.dat" )
        {
            open(RELEASE_DAT,"< $OWN/Release.dat");
            $Release=<RELEASE_DAT>;
            close(RELEASE_DAT);
        }
        else
        {
            print "There's no release known for $OWN !\nRelease:";
            $Release=<STDIN>;
            chomp $Release;
        }
        dbmclose (%ReleaseDB);
    }
}


if ($Release eq "?.?") {
  if ( dbmopen(%ProfileDB, $ProfileDB, 0) )
  {
    @ProfileKeys = keys %ProfileDB;
    print "\nthese releases are present:\n\n";
    foreach $Release (sort @ProfileKeys)
    {  print "$Release\n";  }
    dbmclose(%ProfileDB);
    if ( -f "$OWN/Release.dat" )
    {
        open(RELEASE_DAT,"< $OWN/Release.dat");
        $OldRelease=<RELEASE_DAT>;
        close(RELEASE_DAT);
    }
    print "\nchose one (Default=$OldRelease): ";
    $Release = <STDIN>;
    chomp $Release;
    $Release = $OldRelease if ($Release eq "");
    print "\n$Release\n";
  }
}

if ($opt_t)
{
    $ENV{TOOL}=$opt_t;
}
else
{
    $ENV{TOOL}="D:\\DevTool" if ( not $ENV{TOOL} );  # CTS 1103200
}

if ( $opt_i)
{
    if (! -d $OWN)
    {
        print "\nPlease use devstudio without \"-i\" to create a new branch, first.\n\n";
        Usage();
    }
    CreateNewIcon($OWN, $Release, $opt_d );
    exit;
}

# New section: iprofile.bat is now created by createlcprofile:
my $iprofile;
my $ret_code;

if ($Release =~ /tool/i)
{
    $ret_code = 1;
}
else
{
    my $command_str = "$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/createlcprofile.pl -own $OWN";
    $command_str   .= " -tool $opt_t" if ($opt_t);
    $ret_code = system($command_str);
}

if ($ret_code > 0)
{
    %ProfileDB;
    dbmopen(%ProfileDB, $ProfileDB, 0) || warn "Can't open Profile-Database!: $!\n";
    $iprofile = %ProfileDB->{$Release};
    dbmclose(%ProfileDB);

    if ( !$iprofile )
    {
        warn "There's no profile for $Release\nworking with local profile\npress any key ...\n";
        <STDIN>;
        $iprofile = "iprofile.bat";
    }
}
else
{
    $iprofile = "$OWN\\iprofile.bat";
}

$ENV{OWN}=$OWN;
if ( ! -d $OWN )
{
    unless ($opt_f)
    {
        print "$OWN does not exist.\nDo you want the directory to be created (y/n)? ";
        local $in = <STDIN>;
        until ( $in eq "\n" || $in =~ /^y/i )
        {
            exit if ( $in =~ /^no?\n$/i );
            print "$OWN does not exist.\nDo you want the directory to be created (y/n)? ";
            $in = <STDIN>;
        }
    }

    CreateNewStudio($OWN, $Release, $opt_d);
    print "Developer Studio installed.\npress any key..."; <STDIN>;
    exit;
}

chdir($OWN) || warn "Can't change directory : $!\n";

if ( -f $iprofile )
{
    local $host = Win32::NodeName();
    local $args;


    if ( dbmopen(%ReleaseDB, $ReleaseDB, 0) )
    {
        if (%ReleaseDB->{$OWN_for_hash} =~ /^(\S*)\s?\((.*)\)/)
        {
            $oldRelease = $1;
        }
        elsif ( -f "$OWN/Release.dat" )
        {
            open(RELEASE_DAT,"< $OWN/Release.dat");
            $oldRelease=<RELEASE_DAT>;
            close(RELEASE_DAT);
        }
        else
        {
            $oldRelease = "";
        }
        dbmclose (%ReleaseDB);
    }

    if ( $oldRelease ne $Release )
    {
        unless ($opt_f || ($oldRelease eq "") )
        {
            print ("$OWN is configured for Release $RelDat !\n");
            $ANSW = "x";
            until ($ANSW =~ /^(y|n)/i)
            {
                print ("Overwriting with $Release (y/n) ?");
                $ANSW = <STDIN>;
                 chop $ANSW;
                 lc $ANSW;
            }
            if ( $ANSW =~ /^n/i)
            {
                print "Start Devstudio canceled\npress any key ...\n";
                <STDIN>;
                exit;
            }
        }
    }

    if ( dbmopen(%ReleaseDB, $ReleaseDB, 0777) )
    {
        # parse for                    release  users     share
        if (%ReleaseDB->{$OWN_for_hash} =~ /^(\S*)\s?\((.*)\)\s?(\[.*\])?/)
        {
            my $users=$2;
            my $share=$3;
            $users .= ",$ENV{USERNAME}" unless ($users =~ /$ENV{USERNAME}/);
            %ReleaseDB->{$OWN_for_hash} = "$Release ($users) $share";
        }
        else
        {
            %ReleaseDB->{$OWN_for_hash} = "$Release ($ENV{USERNAME})";
        }
        dbmclose(%ReleaseDB);
    }


    if ($opt_X eq "64")
    {
        unless ( -f "X64.dat")
        {
            unless ($opt_f)
            {
                print ("$OWN is configured for 32 bit  !\n");
                $ANSW = "x";
                until ($ANSW =~ /^(y|n)/i)
                {
                    print ("Overwriting for 64 bit  (y/n) ?");
                    $ANSW = <STDIN>;
                    chop $ANSW;
                    lc $ANSW;
                }
                if ( $ANSW =~ /^n/i)
                {
                    print "Devstudio canceled\npress any key ...\n";
                    <STDIN>;
                    exit;
                }
            }
            open(X64_DAT,"> X64.dat");
            close(X64_DAT);
        }
    }
    elsif (-f "X64.dat")
    {
        unless ($opt_f)
        {
            print ("$OWN is configured for 64 bit  !\n");
            $ANSW = "x";
            until ($ANSW =~ /^(y|n)/i)
            {
                print ("Overwriting for 32 bit  (y/n) ?");
                $ANSW = <STDIN>;
                chop $ANSW;
                lc $ANSW;
            }
            if ( $ANSW =~ /^n/i)
            {
                print "Devstudio canceled\npress any key ...\n";
                <STDIN>;
                exit;
            }
        }
        unlink "X64.dat";
    }


    # CTS 1103596 ->
    my @statinfo_srv=stat "$iprofile";
    my @statinfo_lcl=stat "$OWN/iprofile.bat";
    if ( ($statinfo_srv[9] != $statinfo_lcl[9]) || $getiprofile )
    {
        if (system("copy $iprofile $OWN\\iprofile.bat") != 0)
        {
            print "ERROR: Can't copy $iprofile to $OWN\\iprofile.bat\n";
            sleep 3;
            die ;
        }
    }

    # <- CTS 1103596
    if ( ! $opt_c)
    {
        # <- CTS 1102601
        # $args  = do { $userargs ? "/c " : "/k "} . $iprofile;
        # &gar  CTS 1102601
        # PTS 1106897
        if ( $opt_x )
        {
            $args  = "$OWN\\iprofile.bat";
            $args .= " && $userargs" if $userargs;
            exit system($args);
        }
        else
        {
            $args  = do { $userargs ? "/c " : "/k "} . "$OWN\\iprofile.bat";
            $args .= " -l" if $opt_l;
            $args .= " && $userargs" if $userargs;
            local $Shell;
            if ( $opt_s ) { $Shell = $opt_s } else { $Shell = $ENV{COMSPEC} }
            Win32::Spawn $Shell, $args, $PID;
            system("\\\\$host\\DevTool\\pgm\\lowprio $PID >nul") if $opt_l;
        }
    }
}
else
{
    warn "Profile $iprofile doesn't exists\npress any key ...\n";
    <STDIN>;
}

###########################################################################

sub Usage
{

  print <<End;
USAGE: DevStudio [-f] [-h] [-s shell] [-c] [-y|-n] [-d] [-x] [-l] [-X64]
                 [-t <TOOLDIR>] [-U <perforce user>] [-C <perforce client>]
                 [-P <perforce port>] OWN Release [["]arguments["]]
       DevStudio -i [-l] [-d] TOOLDIR>] [OWN] [Release]
       DevStudio -L

    -c   copy only iprofile.bat to OWN (no exceute)
    -d   create shortcut on desktop
    -h   help information
    -l   set process priority low
    -t   change %TOOL% in environment
    -s   use 'shell' instead of COMSPEC
    -x   exit after execution
    -i   Create new Shortcut
    -p   Create new Shortcut and set Perforce environment
    -L   list all installed release trees
    -f   force - overwrite old settings without questions
    -y   sync files
    -n   no file sync
    -X64 setting environment for ITANIUM cross development
    -o   use old system to generate new DevStudio's

    If OWN does not exist we are able to install
    a new "Developer Studio" for the given release
End
  if ( dbmopen(%ProfileDB, $ProfileDB, 0) ) {

    @ProfileKeys = keys %ProfileDB;

    print "\nthese releases are present:\n\n";

    foreach $Release (sort @ProfileKeys) {
      print "$Release\n";
    }

    dbmclose(%ProfileDB);
  }

  exit 1;

}

###########################################################################

sub CreateNewStudio
{

    my ($OWN, $Release, $desktop) = @_;
    local $ShortRelease = $Release;
    $ShortRelease =~ s!\.!!g;
    $ShortRelease =~ s/(..)(.)/${1}0$2/ if (length($ShortRelease) == 3);

    File::Path::mkpath(($OWN), 0, 0777) or
        do {warn "Can't create $OWN: $!\npress any key..."; <STDIN>;};
    $OWN =~ tr[\\][/];
    local @Dirs;
    push @Dirs, "$OWN/sys/desc/fast", "$OWN/sys/desc/quick", "$OWN/sys/desc/slow";
    push @Dirs, "$OWN/sys/src";
    push @Dirs, "$OWN/test/kern/sut", "$OWN/test/kern/mut";
    push @Dirs, "$OWN/test/port", "$OWN/test/support", "$OWN/test/pc";
    push @Dirs, "$OWN/tmp";
    push @Dirs, "$OWN/usr";
    File::Path::mkpath(\@Dirs, 0, 0777);

    if ( open(FILE_OUT, ">$OWN/iuser.bat") )
    {
        print FILE_OUT <<End_of_File;
\@echo off
rem Here, you can create your own environment.
rem iuser.bat was called after global environment was defined and
rem before basic requirements was checked.

rem define global macros:
if exist %TOOL%\\imacro.bat call %TOOL%\\imacro.bat

End_of_File
        close(FILE_OUT);
    }
    else
    {
        warn "Can't create $OWN/iuser.bat: $!\npress any key";
        <STDIN>;
    }
    local $Share;
    if ( $Release eq "ToolSrc")
    {
        $Share = "ToolSrc";
    }
    else
    {
        # local $MajorRelease = substr($ShortRelease, 0, 2);
        $Share = $Release;
        $Share =~ s/\.//g;
        $Share = "V$Share";

    }
    $OWN =~ tr[/][\\];
#    !system("xcopy",
#        "\\\\P30883\\$Share\\develop\\sys\\src $OWN\\sys\\src",
#        "/s /t /e")
#       || warn "Can't get directory structure from \\\\P30883\\$Share\\develop: $!\n";

    local %ShareInfo;
    local $overwrite=0;
    while ( Win32::NetResource::NetShareGetInfo($Share, $ShareInfo ) && ! $overwrite )
    {
        if ($opt_f)
        {
            $overwrite=1;
        }
        else
        {
            print ("\nShare \"$Share\" already used for \"$ShareInfo->{'path'}\"\n");
            print ("Please input new share name (type \"o\" to overwite, type nothing for no share):");
            $tmp_share = <STDIN>;
            chop $tmp_share;
            if ($tmp_share eq "o")
            {   $overwrite=1; }
            else
            {   $Share = $tmp_share; }
        }
    }
    unless  ($Share eq "")
    {
        $ShareInfo = {
                'path' => $OWN,
                'netname' => $Share,
                'remark' => "",
                'passwd' => "",
                'current-users' =>0,
                'permissions' => 0,
                'maxusers' => -1,
                'type'  => 0,
            };
        if ( $overwrite )
        {
            if (Win32::NetResource::NetShareDel( $Share ))
            {
                # remove share from release db
                dbmopen(%ReleaseDB, $ReleaseDB, 0777) || warn "Can't open Release-Database!: $!\n";
                @ReleaseKeys = keys %ReleaseDB;
                foreach $RelOWN (@ReleaseKeys)
                {
                    %ReleaseDB->{$RelOWN} = "$1 $2 []"
                       if (%ReleaseDB->{$RelOWN} =~ /^(\S*)\s?(\(.*\))?\s?(\[$Share\])/);
                }
                dbmclose(%ReleaseDB);
            }
            else
            {
                my $err = undef;
                Win32::NetResource::GetError($err);
                warn ("Error while removing the share $Share ($err)\n" );
            }
        }

        unless (Win32::NetResource::NetShareAdd( $ShareInfo ))
        {
            my $err = undef;
            Win32::NetResource::GetError($err);
            warn ("Error while adding the share $Share ($err)\n" );
        }
    }

    if ($opt_X eq "64")
    {
        open(X64_DAT,"> $OWN/X64.dat");
        close(X64_DAT);
    }

    CreateNewIcon($OWN, $Release, $desktop);

    if ( dbmopen(%ReleaseDB, $ReleaseDB, 0777) )
    {
        %ReleaseDB->{$OWN_for_hash} = "$Release ($ENV{USERNAME}) [$Share]";
        dbmclose(%ReleaseDB);
    }

    if ($ENV{P4CLIENT})
    {
        print "\nDo you want to sync this branch now (y/n)? ";
        if (<STDIN> =~ /^[1jy]/i)
        {
            PerforceEnv::sync_perforce_branch (PerfSet::GetReleasePath($Release));
        }
    }
}

#########################################

sub CreateNewIcon
{

    my ($OWN, $Release, $desktop ) = @_;
    local ($sname, $IconNumber, $IconLocation, $ShortcutDir, $ShortcutName);
    if ($desktop)
    {
        $ShortcutDir = "$ENV{USERPROFILE}\\..\\All Users\\Desktop";
        $ShortcutDir =~ s/\\[^\\]*\\\.\.//g;
    }
    elsif ($ENV{DEVSTUDIO_SHORTCUTS})
    {

        $ShortcutDir = "$ENV{DEVSTUDIO_SHORTCUTS}";
    }
    else
    {
        $ShortcutDir = "$ENV{USERPROFILE}\\..\\All Users\\Start Menu\\SAPDevelop $ENV{USERNAME}";
        $ShortcutDir =~ s/\\[^\\]*\\\.\.//g;
    }
    unless (-d $ShortcutDir )
    {
        File::Path::mkpath(($ShortcutDir), 0, 0777) or
        do {warn "Can't create \"$ShortcutDir\": $!\npress any key..."; <STDIN>;};
    }
    $ShortcutDir .= "\\";

    if ( $Release eq "ToolSrc" )
    {
        $ShortcutName = "Developer Studio Tools";
        $IconLocation = "$ENV{SYSTEMROOT}\\System32\\progman.exe";
        $IconNumber = 22;
    }
    else
    {
        $ShortcutName = "Developer Studio $Release";
        $IconLocation = "$ENV{SYSTEMROOT}\\System32\\shell32.dll";
        $IconNumber = 41;
    }

    local $Shortcut = new Win32::Shortcut();
    local $rc=1;
    $Sc_FullPath="";
    while ($rc)
    {
        $Sc_FullPath=$ShortcutDir.$ShortcutName.".lnk";
        $rc = $Shortcut->Load($Sc_FullPath);
        last if ($opt_f);
        if ($rc)
        {
            #if ($Shortcut->WorkingDirectory() eq $OWN)
            #{
            #    return 1;
            #}
            $input="";
            while ($input eq "")
            {
                print ("\nShortcut \"$ShortcutName\" already defined for \"".$Shortcut->WorkingDirectory()."\"\n");
                print ("Please input new name for shortcut(type \"o\" to overwite):");
                $input = <STDIN>;
                chop $input;
            }
            last if ($input eq "o");
            $ShortcutName=$input;
        }
    }

    if ( defined($Shortcut) )
    {
        $Shortcut->Path("$ENV{TOOL}\\Perl\\bin\\perl.exe");
        $DevArgs=" $ENV{TOOL}\\bin\\DevStudio.pl";
        $DevArgs.=" -l" if ($opt_l);
        $DevArgs.=" -t $opt_t" if ($opt_t);
        $DevArgs.=" -X$opt_X" if ($opt_X);
        $DevArgs.=" -C $ENV{P4CLIENT}" if ($ENV{P4CLIENT});
        $DevArgs.=" -U $ENV{P4USER}" if ($ENV{P4USER});
        $DevArgs.=" -P $ENV{P4PORT}" if ($ENV{P4PORT});
        $DevArgs.=" $OWN $Release";
        $Shortcut->Arguments($DevArgs);
        $Shortcut->WorkingDirectory($OWN);
        $Shortcut->ShowCmd(SW_SHOWNORMAL);
        $Shortcut->Description("Startup of the Developer Studio");
        $Shortcut->IconLocation($IconLocation);
        $Shortcut->IconNumber($IconNumber);
        $Shortcut->Save($ShortcutDir.$ShortcutName.".lnk");
        $Shortcut->Resolve();
        $Shortcut->Close();
     }
     else
     {
        warn "Can't create shortcut for Developer Studio!\n"
     }

     if ( dbmopen(%ShortcutDB, $ShortcutDB, 0777) )
     {
        %ShortcutDB->{$Sc_FullPath} = "$OWN";
        dbmclose(%ShortcutDB);
     }
     printf ("Shortcut \"$ShortcutName\" created under \n\"$ShortcutDir\"\n") if (-f "$ShortcutDir$ShortcutName.lnk");
}

################################################

sub RemoveDevStudio
{
    my ($OWN) = $_;
    # remove share
    # remove from share data base
    # remove from release data base
    # remove directory

    #remove icon
    RemoveIcon ($OWN);

}


################################################

sub RemoveIcon
{
    my ($OWN) = $_;
    if ( dbmopen(%ShortcutDB, $ShortcutDB, 0666) )
    {
        @ShortcutKeys = keys %ShortcutDB;
        foreach $Sc_FullPath (@SortcutKeys)
        {
            if (%ShortcutDB->{$Sc_FullPath} =~ /$OWN\s*$/i)
            {
                unlink "$Sc_FullPath";
                print ("Shortcut \"$Sc_FullPath\" deleted\n");
                %ShortcutDB->{$Sc_FullPath} = undef; # remove from database
            }
        }

        %ShortcutDB->{$Sc_FullPath} = "$OWN";
        dbmclose(%ShortcutDB);
    }
    # scan desktop

    @ShortcutDirs=("$ENV{USERPROFILE}\\..\\All Users\\Desktop",
                    "$ENV{USERPROFILE}\\..\\All Users\\Start Menu\\SAPDevelop $ENV{USERNAME}");
    push @ShortcutDirs, $ENV{DEVSTUDIO_SHORTCUTS} if ($ENV{DEVSTUDIO_SHORTCUTS});

    local $Shortcut = new Win32::Shortcut();

    foreach $ScDir (@ShortcutDirs)
    {
        if (opendir(DIR_SC, $ShortcutDir))
        {
            @found_Shortcuts = grep /^.*\.lnk/i, readdir(DIR_SC);
            foreach $found_Shortcut (@found_Shortcuts)
            {
                if ($Shortcut->Load("$ShortcutDir\\$found_Shortcut"))
                {
                    if  ($Shortcut->Arguments() =~ /.*perl.*devstudio.*\s$OWN\\*\s/i)
                    {
                        $Shortcut->close();
                        unlink ("$ShortcutDir\\$found_Shortcut");
                    }
                }
                $Shortcut->close();
            }
        }
        closedir DIR_SC;
    }
}
