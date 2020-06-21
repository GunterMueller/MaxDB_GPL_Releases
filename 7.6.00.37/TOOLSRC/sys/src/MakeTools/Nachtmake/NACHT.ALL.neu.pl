#! /usr/bin/perl
#
#    NACHT.ALL.neu.pl - Performs a nightly make, new version.
#
#    @(#)NACHT.ALL.neu.pl     2003-02-24
#
#    U. Jansen, SAP AG - Original by F. Strassenburg, SAP AG
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

################################################################################
# Includes:
################################################################################

unshift @INC, "$ENV{TOOL}/lib/Perl";

use strict;

use FileHandle;
use File::Path;
use File::Copy;
use Getopt::Long;
use XML::DOM;
use POSIX;
use qadb;
use imake;
use CCVers;
use BuildPackage::Vars;

################################################################################
# Globals:
################################################################################

my $RC              =   0;
my $Glob            =  {};
my $Check           =   0;
my @LocalTime       = localtime (time);
my $ProgramName     = "NACHT.ALL.neu";
my $Version         = "1.15";
my $VerDate         = "2004-09-30";

################################################################################
# Main routine:
################################################################################

#-------------------------------------------------------------------------------
# Basic settings
#-------------------------------------------------------------------------------

$Glob->{'MACHINE_CONFIG'}               = $ENV{'MACHINE_CONFIG'};
$Glob->{'OWN'}                          = $ENV{'OWN'};
$Glob->{'QA_ID'}                        = 0;
$Glob->{'LC_POOL_ID'}                   = 0;
$Glob->{'STATISTICS'}->{'START_TIME'}   = (scalar localtime);
$Glob->{'STATISTICS'}->{'TARGET_SUM'}   = 0;
$Glob->{'STATISTICS'}->{'TARGET_FAIL'}  = 0;
$Glob->{'QA_ID_WRITTEN'}                = 0;
$Glob->{'LC_POOL_ERROR_SET'}            = 0;
#-------------------------------------------------------------------------------
# Console initialisation
#-------------------------------------------------------------------------------

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

#-------------------------------------------------------------------------------
# Read and check configuration (cmdline, machine info, env vars, xml-settings)
#-------------------------------------------------------------------------------

my $CMDParamHash = CheckCmdLineParams() or Usage();

HashRefToGlobVar ($CMDParamHash);

ProtHeader("Reading Makeprofile.xml (_default, $Glob->{'MACHINE_CONFIG'})");

unless(FillPlatformHash("$Glob->{'OWN'}/sys/wrk/config/$Glob->{'CFG_FILE'}", "_default"))
{
    print "WARNING! Default configuration could not be loaded!\n\n";
}

if(!FillPlatformHash("$Glob->{'OWN'}/sys/wrk/config/$Glob->{'CFG_FILE'}", "$Glob->{'MACHINE_CONFIG'}"))
{
    print "WARNING! Configuration '$Glob->{'MACHINE_CONFIG'}' could not be loaded!\n\n";
    if ($Glob->{'MACHINE_CONFIG'} =~ /^rs6000_51(.*)/)
    {
        unless (FillPlatformHash("$Glob->{'OWN'}/sys/wrk/config/$Glob->{'CFG_FILE'}", "rs6000$1"))
        {
            print "WARNING! Configuration 'rs6000$1' could not be loaded!\n\n";
        }
    }
}

HashRefToGlobVar ($CMDParamHash);

CheckConfigIntegrity();

unlink ("$Glob->{'MAKEID_FILE'}");
unlink ("$Glob->{'ERR_OUTFILE'}") if ($Glob->{'ERR_OUTFILE'});

#-------------------------------------------------------------------------------
# Store Build PID
#-------------------------------------------------------------------------------

ProtHeader("Storing build.pid");

mkpath (["$Glob->{'PROT_DIR'}"], 0, 0775);

WriteFileLine("$Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}build.pid", "$$");

#-------------------------------------------------------------------------------
# Prepare make phase
#-------------------------------------------------------------------------------

ProtHeader("Prepare make");

if ($Glob->{'SCRATCH_MAKE'} eq "1")
{
    chdir("$Glob->{'OWN'}");
    if ($^O =~ /win32/i) {
        foreach my $del_dir (@{$Glob->{'DEL_TREE'}}) {
            system("ATTRIB -R -H -S $Glob->{'OWN'}\\$del_dir");
            system("ATTRIB -R -H -S $Glob->{'OWN'}\\$del_dir\\*.* /S /D");
        }
    } else {
        foreach my $del_dir (@{$Glob->{'DEL_TREE'}}) {
            system("chmod -R a+wrx $Glob->{'OWN'}/$del_dir");
        }
    }
    system("$Glob->{'DEL_CMD'} @{$Glob->{'DEL_TREE'}}");
}

mkpath (["$Glob->{'PROT_DIR'}"], 0, 0775);

if ($Glob->{'SCRATCH_MAKE'} eq "1")
{
    my $imf_exit;

    eval
    {
        $imf_exit = imf ( "config" ) ;
    };

    if ($@)
    {
        print STDERR "ERROR: parsing imf('config')!\n$@\n";
    }
    elsif ($imf_exit != 0)
    {
        print STDERR "ERROR: imf('config') returned with an error!\n$!\n";
    }
}

#-------------------------------------------------------------------------------
# Redirect STDOUT and STDERR
#-------------------------------------------------------------------------------

ProtHeader("Redirect STDOUT and STDERR");

if ($Check)
{
    print "redirection of STDOUT and STDERR\n";
}
else
{
    print "OUTPUT_FILTER: OPEN FILE $Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}$Glob->{'PROT_NAME'}\n";
}

print "\nINFO: Tool directory is '$ENV{'TOOL'}'.\n\n";

#-------------------------------------------------------------------------------
# SAPDB_PURPOSE
#-------------------------------------------------------------------------------

ProtHeader("Set build purpose");

$Glob->{'BUILD_PURPOSE'} = $1 if ($Glob->{'MAKE_TYPE'} =~ /^(\S).*$/);

print "BUILD_PURPOSE has been set to '$Glob->{'BUILD_PURPOSE'}'.\n";

#-------------------------------------------------------------------------------
# Creating new qadb entry
#-------------------------------------------------------------------------------

