
#! /usr/bin/perl
#
#    LCEnv.pm - Creates and syncs a complete SAPDB LC make environment.
#
#    @(#)LCEnv     2003-06-17
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

require Exporter;

@ISA        = qw(Exporter);
@EXPORT_OK  = qw(LCECheckEnv LCESetDefaults LCECheckP4Env LCEListRels LCECheckLCVersion LCECheckP4ServerPath LCECheckP4ClientPath LCECheckLCOwn LCECheckLCChangeList LCECreateEnv LCECreateLCOwn LCECreateX64Env LCECreateP4User LCECreateP4Client LCEP4SyncEnv LCECreateWinIcon LCECreateRelDBEntry LCERemRelDBEntry LCERemWinIcon LCECreateWinShare LCECreateIProfile LCEDoMake LCERemWinShare LCERemP4Files LCERemP4Client LCERemP4User LCERemLCOwn LCESendMail LCECopyIUser LCESetState LCEGetState);

use strict;
use IO::CurrentDir;
use IO::FilePath;
use Tool::HashRef;
use Tool::RequestInput;
use Perforce::P4Admin;
use Perforce::P4Request;
use Perforce::P4Info;
use Perforce::P4Sync;
use File::Path;
use File::Copy;

my %DBFiles = ( 'SHORTCUTS' => "$ENV{'TOOL'}\\data\\shortcuts",
                'SHARES'    => "$ENV{'TOOL'}\\data\\shares",
                'RELEASES'  => "$ENV{'TOOL'}\\data\\releases" );

my $PathSep = ($^O =~ /MSWin32/i) ? "\\" : "/";

my $WatchDetail = "";
my $ErrorDetailLink = "";

1;

##############################################################################
# LCECheckEnv() - Check a complete LC environment by calling funcs below.
##############################################################################

