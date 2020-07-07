#!/usr/bin/perl
#
#    lcinst.pl - Gets a previously made LC version and installs it into
#                the local $INSTROOT
#
#    @(#)lcinst     2003-03-17
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
#############################################################################
# Static includes:

#unshift @INC, "$ENV{TOOL}/bin";
#unshift @INC, "$ENV{TOOL}/lib/Perl";

use strict;
use Getopt::Long;
use File::Path;
use File::Copy;
use Net::FTP;

##############################################################################
# Console Preparation (redirect STDERR to STDOUT and disable buffering):

close   STDERR;
open    STDERR, ">&STDOUT";
$| = 1;

##############################################################################
# Global variables:

# Program:
my $ProgramName         = "get_components";
my $Version             = "1";
my $Revision            = "01";
my $Date                = "2003-12-02";

my %Opts                = ();
my $Glob                = {};

my @Packets;
my @InstallPackets;
my @CopiedPackets;
my @UnpackedPackets;
my @CleanUp;

##############################################################################
# Main routine:
##############################################################################

print "\n$ProgramName v$Version.$Revision ($Date), (c)2003 SAP AG\n\n";

# Checking ENV:

#unless ($ENV{'TOOL'})
#{
#    print "ERROR! '\$TOOL' is not set in environment! Exiting...\n";
#}

# Parsing command line parameters:

if (!GetOptions( \%Opts, 'h|help|?', 'v|verbose', 'y|yes', 'own=s', 'rel=s', 'no_bas') || $Opts{'h'})
{
    Usage();
}

ParseCmdLineParams();

ShowHashRef($Glob) if ($Glob->{'GLOBAL'}->{'VERBOSE'});

unless ($Glob->{'GLOBAL'}->{'ASSUME_YES'})
{
    print "\nBefore you can install new components, *ALL* running SAPDB\n";
    print "components must be stopped! Please, check now for any DBM servers,\n";
    print "xservers or database instances still running.\n\n";
    print "Press any key to continue...";
    <STDIN>;
}

@Packets    = @ARGV;

# Create directories:

print "Creating directories...\n" if ($Glob->{'GLOBAL'}->{'VERBOSE'});

unless ((-d $Glob->{'RELEASE'}->{'OWN'}) || (mkpath($Glob->{'RELEASE'}->{'OWN'}, $Glob->{'GLOBAL'}->{'VERBOSE'}, 0777)))
{
    die "ERROR! Can't create directory '$Glob->{'RELEASE'}->{'OWN'}'!\n Exiting...";
}

unless ((-d $Glob->{'GLOBAL'}->{'INST_DIR'}) || (mkpath($Glob->{'GLOBAL'}->{'INST_DIR'}, $Glob->{'GLOBAL'}->{'VERBOSE'}, 0777)))
{
    die "ERROR! Can't create directory '$Glob->{'GLOBAL'}->{'INST_DIR'}'!\n Exiting...";
}

unless ((-d $Glob->{'GLOBAL'}->{'TMP_DIR'}) || (mkpath($Glob->{'GLOBAL'}->{'TMP_DIR'}, $Glob->{'GLOBAL'}->{'VERBOSE'}, 0777)))
{
    die "ERROR! Can't create directory '$Glob->{'GLOBAL'}->{'TMP_DIR'}'!\n Exiting...";
}

unless ((-d $Glob->{'GLOBAL'}->{'WRK_DIR'}) || (mkpath($Glob->{'GLOBAL'}->{'WRK_DIR'}, 1, 0777)))
{
    die "ERROR! Can't create directory '$Glob->{'GLOBAL'}->{'INST_DIR'}/wrk'!\n Exiting...";
}

# Retrieve source directory:

if ($Glob->{'RELEASE'}->{'IS_V74'})
{
    $Glob->{'RELEASE'}->{'SRC_PATH'} = GetInstallPath($Glob->{'RELEASE'}->{'VERSION'});
}
else
{
    ($Glob->{'RELEASE'}->{'SRC_PATH'}, $Glob->{'GLOBAL'}->{'PKG_IS_SAR'}) = GetSar($Glob->{'RELEASE'}->{'VERSION'}, $Glob->{'GLOBAL'}->{'INST_DIR'});
}

