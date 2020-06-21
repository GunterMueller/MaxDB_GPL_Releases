#!/usr/bin/perl
#
# @(#)ApoDev     2002-02-08
#
# G. Arnold SAP AG
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

package ApoDev;
require 5.000;
use Getopt::Std;
use SDBM_File;
use Carp;
use File::Path;
use Win32::NetResource;
use Win32::Shortcut;
use Env;
use PerforceEnv;
use APOSettings;


# $BranchDir ="//apo/";



$ReleaseDB = "$TOOL\\data\\releases";
$ShortcutDB= "$TOOL\\data\\shortcuts";
$ShareDB = "$TOOL\\data\\shares";
$CalledOWN=$ENV{OWN};

$APODevDir = $ENV{BUILD_PATH};
unless ($APODevDir)
{
     $APODevDir= $1 if ( $ENV{TOOL} =~ /^(.*)[\\\/]Devtool/i );
}	
$ENV{BUILD_PATH}=$APODevDir;

unless (-d "$TOOL/data")
{
	mkdir ("$TOOL/data", 0777);
}

if (!getopts('fLlhcdixynDU:C:P:O:t:X:R:') || $opt_h ) { Usage(); }

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
$ENV{BUILD_PATH} = $opt_R if ($opt_R);

$OWN= $opt_O;
$Release = shift;

$ENV{'P4USER'} = PerforceEnv::check_perforce_user();
$ENV{'P4CLIENT'} = PerforceEnv::check_perforce_client($ENV{'P4USER'}, $opt_C);
#$ENV{'P4CLIENT'} = PerforceEnv::check_perforce_port($ENV{'P4PORT'});
$ClientRoot=getClientRoot();

if ( $ClientRoot ne $ENV{BUILD_PATH})
{
	print "Current ClientRoot differs to ClientRoot of Perforce client\n";
	$ANSW = "x";
	until ($ANSW =~ /^(y|n)/i)
	{
		print ("Set current ClientRoot to $ClientRoot (y/n) ?");
		$ANSW = <STDIN>;
		chop $ANSW;
		lc $ANSW;
	}
	if ( $ANSW =~ /^n/i)
	{
		print "ApoDev canceled\n";
		exit;
	}
	$ENV{BUILD_PATH}=$ClientRoot;
}


$OWN=$CalledOWN if ( !$OWN && $opt_i);

if ( ! $Release && ! $opt_i ) { Usage(); }

while ($_ = $ARGV[0])
{
    shift;
    last if /^--$/;
    $userargs .= " $_" ;
}

