#!/usr/bin/perl
#
#    LCEnv.pl - Creates and syncs a complete SAPDB make environment.
#
#    @(#)LCEnv     2003-04-22
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
		if ($^O=~/.*win32.*/i)
		{
		    $ENV{'PATH'} = "$ENV{'TOOL'};$ENV{'PATH'}";
		}
		else
		{
		    $ENV{'PATH'} = "$ENV{'TOOL'}:$ENV{'PATH'}";
		}
		print "NOTE: TOOL has been set to $ENV{'TOOL'}\n";
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

use Getopt::Long;
require IO::SimpleCfg;
import  IO::SimpleCfg;
require IO::FilePath;
import  IO::FilePath;
require IO::CurrentDir;
import  IO::CurrentDir;
require Tool::HashRef;
import  Tool::HashRef;
require Tool::RequestInput;
import  Tool::RequestInput;
require Env::LCEnv;
import  Env::LCEnv;
require Env::LCEnvAdm;
import  Env::LCEnvAdm;

##############################################################################
# Console Preparation (redirect STDERR to STDOUT and disable buffering):

$| = 1;

##############################################################################
# Global variables:

my $ProgramName     = "LCEnv";
my $Version         = "1";
my $Revision        = "73";
my $Date            = "2005-10-21";

my %Opts            = ();
my $Glob            = {};
my $PathSep         = ($^O =~ /MSWin32/i) ? "\\" : "/";

$Glob->{'CFG'}      = {};
$Glob->{'DEF_CFG'}  = {};

##############################################################################
# Main routine:
##############################################################################

print "\n$ProgramName v$Version.$Revision ($Date), (c)2004 SAP AG\n\n";

unless ($ENV{'TOOL'})
{
    print "%TOOL% is not set in the environment!\n";
    CleanExit (1);
}

# Check command line parameters for correctness:

if (!GetOptions( \%Opts,
                'h|help|?',
                'v|verbose',
                'y|yes',
                'b|batch',
                'ccl|check_cl=i',
                'd|default',
                'resync=i',
                'lockfile=s',
                'lockforce',
                'lockwait',
                'lockignore',
                'own=s',
                'rel=s',
                'cl=i',
                'p4u=s',
                'p4c=s',
                'p4r=s',
                'p4pw=s',
                'prod=i',
                'prod_cfg=s',
                'prod_email=s',
				'relstat=s',
                'watcher=s',
                'm=s',
                'mc|machine_cfg=s',
                'cfg=s',
                'scratch=i',
                'c|clear',
                'wi|win_icon=i',
                'ws|win_share=i',
                'win|win_icon_name=s',
                'wsn|win_share_name=s',
                'tool=s',
                'X64',
                '64=i',
                'sync=i',
                'syncinfo=i',
                'l|list',
                'shell',
                'rm|remove',
                'p4=i',
                'save=i',
                'grp=s',
                'cmd=s',
                'low=i',
                'color=s',
                'iuser=i',
                'so|save_only',
                'man|manual',
                'iprofile=i', 
                'sec|secure_cfg=i',
                'wln|win_logo_name=s',
                'wlo|win_logo_offset=i',
                'ed|edit_defaults',
                'mailfilter=i',
                'xout=i',
                'xprot=s',
                'make=i',
                'html=i',
                'syscall=i',
                'en|env_name=s@',
                'ev|env_val=s@') || $Opts{'h'} || $Opts{'man'} || @ARGV)
{
    Usage();
}
    
# Now, try loading the system defaults.
my $sel = "";

if (-e "$ENV{'TOOL'}/config/LCEnv.default")
{
    $Glob->{'DEF_CFG'} = LoadSimpleCfg("$ENV{'TOOL'}/config/LCEnv.default");
}
elsif (-e "$ENV{'TOOL'}/data/LCEnv.default")
{
    $Glob->{'DEF_CFG'} = LoadSimpleCfg("$ENV{'TOOL'}/data/LCEnv.default");
}
else
{
    if ($Opts{'shell'} or $Opts{'cfg'} or $Opts{'b'})
    {
        $sel = "n";
    }
    else
    {
        $sel = RequestInput("Could not find any system defaults. Do you want to set them up now ? (Y/n) ", "y", "[yYnNjJ]", "Input incorrect! Please, just answer 'y' or 'n'.\n");
    }
}

if (($sel =~ /^[yj]$/i) or $Opts{'ed'})
{
    LCESetDefaults($Glob->{'DEF_CFG'});
    SaveSimpleCfg("$ENV{'TOOL'}/config/LCEnv.default", $Glob->{'DEF_CFG'});
    print "\nGlobal Configuration saved under '$ENV{'TOOL'}" . $PathSep . "config" . $PathSep . "LCEnv.default'.\n";
    print "\nExiting...\n";
    CleanExit(0);
}

# Then, load the configuration file:
if ($Opts{'cfg'})
{

    if ($Opts{'cfg'} =~ /[\\\/]/)
    {
        $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $Opts{'cfg'};
    }
    elsif (-f $Opts{'cfg'})
    {
        $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = GetCurrentDir() . $PathSep . $Opts{'cfg'};
    }
    elsif ($Glob->{'DEF_CFG'}->{'GLOBAL'}->{'CFG_PATH'})
    {
        $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $Glob->{'DEF_CFG'}->{'GLOBAL'}->{'CFG_PATH'} . $PathSep . $Opts{'cfg'};
    }
    else
    {
        $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $Opts{'cfg'};
    }

    if ($Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'})
    {
        unless ($Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} =~ /\.cfg$/)
        {
            $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} .= ".cfg";
        }

        if (-f $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'})
        {

	        my $cfg_file = $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'};
	
	        $Glob->{'CFG'} = LoadSimpleCfg($cfg_file);
	        $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $cfg_file;
	
	        print "Configration '$Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}' loaded.\n\n" if ($Opts{'v'});
        }
        else
        {
            print "Warning: Configuration '$Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}' not found.\n\n";
            $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} = 1;
        }
    }
} else { $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} = 1; }

# Then, overload settings by given command line parameters:
ParseCmdLineParams();

my $old_state = LCEGetState($Glob->{'CFG'});

# Check, if this process has to have low priority:
if ($Glob->{'CFG'}->{'GLOBAL'}->{'LOW'})
{
    if ($^O =~ /MSWin32/i)
    {
        system("$ENV{'TOOL'}\\pgm\\lowprio.exe $$ >nul");
        $ENV{'LOW_PRIO'} = 1;
    }
    else
    {
        print "Warning: Setting '-low' ignored under UNIX.\n\n";
    }
}

