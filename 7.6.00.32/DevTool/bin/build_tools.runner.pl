#!/usr/bin/perl
#
#    build_tools - Performs a productive make of tools.
#
#    @(#)build_tools.pl     2003-02-24
#
#    UliJ, SAP AG
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

use strict;
use Getopt::Long;
use File::Path;
use imake;
use qadbtool;

################################################################################
# Globals:
################################################################################

my $ProgramName     = "build_tools";
my $Version         = "1";
my $Revision        = "04";
my $Date            = "2003-06-04";

my $Glob            =  {};
my %Opts;
my @LocalTime       = localtime (time);
my @ErrorProts;

################################################################################
# Global program constants:
################################################################################

$Glob->{'QA_STATUS'}->{'TOOL_MAKE_START'}      = 21;
$Glob->{'QA_STATUS'}->{'TOOL_MAKE_END'}        = 22;
$Glob->{'QA_STATUS'}->{'TOOL_MAKE_NOT_OK'}     = 29;
$Glob->{'QA_STATUS'}->{'TOOL_MAKE_OK'}         = 30;

$Glob->{'TOOL_RUNS'}    = 1;
$Glob->{'TOOL_PARTS'}   = ['VMAKE', 'ALLTOOLS'];
$Glob->{'TOOL_PROT'}    = "$ENV{OWN}/sys/wrk/fast/prot/";

$Glob->{'VMAKE_TRG'}    = ['vmake.mac'];
$Glob->{'VMAKE_OPT'}    = ['1', 'imf', 'MAKE_TARGETS'];
$Glob->{'ALLTOOLS_TRG'} = ['alltools.mac'];
$Glob->{'ALLTOOLS_OPT'} = ['1', 'imf', 'MAKE_TARGETS'];

################################################################################
# Main routine:
################################################################################

select(STDERR);$| = 1;
select(STDOUT);$| = 1;

print "\n$ProgramName v$Version.$Revision ($Date), (c)2003 SAP AG\n\n";

if (!GetOptions( \%Opts, 'h|?|help', 'private', 'scratch', 'email=s', 'machine=s', 'toolrel=s') || @ARGV)
{
    Usage(1);
}
elsif ($Opts{'h'})
{
    Usage(0);
}

CheckCmdLineParams();

$Glob->{'OWN'}            = $ENV{'OWN'}            ? $ENV{'OWN'}            : _throw_error("main", "Mandatory environment variable 'OWN' is not set!\nPlease check your environment configuration.");
$Glob->{'TOOL'}           = $ENV{'TOOL'}           ? $ENV{'TOOL'}           : _throw_error("main", "Mandatory environment variable 'TOOL' is not set!\nPlease check your environment configuration.");
$Glob->{'TOOLSHELL'}      = $ENV{'TOOLSHELL'}      ? $ENV{'TOOLSHELL'}      : _throw_error("main", "Mandatory environment variable 'TOOLSHELL' is not set!\nPlease check your environment configuration.");
$Glob->{'TOOLREL'}        = ($Glob->{'TOOLREL'}? $Glob->{'TOOLREL'} : ($ENV{'TOOLREL'}        ? $ENV{'TOOLREL'}        : _throw_error("main", "Mandatory environment variable 'TOOLREL' is not set!\nPlease check your environment configuration!")));
$Glob->{'MACHINE_CONFIG'} = ($Glob->{'MACHINE_CONFIG'} ? $Glob->{'MACHINE_CONFIG'} : ($ENV{'MACHINE_CONFIG'} ? $ENV{'MACHINE_CONFIG'} : _throw_error("main", "Mandatory environment variable 'MACHINE_CONFIG' is not set!\nPlease check your environment configuration!")));
$Glob->{'HOST_NAME'}      = (`hostname` =~ /^(\S+).*/ ? "$1" : "");
$Glob->{'PROT_PATH'}      = "$Glob->{'OWN'}/prot";
$Glob->{'PROT_FILE'}      = sprintf("%s_%s_%s_%02d%s%04d_%02d%02d.prot", $ProgramName, $Glob->{'TOOLREL'}, $Glob->{'HOST_NAME'}, $LocalTime[3], ('Jan','Feb','Mar','Apr','Mai','Jun','Jul','Aug','Sep','Oct','Nov','Dec')[$LocalTime[4]], $LocalTime[5] + 1900, $LocalTime[2], $LocalTime[1]);

#-------------------------------------------------------------------------------

ProtHeader("Redirecting STDOUT and STDERR");

mkpath($Glob->{'PROT_PATH'}, 1, 0777) unless (-d $Glob->{'PROT_PATH'});

print "OUTPUT_FILTER: OPEN FILE $Glob->{'PROT_PATH'}/$Glob->{'PROT_FILE'}\n";

#-------------------------------------------------------------------------------

unless ($Glob->{'PRIVATE'})
{
    ProtHeader("Reading changelist number");

    unless (-e "$Glob->{'OWN'}/changelist.log")
    {
        print "Warning: $Glob->{'OWN'}/changelist.log not found! Setting change list to zero!\n";
        $Glob->{'CHANGE_LIST'} = 0;
    }

    $Glob->{'CHANGE_LIST'} = ReadFileLines("$Glob->{'OWN'}/changelist.log", 1);

    unless ($Glob->{'CHANGE_LIST'})
    {
        $Glob->{'CHANGE_LIST'} = 0;
    }
}