# Collect packages to install:

if ($Glob->{'GLOBAL'}->{'PKG_IS_SAR'})
{
    if (@Packets)
    {
        @InstallPackets = @Packets;
    }
    else
    {
        print "\nRetrieving available packets...\n";
        @Packets = GetFileList($Glob->{'RELEASE'}->{'SRC_PATH'});
        print "  -> " . @Packets . " packages found.\n" if $Glob->{'GLOBAL'}->{'VERBOSE'};

        if ($Glob->{'GLOBAL'}->{'ASSUME_YES'})
        {
            @InstallPackets = @Packets;
        }
        else
        {
            push @InstallPackets, GetPrecompiler(@Packets);
            push @InstallPackets, GetMainPacks(@Packets);
        }
    }

    if (@InstallPackets)
    {
        print "\nCopying files...\n";
        foreach my $pkg (@InstallPackets)
        {
            my $file = CopyFile("$Glob->{'RELEASE'}->{'SRC_PATH'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$pkg", "$Glob->{'GLOBAL'}->{'INST_DIR'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$pkg");
            if($file)
            {
                push @CopiedPackets, $pkg;
            }
        }
        print "\n" . @CopiedPackets . " packet(s) copied.\n";
    }
    else
    {
        print "\nYou selected no files. Aborting...\n";
        CleanExit(0);
    }

    if (@CopiedPackets)
    {
        print "\nInstalling files...\n";
        chdir($Glob->{'GLOBAL'}->{'INST_DIR'});
        foreach my $file (@CopiedPackets)
        {
            push @UnpackedPackets, $file if (UnpackFile($file));
        }
        print "\n" . @UnpackedPackets . " packet(s) installed.\n";
    }
    else
    {
        print "\nNo files have been copied. Aborting...\n";
        CleanExit (0);
    }
}

if (@UnpackedPackets or ($Glob->{'GLOBAL'}->{'PKG_IS_SAR'} == 0))
{
    print "\nRegistering packages...\n";
    RegisterFiles();
    print "\nPackages registered.\n\n";
}
else
{
    print "\nNo files have been installed. Aborting...\n";
    CleanExit (0);
}

CleanExit(0);

##############################################################################
# ParseCmdLineParams() - Checks all Command line parameters.
##############################################################################

sub ParseCmdLineParams
{

    $Glob->{'FTP'}->{'SERVER'}          = "P26325.ber.sap.corp";
    $Glob->{'FTP'}->{'USER'}            = "ftp";
    $Glob->{'FTP'}->{'PASSWORD'}        = "ftp";
    $Glob->{'FTP'}->{'ROOT'}            = "/CD-Images/DB_REL/";

    $Glob->{'RELEASE'}->{'IS_V74'}      = 0;
    $Glob->{'RELEASE'}->{'SRC_PATH'}    = "";
    $Glob->{'RELEASE'}->{'OWN'}         = ($Opts{'own'} ? $Opts{'own'} : ($ENV{'OWN'} ? $ENV{'OWN'} : die "ERROR! Can't determine path to install DB to. Please use '-own' option.\n"));
    $Glob->{'RELEASE'}->{'VERSION'}     = ($Opts{'rel'} ? CheckLCVersion($Opts{'rel'}) : die "ERROR! You have to give a release to be installed.\n");

    $Glob->{'GLOBAL'}->{'PATH_SEP'}     = ($^O =~ /MSWin32/i ? "\\" : "/");
    $Glob->{'GLOBAL'}->{'TMP_DIR'}      = $Glob->{'RELEASE'}->{'OWN'} . $Glob->{'GLOBAL'}->{'PATH_SEP'} . "tmp";
    $Glob->{'GLOBAL'}->{'INST_DIR'}     = $Glob->{'RELEASE'}->{'OWN'} . $Glob->{'GLOBAL'}->{'PATH_SEP'} . "usr";
    $Glob->{'GLOBAL'}->{'WRK_DIR'}      = $Glob->{'GLOBAL'}->{'INST_DIR'} . $Glob->{'GLOBAL'}->{'PATH_SEP'} . "wrk";
    $Glob->{'GLOBAL'}->{'VERBOSE'}      = ($Opts{'v'} ? 1 : 0);
    $Glob->{'GLOBAL'}->{'ASSUME_YES'}   = ($Opts{'y'} ? 1 : 0);
    $Glob->{'GLOBAL'}->{'PKG_IS_SAR'}   = 1;
    $Glob->{'GLOBAL'}->{'TOOL_PATH'}    = $1 if ($0 =~ /^(.*)[\\\/]([^\\\/]+)/);
}