if ($Opts{'rm'})
{
    #=========================================================================
    # REMOVE INSTANCE
    #=========================================================================

    unless ($Opts{'cfg'} && -e $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'})
    {
        print "Sorry, you can only remove EXISTING environments. Please, use '-cfg'\nand make sure that the given configuration exists!\n";
        CleanExit(1);
    }

    print "Removing environment for '$Glob->{'CFG'}->{'RELEASE'}->{'VERSION'}'. Please be patient...\n\n";

    LCERemEnv($Glob->{'CFG'});
}
elsif ($Opts{'l'})
{
    #=========================================================================
    # LIST ALL INSTANCES
    #=========================================================================

    LCEListRels();
}
else
{
    #=========================================================================
    # INSTALL INSTANCE
    #=========================================================================

    #-------------------------------------------------------------------------
    # First, check configuration integrity and ask for missing parameters:
    #-------------------------------------------------------------------------
    LCECheckEnv($Glob->{'CFG'});

    if ((!$Glob->{'CFG'}->{'PERFORCE'}->{'USE'}) && !(-d $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}) && ($Glob->{'CFG'}->{'GLOBAL'}->{'IPROFILE'}))
    {
        print "\nWARNING! Since you don't want to use Perforce and the OWN-Directory doesn't\n";
        print "exist, the profile generation may fail! If this happens, you have to sync your\n";
        print "sources, first.\n\n";
    }

    #-------------------------------------------------------------------------
    # Set path and name for configuration file:
    #-------------------------------------------------------------------------

    unless ($Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'})
    {
        if ($Glob->{'DEF_CFG'}->{'GLOBAL'}->{'CFG_PATH'})
        {
            $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $Glob->{'DEF_CFG'}->{'GLOBAL'}->{'CFG_PATH'} . $PathSep;
            my $tmpstr = $Glob->{'CFG'}->{'RELEASE'}->{'VERSION'};
            $tmpstr =~ s/\.//g;
            $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} .= $tmpstr;
        }
        else
        {
            $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $Glob->{'CFG'}->{'RELEASE'}->{'OWN'} . $PathSep . "LCEnv";
        }
    }

    if ($Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} && -e "$Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}.cfg") {
        if(RequestInput("WARNING: Config '$Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}.cfg'\nalready exists. Do you want to define an additional suffix? (Y/n) ", "y", "[yYnNjJ]", "Input incorrect! Please, just answer 'y' or 'n'.\n") =~ /[jy]/i) {
            $Glob->{'CFG'}->{'GLOBAL'}->{'NAME_SUFF'} = RequestInput("Suffix to use: ", "", ".+", "Input incorrect! you have to enter at least one character.\n");
            $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} .= "_" . lc($Glob->{'CFG'}->{'GLOBAL'}->{'NAME_SUFF'});
        }
    }
    $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} .= ".cfg";
    
    
    #-------------------------------------------------------------------------
    # Save and exit, if option is given:
    #-------------------------------------------------------------------------

    if ($Opts{'so'})
    {
        SaveCfg();
        LCESetState($Glob->{'CFG'}, $old_state);
        CleanExit(0, 1);
    }

    #-------------------------------------------------------------------------
    # Check reset values for shell call:
    #-------------------------------------------------------------------------

	if ($Opts{'shell'} && $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'}) {
		#Resetting some environments:
		$Glob->{'CFG'}->{'ACTIONS'}->{'MAKE'} = 0;
		$Glob->{'CFG'}->{'GLOBAL'}->{'IPROFILE'} = 0;
		$Glob->{'CFG'}->{'PERFORCE'}->{'SYNC'} = 0;
		$Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'} = 0;
		$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKIGNORE'} = 1;
	}

    #-------------------------------------------------------------------------
    # Check for any locks:
    #-------------------------------------------------------------------------

	unless (CheckLock())
    {
	    LCESetState($Glob->{'CFG'}, $old_state);
	    CleanExit (1, 1);
    }
        
    if ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'})
    {
        unlink ("$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.pid");
        if (open (PID_FILE, "> $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.pid"))
        {
            print PID_FILE "$$ 'LCEnv $Version.$Revision' $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}\n";
            close (PID_FILE);
        }
        else
        {
            print "ERROR: Writing PID file *FAILED* Make will be 'unstoppable'!\n";
        }
    }
    
    #-------------------------------------------------------------------------
    # Kill any running vmake:
    #-------------------------------------------------------------------------

    if (open (PID_FILE, "< $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}/sys/wrk/vmake.pid"))
    {
        if (flock (PID_FILE, 6) || $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFORCE'})
        {
            close (PID_FILE);
            LCEAdmKillVMake($Glob->{'CFG'});
        }
        else
        {
            my $vmake_pid = 0;
            while (<PID_FILE>)
            {
                $vmake_pid = $1 if (/^(\d+)\D*$/);
            }
            print "WARNING: Another make is running (PID $vmake_pid)! IProfile generation / sync disabled.\n\n";
            $Glob->{'CFG'}->{'GLOBAL'}->{'IPROFILE'} = 0;
            $Glob->{'CFG'}->{'PERFORCE'}->{'SYNC'} = 0;
            close (PID_FILE);
        }
    }

    #-------------------------------------------------------------------------
    # Print out generated configuration:
    #-------------------------------------------------------------------------

    if ($Glob->{'CFG'}->{'GLOBAL'}->{'VERBOSE'})
    {
        print "\nThe following configuration will be used:\n\n";

        ShowHashRef($Glob->{'CFG'});

        unless (!$Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} || $Glob->{'CFG'}->{'GLOBAL'}->{'ASSUME_YES'} || $Glob->{'CFG'}->{'GLOBAL'}->{'BATCH_MODE'} || (RequestInput("Is this correct (Y/n)?", "y", "[yYnN]", "Input incorrect. Please, use just [y or n].\n") =~ /^y$/i))
        {
            print "Aborting...\n";
            CleanExit (0);
        }

        print "\n";
    }

    #-------------------------------------------------------------------------
    # Now test / create / update the environment:
    #-------------------------------------------------------------------------

    LCECreateEnv($Glob->{'CFG'});

    #-------------------------------------------------------------------------
    # Save the generated configuration including all changes made:
    #-------------------------------------------------------------------------

    # First, delete the backup entries again:

    if ($Opts{'save'})
    {
        SaveCfg();
    }
    
    #-------------------------------------------------------------------------
    # Make any given target:
    #-------------------------------------------------------------------------

    my $make_ok = LCEDoMake($Glob->{'CFG'});
    LCESendMail ($Glob->{'CFG'}, $make_ok);

    #-------------------------------------------------------------------------
    # Check for CLEAR flag and eventually remove everything again:
    #-------------------------------------------------------------------------
    if ($Glob->{'CFG'}->{'ACTIONS'}->{'CLEAR'})
    {
        LCERemEnv($Glob->{'CFG'});
    }
    elsif ($Opts{'shell'})
    {
        LCESetState($Glob->{'CFG'}, "running shell");
		
        $ENV{'P4USER'} = $Glob->{'CFG'}->{'PERFORCE'}->{'USER'};
        $ENV{'P4CLIENT'} = $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'};
        $ENV{'P4PASSWD'} = $Glob->{'CFG'}->{'PERFORCE'}->{'PASSWD'} if ($Glob->{'CFG'}->{'PERFORCE'}->{'PASSWD'});
        $ENV{'P4PORT'} = "perforce3006.wdf.sap.corp:3006";

        if ($^O =~ /MSWin32/i)
        {
            if ($ENV{'COMSPEC'})
            {
                system("$ENV{'COMSPEC'} /T:$Glob->{'CFG'}->{'GLOBAL'}->{'COLOR'} /K $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}\\iprofile.bat");
            }
            else
            {
                system("cmd.exe /T:$Glob->{'CFG'}->{'GLOBAL'}->{'COLOR'} /K $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}\\iprofile.bat");
            }
        }
        else
        {
            print "Running shell...\n";
            open (CMD_IN, "which bash |");
            my $bashfile = "";
            while (<CMD_IN>)
            {
                $bashfile = $1 if (/^(\S+\/bash)$/);
            }
            close (CMD_IN);
            $bashfile = "/usr/local/bin/bash" unless ($bashfile);            
            open (TMP_OUT, ">$Glob->{'CFG'}->{'RELEASE'}->{'OWN'}/tmprc.bash");
            print TMP_OUT "clear\n";
            print TMP_OUT "echo Welcome to SAP DB / MAX DB Development!\necho\n";
            print TMP_OUT "cd $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}\n";
            print TMP_OUT "source .iprofile\n";
            print TMP_OUT "export PS1=\"\$DEV_STUDIO_PROMPT : \"\n";
            if (-e "/etc/bashrc")
            {
                print TMP_OUT "source /etc/bashrc\n";
            }
            elsif (-e "$ENV{'HOME'}/.bashrc")
            {
                print TMP_OUT "source $ENV{'HOME'}/.bashrc\n";
            }
            print TMP_OUT "echo\n";
            close (TMP_OUT);
            system("$bashfile -rcfile $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}/tmprc.bash");
            unlink ("$Glob->{'CFG'}->{'RELEASE'}->{'OWN'}/tmprc.bash");
        }
        LCESetState($Glob->{'CFG'}, "idle");
    }
    CleanExit(0);
}

##############################################################################
# SaveCfg() - Saves the current Environment.
##############################################################################

sub SaveCfg
{
    if ($Glob->{'CFG'}->{'GLOBAL'}->{'INIT'})
    {
        $Glob->{'CFG'}->{'PERFORCE'}->{'USE'} = (defined $Opts{'p4'}) ? $Opts{'p4'} : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'USE'};
    }
    undef $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'};
    my $cfg_file = $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'};
    undef $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'};
    SaveSimpleCfg($cfg_file, $Glob->{'CFG'}, $Glob->{'CFG'}->{'GLOBAL'}->{'SEC_CFG'});
    $Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'} = $cfg_file;
    print "Release settings saved under '$Glob->{'CFG'}->{'GLOBAL'}->{'CFG_FILE'}'.\n\n";
}

##############################################################################
# ParseCmdLineParams() - Checks and copies the command line parameters.
##############################################################################