ProtHeader("Creating new qadb entry");
QADBCreateHandle();
QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'SET_TOOL_CL'}, 'TOOLCL' => $Glob->{'TOOL_CHLIST'}}) if ($Glob->{'TOOL_CHLIST'} ne "");

#-------------------------------------------------------------------------------
# Final configuration
#-------------------------------------------------------------------------------

ProtHeader("Final configuration");
print "SCRIPT_VERSION   = $ProgramName v$Version ($VerDate)\n";
ShowHashRef($Glob);

#-------------------------------------------------------------------------------
# make process
#-------------------------------------------------------------------------------

ProtHeader("Start make");

QADBWriteLog("INFO: Tool directory is '$ENV{'TOOL'}'.");

#QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'INIT_START'}});

$Glob->{'MAKE_RESULT'}      = 1;
$Glob->{'ARCH_PCR_RESULT'}  = 1;
$Glob->{'BUILDPKG_RESULT'}  = 1;

if ($Glob->{'CLI_32_BIT'} eq "1")
{
    MakeTarget("CLI32", "32 bit client", "MAKE_RESULT");
}
elsif ($Glob->{'PCR_32_BIT'} eq "1")
{
    MakeTarget("PCR32", "32 bit precompiler runtime library", "MAKE_RESULT");
}
else
{
    MakeTarget($Glob->{'MAKE_PROFILE'}, "liveCache/SAP DB", "MAKE_RESULT");
}

ProtHeader("End make");

#-------------------------------------------------------------------------------
# build packages
#-------------------------------------------------------------------------------

if ($Glob->{'MAKE_RESULT'})
{
    ProtHeader("Buildpackage");
    MakeTarget("BUILDPKG", "buildpackage", "BUILDPKG_RESULT");
}

#-------------------------------------------------------------------------------
# archive precompiler runtimes
#-------------------------------------------------------------------------------

if ($Glob->{'MAKE_RESULT'} and $Glob->{'BUILDPKG_RESULT'})
{
    ProtHeader("Archiving precompiler runtimes");
    MakeTarget("ARCH_PCR", "precompiler runtime archive", "ARCH_PCR_RESULT");
}

#-------------------------------------------------------------------------------
# Expand e0 files
#-------------------------------------------------------------------------------

ProtHeader("Expanding e0 file paths");

push @{$Glob->{'TARGETS_WITH_PATH'}}, ExpandPath("FAST", @{$Glob->{'MAKE_TARGETS_FAST'}});
push @{$Glob->{'TARGETS_WITH_PATH'}}, ExpandPath("FAST_32", @{$Glob->{'MAKE_TARGETS_32'}});
push @{$Glob->{'TARGETS_WITH_PATH'}}, ExpandPath("QUICK", @{$Glob->{'MAKE_TARGETS_QUICK'}});
push @{$Glob->{'TARGETS_WITH_PATH'}}, ExpandPath("SLOW", @{$Glob->{'MAKE_TARGETS_SLOW'}});

if ($Check)
{
    print "MAKE_TARGETS_FAST  = " . join(", ", @{$Glob->{'MAKE_TARGETS_FAST'}}) . "\n";
    print "MAKE_TARGETS_32    = " . join(", ", @{$Glob->{'MAKE_TARGETS_32'}}) . "\n";
    print "MAKE_TARGETS_QUICK = " . join(", ", @{$Glob->{'MAKE_TARGETS_QUICK'}}) . "\n";
    print "MAKE_TARGETS_SLOW  = " . join(", ", @{$Glob->{'MAKE_TARGETS_SLOW'}}) . "\n";
    print "TARGETS_WITH_PATH  = " . join(", ", @{$Glob->{'TARGETS_WITH_PATH'}}) . "\n";
}

#-------------------------------------------------------------------------------
# Send mail
#-------------------------------------------------------------------------------

ProtHeader("Sending mails");

if ( $Check )
{
    print "system ( checkmake ... )\n";
}
else
{
    my $ret = 0;
    my @cmd = ("$ENV{TOOLSHELL}", "$ENV{'TOOL'}/bin/checkmake.pl");

    push @cmd, ("-qaid", "$Glob->{'QA_ID'}") if ($Glob->{'QA_ID'});
    push @cmd, ("-email", "$Glob->{'USE_EMAIL'}") if ($Glob->{'USE_EMAIL'});
    push @cmd, ("-chlist", "$Glob->{'CHLIST_NUM'}") if ($Glob->{'CHLIST_NUM'});
    push @cmd, ("-outfile", "$Glob->{'ERR_OUTFILE'}") if ($Glob->{'ERR_OUTFILE'});

    unless ($Glob->{'PRIVATE_MAKE'} && !$Glob->{'USE_EMAIL'})
    {
        $ret = system(@cmd, @{$Glob->{'TARGETS_WITH_PATH'}});

        if (( !defined $ret ) or ( $ret != 0 ))
        {
            print STDERR "ERROR: file \"checkmake.pl\" failed\n";
        }
    }
}

#-------------------------------------------------------------------------------
# Get test profile
#-------------------------------------------------------------------------------

ProtHeader("Getting test profile");

if ($Check)
{
    print "Getting test profile...\n";
}
else
{
    if ($Glob->{'MAKE_RESULT'} and $Glob->{'ARCH_PCR_RESULT'} and $Glob->{'BUILDPKG_RESULT'} and $Glob->{'TEST_PFL_NAME'})
    {
        if (open(TESTS, "$Glob->{'TEST_PFL_NAME'}"))
        {
            while(<TESTS>)
            {
                chomp $_;
                unless((/^#/) || (/^$/))
                {
                    $Glob->{'TEST_PROFILE'} = $_;
                }
            }
            close (TESTS);
        }
        unless ($Glob->{'TEST_PROFILE'})
        {
            print "Warning: Test profile could not be read.\n";
        }

        print "Using test profile: $Glob->{'TEST_PROFILE'}\n";
    }
    else
    {
        if ($Glob->{'TEST_PFL_NAME'})
        {
            print "Make was not correct. Getting test profile aborted.\n";
        }
        else
        {
            print "No Test profile, no tests.\n";
        }
    }
}

#-------------------------------------------------------------------------------
# update LC pool ID
#-------------------------------------------------------------------------------

ProtHeader("Get LC pool ID");

if ($Glob->{'MAKE_RESULT'} and $Glob->{'ARCH_PCR_RESULT'} and $Glob->{'BUILDPKG_RESULT'})
{
    $Glob->{'LC_POOL_ID'} = GetLCPoolID();
    print "LC_POOL_ID has been set to '$Glob->{'LC_POOL_ID'}'\n";
}
else
{
    $Glob->{'LC_POOL_ID'} = "0";
    print "Since there have been errors, LC_POOL_ID is set to 0.\n";
}

QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'SET_LC_POOL_ID'}, 'LCPOOLID' => $Glob->{'LC_POOL_ID'}});