##############################################################################
# Usage() - Shows the program usage and exits the script.
##############################################################################

sub Usage
{
    print <<USAGE_END;

Usage:  get_components [-own <OWN_DIR>] [-rel <RELEASE>] [-(v|verbose)]
                       [-no_bas] [-(y|yes)] [Packet1 [.. , PacketN]]

Where:
            -own <OWN_DIR>
                With this option, you can specify an output path to install
                the liveCache to. It will be installed into <OWN_DIR>/db.
                NOTE: If you don't give this option, the environment variable
                'INSTROOT' will be used. Take care that this is not assigned
                to another release.

            -rel <RELEASE>
                This is the LC version you want to install. The name should
                be given in one of the following formats:

                - 7403DEV
                - 740311
                - 7.4.03.DEV
                - 7.4.03DEV
                - 7.4.03.11
                - 7403 (build will be asked for)

                NOTE: If you don't give a build number, it will be asked for.
                      If you have set '-y', 'DEV' will be assumed.

            -v or -verbose
                With this option turned on, the program delivers a more
                detailed output.

            -no_bas
                Prevents program from using "/bas"-tree to get components.

            -y or -yes
                Automatically answer all questions with (y)es.

            Packet1 .. PacketN

                You can also specify the packets you want to install. If
                none are given, you'll be asked for. If you have set '-y',
                all available packets will be installed.

USAGE_END

    CleanExit(0);
}

##############################################################################
# RequestInput() - Asks for a user input
##############################################################################

sub RequestInput
{
    my ($outstr,$defval,$inpmask, $errstr) = @_;
    my $tempval;

    if ($outstr)
    {
        print $outstr;
    }

    unless ((!$defval) || ($defval =~ /^$inpmask$/))
    {
        print "\nThe input mask doesn't seem to be appropriate! Exiting...".$inpmask;
        CleanExit(0);
    }

    $tempval = <STDIN>;
    chomp $tempval;

    until (($tempval =~ /^$inpmask$/) or ($defval and ($tempval =~ /^$/)))
    {
        print "Input incorrect. Please, just use [$errstr].\n".$outstr;
        $tempval = <STDIN>;
        chomp $tempval;
    }

    if ($tempval =~ /^$/)
    {
        $tempval = $defval;
    }

    return $tempval;
}

##############################################################################
# CheckLCVersion() - Checks for the correct LC version format.
##############################################################################

sub CheckLCVersion
{
    my $lcver = shift;

    $lcver  = uc($lcver);
    $Glob->{'RELEASE'}->{'IS_V74'}    = ($Opts{'no_bas'} ? 0 : (($lcver ge "74") ? 1 : 0));

    if ($Glob->{'RELEASE'}->{'IS_V74'})
    {

        if (($lcver =~ /^\d{4}?(DEV|COR|RAMP)$/) or ($lcver =~ /^\d{6}?$/) or ($lcver =~ /^\d\.\d\.\d\d\.(DEV|COR|RAMP)$/))
        {
            return $lcver;
        }

        if (($lcver =~ /^\d{4}?$/) or ($lcver =~ /^\d\.\d\.\d\d$/))
        {
            if ($Glob->{'GLOBAL'}->{'ASSUME_YES'})
            {
                return ($lcver . "DEV");
            }

            print "\nPlease, select one of the following builds:\n\n";
            print "[d] = DEV (default)\n";
            print "[c] = COR\n";
            print "[r] = RAMP\n";
            print "[b] = Select build number manually\n";

            my $sel = RequestInput("\nPlease select: ", "d", "[dDcCrRbB]", "d, c, r or b");

            if ($sel =~ /^d$/)
            {
                return ($lcver . "DEV");
            }
            elsif ($sel =~ /^c$/)
            {
                return ($lcver . "COR");
            }
            elsif ($sel =~ /^r$/)
            {
                return ($lcver . "RAMP");
            }
            else
            {
                return ($lcver . RequestInput("\nPlease enter the build number: ", "", "[0-9]+", "0-9"));
            }
        }
    }
    else
    {
        $lcver =~ s/\.//g;

        return $lcver;
    }


    print "\nThe liveCache version you entered has not a valid format!\n";

    Usage();
}