sub ParseCmdLineParams
{
    # Global options:
    $Glob->{'CFG'}->{'GLOBAL'}->{'VERBOSE'}         = $Opts{'v'}    ? 1 : 0;
    $Glob->{'CFG'}->{'GLOBAL'}->{'ASSUME_YES'}      = $Opts{'y'}    ? 1 : 0;
    $Glob->{'CFG'}->{'GLOBAL'}->{'BATCH_MODE'}      = $Opts{'b'}    ? 1 : 0;
    $Glob->{'CFG'}->{'GLOBAL'}->{'USE_DEFAULT'}     = $Opts{'d'}    ? 1 : 0;
    $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'}       = (defined $Opts{'prod'}) ? $Opts{'prod'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'SCRATCH'}         = (defined $Opts{'scratch'}) ? $Opts{'scratch'} : $Glob->{'CFG'}->{'GLOBAL'}->{'SCRATCH'} ? 1 : 0;
    $Glob->{'CFG'}->{'GLOBAL'}->{'TOOL_DIR'}        = FixPathSep ($Opts{'tool'}    ? $Opts{'tool'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'TOOL_DIR'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'TOOL_DIR'} : ""));
    $Glob->{'CFG'}->{'GLOBAL'}->{'X64'}             = $Opts{'X64'}  ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'X64'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'64'}              = (defined $Opts{'64'}) ? $Opts{'64'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'64'}  ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'ICON_PATH'}       = FixPathSep ($Glob->{'DEF_CFG'}->{'GLOBAL'}->{'ICON_PATH'});
    $Glob->{'CFG'}->{'GLOBAL'}->{'ICON_TEMPLATE'}   = $Glob->{'DEF_CFG'}->{'GLOBAL'}->{'ICON_TEMPLATE'};
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOW'}             = (defined $Opts{'low'}) ? $Opts{'low'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'LOW'}  ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'}            = (defined $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} : (defined $Opts{'cfg'} ? 0 : 1));
    $Glob->{'CFG'}->{'GLOBAL'}->{'COLOR'}           = (defined $Opts{'color'}) ? $Opts{'color'} : ((defined $Glob->{'CFG'}->{'GLOBAL'}->{'COLOR'}) ? $Glob->{'CFG'}->{'GLOBAL'}->{'COLOR'} : "00");
    $Glob->{'CFG'}->{'GLOBAL'}->{'CHECK_CL'}        = (defined $Opts{'ccl'}) ? $Opts{'ccl'} : (defined $Glob->{'CFG'}->{'GLOBAL'}->{'CHECK_CL'}  ? $Glob->{'CFG'}->{'GLOBAL'}->{'CHECK_CL'} : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_CFG'}        = FixPathSep ((defined $Opts{'prod_cfg'}) ? $Opts{'prod_cfg'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_CFG'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_CFG'} : ""));
    $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_EMAIL'}      = (defined $Opts{'prod_email'}) ? $Opts{'prod_email'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_EMAIL'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_EMAIL'} : "");
    $Glob->{'CFG'}->{'GLOBAL'}->{'WATCHER'}         = (defined $Opts{'watcher'}) ? $Opts{'watcher'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'WATCHER'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'WATCHER'} : "");
    $Glob->{'CFG'}->{'GLOBAL'}->{'HTML_OUT'}        = (defined $Opts{'html'}) ? $Opts{'html'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'HTML_OUT'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'HTML_OUT'} : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'MAILFILTER'}      = (defined $Opts{'mailfilter'}) ? $Opts{'mailfilter'} : (defined $Glob->{'CFG'}->{'GLOBAL'}->{'MAILFILTER'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'MAILFILTER'} : 31);    
    $Glob->{'CFG'}->{'GLOBAL'}->{'XOUT'}            = (defined $Opts{'xout'}) ? $Opts{'xout'} : (defined $Glob->{'CFG'}->{'GLOBAL'}->{'XOUT'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'XOUT'} : 0);    
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}        = FixPathSep ((defined $Opts{'lockfile'}) ? $Opts{'lockfile'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'} : ""));
    $Glob->{'CFG'}->{'GLOBAL'}->{'MACHINE_CFG'}     = ($Opts{'mc'} ? $Opts{'mc'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'MACHINE_CFG'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'MACHINE_CFG'} : ""));
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFORCE'}       = ($Opts{'lockforce'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKWAIT'}        = ($Opts{'lockwait'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKIGNORE'}      = ($Opts{'lockignore'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'IPROFILE'}        = (defined $Opts{'iprofile'} ? $Opts{'iprofile'} : 1);
    $Glob->{'CFG'}->{'GLOBAL'}->{'SEC_CFG'}         = (defined $Opts{'sec'} ? $Opts{'sec'} : (defined $Glob->{'CFG'}->{'GLOBAL'}->{'SEC_CFG'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'SEC_CFG'} : (defined $Glob->{'DEF_CFG'}->{'GLOBAL'}->{'SEC_CFG'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'SEC_CFG'} : 0)));
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOGO_NAME'}       = (defined $Opts{'wln'} ? $Opts{'wln'} : "$ENV{'SYSTEMROOT'}\\System32\\shell32.dll");
    $Glob->{'CFG'}->{'GLOBAL'}->{'LOGO_OFFSET'}     = (defined $Opts{'wlo'} ? $Opts{'wlo'} : 41);
    $Glob->{'CFG'}->{'GLOBAL'}->{'XPROT'}           = ($Opts{'xprot'} ? $Opts{'xprot'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'XPROT'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'XPROT'} : ""));
    $Glob->{'CFG'}->{'GLOBAL'}->{'SYSCALL'}         = (defined $Opts{'syscall'}) ? $Opts{'syscall'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'SYSCALL'} ? $Glob->{'CFG'}->{'GLOBAL'}->{'SYSCALL'} : 0);
	$Glob->{'CFG'}->{'GLOBAL'}->{'RELSTAT'}			= (defined $Opts{'relstat'}) ? $Opts{'relstat'} : $Glob->{'CFG'}->{'GLOBAL'}->{'RELSTAT'};
	
    # Release specific options:
    $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}            = FixPathSep ($Opts{'own'}      ? $Opts{'own'} : ($Glob->{'CFG'}->{'RELEASE'}->{'OWN'} ? $Glob->{'CFG'}->{'RELEASE'}->{'OWN'} : ""));
    $Glob->{'CFG'}->{'RELEASE'}->{'GRP'}            = FixPathSep ($Opts{'grp'}      ? $Opts{'grp'} : ($Glob->{'CFG'}->{'RELEASE'}->{'GRP'} ? $Glob->{'CFG'}->{'RELEASE'}->{'GRP'} : ""));
    $Glob->{'CFG'}->{'RELEASE'}->{'VERSION'}        = $Opts{'rel'}      ? $Opts{'rel'} : ($Glob->{'CFG'}->{'RELEASE'}->{'VERSION'} ? $Glob->{'CFG'}->{'RELEASE'}->{'VERSION'} : "");
    $Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'}     = (defined $Opts{'cl'}) ? $Opts{'cl'}  : ($Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'} ? $Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'} : 0);

    # Perforce specific options:

    $Glob->{'CFG'}->{'PERFORCE'}->{'USE'}           = (defined $Opts{'p4'}) ? $Opts{'p4'} : ((!$Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} && !$Opts{'rm'}) ? (defined $Glob->{'CFG'}->{'PERFORCE'}->{'USE'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'USE'} : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'USE'}) : 1);
    $Glob->{'CFG'}->{'PERFORCE'}->{'USER'}          = $Opts{'p4u'} ? $Opts{'p4u'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'USER'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'USER'} : (($Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'} or !$Glob->{'DEF_CFG'}->{'PERFORCE'}->{'USER'}) ? "" : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'USER'}));
    $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'}        = $Opts{'p4c'} ? $Opts{'p4c'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'} : (($Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'} or !$Glob->{'DEF_CFG'}->{'PERFORCE'}->{'CLIENT'}) ? "" : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'CLIENT'}));
    $Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'}          = $Opts{'p4r'} ? $Opts{'p4r'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'} : (($Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'} or !$Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ROOT'}) ? "" : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ROOT'}));
    $Glob->{'CFG'}->{'PERFORCE'}->{'_MAN_ROOT'}     = $Opts{'p4r'} ? 1 : 0;
    $Glob->{'CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'}    = $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'};
    $Glob->{'CFG'}->{'PERFORCE'}->{'PASSWD'}        = $Opts{'p4pw'} ? $Opts{'p4pw'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'PASSWD'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'PASSWD'} : (($Glob->{'DEF_CFG'}->{'PERFORCE'}->{'ALWAYS_ASK'} or !$Glob->{'DEF_CFG'}->{'PERFORCE'}->{'PASSWD'}) ? "" : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'PASSWD'}));
    $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'}   = $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'} : "";
    $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'}   = $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'} : "";
    $Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'}        = (defined $Opts{'resync'}) ? $Opts{'resync'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'} ? 1 : 0);
    $Glob->{'CFG'}->{'PERFORCE'}->{'SYNC'}          = (defined $Opts{'sync'}) ? $Opts{'sync'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'SYNC'} ? 1 : $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'SYNC'});
    $Glob->{'CFG'}->{'PERFORCE'}->{'SYNCINFO'}      = (defined $Opts{'syncinfo'}) ? $Opts{'syncinfo'} : (defined $Glob->{'CFG'}->{'PERFORCE'}->{'SYNCINFO'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'SYNCINFO'} : (defined $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'SYNCINFO'} ? $Glob->{'DEF_CFG'}->{'PERFORCE'}->{'SYNCINFO'} : 0));

    # Actions to be performed:
    $Glob->{'CFG'}->{'ACTIONS'}->{'CLEAR'}          = $Opts{'c'}  ? 1 : 0;
    $Glob->{'CFG'}->{'ACTIONS'}->{'CMD'}            = $Opts{'cmd'} ? $Opts{'cmd'} : ($Glob->{'CFG'}->{'ACTIONS'}->{'CMD'} ? $Glob->{'CFG'}->{'ACTIONS'}->{'CMD'} : "");
    $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'}         = $Opts{'m'}  ? $Opts{'m'} : ($Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} ? $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} : "");
    $Glob->{'CFG'}->{'ACTIONS'}->{'MAKE'}           = (defined $Opts{'make'} ? $Opts{'make'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'} || $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} || $Glob->{'CFG'}->{'ACTIONS'}->{'CMD'}) ? 1 : 0);
    $Glob->{'CFG'}->{'ACTIONS'}->{'CREATE_ICON'}    = (defined $Opts{'wi'} ? $Opts{'wi'} : (!$Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} ? 0 : $Glob->{'DEF_CFG'}->{'ACTIONS'}->{'CREATE_ICON'}));
    $Glob->{'CFG'}->{'ACTIONS'}->{'ICON_NAME'}      = ($Opts{'win'} ? $Opts{'win'} : ($Glob->{'CFG'}->{'ACTIONS'}->{'ICON_NAME'} ? $Glob->{'CFG'}->{'ACTIONS'}->{'ICON_NAME'} : ""));
    $Glob->{'CFG'}->{'ACTIONS'}->{'CREATE_SHARE'}   = (defined $Opts{'ws'} ? $Opts{'ws'} : (!$Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} ? 0 : $Glob->{'DEF_CFG'}->{'ACTIONS'}->{'CREATE_SHARE'}));
    $Glob->{'CFG'}->{'ACTIONS'}->{'SHARE_NAME'}     = ($Opts{'wsn'} ? $Opts{'wsn'} : ($Glob->{'CFG'}->{'ACTIONS'}->{'SHARE_NAME'} ? $Glob->{'CFG'}->{'ACTIONS'}->{'SHARE_NAME'} : ""));
    $Glob->{'CFG'}->{'ACTIONS'}->{'COPY_IUSER'}     = (defined $Opts{'iuser'} ? $Opts{'iuser'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} ? $Glob->{'DEF_CFG'}->{'ACTIONS'}->{'COPY_IUSER'} : 0)); 

	$ENV{'RELSTAT'} = $Glob->{'CFG'}->{'GLOBAL'}->{'RELSTAT'} if ($Glob->{'CFG'}->{'GLOBAL'}->{'RELSTAT'});

    $Opts{'save'} = (defined $Opts{'save'} ? $Opts{'save'} : ($Glob->{'CFG'}->{'GLOBAL'}->{'INIT'} ? 1 : 0));

    if ($Opts{'en'})
    {
	    while (@{$Opts{'en'}})
	    {
	        my $env_name = shift @{$Opts{'en'}};
	        if ($Opts{'ev'})
	        {
	            $Glob->{'CFG'}->{'ENV'}->{$env_name} = shift @{$Opts{'ev'}};
	        }
	        else
	        {
	            $Glob->{'CFG'}->{'ENV'}->{$env_name} = undef;
	        }
	    }
    }

    if ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'} && $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'})
    {
        print "WARNING! Setting a specific target in productive mode is prohibited.\nThe target '$Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'}' will be ignored.\n\n";
        $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} = "";
    }

    if (!$Glob->{'CFG'}->{'PERFORCE'}->{'SYNC'} && $Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'})
    {
        print "WARNING! options -resync=1 AND -sync=0 are set! -resync will be ignored!\n\n";
        $Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'} = 0;
    }
    
    if ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKWAIT'} && $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFORCE'})
    {
        print "WARNING! Options -lockwait and -lockforce don't go together. -lockwait will be ignored!\n\n";
        $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKWAIT'} = 0;
    }
    
    $ENV{'MACHINE_CONFIG'} = $Glob->{'CFG'}->{'GLOBAL'}->{'MACHINE_CFG'} if ($Glob->{'CFG'}->{'GLOBAL'}->{'MACHINE_CFG'});
}