#-------------------------------------------------------------------------------
# Finalize QADB status
#-------------------------------------------------------------------------------

ProtHeader("Finalizing");

if ($Glob->{'MAKE_RESULT'} and $Glob->{'ARCH_PCR_RESULT'} and $Glob->{'BUILDPKG_RESULT'} and ($Glob->{'LC_POOL_ID'} ne "0"))
{
    print "\nMAIN EXIT: Congratulations!!! No make errors occured!\n";

    if ($Glob->{'PCR_32_BIT'})
    {
        QADBCreateTestprofile($Glob->{'TEST_PROFILE'}) if ($Glob->{'TEST_PROFILE'});
        QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'PCR_MAKE_OK'}});
    }
    else
    {
        QADBCreateTestprofile($Glob->{'TEST_PROFILE'}) if ($Glob->{'TEST_PROFILE'});
        QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'MAKE_OK'}});
    }

    $Glob->{'MAIN_EXIT'} = 0;
}
else
{
    QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{'MAKE_NOT_OK'}});
    print "\nMAIN EXIT: ATTENTION---ERRORS---ATTENTION---ERRORS---ATTENTION\n" ;
    $Glob->{'MAIN_EXIT'} = 1;
}

#-------------------------------------------------------------------------------
# Final Statistics
#-------------------------------------------------------------------------------

ProtHeader("*** FINAL STATISTICS ***");

$Glob->{'STATISTICS'}->{'TARGET_OVR'} = $Glob->{'STATISTICS'}->{'TARGET_SUM'} + ($Glob->{'MAKE_RESULT'} ? ($Glob->{'BUILDPKG_RESULT'} ? 0 : 1) : 2);

print "MACHINE          = $Glob->{'HOST_NAME'}\n";
print "MACHINE CONFIG   = $Glob->{'MACHINE_CONFIG'}\n";
print "START TIME       = $Glob->{'STATISTICS'}->{'START_TIME'}\n";
print "END TIME         = " . (scalar localtime) . "\n";
print "TARGETS OVERALL  = $Glob->{'STATISTICS'}->{'TARGET_OVR'}\n";
print "TARGETS MADE     = $Glob->{'STATISTICS'}->{'TARGET_SUM'}\n";
print "TARGETS FAILED   = $Glob->{'STATISTICS'}->{'TARGET_FAIL'}\n";
print "MAIN MAKE        = " . ($Glob->{'MAKE_RESULT'} ? "OK\n" : "FAILED\n");
print "BUILD PACKAGE    = " . ($Glob->{'MAKE_RESULT'} ? ($Glob->{'BUILDPKG_RESULT'} ? "OK\n" : "FAILED\n") : "NOT STARTED\n");
print "ARCHIVE PCR      = " . (($Glob->{'MAKE_RESULT'} && $Glob->{'BUILDPKG_RESULT'}) ? ($Glob->{'ARCH_PCR_RESULT'} ? "OK\n" : "FAILED\n") : "NOT STARTED\n");
print "ALL OK           = " . (($Glob->{'MAKE_RESULT'} && $Glob->{'BUILDPKG_RESULT'} && $Glob->{'ARCH_PCR_RESULT'} && ($Glob->{'STATISTICS'}->{'TARGET_FAIL'} == 0)) ? "YES\n" : "NO\n");


if (defined($ENV{DTM_TASKEXPORTFILE}) ) {
    if (open (EXPORTFILE, "> $ENV{DTM_TASKEXPORTFILE}")) {
        print EXPORTFILE "SUBTASKS_OK=$Glob->{'STATISTICS'}->{'TARGET_SUM'}\n";
        print EXPORTFILE "SUBTASKS_ERROR=$Glob->{'STATISTICS'}->{'TARGET_FAIL'}\n";
        close(EXPORTFILE);
        print "STATS EXPORTED   = YES\n";
    } else {
        print "STATS EXPORTED   = NO (Error opening file '$ENV{DTM_TASKEXPORTFILE}')\n";
    }
} else {
  print "STATS EXPORTED   = No DTM Support\n";
}

#-------------------------------------------------------------------------------
# Resetting file output
#-------------------------------------------------------------------------------

ProtHeader("PROTOCOL END");

if ($Check)
{
    print "Resetting file output\n";
}
else
{
    print "OUTPUT_FILTER: CLOSE FILE\n";
}

#-------------------------------------------------------------------------------
# Write make protocol into QADB
#-------------------------------------------------------------------------------

my $WriteProtOK = QADBWriteProt("$Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}$Glob->{'PROT_NAME'}", "$Glob->{'PROT_NAME'}", "Make Protocol (complete)");

#-------------------------------------------------------------------------------
# Scan protocol for warnings / failures
#-------------------------------------------------------------------------------

ScanMakeProt();

#-------------------------------------------------------------------------------
# Remove prot, if everything was fine.
#-------------------------------------------------------------------------------

if ($WriteProtOK && $Glob->{'MAKE_RESULT'} && $Glob->{'BUILDPKG_RESULT'} && $Glob->{'ARCH_PCR_RESULT'} && ($Glob->{'STATISTICS'}->{'TARGET_FAIL'} == 0))
{
    unlink("$Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}$Glob->{'PROT_NAME'}");
}

CleanExit($Glob->{'MAIN_EXIT'});

################################################################################
# ScanMakeProt() - Scans a make protocol for any warnings / errors.
################################################################################