if (($OWN) && (! $Release) )
{
	# Release not as parameter
	# look in ReleaseDb and in Release.dat
	if ( dbmopen(%ReleaseDB, $ReleaseDB, 0) )
	{
		if (%ReleaseDB->{$OWN} =~ /^(\S*)\s?\((.*)\)/)
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

if ($Release eq "?") {
  print "\nthese releases are present:\n\n";
  foreach $Release (sort keys %APOSettings::Depots)
	{  print "$Release\n";  }
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
	CreateNewIcon($OWN, $Release, $opt_d );
	exit;
}

#$ENV{'P4USER'} = PerforceEnv::check_perforce_user();
#$ENV{'P4CLIENT'} = PerforceEnv::check_perforce_client($ENV{'P4USER'});
#$ENV{'P4CLIENT'} = PerforceEnv::check_perforce_port($ENV{'P4PORT'});
#$ClientRoot=getClientRoot();

undef $BranchKey;
foreach (sort keys %APOSettings::Depots)
{
	# convert to right case	
	if (/^$Release$/i)
	{
		$BranchKey = $_ ;
		last;
	}
}
unless ( $BranchKey )
{
    warn "There's no branch directory for $Release\n";
	<STDIN>;
	print "releases:\n\n"; 

	foreach (sort keys %APOSettings::Depots)
	{
		print " $_\n";
	}

    exit; 
}

$OWN=$ENV{BUILD_PATH}."\\".$BranchKey;
$ENV{OWN}=$OWN;

# new branch
if  (PerforceEnv::check_perforce_branch($ENV{'P4CLIENT'},$APOSettings::Depots{$BranchKey},$BranchKey,'sys/src/'))
{
    CreateNewStudio($OWN, $Release, $opt_d);
	PerforceEnv::sync_perforce_branch ( $APOSettings::Depots{$BranchKey}."/".$BranchKey, 0 );	
    print "APO Development installed.\npress any key..."; <STDIN>;
    exit;
}


chdir($OWN) || warn "Can't change directory : $!\n";


if ( open(CHLST, "<$OWN/changelist.log") )
{
	$Changelist=<CHLST>;
	close CHLST;
	print ("Current changelist is $Changelist\n");
}
unless ($opt_n)
{
	unless ($opt_y)
	{
		$ANSW = "x";
		until ($ANSW =~ /^(y|n|o)/i)
		{
			print ("sync files - yes/no/other changelist(y/n/o) ?");
			$ANSW = <STDIN>;
			chop $ANSW;
			lc $ANSW;
		}
	}
	if ( ($ANSW =~ /^y/i) || $opt_y )
	{
		print "sync files ...\n";
		PerforceEnv::sync_perforce_branch ( $APOSettings::Depots{$BranchKey}."/".$BranchKey, 0 );	
		open(CHLST, ">$OWN/changelist.log");
		print CHLST "$ENV{LATEST_CHANGE_LIST}";
		close CHLST;
	}
	elsif ($ANSW =~ /^o/i)
	{
		print "changelist: ";
		$ANSW = <STDIN>;
		chop $ANSW;
		PerforceEnv::sync_perforce_branch ( $APOSettings::Depots{$BranchKey}."/".$BranchKey, $ANSW );	
		open(CHLST, ">$OWN/changelist.log");
		print CHLST "$ENV{LATEST_CHANGE_LIST}";
		close CHLST;
	}	
}

if ( dbmopen(%ReleaseDB, $ReleaseDB, 0777) )
{
	# parse for                    release  users     share
	if (%ReleaseDB->{$OWN} =~ /^(\S*)\s?\((.*)\)\s?(\[.*\])?/)
	{
		my $users=$2;
		my $share=$3;
		$users .= ",$ENV{USERNAME}"	unless ($users =~ /$ENV{USERNAME}/);
		%ReleaseDB->{$OWN} = "$Release ($users) $share";
	}
	else
	{
		%ReleaseDB->{$OWN} = "$Release ($ENV{USERNAME})";
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
				print "ApoDev canceled\npress any key ...\n";
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
			print "ApoDev canceled\npress any key ...\n";
			<STDIN>;
			exit;
		}
	}
	unlink "X64.dat";
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
		    $args  = "$OWN\\sys\src\\\iprofile.bat";
			$args .= " && $userargs" if $userargs;
			exit system($args);
	}
	else
	{
		$args  = do { $userargs ? "/c " : "/k "} . "$OWN\\sys\\src\\iprofile.bat";
		$args .= " -l" if $opt_l;
		$args .= " && $userargs" if $userargs;
		local $Shell;
		if ( $opt_s ) { $Shell = $opt_s } else { $Shell = $ENV{COMSPEC} }
		Win32::Spawn $Shell, $args, $PID;
		system("\\\\$host\\DevTool\\pgm\\lowprio $PID >nul") if $opt_l;
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
USAGE: ApoDev [-f] [-h] [-s shell] [-c] [-S] [-y|-n] [-d] [-x] [-l] [-X64] [-t <TOOLDIR>]
              [-U <perforce user>] [-C <perforce client>] [-P <perforce port>]
              [-R <client root>]  <BRANCH> [["]arguments["]]
       ApoDev -i [-l] [-d] TOOLDIR>] [<BRANCH>]
       ApoDev -L

    -c   copy only iprofile.bat to OWN (no exceute)
    -d   create shortcut on desktop
    -h   help information
    -l   set process priority low
    -t   change %TOOL% in environment
    -s   use 'shell' instead of COMSPEC
    -x   exit after execution
    -i   Create new Shortcut
    -L   list all installed release trees
    -f   force - overwrite old settings without questions
    -S   create public share of OWN directory
    -y   sync files 
    -n   no file sync 
    -X64 setting environment for ITANIUM cross development


    If OWN does not exist we are able to install
    a new "Developer Studio" for the given release

End

	
  foreach (sort keys %APOSettings::Depots)
	{
		print "    $_\n";
	}

  exit 1;

}

###########################################################################

sub getClientRoot
{
	# my ($p4client, $p4user ) = @_;
	open(PIPE_IN, "p4 info |") or croak "Can't pipe (output): $!\n";
	while(<PIPE_IN>) 
	{ 
		if (/Client root:\s*(\S*)\s*$/)
		{
			close PIPE_IN;		
			return $1;	
		}
	}
	close PIPE_IN;
	return 0;
}

##########################################################################