##############################################################################
# CheckLock() - Checks of any blocking locks.
##############################################################################

sub CheckLock
{
    return 1 unless ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'} && !$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKIGNORE'});
    
    print "Checking locks..";
    
    if (-e "$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.blocked")
    {
        print "..FAILED (Release locked by admin).\n\n";
        return 0;
    }
    
    if (open (LOCK_FILE, "> $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.lock"))
    {
        if (flock (LOCK_FILE, ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKWAIT'} ? 2 : 6)))
        {
            print "..OK\n\n";
            return 1;
        }
        unless ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFORCE'})
        {
            print "..FAILED (other make still running).\n\n";
            return 0;
        }
        
        print "..Found. Terminating other make:\n\n";
        
        LCEAdmKill($Glob->{'CFG'});
        
        print "\n";

        close (LOCK_FILE);
        
        print "Re-checking locks..";
        unless (open (LOCK_FILE, "> $Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.lock"))
        {
            print "..FAILED! (Can't open lock file)\n\n";
            return 0;
        }
        if (flock (LOCK_FILE, ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKWAIT'} ? 2 : 6)))
        {
            print "..OK\n\n";
            return 1;
        }
        print "..FAILED! (Can't fetch lock)\n\n";
    }
    else
    {
        print "..FAILED! (Can't open lock file)\n\n";
    }
    return 0;
}

##############################################################################
# CleanExit() - Exits the program with killing all lock/pid files.
##############################################################################

sub CleanExit
{
    my ($rc_code, $no_kill) = @_;
    
    exit ($rc_code) if ($no_kill);
    
    if ($Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'})
    {
        close (LOCK_FILE);
        unlink ("$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.lock");
        unlink ("$Glob->{'CFG'}->{'GLOBAL'}->{'LOCKFILE'}.pid");
    } 
    
    exit ($rc_code);       
}

##############################################################################
# Usage() - Shows the program usage and exits the script.
##############################################################################

sub Usage
{
    my ($msg) = @_;
    
    eval {
        require Pod::Usage;
        import  Pod::Usage;
    };
    
    if ($@)
    {
        print <<OLDHELP_END;
 LCEnv [-64 <0|1>] [-(b|batch)] [-(c|clear)] [-cfg <CONFIG_FILE>] 
       [-(ccl|check_cl) <1|0>] [-cl <CHANGELIST>] [-cmd <COMMAND>] 
       [-color <COLOR_CODE>] [-(d|default)] [-(ed|edit_defaults)]  
       [-(en|env_name) <NAME>] [-(ev|env_val) <VALUE>] [-grp <PATH>] 
       [-(h|help|?)] [-iprofile <0|1>] [-iuser <0|1>] [-(l|list)] 
       [-lockfile <NAME>] [-lockforce] [-lockwait] [-low <1|0>] 
       [-m <MAKE_TARGET>] [-(man|manual)] [-(mc|machine_cfg) <MACH_CFG>] 
       [-mailfilter <0|1>] [-own <OWN_DIR>] [-p4 <0|1>] [-p4c <P4CLIENT>] 
       [-p4pw <P4PASSWD>] [-p4r <P4ROOT>] [-p4u <P4USER>] [-prod <0|1>] 
       [-prod_cfg <XML_FILE>] [-prod_email <EMAIL_ADRS>] [-rel <RELEASE>] 
       [-resync <0|1>] [-(rm|remove)] [-save <0|1>] [-scratch <0|1>] 
       [-(sec|secure_cfg) <0|1>] [-shell] [-(so|save_only)] [-sync <0|1>] 
       [-syncinfo <NUMBER>] [-tool <TOOL_DIR>] [-(v|verbose)] 
       [-watcher <EMAIL_ADRS>] [-(wi|win_icon) <0|1>] 
       [-(win|win_icon_name) <NAME>] [-(wln|win_logo_name) <FILE>] 
       [-(wlo|win_logo_offset) <POS>] [-(ws|win_share) <0|1>] 
       [-(wsn|win_share_name) <NAME>] [-X64] [-(y|yes)]

 Options:

   -64 <0|1>                Enables/disables 64 bit flag in environment.
   -batch                   Starts in batch mode. No interaction allowed.
   -clear                   Removes environment again after execution.
   -cfg <CONFIG_FILE>       Loads a configuration file.
   -check_cl <0|1|mins>     Only start makes, if changelist is new.
   -cl <CHANGELIST>         Take a specific changelist when syncing.
   -cmd <COMMAND>           Executes a given command when make finished.
   -color <COLOR_CODE>    * Set fore- and background color of window.
   -default                 Use default values whenever possible.
   -edit_defaults           Edit machine-whide default values.
   -env_name <NAME>         Set a envronment variable name to be set.
   -env_val <VALUE>         Sets <NAME> given before to <VALUE>.
   -grp <PATH>              Sets a groups source path.
   -help                    Shows a brief help file.
   -iprofile <0|1>          Creates a new iprofile, if set to 1.
   -iuser <0|1>             Copies a iuser.default to OWN, if set to 1.
   -list                  * Lists all currently installed instances.
   -lockfile <NAME>         Filename to be used as lock file (w/o suffix).
   -lockforce               Enforce override of lock file, kill other run.
   -lockwait                Wait for lockfile to be unlocked.
   -low <0|1>             * Sets process priority to low, if set.
   -m <MAKE_TARGET>         Starts an 'imf MAKE_TARGET'.
   -man                     Shows the complete manual.
   -machine_cfg <MACH_CFG>  Uses a specific machine config for NACHT.ALL.
   -mailfilter <0|1>        If set to 1, monitor mails will always be sent.
   -own <OWN_DIR>           Sets the OWN directory to be used.
   -p4 <0|1>                LCEnv uses Perforce, if set to 1.
   -p4c <P4CLIENT>          Perforce client to be used.
   -p4pw <P4PASSWD>         A Perforce password which might be used.
   -p4r <P4ROOT>            The root directory to be used by client.
   -p4u <P4USER>            The Perforce user to be used.
   -prod <0|1>              Start a productive make, if set to 1.
   -prod_cfg <XML_FILE>     A XML config file to be used for NACHT.ALL.
   -prod_email <EMAIL_ADRS> Email adresses, separated by colon.
   -rel <RELEASE>           Release used (RR[.][CC[.]][dev|cor|ramp]).
   -resync <0|1>            Enforce a complete re-sync of all sources.
   -remove                  Remove instance.
   -save <0|1>              Enforce save of configuration.
   -scratch <0|1>           Enforce a scratch make.
   -secure_cfg <0|1>        Restrict file permissions of config file.
   -shell                 * Open a shell after loading environment.
   -save_only               Just save, don't start environment.
   -sync <0|1>              Initiate an incremental sync.
   -syncinfo <NUMBER>       Print out sync info - lines limited by NUMBER.
   -tool <TOOL_DIR>         Set a different TOOL directory.
   -verbose                 Start in verbose mode.
   -watcher <EMAIL_ADRS>    Send monitor email to adress list.
   -win_icon <0|1>        * Create a windows shortcut icon, if set to 1.
   -win_icon_name <NAME>  * Use given icon name rather than default name.
   -win_logo_name <FILE>  * Use a different icon file than shell32.dll.
   -win_logo_offset <POS> * Use a specific offset within the icon file. 
   -win_share <0|1>       * Create a windows network share, if set to 1.
   -win_share_name <NAME> * Use given share name rather than default name.
   -X64                     Enable 64 bit cross compilation.
   -yes                     Answer questions with 'y' when possible.

 Note:
   * These settings are currently only available under Windows OS.

 For a more detailed help, please use LCEnv -h or LCEnv -man under an OS
 which fully supports perldoc /POD standard (eg. Windows).

OLDHELP_END
        exit(1);    
    }
    else
    {
        my $verb = ($Opts{'man'} ? 2 : ($Opts{'h'} ? 1 : 0));
        pod2usage(-exitcode => 1, -verbose => $verb, -message => $msg);
    }
}

__END__

=head1 NAME

LCEnv - a tool to handle liveCache / SAP DB environments.

=head1 NOTE

This tool is a 'strictly internal' program since it uses internal Perforce
settings to gather release information. There is no confidential data like
passwords in the code - therefore it could be released but it wouldn't
work outside SAP.

=head1 ABSTRACT

This tool is used to install and manage local development environments. There
are many different usage aspects which include:

=over 4

=item *
setting / editing global (machine) environment defaults

=item *
creating new environments

=item *
editing development environment configurations at any time

=item *
removing complete environments again

=item *
syncronizing source content

=item *
automatically adapting Perforce structure changes

=item *
linking specific tasks directly to the environment

=item *
and many more...

=back

Note: You might also take a look into the L<"examples"> section.

=head1 SYNOPSIS

 LCEnv [-64 <0|1>] [-(b|batch)] [-(c|clear)] [-cfg <CONFIG_FILE>] 
       [-(ccl|check_cl) <1|0>] [-cl <CHANGELIST>] [-cmd <COMMAND>] 
       [-color <COLOR_CODE>] [-(d|default)] [-(ed|edit_defaults)]  
       [-(en|env_name) <NAME>] [-(ev|env_val) <VALUE>] [-grp <PATH>] 
       [-(h|help|?)] [-iprofile <0|1>] [-iuser <0|1>] [-(l|list)] 
       [-lockfile <NAME>] [-lockforce] [-lockwait] [-low <1|0>] 
       [-m <MAKE_TARGET>] [-(man|manual)] [-(mc|machine_cfg) <MACH_CFG>] 
       [-mailfilter <0|1>] [-own <OWN_DIR>] [-p4 <0|1>] [-p4c <P4CLIENT>] 
       [-p4pw <P4PASSWD>] [-p4r <P4ROOT>] [-p4u <P4USER>] [-prod <0|1>] 
       [-prod_cfg <XML_FILE>] [-prod_email <EMAIL_ADRS>] [-rel <RELEASE>] 
       [-resync <0|1>] [-(rm|remove)] [-save <0|1>] [-scratch <0|1>] 
       [-(sec|secure_cfg) <0|1>] [-shell] [-(so|save_only)] [-sync <0|1>] 
       [-syncinfo <NUMBER>] [-syscall <0|1>] [-tool <TOOL_DIR>] [-(v|verbose)] 
       [-watcher <EMAIL_ADRS>] [-(wi|win_icon) <0|1>] 
       [-(win|win_icon_name) <NAME>] [-(wln|win_logo_name) <FILE>] 
       [-(wlo|win_logo_offset) <POS>] [-(ws|win_share) <0|1>] 
       [-(wsn|win_share_name) <NAME>] [-X64] [-(y|yes)]

 Options:

   -64 <0|1>                Enables/disables 64 bit flag in environment.
   -batch                   Starts in batch mode. No interaction allowed.
   -clear                   Removes environment again after execution.
   -cfg <CONFIG_FILE>       Loads a configuration file.
   -check_cl <0|1|mins>     Only start makes, if changelist is new.
   -cl <CHANGELIST>         Take a specific changelist when syncing.
   -cmd <COMMAND>           Executes a given command when make finished.
   -color <COLOR_CODE>    * Set fore- and background color of window.
   -default                 Use default values whenever possible.
   -edit_defaults           Edit machine-whide default values.
   -env_name <NAME>         Set a envronment variable name to be set.
   -env_val <VALUE>         Sets <NAME> given before to <VALUE>.
   -grp <PATH>              Sets a groups source path.
   -help                    Shows a brief help file.
   -iprofile <0|1>          Creates a new iprofile, if set to 1.
   -iuser <0|1>             Copies a iuser.default to $OWN, if set to 1.
   -list                  * Lists all currently installed instances.
   -lockfile <NAME>         Filename to be used as lock file (w/o suffix).
   -lockforce               Enforce override of lock file, kill other run.
   -lockwait                Wait for lockfile to be unlocked.
   -low <0|1>             * Sets process priority to low, if set.
   -m <MAKE_TARGET>         Starts an 'imf MAKE_TARGET'.
   -man                     Shows the complete manual.
   -machine_cfg <MACH_CFG>  Uses a specific machine config for NACHT.ALL.
   -mailfilter <0|1>        Filter for mails to be sent (bit mask).
   -own <OWN_DIR>           Sets the OWN directory to be used.
   -p4 <0|1>                LCEnv uses Perforce, if set to 1.
   -p4c <P4CLIENT>          Perforce client to be used.
   -p4pw <P4PASSWD>         A Perforce password which might be used.
   -p4r <P4ROOT>            The root directory to be used by client.
   -p4u <P4USER>            The Perforce user to be used.
   -prod <0|1>              Start a productive make, if set to 1.
   -prod_cfg <XML_FILE>     A XML config file to be used for NACHT.ALL.
   -prod_email <EMAIL_ADRS> Email adresses, separated by colon.
   -rel <RELEASE>           Release used (RR[.][CC[.]][dev|cor|ramp]).
   -resync <0|1>            Enforce a complete re-sync of all sources.
   -remove                  Remove instance again.
   -save <0|1>              Enforce save of configuration.
   -scratch <0|1>           Enforce a scratch make.
   -secure_cfg <0|1>        Restrict file permissions of config file.
   -shell                 * Open a shell after loading environment.
   -save_only               Just save, don't start environment.
   -sync <0|1>              Initiate an incremental sync.
   -syncinfo <NUMBER>       Print out sync info - lines limited by NUMBER.
   -syscall <0|1>           Uses 'system' instead of 'open' for syscalls.
   -tool <TOOL_DIR>         Set a different TOOL directory.
   -verbose                 Start in verbose mode.
   -watcher <EMAIL_ADRS>    Send monitor email to adress list.
   -win_icon <0|1>        * Create a windows shortcut icon, if set to 1.
   -win_icon_name <NAME>  * Use given icon name rather than default name.
   -win_logo_name <FILE>  * Use a different icon file than shell32.dll.
   -win_logo_offset <POS> * Use a specific offset within the icon file. 
   -win_share <0|1>       * Create a windows netwo
   -win_share_name <NAME> * Use given share name rather than default name.
   -X64                     Enable 64 bit cross compilation.
   -yes                     Answer questions with 'y' when possible.

 Note:
   * These settings are currently only available under Windows OS.

=head1 OPTIONS

=over 4

=item B<-64 <0|1>>

Set 64 bit flag in environment (BIT64=1). NOTE: This is not that same as the
'X64' option which enables 64 bit CROSS compilation.

=item B<-(b|batch)>

Start program in batch mode. This option prohibits any interactive action by 
users - if any option is missing, it causes the program to fail.

=item B<-(c|clear)>

Clears the generated environment again after make.

=item B<-cfg <CONFIG_FILE>>

Uses a specific configuration. If a configuration file does not exist, it will 
be created.

=item B<-(ccl|check_cl) <0|1|mins>>

With this option activated, LCEnv will check if the changelist has changed 
since last run. If not, LCEnv will not start any makes.
Instead of 1 or zero, you can also give a number of minutes. If then the 
change list haven't changed since x minutes, the make will be diabled.

=item B<-cl <CHANGELIST>>

Sets a specific change list you want to use when syncing files.

=item B<-cmd <COMMAND>>

A commend to be exeuted after make. NOTE: If the command contains spaces, it
has to be put in double quotes.

=item B<-color <COLOR_CODE>>

Automatically sets background and foreground colors of the shell window. For 
more information, please take a look into the "color manual" by typing 
'color /?'.

=item B<-(d|default)>

Use default settings whenever possible. NOTE: This setting is not compatible
with 'prod' since all settings have to be unambiguous in productive mode.

=item B<-(ed|edit_defaults)>

Edit default parameters. If this option is given, you will be asked for the 
global program defaults. The program will exit after that.

=item B<-(en|env_name) <NAME>>

Gives a name of a environment variable to be set. NOTE: This option has to be
used together with 'ev' since it needs a value to be stored under 'NAME'. If
a corresponding 'ev' is not given, the environment variable will be empty and
therefore UNSET. You can give as many 'en'/'ev' pairs as you want. Be aware
that the order of your names and values is vital. The following examples work
correctly: -en NAME1 -ev VAL1 -en NAME2 -ev VAL1, -en NAME1 -en NAME2 -ev
VAL1 -ev VAL2. The following do NOT: -en NAME1 -en NAME2 -ev VAL2 -ev VAL1, 
would assign NAME1=VAL2 and NAME2=VAL1.

=item B<-(ev|env_val) <VALUE>>

The value of the environment varable named with 'en' before. See above for more
information.

=item B<-grp <PATH>>

Inserts a second path into vmake's source paths. This may be any path allowed 
by vmake (including 'remote'-paths). NOTE: If you have Perforce support turned
on, it syncs files to the grp path rather than to the OWN dirrectory.

=item B<-(h|help|?)>

Displays a brief help description and exits the program.

=item B<-iprofile <0|1>>

If set to 1, a new iprofile will be created by using createlcenv. This setting
is always set to 1, by default.

=item B<-iuser <0|1>>

If set to 1, the file $TOOL/config/iuser.default will be copied into the
releases OWN directory.

=item B<-(l|list)>

Lists up all installed instances and exits.

=item B<-lockfile <NAME>>

Activates an flock mechanism which can be used to synchronize LCEnv runs. The
file name given may include a path but should not contain a suffix. The
following two files will be created:

=over 4

=item <NAME>.lock

The empty lock file which is used by flock.

=item <NAME>.pid

Which holds the program process ID and some additional information.

=back 

=item B<-lockforce>

If this option is given, LCEnv will enforce its run. If the lockfile is locked
the other process will be terminated.

=item B<-lockwait>

If this option is set and the lockfile is locked by another process, LCEnv will
wait until the file is unlocked again rather that just exiting the program.

=item B<-low <0|1>>

Starts the new process in low priority mode.

=item B<-m <TARGET>>

Makes a specific target by calling 'imf <MAKE_TARGET>' after finishing sync.
NOTE: This option is not compatible with 'prod' and will be deactivated, if
'prod' is set.

=item B<-man>

Displays the complete manual and exits.

=item B<-(mc|machine_cfg) <MACH_CFG>>

Sets the environment variable MACHINE_CONFIG to a specific value. NOTE: This
variable is needed by productive makes to identify the make machine type.
Valid values are:

=over 4

=item *
AIX: rs6000, rs6000_64

=item *
DEC: alphaosf

=item *
HP: hp_64, hpia64

=item *
LINUX: linuxintel, linuxia64

=item *
NT: NTintel, NTia64

=item *
SUN: sun, sun_64, sunx86_64

=back

NOTE: These values may change in future.

=item B<-mailfilter <0|1>>

Bit mask for monitor mails to be sent:

=over 4

=item *
0 = Don't send any mails.

=item *
1 = Send 'No Make' mails. (Bit 0)

=item *
2 = Send 'All OK' (no warnings) mails. (Bit 1)

=item *
4 = Send mails for makes containing warnings. (Bit 2)

=item *
8 = Send mail when make failed. (Bit 3)

=item *
16 = Send mail when make was terminated. (Bit 4)

=back

=item B<-own <OWN_DIR>>

Set the own directory to be used. If 'p4' option is also set and configuration
files are stored centrally, this option may also be used in conjuction with
'save' to move complete source trees savely without confusing Perforce.

=item B<-p4 <0|1>>

Use Perforce source control. If set to 1, it enables LCEnv to sync sources, 
determine changelists and automatically create and remove complete source 
tree's. NOTE: This option also have to be turned on to enable LCEnv to
automatically set new correction levels or build prefixes.

=item B<-p4c <P4CLIENT>>

Set Perforce client to be used.

=item B<-p4pw <P4PASSWD>>

Set Perforce password to be used for the client. NOTE: Up to now, this password
will be stored UNENCRYPTED!

=item B<-p4r <P4ROOT>>

Manually set a Perforce client. NOTE: If a given Perforce client already exist,
its root will be OVERWRITTEN - use this option with CARE!

=item B<-p4u <P4USER>>

Set the Perforce user to be used.

=item B<-prod <0|1>>

Initiates a productive make by calling 'NACHT.ALL', if set to 1. NOTE: This 
option is not compatible with 'default' or 'm'.

=item B<-prod_cfg <XML_FILE>>

Use a different nachtmake definition file rather than the default 
'Nachtmake.xml'. NOTE: This file has to be stored in OWN/sys/wrk.

=item B<-prod_email <EMAIL_ADRS>>

Use email recipents given in EMAIL_ADRS to send error make mails to instead of
letting checkmake determine user names. Email recipents should be given in
double quotes and separated by a simple colon without spaces:
"me@here.org,you@there.com"

=item B<-rel <RELEASE>>

Set the release to be installed. The release string should be given in one
of the following formats:

=over 4

=item *
7403(DEV|COR|DEVELOP)

=item *
740320

=item *
7.4.03.(dev|cor|develop)

=item *
7.4.03.20

=item *
75dev

=item *
7.5.dev

=item *
75.dev

=back

NOTE: If any part is missing, it will be asked for. It is possible to leave
the correction level empty, if 'dev' is given as build prefix. This means that
LCEnv will always use the latest development branch of the given release.

=item B<-resync <0|1>>

Causes LCEnv to do a complete resync rather than the incremental one. To use
this feature, 'p4' has to be set to 1.

=item B<-(rm|remove)>

Removes a previously installed release. To remove a tree AFTER execution (make),
please use 'clear' instead.

=item B<-save <0|1>>

Enforce a manual save of the configuration. If LCEnv is in initial mode (no
configuration file given), this value will be automatically set to 1 unless
it has been explicitly set to 0.

=item B<-scratch <0|1>>

Initiate a scratch make rather than an incremental one.

=item B<-(sec|secure_cfg) <0|1>>

If set to 1, the release configuration files are secured by restricting
access permission to user only.

=item B<-shell>

Runs a shell with the created environment instead of just exiting.

=item B<-(so|save_only)>

Saves any changed parameter and just exits the program instead of really doing
anything.

=item B<-sync <0|1>>

Perform an incremental sync. The option 'p4' has to be activated, otherwise this
option will be ignored.

=item B<-syncinfo <NUMBER>>

Print out a sync info of new files. The output is limited to NUMBER lines. If
NUMBER is set to -1, all changed files since last sync will be printed out.

=item B<-tool <TOOL_DIR>>

Set a different TOOL directory to use. Normally, LCEnv would always use the same
TOOL directory where itself was started from.

=item B<-(v|verbose)>

Run LCEnv in verbose mode. 

=item B<-watcher <EMAIL_ADRS>>

Send a status mail to a list of monitor users. Again, these email adresses 
should be surrounded by double quotes and separated by a single colon, see
'prod_email' option for an example.

=item B<-(wi|win_icon) <0|1>>

Create a windows shortcut icon, if set to 1. Its location is derrived from 
the default settings.

=item B<-(win|win_icon_name) <NAME>>

Use given name as windows shortcut name. If not given, a default name will be
created (namely "Developer Studio REL.(CORR.)Build").

=item B<-(wln|win_logo_name) <FILE>>

Specify a different (DLL)-File to load the icon from. If obmitted, the 
default file shell32.dll will be taken.

=item B<-(wlo|win_logo_offset) <POS>>

Specify a different offset within the icon file to load the icon from. If
obmitted, offset 41 will be taken.

=item B<-(ws|win_share) <0|1>>

Create a windows network share for this release, if set to 1.

=item B<-(wsn|win_share_name) <NAME>>

Use the given share NAME instead of the default one.

=item B<-X64>

Enable 64 bit cross compilation. If this option is not given, cross compilation
will automatically be deactivated.

=item B<-(y|yes)>

Assume 'yes' to questions whenever possible. Mostly makes sense in verbose
mode.

=back

=head1 NEWS / CHANGES

 1.53:
        New Features:
        
            - Monitor mails can now be supressed, if everything worked fine.
            
            - Added extended outputs to monitor mails.

 1.52:  New Features:
            
            - Introduced file security for config files.
            
        Bug Fixes:
        
            - Refined error code handling.

 1.50:  New Features:
 
            - Introduced new locking mechanisms. For more information take a 
              look at options 'lockfile', 'lockforce' or 'lockwait'
            
            - iuser.bat can now be copied automatically. NOTE: A file 
              iuser.default must be present in $TOOL\config directory.
              
            - LCEnv can now send status information to a monitor user or a 
              list of users by calling the 'watcher' option.
              
            - Various arguments can now be forwarded to NACHT.ALL by using 
              'prod_xxx' options.
              
            - A sync info ca be printed out by using the 'syncinfo' options.
              Output is limited to the value given with 'syncinfo'.
              
            - A new option 'save_only' was introduced to be able to just change
              and save new options.
              
            - Additional environments can now be defined by using 'en' and 'en'
              options.
              
            - The help output has been reimplemented. 'help' now just prints out
              a short help while 'man' prints the complete help page.
              
        Bug fixes:
        
            - Too many to be mentioned here...;-)

 1.33:  New Features:
 
            - New parameter -prod_cfg introduced which enables NACHT.ALL to
              handle multiple configurations.
              
            - New parameter -prod_email introduced which causes NACHT.ALL only
              to send emails to this adress.
              
            - New parameter -watcher introduced which causes LCEnv to send 
              status emails to a watcher's email.

 1.32:  Bug fixes:
 
            - Check for P4PASSWD was too late, resp. env variable was not set
              in time.

 1.31:  New Features:
            
            - If option 'cfg' is given but file doesn't exist, LCEnv will
              start in 'initial' mode and use the cfg file name as template.
              
        Bug fixes:
            
            - If option p4c is given, it will also reset p4r to the new
              client's root.
 
 1.30:  New Features:
 
            - New options 'win' and 'wsn' to manually name windows icon
              and share names.
 
        Bug fixes:
 
            - When installing a 76dev release, LCEnv failed to set correction
              level and build prefix correctly.
              
            - ReleaseDB entries are now alwys updated again since they
              disappeared sometimes.

 1.29:  New Features:

            - Added option 'ccl'. With this option turned on, a make will
              only start, if the change list has changed since last run.

 1.28:  Bug fixes:

            - LCEnv asked for a "missing correction level" by mistake when
              using Perforce support in conjunction with an "always latest
              development branch" window (no real correction level). Affected
              only V75.(00).develop so far and has now been removed.

            - Fixed problem with "\\" path separators under UNIX systems.

 1.27:  Started this section.

        Bug fixes:

            - Configuration file name got lost while installation of new
              Studios. Therefore, all new shortcuts got empty configuration
              names.

        New Features:

            - Built-in color control. Use -color=<2-digit-color-code> to
              colour your windows. (Check command 'color /?' for color
              codes.)

            - Info string '(low prio)' is now shown in windows title when
              having set a window to low priority.

=head1 DESCRIPTION

LCEnv was originally developed to be a redesigned and cleaned-up version
of DevStudio. However, there were so many new functions and external
requirements that the final outcome became a completely new tool which
has not much to do with the original any more.
It was designed to be able to do anything the old tool could do and much
more - the focus there was to implement functions which are needed by the
developers and are implemented e.g. as shell scripts by every developer himself
so far. Additionally, the intention was to tie DevStudio to Perforce to be
able to install a Studio for everything you can see under Perforce //sapdb
root. When we finally decided to implement an iprofile generation mechnism,
the way was free to create a completely new version of DevStudio which is
now 100% dynamic and does not need any patch when introducing new Releases.

So far, so good...Now let's take a deeper look into its usage.

Usage of this tool can divided into 4 groups: Initial setup, managing releases,
using environments and performing automatic tasks.

First comes the initial setup. This step is a bit annoying but quite important
for LCEnv and sould be done at first use - if you don't, LCEnv will not stop
asking you lot's of questions which normally only have to be answered once.
A detailed walkthrough is described in the next section 'Initial Configuration'.

When initial setup is done, you can proceed with generating new studio's.
To learn more about how to do this, please take a look into the examples and
read the section 'Creating New Environments'.

=head2 Initial Configuration

When starting LCEnv for the very first time, it'll inform you that global
system defaults were not found:

    C:\>lcenv

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Could not find any system defaults. Do you want to set them up now ? (Y/n) y

Alternatively, you can force LCEnv to setup system defaults by calling it with
the '-ed' option.

Now you'll be faced with the following questions:

=over 4

=item *
Configuration file location [default = 'c']

    Where do you want your Release configurations to be stored:

    [o] = Each in its release's own directory.
    [c] = Centrally, under C:\SAPDevelop\Devtool\data\LCEnv. (default)
    [s] = Centrally, i'd like to choose a path manually.

    NOTE: If you store configurations centrally, you don't have to give a complete
          path when selecting a configuration.

    Please select :

Here you can decide where the configuration file for one specific SAP DB
release should be stored. This configuration is needed every time you want to
open a new Devstudio window or change some settings ('-cfg <config file>').
Since this option is needed quite often, it may be useful to store these files
centrally. This has the advantage, that LCEnv knows where to look for these
files - in other words: If you have to type the command manually for any
reasons, you don't have to type the complete path to the configuration file.
Additionally, it may make managing configurations easier when everything is
stored centrally.

But, don't be too serious about this question - LCEnv will work fine
regardless what option you choose here. That's what i did:

    Please select : s
    Configuration dir : c:\sapdevelop\profiles

=item *
Always check Perforce settings [default = 'n']

    Do you want all your settings checked / compared with Perforce every time you
    open a new development window (makes LCEnv robust against changes but slow
    when starting) ? (y/N)

If this question is answered with 'y', LCEnv will check all your Perforce
settings every time you open a development window. These settings include:

=over 4

=item *
P4 User

=item *
P4 Client

=item *
Release's position within the Perforce tree

=item *
P4 Server path for this release

=item *
Actual P4 change list for this release

=back

Please note that this option makes LCEnv more robust against environment
changes and misconfiguration from outside: If a P4 client is deleted, it
will be re-created; if - for example - somebody decides to introduce a new
development tree 'V7405dev', LCEnv would notice that and would move the
actual 'V7404dev' tree from '//sapdb/V74/develop' to '//sapdb/V74/c_04/develop'.
On the other hand, this option makes starting devstudio's quite slow -
opening a window would take between 5 and 10 seconds at least. For this
reason, this option is deactivated by default.

=item *
Sync on window open [default = 'n']

    Do you want Perforce to sync your files when opening a new window ? (y/N)

With this option, you can tell LCEnv to synchronize all sources of a release
when opening a new development window. Please note, that this option has no
effect when you answered 'n' at the question above.

Since synchronizing can take quite some time and is therefore mostly done at
night time, this option is deactivated by default.

=item *
Always ask for Perforce settings [default = 'n']

    Do you want to be asked for your perforce settings every time when creating
    a new instance (to manage multiple configurations) ? (y/N)

This option allowes you to handle multiple Perforce settings for each release.
Some users like to have for example different clients for each release. These
users should answer this question with 'y', all others should say 'n' here to
avoid answering stupid questions every time when creating a new studio.

=item *
Setting up Perforce defaults [default = 'y']

    Do you want to set up your default perforce settings now ? (Y/n)

Here you can set your Perforce default settings which are used to generate new
Studios unless you explicitly tell LCEnv to use different settings.

NOTE: It is quite important that you setup your defaults here since LCEnv
needs these information to generate new Studio's. Not setting them up is like
answering the question above (Always aks for Perforce settings) with 'y'.

=over 4

=item *
Perforce user [default = 'l']

    Please select which Perforce user you want to use:

    [l] = use your login name (d040173), default
    [s] = select a different user name manually

    Please select:

Here you can enter your Perforce user. Normally, 'l' should do it *BUT BE
VERY CAREFUL HERE* since Perforce names are CASE SENSITIVE! If the given
login name doesn't fit, *DON'T USE IT* - take 's' instead and enter your
user manually.

HINT: You can check your actual Perforce user and client by looking into
the window title of the Perforce window. Please check here, whether your
user and client are written lower- or upper-case.

=item *
Perforce client [default = 'h']

    Please select which Perforce client you want to use:

    [h] = use the local host name (P73709), default
    [s] = select a different client name manually

    Please select:

Now, you have to enter the Perforce client you want to use. Again, this
name ist CASE-SENSITIVE. Read section above to check for the correct case -
if the host name doesn't fit, enter the client name manually by selecting 's'.

=item *
Perforce client root [default = 'p']

    Please select which client root you want to use for your P4 client:

    [c] = use the current directory (C:\)
    [p] = use current P4 client root (c:\SAPDevelop)
    [s] = select a different client name manually

    Please select:

Here, you can select the Perforce client root you want to use. Please note,
that all releases have to be installed into a sub directory of this root in
order to use Perforce correctly. In fact, LCEnv will deny installing any
releases outside this root.

HINT: If the second line ('[p] = use current P4 client root (c:\SAPDevelop)')
appears, the Perforce user and client you entered above were correct. If not,
LCEnv couldn't either connect to Perforce with the settings you entered, or
you just created a new client.

NOTE: Normally, you should always use 'p' here since any other option will
permanently cheange your Perforce client.

=back

=item *
Create shortcuts [default = 'y'], Windows only

    Do you want a new shortcut to be created by default ? (Y/n)

Here you can decide wether you want a shortcut to be created by default or
not. Most users answer 'y' here.

=over 4

=item *
Shortcut storage place [default = 'm'], Windows only

    Where do you want to store your shortcuts:

    [m] = In Windows start menu. (default)
    [d] = On desktop.
    [e] = At a place stored in %DEVSTUDIO_SHORTCUTS%
    [s] = Select a different place manually.

    Please select :

Now, you can tell LCEnv where to create the shortcuts. If using 'm', LCEnv
will create a folder in the start menu called "SAPDevelop Dxxxxxx" where
all shortcuts are stored. Alternatively, you can store your shortcuts on
desktop ('d') or at any other place ('e', 's').

=back

=item *
Create network shares [default = 'y'], Windows only

    Do you want a new windows network share to be created by default ? (Y/n)

If you want to have a network share to be created with every new studio,
answer 'y' here.

NOTE: To meet earlier specifications, LCEnv creates share names in the format
'V[RELVER]' e.g. 'V75' for the latest development branch of a release. For any
other release it takes the format 'V[RELVER][CORR][LC_STATE]' e.g. 'V7404COR'

=back

When you have answered all these questions, the initial setup is complete.
The program will exit then and has to be restarted.

The generated configuration file can be found under:
'%TOOL%\data\LCEnv.default'.

=head2 Creating New Environments

To create a new environment you just have to call LCEnv without parameters.
If you do so, you'll be asked for a release, correction level, build prefix
and OWN directory. After that, all directories are created and at least a
minimal sync was performed to be able to create a iprofile.bat.

Alternatively, you can call LCEnv with the parameters '-rel <RELEASE>' and
'-own <OWN-Directory>' - the order is not important. You can also add any
other parameter given above to override default parameters or to perform
default tasks.
But be careful here: Any parameter given at this time will be stored
permanently. Example: If you give a make target here, it will be executed
every time, you open a window!

After creating a new instance, you'll find a release configuration file
which is either stored centrally with the release as name, e.g. 'V7600DEV.cfg'
or within the OWN-Directory, stored as 'LCEnv.cfg'. To have a deeper look
into these files, please take a look into the section 'Release Configuration'.

Instead of starting the studio by clicking the Icon, you can also call it by
using LCEnv from command line (any development window): 'LCEnv -cfg <Cfg-File>'.
If you want to change any settings later, you adding them as parameter in
the command line. Note: If you want to have them saved permanently, you
also have to enter '-save'. Take a look into the 'EXAMPLES' section for more
information.

=head2 Configuration Files

Since configuration files should normally not be edited by hand, i only show
some example files here instead of completely explaining all parameters.

=over 4

=item *
Default configuration [%TOOL%\data\LCEnv.default]

    # File generated by SimpleCfg v1.00
    # Generation time: Wed Aug 20 16:53:07 2003

    [GLOBAL]
    ICON_PATH=C:\Documents and Settings\All Users\Start Menu\SAPDevelop D010101
    CFG_PATH=c:\sapdevelop\profiles

    [PERFORCE]
    ROOT=c:\SAPDevelop
    USE=0
    USER=d010101
    ALWAYS_ASK=0
    CLIENT=P10101
    PORT=perforce3006.wdf.sap.corp:3006
    SYNC=0

    [ACTIONS]
    CREATE_SHARE=1
    CREATE_ICON=1

=item *
Release Configuration [Example here: V7600DEV.cfg]

    # File generated by SimpleCfg v1.00
    # Generation time: Tue Aug 19 18:15:00 2003

    [RELEASE]
    GRP=
    OWN=c:\SAPDevelop\V76\develop
    VERSION=76.00.DEV
    _CHANGELIST=50253
    CHANGELIST=0

    [GLOBAL]
    LOW=0
    BATCH_MODE=0
    VERBOSE=0
    64=0
    PROD_MAKE=0
    ASSUME_YES=0
    CFG_FILE=c:\sapdevelop\Profiles\7600DEV.cfg
    ICON_PATH=C:\Documents and Settings\All Users\Start Menu\SAPDevelop D010101
    USE_DEFAULT=0
    INIT=
    TOOL_DIR=
    X64=0
    SCRATCH=0

    [PERFORCE]
    _VIEW_REMOVED=
    ROOT=c:\SAPDevelop
    USE=0
    USER=d010101
    _USER_CREATED=0
    CLIENT_PATH=//P10101/V76/develop
    _ROOT_ADDED=
    RESYNC=0
    PASSWD=
    _ROOT_REMOVED=
    _CLIENT_CREATED=0
    SERVER_PATH=//sapdb/V76/develop
    _VIEW_ADDED=
    CLIENT=P10101
    PORT=perforce3006.wdf.sap.corp:3006
    SYNC=0

    [ACTIONS]
    CREATE_SHARE=1
    CREATE_ICON=1
    _ICON_CREATED=C:\Documents and Settings\All Users\Start Menu\SAPDevelop D010101\Developer Studio 76.00.DEV.lnk
    MAKE=0
    CMD=
    CLEAR=0
    _SHARE_CREATED=V76
    TARGET=

As you can see, some settings overlap with the default settings. This is,
because they were copied from the defaults at creation time. Some of them
are even not really used (such as 'GLOBAL/ICON_PATH') but they were
essential when creating the environment...

=back

=head1 EXAMPLES

Here you can find some usage examples for LCEnv. Please note that this section
implicates that you have set up the default settings before. If you don't,
please do this first.

=head2 Interactive Mode

Scenario: I want to create a new Studio for Release 7.6 using default settings.

Screenshot:

    C:\>LCEnv

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG


    The LC release is not set. Please select one:

    [1] = 72
    [2] = 73
    [3] = 74
    [4] = 75
    [5] = 76

    Please select: 5

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....NOT SET!

    Please select which $Own directory you want to use:

    [c] = use the current directory (C:\)
    [p] = use P4 client root (c:\SAPDevelop)
    [r] = use releases default directory (c:\SAPDevelop\V76\develop), default
    [s] = select a different client name manually

    Please select: r

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Updating Perforce client 'P73709'....OK

    Doing minimal sync to be able to create profile...

    Creating iprofile....OK

    Creating Windows shortcut....OK

    Creating Windows share....OK

    Updating Release DB entries....OK

    Release settings saved.

As you can see above, LCEnv didn't ask for a correction level and a build
prefix - it realised that there exists only one release '76' within the
Perforce tree.

NOTE: Even though i told LCEnv NOT to use Perforce by default, it still
uses Perforce to create new studios. That's why you really should setup
your defaults, first.

Scenario: Same as above, but i want to have a low priority window now.

Screenshot:

    C:\>LCEnv -low=1

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG


    The LC release is not set. Please select one:

    [1] = 72
    [2] = 73
    [3] = 74
    [4] = 75
    [5] = 76

    Please select: 5

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....NOT SET!

    Please select which $Own directory you want to use:

    [c] = use the current directory (C:\)
    [p] = use P4 client root (c:\SAPDevelop)
    [r] = use releases default directory (c:\SAPDevelop\V76\develop), default
    [s] = select a different client name manually

    Please select: r

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Updating Perforce client 'P73709'....OK

    Doing minimal sync to be able to create profile...

    Creating iprofile....OK

    Creating Windows shortcut....OK

    Creating Windows share....OK

    Updating Release DB entries....OK

    Release settings saved.

=head2 Default Use / Creating a New Instance

Scenario: I want to create new window for release 7.6

Screenshot:

    C:\>LCEnv -rel 7600dev -own c:\sapdevelop\V76\develop

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....OK

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Updating Perforce client 'P73709'....OK

    Doing minimal sync to be able to create profile...

    Creating iprofile....OK

    Creating Windows shortcut....OK

    Creating Windows share....OK

    Updating Release DB entries....OK

    Release settings saved.

And again: Since there's only one release 7.6, it'S enough to say:

    C:\>LCEnv -rel 76 -own c:\sapdevelop\V76\develop

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....OK

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Updating Perforce client 'P73709'....OK

    Doing minimal sync to be able to create profile...

    Creating iprofile....OK

    Creating Windows shortcut....OK

    Creating Windows share....OK

    Updating Release DB entries....OK

    Release settings saved.

=head2 Stating a Studio From Command Line Prompt

Scenario: I want to start my release 7.6 now (using central configurations).

Screenshot:

    C:\>LCEnv -cfg 7600dev -shell

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking release OWN....OK

    Creating iprofile....OK

    Updating Release DB entries....OK

    Welcome to 7.6.00.00 SAP DB Developer Studio (DEV) !

    ---- ! Microsoft SDK is used   ! ----
    Targeting Windows 2000 and IE 5.0 RETAIL


    %OWN%: c:\sapdevelop\V76\develop
    %GRP%: \\P79775\V76\develop
    %ALL%:
    %REL%:

    %VMAKE_PATH%: c:\sapdevelop\V76\develop


    C:\SAPDevelop\V76\develop>

Scenario: I want to start my release 7.6 now (using local configurations).

Screenshot:

    C:\>LCEnv -cfg c:\sapdevelop\V76\develop\LCEnv.cfg -shell

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking release OWN....OK

    Creating iprofile....OK

    Updating Release DB entries....OK

    Welcome to 7.6.00.00 SAP DB Developer Studio (DEV) !

    ---- ! Microsoft SDK is used   ! ----
    Targeting Windows 2000 and IE 5.0 RETAIL


    %OWN%: c:\sapdevelop\V76\develop
    %GRP%: \\P79775\V76\develop
    %ALL%:
    %REL%:

    %VMAKE_PATH%: c:\sapdevelop\V76\develop


    C:\SAPDevelop\V76\develop>

=head2 Temporarily Using Different Parameters

Scenario: I want to sync my freshly created release 7.6.

Screenshot:

    C:\>LCEnv -cfg 7600dev -p4=1 -sync=1

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....OK

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Sync files (incremental)....OK

    Creating iprofile....OK

    Updating Release DB entries....OK

Note: To be able to use the 'sync' option, you have to temporarily activate
Perforce support by also giving '-p4=1'. Otherwise '-sync' wouldn't have
any effect.

=head2 Permanently Changing Parameters

Scenario: I liked that autosync thingy, let's make it permanent!

Screenshot:

    C:\>LCEnv -cfg 7600dev -p4=1 -sync=1 -save

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Checking perforce path for LC V76.00.develop....OK

    Checking release OWN....OK

    Checking Perforce client path....OK

    Retrieving actual changelist....OK (50356)

    Sync files (incremental)....OK

    Creating iprofile....OK

    Updating Release DB entries....OK

    Release settings saved.

Note here that the program said that it saved the setting at the end. They're
now permanent.

=head2 Removing an Instance

Scenario: I want to remove my old release 7.4.02 DEV

Screenshot:

    C:\>LCEnv -cfg 7402dev -rm

    LCEnv v1.23 (2003-08-20), (c)2003 SAP AG

    Removing environment for '74.02.DEV'. Please be patient...

    Removing Release DB entry....OK

    Perforce client restored.

    Directory tree removed.

=head2 Performing a Nightly Sync and Make

To perform a nightly sync and make, you could create a new scheduled task
which calls this command:

    perl c:\sapdevelop\devtool\bin\LCEnv.pl -cfg 7600dev -p4=1 -sync=1 -m all

This commandy temporarily activates Perforce support, synchronizes all files
and then call imf with target 'all'.

=head1 COPYRIGHT

Copyright 2003 SAP AG