sub ScanMakeProt
{
    return unless ($Glob->{'ERR_OUTFILE'});
    
    return unless (open (IN_FILE, "< $Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}$Glob->{'PROT_NAME'}"));
    my $OutLines = {};
    my $line_count = 0;
    my $found_line = 0;
    while (<IN_FILE>)
    {
        $line_count += 1;
        my $line = $_;
        chomp $line;
        if ($line =~ /^OUTPUT_FILTER: SCAN (\S+) (ON|OFF)/) {
            $Glob->{$1 . '_ACTIVE'} = ($2 =~ /ON/ ? 1 : 0);
            next;
        }
        foreach my $PScan_Part (@{$Glob->{'PSCAN_PARTS'}})
        {
            if ($Glob->{$PScan_Part . '_ACTIVE'})
            {
                if ($line =~ /$Glob->{$PScan_Part . '_MATCH'}/ && $line !~ /$Glob->{$PScan_Part . '_EXCLUDE'}/)
                {
                    if ($Glob->{'WEBDAV_PROT'} && $Glob->{'HTML_OUTPUT'})
                    {
                        $OutLines->{$PScan_Part} .= "<A HREF=\"$Glob->{'WEBDAV_PROT'}#$line_count\">Line $line_count</A>: $line\n";
                    }
                    else
                    {
                        $OutLines->{$PScan_Part} .= "Line $line_count: $line\n";
                    }
                    $OutLines->{$PScan_Part . '_FLAG'} = 1;
                    $found_line = 1;
                }
                elsif ($OutLines->{$PScan_Part . '_FLAG'} && $line =~ /^\s+\S.*/)
                {
                    $OutLines->{$PScan_Part} .= "$line\n";
                }
                else
                {
                    $OutLines->{$PScan_Part . '_FLAG'} = 0;
                }
            }
        }
    }
    close (IN_FILE);

    if ($found_line)
    {
	    if (open (OUT_FILE, ">> $Glob->{'ERR_OUTFILE'}"))
	    {
	        print OUT_FILE "\n\n(----------------------- PROTOCOL SCAN -----------------------)\n\n";
            print OUT_FILE "Protocol link: $Glob->{'WEBDAV_PROT'}\n\n" if ($Glob->{'WEBDAV_PROT'});
	        foreach my $PScan_Part (@{$Glob->{'PSCAN_PARTS'}})
	        {
	            if ($OutLines->{$PScan_Part})
	            {
	                print OUT_FILE "\n" . $Glob->{$PScan_Part . '_TITLE'} . ":\n\n";
	                print OUT_FILE "$OutLines->{$PScan_Part}";
	            }
	        }
		    close (OUT_FILE);
	    }
    }
}

################################################################################
# CheckCmdLineParams() - Checks, if the correct cmdline parameters were given
################################################################################

sub CheckCmdLineParams
{
    my %Opts;
    my $hashref = {};

    ProtHeader ("Read and check command line parameters") ;

    if (!GetOptions( \%Opts, 'force_rm|scratch', 'cli_bit_32', 'pcr_bit_32', 'html', 'make_type=s', 'check|test', 'h|help|?', 'private', 'email=s', 'cfgfile=s', 'errout=s', 'make_profile=s', 'no_qadb') || $Opts{'h'})
    {
        return;
    }

    $hashref->{'SCRATCH_MAKE'}  = defined $Opts{'force_rm'}     ? "1" : "0";
    $hashref->{'CLI_32_BIT'}    = defined $Opts{'cli_bit_32'}   ? "1" : "0";
    $hashref->{'PCR_32_BIT'}    = defined $Opts{'pcr_bit_32'}   ? "1" : "0";
	$hashref->{'MAKE_PROFILE'}	= defined $Opts{'make_profile'} ? $Opts{'make_profile'} : "SERVER";
    $hashref->{'PRIVATE_MAKE'}  = defined $Opts{'private'}      ? "1" : "0";
    $hashref->{'HTML_OUTPUT'}   = defined $Opts{'html'}         ? "1" : "0";
    $hashref->{'NO_QADB'}       = defined $Opts{'no_qadb'}         ? "1" : "0";
    $hashref->{'USE_EMAIL'}     = $Opts{'email'} if($Opts{'email'});
    $Check                      = defined $Opts{'check'}        ?  1  :  0;
    $hashref->{'CFG_FILE'}      = $Opts{'cfgfile'} ? $Opts{'cfgfile'} : ($ENV{'NACHTMAKE_CFG'} ? $ENV{'NACHTMAKE_CFG'} : "Nachtmake.xml");
    $hashref->{'ERR_OUTFILE'}   = defined ($Opts{'errout'}) ? "$Opts{'errout'}" : "";

    print "Private = $hashref->{'PRIVATE_MAKE'}\n";

    if (defined $Opts{'make_type'})
    {
        if (($Opts{'make_type'} eq 'OLTP')  or
            ($Opts{'make_type'} eq 'LC')    or
             ($Opts{'make_type'} eq 'ALL'))
        {
            $hashref->{'MAKE_TYPE'} = $Opts{'make_type'};
        }
        elsif    ($Opts{'make_type'} eq 'LC_OLTP')
        {
            $hashref->{'MAKE_TYPE'} = "ALL";
        }
        else
        {
            print STDERR "ERROR: Wrong make type >>>$Opts{'make_type'}<<<!\n";
            return;
        }
    }

    if ($Check)
    {
        ShowHashRef($hashref);
    }

    return($hashref);
}

################################################################################
# CheckConfigIntegrity() - Checks for errors in configuration.
################################################################################