##############################################################################
# GetInstallPath() - Retrieves the path where the LC package lies.
##############################################################################

sub GetInstallPath
{
    my $lcver = shift;
    my $lcown;

    if ($Glob->{'RELEASE'}->{'IS_V74'})
    {
        local @ARGV = ("-release", "dev", "-lcversion", "$lcver");
        do "$ENV{TOOL}/bin/vmakeEnv.pl";

        $lcown = vmakeEnv::getLC_OWN();

        unless ($lcown)
        {
            print "Can't determine installation path for '$lcver'!\nExiting...\n";
            CleanExit(0);
        }

        $lcown .= $Glob->{'GLOBAL'}->{'PATH_SEP'} . "SAPDB_COMPONENTS";
        print "Using Directory: $lcown\n";
    }

    return $lcown;
}

##############################################################################
# GetPlatformPath() - Retrieves the platform path.
##############################################################################

sub GetPlatformPath
{
    my $uname="";
    my %mapping = ();

    $mapping{'AIX_32'}              = "UNIX/AIX_32";
    $mapping{'AIX_64'}              = "UNIX/AIX_64";
    $mapping{'AIX5_32'}             = "UNIX/AIX5_32";
    $mapping{'AIX5_64'}             = "UNIX/AIX5_64";
    $mapping{'OSF1_64'}             = "UNIX/DEC_64";
    $mapping{'HP-UX_32'}            = "UNIX/HP11_32";
    $mapping{'HP-UX_64'}            = "UNIX/HP11_64";
    $mapping{'Linux_32'}            = "UNIX/LINUX_32";
    $mapping{'SunOS_32'}            = "UNIX/SUNOS_32";
    $mapping{'SunOS_64'}            = "UNIX/SUNOS_64";
    $mapping{'ReliantUNIX-N_32'}    = "UNIX/RLX_32";
    $mapping{'ReliantUNIX-N_64'}    = "UNIX/RLX_64";
    $mapping{'Win_32'}              = "NT/I386";
    $mapping{'Win_64'}              = "NT/IA64";

    if($^O =~ /MSWin32/i)
    {
        $uname = "Win_" . ($ENV{'BIT64'} ? "64" : "32");
    }
    else
    {
        open UNAME_OUT, "uname |" or die "Can't open 'uname'!\n$!\n";
        $uname = <UNAME_OUT>;
        chomp $uname;
        close UNAME_OUT or die "Can't close 'uname'!\n$!\n";

        if ($uname =~ /^AIX$/)
        {
            open UNAME_OUT, "uname -v |" or die "Can't open 'uname -v'!\n$!\n";
            my $uadd = <UNAME_OUT>;
            chomp $uadd;
            close UNAME_OUT or die "Can't close 'uname -v'!\n$!\n";

            $uname .= "5" if ($uadd == 5);
        }

        if      ($uname =~ /^OSF1$/)    { $uname .= "_64" }
        elsif   ($uname =~ /^Linux$/)   { $uname .= "_32" }
        else
        {
            $uname .= ($ENV{'BIT64'} ? "_64" : "_32");
        }
    }

    return $mapping{$uname};
}

##############################################################################
# GetSar() - Retrieves and unpacks a SAR packet.
##############################################################################

