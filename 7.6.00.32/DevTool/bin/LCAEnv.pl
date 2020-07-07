#!/usr/bin/perl
#
#    LCAEnv.pl - Creates and syncs a complete LCA environment.
#
#    @(#)LCAEnv     2003-02-18
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

use strict;

BEGIN
{
    unless (defined $ENV{'TOOL'})
    {
    	my $callers_path = (caller())[1];
	    $callers_path  =~ tr/\//\\/ if ($^O=~/.*win32.*/i);
		$ENV{'TOOL'} = $1 if ($callers_path =~ /^(.*)[\\\/]bin[\\\/]LCAEnv.*/);
		print "NOTE: TOOL has been set to $ENV{'TOOL'}\n";
    }

    unshift @INC, "$ENV{'TOOL'}/lib/perl5";
    unshift @INC, "$ENV{'TOOL'}/bin";
    
}

require Getopt::Long;
import  Getopt::Long;
require Env::LCAEnv;
import  Env::LCAEnv;
require IO::SimpleCfg;
import  IO::SimpleCfg;
require Tool::HashRef;
import  Tool::HashRef;

##############################################################################
# Console Preparation (redirect STDERR to STDOUT and disable buffering):

close   STDERR;
open    STDERR, ">&STDOUT";
$| = 1;

##############################################################################
# Global variables:

my $ProgramName         = "LCAEnv";
my $Version             = "1";
my $Revision            = "11";
my $Date                = "2005-06-27";

my %Opts;
my $Glob                = {};


##############################################################################
# Main routine:
##############################################################################

unless ($ENV{'TOOL'})
{
    print "%TOOL% is not set in the environment!\n";
    exit (10);
}

$ENV{'SHELL'} = "/bin/sh";

print "\n$ProgramName v$Version.$Revision ($Date), (c)2003 SAP AG\n\n";

if (!GetOptions( \%Opts, 'b|batch', 'cfg=s', 'cl=i', 'c|clear', 'd|default', 'h|help',
                 'lc=s', 'm=s', 'nvc', 'own=s', 'p4c=s', 'p4r=s', 'p4u=s', 'no_profile',
                 'prod', 'rel=s', 'rm|remove', 'rs|resync', 'v|verbose', 'y|yes', 'platform=s') || $Opts{'h'} || @ARGV)
{
    Usage();
}

# First, load the configuration file:
$Glob->{'_CFG_LOADED'} = 0;

if ($Opts{'cfg'})
{
    $Glob->{'CFG'} = LoadSimpleCfg($Opts{'cfg'});
    $Glob->{'_CFG_LOADED'} = 1;
}
elsif ($Opts{'own'} && -d $Opts{'own'})
{
    my $cfg_file = JoinPath($Opts{'own'}, "LCAEnv.cfg");
    if (-e $cfg_file)
    {
        $Glob->{'CFG'} = LoadSimpleCfg($cfg_file);
        $Glob->{'_CFG_LOADED'} = 1;
    }
}

# Then, overload settings by given command line parameters:
ParseCmdLineParams();