sub CheckConfigIntegrity
{
    ProtHeader("Checking configuration integrity");

    if ($Glob->{'64_BIT_ONLY'} and $Glob->{'32_BIT_ONLY'})
    {
        print STDERR "ERROR: '64_BIT_ONLY' *AND* '32_BIT_ONLY' are set! This makes no sense...\n";
        CleanExit(1);
    }

    if ($Glob->{'CLI_32_BIT'} and $Glob->{'PCR_32_BIT'})
    {
        print STDERR "ERROR: '-cli_bit_32' *AND* '-pcr_bit_32' are set! This makes no sense...\n";
        CleanExit(1);
    }

    if ($Glob->{'64_BIT_ONLY'})
    {
        if ($Glob->{'32_BIT_ONLY'} or $Glob->{'CLI_32_BIT'} or $Glob->{'PCR_32_BIT'})
        {
            $Glob->{'32_BIT_ONLY'}  = "0";
            $Glob->{'CLI_32_BIT'}   = "0";
            $Glob->{'PCR_32_BIT'}   = "0";
            print "WARNING: No 32 bit client makes allowed.\n" ;
        }
    }
    elsif ($Glob->{'CLI_32_BIT'} or $Glob->{'PCR_32_BIT'})
    {
        $Glob->{'32_BIT_ONLY'} = "1";
    }

    if (!$Glob->{'32_BIT_ONLY'} and !$Glob->{'64_BIT_ONLY'})
    {
        push @{$Glob->{'DEL_TREE'}}, ("sys" . $Glob->{'PATH_SEP'} . "wrk32");
    }

    if ($Glob->{'REL_VER'} eq "")
    {
        print STDERR "ERROR: Wrong Release >>>$Glob->{'REL_VER'}<<<\n"; CleanExit(1);
    }

    #-------------------------------------------------------------------------------
    #
    #-------------------------------------------------------------------------------

    if ( ! defined $Glob->{'MAKE_TYPE'})
    {
        print "WARNING: MAKE_TYPE not defined\n";
        print "         set MAKE_TPYE to 'ALL'\n";
        $Glob->{'MAKE_TYPE'} = "ALL";
    }
    elsif ( $Glob->{'MAKE_TYPE'} =~ /^LC_OLTP$/)
    {
        print "WARNING: MAKE_TYPE have a obsolete value (LC_OLTP)\n";
        print "         set MAKE_TPYE to 'ALL'\n";
        $Glob->{'MAKE_TYPE'} = "ALL";
    }
    
    if ($Glob->{'REL_VER'} =~ /73/) {
        print "INFO: QADB support disabled for release 73.\n";
        $Glob->{'NO_QADB'} = 1;
    }

    if ($Check)
    {
        print "64_BIT_ONLY      = $Glob->{'64_BIT_ONLY'}\n";
        print "32_BIT_ONLY      = $Glob->{'32_BIT_ONLY'}\n";
        print "CLI_32_BIT       = $Glob->{'CLI_32_BIT'}\n";
        print "PCR_32_BIT       = $Glob->{'PCR_32_BIT'}\n";
        print "REL_VER          = $Glob->{'REL_VER'}\n";
        print "CORR_LEVEL       = $Glob->{'CORR_LEVEL'}\n";
        print "BUILD_PREFIX     = $Glob->{'BUILD_PREFIX'}\n";
        print "DEL_TREE         = " . join(", ", @{$Glob->{'DEL_TREE'}}) . "\n";
    }
}

################################################################################
# CleanExit() - Cleans up and exits the module.
################################################################################

sub CleanExit
{
    $RC = shift;

    unlink("$Glob->{'PROT_DIR'}$Glob->{'PATH_SEP'}build.pid");

    goto PROGRAM_END;
}


################################################################################
# ExpandPath() - Takes a protocol array and adds paths to it.
################################################################################

sub ExpandPath
{
    my $arrname = shift;
    my @arr     = @_;
    my @erg     = ();

    foreach my $target (@arr)
    {
        push @erg, $Glob->{$arrname . '_PROT'} . $target . '.e0';
    }

    return @erg;
}

################################################################################
# FillPlatformHash() - Reads platform settings from a XML-File
################################################################################

sub FillPlatformHash
{
    my $XMLFile  = shift;
    my $platform = shift;
    my $node;
    my $parser   = new XML::DOM::Parser or return;
    my $doc      = $parser->parsefile($XMLFile) or return;

    foreach my $elem ($doc->getElementsByTagName("platform"))
    {
        if ($elem->getAttribute("id") =~ /^\Q$platform\E$/i) { $node = $elem; last; }
    }

    return 0 unless ($node);

    foreach my $elem ($node->getChildNodes())
    {
        if (($elem->getNodeType() == 1) and ($elem->getNodeName() eq "option"))
        {
            my $name = $elem->getAttribute("name");
            my $type = $elem->getAttribute("type");
            my $val  = $elem->getFirstChild()->getData();

            if      ($type eq "scalar") { $$Glob{$name} = $elem->getFirstChild()->getData() }
            elsif   ($type eq "array")
            {
                my @tmparr = split(/,/, $elem->getFirstChild()->getData());
                $Glob->{$name} = \@tmparr;
            }
            elsif   ($type eq "hash")
            {
                my %tmphash     = split(/,/, $elem->getFirstChild()->getData());
                $Glob->{$name}  = \%tmphash;
            }
            elsif ($type eq "strevl")
            {
                eval '$Glob->{$name} = "' . $elem->getFirstChild()->getData() . '";';
                if ($@) { print STDERR "XMLparse eval error: $@\n" }
            }
            elsif ($type eq "cmdevl")
            {
                eval '$Glob->{$name} = ' . $elem->getFirstChild()->getData() . ';';
                if ($@) { print STDERR "XMLparse eval error: $@\n" }
            }
            else { print STDERR "XMLparse type error: Unknown option format found ($name, $type, $val)\n" }
        }
    }

    return 1;
}

################################################################################
# GetLCPoolID() - Retrieves the correct LC pool ID
################################################################################

sub GetLCPoolID
{
    my $cmdstr;
    my $erg;

    if (!$Glob->{'LB_SYM_LINK'})
    {
        return "NONE";
    }

    if($^O=~/.*win32.*/i)
    {
        require WinLink;
        import WinLink;
        $erg = readLink ("$Glob->{'LB_SYM_LINK'}");
    }
    else
    {   $erg = readlink "$Glob->{'LB_SYM_LINK'}"; }

    unless (defined $erg)
    {
        QADBWriteLog("ERROR: Fetching LC_POOL_ID failed! [File: '$Glob->{'LB_SYM_LINK'}']");
        $erg = "0";
        print STDERR "Fetching LC_POOL_ID failed! [File: '$Glob->{'LB_SYM_LINK'}']\n";
    }

    print "LB_SYM_LINK      = $Glob->{'LB_SYM_LINK'}\n" if ($Glob->{'LB_SYM_LINK'});
    print "LC_POOL_ID       = $erg\n";

    return $erg;
}

################################################################################
# HashRefToGlobVar() - Copies a hash into the global variable.
################################################################################

sub HashRefToGlobVar
{
    my $hashref = shift;
    foreach (keys %{$hashref}) { $Glob->{$_} = $hashref->{$_} }
}