sub GetSar
{
    my $lcver           = shift;
    my $destpath        = shift;
    my $lcown           = CheckSarBuilds($lcver);
    my @dir             = grep { (/^\S+\.SAR$/ && !/^SAPDBPRE.SAR$/) || (/^\S+\.CAR$/ && !/^SAPDBPRE.CAR$/) } GetFTPDir($lcown);
    my @files;
    my $count           = 1;
    my $outown;
    my $sar = 1;
    my $sapcarpath;

    unless (@dir)
    {
        print "\nThe liveCache release '$lcver' does not exist for this platform!\n";
        CleanExit(0);
    }

    if (@dir > 1)
    {
        print "\nThere is more than one SAR packet available.\n\n";

        foreach (@dir)
        {
            print "[$count] = $_\n";
            $count ++;
        }

        print "[a] = install all packets.\n";
        print "[n] = install no packet.\n\n";

        my $sel = RequestInput("Please select (separate multi-selects by ' ') : ", "", "[0-9aAnN ]+", "Input incorrect. Please use [0-9, n or a].\n");

        if ($sel =~ /^a$/i)
        {
            push @files, @dir;
        }
        elsif ($sel =~ /^n$/i)
        {
            print "\nAborting...\n";
            CleanExit(0);
        }
        else
        {
            foreach (split( / /, $sel))
            {
                push @files, $dir[$_ - 1];
            }
        }
    }
    else
    {
        push @files, $dir[0];
    }

    unless ($Glob->{'GLOBAL'}->{'ASSUME_YES'})
    {
        print "\nPrior to version 7.4, the complete SAR packet has to be copied, first in order\nto install the single packets.\n";

        unless (RequestInput("Do you want to do this now? [Y/n] : ", "y", "[yYnN]", "y or n") =~ /^[jy1]$/i)
        {
            print "\nNo SAR, no installation. Aborting...\n";
            CleanExit(0);
        }
    }

    foreach my $file (@files)
    {
        print "\nCopying $file...\n";

        if ($file =~ /.*\.CAR$/)
        {
            $Glob->{'GLOBAL'}->{'TMP_DIR'} = $destpath;
            $sar = 0;
        }

        unless (GetFTPFile("$lcown/$file", "$Glob->{'GLOBAL'}->{'TMP_DIR'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$file"))
        {
            print "\nCopying for SAR failed. Exiting...\n";
            CleanExit(0);
        }

        push @CleanUp, "$Glob->{'GLOBAL'}->{'TMP_DIR'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$file";

        print "\nExtracting SAR/CAR archive...\n";

        chdir($Glob->{'GLOBAL'}->{'TMP_DIR'});

        if ($^O =~ /MSWin32/i)
        {
            $sapcarpath = $Glob->{'GLOBAL'}->{'TOOL_PATH'};
            $sapcarpath =~ s/bin$/pgm/;
        }
        else
        {
            $sapcarpath = "/usr/local/bin";
        }
        open CMD_OUT, "$sapcarpath$Glob->{'GLOBAL'}->{'PATH_SEP'}SAPCAR -xvf $file |" or die "Can't open 'SAPCAR -xvf $file'!\n$!\n";

        while (<CMD_OUT>)
        {
            if (/\s(\S+)\/(\S+\.TGZ)/i)
            {
                $outown = "$Glob->{'GLOBAL'}->{'TMP_DIR'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$1";
            }
            print "$_" if $Glob->{'GLOBAL'}->{'VERBOSE'};
        }
        unless (close(CMD_OUT))
        {
            print "SAPCAR: Archive extraction failed.\n$!\n";
            CleanExit(0);
        }
    }

    push @CleanUp, $outown;

    return($outown, $sar);
}

##############################################################################
# CheckSarBuilds() - Returns a complete lcown this release/platform.
##############################################################################