#-------------------------------------------------------------------------------

unless ($Glob->{'PRIVATE'})
{
    ProtHeader("Creating new QADBTOOL handle");
    QADBCreateID();
}

#-------------------------------------------------------------------------------

ProtHeader("Storing build PID");

WriteFileLine("$Glob->{'PROT_PATH'}/build.pid", "$$");

#-------------------------------------------------------------------------------

unless ($Glob->{'PRIVATE'})
{
    ProtHeader("Storing Make ID");
    WriteFileLine("$Glob->{'OWN'}/make.id", "$Glob->{'QA_ID'}");
    if ($Glob->{'QA_ID'} == 0)
    {
        print "WARNING! QA-ID is zero!\n";
    }
}

#-------------------------------------------------------------------------------

ProtHeader("Preparing Make");

if ($Glob->{'SCRATCH'})
{
    print "Cleaning-up for scratch make...\n";
    rmtree (['sys/wrk', 'usr', 'test'], 1, 0);
}

#-------------------------------------------------------------------------------

ProtHeader("Making target 'TOOL'");

MakeTarget("TOOL", "SAP DB Tools", "MAKE_RESULT");

#-------------------------------------------------------------------------------

unless ($Glob->{'MAKE_RESULT'})
{
    ProtHeader("Collecting error protocols");

    push @{$Glob->{'TARGETS_WITH_PATH'}}, ExpandPath("TOOL", @{$Glob->{'MAKE_TARGETS'}});

    #---------------------------------------------------------------------------

    ProtHeader("Sending error mails");

    my $ret     = 0;
    my @params;

    #$Glob->{'EMAIL'} = 'ulrich.jansen@sap.com,falko.flessner@sap.com,gerald.arnold@sap.com';

    push @params, ('-qaid', "$Glob->{'QA_ID'}") unless ($Glob->{'PRIVATE'});
    push @params, ('-email', "$Glob->{'EMAIL'}") if ($Glob->{'EMAIL'});
    push @params, ('-chlist', "$Glob->{'CHANGE_LIST'}") if ($Glob->{'CHANGE_LIST'});
    push @params, @{$Glob->{'TARGETS_WITH_PATH'}};

    if (system("$ENV{'TOOLSHELL'}", "$ENV{'TOOL'}/bin/checkmake.pl", @params) != 0)
    {
        print "ERROR: file \"checkmake.pl\" failed\n";
    }
}

#-------------------------------------------------------------------------------

ProtHeader("Resetting redirection of STDOUT and STDERR");

print "OUTPUT_FILTER: CLOSE FILE\n";

#-------------------------------------------------------------------------------

unless ($Glob->{'PRIVATE'})
{
    ProtHeader("Writing make protocol into QADB");
    QADBWriteProt("$Glob->{'PROT_PATH'}/$Glob->{'PROT_FILE'}", "$Glob->{'PROT_FILE'}", "Tool Make Protocol (complete)");
}

################################################################################
# CheckCmdLineParams() - Checks the command line parameters for correctness.
################################################################################

sub CheckCmdLineParams
{
    $Glob->{'PRIVATE'}  = $Opts{'private'} ? 1 : 0;
    $Glob->{'SCRATCH'}  = $Opts{'scratch'} ? 1 : 0;
    $Glob->{'EMAIL'}    = $Opts{'email'}   ? $Opts{'email'} : "";
    $Glob->{'MACHINE_CONFIG'} = $Opts{'machine'} ? $Opts{'machine'} : "";
    $Glob->{'TOOLREL'} = $Opts{'toolrel'} ? $Opts{'toolrel'} : "";
}

################################################################################
# CleanExit() - Clears all dirt and the exits.
################################################################################

