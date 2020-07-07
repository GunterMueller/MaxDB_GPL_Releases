#! /usr/bin/perl
#
#    LCAEnv.pm - Creates and syncs a complete APO/LC make environment.
#
#    @(#)LCEnv     2003-05-12
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
@EXPORT_OK  = qw(LCACheckEnv LCACreateEnv LCAExecuteEnv LCARemEnv LCACheckP4Env LCACheckAPORel LCACheckLCVer LCACheckP4ServerPath LCACheckOwn LCACheckP4ClientPath LCACheckAPOChangeList LCACreateOwn LCACreateP4User LCACreateP4Client LCAP4SyncEnv LCAMakeEnv LCADoMake LCARemP4Files LCARemP4Client LCARemP4User LCARemOwn);

use strict;
use APOSettings;
use Tool::RequestInput;
use Perforce::P4Request;
use Perforce::P4Info;
use Perforce::P4Admin;
use Perforce::P4Sync;
use IO::CurrentDir;
use IO::FilePath;
#use IO::DelTree;
use File::Path;

my %machine_defaults;

$machine_defaults{'ntintel'}{'CALL'}="C:\\Makeres\\VS98\\VC98\\bin\\vcvars32.bat";
$machine_defaults{'ntintel'}{'TOOLSHELL'} = "d:\\depot\\tools\\gen\\ntintel\\OpenSource\\perl\\5.6.1\\bin\\perl.exe";
$machine_defaults{'ntintel'}{'LCAPPS_SRC_DIR'} = "\\\\<src_host>\\depot\\bas\\LCAPPS";
$machine_defaults{'ntintel'}{'LC_POOL_ROOT'} = "\\\\production\\SCM\\MaxDB\\LC_POOL\\ntintel";

$machine_defaults{'ntia64'}{'CALL'}="\"%ProgramFiles%\\Microsoft SDK\\SetWin64.bat\"";
$machine_defaults{'ntia64'}{'TOOLSHELL'} = "d:\\depot\\tools\\gen\\ntintel\\OpenSource\\perl\\5.6.1\\bin\\perl.exe";
$machine_defaults{'ntia64'}{'BIT64'} = "1";
$machine_defaults{'ntia64'}{'LC_POOL_ROOT'} = "\\\\production\\SCM\\MaxDB\\LC_POOL\\ntia64";
$machine_defaults{'ntia64'}{'NO_VC_NET'} = "1";
$machine_defaults{'ntia64'}{'LCAPPS_SRC_DIR'} = "\\\\<src_host>\\depot\\bas\\LCAPPS";

$machine_defaults{'ntamd64'}{'LC_POOL_ROOT'} = "\\\\production\\SCM\\MaxDB\\LC_POOL\\ntamd64";
$machine_defaults{'ntamd64'}{'CALL'}="\"%ProgramFiles%\\Microsoft Platform SDK\\sdkenv.bat\"";
$machine_defaults{'ntamd64'}{'TOOLSHELL'} = "d:\\depot\\bas\\VMAKE\\Perl\\bin\\perl.exe";
$machine_defaults{'ntamd64'}{'BIT64'} = "1";
$machine_defaults{'ntamd64'}{'NO_VC_NET'} = "1";
$machine_defaults{'ntamd64'}{'LCAPPS_SRC_DIR'} = "\\\\<src_host>\\depot\\bas\\LCAPPS";

##############################################################################
# LCACheckEnv() - Checks all settings for the APO/LC environment
##############################################################################