sub CheckSarBuilds
{
    my $lcver           = shift;
    my $platformpath    = GetPlatformPath();
    my $lcown           = $Glob->{'FTP'}->{'ROOT'} . $lcver . "/" . $platformpath;
    my @dir             = grep { (/^\S+\.SAR$/ && !/^SAPDBPRE.SAR$/) || (/^\S+\.CAR$/ && !/^SAPDBPRE.CAR$/) } GetFTPDir($lcown);
    my @foundbuilds;
    my $count = 1;

    if (@dir >= 1)
    {
        return $lcown;
    }

    if($lcver =~ /^(\d\d\d\d)(.*)$/)
    {
        $lcver = $1;
        if ($2)
        {
            print "\nThe build number you have given doesn't support this platform.\n";
        }
    }

    print "\nLooking for builds in release '$lcver'...\n\n";

    foreach my $tmpdir (grep { /^$lcver.*$/ } GetFTPDir($Glob->{'FTP'}->{'ROOT'}))
    {
        @dir = grep { (/^\S+\.SAR$/ && !/^SAPDBPRE.SAR$/) || (/^\S+\.CAR$/ && !/^SAPDBPRE.CAR$/) } GetFTPDir($Glob->{'FTP'}->{'ROOT'} . $tmpdir . "/" . $platformpath);
        push @foundbuilds, $tmpdir if (@dir >= 1);
    }

    if (@foundbuilds == 0)
    {
        print "Sorry, there's no build of version $lcver for this platform.\nExiting...\n";
        CleanExit(0);
    }
    elsif (@foundbuilds == 1)
    {
        print "Taking release '$foundbuilds[0]'...\n";
        return $Glob->{'FTP'}->{'ROOT'} . $foundbuilds[0] . "/" . $platformpath;
    }

    print "There ist more than one build of version $lcver for this platform:\n\n";

    foreach (@foundbuilds)
    {
        print "[$count] = $_\n";
        $count ++;
    }

    print "[n] = install no version (abort).\n\n";

    my $sel = RequestInput("Please select one : ", "", "[0-9nN]+", "Input incorrect. Please use [0-9 or n].\n");

    if ($sel =~ /^n$/i)
    {
        print "\nAborting...\n";
        CleanExit(0);
    }
    else
    {
        return $Glob->{'FTP'}->{'ROOT'} . $foundbuilds[$sel - 1] . "/" . $platformpath;
    }
}

##############################################################################
# GetFileList() - Retrieves a list of files with specific attributes.
##############################################################################

sub GetFileList
{
    my ($dir) = @_;

    opendir DIR_OUT, "$dir" or die "Can't read directory!\n$!\n";
    my @files = grep { /^\S+\.tgz$/i && -f "$dir$Glob->{'GLOBAL'}->{'PATH_SEP'}$_" } readdir(DIR_OUT);
    closedir(DIR_OUT);

    return @files;
}

##############################################################################
# GetFTPDir() - Retrieve a specific directory via FTP.
##############################################################################

sub GetFTPDir
{
    my $path = shift;
    my @remotedir;

    my $ftp = ConnectFTP($path);

    if ($ftp)
    {
        @remotedir = $ftp->ls();
        $ftp->quit;
    }

    return @remotedir;
}

##############################################################################
# GetFTPFile() - Retrieves a specific file via FTP.
##############################################################################

sub GetFTPFile
{
    my $source = shift;
    my $dest = shift;

    my $ftp = ConnectFTP("/");

    if ($ftp)
    {

        if (!$ftp->binary())
        {
          $ftp->quit;
          return;
        }
        if (!$ftp->get($source, $dest))
        {
            print "Cannot copy $source to $dest";
            $ftp->quit;
            return 0;
        }

        $ftp->quit;
        return 1;
    }

    return 0;
}

##############################################################################
# ConnectFTP() - Initializes a FTP connection.
##############################################################################

sub ConnectFTP
{
    my $path = shift;

    my $ftp = Net::FTP->new($Glob->{'FTP'}->{'SERVER'}, Debug =>0);

    if (!$ftp)
    {
      print"FTP connection to $Glob->{'FTP'}->{'SERVER'} failed";
      return;
    }

    if (!$ftp->login($Glob->{'FTP'}->{'USER'},$Glob->{'FTP'}->{'PASSWORD'}))
    {
      print "FTP login with $Glob->{'FTP'}->{'USER'} failed";
      $ftp->quit;
      return;
    }

    if (!$ftp->cwd($path))
    {
      $ftp->quit;
      return;
    }

    if (!$ftp->ascii())
    {
      $ftp->quit;
      return;
    }

    return $ftp;
}

##############################################################################
# GetPrecompiler() - Assembles the list of packets to install
##############################################################################