sub CleanExit
{
    my $retcode = shift;

    exit ($retcode);
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
    QADBWriteLog("MAKE START: Make of '$longname' started.");

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

                QADBWriteLog("START: '$opts[1](@targets)' started." . ($sharpmake ? "" : " (Errors will not be logged.)"));
                print "Start making '$opts[1](@targets)'...\n";
                eval ('$imf_erg = ' . "$opts[1]" . '(@targets);');
                if ($@)
                {
                    print STDERR "Make/Parse error ($opts[1](@targets)): $@\n";
                    $Glob->{$resname} = 0 if ($sharpmake);
                    QADBWriteLog("SCRIPT ERROR: Parse error while parsing make command '$opts[1]'! [SEC='$longname', AFFECTED TARGETS='@targets', ERRORTEXT='$@']") if ($sharpmake);
                }
                elsif($imf_erg != 0)
                {
                    $Glob->{$resname} = 0 if ($sharpmake);
                    QADBWriteLog("ERROR: '$opts[1](@targets)' reported errors! [SEC='$longname', ERROR CODE='$imf_erg']") if ($sharpmake);
                    print "Make of '$opts[1](@targets)' *FAILED*!\n";
                }
                else
                {
                    QADBWriteLog("SUCCESS: '$opts[1](@targets)' successfully finished.") if ($sharpmake);
                    print "Make of '$opts[1](@targets)' is OK.\n";
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
# QADBCreateID() - Creates a new QADB ID
################################################################################

sub QADBCreateID
{
    # Create a new QADB handle:

    unless ($Glob->{'PRIVATE'})
    {
        print "QADB: Creating new QADB connection\n\n";
        my $qah = qadbtool->new( {'TOOLRELEASE' => "$Glob->{'TOOLREL'}" ,
                                  'CHANGELIST'  => "$Glob->{'CHANGE_LIST'}" ,
                                  'PLATFORM'    => "$Glob->{'MACHINE_CONFIG'}" });

        if ($qah->{'error_code'} == 0)
        {
            print "QADB: QA ID has been set to '$qah->{'ID'}'.\n";
            $Glob->{'QA_ID'} = $qah->{'ID'};
        }
        else
        {
            print "QADB: ERROR! Creating / reopening QADB handle *FAILED*!\n";
            print "Error text: '$qah->{'error_text'}'\n";
            $Glob->{'QA_ID'} = 0;
        }
    }
}

################################################################################
# QADBUpdateCol() - updates a column in the QADB
################################################################################

sub QADBUpdateCol
{
    my ($hash) = @_;

    return if ($Glob->{'PRIVATE'});

    if ($Glob->{'QA_ID'} == 0)
    {
        print "WARNING! MakeID is ZERO! Cannot update QADB columns.\n";
        return;
    }

    my $qah = qadbtool->new( {'ID' => "$Glob->{'QA_ID'}" } );

    if ($qah->{'error_code'} == 0)
    {
        $qah->update_columns($hash);

        if ($qah->{'error_code'} != 0)
        {
            print "QADB-ERROR while updating column:\n$qah->{'error_text'}\n";
        }
    }
    else
    {
        print "ERROR: Can't update QADB because of an open error\n" ;
    }
}

################################################################################
# QADBWriteLog() - writes a log entry into QADB
################################################################################

sub QADBWriteLog
{
    my $logtext = shift;

    return if ($Glob->{'PRIVATE'});

    if ($Glob->{'QA_ID'} == 0)
    {
        print "WARNING! MakeID is ZERO! Cannot wite log to QADB.\n";
        return;
    }

    my $qah = qadbtool->new( {'ID' => "$Glob->{'QA_ID'}" } );

    if ($qah->{'error_code'} == 0)
    {
        $qah->write_log($logtext);

        if ($qah->{'error_code'} != 0)
        {
            print "QADB-ERROR while writing log:\n$qah->{'error_text'}\n";
        }
    }
    else
    {
        print "ERROR: Can't write QADB log entry because of an open error\n" ;
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

    return if ($Glob->{'PRIVATE'});

    if ($Glob->{'QA_ID'} == 0)
    {
        print "WARNING! MakeID is ZERO! Cannot wite protocol to QADB.\n";
        return;
    }

    my $qah      = qadbtool->new( {'ID' => "$Glob->{'QA_ID'}" } );
    my $protocol = ReadFileLines($protpath, 0);

    if ($qah->{'error_code'} == 0)
    {
        if ($showname)
        {
            $qah->write_prot("$protname", $protocol, $showname);
        }
        else
        {
            $qah->write_prot("$protname", $protocol);
        }

        if ($qah->{'error_code'} != 0)
        {
            print "QADB-ERROR while writing protocol:\n$qah->{'error_text'}\n";
        }
    }
    else
    {
        print "ERROR: Can't write protocol because of an open error\n" ;
    }
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
# Usage() - Prints out the program usage and ends the program.
################################################################################

sub Usage
{
    my $rc = shift;

    print <<USAGE_END;

Usage:  build_tools  [-(h|?|help)] [-scratch] [-private] [-machine <config>]
                     [-(e|email) <email address>] [-toolrel <rel>]

Where:
        -h or -? or -help
            Shows this help an the exits the program.

        -scratch
            Initiates a scratch make instead of an incremental one.

        -private
            Initiates a 'private' run which is not logged into QADB. It is
            recommended to use this option together with '-email'.

        -email <email address>
            Allows you to set a specific email address to send the error
            mails to. If this option is set, all normal recipients will be
            ignored.
            
        -machine <config>
            Sets the machine config, e.g. "linuxintel".
            
        -toolrel <rel>
            Sets the tool release, e.g. "dev" or "cons".

USAGE_END

    CleanExit($rc);
}

################################################################################
# _throw_error() - Throws an error and exits.
################################################################################

sub _throw_error
{
    my ($funcname, $errortext) = @_;

    print "\n========================================\n";
    print "    ERROR !!! ATTENTION !!! ERROR !!!\n";
    print "========================================\n\n";
    print "There has been an error in program 'build_tools'\n";
    print "in function '$funcname':\n\n";
    print "$errortext\n\n";
    print "Exiting now...\n";

    CleanExit(10);
}