################################################################################
# MakeTarget() - Makes a target given as parameter
################################################################################

sub MakeTarget
{
    my $targetname  = shift;
    my $longname    = shift;
    my $resname     = shift;
    my $imf_erg     = 1;
    my $do_make;

    print "Making $longname...\n\n";
    QADBWriteLog("[" . (scalar localtime) . "] MAKE START: Make of '$longname' started.");

    QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{$targetname . '_MAKE_START'}});

    for (my $count = 1; $count <= $Glob->{$targetname . '_RUNS'}; $count ++)
    {
        $Glob->{$resname} = 1;

        my $last = ($count == $Glob->{$targetname . '_RUNS'}) ? 1 : 0;

        foreach my $trg (@{$Glob->{$targetname . "_PARTS"}})
        {
            my @opts    = @{$Glob->{$trg . "_OPT"}};
            my @targets = @{$Glob->{$trg . "_TRG"}};
            $do_make    = 0;

            eval ('$do_make = ' . "$opts[0];");

            if ($@)
            {
                print STDERR "ERROR: parsing '\$do_make' -> $@\n";
                $Glob->{$resname} = 0;
                QADBWriteLog("SCRIPT ERROR: Parse error while parsing make condition '$opts[0]'! [SEC='$longname', AFFECTED TARGETS='@targets', ERRORTEXT='$@']");
            }
            elsif ($do_make)
            {
                my $sharpmake = (defined $opts[3] ? ($opts[3] == 0 ? 0 : $last) : $last);

                if ($sharpmake)
                {
                    foreach my $target (@targets)
                    {
                        my $tmptrg = $target;
                        $tmptrg =~ s/://g;
                        unless ($tmptrg =~ /^-.*$/) { push @{$Glob->{$opts[2]}}, $tmptrg }
                    }
                }

                if ($Check)
                {
                    print "MAKE: $trg STEP: $count LAST: " . ($last ? "yes" : "no") . " SHARP: " . ($sharpmake ? "yes" : "no") . "\n";
                    print "CMD : '$opts[1](@targets)'\n";
                    print "$opts[2] = (" . join(", ", @{$Glob->{$opts[2]}}) . ")\n" if ($sharpmake);
                    $imf_erg = 0;
                    $Glob->{$resname} = 1;
                }
                else
                {
                    QADBWriteLog("START: '$opts[1](@targets)' started." . ($sharpmake ? "" : " (Errors will not be logged.)"));
                    print "[" . (scalar localtime) . "] Start making '$opts[1](@targets)'" . ($sharpmake ? "...\n" : " (Errors ignored)...\n");
                    $Glob->{'STATISTICS'}->{'TARGET_SUM'} += 1;
                    eval ('$imf_erg = ' . "$opts[1]" . '(@targets);');
                    if ($@)
                    {
                        print "[" . (scalar localtime) . "] Make/Parse error ($opts[1](@targets)): $@\n";
                        if ($sharpmake)
                        {
                            $Glob->{$resname} = 0;
                            $Glob->{'STATISTICS'}->{'TARGET_FAIL'} += 1;
                            QADBWriteLog("SCRIPT ERROR: Parse error while parsing make command '$opts[1]'! [SEC='$longname', AFFECTED TARGETS='@targets', ERRORTEXT='$@']");
                            if ($Glob->{'LC_POOL_ERROR_SET'} == 0) {
                                QADBUpdateCol({'LCPOOLID' => '!run'});
                                $Glob->{'LC_POOL_ERROR_SET'} = 1;                            
                            }
                        }
                    }
                    elsif($imf_erg != 0)
                    {
                        if ($sharpmake)
                        {
                            $Glob->{$resname} = 0;
                            $Glob->{'STATISTICS'}->{'TARGET_FAIL'} += 1;
                            QADBWriteLog("ERROR: '$opts[1](@targets)' reported errors! [SEC='$longname', ERROR CODE='$imf_erg']");
                            if ($Glob->{'LC_POOL_ERROR_SET'} == 0) {
                                QADBUpdateCol({'LCPOOLID' => '!run'});
                                $Glob->{'LC_POOL_ERROR_SET'} = 1;                            
                            }                        
                        }
                        print "[" . (scalar localtime) . "] Make of '$opts[1](@targets)' *FAILED*!\n";
                    }
                    else
                    {
                        QADBWriteLog("SUCCESS: '$opts[1](@targets)' successfully finished.") if ($sharpmake);
                        print "[" . (scalar localtime) . "] Make of '$opts[1](@targets)' is OK.\n";
                    }
                }
            }
            else
            {
                QADBWriteLog("INFO: Skipping '$opts[1](@targets)' since make rule does not apply.");
                print "Skipping '$opts[1](@targets)' since make rule does not apply.\n";
            }
        }
    }

    QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{$targetname . '_MAKE_END'}});

    if ($Glob->{$resname})
    {
        QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{$targetname . '_MAKE_OK'}});
        QADBWriteLog("MAKE SUCCESS: Make of '$longname' was successful.");
        print "\nMake of $longname is OK!\n";
    }
    else
    {
        QADBUpdateCol({'IDOBJSTATUS' => $Glob->{'QA_STATUS'}->{$targetname . '_MAKE_NOT_OK'}});
        QADBWriteLog("MAKE ERROR: Make of '$longname' was not successful.");
        print STDERR "\nERROR: Can't build $longname!\n";
    }
}

################################################################################
# ProtHeader() - Prints a small protocol header to STDOUT.
################################################################################

sub ProtHeader
{
   my $prot_part = shift ;

   print "\n\n===========================================================\n" ;
   print "=   $prot_part\n" ;
   print "===========================================================\n\n" ;
}

################################################################################
# QADBCheckHandle() - Checks whether the QA handle is ok or not.
################################################################################

sub QADBCheckHandle
{
    return 1 if ($Glob->{'NO_QADB'});
    if (($Glob->{'QAH'}->{'error_code'} != 0) or (!$Glob->{'QA_ID'}))
    {
        if ($Glob->{'QAH'}->{'error_code'} != 0)
        {
            print STDERR "ERROR: QADB got an error: '$Glob->{'QAH'}->{'error_text'}'\n" ;
        }
        else
        {
            print STDERR "ERROR: QADB handle is zero!\n";
        }

        QADBCreateHandle();

        if (($Glob->{'QAH'}->{'error_code'} != 0) or (!$Glob->{'QA_ID'}))
        {
        return 0;
        }
    }

    return 1;
}