sub GetPrecompiler
{
    my @packs = @_;
    my @out;
    my $count = 1;
    my @pre;

    @pre = grep { /^PCR\d+\.TGZ$/ } @packs;

    unless (@pre)
    {
        return @out;
    }

    print "\nWhich pre-compiler do you want to install?\n\n";

    foreach (@pre)
    {
        print "[$count] = install $_\n";
        $count ++;
    }

    print "[a] = install all versions.\n" if (@pre > 1);
    print "[n] = install no version.\n\n";

    my $sel = RequestInput("Please select (separate multi-selects by ' ') : ", "", (@pre > 1 ? "[0-9aAnN ]+" : "[0-9nN ]+"), "Input incorrect. Please use [0-9" . (@pre > 1 ? ", n or a].\n" : " or a].\n"));

    if ($sel =~ /^a$/i)
    {
        return @pre;
    }
    elsif ($sel =~ /^n$/i)
    {
        return @out;
    }
    else
    {
        my @sels = split( / /, $sel);

        foreach (@sels)
        {
            push @out, $pre[($_ - 1)];
        }
    }

    return @out;
}

##############################################################################
# GetMainPacks() - Checks which main packets should be installed
##############################################################################

sub GetMainPacks
{
    my @packs = @_;
    my @mainp;
    my @out;
    my $count = 1;
    my $packet;

    foreach my $pack (@packs)
    {
        push @mainp, $pack unless (($pack =~ /^PCR\d+\.TGZ$/) or ($pack =~ /^SDBINST\.TGZ$/));
    }

    print "\nWhich of the main packets should be installed?\n\n";

    foreach (@mainp)
    {
        print "[$count] = $_\n";
        $count ++;
    }

    print "[a] = All packets.\n";
    print "[n] = No packet.\n\n";

    my $sel = RequestInput("Please select (separate multi-selects by ' ') : ", "", "[0-9aAnN ]+", "Input incorrect. Please use [0-9, n or a].\n");

    if ($sel =~ /^a$/i)
    {
        return @mainp;
    }
    elsif ($sel =~ /^n$/i)
    {
        return @out;
    }
    else
    {
        my @sels = split( / /, $sel);

        foreach (@sels)
        {
            push @out, $mainp[($_ - 1)];
        }
    }

    return @out;
}

##############################################################################
# CleanExit() - Cleans up all mess made and exits the program.
##############################################################################

sub CleanExit
{
    my $ret = shift;

    if (@CleanUp)
    {
        foreach my $entry (@CleanUp)
        {
            if (-f $entry)
            {
                unlink ($entry);
                next;
            }

            if (-d $entry)
            {
                DelTree ($entry);
                next;
            }
        }
    }

    exit ($ret);
}

##############################################################################
# DelTree() - Removes a complete tree
##############################################################################

sub DelTree
{
    my ($dir) = @_;

    rmtree($dir, $Glob->{'GLOBAL'}->{'VERBOSE'}, 1);
}

##############################################################################
# CopyFile() - Copies a file with eventually asking for it before...
##############################################################################

sub CopyFile
{
    my ($from, $to) = @_;

    if (copy("$from", "$to"))
    {
        print "File '$from' copied.\n" if $Glob->{'GLOBAL'}->{'VERBOSE'};
        push @CleanUp, $to;
        return "$to";
    }
    else
    {
        print "File '$from' not copied!\n$!\n";
    }

    return "";
}

##############################################################################
# UnpackFile() - Decompresses and untars a file.
##############################################################################

sub UnpackFile
{
    my $file = shift;
    my $rc;
    my $tarpath = "";
    my $tarparams = ($Glob->{'GLOBAL'}->{'VERBOSE'} ? "xvf" : "xf");

    if (-e "$Glob->{'GLOBAL'}->{'TOOL_PATH'}$Glob->{'GLOBAL'}->{'PATH_SEP'}gzip")
    {
        $rc = system("$Glob->{'GLOBAL'}->{'TOOL_PATH'}$Glob->{'GLOBAL'}->{'PATH_SEP'}gzip -df $file");
    }
    else
    {
        $rc = system("gzip -df $file");
    }

    if ($rc)
    {
        print "Error: Can't gunzip $file!\n";
        return 0;
    }

    print "File $file unzipped.\n" if $Glob->{'GLOBAL'}->{'VERBOSE'};

    $file =~ s/TGZ/tar/;

    if ($^O =~ /MSWin32/i)
    {
        $tarpath = $Glob->{'GLOBAL'}->{'TOOL_PATH'};
        $tarpath =~ s/bin$/Posix/;

        $rc = system("$tarpath$Glob->{'GLOBAL'}->{'PATH_SEP'}tar $tarparams $file");
    }
    else
    {
        $rc = system("tar xvf $file");
    }

    if ($rc)
    {
        print "WARNING! There have benn warnings/errors while untar '$file'!\n";
    }

    print "File $file untared.\n" if $Glob->{'GLOBAL'}->{'VERBOSE'};

    $file =~ s/TGZ/tar/;
    push @CleanUp, "$Glob->{'GLOBAL'}->{'INST_DIR'}$Glob->{'GLOBAL'}->{'PATH_SEP'}$file";

    return 1;
}

