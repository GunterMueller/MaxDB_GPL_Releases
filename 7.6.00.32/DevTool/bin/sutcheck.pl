#!/usr/bin/perl
#
#    sutcheck.pl
#
#    @(#)sutcheck 2003-12-10
#
#    U. Jansen, SAP AG
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#
##############################################################################
# Static includes:

BEGIN
{
    unless (defined $ENV{'TOOL'})
    {
    	my $callers_path = (caller())[1];
	    $callers_path  =~ tr/\//\\/ if ($^O=~/.*win32.*/i);
		$ENV{'TOOL'} = $1 if ($callers_path =~ /^(.*)[\\\/]bin[\\\/]LCEnv.*/);
    }

    unshift @INC, "$ENV{'TOOL'}/lib/perl5";
    unshift @INC, "$ENV{'TOOL'}/bin";
    
    if (-e "/devtool/local/bin")
    {
        $ENV{'PATH'} = "/devtool/local/bin:$ENV{'PATH'}";
    }
    
    # Fix for Perforce path:
    
    if (-e "/usr/local/bin")
    {
        $ENV{'PATH'} = "/usr/local/bin:$ENV{'PATH'}";
    }
}

use strict;
use IO::ZipFile;
use IO::SimpleCfg;
use IO::FilePath;
use IO::DelTree;
use File::Path;
use File::Copy;
use File::Compare;
use Getopt::Long;
use Perforce::P4Info;
use Perforce::P4Admin;
use Perforce::P4Sync;
use Perforce::P4File;
use Tool::RequestInput;
use Tool::HashRef;
use Tool::SQLCli;
use Net::HTTP;
use HTTP::Status;

###############################################################################
# Global Variables:
###############################################################################
my $ProgramName       = "sutcheck";
my $Version           = "1";
my $Revision          = "09";
my $Date              = "2004-03-23"; 
my %Opts;
my $Cfg               = {};
my $CfgFile           = "$ENV{'TOOL'}/config/sutcheck.cfg";
my $SUTDir            = "/sys/src/su";
my $DESCDir           = "/sys/desc";
my $SUTTypes          = { 'SLOW'     => ['punix'],
                          'QUICK'    => ['tpunx', 'punix'],
                          'FAST'     => ['tpunx', 'punix'],
                          'DBUNI'    => ['uunix', 'tpunx', 'punix'],
                          'UNI'      => ['upnix', 'uunix', 'tpunx', 'punix'],
                          'XCI'      => ['cunix', 'upnix', 'uunix', 'tpunx', 'punix'],
                          'ALLUNI'   => ['cpnix', 'cunix', 'upnix', 'uunix', 'tpunx', 'punix'] };


###############################################################################
# Program initialisation:
###############################################################################
print "\n$ProgramName v$Version.$Revision ($Date), (c)2004 SAP AG\n\n";

if (!GetOptions( \%Opts,
                'h|help|?',
                'register',
                'ed=s',
                'sqlcli=s',
                'unregister',
                'wrkdir=s',
                'user=s',
                'use',
                'all=i',
                'p4p=s',
                'p4u=s',
                'p4pw=s',
                'cleanup=i',
                'save',
                'showcfg',
                'askforuser=i') || $Opts{'h'})
{
    Usage();
}

LoadCfg();

###############################################################################
# Windows registration handling:
###############################################################################
if ($Opts{'register'} || $Opts{'unregister'})
{
    if ($^O =~ /MsWin32/i)
    {
        if ($Opts{'register'})
        {
            RegisterFileExt();
        }
        else
        {
            UnregisterFileExt();
        }
    }
    else
    {
        print "Sorry! This function is only available for Windows.\n";
    }
    exit(0);
}

###############################################################################
# Show config and exit:
###############################################################################
if ($Opts{'showcfg'})
{
    ShowHashRef($Cfg);
    exit(0);
}


###############################################################################
# Configuration handling:
###############################################################################
if ($Opts{'save'})
{
    SaveSimpleCfg($CfgFile, $Cfg, 0);
    exit(0);
}

###############################################################################
# Continue previous work:
###############################################################################
$Cfg->{'__USE'} = 1 if ($Opts{'use'});

Usage() unless (@ARGV);

###############################################################################
# Data initialisation:
###############################################################################
mkdir ($Cfg->{'WRKDIR'});
$Cfg->{'P4CLIENT'} = mktemp() unless ($Cfg->{'P4CLIENT'});

###############################################################################
# Create signal handler for ctrl-c to cleanup p4 settings :
###############################################################################
$SIG{INT} = \&CleanupRunAndExit;