################################################################################
# QADBCreateHandle() - Creates a new QADB handle
################################################################################

sub QADBCreateHandle
{
    # Create a new QADB handle:

    $Glob->{'QAH'} = undef;
    return 1 if ($Glob->{'NO_QADB'});

    if ($Check || $Glob->{'PRIVATE_MAKE'})
    {
        if ($Glob->{'QA_ID'} > 0)
        {
            print "qadb -> new ( {'ID'        => \"$Glob->{'QA_ID'}\" } );\n";
        }
        else
        {
            if ($Glob->{'QADB_SYSNAME'})
            {
                print "qadb -> new ( {'VERSION'         => \"$Glob->{'REL_VER'}$Glob->{'CORR_LEVEL'}\" ,\n";
                print "               'BUILDPFX'        => \"$Glob->{'BUILD_PREFIX'}\" ,\n";
                print "               'QASTATUS'        => \"$Glob->{'LC_STATE'}\" ,\n";
                print "               'CHANGELIST'      => \"$Glob->{'CHLIST_NUM'}\" ,\n";
                print "               'PLATFORM'        => \"$Glob->{'QADB_SYSNAME'}\" ,\n";
                print "               'BUILD_PURPOSE'   => \"$Glob->{'BUILD_PURPOSE'}\" });\n";
            }
            else
            {
                print "qadb -> new ( {'VERSION'         => \"$Glob->{'REL_VER'}$Glob->{'CORR_LEVEL'}\" ,\n";
                print "               'BUILDPFX'        => \"$Glob->{'BUILD_PREFIX'}\" ,\n";
                print "               'QASTATUS'        => \"$Glob->{'LC_STATE'}\" ,\n";
                print "               'CHANGELIST'      => \"$Glob->{'CHLIST_NUM'}\" ,\n";
                print "               'BUILD_PURPOSE'   => \"$Glob->{'BUILD_PURPOSE'}\" });\n";
            }
        }
    }
    else
    {
        if ($Glob->{'QA_ID'} > 0)
        {
            $Glob->{'QAH'} = qadb->new( {'ID'        => "$Glob->{'QA_ID'}" } );
        }
        else
        {
            if ($Glob->{'QADB_SYSNAME'})
            {
                $Glob->{'QAH'} = qadb->new( {'VERSION'          => "$Glob->{'REL_VER'}$Glob->{'CORR_LEVEL'}" ,
                                             'BUILDPFX'         => "$Glob->{'BUILD_PREFIX'}" ,
                                             'QASTATUS'         => "$Glob->{'LC_STATE'}" ,
                                             'CHANGELIST'       => "$Glob->{'CHLIST_NUM'}" ,
                                             'PLATFORM'         => "$Glob->{'QADB_SYSNAME'}",
                                             'BUILD_PURPOSE'    => "$Glob->{'BUILD_PURPOSE'}" });
            }
            else
            {
                $Glob->{'QAH'} = qadb->new( {'VERSION'          => "$Glob->{'REL_VER'}$Glob->{'CORR_LEVEL'}" ,
                                             'BUILDPFX'         => "$Glob->{'BUILD_PREFIX'}" ,
                                             'QASTATUS'         => "$Glob->{'LC_STATE'}" ,
                                             'CHANGELIST'       => "$Glob->{'CHLIST_NUM'}" ,
                                             'BUILD_PURPOSE'    => "$Glob->{'BUILD_PURPOSE'}" });
            }
        }

        $Glob->{'QA_ID'} = (($Glob->{'QAH'}->{'error_code'} == 0) ? $Glob->{'QAH'}->{'ID'} : 0);
        
        if (($Glob->{'QA_ID'} > 0) && ($Glob->{'QA_ID_WRITTEN'} == 0))
        {
            WriteFileLine($Glob->{'MAKEID_FILE'}, $Glob->{'QA_ID'});
            $ENV{'MAKEID'} = $Glob->{'QA_ID'};
            $Glob->{'QA_ID_WRITTEN'} = 1;
        }
    }
}

################################################################################
# QADBCreateTestprofile() - Adds a test-profile for this make.
################################################################################

sub QADBCreateTestprofile
{
    my ($profile) = @_;

    return if($Glob->{'NO_QADB'});
    
    if ($Check || $Glob->{'PRIVATE_MAKE'})
    {
        print "QADB ($Glob->{'QA_ID'}): adding test profile: '$profile'\n";
        return;
    }

    if (QADBCheckHandle())
    {
        $Glob->{'QAH'}->create_testprofile($profile);
        QADBCheckHandle();
    }
    else
    {
        print STDERR "ERROR: Can't create QADB test profile because of an open error\n" ;
    }
}

################################################################################
# QADBUpdateCol() - updates a column in the QADB
################################################################################

sub QADBUpdateCol
{
    my ($hash) = @_;

    return if ($Glob->{'NO_QADB'});
    
    if ($Check || $Glob->{'PRIVATE_MAKE'})
    {
        print "QADB ($Glob->{'QA_ID'}): updating col(s):\n";
        foreach my $key (keys %$hash) { print "    '$key' => '$hash->{$key}'\n"; }
        return;
    }

    if (QADBCheckHandle())
    {
        $Glob->{'QAH'}->update_columns($hash);
        QADBCheckHandle();
    }
    else
    {
        print STDERR "ERROR: Can't update QADB because of an open error\n" ;
    }
}

################################################################################
# QADBWriteLog() - writes a log entry into QADB
################################################################################

sub QADBWriteLog
{
    my $logtext = shift;

    return if($Glob->{'NO_QADB'});
    
    if ($Check || $Glob->{'PRIVATE_MAKE'})
    {
        print "QADB ($Glob->{'QA_ID'}): writing log entry '$logtext'\n" ;
        return;
    }

    if (QADBCheckHandle())
    {
        $Glob->{'QAH'}->write_log($logtext);
        QADBCheckHandle();
    }
    else
    {
        print STDERR "ERROR: Can't write log entry because of an open error\n" ;
    }
}