sub LCECheckEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCECheckEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    LCESetState($href, "checking");

    unless (LCECheckP4Env        (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Checking Perforce settings failed!") }
    unless (LCECheckLCVersion    ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Checking LC Version failed! Please check the version\nformat of '$href->{'RELEASE'}->{'VERSION'}'") }
    unless (LCECheckP4ServerPath ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Checking P4 server path failed! Please check the LC settings and\nmake sure that the desired LC version exists.") }
    unless (LCECheckLCOwn        ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Checking of Own directory failed! Please check your P4 / Release settings!") }
    unless (LCECheckP4ClientPath ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Setting the Perforce client path failed!\nPlease check Own, P4Client and P4Root. Make also sure that\nOwn is a sub-directory of the Perforce root directory.") }
    unless (LCECheckLCChangeList ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECheckEnv", "Checking the changelist number failed!\nPlease check your perforce settings, especially 'CLIENT_PATH'.") }
    
    $WatchDetail .= "\nEnvironment check completed successfully.\n";
    
    LCESetState($href, "checked");
}

##############################################################################
# LCECreateEnv() - Creates everything needed for the environment.
##############################################################################

sub LCECreateEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCECreateLCEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    LCESetState($href, "initializing");

    unless (LCECreateLCOwn       ($href->{'RELEASE'}                     , $href->{'GLOBAL'})) { _throw_error("LCECreateLCEnv", "Cannot create Own directory! Please make sure that\n'$href->{'RELEASE'}->{'OWN'}' is a valid name.") }
    unless (LCECreateX64Env      ($href->{'RELEASE'}                     , $href->{'GLOBAL'})) { _throw_error("LCECreateLCEnv", "Cannot apply 64 bit cross-development patch.") }
    unless (LCECreateP4User      (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECreateLCEnv", "Creating Perforce user '$href->{'PERFORCE'}->{'USER'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCECreateP4Client    (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECreateLCEnv", "Creating / updating Perforce client '$href->{'PERFORCE'}->{'CLIENT'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCEP4SyncEnv         ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCECreateLCEnv", "Sync of sources failed! Please check your Perforce and release settings.") }
    unless (LCECreateIProfile    ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCECreateLCEnv", "Can't create iprofile! Please check you ENV variable 'TOOL' and your write permissins!") }
    unless (LCECopyIUser         ($href->{'RELEASE'}                     , $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCECreateLCEnv", "Copying IUser.bat failed!") }
    unless (LCESetEnv            ($href->{'ENV'})) { _throw_error("LCECreateLCEnv", "Setting Environment failed!") }
    unless (LCECreateWinIcon     ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCECreateLCEnv", "Can't create Windows shortcut! Please check your user settings in environment.") }
    unless (LCECreateWinShare    ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCECreateLCEnv", "Can't create Windows share!") }
#    unless (LCECreateRelDBEntry  ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCECreateLCEnv", "Can't add entry to release DB!") }

    $WatchDetail .= "\nEnvironment creation completed successfully.\n";

    LCESetState($href, "initialized");
}

##############################################################################
# LCERemEnv() - Removes the complete environment.
##############################################################################

sub LCERemEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCERemEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    LCESetState($href, "removing");

#    unless (LCERemRelDBEntry     ($href->{'RELEASE'}                     , $href->{'GLOBAL'})) { _throw_error("LCERemEnv", "Removing entry from release DB failed!") }
    unless (LCERemWinShare       ($href->{'RELEASE'}                     , $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCERemEnv", "Removing Windows share FAILED!") }
    unless (LCERemWinIcon        ($href->{'RELEASE'}                     , $href->{'GLOBAL'}, $href->{'ACTIONS'})) { _throw_error("LCERemEnv", "Removing Windows shortcut FAILED???!!!") }
    unless (LCERemP4Files        (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCERemEnv", "Removing Perforce administrated files failed!\nPlease check your Perforce settings.") }
#    unless (LCERemP4Client       (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCERemEnv", "Removing Perforce client '$href->{'PERFORCE'}->{'CLIENT'}' failed!\nPlease check your Perforce settings and connection.") }
#    unless (LCERemP4User         (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCERemEnv", "Removing Perforce user '$href->{'PERFORCE'}->{'USER'}' failed!\nPlease check your Perforce settings and connection.") }
#    unless (LCERemLCOwn          ($href->{'RELEASE'}                     , $href->{'GLOBAL'})) { _throw_error("LCERemEnv", "Removing of Own directory '$href->{'RELEASE'}->{'OWN'}' failed!\nThis could be caused by the 'rm' system command called.") }

    $WatchDetail .= "\nEnvironment removal completed successfully.\n";
    
    LCESetState($href, "removed");
}

##############################################################################
# LCESetDefaults() - Sets default values for LCEnv.
##############################################################################

sub LCESetDefaults
{
    my $href = shift;
    my $sel;

    unless (ref($href)) { _throw_error("LCESetDefaults", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nWhere do you want your Release configurations to be stored:\n\n";
    print "[o] = Each in its release's own directory.\n";
    print "[c] = Centrally, under $ENV{'TOOL'}" . $PathSep . "config" . $PathSep . "LCEnv. (default)\n";
    print "[n] = No change ($href->{'GLOBAL'}->{'CFG_PATH'})\n" if ($href->{'GLOBAL'}->{'CFG_PATH'});
    print "[s] = Centrally, i'd like to choose a path manually.\n\n";

    print "NOTE: If you store configurations centrally, you don't have to give a complete\n";
    print "      path when selecting a configuration.\n\n";

    $sel = RequestInput ("Please select : ", "c", ($href->{'GLOBAL'}->{'CFG_PATH'} ? "[oOcCsSnN]" : "[oOcCsS]"), "Input incorrect. Please, just use 'o', 'c'" . ($href->{'GLOBAL'}->{'CFG_PATH'} ? ", 'n' " : "") . " or 's'.\n");

    if ($sel =~ /^o$/i)
    {
        $href->{'GLOBAL'}->{'CFG_PATH'} = "";
    }
    elsif ($sel =~ /^c$/i)
    {
        $href->{'GLOBAL'}->{'CFG_PATH'} = "$ENV{'TOOL'}" . $PathSep . "config" . $PathSep . "LCEnv";
        mkpath ($href->{'GLOBAL'}->{'CFG_PATH'},0 ,0777);
    }
    elsif ($sel =~ /^n$/i)
    {
        $href->{'GLOBAL'}->{'CFG_PATH'} = $href->{'GLOBAL'}->{'CFG_PATH'};
    }
    else
    {
        $href->{'GLOBAL'}->{'CFG_PATH'} = InterpolatePath(RequestInput ("Configuration dir : ", "", ".+", "Input incorrect. Empty strings are not allowed.\n"));
        mkpath ($href->{'GLOBAL'}->{'CFG_PATH'},0 ,0777);
    }

    if (RequestInput("\nDo you want the release configuration files to be restricted\nto your user only (nobody else can read these files) ? (y/N) ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
    {
        $href->{'GLOBAL'}->{'SEC_CFG'} = 1;
    }
    else
    {
        $href->{'GLOBAL'}->{'SEC_CFG'} = 0;
    }

    if (RequestInput("\nPerforce support makes LCEnv robust against Perforce changes, but makes opening\nwindows slower. Any given Perforce environment will be set in either case, but\nwithout Perforce support you will not be able to use -sync option. If you say\n'no' now and want to use -sync later, you always have to also use -p4=1 to\nreactivate Perforce support.\nDo you want to activate Perforce support for LCEnv ? (y/N) ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
    {
        $href->{'PERFORCE'}->{'USE'} = 1;
    }
    else
    {
        $href->{'PERFORCE'}->{'USE'} = 0;
    }

    if (RequestInput("\nDo you want Perforce to sync your files when opening a new window ? (y/N) ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
    {
        $href->{'PERFORCE'}->{'SYNC'} = 1;
    }
    else
    {
        $href->{'PERFORCE'}->{'SYNC'} = 0;
    }

    if (RequestInput("\nDo you want to be asked for your perforce settings every time when creating\na new instance (to manage multiple configurations) ? (y/N) ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
    {
        $href->{'PERFORCE'}->{'ALWAYS_ASK'} = 1;
    }
    else
    {
        $href->{'PERFORCE'}->{'ALWAYS_ASK'} = 0;
    }

    if (RequestInput("\nDo you want to set up your default perforce settings now ? (Y/n) ", "y", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
    {
        unless (LCECheckP4Env($href->{'PERFORCE'}, $href->{'GLOBAL'}, 1)) { _throw_error("LCESetDefaults", "Checking Perforce settings failed!") }
    }

    if ($^O =~ /MSWin32/i)
    {
        require Win32::Shortcut;
        require Win32::TieRegistry;

        if (RequestInput("\nDo you want a new shortcut to be created by default ? (Y/n) ", "y", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
        {
            $href->{'ACTIONS'}->{'CREATE_ICON'} = 1;
        }
        else
        {
            $href->{'ACTIONS'}->{'CREATE_ICON'} = 0;
        }

        print "\nWhere do you want to store your shortcuts:\n\n";
        print "[m] = In Windows start menu. (default)\n";
        print "[n] = No change ($href->{'GLOBAL'}->{'ICON_PATH'})\n" if ($href->{'GLOBAL'}->{'ICON_PATH'});
        print "[e] = At a place stored in \%DEVSTUDIO_SHORTCUTS\%\n" if ($ENV{'DEVSTUDIO_SHORTCUTS'});
        print "[d] = On desktop.\n\n";
        
        $sel = RequestInput ("Please select : ", "m", ($href->{'GLOBAL'}->{'ICON_PATH'} ? "[mMdDeEsSnN]" : "[mMdDeEsS]"), "Input incorrect. Please, just use 'm', 'd', 'e'" . ($href->{'GLOBAL'}->{'ICON_PATH'} ? ", 'n' " : "") . " or 's'.\n");

        if ($sel =~ /^m$/i)
        {
            $href->{'GLOBAL'}->{'ICON_PATH'} = Win32::TieRegistry->new("LMachine")->TiedRef()->{'Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\\\Common Start Menu'} . "\\SAPDevelop $ENV{'USERNAME'}";
        }
        elsif ($sel =~ /^d$/i)
        {
            $href->{'GLOBAL'}->{'ICON_PATH'} = Win32::TieRegistry->new("LMachine")->TiedRef()->{'Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\\\Common Desktop'};
        }
        elsif ($sel =~ /^e$/i)
        {
            $href->{'GLOBAL'}->{'ICON_PATH'} = "";
        }
        elsif ($sel =~ /^n$/i)
        {
            $href->{'GLOBAL'}->{'ICON_PATH'} = $href->{'GLOBAL'}->{'ICON_PATH'};
        }
        else
        {
            $href->{'GLOBAL'}->{'ICON_PATH'} = InterpolatePath (RequestInput ("Shortcut path : ", "", ".+", "Input incorrect. Please, don't use empty strings.\n"));
        }
        
        if (RequestInput("\nShould a \$TOOL/config/LCEnvIcon.lnk to be used as template for shortcuts ? (y/N) ", "n", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i) {
            $href->{'GLOBAL'}->{'ICON_TEMPLATE'} = 1;
        } else {
            $href->{'GLOBAL'}->{'ICON_TEMPLATE'} = 0;
        }

        if (RequestInput("\nDo you want a new windows network share to be created by default ? (Y/n) ", "y", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
        {
            $href->{'ACTIONS'}->{'CREATE_SHARE'} = 1;
        }
        else
        {
            $href->{'ACTIONS'}->{'CREATE_SHARE'} = 0;
        }

        if (RequestInput("\nShould a \$TOOL/config/iuser.default copied to \$OWN/iuser.bat by default ? (Y/n) ", "y", "[yYnN]", "Input incorrect. Please, just use 'y' or 'n'.\n") =~ /^y/i)
        {
            $href->{'ACTIONS'}->{'COPY_IUSER'} = 1;
        }
        else
        {
            $href->{'ACTIONS'}->{'COPY_IUSER'} = 0;
        }
    }
}

##############################################################################
# LCEDoMake() - Executes the make process.
##############################################################################

sub LCEDoMake
{
    my $href = shift;
    my ($makecmd, $rc, $makeid, $runcmd);

    unless (ref($href)) { _throw_error("LCEDoMake", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unlink ("$href->{'RELEASE'}->{'OWN'}/make.id");
    $href->{'GLOBAL'}->{'__MAKE_RESULT'} = 1;

    return 1 unless ($href->{'ACTIONS'}->{'MAKE'});
    
    LCESetState($href, "running make");

    if ($href->{'GLOBAL'}->{'PROD_MAKE'})
    {
        $ENV{'NACHTMAKE_CFG'} = $href->{'GLOBAL'}->{'PROD_CFG'} if ($href->{'GLOBAL'}->{'PROD_CFG'});
        $makecmd = "NACHT.ALL";
        $makecmd .= " -scratch" if ($href->{'GLOBAL'}->{'SCRATCH'});
        $makecmd .= " -html" if ($href->{'GLOBAL'}->{'HTML_OUT'});
        $makecmd .= " -email $href->{'GLOBAL'}->{'PROD_EMAIL'}" if ($href->{'GLOBAL'}->{'PROD_EMAIL'});
        if ($href->{'GLOBAL'}->{'WATCHER'})
        {
            $makecmd .= " -errout \"$href->{'RELEASE'}->{'OWN'}/ErrorMakeMail.txt\"";
            unlink "$href->{'RELEASE'}->{'OWN'}/ErrorMakeMail.txt";
        }
        $WatchDetail .= "\nStarting productive make using command: $makecmd\n";
    }
    elsif ($href->{'ACTIONS'}->{'TARGET'})
    {
        if ($href->{'GLOBAL'}->{'SCRATCH'})
        {
            $makecmd = "imf -U $href->{'ACTIONS'}->{'TARGET'}";
        }
        else
        {
            $makecmd = "imf $href->{'ACTIONS'}->{'TARGET'}";
        }
        
        $WatchDetail .= "\nStarting normal make using command: $makecmd\n";
    }
    else
    {
        $makecmd = "";
    }

    if ($^O =~ /MSWin32/i)
    {
        open (CMD_OUT, ">$href->{'RELEASE'}->{'OWN'}\\irun.bat");
        print CMD_OUT "\@echo off\n";

        if ($href->{'RELEASE'}->{'OWN'} =~ /^(\S):.*/)
        {
            print CMD_OUT "$1:\n";
        }

        print CMD_OUT "cd $href->{'RELEASE'}->{'OWN'}\n";
        print CMD_OUT "call iprofile.bat\n";
        if ($makecmd ne "")
        {
            print CMD_OUT "$makecmd\n";
            print CMD_OUT "set MAIN_ERR=\%errorlevel\%\n";
        }
        if ($href->{'ACTIONS'}->{'CMD'})
        {
            print CMD_OUT "$href->{'ACTIONS'}->{'CMD'}\n";
            print CMD_OUT "set CMD_ERR=\%errorlevel\%\n";
        }

        if (($makecmd ne "") && !$href->{'ACTIONS'}->{'CMD'})
        {
            print CMD_OUT "exit \%MAIN_ERR\%\n";
        }
        elsif (($makecmd eq "") && $href->{'ACTIONS'}->{'CMD'})
        {
            print CMD_OUT "exit \%CMD_ERR\%\n";
        }
        elsif (($makecmd ne "") && $href->{'ACTIONS'}->{'CMD'})
        {
            print CMD_OUT "IF \%MAIN_ERR\% GTR 0 (exit \%MAIN_ERR\%) ELSE exit \%CMD_ERR\%\n";
        }
        
        close CMD_OUT;
        if ($ENV{'COMSPEC'})
        {
            $runcmd = "$ENV{'COMSPEC'} /C $href->{'RELEASE'}->{'OWN'}\\irun.bat";
        }
        else
        {
            $runcmd = "cmd.exe /C $href->{'RELEASE'}->{'OWN'}\\irun.bat";
        }
        #unlink "$href->{'RELEASE'}->{'OWN'}\\irun.bat";
    }
    else
    {
        open (CMD_IN, "which bash |");
        my $bashfile = <CMD_IN>;
        if ($bashfile =~ /no\sbash\sin/)
        {
            $bashfile = "/usr/local/bin/bash";
        }
        close (CMD_IN);
        open (CMD_OUT, ">$href->{'RELEASE'}->{'OWN'}/irun.sh");
        print CMD_OUT "#!$bashfile\n";
        print CMD_OUT 'echo $$ > ' . "$href->{'RELEASE'}->{'OWN'}/irun.pid\n";
        print CMD_OUT "cd $href->{'RELEASE'}->{'OWN'}\n";
        print CMD_OUT "source .iprofile\n";
        if ($makecmd ne "")
        {
            print CMD_OUT "$makecmd\n";
            print CMD_OUT "MAIN_ERR=\$?\n";
        }
        print CMD_OUT "$href->{'ACTIONS'}->{'CMD'}\n" if ($href->{'ACTIONS'}->{'CMD'});
        print CMD_OUT "rm -rf $href->{'RELEASE'}->{'OWN'}/irun.pid\n";
        print CMD_OUT "echo Main target reported RC \$MAIN_ERR\n";
        print CMD_OUT "exit \$MAIN_ERR\n";
        close CMD_OUT;
        system("chmod ug+x $href->{'RELEASE'}->{'OWN'}/irun.sh");
        $runcmd = "$href->{'RELEASE'}->{'OWN'}/irun.sh";

        #unlink "$href->{'RELEASE'}->{'OWN'}/irun.bash";
    }

    my $xout_buffer = "";

    if ($href->{'GLOBAL'}->{'SYSCALL'})
    {
        $rc = system("$runcmd 2>&1");
    }
    else
    {
	    if (open(RUN_OUT, "$runcmd 2>&1 |"))
	    {
	        while (<RUN_OUT>)
	        {
	            print "$_";
	            $xout_buffer .= "$_" if ($href->{'GLOBAL'}->{'XOUT'});
	        }
	        close (RUN_OUT);
	        $rc = $?;
	    }
	    else
	    {
	        $rc = 99;
	    }
    }
    
    $rc = ($rc >> 8) if ($rc > 255);
    $rc = ($rc - 128) if ($rc > 128);
    print "\nBatch execute resulted in RC $rc\n";        

    $WatchDetail .= "Using post make cmd: $href->{'ACTIONS'}->{'CMD'}\n" if ($href->{'ACTIONS'}->{'CMD'});
    $WatchDetail .= ($rc == 0 ? "\nMake was successful!\n" : (($rc == 9) ? "\nMake *TERMINATED*!!\n" : "\nMake *FAILED*!!\n"));

    if (-e "$href->{'RELEASE'}->{'OWN'}/ErrorMakeMail.txt")
    {
        if (open (ERR_FILE, "< $href->{'RELEASE'}->{'OWN'}/ErrorMakeMail.txt"))
        {
            while (<ERR_FILE>)
            {
                $WatchDetail .= "$_";
                if (/^Error\sProtocol\s:\s(.*)$/)
                {
                    $ErrorDetailLink = $1;
                }
            }
            close (ERR_FILE);
        }
    }

    $href->{'GLOBAL'}->{'__MAKE_RESULT'} = ($rc == 0 ? ((-e "$href->{'RELEASE'}->{'OWN'}/ErrorMakeMail.txt") ? 4 : 2) : ($rc == 9 ? 16 : 8));

    if ($href->{'GLOBAL'}->{'XOUT'})
    {
        $WatchDetail .= "\n(----------------------- EXTENDED OUTPUT -----------------------)\n\n";
        $WatchDetail .= $xout_buffer;
    }

    LCESetState($href, "make done");
    
    if ($href->{'GLOBAL'}->{'PROD_MAKE'} && ($rc == 9) && (-e "$href->{'RELEASE'}->{'OWN'}/make.id"))
    {
        if (open (IN_FILE, "< $href->{'RELEASE'}->{'OWN'}/make.id"))
        {
            my $makeid = 0;
            while (<IN_FILE>)
            {
                $makeid = $1 if (/^(\d+)$/);
            }
            close (IN_FILE);
            
            if ($makeid > 0)
            {
                eval
                {
                    require qadb;
                    import qadb;
                    
                    my $qah = qadb->new( {'ID' => "$makeid" } );
                    if ($qah->{'error_code'} == 0)
                    {
                        $qah->update_columns({'IDOBJSTATUS' => "990", 'LCPOOLID' => 'cncl'});
                    }
                };
                
                if ($@)
                {
                    print "Error updating qadb cols: $@\n";
                }
            }
        }
    }

    return $rc;
}

##############################################################################
# LCEListRels() - Lists all installed releases.
##############################################################################

sub LCEListRels
{
    my %ReleaseDB;
    my @RelOut;
    my ($oldkey, $oldval);

    print "Instances currently installed:\n\n";

    if (dbmopen(%ReleaseDB, $DBFiles{'RELEASES'}, 0777))
    {
        foreach my $key (sort keys %ReleaseDB)
        {
            #$key = lc $key;
            if (($key eq $oldkey) && ($ReleaseDB{$key} eq $oldval))
            {
                next;
            }
            else
            {
                $oldkey = $key;
                $oldval = $ReleaseDB{$key};
            }

            unless (-d $key)
            {
                $ReleaseDB{$key} = undef;
                next;
            }

            next if ($ReleaseDB{$key} eq "");

            if ($ReleaseDB{$key} =~ /^(\S*)\s?(\(.*\))?\s?(\[.*\])?/)
            {
                push @RelOut, (pack "A12", $1) . "$key $3 $2";
            }
        }
        dbmclose(%ReleaseDB);

        foreach (sort @RelOut)
        {
            print "$_\n";
        }
    }
    else
    {
        print "Error! Can't open Database!\n";
    }
}

##############################################################################
# LCECheckP4Env() - Checks / asks for the Perforce environment to be used.
##############################################################################

sub LCECheckP4Env
{
    my ($p4env, $globenv, $force) = @_;

    unless (ref($p4env))   { _throw_error("LCECheckP4Env", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckP4Env", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return $p4env unless ($p4env->{'USE'} or $force);

    $p4env->{'PORT'}   = "perforce3006.wdf.sap.corp:3006" unless ($p4env->{'PORT'});
    $ENV{'P4PORT'}     = $p4env->{'PORT'};
    $p4env->{'USER'}   = ($force ? P4RequestUser(0) : ($p4env->{'USER'}    ? $p4env->{'USER'}     : ($p4env->{'ALWAYS_ASK'} ? P4RequestUser($globenv->{'USE_DEFAULT'}) : ($ENV{'P4USER'} ? $ENV{'P4USER'}      : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckP4Env", "Perforce user missing!")   : P4RequestUser($globenv->{'USE_DEFAULT'}))))));
    $ENV{'P4USER'}     = $p4env->{'USER'};

    if (P4PasswdNeeded($p4env->{'PORT'}, $p4env->{'USER'}))
    {
        $p4env->{'PASSWD'} = ($force ? P4RequestPasswd($p4env->{'USER'}) : ($p4env->{'PASSWD'} ? $p4env->{'PASSWD'} : ($p4env->{'ALWAYS_ASK'} ? P4RequestPasswd($p4env->{'USER'}) : ($ENV{'P4PASSWD'} ? $ENV{'P4PASSWD'} : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckP4Env", "Perforce password missing!") : P4RequestPasswd($p4env->{'USER'}))))));
        $ENV{'P4PASSWD'}   = $p4env->{'PASSWD'};
    }
    else
    {
        $p4env->{'PASSWD'} = $ENV{'P4PASSWD'};
    }

    $p4env->{'CLIENT'} = ($force ? P4RequestClient(0) : ($p4env->{'CLIENT'}  ? $p4env->{'CLIENT'}   : ($p4env->{'ALWAYS_ASK'} ? P4RequestClient($globenv->{'USE_DEFAULT'}) : ($ENV{'P4CLIENT'} ? $ENV{'P4CLIENT'}  : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckP4Env", "Perforce client missing!") : P4RequestClient($globenv->{'USE_DEFAULT'}))))));
    $ENV{'P4CLIENT'}   = $p4env->{'CLIENT'};

    my $clientinfo = P4GetClientInfo($p4env->{'PORT'}, $p4env->{'CLIENT'});

    if (!$p4env->{'_MAN_ROOT'} && ($p4env->{'ROOT'} ne $clientinfo->{'Root'}) && P4ClientExists($p4env->{'PORT'}, $p4env->{'CLIENT'}))
    {
        $p4env->{'ROOT'} = $clientinfo->{'Root'};
        print "\nUsing existing client root '$p4env->{'ROOT'}'.\n";
    }
    $p4env->{'_MAN_ROOT'} = undef;
    
    $p4env->{'ROOT'}   = ($p4env->{'ROOT'}    ? $p4env->{'ROOT'}     : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckP4Env", "Perforce client root path is missing!") : P4RequestRoot($p4env->{'PORT'}, $p4env->{'CLIENT'}, $globenv->{'USE_DEFAULT'})));
    $p4env->{'ROOT'}   = InterpolatePath($p4env->{'ROOT'});

    if (grep {/^\/\/sapdb\/\.\.\./} @{$clientinfo->{'View'}}) {
        print "WARNING: Your Perforce client has defined a general rule for directory 'sapdb'!\n*** THIS MIGHT RESULT IN UNPREDICTABLE BEHAVIOUR OF LCENV ***\n\n";
        exit(1) if ($globenv->{'INIT'} && (RequestInput("Do you REALLY want to continue (y/N)? : ", "n", "[ynYN]", "Input incorrect, please just use y or n.\n") =~ /n/i))
    }
    
    return $p4env;
}

##############################################################################
# LCECheckLCVersion() - Checks for the correct LC version format.
##############################################################################

sub LCECheckLCVersion
{
    my ($lcenv, $p4env, $globenv) = @_;

    my $relver;
    my $corr;
    my $build;
    my $rel_ok = 0;

    unless (ref($lcenv))   { _throw_error("LCECheckLCVersion", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECheckLCVersion", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckLCVersion", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    ($relver, $corr, $build) = _SplitLCVersion($lcenv->{'VERSION'});

    $relver = ($relver ne "" ? $relver : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckLCVersion", "Release version could not be determinated!") : ($p4env->{'USE'} ? _SelectP4Dir($p4env->{'PORT'}, $p4env->{'USER'}, "//sapdb", 'V(\d\d)|(DEV)', "LC release") : RequestInput("\nRelease version (e.g. '74') : ", "", "\\d\\d|DEV", "Input incorrect. Please, use a two-digit number or DEV.\n"))));
    unless ($relver =~ /^DEV$/) {
        $corr   = ($corr ne "" ? $corr : (($globenv->{'INIT'} && !$globenv->{'BATCH_MODE'}) ? ($p4env->{'USE'} ? _SelectP4Dir($p4env->{'PORT'}, $p4env->{'USER'}, "//sapdb/V$relver", 'c_(\d\d)', "LC correction level", 2) : RequestInput("\nCorrection level (e.g. '04', 'l' for latest dev branch) : ", "", "\\d\\d|l", "Input incorrect. Please, use a two-digit number\n")) : ""));
    
        if ($corr =~ /dev/i || $corr =~ /l/i)
        {
            $corr = "";
            $build = "dev";
        }
    
        unless (!$build || ($build =~ /^(\d\d|dev|cor|ramp|develop)$/i))
        {
            print STDERR "WARNING: Incorrect build prefix '$build' has been ignored.\n";
            $build = undef;
        }
    
        $build = ($build ? $build : ($globenv->{'BATCH_MODE'} ? _throw_error("LCECheckLCVersion", "Build prefix could not be determinated!") : ($p4env->{'USE'} ? _SelectP4Dir($p4env->{'PORT'}, $p4env->{'USER'}, "//sapdb/V$relver/c_$corr", 'b?_?(.*)', "LC build prefix", 1) : RequestInput("\nBuild prefix (e.g. '03' or 'dev') : ", "", "\\d\\d|dev|cor|ramp", "Input incorrect. Please, use two.digit numbers or 'dev', 'cor' or 'ramp'.\n"))));
        $build = lc ($build);
    
        if ($build =~ /^develop$/i) { $build = "dev" }
    }
    $lcenv->{'VERSION'} = _JoinLCVersion($relver, $corr, $build);

    $ENV{'RELVER'} = "R$relver";
    $ENV{'CORRECTION_LEVEL'} = "$corr";
    $ENV{'BUILD_PRAEFIX'} = "$build";

    $ENV{'LC_VER_STR'} = "$lcenv->{'VERSION'}";

    return $lcenv->{'VERSION'};
}

##############################################################################
# LCECheckP4ServerPath() - Retrieves the P4 server path for a specific LC Version
##############################################################################

sub LCECheckP4ServerPath
{
    my ($lcenv, $p4env, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCECheckP4ServerPath", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECheckP4ServerPath", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckP4ServerPath", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return $p4env unless ($p4env->{'USE'});

    my ($relver, $corr, $build) = _SplitLCVersion($lcenv->{'VERSION'});

    if ($relver =~ /^DEV$/) {
            $p4env->{'SERVER_PATH'} = "//sapdb/DEV/develop";
            $ENV{'DEPOT'} = $p4env->{'SERVER_PATH'};
            return $p4env;
    }
    
    if ($relver && !$corr && ($build =~ /dev/i))
    {
        print "Checking perforce path for LC V$relver.$build..";

        my $dir = P4GetDir($p4env->{'PORT'}, $p4env->{'USER'}, "//sapdb/V$relver/develop");
        if ($dir->{'DIRS'})
        {
            $p4env->{'SERVER_PATH'} = "//sapdb/V$relver/develop";
            print "..OK\n\n";
            $ENV{'DEPOT'} = $p4env->{'SERVER_PATH'};
            return $p4env;
        }
        else
        {
            print "..FAILED!\n\n";
            return undef;
        }
    }

    unless ($relver && $corr && $build) { _throw_error("LCECheckP4ServerPath", "LC Version doesn't have the correct format ($relver.$corr.$build)!\nPlease, use 'Rel.Corr.Build', ie. '74.03.develop' or '74.03.20'.") }

    if ($build =~ /^dev$/i) { $build = "develop" }

    print "Checking perforce path for LC V$relver.$corr.$build..";

    $p4env->{'SERVER_PATH'} = _GetP4ServerPath($p4env->{'PORT'}, $p4env->{'USER'}, $relver, $corr, $build);

    if ($p4env->{'SERVER_PATH'})
    {
        print "..OK\n\n";
        $ENV{'DEPOT'} = $p4env->{'SERVER_PATH'};
        return $p4env;
    }
    else
    {
        print "..FAILED!\n\n";
        return undef;
    }
}

##############################################################################
# LCECheckP4ClientPath() - Sets the P4 client path
##############################################################################

sub LCECheckP4ClientPath
{
    my ($lcenv, $p4env, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCECheckP4ClientPath", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECheckP4ClientPath", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckP4ClientPath", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return ($lcenv->{'GRP'} ? $lcenv->{'GRP'} : $lcenv->{'OWN'}) unless ($p4env->{'USE'});

    print "Checking Perforce client path..";

    $p4env->{'CLIENT_PATH'} = $lcenv->{'GRP'} ? $lcenv->{'GRP'} : $lcenv->{'OWN'};
    $p4env->{'CLIENT_PATH'} =~ s/\Q$p4env->{'ROOT'}\E/\/\/$p4env->{'CLIENT'}/i;
    $p4env->{'CLIENT_PATH'} =~ tr/\\/\//;

    if ($p4env->{'CLIENT_PATH'} =~ /\Q$p4env->{'CLIENT'}\E/i)
    {
        print "..OK\n\n";
        return $p4env->{'CLIENT_PATH'};
    }
    else
    {
        print "..FAILED!\n\n";
        return undef;
    }
}

##############################################################################
# LCECheckLCOwn() - Checks / Requests for an own directory.
##############################################################################

sub LCECheckLCOwn
{
    my ($lcenv, $p4env, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCECheckLCOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECheckLCOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckLCOwn", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "Checking release OWN..";

    if ($lcenv->{'OWN'})
    {
        print "..OK\n\n";
    }
    else
    {
        my $curdir =  GetCurrentDir();
        my $reldef =  $p4env->{'SERVER_PATH'};

        if ($p4env->{'USE'})
        {
            $reldef    =~ s/^\/\/sapdb/$p4env->{'ROOT'}/;
            $reldef    =  FixPathSep($reldef);
        }

        if ($globenv->{'BATCH_MODE'})
        {
            print "..FAILED!\n\n";
            print "In batch mode, you have to give an Own directory!\n";
        }
        elsif ($globenv->{'USE_DEFAULT'})
        {
            if ($p4env->{'USE'})
            {
                print "..OK\n\n";
                $lcenv->{'OWN'} = $reldef;
            }
            else
            {
                print "..FAILED!\n\n";
                $lcenv->{'OWN'} = undef;
            }
        }
        else
        {
            my $sel;
            print "..NOT SET!\n\n";
            print "\nPlease select which \$Own directory you want to use:\n\n";
            print "[c] = use the current directory ($curdir)\n";

            if ($p4env->{'USE'})
            {
                print "[p] = use P4 client root ($p4env->{'ROOT'})\n";
                print "[r] = use releases default directory ($reldef), default\n";
                print "[s] = select a different own directory manually\n\n";

                $sel = RequestInput("Please select: ", "r", "[cCpPrRsS]", "Input incorrect. Please, just use [c, p, r or s].\n");
            }
            else
            {
                print "[s] = select a different own directory manually, default\n\n";
                $sel = RequestInput("Please select: ", "s", "[cCsS]", "Input incorrect. Please, just use [c or s].\n");
            }

            if ($sel =~ /^c$/i)     { $lcenv->{'OWN'} = $curdir }
            elsif ($sel =~ /^p$/i)  { $lcenv->{'OWN'} = $p4env->{'ROOT'} }
            elsif ($sel =~ /^r$/i)  { $lcenv->{'OWN'} = $reldef }
            else
            {
                if ($p4env->{'USE'})
                {
                    $lcenv->{'OWN'} = JoinPath($p4env->{'ROOT'}, RequestInput("Own directory: $p4env->{'ROOT'}$PathSep", "", ".+", "Input incorrect. Please, just use normal characters.\n"));
                }
                else
                {
                    $lcenv->{'OWN'} = RequestInput("Own directory: ", "", ".+", "Input incorrect. Please, just use normal characters.\n");
                }
            }
        }
    }

    $lcenv->{'OWN'} = InterpolatePath($lcenv->{'OWN'});

    $ENV{'OWN'} = $lcenv->{'OWN'};

    return $lcenv->{'OWN'};
}

##############################################################################
# LCECheckLCChangeList() - Checks / retrieves the LC changelist.
##############################################################################

sub LCECheckLCChangeList
{
    my ($lcenv, $p4env, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCECheckLCChangeList", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECheckLCChangeList", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECheckLCChangeList", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    if ($lcenv->{'CHANGELIST'} > 0)
    {
        print "Setting changelist to '$lcenv->{'CHANGELIST'}'..";
        $lcenv->{'_CHANGELIST'} = $lcenv->{'CHANGELIST'};
        print "..OK\n\n";
    }
    else
    {
        print "Retrieving actual changelist..";
        $lcenv->{'_CHANGELIST'} = P4GetChangeList($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'SERVER_PATH'});
        if ($lcenv->{'_CHANGELIST'})
        {
            print "..OK ($lcenv->{'_CHANGELIST'})\n\n";
        }
        else
        {
            print "..FAILED!\n\n";
        }
    }

    $ENV{'CHANGELIST'} = $lcenv->{'_CHANGELIST'};
    return $lcenv->{'_CHANGELIST'};
}

##############################################################################
# LCECreateLCOwn() - Creates the Own directory, if non-existent.
##############################################################################

sub LCECreateLCOwn
{
    my ($lcenv, $globenv) = @_;
    my @dirs;

    unless (ref($lcenv))   { _throw_error("LCECreateLCOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateLCOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if (-d $lcenv->{'OWN'})
    {
        return 1;
    }

    print "Creating directories..";

    push @dirs, $lcenv->{'OWN'};
    push @dirs, "$lcenv->{'OWN'}/sys/desc/fast", "$lcenv->{'OWN'}/sys/desc/quick", "$lcenv->{'OWN'}/sys/desc/slow";
    push @dirs, "$lcenv->{'OWN'}/sys/src";
    push @dirs, "$lcenv->{'OWN'}/test/kern/sut", "$lcenv->{'OWN'}/test/kern/mut";
    push @dirs, "$lcenv->{'OWN'}/test/port", "$lcenv->{'OWN'}/test/support", "$lcenv->{'OWN'}/test/pc";
    push @dirs, "$lcenv->{'OWN'}/tmp";
    push @dirs, "$lcenv->{'OWN'}/usr";

    mkpath(\@dirs, 0, 0777);

    if (-d $lcenv->{'OWN'})
    {
        print "..OK\n\n";
    }
    else
    {
        print "..FAILED!\n\n";
        return 0;
    }

    return 1;
}

##############################################################################
# LCECreateX64Env() - Creates / changes a 64bit development environment.
##############################################################################

sub LCECreateX64Env
{
    my ($lcenv, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCECreateLCOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateLCOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if ($globenv->{'X64'})
    {
        unless (-f "$lcenv->{'OWN'}/X64.dat")
        {
            open BIT_64, "> $lcenv->{'OWN'}/X64.dat" or return 0;
            close BIT_64 or return 0;
        }
        return 1;
    }
    else
    {
        if (-f "$lcenv->{'OWN'}/X64.dat")
        {
            unlink "$lcenv->{'OWN'}/X64.dat";

            if (-f "$lcenv->{'OWN'}/X64.dat")
            {
                return 0;
            }
        }
        return 1;
    }
}

##############################################################################
# LCECreateP4User() - Creates / changes the P4 user, if needed.
##############################################################################

sub LCECreateP4User
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCECreateP4User", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateP4User", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    if (!$globenv->{'INIT'} && P4UserExists($p4env->{'PORT'}, $p4env->{'USER'}))
    {
        return 1;
    }

    if (P4UserExists($p4env->{'PORT'}, $p4env->{'USER'}))
    {
        print "Perforce user '$p4env->{'USER'}' already exists.\n\n" if ($globenv->{'VERBOSE'});
        $p4env->{'_USER_CREATED'} = 0;
    }
    else
    {
        print "Creating Perforce user '$p4env->{'USER'}'..";
        unless (P4SetUser($p4env->{'PORT'}, $p4env->{'USER'}, "$p4env->{'USER'} (automatically created by LCEnv)"))
        {
            print "..FAILED!\n\n";
            return 0;
        }
        print "..OK\n\n";
        $p4env->{'_USER_CREATED'} = 1;
    }

    return 1;
}

##############################################################################
# LCECreateP4Client() - Creates / changes the P4 client, if needed.
##############################################################################

sub LCECreateP4Client
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCECreateP4Client", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateP4Client", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    if (P4ClientExists($p4env->{'PORT'}, $p4env->{'CLIENT'}))
    {
        print "Updating Perforce client '$p4env->{'CLIENT'}'..";

        my $c_info    = P4GetClientInfo($p4env->{'PORT'}, $p4env->{'CLIENT'});
        my @views     = grep {!/^\Q$p4env->{'SERVER_PATH'}\E/} @{$c_info->{'View'}};
        @views        = grep {!/.*\Q$p4env->{'CLIENT_PATH'}\E\/\.\.\.$/} @views;
        my @old_views = grep {/.*\Q$p4env->{'CLIENT_PATH'}\E\/\.\.\.$/} @{$c_info->{'View'}};
        my $view_path = "$p4env->{'SERVER_PATH'}/... $p4env->{'CLIENT_PATH'}/...";

        if (@old_views)
        {
	        if (($old_views[0] eq $view_path) && ($c_info->{'Root'} eq $p4env->{'ROOT'}))
	        {
	            print "..All settings up-to-date.\n\n";
	            return 1;
	        }

            if ($old_views[0] eq $view_path)
            {
                $p4env->{'_VIEW_REMOVED'}  = "";
                $p4env->{'_VIEW_ADDED'}    = "";
            }
            else
            {
                $p4env->{'_VIEW_REMOVED'}  = $old_views[0];
                $p4env->{'_VIEW_ADDED'}    = $view_path;
            }
        }
        else
        {
            $p4env->{'_VIEW_ADDED'} = $view_path;
        }

        if ($c_info->{'Root'} ne $p4env->{'ROOT'})
        {
            $p4env->{'_ROOT_REMOVED'} = $c_info->{'Root'};
            $p4env->{'_ROOT_ADDED'}   = $p4env->{'ROOT'};
        }
        else
        {
            $p4env->{'_ROOT_REMOVED'} = "";
            $p4env->{'_ROOT_ADDED'}   = "";
        }

        unshift @views, $view_path;

        unless (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $p4env->{'ROOT'}, undef, undef, undef, undef, \@views))
        {
            print "..FAILED!\n\n";
            return 0;
        }

        $p4env->{'_CLIENT_CREATED'} = 0;
    }
    else
    {
        print "Creating Perforce client '$p4env->{'CLIENT'}'..";
        my @descs = ("Automatically created by LCEnv");
        my @views = ("$p4env->{'SERVER_PATH'}/... $p4env->{'CLIENT_PATH'}/...");
        unless (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $p4env->{'ROOT'}, $p4env->{'USER'}, undef, undef, \@descs, \@views))
        {
            print "..FAILED!\n\n";
            return 0;
        }

        $p4env->{'_CLIENT_CREATED'} = 1;
    }

    print "..OK\n\n";

    return 1;
}

##############################################################################
# LCEP4SyncEnv() - Syncs the environment from the Perforce server.
##############################################################################

sub LCEP4SyncEnv
{
    my ($lcenv, $p4env, $globenv, $actenv) = @_;
    my $old_chlist;

    unless (ref($lcenv))   { _throw_error("LCEP4SyncEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCEP4SyncEnv", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCEP4SyncEnv", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCEP4SyncEnv", "Error calling function! Fourth parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless ($p4env->{'USE'}) {
        print "WARNING! Sync is disabled. To enable Perforce support, please use -p4=1.\n\n" if ($p4env->{'SYNC'});
        return 1;
    }
    return 1 if (!$p4env->{'SYNC'} && !$globenv->{'IPROFILE'});

    if (open(F_IN, "< $lcenv->{'OWN'}/changelist.log"))
    {
        $old_chlist = <F_IN>;
    }
    close (F_IN);


    unless ($p4env->{'SYNC'})
    {
        print "Doing minimal sync to be able to create profile...\n\n";
        my @flist = ("sys/desc/config.mac", "sys/src/config/...", "sys/desc/sp.com", "sys/desc/eo.com", "sys/desc/en.com", "sys/desc/os.com",  "sys/src/sp/gsp100.h");
        P4Sync($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'CLIENT'}, $p4env->{'SERVER_PATH'}, $lcenv->{'_CHANGELIST'}, 0, 0, @flist);
        return 1;
    }

    print "Sync files " . ($p4env->{'RESYNC'} ? "(complete re-sync).." : "(incremental)..");
    print ".\n" if ($globenv->{'VERBOSE'});

    if ($old_chlist == $lcenv->{'_CHANGELIST'})
    {
        print ($globenv->{'VERBOSE'} ? "All file up-to-date.\n\n" : "..All files up-to-date.\n\n");

        if ($globenv->{'CHECK_CL'} && !$globenv->{'SCRATCH'})
        {
            if (($globenv->{'CHECK_CL'} == 1) or (P4GetChangeListAge($p4env->{'PORT'}, $p4env->{'USER'}, $lcenv->{'_CHANGELIST'}) > $globenv->{'CHECK_CL'}))
            {
	            $actenv->{'MAKE'} = 0;
	            print "Disabled make since there's nothing to do.\n\n";
	            $WatchDetail .= "\nSYNC: All files were up-to-date! Make disabled!\n";
            }
            else
            {
                print "INFO: Make still active since CL is younger than $globenv->{'CHECK_CL'} minutes.\n\n";
            }
        }
    }
    else
    {
        unless (P4Sync($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'CLIENT'}, $p4env->{'SERVER_PATH'}, $lcenv->{'_CHANGELIST'}, $p4env->{'RESYNC'}, $globenv->{'VERBOSE'}))
        {
            print ($globenv->{'VERBOSE'} ? "Sync finished with warnings.\n\n" : "..finished with warnings.\n\n");
        }
        else
        {
            print ($globenv->{'VERBOSE'} ? "Sync OK.\n\n" : "..OK\n\n");
        }

        if (open (F_OUT, "> $lcenv->{'OWN'}/changelist.log"))
        {
            print F_OUT "$lcenv->{'_CHANGELIST'}";
        }
        close (F_OUT);
        $WatchDetail .= "\nSYNC: Finished. There were new files. ($old_chlist <> $lcenv->{'_CHANGELIST'})\n";
    
        if ($p4env->{'SYNCINFO'})
        {
            my $syncinfo = "SYNCINFO:\n\n" . P4GetChanges($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'SERVER_PATH'}, $lcenv->{'_CHANGELIST'}, $old_chlist, $p4env->{'SYNCINFO'}) . "\n";
            print "$syncinfo";
            $WatchDetail .= "\n$syncinfo";
        }
    }
    
    return 1;
}

##############################################################################
# LCECreateWinIcon() - Creates a windows shortcut icon.
##############################################################################

sub LCECreateWinIcon
{
    my ($lcenv, $p4env, $globenv, $actenv) = @_;
    my ($scdir, $scname, $scfullpath);
    my %ShortcutDB;

    unless (ref($lcenv))   { _throw_error("LCECreateWinIcon", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECreateWinIcon", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateWinIcon", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCECreateWinIcon", "Error calling function! Fourth parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($actenv->{'CREATE_ICON'});

    unless ($^O =~ /MSWin32/i)
    {
        print  "\nWARNING! Cannot create Windows shortcut on an UNIX system!\n";
        return 1;
    }

    print "Creating Windows shortcut..";

    unless ($ENV{'TOOLSHELL'})
    {
        print "..ERROR: Missing \%TOOLSHELL\%! Please restart LCEnv\nfrom a DevTool or another SAPDevelop window. To create a new icon, use:\n $0 -cfg $globenv->{'CFG_FILE'} -wi\n";
        return 1;
    }

    if ($globenv->{'ICON_PATH'})
    {
        $scdir = $globenv->{'ICON_PATH'};
    }
    else
    {
        if ($ENV{'DEVSTUDIO_SHORTCUTS'})
        {
            $scdir = $ENV{'DEVSTUDIO_SHORTCUTS'};
        }
        else
        {
            require Win32::TieRegistry;

            my $common_folder = Win32::TieRegistry->new("LMachine")->TiedRef()->{'Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\\\\Common Start Menu'};

            $scdir = "$common_folder\\SAPDevelop $ENV{'USERNAME'}";
        }
    }

    unless (-d $scdir)
    {
        unless(mkpath($scdir, 0, 0777))
        {
            print "..FAILED! (Can't create shortcut dir)\n\n";
            return 0;
        }
    }

    $scname     = ($actenv->{'ICON_NAME'} ? $actenv->{'ICON_NAME'} : "Developer Studio $lcenv->{'VERSION'}" . ($globenv->{'NAME_SUFF'} ? " (" . uc($globenv->{'NAME_SUFF'}) . ")" : ""));
    $scfullpath = $scdir . "\\" . $scname . ".lnk";

    require Win32::Shortcut;
    
    my $Shortcut = undef;
    if ($globenv->{'ICON_TEMPLATE'} && -e "$ENV{'TOOL'}/config/LCEnvIcon.lnk") {
        $Shortcut = new Win32::Shortcut("$ENV{'TOOL'}/config/LCEnvIcon.lnk");
    } else {
        $globenv->{'ICON_TEMPLATE'} = 0;
        $Shortcut = new Win32::Shortcut();
    }

    if (defined($Shortcut))
    {
        $Shortcut->Path("$ENV{'TOOLSHELL'}");
        $Shortcut->Arguments("$0 -cfg $globenv->{'CFG_FILE'} -shell");
        $Shortcut->WorkingDirectory($lcenv->{'OWN'});
        $Shortcut->ShowCmd(1);
        $Shortcut->Description("Startup of the Developer Studio $lcenv->{'VERSION'}");
        $Shortcut->IconLocation($globenv->{'LOGO_NAME'}) unless ($globenv->{'ICON_TEMPLATE'});
        $Shortcut->IconNumber($globenv->{'LOGO_OFFSET'}) unless ($globenv->{'ICON_TEMPLATE'});
        $Shortcut->Save($scfullpath);
        $Shortcut->Resolve();
        $Shortcut->Close();
    }
    else
    {
        print "..FAILED! (Error fetching resource)\n\n";
        return 0;
    }

    if (-f $scfullpath)
    {
        if ( dbmopen(%ShortcutDB, $DBFiles{'SHORTCUTS'}, 0777) )
        {
            $ShortcutDB{$scfullpath} = "$lcenv->{'OWN'}";
            dbmclose(%ShortcutDB);
        }

        print "..OK\n\n";
        $actenv->{'_ICON_CREATED'} = $scfullpath;
        return 1;
    }
    else
    {
        print "..FAILED! (Shortcut creation not successful)\n\n";
        return 0;
    }
}

##############################################################################
# LCECreateWinShare() - Creates a Windows share for the enironment.
##############################################################################

sub LCECreateWinShare
{
    my ($lcenv, $globenv, $p4env, $actenv) = @_;
    my ($sharename, $shareinfo);

    unless (ref($lcenv))   { _throw_error("LCECreateWinShare", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECreateWinShare", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateWinShare", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCECreateWinShare", "Error calling function! Fourth parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless (($actenv->{'CREATE_SHARE'}) || ($globenv->{'USE_DEFAULT'}));

    unless ($^O =~ /MSWin32/i)
    {
        print  "\nWARNING! Cannot create Windows share on an UNIX system!\n";
        return 1;
    }

    print "Creating Windows share..";

    require Win32::NetResource;

    my ($relver, $corr, $build) = _SplitLCVersion($lcenv->{'VERSION'});

    if ($build =~ /dev/i)
    {
        $sharename = "V$relver";
        if ($p4env->{'USE'})
        {
            if ($p4env->{'SERVER_PATH'} =~ /.*c_$corr.*/i)
            {
                $sharename .= $corr;
            }
        }
    }
#    if ($lcenv->{'VERSION'} =~ /^(\d\d)\.(\d\d)\.dev/i)
#    {
#        $sharename = "V$1";
#        my $corr = $2;
#        if ($p4env->{'USE'})
#        {
#            if ($p4env->{'SERVER_PATH'} =~ /.*c_$corr.*/i)
#            {
#                $sharename .= $corr;
#            }
#        }
#    }
    else
    {
        $sharename = $lcenv->{'VERSION'};
        $sharename =~ s/\.//g;
        $sharename = "V$sharename";
    }

    if ($globenv->{'NAME_SUFF'}) {
        $sharename .= "_" . lc($globenv->{'NAME_SUFF'});
    }
    
    $sharename = $actenv->{'SHARE_NAME'} if ($actenv->{'SHARE_NAME'});

    if (Win32::NetResource::NetShareGetInfo($sharename, $shareinfo ))
    {
        unless (Win32::NetResource::NetShareDel($sharename))
        {
            my $err = undef;
            Win32::NetResource::GetError($err);
            print "..FAILED! ($err)\n\n";
            return 0;
        }
    }

    $shareinfo = {
        'path'          => $lcenv->{'OWN'},
        'netname'       => $sharename,
        'remark'        => "Share for $lcenv->{'VERSION'}, created by LCEnv",
        'passwd'        => "",
        'current-users' => 0,
        'permissions'   => 0,
        'maxusers'      => -1,
        'type'          => 0
    };

    if (Win32::NetResource::NetShareAdd($shareinfo))
    {
        print "..OK\n\n";
        $actenv->{'_SHARE_CREATED'} = $sharename;
    }
    else
    {
        my $err = undef;
        Win32::NetResource::GetError($err);
        print "..FAILED! ($err)\n\n";
        return 0;
    }

    return 1;
}

##############################################################################
# LCECreateIProfile() - Creates a profile for the enviroment.
##############################################################################

sub LCECreateIProfile
{
    my ($lcenv, $p4env, $globenv) = @_;

    my ($cmdstr, $errstr);

    unless (ref($lcenv))   { _throw_error("LCECreateIProfile", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECreateIProfile", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateIProfile", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($globenv->{'IPROFILE'});

    print "Creating iprofile..";

    my ($relver, $corr, $build) = _SplitLCVersion($lcenv->{'VERSION'});

    $ENV{'RELVER'} = "R$relver";
    $ENV{'CORRECTION_LEVEL'} = $corr if ($corr);

#    if ($lcenv->{'VERSION'} =~ /^(\d\d)\.(\d\d)\.(.*)$/)
#    {
#        $ENV{'RELVER'}              = "R$1";
#        $ENV{'CORRECTION_LEVEL'}    = $2;
#        $build                      = $3;
#    }

    $cmdstr = "$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/createlcprofile.pl";
    $cmdstr .= " -own $lcenv->{'OWN'}";
    $cmdstr .= " -tool $globenv->{'TOOL_DIR'}"  if ($globenv->{'TOOL_DIR'});
    $cmdstr .= " -bit64" if ($globenv->{'64'});
#    $cmdstr .= " -prod"  if ($globenv->{'PROD_MAKE'});
    $cmdstr .= " -src $lcenv->{'GRP'}" if ($lcenv->{'GRP'});

    if (!$build) {
        $cmdstr .= " -lc_state DEV";
    }
    elsif ($build =~ /^\d\d$/)
    {
        $cmdstr .= " -lc_state RAMP";
    }
    else
    {
        $cmdstr .= " -lc_state $build";
    }

    $cmdstr .= " >errout 2>&1";

    my $rc = system($cmdstr);

    if ($rc > 0)
    {
        open (ERR_OUT, "< errout");
        while (<ERR_OUT>) { $errstr .= $_ }
        close (ERR_OUT);
        unlink "errout";
        print "..FAILED!\n$errstr\n\n";
        $WatchDetail .= "\niprofile generation FAILED!\n";
    }
    else
    {
        print "..OK\n\n";
        $WatchDetail .= "\niprofile generation OK.\n";
    }

    unless ($^O =~ /MSWin32/i)
    {
        open (CMD_OUT, ">$lcenv->{'OWN'}/.istart");
        if ($p4env->{'USE'})
        {
            print CMD_OUT "export P4PORT=$p4env->{'PORT'}\n";
            print CMD_OUT "export P4USER=$p4env->{'USER'}\n";
            print CMD_OUT "export P4CLIENT=$p4env->{'CLIENT'}\n";
            print CMD_OUT "export P4PASSWD=$p4env->{'PASSWD'}\n" if ($p4env->{'PASSWD'});
        }
        print CMD_OUT "cd $lcenv->{'OWN'}\n";
        print CMD_OUT "perl \$TOOL/bin/LCEnv.pl -cfg $globenv->{'CFG_FILE'}\n";
        print CMD_OUT ". .iprofile\n";
        close CMD_OUT;
    }

    unlink "errout";

    return 1;
}

##############################################################################
# LCECopyIUser() - Copies iuser.bat to $OWN, if needed.
##############################################################################

sub LCECopyIUser
{
    my ($lcenv, $globenv, $actenv) = @_;

    my ($cmdstr, $errstr);

    unless (ref($lcenv))   { _throw_error("LCECreateIProfile", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateIProfile", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))   { _throw_error("LCECreateIProfile", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($actenv->{'COPY_IUSER'});

    print "Copying iuser..";

    unless (-e "$ENV{'TOOL'}/config/iuser.default")
    {
        print "..No default!\n\n";
        return 1;
    }

    my $destname = (($^O =~ /MSWin32/i) ? "iuser.bat" : ".iuser");
    
    if (copy("$ENV{'TOOL'}/config/iuser.default", "$lcenv->{'OWN'}/$destname"))
    {
        print "..OK\n\n";
    }
    else
    {
        print "..FAILED!\n\n";
    }
    
    return 1;
}

##############################################################################
# LCECreateReleaseDBEntry() - Adds an entry to the release DB.
##############################################################################

sub LCECreateRelDBEntry
{
    my ($lcenv, $p4env, $globenv, $actenv) = @_;
    my %ReleaseDB;

    unless (ref($lcenv))   { _throw_error("LCECreateRelDBEntry", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCECreateRelDBEntry", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCECreateRelDBEntry", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCECreateRelDBEntry", "Error calling function! Fourth parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless (($^O =~ /MSWin32/i));

    print "Updating Release DB entries..";

    if (dbmopen (%ReleaseDB, $DBFiles{'RELEASES'}, 0777))
    {
        $ReleaseDB{lc($lcenv->{'OWN'})} = "$lcenv->{'VERSION'}" . ($p4env->{'USE'} ? " ($p4env->{'USER'})" : "") . ($actenv->{'_SHARE_CREATED'} ? " [$actenv->{'_SHARE_CREATED'}]" : "");
        dbmclose (%ReleaseDB);
        print "..OK\n\n";
    }
    else
    {
        print "..FAILED!\n\n";
    }

    return 1;
}

##############################################################################
# LCERemReleaseDBEntry() - Removes an entry from the release DB again.
##############################################################################

sub LCERemRelDBEntry
{
    my ($lcenv, $globenv) = @_;
    my %ReleaseDB;

    unless (ref($lcenv))   { _throw_error("LCERemRelDBEntry", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemRelDBEntry", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($^O =~ /MSWin32/i);

    print "Removing Release DB entry..";

    if (dbmopen (%ReleaseDB, $DBFiles{'RELEASES'}, 0777))
    {
        $ReleaseDB{$lcenv->{'OWN'}} = undef;
        dbmclose (%ReleaseDB);
        print "..OK\n\n";
    }
    else
    {
        print "..FAILED!\n\n";
    }

    return 1;
}

##############################################################################
# LCERemWinShare() - Removes a windows share again.
##############################################################################

sub LCERemWinShare
{
    my ($lcenv, $globenv, $actenv) = @_;
    my $sharename;

    unless (ref($lcenv))   { _throw_error("LCERemWinShare", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemWinShare", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCERemWinShare", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless (($^O =~ /MSWin32/i) && ($actenv->{'_SHARE_CREATED'} ne ""));

    print "Removing Windows share.." if ($globenv->{'VERBOSE'});

    require Win32::NetResource;

    $sharename = _convert_version($lcenv->{'VERSION'});
    $sharename =~ s/\.//g;
    $sharename = "V$sharename";

    if (Win32::NetResource::NetShareDel($sharename))
    {
        print "..OK\n\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        my $err = undef;
        Win32::NetResource::GetError($err);
        print "..FAILED! (Error $err)\n\n" if ($globenv->{'VERBOSE'});
    }

    return 1;
}

##############################################################################
# LCERemWinIcon() - Removes a windows shortcut again.
##############################################################################

sub LCERemWinIcon
{
    my ($lcenv, $globenv, $actenv) = @_;
    my %ShortcutDB;

    unless (ref($lcenv))   { _throw_error("LCERemWinIcon", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemWinIcon", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCERemWinIcon", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless (($^O =~ /MSWin32/i) && ($actenv->{'_ICON_CREATED'}));

    print "Removing Windows shortcut.." if ($globenv->{'VERBOSE'});

    if (-f $actenv->{'_ICON_CREATED'})
    {
        if (dbmopen(%ShortcutDB, $DBFiles{'SHORTCUTS'}, 0777))
        {
            $ShortcutDB{$actenv->{'_ICON_CREATED'}} = undef;
            dbmclose (%ShortcutDB);
        }

        unlink ($actenv->{'_ICON_CREATED'});
        print "..OK\n\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        print "..FAILED! (not found)\n\n" if ($globenv->{'VERBOSE'});
    }

    return 1;
}

##############################################################################
# LCERemP4Files() - Removes all Perforce administrated files again.
##############################################################################

sub LCERemP4Files
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCERemP4Files", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemP4Files", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    # First, remove the Perforce administrated files by using P4Sync:
    print "Removing Perforce administrated files...\n" if ($globenv->{'VERBOSE'});
    if (P4Sync($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'CLIENT'}, $p4env->{'SERVER_PATH'}, 0, 0, $globenv->{'VERBOSE'}))
    {
        print "Perforce files removed.\n\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        print "WARNING! Could not remove all Perforce administrated files!\n\n";
    }

    return 1;
}

##############################################################################
# LCERemP4Client() - Removes the client again, if created flag is set.
##############################################################################

sub LCERemP4Client
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCERemP4Client", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemP4Client", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    # Remove any made perforce client settings (if unchanged):
    if ($p4env->{'_CLIENT_CREATED'})
    {
        print "Removing Perforce client '$p4env->{'CLIENT'}'...\n" if ($globenv->{'VERBOSE'});
        if(P4RemClient($p4env->{'PORT'}, $p4env->{'CLIENT'}))
        {
            print "Perforce client removed.\n\n";
        }
        else
        {
            print "WARNING! There were problems removing Perforce client '$p4env->{'CLIENT'}'!\n\n";
        }
    }
    else
    {
        print "Restoring old Perforce client settings...\n" if ($globenv->{'VERBOSE'});

        my $c_info    = P4GetClientInfo($p4env->{'PORT'}, $p4env->{'CLIENT'});
        my @views;

        if ($p4env->{'_VIEW_ADDED'})
        {
            @views = grep {!/^\Q$p4env->{'_VIEW_ADDED'}\E/} @{$c_info->{'View'}};
        }
        else
        {
            push @views, @{$c_info->{'View'}};
        }

        if ($p4env->{'_VIEW_REMOVED'})
        {
            push @views, $p4env->{'_VIEW_REMOVED'};
        }

        if ($p4env->{'_ROOT_ADDED'})
        {
            if ($c_info->{'Root'} ne $p4env->{'_ROOT_ADDED'})
            {
                print "Warning: Perforce client root has been changed since installation.\nRestoring of root directory cancelled.\n\n";
            }
            else
            {
                $c_info->{'Root'} = $p4env->{'_ROOT_REMOVED'};
            }
        }

        if (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $c_info->{'Root'}, undef, undef, undef, undef, \@views))
        {
            print "Perforce client restored.\n\n";
        }
        else
        {
            print "WARNING! There were problems restoring the Perforce client!\n\n";
        }
    }
    return 1;
}

##############################################################################
# LCERemP4User() - Removes the Perforce user again, if created before.
##############################################################################

sub LCERemP4User
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCERemP4User", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemP4User", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($p4env->{'USE'});

    if ($p4env->{'_USER_CREATED'})
    {
        print "Removing Perforce user '$p4env->{'USER'}'...\n" if ($globenv->{'VERBOSE'});
        if (P4RemUser($p4env->{'PORT'}, $p4env->{'USER'}))
        {
            print "Perforce user removed.\n";
        }
        else
        {
            print "WARNING! Could not remove Perforce user '$p4env->{'USER'}!\n";
        }
    }
    return 1;
}

##############################################################################
# LCERemLCOwn() - Removes the Own directory (softly, no readonly's removed).
##############################################################################

sub LCERemLCOwn
{
    my ($lcenv, $globenv) = @_;

    unless (ref($lcenv))   { _throw_error("LCERemLCOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCERemLCOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "Removing directory tree '$lcenv->{'OWN'}'...\n" if ($globenv->{'VERBOSE'});
    if (rmtree($lcenv->{'OWN'}, $globenv->{'VERBOSE'}, 0))
    {
        print "Directory tree removed.\n";
    }
    else
    {
        print "There were problems while removing directory tree. Please, check manually.\n";
    }
    return 1;
}

##############################################################################
# LCESendMail () - Sends a status mail.
##############################################################################

sub LCESendMail
{
    my $href = shift;
    my $rc   = shift;
    my $WatchHead = "";

    unless (ref($href)) { _throw_error("LCESendMail", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return unless ($href->{'GLOBAL'}->{'WATCHER'} && ($href->{'GLOBAL'}->{'MAILFILTER'} & $href->{'GLOBAL'}->{'__MAKE_RESULT'}));

    LCESetState($href, "sending mail");

    require Mail::Automail;
#    import  Mail::Automail;

    my $host = `hostname`;
    chomp ($host);
    
#    my $Sub  = "$host - Make $href->{'RELEASE'}->{'VERSION'} ";
    my $MState = ($href->{'GLOBAL'}->{'__MAKE_RESULT'} == 1 ? "No Make" : ($href->{'GLOBAL'}->{'__MAKE_RESULT'} == 2 ? "OK (No Warnings)" : ($href->{'GLOBAL'}->{'__MAKE_RESULT'} == 4 ? "OK (Warnings)" : ($href->{'GLOBAL'}->{'__MAKE_RESULT'} == 8 ? "FAILED!" : "TERMINATED (Killed)!"))));   

#    $Sub .= $MState;
       
    $WatchHead .= "STATUS      = $MState\n";
    $WatchHead .= "RETURN CODE = $rc\n";
    $WatchHead .= "HOSTNAME    = $host\n";
    $WatchHead .= "OWN         = $href->{'RELEASE'}->{'OWN'}\n";
    $WatchHead .= "CHANGELIST  = $href->{'RELEASE'}->{'_CHANGELIST'}\n";   

    if ($href->{'ACTIONS'}->{'MAKE'} == 0)
    {
        $WatchDetail .= "\nNo Make.\n";
    }
    elsif (-e "$href->{'RELEASE'}->{'OWN'}/make.id")
    {
        open (MAKE_ID, "<$href->{'RELEASE'}->{'OWN'}/make.id");
        my $makeid = <MAKE_ID>;
        close (MAKE_ID);
        $WatchHead .= "MAKE ID     = $makeid\n";
        if ($href->{'GLOBAL'}->{'HTML_OUT'})
        {
            $WatchHead .= "Protocol    = <A HREF=\"http://pts:1081/TestMonitor/Make_Details.jsp?id=$makeid\">Make Details</A>\n";
            $WatchHead .= "Error Link  = <A HREF=\"$ErrorDetailLink\">Error Details (.e0)</A>\n" if ($ErrorDetailLink);
        }
        else
        {
            $WatchHead .= "Protocol    = http://pts:1081/TestMonitor/Make_Details.jsp?id=$makeid\n";
            $WatchHead .= "Error Link  = $ErrorDetailLink\n" if ($ErrorDetailLink);        }
    }
    
    my %mail = ('To'             => "$href->{'GLOBAL'}->{'WATCHER'}",
                'Category'       => 'Make', 
                'Type'           => 'Monitor',
                'Infotext'       => "$MState");

    if ($href->{'GLOBAL'}->{'HTML_OUT'})
    {                 
        $mail{'Message'} = "<HTML><BODY><PRE>\n$WatchHead\n$WatchDetail\n</PRE></BODY></HTML>\n";
        $mail{'Content-Type'} = 'text/html';
    }
    else
    {
        $mail{'Message'} = "$WatchHead\n$WatchDetail";
    }
                   
    unless (Mail::Automail::Send(%mail))
    {
        print "Sendmail error: $Mail::Sendmail::error\n";
    }
        
#    my %mail;
                 
#    if ($href->{'GLOBAL'}->{'HTML_OUT'})
#    {
#        %mail = ('To'      => "$href->{'GLOBAL'}->{'WATCHER'}",
#				 'From'    => 'monitor@sap.corp',
#				 'Message' => "<HTML><BODY><PRE>\n$WatchHead\n$WatchDetail\n</PRE></BODY></HTML>\n",
#				 'Cc'      => "",
#				 'Smtp'    => 'mail.sap.corp',
#				 'Content-Type' => 'text/html',
#				 'Subject' => "$Sub" );
#    }
#    else
#    {
#        %mail = ('To'      => "$href->{'GLOBAL'}->{'WATCHER'}",
#				 'From'    => 'monitor@sap.corp',
#				 'Message' => "$WatchHead\n$WatchDetail",
#				 'Cc'      => "",
#				 'Smtp'    => 'mail.sap.corp',
#				 'Subject' => "$Sub" );    
#    }
    
#    unless (sendmail(%mail))
#    {
#        print "Sendmail Error: " . $Mail::Sendmail::error . "\n";
#    }
    
    LCESetState($href, "mail sent");
}

##############################################################################
# LCESetEnv() - Sets additional environment
##############################################################################

sub LCESetEnv
{
    my ($henv) = @_;

    if ($henv)
    {
	    foreach my $key (%{$henv})
	    {
	        $ENV{$key} = $henv->{$key};
	    }
    }
    return 1;
}

##############################################################################
# _GetP4LCServerPath() - Retrieves the correct server path for a LC version.
##############################################################################

sub _GetP4ServerPath
{
    my ($p4port, $p4user, $lc_ver, $lc_cor, $lc_build) = @_;

    my $dirstr = "//sapdb/V$lc_ver/c_$lc_cor/";
    
    $lc_build = lc ($lc_build);

    if ($lc_build =~ /^\d\d$/)
    {
        $dirstr .= "b_";
    }
    $dirstr .= "$lc_build";

    my $dir = P4GetDir($p4port, $p4user, $dirstr);

    if ($dir->{'DIRS'})
    {
        return $dirstr;
    }
    elsif ($lc_build =~ /^ramp$/)
    {
        if ($dir->{'FILES'})
        {
            if (open(BUILD_NR, "p4 -p $p4port -u $p4user print -q $dirstr/buildnumber.cfg |"))
            {
                if (<BUILD_NR> =~ /^build=(b_\d\d)$/)
                {
                    $lc_build = $1;
                    $dirstr =~ s/ramp/$lc_build/;
                    close (BUILD_NR);
                    return $dirstr;
                }
                close (BUILD_NR);
                return "";
            }
        }
    }
    elsif (($lc_build !~ /^dev.*$/) && $lc_build)
    {
        return "";
    }

    $lc_build = "develop";

    undef $dir;

    $dir = P4GetDir($p4port, $p4user, "//sapdb/V$lc_ver");

    if ($dir->{'DIRS'})
    {
        my @cors = grep { /^c_\d\d$/ } reverse sort @{$dir->{'DIRS'}};

        if (@cors)
        {
            my $tmp_cor = ($lc_cor - 1);
            if (($cors[0] =~ /^c_$lc_cor$/) || ($cors[0] =~ /^c_0?$tmp_cor$/))
            {
                return "//sapdb/V$lc_ver/develop";
            }
        }
        elsif ($dir->{'DIRS'}->[0] =~ /^develop$/)
        {
            return "//sapdb/V$lc_ver/develop";
        }
    }

    return "";
}

##############################################################################
# _SelectP4Dir() - Selects a directory in perforce.
##############################################################################

sub _SelectP4Dir
{
    my ($p4port, $p4user, $p4path, $match_pattern, $msg, $add_dev) = @_;
    my @ergs;
    my $count       = 0;
    my $sel         = 0;
    my $dev_found   = 0;
    my $show_num    = 1;
    my %erg_map;

    my $href = P4GetDir($p4port, $p4user, $p4path);

    foreach (@{$href->{'DIRS'}}) { push @ergs, ($1 ? $1 : $2) if(/^$match_pattern$/) }

    if (@ergs == 0) { return ($add_dev ? "develop" : "") }
    if ((@ergs == 1) && (!$add_dev || ($ergs[0] eq "develop")))
    {
        print "\nThe $msg has automatically been set to '$ergs[0]'.\n";
        return $ergs[0];
    }

    print "\nThe " . $msg . " is not set. Please select one:\n\n";

    foreach (@ergs)
    {
        if (($show_num + 1) == $ergs[$count]) { $show_num ++; }
        $erg_map{$show_num} = "$ergs[$count]";
        print "[$show_num] = $erg_map{$show_num}\n";
        $dev_found = 1 if (/^develop$/);
        $count ++;
        $show_num ++;
    }

    if ($add_dev && !$dev_found)
    {
        $erg_map{$show_num} = "develop";
        if ($add_dev == 1) { print "[$show_num] = develop\n"; }
        else { print "[$show_num] = always latest branch (develop)\n"; }
    } else { $show_num --; }

    my $ok = 0;
    while (!$ok)
    {
        $sel = RequestInput("\nPlease select: ", "", "[0-9]+", "Input incorrect. Please, just use numbers.\n");
        if ((($sel < 1) or ($sel > $show_num)) or ($erg_map{$sel} eq "")) {
            print "Input error! Please, just use numbers between 1 and $show_num" . (($erg_map{$sel} eq "") ? " and NOT $sel" : "") . ".\n";
        } else { $ok = 1; }
    }

    return $erg_map{$sel};
}

##############################################################################
# _convert_version() - Converts a version string from new into old format.
##############################################################################

sub _convert_version
{
    my $version = shift;

    my ($profilehost, $oldver, %ProfileDB);
    my ($may, $min, $cor, $bui);

    if ($ENV{'SAPDB_MAKE_LOCATION'} eq "WDF")
    {
        $profilehost = "\\\\PWDFM017\\ToolSrc\\profiles";
    }
    elsif ($ENV{'SAPDB_MAKE_LOCATION'} eq "KLN")
    {
        $profilehost = "\\\\P47368\\ToolSrc\\profiles";
    }
    else
    {
        $profilehost = "\\\\p30883\\ToolSrc\\profiles";
    }

    $may = $1 if ($version =~ /^(\d).*/);
    $min = $1 if ($version =~ /^\d(\d).*/);
    $cor = $1 if ($version =~ /^\d\d\.(\d\d).*/);
    $bui = $1 if ($version =~ /^\d\d\.\d\d\.(\d\d)/);

    $oldver = $version;

    if (dbmopen(%ProfileDB, $profilehost, 0))
    {
        if ($ProfileDB{"$may.$min.$cor.$bui"})
        {
            $oldver = "$may.$min.$cor.$bui";
        }
        elsif ($ProfileDB{"$may.$min.$cor"})
        {
            $oldver = "$may.$min.$cor";
        }
        elsif ($ProfileDB{"$may.$min"})
        {
            $oldver = "$may.$min";
        }
        else
        {
            $oldver = $version;
        }

        dbmclose(%ProfileDB);
    }

    return $oldver;
}

##############################################################################
# _SplitLCVersion()
##############################################################################

sub _SplitLCVersion
{
    my ($version) = @_;

    return ("DEV","","") if ($version =~ /^DEV$/i);
    my ($rel, $corr, $build) = ("", "", "");

    if ($version =~ /^(\d)\.?(\d)/)
    {
        $rel = "$1$2";
    }

    if ($version =~ /^\d\.?\d\.?(\d\d)/)
    {
        $corr = "$1";
    }

    if ($version =~ /^\d\.?\d\.?\d?\d?\.?(.*)$/)
    {
        $build = "$1";
    }

    return ($rel, $corr, $build);
}

##############################################################################
# _JoinLCVersion()
##############################################################################

sub _JoinLCVersion
{
    my ($relver, $corr, $build) = @_;
    my $version = "";

    $version = $relver;
    $version .= ".$corr" if ($corr);
    $version .= ".$build" if ($build);

    $version = lc ($version);

    return $version;
}

##############################################################################
# LCESetState() - Sets a run state.
##############################################################################
sub LCESetState
{
    my ($href, $state) = @_;
    unless (ref($href)) { _throw_error("LCESetState", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    return 1 unless ($href->{'GLOBAL'}->{'LOCKFILE'});
    
    if ($state ne "")
    {
	    if (open(STATE_OUT, "> $href->{'GLOBAL'}->{'LOCKFILE'}.state"))
	    {
	        print STATE_OUT "$state\n";
	        close (STATE_OUT);
	        return 1;
	    }
	    print "Error! Can't open '$href->{'GLOBAL'}->{'LOCKFILE'}.state' for writing!\n";
	    return 0;
    }
    else
    {
        unlink ("$href->{'GLOBAL'}->{'LOCKFILE'}.state");
        return 1;
    }
}

##############################################################################
# LCEGetState() - Retrieves a run state.
##############################################################################
sub LCEGetState
{
    my ($href) = @_;
    unless (ref($href)) { _throw_error("LCEGetState", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    my $state = "";
    return "" unless ($href->{'GLOBAL'}->{'LOCKFILE'});
    
    if (open (STATE_IN, "< $href->{'GLOBAL'}->{'LOCKFILE'}.state"))
    {
        while (<STATE_IN>)
        {
            $state = $1 if (/^(.+)$/);
        }
    }
    
    close (STATE_IN);
    
    return $state;
}

##############################################################################
# _throw_error() - Throws an heavy error and dies.
##############################################################################
sub _throw_error
{
    my ($func, $err_text) = @_;
    print STDERR "\nERROR in module 'LCEnv.pm' in function '$func'!\n\nDetailed error text:\n\n$err_text\n\n";
    exit(10);
}


__END__

=head1 NAME

LCEnv.pm - Perl module to handle LC development environments.

=head1 NOTE

This tool is for internal use, only.
In order to work correctly, this tool needs a pre-installed tool
environment. Since it also copies data from internal servers, it is very
likely not working outside SAP.

=head1 ABSTRACT

There are different ways to use this module. You can either call the
"all-in-one" function "MakeLCEnv" or use different functions to check/set
just a single aspect of the environment.

=head1 SYNOPSIS OF CREATING A COMPLETE LC ENVIRONMENT

=head1 SYNOPSIS OF USING OTHER METHODS

...

=head1 DESCRIPTION

=head1 METHODS

=head1 PARAMETER STRUCTURES

All parameters used in functions in this module are typically hash references.
Each hash reference is a logical entity (one for global settings, one for
Perforce settings etc.) which makes it sometimes neccessary to ask for more
than one parameter. In the following, all used references will be explained
in detail.
NOTE: If you take a look into the LCEnv configuration file stored in the
created Own directory (LCEnv.cfg), you'll probably find some values beginning
with an underscore ('_'). These values are to be used internally only and
are therefore *NOT* explained here.

=head2 $GLOBENV (hash reference)

This hash reference holds the globally used settings which are not part of
one of the other hashes.

=over 4

=item BATCH_MODE (0 or 1)

This describes whether a program is running in batch mode or not. In batch
mode any interactive input is forbidden - if the program cannot go on without
user interaction, it will throw an error and terminate itself. NOTE: This
option overrides the USE_DEFAULT option to avoid ambigouities.


=item USE_DEFAULT (0 or 1)

If this setting is set to 1, alls functions will try to use defaults instead of
bothering the user. NOTE: This could lead to dangerous results! Eg. using this
together with 'LCECheckP4Env()' would most likely set your default Perforce
user/client. If you've then given a different Perforce root, it would overwrite
your default settings!

=item VERBOSE (0 or 1)

With this option turned on, functions will provide a more detailed output of
all actions.

=item ASSUME_YES (0 or 1)

This setting causes the program not to ask any 'yes/no' questions, but
answers themselves automatically with 'yes'. Using this setting in
conjuction with 'USE_DEFAULT' provides you with a somehow "BATCH_MODE with
using default settings" mode. NOTE: Using 'BATCH_MODE' includes this
option.

=back

=head2 $P4ENV (hash reference)

=over 4

This hash reference holds all values being used for Perforce.

=item PORT (string)

The Perforce port to be used. This should normally be
'perforce3006.wdf.sap.corp:3006' for liveCache releases.

=item USER (string)

The Perforce user you want to use. Normally, this is your D-User. Please
note that any user you use has to have access rights for the directories
you want to sync. These rights are not set implicitly but have to be
granted by a Perforce administrator.

=item CLIENT (string)

The Perforce Client to be used. Normally, this is the host name you're
working on, maybe in conjunction with a suffix. Any user may create his
own clients without any access restrictions.

=item ROOT (string)

This settings is part of the Perforce client and describes the root
directory to be used. Normally, it is something like 'c:\SAPDevelop'.

=item SERVER_PATH (string)

This setting contains the path to the sources on the Perforce server.
This should be a string beginning with "//sapdb/"...
NOTE: This string is normally set internally and is not needed to be
changed from outside. Its documentation is for completeness, only.

=item CLIENT_PATH (string)

This string containg the Perforce path for the client machine. This
should be something like "//PXXXXX/"... where PXXXXX is your client.
NOTE: This string is normally set internally and is not needed to be
changed from outside. Its documentation is for completeness, only.

=back

=head2 $LCENV (hash reference)

=over 4

This hash reference holds all values being used for LC release itself.

=item VERSION (string)

This string holds the LC version in the well-known format, ie. "7403DEV".
The complete format description is: RR[.]CC[.](DEV|COR|BB)
Where "RR" is the Release, "CC" is the correction level and "BB" is the
build prefix.

=item OWN (string)

This is the OWN directory you want to use. Eg. for the latest 7404
development branch this would be "C:\SAPDevelop\V74\develop", by dafault.
NOTE: This directory has to be a sub-directory of the Perforce root
directory. Otherwise, syncing any sources would fail.

=item CHANGELIST (number)

With this option you can set a special change list you want to sync to.
If you don't use this option, the change list will be set to 0. This means
that the latest changelist number will be used.

=back

=head1 EXAMPLE

=head1 COPYRIGHT

Copyright 2003 SAP AG