if ($Glob->{'CFG'}->{'ACTIONS'}->{'REMOVE'})
{
    #=========================================================================
    # REMOVE INSTANCE
    #=========================================================================

    unless ($Glob->{'_CFG_LOADED'})
    {
        print "ERROR! To remove a LCAPPS environment you have to give either an OWN\ndirectory or a configuration file as command line parameter.\nExiting...\n";
        exit 10;
    }

    LCARemEnv($Glob->{'CFG'});
}
else
{
    #=========================================================================
    # INSTALL INSTANCE
    #=========================================================================

    #-------------------------------------------------------------------------
    # First, check configuration integrity and ask for missing parameters:
    #-------------------------------------------------------------------------
    LCACheckEnv($Glob->{'CFG'});

    #-------------------------------------------------------------------------
    # Print out generated configuration:
    #-------------------------------------------------------------------------
    print "\nThe following configuration will be used:\n\n";

    ShowHashRef($Glob->{'CFG'});

    unless ($Glob->{'CFG'}->{'GLOBAL'}->{'ASSUME_YES'} || $Glob->{'CFG'}->{'GLOBAL'}->{'BATCH_MODE'} || (RequestInput("Is this correct (Y/n)?", "y", "[yYnN]", "Input incorrect. Please, use just [y or n].\n") =~ /^y$/i))
    {
        print "Aborting...\n";
        exit (0);
    }

    print "\n";

    #-------------------------------------------------------------------------
    # Now test / create / update the environment:
    #-------------------------------------------------------------------------
    LCACreateEnv($Glob->{'CFG'});

    #-------------------------------------------------------------------------
    # Save the generated configuration including all changes made:
    #-------------------------------------------------------------------------
    SaveSimpleCfg(JoinPath($Glob->{'CFG'}->{'RELEASE'}->{'OWN'},"LCAEnv.cfg"), $Glob->{'CFG'});

    #-------------------------------------------------------------------------
    # Eventually compile the a target or the whole thing:
    #-------------------------------------------------------------------------
    if ($Glob->{'CFG'}->{'ACTIONS'}->{'MAKE'})
    {
        LCAExecuteEnv($Glob->{'CFG'});
    }

    #-------------------------------------------------------------------------
    # And finally, clear everything again, if desired:
    #-------------------------------------------------------------------------
    if ($Glob->{'CFG'}->{'ACTIONS'}->{'CLEAR'})
    {
        LCARemEnv($Glob->{'CFG'});
    }
}

##############################################################################
# ParseCmdLineParams() - Checks and copies the command line parameters.
##############################################################################