################################################################################
# QADBWriteProt() - writes a protocol file into QADB
################################################################################

sub QADBWriteProt
{
    my $protpath = shift;
    my $protname = shift;
    my $showname = shift;
    my $protocol;
    my $line = 1;
    my $rc = 0;
    
    if ($Check || $Glob->{'PRIVATE_MAKE'})
    {
        print "QADB ($Glob->{'QA_ID'}): writing protocol file '$protname'\n" ;
        return;
    }

    if ($Glob->{'HTML_OUTPUT'})
    {
        $protocol = "<HTML>\n<BODY>\n<PRE>\n";
    }

    unless (open (IN_FILE, "< $protpath"))
    { 
        print "Error: Can't open $protpath for reading.\n";
        return 0;
    }
    while (<IN_FILE>)
    {
        if ($Glob->{'HTML_OUTPUT'})
        {
            chomp;
            $protocol .= "<A NAME=$line>" . $_ . "</A>\n";
            $line = $line + 1;
        }
        else
        {
            $protocol .= $_;
        }
    }
    close (IN_FILE);
    
    if ($Glob->{'HTML_OUTPUT'})
    {
        $protocol .= "</PRE>\n</BODY>\n</HTML>\n";
    }
    
    if ($Glob->{'NO_QADB'}) {
        if (open (OUT_FILE,">$protname")) {
            print OUT_FILE $protocol;
            close (OUT_FILE)
        }
        if ($^O=~/.*win32.*/i)
        {   $rc = system("python $ENV{TOOL}\\bin\\webdav.py $protname"); }
        else
        {   $rc = system("webdav.py $protname");   }
        $Glob->{'WEBDAV_PROT'} = "http://pts.wdf.sap.corp:1080/webdav/daily_makes/$protname";
        unlink($protname);
    } else {
        if (QADBCheckHandle())
        {
            if (length ($protocol) > 8000)
            {
                eval "require SDB::Protocol::Tools";
                if ($@)
                {	print STDERR "Warning: Error while loading SDB::Protocol::Tools\n($@)\nCompressing file failed - using uncompressed file\n" ;	}
                else
                {
                    import SDB::Protocol::Tools;
                    my $ref_to_zip;
                    my $zip_mode = 0;
		    		   		
                    eval '($ref_to_zip, $zip_mode) = SDB::Protocol::Tools::PackScalar(\$protocol, $protname , {\'toDOS\' => 1});';
                    if ($@) 
                    {	print STDERR "Warning: Error while compress protocol\n($@)\nUsing uncompressed file\n" ;	}
                    else
                    {	$zip_mode and $protname .= ".zip"; }
                }
            }
            if ($showname)
            {
                $Glob->{'WEBDAV_PROT'} = $Glob->{'QAH'}->write_prot("$protname", $protocol, $showname);
            }
            else
            {
                $Glob->{'WEBDAV_PROT'} = $Glob->{'QAH'}->write_prot("$protname", $protocol);
            }
            $rc = QADBCheckHandle();
        }
        else
        {
            print STDERR "ERROR: Can't write protocol because of an open error\n" ;
        }
    }
    return $rc;
}

################################################################################
# ReadFileLines() - Reads [n] lines from a file
################################################################################

sub ReadFileLines
{
    my $file    = shift;
    my $lines   = shift;
    my $line    = 1;
    my $tmpvar;

    unless (open (IN_FILE, "<$file"))
    {
        print STDERR "ERROR: Can't open file '$file'!\n" ;
        return 0;
    }

    while (<IN_FILE>)
    {
        chomp $_;
        $tmpvar .= ($tmpvar eq "" ? "$_" : "\n$_");
        last if ($line == $lines);
        $line ++;
    }

    close (IN_FILE) or print STDERR "ERROR: Can't close file '$file'!\n$!\n";

    return $tmpvar;
}

################################################################################
# ShowHashRef() - Displays a hash reference on screen.
################################################################################

sub ShowHashRef
{
    my $ref = shift;

    foreach my $key (sort keys %{$ref})
    {
        if (ref($ref->{$key}) eq "ARRAY")
        {
            my $tmpstr = sprintf("%-16s = %s\n", $key, join(", ", @{$ref->{$key}}));
            print $tmpstr;
        }
        elsif (ref($ref->{$key}) eq "HASH")
        {
            my $tmpstr = sprintf("%-16s = HASH:\n", $key);
            print $tmpstr;
            foreach my $hkey (sort keys %{$ref->{$key}})
            {
                print "    $hkey => $ref->{$key}->{$hkey}\n";
            }
        }
        else
        {
            my $tmpstr = sprintf("%-16s = %s\n", $key, $ref->{$key});
            print $tmpstr;
        }
    }
}

################################################################################
# Usage() - Prints out the program usage and exits.
################################################################################

sub Usage
{
    print "usage: $0\n\n";
    print "       [-force_rm]\n" ;
    print "          initialise a scratch make\n\n" ;
    print "       [-cli_bit_32]\n" ;
    print "          make the 32 bit client package\n\n" ;
    print "       [-pcr_bit_32]\n" ;
    print "          make the 32 bit precompiler runtime library\n\n" ;
    print "       [-make_type=<OLTP | LC | LC_OLTP>]\n" ;
    print "          specifies the type of the make, default is LC_OLTP\n\n" ;
    print "       [-check]\n" ;
    print "          checks and shows only the workflow, don't execute anything\n\n" ;
    print "       [-private]\n";
    print "          initiates a private make which doesn't produces QADB entries\n\n";
    print "       [-email <email-adress>]\n";
    print "          Just sends emails to the given email-adress.\n\n";
    print "       [-html]\n";
    print "          Produces outputs in HTML format.\n\n";
    print "       [-help | -h]\n" ;
    print "          show this help message\n" ;

    CleanExit(1);
}

################################################################################
# WriteFileLine() - Writes a string into a file
################################################################################

sub WriteFileLine
{
    my ($file, $line) = @_;

    unless (open (OUT_FILE, ">$file"))
    {
        print "WARNING: Can't open file '$file' for writing!\n" ;
        return 0;
    }

    print OUT_FILE $line;
    close (OUT_FILE);
}

################################################################################

PROGRAM_END:

caller () ? return ( $RC ) : exit ( $RC );