###############################################################################
# Main loop (for each given ZIP-File):
###############################################################################
foreach my $file (@ARGV)
{
    my ($relver, $corr, $build) = ($1,$2,$3) if ($file =~ /(\d\d)(\d?\d?)(DEV|COR|RAMP)/i);
    $build = uc($build);
    unless ($relver && $corr && $build)
    {
        print "Error! '$file' is no correct name! Please ensure that:\n";
        print "1. Contains the release name (i.e. 7600DEV_*)\n";
        print "2. The Test was a DEV- or COR-test. Other Releases are not supported, yet.\n";
        sleep (10);
        next;
    }
    
    CheckSutZip ($file, $relver, $corr, $build);
}

sleep (10);
exit(0);

###############################################################################
# CheckSutZip - Handles one zip file
###############################################################################
sub CheckSutZip
{
    ($Cfg->{'__ZIPFILE'}, $Cfg->{'__REL'}, $Cfg->{'__COR'}, $Cfg->{'__BUILD'}) = @_;

    print "Checking SUT differences for $Cfg->{'__REL'}.$Cfg->{'__COR'}.$Cfg->{'__BUILD'}\n\n";

    $Cfg->{'__STATE'} = 0;
    unless (InstallSut())
    {
        print "Installation failed! Aborting...\n";
        CleanupRun();
        return 0;
    }

    $Cfg->{'__STATE'} = 1;

    unless ($Cfg->{'ALL'} || @{$Cfg->{'__FILES'}})
    {
        print "There's nothing to do for user '$Cfg->{'USER'}'. Aborting...\n";
        CleanupRun();
        return 1;
    }

    RemoveDoubles();

    CheckSut();

    CleanupRun();
    return 1;    
}