sub ParseCmdLineParams
{
    # Global options:
    $Glob->{'CFG'}->{'GLOBAL'}->{'VERBOSE'}         = $Opts{'v'}    ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'VERBOSE'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'ASSUME_YES'}      = $Opts{'y'}    ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'ASSUME_YES'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'BATCH_MODE'}      = $Opts{'b'}    ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'BATCH_MODE'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'USE_DEFAULT'}     = $Opts{'d'}    ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'USE_DEFAULT'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'}       = $Opts{'prod'} ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'SCRATCH'}         = $Opts{'s'}    ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'SCRATCH'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'NO_VC_NET'}       = $Opts{'nvc'}  ? 1 : ($Glob->{'CFG'}->{'GLOBAL'}->{'NO_VC_NET'} ? 1 : 0);
    $Glob->{'CFG'}->{'GLOBAL'}->{'IPROFILE'}        = $Opts{'no_profile'} ? 0 : 1;
    $Glob->{'CFG'}->{'GLOBAL'}->{'PLATFORM'}        = lc($Opts{'platform'}) if ($Opts{'platform'});

    # Release specific options:
    $Glob->{'CFG'}->{'RELEASE'}->{'OWN'}            = $Opts{'own'}      ? $Opts{'own'} : ($Glob->{'CFG'}->{'RELEASE'}->{'OWN'} ? $Glob->{'CFG'}->{'RELEASE'}->{'OWN'} : "");
    $Glob->{'CFG'}->{'RELEASE'}->{'APO_RELEASE'}    = $Opts{'rel'}      ? $Opts{'rel'} : ($Glob->{'CFG'}->{'RELEASE'}->{'APO_RELEASE'} ? $Glob->{'CFG'}->{'RELEASE'}->{'APO_RELEASE'} : "");
    $Glob->{'CFG'}->{'RELEASE'}->{'LC_RELEASE'}     = $Opts{'lc'}       ? $Opts{'lc'}  : ($Glob->{'CFG'}->{'RELEASE'}->{'LC_RELEASE'} ? $Glob->{'CFG'}->{'RELEASE'}->{'LC_RELEASE'} : 0);
    $Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'}     = ($Opts{'cl'} > 0) ? $Opts{'cl'}  : ($Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'} ? $Glob->{'CFG'}->{'RELEASE'}->{'CHANGELIST'} : 0);

    # Perforce specific options:
    $Glob->{'CFG'}->{'PERFORCE'}->{'USER'}          = $Opts{'p4u'} ? $Opts{'p4u'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'USER'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'USER'} : "");
    $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'}        = $Opts{'p4c'} ? $Opts{'p4c'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT'} : "");
    $Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'}          = $Opts{'p4r'} ? $Opts{'p4r'} : ($Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'ROOT'} : "");
    $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'}   = $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'SERVER_PATH'} : "";
    $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'}   = $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'} ? $Glob->{'CFG'}->{'PERFORCE'}->{'CLIENT_PATH'} : "";
    $Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'}        = $Opts{'rs'}  ? 1 : ($Glob->{'CFG'}->{'PERFORCE'}->{'RESYNC'} ? 1 : 0);

    # Actions to be performed:
    $Glob->{'CFG'}->{'ACTIONS'}->{'REMOVE'}         = $Opts{'rm'} ? 1 : ($Glob->{'CFG'}->{'ACTIONS'}->{'REMOVE'} ? 1 : 0);
    $Glob->{'CFG'}->{'ACTIONS'}->{'CLEAR'}          = $Opts{'c'}  ? 1 : ($Glob->{'CFG'}->{'ACTIONS'}->{'CLEAR'} ? 1 : 0);
    $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'}         = $Opts{'m'}  ? $Opts{'m'} : ($Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} ? $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} : "");
    $Glob->{'CFG'}->{'ACTIONS'}->{'MAKE'}           = ($Glob->{'CFG'}->{'GLOBAL'}->{'PROD_MAKE'} || $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'}) ? 1 : 0;
	
    if ($Glob->{'CFG'}->{'ACTIONS'}->{'PROD_MAKE'} && $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'})
    {
        print "WARNING! Setting a specific target in productive mode is prohibited.\nThe target '$Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'}' will be ignored.\n\n";
        $Glob->{'CFG'}->{'ACTIONS'}->{'TARGET'} = "";
    }
}

##############################################################################
# Usage() - Shows the program usage and exits the script.
##############################################################################

sub Usage
{
    print <<USAGE_END;

Usage:  LCAEnv [-(b|batch)] [-cfg <CONFIG_FILE>] [-cl <CHANGELIST>]
               [-(c|clear)] [-(d|default)] [-(?|h|help)] [-lc <LC_VERSION>]
               [-m <MAKE_TARGET>] [-no_profile] [-nvc] [-own <OWN_DIR>] 
			   [-p4c <P4CLIENT>] [-p4r <P4CLIENT_ROOT>] [-p4u <P4USER>] 
			   [-prod] [-rel <RELEASE>] [-(rm|remove)] [-(rs|resync)] 
			   [-(s|scratch)] [-(v|verbose)] [-(y|yes)]

Where:
        -b or -batch
                Starts the program in batch mode. This means that no
                interactions of a user are allowed - any missing
                parameter / ambigouity will lead to an error. (See *Options)

        -cfg <CONFIG_FILE>
                Loads program parameters from a previously generated / edited
                configuration file. You can still give command line parameters
                which would then overload the option loaded from the file.

        -cl <CHANGELIST>
                Here you can set a specific changelist to sync to. If not set
                or set to 0, the default change list (latest) will be used.

        -c or -clear
                With this option you can automatically clear the generated
                environment again after the make has finished.

        -d or -default
                Use default values, if an option is not set. NOTE: If the
                program is running in batch mode, this option will be ignored.

        -h, -? or -help
                Displays this help. For more detailed information, please use
                the command 'perldoc LCEnv.pl'.

        -lc <LC_VERSION>
                Sets the liveCache version to be used together with the given
                APO release. If not set, the default for the given APO release
                will be used.

        -m <MAKE_TARGET>
                Initiates the make of a specific target after creating the
                environment. If '-prod' is also set, this option will be
                ignored.

		-no_profile
				Prevents LCAEnv from creating a new iprofile.
				
        -nvc
                With this option set, the environment will not try to use the
                VC .NET compiler.

        -own <OWN_DIR> *
                Sets the root directory for the new environment.

        -p4c <P4CLIENT> *
                The Perforce client you want to use. If it doesn't exist, it
                will be created.

        -p4r <P4CLIENT_ROOT> *
                This is the root path to be assigned to the Perforce client.
                Please note, that the OWN directory has to be a sub-directory
                of this path.

        -p4u <P4USER> *
                The Perforce user you want to use. Please note, that this
                user has to have access rights to the APO release tree. These
                rights are normally granted by the Perforce administrators.

        -rel <RELEASE> *
                Sets the APO release you want to use.

        -rm or -remove
                This option causes the program simply to remove the complete
                environment again. It's somehow like the '-c' option without
                even trying to generate the environment at the beginning.

        -rs or -resync
                With this option, you can force Perforce to re-sync all
                sources.

        -s or -scratch
                This option causes the compiler to initiate a scratch make
                by inserting the '-U' option into make commands.

        -v or -verbose
                This option causes the program to produce a more detailed
                output.

        -y or -yes
                With this option set, the program answers all 'yes/no'
                questions automatically with 'yes'.

(*) These options are mandatory. If any of these options are missing, you'll
    be asked for. If the program is in batch mode and any of these parameters
    are missing, it will throw an error and terminate its execution!

For more detailed information feel free to take a look into the documentation
embedded in the program. You can do this by calling the Perl inline
documentation tool 'perlpod'. If you have problems finding the perl script:
'perldoc \$TOOL/bin/LCAEnv.pl' (Unix) or 'perldoc %TOOL%\\bin\\LCAEnv.pl' (Win)
should do it.

USAGE_END

    exit(1);
}

__END__

=head1 NAME

LCAEnv - LCApps environment creation / deletion tool.

Version: v1.05 (2003-05-27)

=head1 SYNOPSIS

LCAEnv [-(b|batch)] [-cfg <CONFIG_FILE>] [-cl <CHANGELIST>]
       [-(c|clear)] [-(d|default)] [-(?|h|help)] [-lc <LC_VERSION>]
       [-m <MAKE_TARGET>] [-nvc] [-own <OWN_DIR>] [-p4c <P4CLIENT>]
       [-p4r <P4CLIENT_ROOT>] [-p4u <P4USER>] [-prod] [-rel <RELEASE>]
       [-(rm|remove)] [-(rs|resync)] [-(s|scratch)] [-(v|verbose)]
       [-(y|yes)]

To see a detailed description of all commend line options, see "COMMAND LINE
OPTIONS" section.

=head1 DESCRIPTION

With this tool you are able to create a complete LCApps development tree in
one single step. It will set the correct environment, sync all sources into
the target directory and even start the make process, if you want it to.
Additionally, you can specify a liveCache version other than the default
version to be installed together with the APO environment. Finally, you
can automatically clean-up all directories LCAEnv created.

=head2 COMMAND LINE OPTIONS

NOTE: All options marked by an asterix ("*") are mandatory. If any of these
options are missing, you'll be asked for. If the program is in batch mode
and any of these parameters are missing, it will throw an error and terminate
its execution!

=over 4

=item -b or -batch

    Starts the program in batch mode. This means that no
    interactions of a user are allowed - any missing
    parameter / ambigouity will lead to an error. (See *Options)

=item -cfg <CONFIG_FILE>

    Loads program parameters from a previously generated / edited
    configuration file. You can still give command line parameters
    which would then overload the option loaded from the file.

=item -cl <CHANGELIST>

    Here you can set a specific changelist to sync to. If not set
    or set to 0, the default change list (latest) will be used.

=item -c or -clear

    With this option you can automatically clear the generated
    environment again after the make has finished.

=item -d or -default

    Use default values, if an option is not set. NOTE: If the
    program is running in batch mode, this option will be ignored.

=item -h, -? or -help

    Displays this help. For more detailed information, please use
    the command 'perldoc LCEnv.pl'.

=item -lc <LC_VERSION>

    Sets the liveCache version to be used together with the given
    APO release. If not set, the default for the given APO release
    will be used.

=item -m <MAKE_TARGET>

    Initiates the make of a specific target after creating the
    environment.

=item -nvc

    With this option set, the environment will not try to use the
    VC .NET compiler.

=item -own <OWN_DIR> *

    Sets the root directory for the new environment.

=item -p4c <P4CLIENT> *

    The Perforce client you want to use. If it doesn't exist, it
    will be created.

=item -p4r <P4CLIENT_ROOT> *

    This is the root path to be assigned to the Perforce client.
    Please note, that the OWN directory has to be a sub-directory
    of this path.

=item -p4u <P4USER> *

    The Perforce user you want to use. Please note, that this
    user has to have access rights to the APO release tree. These
    rights are normally granted by the Perforce administrators.

=item -rel <RELEASE> *

    Sets the APO release you want to use.

=item -rm or -remove

    This option causes the program simply to remove the complete
    environment again. It's somehow like the '-c' option without
    even trying to generate the environment at the beginning.

=item -rs or -resync

    With this option, you can force Perforce to re-sync all
    sources.

=item -s or -scratch

    This option causes the compiler to initiate a scratch make
    by inserting the '-U' option into make commands.

=item -v or -verbose

    This option causes the program to produce a more detailed
    output.

=item -y or -yes

    With this option set, the program answers all 'yes/no'
    questions automatically with 'yes'.

=back

=head2 CONFIG FILE FORMAT

The format of the config files is quite simple. The following rules apply:

=over 4

=item 1. Each valid line contains exactly one setting or a section definition.

=item 2. Each valid setting line has the following format: <KEY>=<VALUE>

=item 3. Each valid section definition has the format: [<SECTION_NAME>]

=item 4. Each line NOT having one of the formats given above is NOT valid and
will therefore be treated as comment.

=back

=head3 KNOWN KEYS

=over 4

=item [RELEASE]

    This section contains all information connected to the release used.

=over 4

=item OWN

    The $OWN directory to be used. Equals the '-own' command line option.

=item LC_RELEASE

    This descibes the liveCache release to be used together with this APO make.
    This option is the equivalent to the '-lc' command line option. If you
    enter a zero here, the default LC version will be used.

=item APO_RELEASE

    This is the APO release to be used. This option equals the '-rel' command
    line option.

=item CHANGELIST

    This is the changelist which is used to sync all APO sources. If this
    option is set to zero, the latest changelist will be used. This option
    equals the '-cl' command line option.

=back

=item [GLOBAL]

    This section contains 'general' values which steer the overall program
    behaviour.

=over 4

=item BATCH_MODE

    If set to 1, the program is started in batch mode. This means that any
    ambiguities or user input is prohibited. This also means that any option
    marked by an asterix above has to be set.

=item VERBOSE

    This option causes the program to give a more detailed output on most
    actions. It doesn't change the program behaviour but simply gives you
    lots of output.

=item USE_DEFAULT

    If set to 1, this option will cause the program to use default parameters
    whenever it is possible. NOTE: It could be dangerous to use this option
    together with PROD_MAKE / BATCH_MODE.

=item ASSUME_YES

    With this option set to 1, any (annoying) yes/no question will be skipped.

=item NO_VC_NET

    Is this option is turned on (1), the program will not try to load the
    VC.NET variables while environment creation.

=item SCRATCH

    If this option is set to 1, a scratch make will be initiated.

=back

=item [PERFORCE]

    These options configure all settings related to Perforce. Please note that
    options beginning with an underscore "_" are generated automatically and
    for internal use, only. PLEASE DO NOT CHANGE THEM!

=over 4

=item _VIEW_REMOVED

    Indicates a view that had to be removed while Perforce client creation.
    *DO NOT CHANGE*

=item ROOT

    This describes the Perforce client root to be used.

=item USER

    This option contains the Perforce user to be used and will be transferred
    to the P4USER environment variable at run-time.

=item _USER_CREATED

    Indicates that a user has been created while setting up Perforce. Any user
    shown here will be removed, if the environment is removed again.
    *DO NOT CHANGE*

=item CLIENT_PATH

    This option contains the client-side path for the Perforce branch. This
    value has been generated automatically. PLEASE JUST CHANGE THIS VALUE,
    IF YOU KNOW EXACTLY WHAT YOU'RE DOING.

=item _ROOT_ADDED

    This value is generated automatically and shows any Perforce client root
    that has been created during Perforce set-up. This root path will be
    removed again, if the branch is de-installed again.
    *DO NOT CHANGE*

=item _ROOT_REMOVED

    This value eventually shows a Perforce client root path which had to be
    removed during Perforce set-up. If the actual branch is removed again,
    this value would be restored as Perforce client root.
    *DO NOT CHANGE*

=item RESYNC

    Is this option is set to 1, a complete re-sync of all sources will be
    initiated, regardless whether sources a up-to-date or not. This may
    be usefull, if Perforce is confused once again...

=item _CLIENT_CREATED

    This flag indicates whether a Perforce client had to be created during
    installation or not. If this flag is set to 1, the actually used
    perforce user will be removed on branch de-installation.
    *DO NOT CHANGE*

=item SERVER_PATH

    This option contains the server-side path of Perforce branch. This value
    has been generated automatically - DO NOT CHANGE UNLESS YOU REALLY KNOW
    WHAT YOU'RE DOING.

=item _VIEW_ADDED

    This option contains the Perforce client view that had to be created
    during Perforce set-up. If none had to be created, this field is empty.
    *DO NOT CHANGE*

=item CLIENT

    This option describes the Perforce client to use. Normally, this should
    be something like your hostname. Since client creation is not limited,
    you can use virtually any name you want.

=item PORT

    This option contains the Perforce port to be used. This option is normally
    hard-wired to perforce3011 AND SHOULD NOT BE CHANGED UNLESS THE PERFORCE
    SERVER MOVED.

=back

=item [ACTIONS]

    This section contains options for the actions to be performed by the
    program.

=over 4

=item MAKE

    If this option is set to 1, a make will be performed after sync. This
    option should not be changed since it is derrived automatically from
    '-m' option.
    NOTE: If you choose an APO release older than 4.0, this value will
    ALWAYS be zero for compatibility reasons.

=item CLEAR

    With this option set to 1, the program will clear (remove) everything
    again after the make process has finished. This option my be useful
    together with piping any output into a log-file.

=item REMOVE

    If this option is set, MOST OTHER OPTIONS WILL BE IGNORED and the complete
    environment will simply be removed. No sync - no make - just killing.

=item TARGET

    Here you can give a target name to be made after sync.

=back

=back

=head1 EXAMPLES

    Here are some examples to make things even a bit more clear.

=head2 EXAMPLE CALLS

=over 4

=item LCAEnv

    Simply, enjoy the guided tour though all options. ;-)