sub LCACheckEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCACheckEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless (LCACheckP4Env          (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking Perforce settings failed!") }
    unless (LCACheckAPORel         ($href->{'RELEASE'},                      $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking APO release failed! Please check your program parameters.") }
    unless (LCACheckLCVer          ($href->{'RELEASE'},                      $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking LC release failed! Please check your program parameters.") }
    unless (LCACheckP4ServerPath   ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking Perforce server path failed!\nPlease check your APO release and Perforce settings.") }
    unless (LCACheckOwn            ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking OWN directory failed. Please check you parameters.") }
    unless (LCACheckP4ClientPath   ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking Perforce client path failed!\nPlease make sure that your OWN directory is a sub-directory of the Perforce\nroot directory.") }
    unless (LCACheckAPOChangeList  ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACheckEnv", "Checking latest changelist for release '$href->{'RELEASE'}->{'APO_RELEASE'}' failed!\nPlease check Perforce (especially the P4USER) and release settings.") }
}

##############################################################################
# LCACreateEnv() - Creates a previously checked environment.
##############################################################################

sub LCACreateEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCACreateEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless (LCACreateOwn           ($href->{'RELEASE'},                      $href->{'GLOBAL'})) { _throw_error("LCACreateEnv", "Creating OWN directory failed! Please make sure that\n'$href->{'RELEASE'}->{'OWN'}'\nis a valid path.") }
    unless (LCACreateP4User        (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACreateEnv", "Creating Perforce user '$href->{'PERFORCE'}->{'USER'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCACreateP4Client      (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACreateEnv", "Creating / updating Perforce client '$href->{'PERFORCE'}->{'CLIENT'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCAP4SyncEnv           ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACreateEnv", "Sync of sources failed! Please check your Perforce and release settings.") }
    unless (LCAMakeEnv             ($href->{'RELEASE'}, $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCACreateEnv", "Making environment profiles failed!") }

}

##############################################################################
# LCAExecuteEnv() - Executes a previously created environment.
##############################################################################

sub LCAExecuteEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCAExecuteEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless (LCADoMake              ($href->{'RELEASE'}, $href->{'ACTIONS'}, $href->{'GLOBAL'})) { _throw_error("LCAExecuteEnv", "Creating / executing batch file failed! Please check your settings\nand write permissions / quotas in your OWN directory.") }
}

##############################################################################
# LCARemEnv() - Removes the complete environment.
##############################################################################

sub LCARemEnv
{
    my $href = shift;

    unless (ref($href)) { _throw_error("LCARemEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless (LCARemP4Files        (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCARemEnv", "Removing Perforce administrated files failed!\nPlease check your Perforce settings.") }
    unless (LCARemP4Client       (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCARemEnv", "Removing Perforce client '$href->{'PERFORCE'}->{'CLIENT'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCARemP4User         (                    $href->{'PERFORCE'}, $href->{'GLOBAL'})) { _throw_error("LCARemEnv", "Removing Perforce user '$href->{'PERFORCE'}->{'USER'}' failed!\nPlease check your Perforce settings and connection.") }
    unless (LCARemOwn            ($href->{'RELEASE'}                     , $href->{'GLOBAL'})) { _throw_error("LCARemEnv", "Removing of Own directory '$href->{'RELEASE'}->{'OWN'}' failed!\nThis could be caused by the 'rm' system command called.") }
}

##############################################################################
# LCACheckP4Env() - Checks the Perforce settings to be used.
##############################################################################

sub LCACheckP4Env
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCACheckP4Env", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckP4Env", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    $p4env->{'PORT'}   = "perforce3011.wdf.sap.corp:3011" unless ($p4env->{'PORT'});

    $p4env->{'USER'}   = ($p4env->{'USER'}    ? $p4env->{'USER'}     : (($ENV{'P4USER'} && $globenv->{'USE_DEFAULT'}) ? $ENV{'P4USER'}      : ($globenv->{'BATCH_MODE'} ? _throw_error("LCACheckP4Env", "Perforce user missing!")   : P4RequestUser($globenv->{'USE_DEFAULT'}))));
    $p4env->{'CLIENT'} = ($p4env->{'CLIENT'}  ? $p4env->{'CLIENT'}   : (($ENV{'P4CLIENT'} && $globenv->{'USE_DEFAULT'}) ? $ENV{'P4CLIENT'}  : ($globenv->{'BATCH_MODE'} ? _throw_error("LCACheckP4Env", "Perforce client missing!") : P4RequestClient($globenv->{'USE_DEFAULT'}))));
    $p4env->{'ROOT'}   = ($p4env->{'ROOT'}    ? $p4env->{'ROOT'}     : ($globenv->{'BATCH_MODE'} ? _throw_error("LCACheckP4Env", "Perforce client root path is missing!") : P4RequestRoot($p4env->{'PORT'}, $p4env->{'CLIENT'}, $globenv->{'USE_DEFAULT'})));

    $ENV{'P4PORT'}     =  $p4env->{'PORT'};

    return $p4env;

}

##############################################################################
# LCACheckAPORel() - Checks / sets the APO release to use.
##############################################################################

sub LCACheckAPORel
{
    my ($relenv, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckAPORel", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckAPORel", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    my $count = 1;
    my $sel   = 0;
    my @apokeys;

    print "\nChecking APO release.." if ($globenv->{'VERBOSE'});

    if ($relenv->{'APO_RELEASE'})
    {
        unless ($APOSettings::Depots{$relenv->{'APO_RELEASE'}})
        {
            print "WARNING! Unknown APO release '$relenv->{'APO_RELEASE'}' ignored.\n";
            $relenv->{'APO_RELEASE'} = undef;
        }
    }

    if ($relenv->{'APO_RELEASE'})
    {
        print "..OK\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        print "..FAILED!\n" if ($globenv->{'VERBOSE'});

        if ($globenv->{'BATCH_MODE'}) { return 0 }

        print "\nAvailable APO releases:\n\n";

        my $count = 1;

        foreach (reverse sort keys %APOSettings::Depots)
        {
            print "[$count] = $_\n";
            $apokeys[$count] = $_;
            $count ++;
        }

        print "\n";

        while (($sel < 1) || ($sel >= $count))
        {
            $sel = RequestInput("Please select one : ", "", "[0-9]+", "Input incorrect. Please just use numbers.\n");
            if (($sel < 1) || ($sel >= $count))
            {
                print "Input incorrect. Pleae just use numbers between 1 and " . ($count - 1) . "\n";
            }
        }

        $relenv->{'APO_RELEASE'} = $apokeys[$sel];
    }

    return 1;
}

##############################################################################
# LCACheckLCVer() - Checks the liveCache Version to be used.
##############################################################################

sub LCACheckLCVer
{
    my ($relenv, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckLCVer", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckLCVer", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nChecking LC version.." if ($globenv->{'VERBOSE'});

    if ($relenv->{'LC_RELEASE'} =~ /^\S+$/)
    {
        print "..OK\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        print "..UNSET (using default)!\n" if ($globenv->{'VERBOSE'});

        if ($relenv->{'LC_RELEASE'})
        {
            print "\nWARNING: Incorrect LC version was ignored.\n";
        }

        $relenv->{'LC_RELEASE'} = 0;
    }

    return 1;
}

##############################################################################
# LCACheckOwn() - Checks the Own directory for the APO release.
##############################################################################

sub LCACheckOwn
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckP4ServerPath", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCACheckP4ServerPath", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckP4ServerPath", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nChecking Own directory.." if ($globenv->{'VERBOSE'});

    if ($relenv->{'OWN'})
    {
        print "..OK\n" if ($globenv->{'VERVOSE'});
    }
    else
    {
        print "..FAILED!\n" if ($globenv->{'VERBOSE'});

        my $curdir =  GetCurrentDir();
        my $reldef =  $p4env->{'SERVER_PATH'};
        $reldef    =~ s/^\Q$APOSettings::Depots{$relenv->{'APO_RELEASE'}}\E/$p4env->{'ROOT'}/;
        $reldef    =  FixPathSep($reldef);

        if ($globenv->{'BATCH_MODE'})
        {
            print "In batch mode, you have to give an Own directory!\n";
            return 0;
        }
        elsif ($globenv->{'USE_DEFAULT'})
        {
            print " -> Using default value '$reldef'.\n" if ($globenv->{'VERBOSE'});
            $relenv->{'OWN'} = $reldef;
        }
        else
        {
            print "\nPlease select which \$Own directory you want to use:\n\n";

            print "[c] = use the current directory ($curdir)\n";
            print "[p] = use P4 client root ($p4env->{'ROOT'})\n";
            print "[r] = use releases default directory ($reldef), default\n";
            print "[s] = select a different client name manually\n\n";

            my $sel = RequestInput("Please select: ", "r", "[cCpPrRsS]", "Input incorrect. Please, just use [c, p, r or s].\n");

            my $pathsep = ($^O =~ /MSWin32/i) ? "\\" : "/";

            if ($sel =~ /^c$/i)     { $relenv->{'OWN'} = $curdir }
            elsif ($sel =~ /^p$/i)  { $relenv->{'OWN'} = $p4env->{'ROOT'} }
            elsif ($sel =~ /^r$/i)  { $relenv->{'OWN'} = $reldef }
            else                    { $relenv->{'OWN'} = JoinPath($p4env->{'ROOT'}, RequestInput("Own directory: $p4env->{'ROOT'}$pathsep", "", ".+", "Input incorrect. Please, just use normal characters.\n")) }
        }
    }

    return 1;
}

##############################################################################
# LCACheckP4ServerPath() - Checks the server path of the given APO release.
##############################################################################

sub LCACheckP4ServerPath
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckP4ServerPath", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCACheckP4ServerPath", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckP4ServerPath", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nChecking Perforce server path.." if ($globenv->{'VERBOSE'});

    if ($APOSettings::Depots{$relenv->{'APO_RELEASE'}})
    {
        $p4env->{'SERVER_PATH'} = $APOSettings::Depots{$relenv->{'APO_RELEASE'}} . "/" . $relenv->{'APO_RELEASE'};
        print "..OK\n" if ($globenv->{'VERBOSE'});
        return 1;
    }
    else
    {
        $p4env->{'SERVER_PATH'} = undef;
        print "..FAILED!\n" if ($globenv->{'VERBOSE'});
        return 0;
    }
}

##############################################################################
# LCACheckP4ClientPath() - Checks the client path of the given APO release.
##############################################################################

sub LCACheckP4ClientPath
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckP4ClientPath", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCACheckP4ClientPath", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckP4ClientPath", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nChecking Perforce client path.." if ($globenv->{'VERBOSE'});

    $p4env->{'CLIENT_PATH'} = $relenv->{'OWN'} . ($globenv->{'PROD_MAKE'} ? "/SRC/sys/src" : "/sys/src");
    $p4env->{'CLIENT_PATH'} =~ s/\Q$p4env->{'ROOT'}\E/\/\/$p4env->{'CLIENT'}/i;
    $p4env->{'CLIENT_PATH'} =~ tr/\\/\//;

    if ($p4env->{'CLIENT_PATH'} =~ /\Q$p4env->{'CLIENT'}\E/i)
    {
        print "..OK\n" if ($globenv->{'VERBOSE'});
        return 1;
    }
    else
    {
        $p4env->{'CLIENT_PATH'} = undef;
        print "..FAILED!\n" if ($globenv->{'VERBOSE'});
        return 0;
    }
}

##############################################################################
# LCACheckAPOChangeList () - Checks / sets the change list for an APO release.
##############################################################################

sub LCACheckAPOChangeList
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACheckAPOChangeList", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCACheckAPOChangeList", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACheckAPOChangeList", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nChecking latest change list for release '$relenv->{'APO_RELEASE'}'.." if ($globenv->{'VERBOSE'});

    if ($relenv->{'CHANGELIST'} > 0)
    {
        print "..OK\n" if ($globenv->{'VERBOSE'});
        return 1;
    }

    $relenv->{'CHANGELIST'} = P4GetChangeList($p4env->{'PORT'}, $p4env->{'USER'}, "$p4env->{'SERVER_PATH'}/...");

    if ($relenv->{'CHANGELIST'} > 0)
    {
        print "..OK\n" if ($globenv->{'VERBOSE'});
        print " -> Change list number has been set to '$relenv->{'CHANGELIST'}'.\n" if ($globenv->{'VERBOSE'});
        return 1;
    }
    else
    {
        print "..FAILED!\n";
        return 0;
    }
}

##############################################################################
# LCACreateOwn() - Creates the Own directory, if needed.
##############################################################################

sub LCACreateOwn
{
    my ($relenv, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCACreateOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACreateOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if (-d $relenv->{'OWN'})
    {
        print "\nOWN directory already exists.\n\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        print "\nCreating OWN directory.." if ($globenv->{'VERBOSE'});

        if (mkpath($relenv->{'OWN'}, 0, 0777))
        {
            print "..OK\n" if ($globenv->{'VERBOSE'});
        }
        else
        {
            print "..FAILED!\n" if ($globenv->{'VERBOSE'});
            return 0;
        }
        print "\nCreating OWN/SRC directory.." if ($globenv->{'VERBOSE'});

        if (mkpath("$relenv->{'OWN'}/SRC", 0, 0777))
        {
            print "..OK\n" if ($globenv->{'VERBOSE'});
        }
        else
        {
            print "..FAILED!\n" if ($globenv->{'VERBOSE'});
            return 0;
        }
    }

    return 1;
}

##############################################################################
# LCACreateP4User() - Creates / changes the P4 user, if needed.
##############################################################################

sub LCACreateP4User
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCACreateP4User", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACreateP4User", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if (P4UserExists($p4env->{'PORT'}, $p4env->{'USER'}))
    {
        print "Perforce user '$p4env->{'USER'}' already exists.\n\n" if ($globenv->{'VERBOSE'});
        $p4env->{'_USER_CREATED'} = 0;
    }
    else
    {
        print "Creating Perforce user '$p4env->{'USER'}'...\n\n" if ($globenv->{'VERBOSE'});
        unless (P4SetUser($p4env->{'PORT'}, $p4env->{'USER'}, "$p4env->{'USER'} (automatically created by LCEnv)"))
        {
            return 0;
        }
        $p4env->{'_USER_CREATED'} = 1;
    }
    return 1;
}

##############################################################################
# LCACreateP4Client() - Creates / changes the P4 client, if needed.
##############################################################################

sub LCACreateP4Client
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCACreateP4Client", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCACreateP4Client", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if (P4ClientExists($p4env->{'PORT'}, $p4env->{'CLIENT'}))
    {
        print "Perforce client '$p4env->{'CLIENT'}' already exists.\n\n" if ($globenv->{'VERBOSE'});

        unless ($globenv->{'ASSUME_YES'} || $globenv->{'BATCH_MODE'} || (RequestInput("Overwrite Perforce client with new values (Y/n)?", "y", "[yYnN]", "Input incorrect. Please, just use [y or n].\n") =~ /^y$/i))
        {
            print "Update of Perforce client '$p4env->{'CLIENT'}' aborted.\nExiting...\n";
            exit(0);
        }

        print "Updating Perforce client '$p4env->{'CLIENT'}'...\n" if ($globenv->{'VERBOSE'});
        my $c_info    = P4GetClientInfo($p4env->{'PORT'}, $p4env->{'CLIENT'});
        my @views     = grep {!/^\Q$p4env->{'SERVER_PATH'}\E/} @{$c_info->{'View'}};
        my @old_views = grep {/^\Q$p4env->{'SERVER_PATH'}\E/} @{$c_info->{'View'}};
        my $view_path = "$p4env->{'SERVER_PATH'}/... $p4env->{'CLIENT_PATH'}/...";

        if (@old_views)
        {
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

        push @views, $view_path;

        unless (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $p4env->{'ROOT'}, undef, undef, undef, undef, \@views))
        {
            return 0;
        }

        $p4env->{'_CLIENT_CREATED'} = 0;
    }
    else
    {
        my @descs = ("Automatically created by LCEnv");
        my @views = ("$p4env->{'SERVER_PATH'}/... $p4env->{'CLIENT_PATH'}/...");
        print "Creating Perforce client '$p4env->{'CLIENT'}'...\n" if ($globenv->{'VERBOSE'});
        unless (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $p4env->{'ROOT'}, $p4env->{'USER'}, undef, undef, \@descs, \@views))
        {
            return 0;
        }

        $p4env->{'_CLIENT_CREATED'} = 1;
    }

    return 1;
}

##############################################################################
# LCAP4SyncEnv() - Syncs the environment from the Perforce server.
##############################################################################

sub LCAP4SyncEnv
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCAP4SyncEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCAP4SyncEnv", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCAP4SyncEnv", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nSync files " . ($p4env->{'RESYNC'} ? "(complete re-sync).." : "(incremental)..");
    print ".\n" if ($globenv->{'VERBOSE'});

    if (P4Sync($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'CLIENT'}, $p4env->{'SERVER_PATH'}, $relenv->{'CHANGELIST'}, $p4env->{'RESYNC'}, $globenv->{'VERBOSE'}))
    {
        print ($globenv->{'VERBOSE'} ? "Sync OK.\n" : "..OK\n");
    }
    else
    {
        print ($globenv->{'VERBOSE'} ? "Sync finished with warnings.\n" : "..OK (Warnings)\n");
    }

    if (open (F_OUT, "> $relenv->{'OWN'}/changelist.log"))
    {
        print F_OUT "$relenv->{'CHANGELIST'}";
    }
    close (F_OUT);

    return 1;
}

##############################################################################
# LCAMakeEnv() - Sets the current environment.
##############################################################################

sub LCAMakeEnv
{
    my ($relenv, $p4env, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCAMakeEnv", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($p4env))   { _throw_error("LCAMakeEnv", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCAMakeEnv", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "\nGenerating environment:\n\n";

    local @ARGV = $relenv->{'LC_RELEASE'}==0 ? ("-release", uc($relenv->{'APO_RELEASE'}), "-own", $relenv->{'OWN'}, "-apo_src", ($globenv->{'PROD_MAKE'} ? "$relenv->{'OWN'}/SRC" : $relenv->{'OWN'})) : ("-release", uc($relenv->{'APO_RELEASE'}), "-own", $relenv->{'OWN'}, "-apo_src", ($globenv->{'PROD_MAKE'} ? "$relenv->{'OWN'}/SRC" : $relenv->{'OWN'}), "-lcversion", "$relenv->{'LC_RELEASE'}");
    do "$ENV{TOOL}/bin/vmakeEnv.pl";
    if ( $@ ) { return 0 }

    if ($^O =~ /MSWin32/i)
    {
        open ENV_OUT, ">$relenv->{'OWN'}\\LCAEnv.bat" or return 0;
        print ENV_OUT "\@echo off\n";
        if ($globenv->{'PLATFORM'}) {
	    my %mach_defs = get_default_settings($globenv->{'PLATFORM'});
            foreach my $setting (keys %mach_defs) {
                if ( $setting =~ /^CALL/i ) { print ENV_OUT "call $mach_defs{$setting}\n"; }
                else { print ENV_OUT "set $setting=$mach_defs{$setting}\n"; }
            }
        }
        print ENV_OUT "set OWN=$relenv->{'OWN'}\n";
        print ENV_OUT "set P4USER=$p4env->{'USER'}\n";
        print ENV_OUT "set P4CLIENT=$p4env->{'CLIENT'}\n";
        print ENV_OUT "set P4PORT=$p4env->{'PORT'}\n";
        print ENV_OUT "set BRANCH_INFO=$relenv->{'APO_RELEASE'}\n";
        print ENV_OUT "set DEPOT=$APOSettings::Depots{$relenv->{'APO_RELEASE'}}/$relenv->{'APO_RELEASE'}\n";
        print ENV_OUT "set FORCED_LCVERSION=$relenv->{'LC_RELEASE'}\n" if ($relenv->{'LC_RELEASE'} != 0);
        print ENV_OUT "set RELSTAT=PROD\n" if ($globenv->{'PROD_MAKE'} == 1);
        print ENV_OUT "set NO_VC_NET=1\n"  if ($globenv->{'NO_VC_NET'} == 1);
        if ($globenv->{'PROD_MAKE'}) {
            print ENV_OUT "set APO_SRC=\%OWN\%\\SRC\n";
            print ENV_OUT "call %OWN%\\SRC\\sys\\src\\iprofile.bat\n";
        } else {
            print ENV_OUT "call %OWN%\\sys\\src\\iprofile.bat\n";
        }
        close ENV_OUT or die "Can't close $relenv->{'OWN'}\\LCAEnv.bat\n$!\n";

        open ENV_IN, "<$relenv->{'OWN'}\\LCAEnv.bat" or die "Can't open $relenv->{'OWN'}\\LCAEnv.bat for reading!\n$!\n";
        while (<ENV_IN>) { print "$_"; }
        close ENV_IN or die "Can't close $relenv->{'OWN'}\\LCAEnv.bat!\n$!\n";

        print "\nOK!\n\n$relenv->{'OWN'}\\LCAEnv.bat has been generated. To generate the correct\nenvironment, please execute this script.\n\n";
    }
    else
    {
        open ENV_OUT, ">tmpout.dat" or die "Can't open 'tmpout.dat' for writing!\n$!\n";
        print ENV_OUT "P4USER=$p4env->{'USER'} export P4USER\n";
        print ENV_OUT "P4CLIENT=$p4env->{'CLIENT'} export P4CLIENT\n";
        print ENV_OUT "P4PORT=$p4env->{'PORT'} export P4PORT\n";
        print ENV_OUT "RELSTAT=PROD export RELSTAT\n" if ($globenv->{'PROD_MAKE'} == 1);
        print ENV_OUT "NO_VC_NET=1 export NO_VC_NET\n" if ($globenv->{'NO_VC_NET'} == 1);
        close ENV_OUT or die "Can't close tmpout.dat!\n$!\n";

        my $platform = vmakeEnv::getPlatform();

        vmakeEnv::printInfo("-platform", $platform);
        vmakeEnv::createProfile ("-all", "-append", "$ENV{TOOL}/profiles/APODev/iprofile.$platform", "-append", "tmpout.dat") if ($globenv->{'IPROFILE'});

        unlink ("tmpout.dat");

        print "\nOK!\n\n$relenv->{'OWN'}/.iprofile has been generated. To generate the correct\nenvironment, please execute this script.\n\n";
    }
}

##############################################################################
# LCADoMake() - Executes the make in a previously created environment.
##############################################################################

sub LCADoMake
{
    my ($relenv, $actenv, $globenv) = @_;

    unless (ref($relenv))  { _throw_error("LCADoMake", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($actenv))  { _throw_error("LCADoMake", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCADoMake", "Error calling function! Third parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    unless ($actenv->{'MAKE'})
    {
        print "Nothing to do.\n" if ($globenv->{'VERBOSE'});
        return 1;
    }

#    if ($relenv->{'APO_RELEASE'} =~ /^3.*/)
#    {
#        print "Can't automatically make APO releases prior to version 4.\nPlease, start the make process manually.\n";
#        return 1;
#    }

    if ($^O =~ /MSWin32/i)
    {
        open FILE_OUT, ">$relenv->{'OWN'}\\LCAMake.bat" or return 0;
        open FILE_IN, "<$relenv->{'OWN'}\\LCAEnv.bat" or return 0;
        while (<FILE_IN>) {
            print FILE_OUT $_;
        }
        close (FILE_IN);

        if ($globenv->{'PROD_MAKE'})
        {
            print FILE_OUT "echo Running 'build_lcapps'...\n";
            print FILE_OUT "build_lcapps -scratch\n";
        }
        else
        {
            print FILE_OUT "echo Running 'python $ENV{'TOOL'}/bin/dsp2vmake.py'...\n";

            if ($globenv->{'VERBOSE'})
            {
                print FILE_OUT "python $ENV{'TOOL'}/bin/dsp2vmake.py -v\n";
            }
            else
            {
                print FILE_OUT "python $ENV{'TOOL'}/bin/dsp2vmake.py\n";
            }

            print FILE_OUT "echo Running 'lcmake " . ($globenv->{'SCRATCH'} ? "-U " : "") . "-i $actenv->{'TARGET'}'...\n";
            print FILE_OUT "lcmake " . ($globenv->{'SCRATCH'} ? "-U " : "") . "-i $actenv->{'TARGET'}\n";
        }

        close FILE_OUT or die "Can't close LCAMake.bat!\n$!\n";

        print "Executing Make script:\n\n";

        open CMD_OUT, "call $relenv->{'OWN'}\\LCAMake.bat 2>&1 |" or return 0;
        while (<CMD_OUT>) { print "$_" }
        close CMD_OUT;

        #unlink("$relenv->{'OWN'}\\LCAMake.bat");
    }
    else
    {
        open FILE_OUT, ">$relenv->{'OWN'}/LCAMake.sh" or return 0;
        print FILE_OUT "#!/bin/sh\n";
        print FILE_OUT ". $relenv->{'OWN'}/.iprofile\n";

        if ($globenv->{'PROD_MAKE'})
        {
            print FILE_OUT "echo Running 'build_lcapps'...\n";
            print FILE_OUT "build_lcapps -scratch\n";
        }
        else
        {
            print FILE_OUT "echo Running 'python $ENV{'TOOL'}/bin/dsp2vmake.py'...\n";

            if ($globenv->{'VERBOSE'})
            {
                print FILE_OUT "python $ENV{'TOOL'}/bin/dsp2vmake.py -v\n";
            }
            else
            {
                print FILE_OUT "python $ENV{'TOOL'}/bin/dsp2vmake.py\n";
            }

            print FILE_OUT "echo Running 'lcmake " . ($globenv->{'SCRATCH'} ? "-U " : "") . "-i $actenv->{'TARGET'}'...\n";
            print FILE_OUT "lcmake " . ($globenv->{'SCRATCH'} ? "-U " : "") . "-i $actenv->{'TARGET'}\n";
        }

        close FILE_OUT or die "Can't close LCAMake.sh!\n$!\n";

        print "Executing Make script:\n\n";

        open CMD_OUT, ". $relenv->{'OWN'}/LCAMake.sh |" or return 0;
        while (<CMD_OUT>) { print "$_" }
        close CMD_OUT;

        unlink ("LCAMake.sh");
    }

    print "\nMake done.\n";

    return 1;
}

##############################################################################
# LCARemP4Files() - Removes all Perforce administrated files again.
##############################################################################

sub LCARemP4Files
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCARemP4Files", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCARemP4Files", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    # First, remove the Perforce administrated files by using P4Sync:
    print "Removing Perforce administrated files...\n" if ($globenv->{'VERBOSE'});
    if (P4Sync($p4env->{'PORT'}, $p4env->{'USER'}, $p4env->{'CLIENT'}, $p4env->{'SERVER_PATH'}, 0, 0, $globenv->{'VERBOSE'}))
    {
        print "Perforce files removed.\n" if ($globenv->{'VERBOSE'});
    }
    else
    {
        return 0;
    }

    return 1;
}

##############################################################################
# LCARemP4Client() - Removes the client again, if created flag is set.
##############################################################################

sub LCARemP4Client
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCARemP4Client", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCARemP4Client", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    # Remove any made perforce client settings (if unchanged):
    if ($p4env->{'_CLIENT_CREATED'})
    {
        print "Removing Perforce client '$p4env->{'CLIENT'}'...\n" if ($globenv->{'VERBOSE'});
        if(P4RemClient($p4env->{'PORT'}, $p4env->{'CLIENT'}))
        {
            print "Perforce client removed.\n";
        }
        else
        {
            return 0;
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
                print "Warning: Perforce client root has been changed since installation.\nRestoring of root directory cancelled.\n";
            }
            else
            {
                $c_info->{'Root'} = $p4env->{'_ROOT_REMOVED'};
            }
        }

        if (P4SetClient($p4env->{'PORT'}, $p4env->{'CLIENT'}, $c_info->{'Root'}, undef, undef, undef, undef, \@views))
        {
            print "Perforce client restored.\n";
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

##############################################################################
# LCARemP4User() - Removes the Perforce user again, if created before.
##############################################################################

sub LCARemP4User
{
    my ($p4env, $globenv) = @_;

    unless (ref($p4env))   { _throw_error("LCARemP4User", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCARemP4User", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    if ($p4env->{'_USER_CREATED'})
    {
        print "Removing Perforce user '$p4env->{'USER'}'...\n" if ($globenv->{'VERBOSE'});
        if (P4RemUser($p4env->{'PORT'}, $p4env->{'USER'}))
        {
            print "Perforce user removed.\n";
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

##############################################################################
# LCARemOwn() - Removes the Own directory (softly, no readonly's removed).
##############################################################################

sub LCARemOwn
{
    my ($relenv, $globenv) = @_;

    unless (ref($relenv))   { _throw_error("LCARemOwn", "Error calling function! First parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }
    unless (ref($globenv)) { _throw_error("LCARemOwn", "Error calling function! Second parameter has to be a hash reference!\nIf it is, make sure that it is initialised.") }

    print "Removing directory tree '$relenv->{'OWN'}'...\n" if ($globenv->{'VERBOSE'});
    if (rmtree($relenv->{'OWN'}, $globenv->{'VERBOSE'}, 1))
    {
        print "\nDirectory tree removed.\n";
    }
    else
    {
        print "\nWARNING: At least one (sub-)directory could not be removed. Please make sure that\n";
        print "         none of these directories is read-only and that you have no open\n";
        print "         development or explorer windows that point to this directory.\n\n";
    }

    return 1;
}

##############################################################################
# _throw_error() - Throws an heavy error and dies.
##############################################################################

sub _throw_error
{
    my ($func, $err_text) = @_;
    print STDERR "\nERROR in module 'LCAEnv.pm' in function '$func'!\n\nDetailed error text:\n\n$err_text\n\n";
    exit(10);
}

sub get_default_settings
{
        my $platform = shift;
        return % {$machine_defaults{$platform}};
}


1;