###############################################################################
# InstallSut() - Installs a sut package.
###############################################################################
sub InstallSut
{
    $Cfg->{'__WRKDIR'} = "$Cfg->{'WRKDIR'}/$Cfg->{'__REL'}$Cfg->{'__COR'}$Cfg->{'__BUILD'}";
    $Cfg->{'__WRKDIR'} = FixPathSep($Cfg->{'__WRKDIR'});
    if (-d $Cfg->{'__WRKDIR'})
    {
        print "Removing existing installation..";
        DelTree("$Cfg->{'__WRKDIR'}");
        print "..OK\n";
    }
    mkpath ($Cfg->{'__WRKDIR'});
    chdir ($Cfg->{'__WRKDIR'});
    push @{$Cfg->{'__REMDIRS'}}, $Cfg->{'__WRKDIR'};
    
    if ($Cfg->{'__USE'})
    {
        if (-d "$Cfg->{'__WRKDIR'}")
        {
	        print "Using existing installation '$Cfg->{'__REL'}$Cfg->{'__COR'}$Cfg->{'__BUILD'}'.\n";
	        $Cfg->{'__ZIPFILE'} = undef;
        }
        else
        {
            print "Error: Installation '$Cfg->{'__REL'}$Cfg->{'__COR'}$Cfg->{'__BUILD'}' not found!\n";
            return 0;
        }
    }
    else
    {
	    print "Unzipping file..";
	    unless (UnzipFile($Cfg->{'__ZIPFILE'}, $Cfg->{'__WRKDIR'}))
	    {
	        return 0;
	    }
	    print "..OK\n";
    }
    
    print "Reading SUT configuration..";
    if (-e "sutresults.txt")
    {
        $Cfg->{'__SUTCFG'} = LoadSimpleCfg("sutresults.txt");
    }
    elsif (-e "SutStarter/sutresults.txt")
    {
        $Cfg->{'__SUTCFG'} = LoadSimpleCfg("SutStarter/sutresults.txt");
    }
    
    unless ($Cfg->{'__SUTCFG'})
    {
        print "..FAILED!\n";
        return 0;
    }
    print "..OK\n";

    print "Checking Perforce server path..";
    my $p4root = GetP4Root($Cfg->{'__REL'}, $Cfg->{'__COR'}, $Cfg->{'__BUILD'});
    unless ($p4root)
    {
        return 0;        
    }
    print "..OK ($p4root)\n";
	sleep (10);
    
    my $P4Client = mktemp();
    print "Creating P4Client..";
    $Cfg->{'__P4BRANCH'} = "$p4root";
    my @view = ("$Cfg->{'__P4BRANCH'}/... //$P4Client/...");
	if (P4SetClient($Cfg->{'P4PORT'}, $P4Client, "$Cfg->{'__WRKDIR'}", $Cfg->{'P4USER'}, undef, undef, undef, \@view))
	{
	    print "..OK\n";
	    $Cfg->{'__P4CLIENT'} = $P4Client;
	}
	else
	{
	    print "..FAILED!\n";
        return 0;
	}
    
#    $Cfg->{'__P4CHLIST'} = P4GetChangeList($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, "$Cfg->{'__P4BRANCH'}");

    unless ($Cfg->{'ALL'})
    {
	    print "Getting responsibles..";
	    P4Sync($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir", undef, 1, 0, ('ISUT.me'));
	    unless (-e "$Cfg->{'__WRKDIR'}$SUTDir/ISUT.me")
	    {
		    print "..FAILED (maybe wrong P4User '$Cfg->{'P4USER'}'?)!\n";
	        return 0;
	    }
        open (IN_FILE, "< $Cfg->{'__WRKDIR'}$SUTDir/ISUT.me");
        @{$Cfg->{'__FILES'}} = map {chomp; s/^D\d+\s+(\S+)\s+.*/$1/i; $_ } grep { /\Q$Cfg->{'USER'}\E/i } <IN_FILE>;
        close (IN_FILE);
	    print "..OK\n";
    }
    
    print "Getting Perforce server file list..";
    $Cfg->{'__P4SUTDIR'} = P4GetDir($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, "$Cfg->{'__P4BRANCH'}$SUTDir");
    unless (@{$Cfg->{'__P4SUTDIR'}->{'FILES'}})
    {
        print "..FAILED!\n";
        return 0;
    }
    print "..OK\n";
            
    return 1;
}

###############################################################################
# CheckSut() - Checks a complete SUT directory.
###############################################################################

sub CheckSut
{
    print "\n";

    foreach my $type ('slow', 'quick', 'fast', 'dbuni', 'uni', 'xci', 'alluni')
    {
        if (-d $type)
        {
            unless (CheckSubDir($type))
            {
                $Cfg->{'__STATE'} = 3;
                last;
            }
        }
    }
}

###############################################################################
# CheckSubDir() - Checks a sub directory.
###############################################################################
sub CheckSubDir
{
    my ($dir) = @_;
    my @files;
    my $found = 0;
    
    if (opendir (DIR_IN, "$dir"))
    {
        @files = grep { /\.prot/ } readdir(DIR_IN);
        closedir(DIR_IN);
    }

    if (@files)
    {
	    print "Checking files in '$dir'\n";
	    print "------------------------------\n\n";
	    
	    foreach my $file (@files)
	    {
	        $file = $1 if ($file =~ /(.*)\.prot/);
	        if ($Cfg->{'ALL'} || (grep { /^\Q$file\E$/ } @{$Cfg->{'__FILES'}}))
	        {
                my @extlist = @{$SUTTypes->{uc($dir)}};
                
                my $fstat = P4FStat($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir/$file.$extlist[0]");
                
                if (!$Cfg->{'__SUTCFG'}->{'changelist'} || ($Cfg->{'__SUTCFG'}->{'changelist'} >= $fstat->{'headChange'}))
                {
                    my $dname = CheckedBy("$dir/$file");
                    if ($dname)
                    {
                        print "File '$file' has already been checked by " . DUserToRealName($dname) . ".\n";
                    }
                    else
                    {
	    	            $found = 1;
		                $Cfg->{'__STATE'} = 2;
		                return 0 unless (CheckFile($dir, $file));
                    }
                }
                else
                {
                    print "File '$file' has already been changed.\n";
                }
	        }
	    }
	    print "No files found.\n" unless ($found);
	    print "\n";
    }
    else
    {
        print "No files found in '$dir'..skipping\n\n";
    }
    
    return 1;
}

###############################################################################
# CheckFile() - Checks a single file (this is the real check!).
###############################################################################
sub CheckFile
{
    my ($dir, $file) = @_;
    my $cmd;
    my $sep = ($^O =~ /MsWin32/i) ? "\\" : "/";
    
    my $ftype = GetFileType($dir, $file);
    my @extlist = @{$SUTTypes->{uc($dir)}};
    
    print "Checking file '$file' [$ftype/$extlist[0]] - ";
    $Cfg->{'__LATEST_FILE'} = "$dir/$file";
    
    if ($Cfg->{'EDITOR'} eq "diffview")
    {
        $cmd = "$ENV{'PERL'}\\bin\\perl.exe $ENV{'TOOL'}\\bin\\diffview.pl $dir\\$file >NUL";
    }
    elsif ($Cfg->{'EDITOR'})
    {
        $cmd = "$Cfg->{'EDITOR'} $dir\\$file.pdiff $dir\\$file.prot";
    }
    else
    {
        $cmd = "$ENV{'EDITOR'} $dir\\$file.pdiff $dir\\$file.prot";
    }
    
    system($cmd);
    
    my $sel = RequestInput("Please select ('h' for help): ", "k", "[dDkKpPqQ]", "\nUnknown option! Known options are:\n\n[d] = Delete file / discard\n[k] = Keep file (default, <Enter>)\n[p] = Submit change to Perforce\n[q] = Quit program\n\n");

    if ($sel =~ /d/i)
    {
        unlink ("$dir\\$file.pdiff", "$dir\\$file.prot");
        unlink ("$dir\\$file.rtcerror") if (-e "$dir\\$file.rtcerror");
        unlink ("$dir\\$file.core") if (-e "$dir\\$file.core");
        unlink ("$dir\\$file.dump") if (-e "$dir\\$file.dump");
        unlink ("$dir\\$file.rtedump") if (-e "$dir\\$file.rtedump");
        unlink ("$dir\\$file.vprot") if (-e "$dir\\$file.vprot");
        print "File(s) deleted.\n";
        SetChecked("$dir/$file");
    }
    elsif ($sel =~ /k/i)
    {
        print "Keeping file(s)..\n";
        
        unless ($Cfg->{'__ASK_CLEAN'} || !$Cfg->{'CLEANUP'})
        {
            print "Auto-cleanup is actived! ";
            if (RequestInput("Deactivate auto-cleanup now (Y/n)? ", "y", "[nNyY]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /y/i)
            {
                $Cfg->{'CLEANUP'} = 0;
            }
            $Cfg->{'__ASK_CLEAN'} = 1;
        }
    }
    elsif ($sel =~ /p/i)
    {
        if (SubmitFile($dir, $file))
        {
            SetChecked("$dir/$file");
        }
        CheckRevertOpenFile();
    }
    elsif ($sel =~ /q/i)
    {
        print "Exiting.\n";
        return 0;
    }
 
    return 1;
}

###############################################################################
# SubmitFile() - Switches a file on Server
###############################################################################
sub SubmitFile
{
    my ($dir, $file) = @_;
    my $AddMac = "";
    
    my @existing = grep { /\Q$file\E/ } @{$Cfg->{'__P4SUTDIR'}->{'FILES'}};
    
    unless (@existing)
    {
        print "ERROR! Can't find server file for '$file'!\nAborting...\n";
        return 0;
    }
    
    my @extlist = @{$SUTTypes->{uc($dir)}};
    my $p4file  = "$file.$extlist[0]";
    
    if (grep { /\Q$p4file\E/ } @existing)
    {
        print "Checking out file '$p4file'..";
        P4Sync($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir", undef, 1, 0, ("$p4file"));
        unless (-e "$Cfg->{'__WRKDIR'}$SUTDir/$p4file")
        {
            print "..SYNC FAILED!\n";
            return 0;
        }
        unless (P4Edit($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir/$p4file"))
        {
            print "..FAILED!\n";
            return 0;
        }
        print "..OK\n";
        $Cfg->{'__OPEN_FILE'} = "$Cfg->{'__P4BRANCH'}$SUTDir/$p4file";
        unlink ("$Cfg->{'__WRKDIR'}$SUTDir/$p4file");
    }
    else
    {
        unless (RequestInput("File '$p4file' does not exist. Create file (y/N)? ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /y/i)
        {
            print "Aborting...\n";
            return 1;
        }
        print "Adding file..";
        unless (P4Add($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir/$p4file"))
        {
            print "..FAILED!\n";
            return 0;
        }
        print "..OK\n";
        $AddMac = "$p4file";
        $Cfg->{'__OPEN_FILE'} = "$Cfg->{'__P4BRANCH'}$SUTDir/$p4file";
    }
    print "Copying file..";
    unless (copy ("$dir/$file.prot", "$Cfg->{'__WRKDIR'}$SUTDir/$p4file"))
    {
        print "..FAILED!\n";
        return 0;
    }
    print "..OK\n";
    
    print "Submitting file..";
    unless (P4Submit($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$SUTDir/$p4file", "File '$p4file' submitted by $ProgramName v$Version.$Revision"))
    {
        print "..FAILED!\n";
        return 0;
    }
    print "..OK\n";
    $Cfg->{'__OPEN_FILE'} = undef;
    
    if ($AddMac ne "")
    {
        return AddFileToMac($dir, $AddMac);
    }
    
    return 1;
}

###############################################################################
# SetChecked() - Mark a file as checked.
###############################################################################
sub SetChecked
{
    my $file = shift;
    
    if ($Cfg->{'__SQL'})
    {
        print "Updating PTS database..";
        $Cfg->{'__SQL'}->execute("insert into TESTER.SUTCHECK values ('$Cfg->{'__SUTCFG'}->{'driverid'}', '$file', '$Cfg->{'__LOGIN'}')");
        print ($Cfg->{'__SQL'}->{'RC'} ? "..FAILED ($Cfg->{'__SQL'}->{'RC'}: $Cfg->{'__SQL'}->{'ERROR_MSG'})\n" : "..OK\n");
    }
}

###############################################################################
# CheckedBy() - Determines weather a file is already chacked or not.
###############################################################################
sub CheckedBy
{
    my $file = shift;
    
    if ($Cfg->{'__SQL'})
    {
        $Cfg->{'__SQL'}->execute("select DUSER from TESTER.SUTCHECK where DRIVERID='$Cfg->{'__SUTCFG'}->{'driverid'}' and TESTNAME='$file'");
        if ($Cfg->{'__SQL'}->{'ROWS'})
        {
            return $Cfg->{'__SQL'}->{'DATA'}[0][0];
        }
        else
        {
            return undef;
        }
    }
    else
    {
        return undef;
    }
}

###############################################################################
# AddWebComment() - Adds a comment line to SUT detail web site.
###############################################################################
sub AddWebComment
{
    my $comment;
    return unless ($Cfg->{'__STATE'});
    
    my $loginname = DUserToRealName($Cfg->{'__LOGIN'});
    my $checkname = DUserToRealName($Cfg->{'USER'});
    
    print "Adding comment to Web site..";
    if ($Cfg->{'__STATE'} == 1)
    {
        $comment = ($loginname eq $checkname ? "I " : "$checkname ") . "had nothing to do.";
    }
    elsif ($Cfg->{'__STATE'} == 2)
    {
        $comment = "I checked all" . ($Cfg->{'ALL'} ? "" : ($loginname eq $checkname ? " of my" : " of $checkname" . "''s")) . " files.";
    }
    elsif ($Cfg->{'__STATE'} == 3)
    {
        if ($Cfg->{'__LATEST_FILE'})
        {
            $comment = "I checked " . ($Cfg->{'ALL'} ? "all " : ($loginname eq $checkname ? "of my " : "of $checkname" . "''s ")) . "files up to $Cfg->{'__LATEST_FILE'}.";
        }
        else
        {
            print "..FAILED! (no last file)\n";
            return;
        }
    }
    else
    {
        print "..FAILED! (Unknown state '$Cfg->{'__STATE'}')\n";
        return;
    }
    
    unless ($Cfg->{'__SUTCFG'}->{'driverid'})
    {
        print "..FAILED! (No diverid found)\n";
        return;
    }
    
    my $now = getisotime(time);
	my %head_content;
	
	$head_content{'User-Agent'} = "LCqaCLI/1.0";
	
	my $dav_req = Net::HTTP->new('Host' => "pts.wdf.sap.corp", 'PeerPort' => 1080);
	
	unless ($dav_req)
	{
	    print "..connect FAILED!\n";
	    return;
	}
	
	$comment = "sutcheck restarted: " . $comment if ($Cfg->{'__USE'});
	my $sqlstmt = "INSERT INTO testcomments (ts, ref_javatestinfo, author, text) VALUES ($now, '$Cfg->{'__SUTCFG'}->{'driverid'}', '$loginname', '$comment')";
	$dav_req->write_request('PUT', "SQLService?action=DirectExecute", %head_content, $sqlstmt);
	my ($code, $mess, %headers) = $dav_req->read_response_headers();
	
	if (is_success($code))
	{
	    print "..OK\n";
	} 
	else
	{
	    print "..FAILED! (" . status_message($code) . " / $mess)\n";
	}
	
	return;
}

sub DUserToRealName
{
    my $duser = shift;
    
    if (open (CMD_OUT, "$ENV{'PERL'}\\bin\\perl.exe $ENV{'TOOL'}\\bin\\tel.pl $duser |"))
    {
        while (<CMD_OUT>)
        {
            $duser = $1 if (/".*".*,\s+(\S+)/);
        }
        close (CMD_OUT);
    }

    return $duser;    
}

sub getisotime {
        my $ts = shift;
        my @tlist = localtime($ts);

        return sprintf("'%04d-%02d-%02d %02d:%02d:%02d.000'",
                $tlist[5] + 1900,
                $tlist[4] + 1,
                $tlist[3],
                $tlist[2],
                $tlist[1],
                $tlist[0]);
}

###############################################################################
# CleanupRunAndExit() - Cleans up and die - called by signal handler
###############################################################################
sub CleanupRunAndExit
{
    $Cfg->{'__STATE'} = 3;
    CleanupRun();
    die;
}

###############################################################################
# CleanupRun() - Cleans up a run again.
###############################################################################
sub CleanupRun
{
    AddWebComment();
    
    if ($Cfg->{'CLEANUP'})
    {
	    if ($Cfg->{'__ZIPFILE'})
	    {
	        print "Removing ZIP file..";
	        unlink ($Cfg->{'__ZIPFILE'});
	        if (-e $Cfg->{'__ZIPFILE'})
	        {
	            print "..FAILED!\n";
	        }
	        else
	        {
	            print "..OK\n";
	        }
	    }
    }
        
    if ($Cfg->{'__P4CLIENT'})
    {
        CheckRevertOpenFile();        
        print "Removing P4 Client..";
        if (P4RemClient($Cfg->{'P4PORT'}, $Cfg->{'__P4CLIENT'}))
        {
            print "..OK\n";
        }
        else
        {
            print "..FAILED!\n";
        }
    }
    
    if ($Cfg->{'CLEANUP'})
    {
	    chdir ($Cfg->{'WRKDIR'});
	    if (@{$Cfg->{'__REMDIRS'}})
	    {
	        print "Removing directories..";
	        rmtree (@{$Cfg->{'__REMDIRS'}},0,0);
	        print "..OK\n";
	    }
    }
}

###############################################################################
# CheckRevertOpenFile() - Checks an reverts any opened files.
###############################################################################

sub CheckRevertOpenFile
{
    if ($Cfg->{'__OPEN_FILE'})
    {
        print "Reverting file '$Cfg->{'__OPEN_FILE'}'..";
        if (P4Revert($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, $Cfg->{'__OPEN_FILE'}))
        {
            print "..OK\n";
        }
        else
        {
            print "..FAILED!\n";
        }
    }
}

###############################################################################
# LoadCfg() - Loads the config file and then overloads cmdline params:
###############################################################################

sub LoadCfg
{
    if (-e "$ENV{'TOOL'}/config/sutcheck.cfg")
    {
        $Cfg = LoadSimpleCfg($CfgFile);
    }
    
    $Cfg->{'ASKFORUSER'} = (defined $Opts{'askforuser'} ? $Opts{'askforuser'} : (defined $Cfg->{'ASKFORUSER'} ? $Cfg->{'ASKFORUSER'} : 0));
    $Cfg->{'EDITOR'}    = (defined $Opts{'ed'} ? $Opts{'ed'} : ($Cfg->{'EDITOR'} ? $Cfg->{'EDITOR'} : ""));
    $Cfg->{'WRKDIR'}    = ($Opts{'wrkdir'} ? $Opts{'wrkdir'} : ($Cfg->{'WRKDIR'} ? $Cfg->{'WRKDIR'} : "$ENV{'TOOL'}/tmp"));
    $Cfg->{'USER'}      = uc($Opts{'user'} ? $Opts{'user'} : ($Cfg->{'USER'} ? $Cfg->{'USER'} : (getlogin() ? getlogin() : getpwuid($<))));
    $Cfg->{'ALL'}       = (defined $Opts{'all'} ? $Opts{'all'} : (defined $Cfg->{'ALL'} ? $Cfg->{'ALL'} : 0));
   
    if ($Cfg->{'ASKFORUSER'} && !$Opts{'save'}) { 
        my $duser = RequestInput("Check tests for ($Cfg->{'USER'}|all):", "$Cfg->{'USER'}", "([dD]?\\d+)|([aA][lL][lL])");
        if ( $duser =~ /all/i ) {
            $Cfg->{'ALL'}  = 1;
        } else {
            $Cfg->{'USER'} = $duser;
        }
    }
    
    $Cfg->{'P4PORT'}    = ($Opts{'p4p'} ? $Opts{'p4p'} : ($Cfg->{'P4PORT'} ? $Cfg->{'P4PORT'} : "perforce3006.wdf.sap.corp:3006"));
    $Cfg->{'P4USER'}    = ($Opts{'p4u'} ? $Opts{'p4u'} : ($Cfg->{'P4USER'} ? $Cfg->{'P4USER'} : $Cfg->{'USER'}));
    $Cfg->{'P4PASSWD'}  = ($Opts{'p4pw'} ? $Opts{'p4pw'} : ($Cfg->{'P4PASSWD'} ? $Cfg->{'P4PASSWD'} : ""));
    $ENV{'P4PASSWD'}    = $Cfg->{'P4PASSWD'};
    $Cfg->{'CLEANUP'}   = (defined $Opts{'cleanup'} ? $Opts{'cleanup'} : (defined $Cfg->{'CLEANUP'} ? $Cfg->{'CLEANUP'} : 1));
    $Cfg->{'SQLCLI'}    = ($Opts{'sqlcli'} ? $Opts{'sqlcli'} : $Cfg->{'SQLCLI'});
    $Cfg->{'__LOGIN'}   = uc(getlogin() ? getlogin() : getpwuid($<));

    $Cfg->{'__SQL'}       = Tool::SQLCli->new( { 	'HOSTNAME' 	=> 'pts.wdf.sap.corp', 
				                                'DBNAME' 	=> 'QADB', 
                                				'USER' 		=> 'TESTER', 
                                  				'PASSWORD' 	=> 'TESTER', 
                                				'SQLCLI' 	=> "$Cfg->{'SQLCLI'}"});
                                				
    $Cfg->{'__SQL'}       = undef unless ($Cfg->{'__SQL'}->validate());
}

###############################################################################
# RegisterFileExt() - Registers file extension for .sck files
###############################################################################
sub RegisterFileExt
{
    require Win32::TieRegistry;
    import  Win32::TieRegistry ( ArrayValues=>1 );
    
    print "Registering file extension..";
    
    my $Reg = Win32::TieRegistry->new("Classes")->TiedRef();
    $Reg->Delimiter("/");
    $Reg->{".sck/"} = {"/" => "sck_auto_ext"};
    $Reg->{"sck_auto_ext/"} = { "shell/" => { "open/" => { "command/" => { "/" => ($ENV{'TOOLSHELL'} ? "$ENV{'TOOLSHELL'}" : ($ENV{'PERL'} ? "$ENV{'PERL'}\\bin\\perl.exe" : "$ENV{'TOOL'}\\Perl\\bin\\perl.exe")) . " $ENV{'TOOL'}\\bin\\sutcheck.pl \"%1\"" } } } };
    print "..OK\n";    
}

###############################################################################
# UnregisterFileExt() - Unregisters .sck file extension again
###############################################################################
sub UnregisterFileExt
{
    require Win32::TieRegistry;
    import  Win32::TieRegistry ( Delimiter=>"/", ArrayValues=>1 );
    
    print "Unregistering file extension..";
    
    my $file_ext = "sck";
    my $Reg = Win32::TieRegistry->new("Classes")->TiedRef();
    $Reg->Delimiter("/");
    
    my $keyname = $file_ext . "_auto_ext";
    
    delete $Reg->{".sck/"};
    delete $Reg->{"sck_auto_ext/shell/open/command/"};
    delete $Reg->{"sck_auto_ext/shell/open/"};
    delete $Reg->{"sck_auto_ext/shell/"};
    delete $Reg->{"sck_auto_ext/"};
    print "..OK\n";    
}

###############################################################################
# Usage() - Shows program usage and exits program.
###############################################################################
sub Usage
{
    print <<USAGE_END;
Usage: sutcheck [-options] sutfile.sck

Options:
    -all <0|1>     Check all users file, if set to 1.
    -ed [Editor]   Editor to view diff files. Use 'diffview' to use diffview.
    -h | help      Shows this help.
    -p4p <PORT>    Sets a Perforce port other than 'perforce:3006'.
    -p4u <USER>    Sets a Perforce user other than '-user'.
    -p4pw          Sets the environmant variable %P4PASSWD%.
    -register      Registers file extension '.sck' to be a sutcheck file.
    -save          Save program options under %TOOL%\\config\\shutcheck.cfg
    -unregister    Removes file extension registration for '.sck' again.
    -user <User>   Take given D-User number and check only these files.
    -wrkdir <Dir>  Use given directory as working directory.
    -showcfg       Show config used and exit.
    -sqlcli <File> Uses <File> to add entries into PTS database.
    -askforuser <0|1> Configure user handling
    
USAGE_END
    #sleep (10);
    exit(0);
}

###############################################################################
# GetP4Root() - Retrieves the actual Perforce root path
###############################################################################
sub GetP4Root
{
    my ($rel, $cor, $build) = @_;
    
	return "//sapdb/DEV/develop" if ($rel =~ /^DEV$/);
    $build = "develop" if ($build =~ /^dev/i);
    $build = lc($build);
    my $p4root = "//sapdb/V$rel";

    my $dir = P4GetDir($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $p4root);
 
    unless ($dir->{'DIRS'})
    {
		
		if (($rel > 0) && ($build =~ /DEV/i)) {
			return "//sapdb/DEV/develop";
		}
        print "ERROR: Can't read Perforce directory '//sapdb/V$rel'!\n";
        return undef;
    }
    
    my $hinum = -1;
	my $build_found = 0;
    
    foreach my $subdir (@{$dir->{'DIRS'}})
    {
        if ($subdir =~ /c_(\d\d)/)
        {
            $hinum = $1 if ($1 > $hinum);
        }
		$build_found = 1 if ($subdir =~ /$build/i);
    }

    if ($hinum == $cor - 1)
    {
        $p4root = "//sapdb/V$rel/$build";
    }
    else
    {
        $p4root = "//sapdb/V$rel/c_$cor/$build";
    }

    $dir = P4GetDir($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $p4root);
    
    if ($dir->{'DIRS'})
    {
        return $p4root;
    }
    elsif ($build =~ /^ramp$/)
    {
        if ($dir->{'FILES'})
        {
            if (open(BUILD_NR, "p4 -p $Cfg->{'P4PORT'} -u $Cfg->{'P4USER'} print -q $p4root/buildnumber.cfg |"))
            {
                if (<BUILD_NR> =~ /^build=(b_\d\d)$/)
                {
                    $build = $1;
                    $p4root =~ s/ramp/$build/;
                    close (BUILD_NR);
                    return $p4root;
                }
                close (BUILD_NR);
                return "";
            }
        }
    }
    elsif (($hinum == $cor) && ($build =~ /^dev/i)) {
		return ($build_found ? "//sapdb/V$rel/$build" : "//sapdb/DEV/develop");
    }
    else
    {
        print "ERROR: Can't read Perforce directory '$p4root'!\n";
        return undef;
    }
}

###############################################################################
# mktemp() - Creates a temporary, unique name
###############################################################################
sub mktemp
{
    my @time = localtime(time);
    
    return "SUT_$time[0]$time[1]$time[2]$time[3]$time[4]$time[5]";
}

###############################################################################
# GetFileType() - Returns the type of a file to be checked.
###############################################################################
sub GetFileType
{
    my ($type, $file) = @_;
    
    my @existing = grep { /\Q$file\E/ } @{$Cfg->{'__P4SUTDIR'}->{'FILES'}};
    
    my @SuffList = @{$SUTTypes->{uc($type)}};
    while (@SuffList)
    {
        my $suff = shift @SuffList;
        if (grep { /$file\.$suff/ } @existing)
        {
            return $suff;
        }
    }
    return "ERROR!";
}

###############################################################################
# RemoveDoubles() - Removes identical entries.
###############################################################################
sub RemoveDoubles
{
    my $counter = 0;
    print "Removing identical entries:\n";
    my @tocheck = ('slow', 'quick', 'fast', 'dbuni', 'uni', 'xci', 'alluni');
    foreach my $type ('slow', 'quick', 'fast', 'dbuni', 'uni', 'xci', 'alluni')
    {
        shift @tocheck;
        
        opendir (IN_DIR, "$type") or next;
        my @files = grep { /\.pdiff/ } readdir(IN_DIR);
        closedir(IN_DIR);

        foreach my $file (@files)
        {
            $file = $1 if ($file =~ /(.*)\.pdiff/);
            my $ftype = GetFileType($type, $file);
            foreach my $checkdir (@tocheck)
            {
                if (-e "$checkdir/$file.pdiff")
                {
                    if ((GetFileType($checkdir,$file) eq $ftype) && (compare("$type/$file.pdiff","$checkdir/$file.pdiff") == 0))
                    {
                        print "Difference $checkdir/$file is identical to $type/$file..";
                        if (unlink ("$checkdir/$file.pdiff", "$checkdir/$file.prot"))
                        {
                            print "..Deleted.\n";
                            SetChecked("$checkdir/$file");
                            $counter += 1;
                        }
                        else
                        {
                            print "..FAILED to delete!\n";
                        }
                    }
                }
            }
        }
    }
    print "$counter duplicates removed.\n";
}

###############################################################################
# AddFileToMac() - Adds a new file to a mac description.
###############################################################################
sub AddFileToMac
{
    my ($dir,$file) = @_;
    
    print "Adding $file to mac description..";
    my $macfile = (($dir =~ /^slow|quick|fast$/i) ? "sut.mac" : "sutuni.mac");

    P4Sync($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$DESCDir", undef, 1, 0, ("$macfile"));
    unless (-e "$Cfg->{'__WRKDIR'}$DESCDir/$macfile")
    {
        print "..P4SYNC FAILED!\n";
        return 0;
    }
    unless (P4Edit($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$DESCDir/$macfile"))
    {
        print "..P4EDIT FAILED!\n";
        return 0;
    }

    $Cfg->{'__OPEN_FILE'} = "$Cfg->{'__P4BRANCH'}$DESCDir/$macfile";

    unless (open(IN_FILE, "< $Cfg->{'__WRKDIR'}$DESCDir/$macfile"))
    {
        print "..Open file FAILED!\n";
        CheckRevertOpenFile();
        return 0;
    }
    
    my @lines;
    my $found = 0;
    while (<IN_FILE>)
    {
        if ($_ eq $file)
        {
            print "..File already up-to-date.\n";
            $found = 1;
            last;
        }            
        push @lines, $_;
    }
    
    close (IN_FILE);
    
    if ($found)
    {
        CheckRevertOpenFile();
        return 1;
    }
    
    push @lines, "$file\n";
    unlink ("$Cfg->{'__WRKDIR'}$DESCDir/$macfile");
    unless (open (OUT_FILE, "> $Cfg->{'__WRKDIR'}$DESCDir/$macfile"))
    {
        print "..Write/Open file FAILED!\n";
        CheckRevertOpenFile();
        return 0;
    }
    while (@lines)
    {
        print OUT_FILE "$lines[0]";
        shift @lines;
    }
    unless (close (OUT_FILE))
    {
        print "..FAILED to save new data!\n";
        CheckRevertOpenFile();
        return 0;
    }

    unless (P4Submit($Cfg->{'P4PORT'}, $Cfg->{'P4USER'}, $Cfg->{'__P4CLIENT'}, "$Cfg->{'__P4BRANCH'}$DESCDir/$macfile", "File '$macfile' submitted by $ProgramName v$Version.$Revision"))
    {
        print "..Submit FAILED!\n";
        CheckRevertOpenFile();
        return 0;
    }

    $Cfg->{'__OPEN_FILE'} = undef;
    print "..OK\n";
    return 1;    
}