##############################################################################
# RegisterFiles() - Registers the installed files (whoever's in need)
##############################################################################

sub RegisterFiles
{
    my $pgm_path;

    if ($^O =~ /MSWin32/i)
    {
        $pgm_path = "$Glob->{'GLOBAL'}->{'INST_DIR'}\\pgm";
    }
    else
    {
        $pgm_path = "$Glob->{'GLOBAL'}->{'INST_DIR'}/bin";
    }

    open CMD_OUT, "$pgm_path$Glob->{'GLOBAL'}->{'PATH_SEP'}dbmcli -s -R $Glob->{'GLOBAL'}->{'INST_DIR'} inst_reg -k $Glob->{'GLOBAL'}->{'INST_DIR'} |" or print STDERR "Can't open 'dbmcli -s -R $Glob->{'GLOBAL'}->{'INST_DIR'} inst_reg -k $Glob->{'GLOBAL'}->{'INST_DIR'}'!\n$!\n";
    while (<CMD_OUT>) { print "$_" if ($Glob->{'GLOBAL'}->{'VERBOSE'}) }
    close CMD_OUT;

    if ($^O =~ /MSWin32/i)
    {
        open CMD_OUT, "$Glob->{'GLOBAL'}->{'INST_DIR'}\\bin\\xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}\\pgm\\dbpinstall |" or print STDERR "Can't open 'xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}\\pgm\\dbpinstall'!\n$!\n";
        while (<CMD_OUT>) { print "$_" if ($Glob->{'GLOBAL'}->{'VERBOSE'}) }
        close CMD_OUT;
    }
    elsif (($ENV{'BIT64'}) and ($^O !~ /^dec_osf$/i))
    {
        open CMD_OUT, "$Glob->{'GLOBAL'}->{'INST_DIR'}/bin/xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}/lib/lib64/dbpinstall |" or print STDERR "Can't open 'xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}/lib/lib64/dbpinstall'!\n$!\n";
        while (<CMD_OUT>) { print "$_" if ($Glob->{'GLOBAL'}->{'VERBOSE'}) }
        close CMD_OUT;
    }
    else
    {
        open CMD_OUT, "$Glob->{'GLOBAL'}->{'INST_DIR'}/bin/xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}/lib/dbpinstall |" or print STDERR "Can't open 'xregcomp -s $Glob->{'GLOBAL'}->{'INST_DIR'}/lib/dbpinstall'!\n$!\n";
        while (<CMD_OUT>) { print "$_" if ($Glob->{'GLOBAL'}->{'VERBOSE'}) }
        close CMD_OUT;
    }
}

##############################################################################
# ShowHashRef() - Displays a hash reference.
##############################################################################

sub ShowHashRef
{
    my $ref = shift;
    my $header = shift;

    unless (ref($ref)) { die "ShowHashRef: ERROR! First parameter (hash reference) is not set correctly!\n" }

    foreach my $key (sort keys %{$ref})
    {
        unless (ref($ref->{$key}))
        {
            print "$header$key=$ref->{$key}\n";
        }
    }

    print "\n";

    foreach my $key (sort keys %{$ref})
    {
        if (ref($ref->{$key}) eq "HASH")
        {
            print "$header>>$key:\n\n";
            ShowHashRef ($ref->{$key}, ($header . "    "));
        }
        elsif (ref($ref->{$key}) eq "ARRAY")
        {
            print "$header>>$key:\n\n";
            my $count = 0;
            foreach my $elem (@{$ref->{$key}})
            {
                print "$header\[$count] : $elem\n";
                $count ++;
            }
        }
    }
}