sub CreateNewStudio
{

    my ($OWN, $Release, $desktop) = @_;
    local $ShortRelease = $Release;
    #$ShortRelease =~ s!\.!!g;
    #$ShortRelease =~ s/(..)(.)/${1}0$2/ if (length($ShortRelease) == 3);
	unless ( -d "$OWN" )
	{
		File::Path::mkpath(($OWN), 0, 0777) or
			do {warn "Can't create $OWN: $!\npress any key..."; <STDIN>;};
	}
    $OWN =~ tr[\\][/];
    local @Dirs;
    push @Dirs, "$OWN/sys/src";
    push @Dirs, "$OWN/tmp";
    push @Dirs, "$OWN/usr";
    File::Path::mkpath(\@Dirs, 0, 0777);

    if ( open(FILE_OUT, ">$OWN/sys/src/iuser.bat") )
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
        warn "Can't create $OWN/sys/src/iuser.bat: $!\npress any key";
        <STDIN>;
    }

	local $Share ="";
	
	if ($opt_S)
	{
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
				{	$overwrite=1; }
				else
				{	$Share = $tmp_share; }
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
					'permissions' => 0300,
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
	}

	if ($opt_X eq "64")
	{
		open(X64_DAT,"> $OWN/X64.dat");
		close(X64_DAT);
	}

    CreateNewIcon($OWN,$Release, $desktop);

	if ( dbmopen(%ReleaseDB, $ReleaseDB, 0777) )
	{
		%ReleaseDB->{$OWN} = "$Release ($ENV{USERNAME}) [$Share]";
		dbmclose(%ReleaseDB);
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
	elsif ($ENV{APODEV_SHORTCUTS})
	{

		$ShortcutDir = "$ENV{APODEV_SHORTCUTS}";
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
		$ShortcutName = "APO Development Tools";
	    $IconLocation = "$ENV{SYSTEMROOT}\\System32\\progman.exe";
        $IconNumber = 22;
    }
    else
    {
		$ShortcutName = "APO Development $Release";
		my $hostname = `hostname`;
		chomp $hostname;
		$ShortcutName.=" ($ENV{P4CLIENT})" unless ( $hostname eq $ENV{P4CLIENT});
		
	    $IconLocation = "$ENV{SYSTEMROOT}\\System32\\shell32.dll";
        $IconNumber = 41;
    }

    local $Shortcut = new Win32::Shortcut();
	local $rc=1;
	$Sc_FullPath="";
	while ($rc )
	{
		$Sc_FullPath=$ShortcutDir.$ShortcutName.".lnk";
		$rc = $Shortcut->Load($Sc_FullPath);
		last if ($opt_f);
		if ($rc)
		{
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
		$DevArgs ="-I %TOOL%\\bin";
		$DevArgs.=" $ENV{TOOL}\\bin\\ApoDev.pl";
		$DevArgs.=" -l" if ($opt_l);
		$DevArgs.=" -t $opt_t" if ($opt_t);
		$DevArgs.=" -X$opt_X" if ($opt_X);
		$DevArgs.=" -C $ENV{'P4CLIENT'}";
		$DevArgs.=" -U $ENV{'P4USER'}";
		$DevArgs.=" -P $ENV{'P4PORT'}" if ($ENV{P4PORT});
		$DevArgs.=" -R $ENV{'BUILD_PATH'}";
		$DevArgs.=" -O $OWN $Release";
	    $Shortcut->Arguments($DevArgs);
	    $Shortcut->WorkingDirectory($OWN);
	    $Shortcut->ShowCmd(SW_SHOWNORMAL);
	    $Shortcut->Description("Startup of the APO Development");
        $Shortcut->IconLocation($IconLocation);
        $Shortcut->IconNumber($IconNumber);
	    $Shortcut->Save($ShortcutDir.$ShortcutName.".lnk");
		$Shortcut->Resolve();
	    $Shortcut->Close();
     }
     else
     {
        warn "Can't create shortcut for ApoDev!\n"
     }

	 if ( dbmopen(%ShortcutDB, $ShortcutDB, 0777) )
	 {
		%ShortcutDB->{$Sc_FullPath} = "$OWN";
		dbmclose(%ShortcutDB);
	 }
     printf ("Shortcut \"$ShortcutName\" created under \n\"$ShortcutDir\"\n") if (-f "$ShortcutDir$ShortcutName.lnk");
}

################################################

sub RemoveApoDev
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
					"$ENV{USERPROFILE}\\..\\All Users\\Start Menu\\APO Develop $ENV{USERNAME}");
	push @ShortcutDirs, $ENV{APODEV_SHORTCUTS} if ($ENV{APODEV_SHORTCUTS});

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
					if 	($Shortcut->Arguments() =~ /.*perl.*ApoDev.*\s$OWN\\*\s/i)
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