=item LCAEnv -cfg c:\meine_configs\APO41HOT.cfg

    Use a predefined configuration file to generate the environment.
    (This file must be created, first by calling LCAEnv.)
    NOTE: Each time you use LCAEnv a cfg file is created in the $OWN directory
    after the sync process has been completed. These files can be copied and
    used later, i.e. to generate templates for other releases.

=item LCAEnv -rm -own c:\apomake\4.1_HOT

    Removes the complete branch again.

=item LCAEnv -rm -cfg c:\apomake\4.1_HOT\LCAEnv.cfg

    Does the same...

=back

=head2 EXAMPLE CONFIG FILE (for Windows)

File "c:\apomake\dev\LCAEnv.cfg":

        # File generated by SimpleCfg v1.00
        # Generation time: Mon May 26 15:46:27 2003

        [RELEASE]
        OWN=c:\apomake\dev
        LC_RELEASE=0
        APO_RELEASE=dev
        CHANGELIST=314128

        [GLOBAL]
        BATCH_MODE=0
        VERBOSE=1
        PROD_MAKE=0
        USE_DEFAULT=0
        ASSUME_YES=0
        NO_VC_NET=1
        SCRATCH=0

        [PERFORCE]
        _VIEW_REMOVED=
        ROOT=c:\apomake
        USER=d010101
        _USER_CREATED=0
        CLIENT_PATH=//P01010/dev/sys/src
        _ROOT_ADDED=
        RESYNC=0
        _ROOT_REMOVED=
        _CLIENT_CREATED=0
        SERVER_PATH=//apo_tc/lcapps/dev
        _VIEW_ADDED=
        CLIENT=P01010
        PORT=perforce3011.wdf.sap.corp:3011

        [ACTIONS]
        MAKE=0
        CLEAR=0
        REMOVE=0
        TARGET=


