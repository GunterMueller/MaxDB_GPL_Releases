#! /usr/bin/perl
#
#    build_sdb.pl - Performs a nightly make.
#
#    @(#)build_sdb.pl     2005-08-05
#
#    U. Jansen, SAP AG
#
#    ========== licence begin LGPL
#    Copyright (C) 2005 SAP AG
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

# System/CPAN Includes:
use Getopt::Long;
use File::Path;


# Selfmade includes:
use IO::SimpleCfg;
use qadb;
use BuildPackage::Vars;

eval {
    require Pod::Usage;
    import Pod::Usage;
};
if ($@) {
    sub pod2usage {
        print "Sorry. POD is not supported on this platform. For help, please call this\nscript under Windows or use a perl that can do Pod's.\n";
        exit(1);
    }
}

################################################################################
# Globals:
################################################################################

my $prog_name = "build_sdb v0.1 (c)2005, SAP AG";
my %cfg;
my %pscan;
my %param_cfg;
my %stats;
my $cfg_file = ($ENV{'NACHTMAKE_CFG'} ? $ENV{'NACHTMAKE_CFG'} : "ProdBuildWdf.cfg");
my $check = 0;
my $qah = undef;

my %QAStatus = (    'INIT_START'            => 0, 
                    'SET_TOOL_CL'           => 50,
                    'MAKE_TARGET_START'     => 201,
                    'MAKE_TARGET_END'       => 202,
                    'MAKE_TARGET_NOT_OK'    => 299,
                    'MAKE_TARGET_OK'        => 300,
                    'SET_LC_POOL_ID'        => 950,
                    'MAKE_CANCELLED'        => 990,
                    'MAKE_OK_NOT_RELEVANT'  => 998,
                    'MAKE_NOT_OK'           => 999,
                    'MAKE_OK'               => 1000);

$stats{'TARGETS_SUM'}   = 0;
$stats{'TARGETS_OK'}    = 0;
$stats{'TARGETS_FAIL'}  = 0;
$stats{'MAKE_RC'}       = 0;

################################################################################
# main():
################################################################################

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

print "\n$prog_name\n\n";

#-------------------------------------------------------------------------------
# Initialize
#-------------------------------------------------------------------------------
ProtHeader("Initializing");

# First, let's parse the params:
GetOptions (
            'bit_32_only=i'             => sub { $param_cfg{'BIT_32_ONLY'} = $_[1]; },
            'build_purpose|make_type=s' => sub { if ($_[1] =~ /^(LC|LC_OLTP|OLTP|ALL)$/) { $param_cfg{'BUILD_PURPOSE'} = ($_[1] =~ /LC_OLTP/ ? "ALL" : $_[1]); } else { pod2usage("Option make_type has to be either LC, LC_OLTP, OLTP or ALL.\n"); } }, 
            'cfgfile=s'                 => \$cfg_file,
            'check'                     => sub { $check = 1; },
            'chlist_file=s'             => sub { $param_cfg{'CHLIST_FILE'} = $_[1]; },
            'chlist_num=i'              => sub { $param_cfg{'CHLIST_NUM'} = $_[1]; },
            'cli_bit_32'                => sub { $param_cfg{'MAKE_TARGET'} = 'cli32.mac'; $param_cfg{'MAKE_TITLE'} = '32 Bit Client'; },  # Just implemented for compatibility reasons...
            'del_cmd=s'                 => sub { $param_cfg{'DEL_CMD'} = $_[1]; },
            'del_dirs=s'                => sub { $param_cfg{'DEL_DIRS'} = $_[1]; },
            'email=s'                   => sub { $param_cfg{'EMAIL'} = $_[1]; } ,
            'errout|errout_file=s'      => sub { $param_cfg{'ERROUT_FILE'} = $_[1]; },
            'h|help'                    => sub { pod2usage(1); },
            'html'                      => sub {},   # depricated...
            'lc_pool_id=i'              => sub { $param_cfg{'LC_POOL_ID'} = $_[1]; },
            'make_cmd=s'                => sub { $param_cfg{'MAKE_CMD'} = $_[1]; },
            'make_id_file=s'            => sub { $param_cfg{'MAKE_ID_FILE'} = $_[1]; },
            'make_prot_path=s'          => sub { $param_cfg{'MAKE_PROT_PATH'} = $_[1]; },
            'make_target=s'             => sub { $param_cfg{'MAKE_TARGET'} = $_[1]; },
            'make_title=s'              => sub { $param_cfg{'MAKE_TITLE'} = $_[1]; },
            'man'                       => sub { pod2usage( -exitstatus => 0, -verbose => 2 ); },
            'pcr_bit_32'                => sub { $param_cfg{'MAKE_TARGET'} = 'pcr32.mac'; $param_cfg{'MAKE_TITLE'} = '32 Bit Precompiler Runtimes'; },  # Just implemented for compatibility reasons...
            'pid_file=s'                => sub { $param_cfg{'PID_FILE'} = $_[1]; },
            'private'                   => sub { $param_cfg{'PRIVATE'} = 1; },
            'prot_dir=s'                => sub { $param_cfg{'PROT_DIR'} = $_[1]; },
            'prot_name=s'               => sub { $param_cfg{'PROT_NAME'} = $_[1]; },
            'qa_id=i'                   => sub { $param_cfg{'QA_ID'} = $_[1]; },
            'scratch|force_rm'          => sub { $param_cfg{'SCRATCH'} = 1; },
            'test_relevant=i'           => sub { $param_cfg{'TEST_RELEVANT'} = $_[1]; },
            'tool_chlist=i'             => sub { $param_cfg{'TOOL_CHLIST'} = $_[1]; },
            'tool_cl_file=s'            => sub { $param_cfg{'TOOL_CL_FILE'} = $_[1]; },
            'use_lcpool=i'              => sub { $param_cfg{'USE_LCPOOL'} = $_[1]; },
            'use_prot_scan=i'           => sub { $param_cfg{'USE_PROT_SCAN'} = $_[1]; },
            'use_qadb=i'                => sub { $param_cfg{'USE_QADB'} = $_[1]; },
            'use_webdav=i'              => sub { $param_cfg{'USE_WEBDAV'} = $_[1]; },
            'use_objstat=i'             => sub { $param_cfg{'USE_OBJSTAT'} = $_[1];},
            'x_bit=i'                   => sub { if ($_[1] == 32 || $_[1] == 64) {$param_cfg{'X_BIT'} = $_[1]; } else { pod2usage("Option x_bit must be set to either 32 or 64.\n"); } },
) or pod2usage("\n");


#-------------------------------------------------------------------------------
# Assemble configuration
#-------------------------------------------------------------------------------

ProtHeader("Assembling configuration");

my $loaded_cfg = LoadSimpleCfg("$ENV{'OWN'}/sys/wrk/config/$cfg_file");
foreach my $key (keys %{$loaded_cfg->{'ENV'}}) { $ENV{$key} = $loaded_cfg->{'ENV'}->{$key}; }
EnvSetCfg('MACHINE_CONFIG');
EnvSetCfg('TOOL');
EnvSetCfg('OWN');
EnvSetCfg('RELVER');
EnvSetCfg('CORRECTION_LEVEL');
EnvSetCfg('BUILDPRAEFIX');
EnvSetCfg('WRK');
EnvSetCfg('LC_STATE');
EnvSetCfg('TOOLSHELL');
if (defined $LC_POOL_DIR) { $cfg{'LC_POOL_DIR'} = $LC_POOL_DIR; }
else { $param_cfg{'USE_LCPOOL'} = 0; }

# Patch basic values:
$cfg{'CORRECTION_LEVEL'} = "0$1" if ($cfg{'CORRECTION_LEVEL'} =~ /^(\d)$/);
$cfg{'BUILDPRAEFIX'} = "0$1" if ($cfg{'BUILDPRAEFIX'} =~ /^(\d)$/);
$cfg{'RELVER'} = substr($cfg{'RELVER'}, 1) if ($cfg{'RELVER'} =~ /^R.*/);
$cfg{'START_TIME'} = (scalar localtime);
my @LocalTime = localtime;
chomp ($cfg{'HOSTNAME'} = `hostname`);

# Now overload config hashes to get final (loaded) config: 
foreach my $key (keys %{$loaded_cfg->{'PROTOCOL_SCAN'}}) { $pscan{$key} = $loaded_cfg->{'PROTOCOL_SCAN'}->{$key}; }
foreach my $key (keys %{$loaded_cfg->{'DEFAULT'}}) { $cfg{$key} = $loaded_cfg->{'DEFAULT'}->{$key}; }
foreach my $key (keys %{$loaded_cfg->{uc($cfg{'MACHINE_CONFIG'})}}) { $cfg{$key} = $loaded_cfg->{uc($cfg{'MACHINE_CONFIG'})}->{$key}; }
foreach my $key (keys %param_cfg) { $cfg{$key} = $param_cfg{$key}; }

# Now setting defaults for all changeable values:
$cfg{'BIT_32_ONLY'} = 0 unless (defined $cfg{'BIT_32_ONLY'});
$cfg{'BUILD_PURPOSE'} = ($ENV{'SAPDB_PURPOSE'} ? $ENV{'SAPDB_PURPOSE'} : 'ALL') unless ($cfg{'BUILD_PURPOSE'});
$cfg{'CHLIST_FILE'} = SetPath($cfg{'CHLIST_FILE'} ? $cfg{'CHLIST_FILE'} : "$cfg{'OWN'}/changelist.log");
$cfg{'CHLIST_NUM'} = ReadFileLines($cfg{'CHLIST_FILE'}, 1) unless ($cfg{'CHLIST_NUM'});
$cfg{'DEL_CMD'} = ($^O =~ /win32/i ? "del /S /Q" : "rm -rf") unless ($cfg{'DEL_CMD'});
$cfg{'DEL_DIRS'} = "sys/wrk,usr,test" unless (defined $cfg{'DEL_DIRS'});
$cfg{'MAKE_CMD'} = "imf" unless ($cfg{'MAKE_CMD'});
$cfg{'MAKE_ID_FILE'} = SetPath($cfg{'MAKE_ID_FILE'} ? $cfg{'MAKE_ID_FILE'} : "$cfg{'OWN'}/make.id");
$cfg{'MAKE_PROT_PATH'} = SetPath($cfg{'MAKE_PROT_PATH'} ? $cfg{'MAKE_PROT_PATH'} : "sys/wrk/fast/prot");
$cfg{'MAKE_TARGET'} = "sdball.mac" unless ($cfg{'MAKE_TARGET'});
$cfg{'MAKE_TITLE'} = "liveCache/MaxDB Server" unless ($cfg{'MAKE_TITLE'});
$cfg{'PID_FILE'} = SetPath($cfg{'PID_FILE'} ? $cfg{'PID_FILE'} : "$cfg{'OWN'}/prot/build.pid");
if ($cfg{'PRIVATE'}) { $cfg{'USE_LCPOOL'} = 0; $cfg{'USE_QADB'} = 0; $cfg{'USE_WEBDAV'} = 0; $cfg{'USE_OBJSTAT'} = 0; }
else {
    $cfg{'USE_LCPOOL'} = ($cfg{'LC_POOL_DIR'} ? 1 : 0) unless (defined $cfg{'USE_LCPOOL'});
    $cfg{'USE_QADB'} = 1 unless (defined $cfg{'USE_QADB'});
    $cfg{'USE_WEBDAV'} = 1 unless (defined $cfg{'USE_WEBDAV'});
    $cfg{'USE_OBJSTAT'} = 1 unless (defined $cfg{'USE_OBJSTAT'});
}
$cfg{'USE_PROT_SCAN'} = ($cfg{'ERROUT_FILE'} ? 1 : 0) unless (defined $cfg{'USE_PROT_SCAN'});
$cfg{'LB_SYM_LINK'} = SetPath($cfg{'LC_POOL_DIR'}, "LastBuild");
$cfg{'SCRATCH'} = 0 unless (defined $cfg{'SCRATCH'});
$cfg{'TEST_PFL_NAME'} = SetPath($cfg{'WRK'}, "config", "tests.pfl");
$cfg{'TEST_RELEVANT'} = ($ENV{'RELSTAT'} =~ /PROD/i ? 1 : 0) unless ($cfg{'TEST_RELEVANT'});
$cfg{'TOOL_CL_FILE'} = SetPath($cfg{'TOOL_CL_FILE'} ? $cfg{'TOOL_CL_FILE'} : "$cfg{'TOOL'}/data/changelist.log");
$cfg{'TOOL_CHLIST'} = ReadFileLines($cfg{'TOOL_CL_FILE'}, 1) unless ($cfg{'TOOL_CHLIST'});
$cfg{'X_BIT'} = ($cfg{'BIT_32_ONLY'} ? '32' : '64') unless (defined $cfg{'X_BIT'});
$cfg{'PROT_DIR'} = SetPath($cfg{'PROT_DIR'} ? $cfg{'PROT_DIR'} : "$cfg{'OWN'}/prot");
$cfg{'PROT_NAME'} = sprintf("NACHT_%d%s%s_%s_%s_%02d%s%04d_%02d%02d.prot", $cfg{'RELVER'}, $cfg{'CORRECTION_LEVEL'}, $cfg{'BUILDPRAEFIX'}, $cfg{'X_BIT'}, $cfg{'HOSTNAME'}, $LocalTime[3], ('Jan','Feb','Mar','Apr','Mai','Jun','Jul','Aug','Sep','Oct','Nov','Dec')[$LocalTime[4]], $LocalTime[5] + 1900, $LocalTime[2], $LocalTime[1]) unless ($cfg{'PROT_NAME'});
$cfg{'PROT_PATH'} = SetPath($cfg{'PROT_DIR'}, $cfg{'PROT_NAME'});


#-------------------------------------------------------------------------------
# Clean leftover files from old makes:
#-------------------------------------------------------------------------------

check_system("$cfg{'DEL_CMD'} $cfg{'MAKE_ID_FILE'}") if (-e $cfg{'MAKE_ID_FILE'});
check_system("$cfg{'DEL_CMD'} $cfg{'ERROUT_FILE'}") if ($cfg{'ERROUT_FILE'} && -e $cfg{'ERROUT_FILE'});

#-------------------------------------------------------------------------------
# Store Build PID
#-------------------------------------------------------------------------------

ProtHeader("Storing build.pid");
WriteFileLine($cfg{'PID_FILE'}, "$$");

#-------------------------------------------------------------------------------
# Prepare make phase
#-------------------------------------------------------------------------------

ProtHeader("Prepare make");
if ($cfg{'SCRATCH'} == 1) {
    chdir("$ENV{'OWN'}");
    my @del_dirs = split(/,/, $cfg{'DEL_DIRS'});
    foreach my $dir (@del_dirs) {
        $dir = SetPath($cfg{'OWN'}, $dir);
        if ($^O =~ /win32/i) { check_system("ATTRIB -R -H -S $dir"); check_system("ATTRIB -R -H -S $dir\\*.* /S /D"); } 
        else { check_system("chmod -R a+wrx $dir"); }
        check_system("$cfg{'DEL_CMD'} $dir");
    }
    my $imf_exit;
    eval { $imf_exit = system ( "imf config" ); };
    if ($@) { print STDERR "ERROR: parsing imf('config')!\n$@\n"; }
    elsif ($imf_exit != 0) { print STDERR "ERROR: imf('config') returned with an error!\n$!\n"; }
}
mkpath (["$cfg{'PROT_DIR'}"], 0, 0775) unless ($check);

#-------------------------------------------------------------------------------
# Redirect STDOUT and STDERR
#-------------------------------------------------------------------------------

ProtHeader("Redirect STDOUT and STDERR");
if ($check) { print "redirection of STDOUT and STDERR\n"; }
else { print "OUTPUT_FILTER: OPEN FILE $cfg{'PROT_PATH'}\n"; }

#-------------------------------------------------------------------------------
# Creating new qadb entry
#-------------------------------------------------------------------------------

ProtHeader("Creating new qadb entry");
QADBCreateHandle();
QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'SET_TOOL_CL'}, 'TOOLCL' => $cfg{'TOOL_CHLIST'}}) if ($cfg{'TOOL_CHLIST'} ne "");

#-------------------------------------------------------------------------------
# Final configuration
#-------------------------------------------------------------------------------

ProtHeader("Final configuration");
foreach my $key (sort keys %cfg) { print "$key = $cfg{$key}\n"; }

#-------------------------------------------------------------------------------
# make process
#-------------------------------------------------------------------------------

ProtHeader("Start make");
QADBWriteLog("INFO: Tool directory is '$ENV{'TOOL'}'.");
if ($ENV{'DTM_TASKLOGFILE'}) {
    my $logfilepath = $ENV{'DTM_TASKLOGFILE'};
    $logfilepath =~ tr/\\/\//; 
    ($logfilepath) = ($logfilepath =~ /(production\/info\/logs.*)$/);
    QADBWriteLog("<a href=\"http://ls3105.wdf.sap.corp:1080/BAS/$logfilepath\" target=\"DTMLogFileWindow\">DTM Logfile</a>");
}
QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'MAKE_TARGET_START'}});

if ($check) { print "Make: $cfg{'MAKE_CMD'} $cfg{'MAKE_TARGET'}\n"; }
else {
    QADBWriteLog("Starting make process for " . ($cfg{'MAKE_TITLE'} ? "'$cfg{'MAKE_TITLE'}'." : "target '$cfg{'MAKE_TARGET'}'."));
    if (open (MAKE_OUT, "$cfg{'MAKE_CMD'} $cfg{'MAKE_TARGET'} |")) {
        while (<MAKE_OUT>) {
            if (/VMAKE\sREPORT:\s+(\S.*'.*'\s+started)/) {
                QADBWriteLog("START: $1");
                $stats{'TARGETS_SUM'} += 1;
            } elsif (/VMAKE\sREPORT:\s+(\S.*'.*')\s+skipped.*previous\serrors/) {
                QADBWriteLog("SKIPPED: $1");
                $stats{'TARGETS_SUM'} += 1;
            } elsif (/VMAKE\sREPORT:\s+(\S.*'.*'\s+skipped.*)/) {
                QADBWriteLog("SKIPPED: $1");
            } elsif (/VMAKE\sREPORT:\s+(\S.*'.*'\s+successfully\sfinished)/) {
                QADBWriteLog("SUCCESS: $1");
                $stats{'TARGETS_OK'} += 1;
            } elsif (/VMAKE\sREPORT:\s+(\S.*'.*'\s+failed)/) {
                QADBWriteLog("ERROR: $1");
                $stats{'TARGETS_FAIL'} += 1;
            }
            print "$_";
        }
        close(MAKE_OUT);
        $stats{'MAKE_RC'} = $? >> 8;
        QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'MAKE_TARGET_END'}});
    } else {
        print "ERROR: Failed to execute '$cfg{'MAKE_CMD'} $cfg{'MAKE_TARGET'}': $!\n";
        $stats{'MAKE_RC'} = -1;
    }
}
ProtHeader("End make");

$stats{'MAKE_OK'} = (($stats{'MAKE_RC'} == 0) && ($stats{'TARGETS_SUM'} == $stats{'TARGETS_OK'})) ? 1 : 0;
if ($stats{'MAKE_OK'}) { QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'MAKE_TARGET_OK'}}); } 
else { QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'MAKE_TARGET_NOT_OK'}}); }
    
#-------------------------------------------------------------------------------
# Running Checkmake / Sending mails
#-------------------------------------------------------------------------------

ProtHeader("Running Checkmake / Sending mails");

my @checkmake_cmd = ("$cfg{TOOLSHELL}", "$cfg{'TOOL'}/bin/checkmake.pl");
push @checkmake_cmd, ("-qaid", "$cfg{'QA_ID'}") if ($cfg{'QA_ID'});
push @checkmake_cmd, ("-email", "$cfg{'EMAIL'}") if ($cfg{'EMAIL'});
push @checkmake_cmd, ("-chlist", "$cfg{'CHLIST_NUM'}") if ($cfg{'CHLIST_NUM'});
push @checkmake_cmd, ("-outfile", "$cfg{'ERROUT_FILE'}") if ($cfg{'ERROUT_FILE'});
push @checkmake_cmd, "$cfg{'OWN'}/$cfg{'MAKE_PROT_PATH'}/$cfg{'MAKE_TARGET'}.e0";
if (!$cfg{'PRIVATE'} or $cfg{'EMAIL'}) {
    my $checkmake_rc = check_system(join (" ", @checkmake_cmd));
    if ((!defined $checkmake_rc ) or ( $checkmake_rc != 0 )) { print STDERR "ERROR: file \"checkmake.pl\" failed\n"; }
}

#-------------------------------------------------------------------------------
# Get test profile
#-------------------------------------------------------------------------------

ProtHeader("Getting test profile");

if ($check) { print "Getting test profile...\n"; }
elsif (!$cfg{'TEST_RELEVANT'}) { print "Getting test profile has been disabled.\n"; }
else {
    if (-e $cfg{'TEST_PFL_NAME'}) {
        if ($stats{'MAKE_OK'}) {
            if (open(TESTS, "<$cfg{'TEST_PFL_NAME'}")) {
                while(<TESTS>) { chomp($cfg{'TEST_PROFILE'} = $_) unless ((/^#/) || (/^$/)); }
                close (TESTS);
            }
            unless ($cfg{'TEST_PROFILE'}) { print "Warning: Test profile could not be read.\n"; }
            print "Using test profile: $cfg{'TEST_PROFILE'}\n";
        } else { print "Make was not correct. No test profile needed.\n"; }
    } else { print "Test profile '$cfg{'TEST_PFL_NAME'}' does not exist. No profile --> No tests!\n"; }
}

#-------------------------------------------------------------------------------
# update LC pool ID
#-------------------------------------------------------------------------------

ProtHeader("Get LC Pool ID");

if ($stats{'MAKE_OK'}) {
    $cfg{'LC_POOL_ID'} = GetLCPoolID();
    print "LC_POOL_ID has been set to '$cfg{'LC_POOL_ID'}'\n";
} else {
    $cfg{'LC_POOL_ID'} = "0";
    print "Since there have been errors, LC_POOL_ID is set to 0.\n";
}
QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'SET_LC_POOL_ID'}, 'LCPOOLID' => $cfg{'LC_POOL_ID'}});

#-------------------------------------------------------------------------------
# Finalize QADB status
#-------------------------------------------------------------------------------

ProtHeader("Finalizing");

if ($stats{'MAKE_OK'} and ($cfg{'LC_POOL_ID'} ne "0")) {
    print "\nMAIN EXIT: Congratulations!!! No make errors occured!\n";
    QADBCreateTestprofile($cfg{'TEST_PROFILE'}) if ($cfg{'TEST_PROFILE'});
    QADBUpdateCol({'IDOBJSTATUS' => ($cfg{'TEST_RELEVANT'} ? $QAStatus{'MAKE_OK'} : $QAStatus{'MAKE_OK_NOT_RELEVANT'})});
} else {
    QADBUpdateCol({'IDOBJSTATUS' => $QAStatus{'MAKE_NOT_OK'}});
    print "\nMAIN EXIT: ATTENTION---ERRORS---ATTENTION---ERRORS---ATTENTION\n" ;
}

#-------------------------------------------------------------------------------
# Final Statistics
#-------------------------------------------------------------------------------

ProtHeader("*** FINAL STATISTICS ***");

# TODO: Fix final statistices:

print "MACHINE          = $cfg{'HOSTNAME'}\n";
print "MACHINE CONFIG   = $cfg{'MACHINE_CONFIG'}\n";
print "START TIME       = $cfg{'START_TIME'}\n";
print "END TIME         = " . (scalar localtime) . "\n";
print "TARGETS OVERALL  = $stats{'TARGETS_SUM'}\n";
print "TARGETS OK       = $stats{'TARGETS_OK'}\n";
print "TARGETS FAILED   = $stats{'TARGETS_FAIL'}\n";
print "MAKE COMMAND RC  = $stats{'MAKE_RC'}\n";
print "ALL OK           = " . ($stats{'MAKE_OK'} ? "YES\n" : "NO\n");


if (defined($ENV{DTM_TASKEXPORTFILE}) ) {
    if (open (EXPORTFILE, "> $ENV{DTM_TASKEXPORTFILE}")) {
        print EXPORTFILE "SUBTASKS_OK=$stats{'TARGETS_OK'}\n";
        print EXPORTFILE "SUBTASKS_ERROR=$stats{'TARGETS_FAIL'}\n";
        foreach my $key (keys %{$loaded_cfg->{'DTM_EXPORT'}}) { print EXPORTFILE "$key=$cfg{$key}\n" if ($loaded_cfg->{'DTM_EXPORT'}->{$key}); }
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

if ($check) { print "Resetting file output\n"; }
else { print "OUTPUT_FILTER: CLOSE FILE\n"; }

#-------------------------------------------------------------------------------
# Write make protocol into QADB
#-------------------------------------------------------------------------------

my $WriteProtOK = QADBWriteProt("$cfg{'PROT_PATH'}", "$cfg{'PROT_NAME'}", "Make Protocol (complete)");

ScanMakeProt();

# Remove Protocol, if everything was OK:
check_system("$cfg{'DEL_CMD'} $cfg{'PROT_PATH'}") if (($stats{'MAKE_RC'} == 0) && ($stats{'TARGETS_SUM'} == $stats{'TARGETS_OK'}) && $WriteProtOK);

CleanExit($stats{'MAKE_RC'});

################################################################################
# ProtHeader() - Prints a small protocol header to STDOUT.
################################################################################

sub ProtHeader {  print "\n\n===========================================================\n=   $_[0]\n===========================================================\n\n"; }

################################################################################
# CleanExit() - Cleans up and exits the module.
################################################################################

sub CleanExit
{
    my $RC = shift;

    check_system("$cfg{'DEL_CMD'} $cfg{'PID_FILE'}");

    exit($RC);
}

################################################################################
# check_system() - Executes a system call, if $check is not set
################################################################################

sub check_system {
    my $cmd = shift;
    if ($check) { print "Execute command: '$cmd'\n"; return 0; }
    else { return system($cmd); }
}

################################################################################
# WriteFileLine() - Writes a string into a file
################################################################################

sub WriteFileLine {
    my ($file, $line) = @_;
    if ($check) { print "Write line '$line' to file '$file'.\n"; } 
    else {
        unless (open (OUT_FILE, ">$file")) { print "WARNING: Can't open file '$file' for writing!\n"; return 0; }
        print OUT_FILE $line;
        close (OUT_FILE);
    }
}

################################################################################
# ReadFileLines() - Reads [n] lines from a file
################################################################################

sub ReadFileLines
{
    my ($file, $lines) = @_;
    my $line    = 1;
    my $tmpvar;
    unless (open (IN_FILE, "<$file")) { print STDERR "ERROR: Can't open file '$file'!\n"; return 0; }
    while (<IN_FILE>) {
        chomp $_;
        $tmpvar .= ($tmpvar eq "" ? "$_" : "\n$_");
        last if ($line == $lines);
        $line ++;
    }
    close (IN_FILE) or print STDERR "ERROR: Can't close file '$file'!\n$!\n";
    return $tmpvar;
}

################################################################################
# SetEnvCfg() - Tranfsers a variable from ENV to cfg or throws an error.
################################################################################
sub EnvSetCfg {
    if (defined $ENV{$_[0]}) { $cfg{$_[0]} = $ENV{$_[0]}; }
    else { pod2usage("Mandatory environment variable '$_[0]' is missing!\n"); }
}

################################################################################
# SetPath() - Joins a filepath and fixes the path separator.
################################################################################
sub SetPath {
    my $str = join("/", @_);
    $str =~ tr/\//\\/ if ($^O =~ /win32/i);
    return ($str);
}

################################################################################
# QADBCheckHandle() - Checks whether the QA handle is ok or not.
################################################################################

sub QADBCheckHandle
{
    return 1 unless ($cfg{'USE_QADB'});
    if (($qah->{'error_code'} != 0) or (!$cfg{'QA_ID'})) {
        if ($qah->{'error_code'} != 0) { print STDERR "ERROR: QADB got an error: '$qah->{'error_text'}'\n"; }
        else { print STDERR "ERROR: QADB handle is zero!\n"; }
        QADBCreateHandle();
        return 0 if (($qah->{'error_code'} != 0) or (!$cfg{'QA_ID'}));
    }
    return 1;
}

################################################################################
# QADBCreateHandle() - Creates a new QADB handle
################################################################################

sub QADBCreateHandle
{
    # Create a new QADB handle:

    $qah = undef;
    return 1 unless ($cfg{'USE_QADB'});

    if ($check) {
    	if ($cfg{'QA_ID'} > 0 ){ print "Reusing QADB entry '$cfg{'QA_ID'}'.\n"; }
    	else { print "Creating new QADB entry.\n"; }
    }
    else {
        if ($cfg{'QA_ID'} > 0) { $qah = qadb->new( {'ID' => $cfg{'QA_ID'} } ); }
        else {
        	if ($cfg{'QADB_SYSNAME'}) {
                $qah = qadb->new( {'VERSION'          => "$cfg{'RELVER'}$cfg{'CORRECTION_LEVEL'}" ,
                                   'BUILDPFX'         => "$cfg{'BUILDPRAEFIX'}" ,
                                   'QASTATUS'         => "$cfg{'LC_STATE'}" ,
                                   'CHANGELIST'       => "$cfg{'CHLIST_NUM'}" ,
                                   'PLATFORM'         => "$cfg{'QADB_SYSNAME'}",
                                   'BUILD_PURPOSE'    => substr($cfg{'BUILD_PURPOSE'}, 0, 1) });
            } else {
                $qah = qadb->new( {'VERSION'          => "$cfg{'RELVER'}$cfg{'CORRECTION_LEVEL'}" ,
                                   'BUILDPFX'         => "$cfg{'BUILDPRAEFIX'}" ,
                                   'QASTATUS'         => "$cfg{'LC_STATE'}" ,
                                   'CHANGELIST'       => "$cfg{'CHLIST_NUM'}" ,
                                   'BUILD_PURPOSE'    => substr($cfg{'BUILD_PURPOSE'}, 0, 1) });
            }
        }

        $cfg{'QA_ID'} = (($qah->{'error_code'} == 0) ? $qah->{'ID'} : 0);
        if (($cfg{'QA_ID'} > 0) && ($cfg{'QA_ID_WRITTEN'} == 0))
        {
            WriteFileLine($cfg{'MAKE_ID_FILE'}, $cfg{'QA_ID'});
            $ENV{'MAKEID'} = $cfg{'QA_ID'};
            $cfg{'QA_ID_WRITTEN'} = 1;
        }
    }
}

################################################################################
# QADBCreateTestprofile() - Adds a test-profile for this make.
################################################################################

sub QADBCreateTestprofile {
    my ($profile) = @_;
    return unless($cfg{'USE_QADB'});
    if ($check) { print "QADB: adding test profile: '$profile'\n"; return; }
    if (QADBCheckHandle()) { $qah->create_testprofile($profile); QADBCheckHandle(); } 
    else { print STDERR "ERROR: Can't create QADB test profile because of an open error\n"; }
}

################################################################################
# QADBUpdateCol() - updates a column in the QADB
################################################################################

sub QADBUpdateCol {
    my ($hash) = @_;
    return unless ($cfg{'USE_QADB'});
    if ($check) { print "QADB: updating col(s):\n"; foreach my $key (keys %$hash) { print "    '$key' => '$hash->{$key}'\n"; } return; }
    unless ($cfg{'USE_OBJSTAT'}) { foreach my $key (keys %$hash) { return if ($key eq "IDOBJSTATUS"); } }
    if (QADBCheckHandle()) { $qah->update_columns($hash); QADBCheckHandle(); } 
    else { print STDERR "ERROR: Can't update QADB because of an open error\n" ; }
}

################################################################################
# QADBWriteLog() - writes a log entry into QADB
################################################################################

sub QADBWriteLog {
    my $logtext = shift;
    return unless($cfg{'USE_QADB'});
    if ($check) { print "QADB ($cfg{'QA_ID'}): writing log entry '$logtext'\n"; return; }
    if (QADBCheckHandle()) { $qah->write_log($logtext); QADBCheckHandle(); } 
    else { print STDERR "ERROR: Can't write log entry because of an open error\n" ; }
}

################################################################################
# QADBWriteProt() - writes a protocol file into QADB
################################################################################

sub QADBWriteProt {
    my $protpath = shift;
    my $protname = shift;
    my $showname = shift;
    my $protocol;
    my $line = 1;
    my $rc = 0;
    
    if ($check || $cfg{'PRIVATE'}) { print "QADB: writing protocol file '$protname'\n"; return; }
    unless (open (IN_FILE, "< $protpath")) { print "Error: Can't open $protpath for reading.\n"; return 0; }
    while (<IN_FILE>) { $protocol .= $_; }
    close (IN_FILE);
    
    if (!$cfg{'USE_QADB'} && $cfg{'USE_WEBDAV'}) {
        if (open (OUT_FILE,">$protname")) { print OUT_FILE $protocol; close (OUT_FILE); }
        if ($^O=~/.*win32.*/i) {   $rc = system("python $ENV{TOOL}\\bin\\webdav.py $protname"); }
        else {   $rc = system("webdav.py $protname");   }
        $cfg{'WEBDAV_PROT'} = "http://pts.wdf.sap.corp:1080/webdav/daily_makes/$protname";
        unlink($protname);
    } elsif ($cfg{'USE_QADB'}) {
        if (QADBCheckHandle()) {
            if (length ($protocol) > 8000) {
                eval "require SDB::Protocol::Tools";
                if ($@) { print STDERR "Warning: Error while loading SDB::Protocol::Tools\n($@)\nCompressing file failed - using uncompressed file\n" ;	}
                else {
                    import SDB::Protocol::Tools;
                    my $ref_to_zip;
                    my $zip_mode = 0;
                    eval '($ref_to_zip, $zip_mode) = SDB::Protocol::Tools::PackScalar(\$protocol, $protname , {\'toDOS\' => 1});';
                    if ($@) { print STDERR "Warning: Error while compress protocol\n($@)\nUsing uncompressed file\n"; }
                    else { 
                        $zip_mode and $protname .= ".zip"; 
                        open TMPZIPFILE , ">$ENV{'WRK'}/$protname";
                        binmode TMPZIPFILE;
                        print TMPZIPFILE $protocol;
                        close TMPZIPFILE;
                    }
                }
            }
            if ($showname) { $cfg{'WEBDAV_PROT'} = $qah->write_prot("$protname", $protocol, $showname); }
            else { $cfg{'WEBDAV_PROT'} = $qah->write_prot("$protname", $protocol); }
            $rc = QADBCheckHandle();
        } else { print STDERR "ERROR: Can't write protocol because of an open error\n"; }
    } 
    return $rc;
}

################################################################################
# GetLCPoolID() - Retrieves the correct LC pool ID
################################################################################

sub GetLCPoolID
{
    my $cmdstr;
    my $erg;

    if ($cfg{'LC_POOL_ID'}) {
        print "Using predefined LC_POOL ID '$cfg{'LC_POOL_ID'}'.\n";
        return $cfg{'LC_POOL_ID'};
    }
    unless ($cfg{'USE_LCPOOL'}) {
        print "LC_POOL usage is disabled.\n";
        return "NONE";
    }
    
    if($^O=~/.*win32.*/i) {
        require WinLink;
        import WinLink;
        $erg = readLink ("$cfg{'LB_SYM_LINK'}");
    } else { $erg = readlink "$cfg{'LB_SYM_LINK'}"; }

    unless (defined $erg)
    {
        QADBWriteLog("ERROR: Fetching LC_POOL_ID failed! [File: '$cfg{'LB_SYM_LINK'}']");
        $erg = "0";
        print STDERR "Fetching LC_POOL_ID failed! [File: '$cfg{'LB_SYM_LINK'}']\n";
    }

    print "LB_SYM_LINK = $cfg{'LB_SYM_LINK'}\n";
    print "LC_POOL_ID = $erg\n";

    return $erg;
}

################################################################################
# ScanMakeProt() - Scans a make protocol for any warnings / errors.
################################################################################

sub ScanMakeProt
{
    return unless ($cfg{'USE_PROT_SCAN'});
    return unless (open (IN_FILE, "< $cfg{'PROT_PATH'}"));
    my $OutLines = {};
    my $line_count = 0;
    my $found_line = 0;
    my @pparts = split(/,/, $pscan{'PSCAN_PARTS'});
    while (<IN_FILE>)
    {
        $line_count += 1;
        my $line = $_;
        chomp $line;
        if ($line =~ /^OUTPUT_FILTER: SCAN (\S+) (ON|OFF)/) { $pscan{$1 . '_ACTIVE'} = ($2 =~ /ON/ ? 1 : 0); next; }
        foreach my $PScan_Part (@pparts) {
            if ($pscan{$PScan_Part . '_ACTIVE'}) {
                if ($line =~ /$pscan{$PScan_Part . '_MATCH'}/ && $line !~ /$pscan{$PScan_Part . '_EXCLUDE'}/) {
                    $OutLines->{$PScan_Part} .= "Line $line_count: $line\n";
                    $OutLines->{$PScan_Part . '_FLAG'} = 1;
                    $found_line = 1;
                } elsif ($OutLines->{$PScan_Part . '_FLAG'} && $line =~ /^\s+\S.*/) { $OutLines->{$PScan_Part} .= "$line\n"; }
                else { $OutLines->{$PScan_Part . '_FLAG'} = 0; }
            }
        }
    }
    close (IN_FILE);

    if ($found_line) {
        if ($cfg{'ERROUT_FILE'}) { return unless (open (OUT_FILE, ">> $cfg{'ERROUT_FILE'}")); } 
        else { return unless (open (OUT_FILE, ">&STDOUT")); }
        print OUT_FILE "\n\n(----------------------- PROTOCOL SCAN -----------------------)\n\n";
        print OUT_FILE "Protocol link: $cfg{'WEBDAV_PROT'}\n\n" if ($cfg{'WEBDAV_PROT'});
        foreach my $PScan_Part (@pparts) {
            if ($OutLines->{$PScan_Part}) {
                print OUT_FILE "\n" . $pscan{$PScan_Part . '_TITLE'} . ":\n\n";
                print OUT_FILE "$OutLines->{$PScan_Part}";
            }
        }
        close (OUT_FILE);
    }
}



__END__

=head1 NAME

build_sdb - Performs a productive make of lC/MaxDB Server

=head1 SYNOPSIS

 build_sdb  [-bit_32_only <0|1>] [-build_purpose <LC|OLTP|ALL>] 
            [-cfgfile <FILENAME>] [-check] [-chlist_file <FILENAME>] 
            [-chlist_num <NUMBER>] [-cli_bit_32] [-del_cmd <COMMAND>]
            [-del_dirs <DIRLIST>] [-email <EMAILLIST>] [-errout <FILENAME>]
            [-(h|help)] [-html] [-lc_pool_id <NUMBER>] [-make_cmd <COMMAND>]
            [--make_id_file <FILENAME>] [-make_prot_path <PATH>] 
            [-make_target <TARGET>] [-make_title <NAME>] [-man] [-pcr_bit_32]
            [-pid_file <FILENAME>] [-private] [-prot_dir <PATH>] 
            [-prot_name <NAME>] [-qa_id <NUMBER>] [-(scratch|force_rm)]
            [-test_relevant <0|1>] [-tool_chlist <NUMBER>] 
            [-tool_cl_file <FILENAME>] [-use_lcpool <0|1>] 
            [-use_prot_scan <0|1>] [-use_qadb <0|1>] [-use_webdav <0|1>]
            [-use_objstat <0|1>] [-x_bit <32|64>]
 
Options:

  -bit_32_only <0|1>            Turns ON/OFF 32-bit-only mode
  -build_purpose <LC|OLTP|ALL>  Sets the build purpose for this make
  -cfgfile <FILENAME>           Defines a specific config file (.cfg)
  -check                        Activates simulation mode
  -chlist_file <FILENAME>       File containing the actual change list
  -chlist_num <NUMBER>          Stamp make to a specific change list
  -cli_bit_32                   Do a 32-bit Client make [DEPRICATED]
  -del_cmd <COMMAND>            Command used for file/dir deletions
  -del_dirs <DIRLIST>           List of dirs to be deleted when scratching
  -email <EMAILLIST>            Manual list of error email receivers
  -errout <FILENAME>            Defines/activates a special error output
  -h or -help                   Shows this brief help
  -html                         Activates HTML output [DEPRICATED] 
  -lc_pool_id <NUMBER>          Manually predefine a LC_POOL ID to be used
  -make_cmd <COMMAND>           Command to be executed as make command
  -make_id_file <FILENAME>      File to store the QADB make ID into
  -make_prot_path <PATH>        Relative path (from $OWN) to the .e0 files
  -make_target <TARGET>         Target (description) to be made
  -make_title <NAME>            Human readable name to the make target
  -man                          Shows the complete manual pages for this tool
  -pcr_bit_32                   Initiates a PCR32 make [DEPRICATED]
  -pid_file <FILENAME>          File to store build_sdb's PID into
  -private                      Initiates a private make (no QADB/Emails)
  -prot_dir <PATH>              Directory to store the main protocol into
  -prot_name <NAME>             Name for the main protocol
  -qa_id <NUMBER>               Define an old QAID rather than using a new one
  -scratch or -force_rm         Do a scratch make (see -del_dirs <DIRLIST>)
  -test_relevant <0|1>          Mark this make as test relevant (ON/OFF)
  -tool_chlist <NUMBER>         Define a TOOL change list manually
  -tool_cl_file <FILENAME>      File containing the TOOL changelist.
  -use_lcpool <0|1>             Turn LC_POOL usage ON/OFF
  -use_prot_scan <0|1>          Turn post-make protocol scanner ON/OFF
  -use_qadb <0|1>               Turn usage of QADB ON/OFF
  -use_webdav <0|1>             Turn usage of Webdav ON/OFF
  -use_objstat <0|1>            Turn modification of OBJSTAT ON/OFF
  -x_bit <32|64>                Set number of bits used to either 32 or 64


=head1 OPTIONS

NOTE: All options have a default value. If you don't give any options at all,
build_sdb would produce a default (Wdf) productive make with full QADB support.
Instead of using command line parameters, you can use a .cfg file which contains
the same options. For more information on this topic, keep on reading or look 
into the default configration files (ProMakeWdf.cfg | TestMakeBln.cfg).

=over 8

=item B<-bit_32_only <0|1>>

Turn OMN/OFF the 32-bit-only mode. With this mode turned on, only 32 bit
outputs will be produced even when running on a 64 bit system. [Default: 0]

=item B<-build_purpose <LC|OLTP|ALL>>

Manually define the build purpose for this make. Values can only be 'LC', 'OLTP'
or 'ALL'. [Default: ALL]

=item B<-cfgfile <FILENAME>>

Define a configuration file other than the default one. [Default: $NACHTMAKE_CFG, 
if defined. Else 'ProdMakeWdf.cfg'.]

=item B<-check>

Starts the script in a simpulation mode. Nothing is actually done. NOTE: This 
option is 'cmd line only'.

=item B<-chlist_file <FILENAME>>

Defines the file to read the lC/MaxDB Perforce change list from. This file 
should be plain text and only contain one single line with the change list 
number without any further formatting.[Default: $OWN/changelist.log]

=item B<-chlist_num <NUMBER>>

Sets the lC/MaxDB Perforce change list to a manual value rather than reading it 
from the changelist file. [Default: Read from CHLIST_FILE, see above]

=item B<-cli_bit_32>

Performs a 32-bit-only Client make of MaxDB. NOTE: This option is depricated!
Use -make_target instead. [Default: <not used>]

=item B<-del_cmd <COMMAND>>

Defines the command to be used to delete operations (e.g. when scratching 
files/directories). [Default: 'rm -rf' for UNIX, 'del /S /Q' for Windows]

=item B<-del_dirs <DIRLIST>>

Comma-separated (non-spaced) list of directories to be deleted when performing
a scratch make. [Default: 'sys/wrk,usr,test']

=item B<-email <EMAILLIST>>

Predefine a list of email receivers for make error mails. [Default: <automatically
generated within checkmake>]

=item B<-errout <FILENAME>>

A file that stores additional error output information. This file is used to
generate the so-called 'watcher-mails' - it contains the checkmake email output
and any output from the protocol scanner. [Default: <not used>]

=item B<-h or -help>

Shows a brief command help.

=item B<-html>

Generate HTML output. NOTE: This feature is DEPRICATED and will *NOT* be 
supported any more due to the newly introduced compression of all output files - 
making these files HTML readable makes no sense any more when they're compressed 
anyway.

=item B<-lc_pool_id <NUMBER>>

Set th LC_POOL_ID manually instead reading it from <POOL>/LastBuild. [Default:
<Read from POOL/LastBuild>]

=item B<-make_cmd <COMMAND>>

Defines the command to be executed as make command. [Default: 'imf']

=item B<-make_id_file <FILENAME>>

Filename for the file to store the actual make ID into. [Default: '$OWN/make.id']

=item B<-make_prot_path <PATH>>

Path to the error protocols (.e0 files) relative to $OWN. This path should
correspond to the make command used. [Default: 'sys/wrk/fast/prot']

=item B<-make_target <TARGET>>

Defines the target to be made. [Default: 'sdball.mac']

=item B<-make_title <NAME>>

Defines a descriptive make for the target to be made. This name will be printed
to the QADB make detail page. [Default: 'liveCache/MaxDB Server']

=item B<-man>

Displays this complete manual and then exits the program.

=item B<-pcr_bit_32>

Initiates a 32-bit PCR (Precompiler run-times) make. NOTE: This option is
DEPRICATED! Please use -make_target instead. [Default: <not used>]

=item B<-pid_file <FILENAME>>

Defines the file for storing the PID auf build_sdb into. 
[Default: $OWN/prot/build.pid]

=item B<-private>

Initiates a private make. This is 'macro' option that has no substitute within
the configuration files. Using -private is equal to: '-use_qadb=0 -use_webdav=0
-use_lcpool=0 -use_objstat=0'. Additionally, checkmake will only be executed, if
you have also given your email adress via '-email <your@ddress.net>'. [Default: 
<not used>]

=item B<-prot_dir <PATH>>

Defines the path where make protocols should be stored into. [Default: '$OWN/prot']

=item B<-prot_name <NAME>>

Defines the name of a make protocol manually. [Default: <auto generated with 
time-stamps>]

=item B<-qa_id <NUMBER>>

This defines a (old) QADB make ID manually rather than generating a new one.
This option can be used when you want to re-use an old make to add some 
information on the make datails page. The option '-use_objstat=0' may be
useful in conjuction. [Default: <auto generated NEW QA_ID>]

=item B<-scratch | -force_rm>

Initiates a scratch make. All directories defined with '-del_dirs' will be
deleted before starting the main make. [Default: <OFF/not used>]

=item B<-test_relevant <0|1>>

Marks a make either test relevant or not. If you use this option manually, you
should really know, what you're diong since this make is then used for the
productive testing machinery. Can have unpredictive side-effects - including 
angry managers wanting to have explanations from you. [Default: <derived 
automatically from $RELSTAT>]

=item B<-tool_chlist <NUMBER>>

Define a TOOL changelist manually rather than reading from the TOOL changelist
file. [Default: <read from file>]

=item B<-tool_cl_file <FILENAME>>

Defines the file that contains the tool changelist. This file should be plain
text and only contain one single line with the change list number without any
further formatting. [Default: '$TOOL/config/changelist.log']

=item B<-use_lcpool <0|1>>

Turns LC_POOL usage ON or OFF. If turned on, the LC_POOL ID derived from 
POOL/LastBuild will be stamped into the make details. See also: '-lc_pool_id'.
[Default: 1 (ON)]

=item B<-use_prot_scan <0|1>>

Activates or deactivates the post-make protocol scanner. If activated, the
PROTOCOL_SCAN section in the configuration file should be mainained properly in
order to get useful results. NOTE: If no scan parts are defined in the 
PROTOCOL_SCAN section, this option will be ignored and set to 0. 
[Default: 1 (ON)]

=item B<-use_qadb <0|1>>

Turns the usage of the QADB ON or OFF. If turned off, there will be no logging
into the QADB (except the main make protocol, which can be uploaded via
webdav). [Default: 1 (ON)]

=item B<-use_webdav <0|1>>

Activates or deactivates the usage of Webdav to upload protocols. NOTE: If
QADB usage is turned on and works correctly, this option has no effect - all
protocols are uploaded directly via QADB/HTTP. [Default: 1 (ON)]

=item B<-use_objstat <0|1>>

Enables or disables the manipulation of the QADB/OBJSTAT for a make. This 
option might be useful to be disabled when doing a second (additive) make an old 
make ID. [Default: 1 (ON)]

=item B<-x_bit <32|64>>

Defines the bits assigned to a long integer. In the moment, the only values
allowed are '32' and '64'. [Default: <derived from -bit_32_only>]

=back

=head1 DESCRIPTION

*** THIS PROGRAM IS FOR INTERNAL USE ONLY ***

This program will perform a productive make of lC/MaxDB Server. In addition to
a normal 'imf sdball.mac', this tool will also log everything into the QADB
and run checkmake afterwards to inform the developers about any errors.

For bugfixes / suggestions, please contact gerald.arnold@sap.com (51346)

=head1 EXAMPLES

 - To do an official make: build_sdb
 - To do a private make: build_sdb -private -email <your@mail.com>

=head2 Example configration file (TestBuildBln.cfg)

 #############################################################################
 # Make Configuration for make script 'build_sdb'.
 # (c)2005 SAP AG, Ulrich Jansen
 #############################################################################
 
 #############################################################################
 # ENV: This section can be used to pre-load variables into ENV.
 #
 # Each keyword in this section will be interpreted as environment variable
 # and its given value will be mapped to it.
 #
 # NOTE: There is no ENV deletion implemented, yet. Assigning an empty value
 #       to a variable will purge its content but will *NOT* delete the 
 #       variable itself!
 #
 [ENV]
 
 RELSTAT=TEST
 
 #############################################################################
 # DTM_EXPORT: This section defines variables that should be exported to
 #             the DTM system at the end.
 #
 # Each variable that should be exported must be assigned to a value that
 # evaluates to true (i.e. '1'). If you set it's value to false (0), it will
 # not be exported. (This might be useful to temporarily disable some exports.)
 #
 # NOTE: Each keyword in this section must correspond exactly to one of the
 #       variables defined below (or used internally).
 #
 [DTM_EXPORT]
 
 QA_ID=1
 LC_POOL_ID=1
 
 #############################################################################
 # DEFAULT: This section will always be loaded first. Any given machine
 #          specific section will then 'overload' these default settings.
 #
 # The following keys are valid [values in brackets indicate defaults]:
 #
 # BUILD_PURPOSE     : Purpose of the build (LC|ALL|OLTP) [ALL]
 # BIT_32_ONLY       : Initiate a 32-bit-only make [0]
 # CHLIST_FILE       : File to read the changelist from [$OWN/changelist.log]
 # CHLIST_NUM        : Manually defined changelist [auto]
 # DEL_CMD           : Command used for file deletions [rm -rf | del /S /Q]
 # DEL_DIRS          : Dirs to be deleted on a scratch make [sys/wrk,usr,test]
 # ERROUT_FILE       : Path for an error output file (for prot scanner) [none]
 # LC_POOL_ID        : Define a LC_POOL ID manually [auto]
 # MAKE_CMD          : Defines the make command to be executed [imf]
 # MAKE_ID_FILE      : File to store make ID into [$OWN/make.id]
 # MAKE_PROT_PATH    : Relative path to .e0 files [sys/wrk/fast/prot]
 # MAKE_TARGET       : Defines the target to be made [sdball.mac]
 # MAKE_TITLE        : Descriptive title for this make [liveCache/MaxDB Server]
 # PID_FILE          : PID file for build_sdb's PID [$OWN/build_sdb.pid]
 # PRIVATE           : Indicate a private make [0]
 # PROT_DIR          : Set protocol dir manually [$OWN/prot]
 # PROT_NAME         : Set name of make protocol manually [auto]
 # QA_ID             : Defines a QA_ID to be reused [NEW QADB entry]
 # SCRATCH           : If set to 1, enforces a scratch make [0]
 # TEST_RELEVANT     : Indicates test relevance [1, if $RELSTAT=PROD]
 # TOOL_CHLIST       : Define a tool changelist manually [auto]
 # TOOL_CL_FILE      : Define location of tool cl file [$TOOL/data/changelist.log]
 # USE_LCPOOL        : Indicates usage of LC_POOL [1 (Wdf), 0 (Bln)]
 # USE_OBJSTAT       : Indicates wether to change OBJSTAT in QADB [1]
 # USE_PROT_SCAN     : Indicates wether to use protocol scanner [-e ERROUT_FILE]
 # USE_QADB          : Indicates usage of QADB [1]
 # USE_WEBDAV        : Use Webdav, if QADB is not present [1]
 # X_BIT             : Set X_BIT manually to 32 or 64 [auto]
 #
 [DEFAULT]
 
 TEST_RELEVANT=0
 
 #############################################################################
 # Platform specific settings:
 #
 # The following sections will contain platform specific settings that 
 # differ from the default ones. Any settings already set in the default part
 # does not have to be stated here again, these sections act as incremental 
 # overload.
 [NTINTEL]
 
 BIT_32_ONLY=1
 #############################################################################
 # PROTOCOL_SCAN: Definitions for the post-make protocol scanner.
 #
 # This section contains all steering information about the protocol scanner
 # that runs after the main make process.
 #
 # PSCAN_PARTS defines the different sections/types/pattern matches to scan
 # for. Each part has its own header and will be shown separately in the 
 # outcoming protocol.
 #
 # Explaination:
 # The protocol scanner scans each line of the make protocol for the MATCHing
 # pattern of each scan section. If a pattern matches, the scanner checks, if 
 # the EXCLUDE filter also matches. If not, the found line is printed.
 # To disable a complete sction, just set ACTIVE to a non-true value (e.g. 0).
 #
 # NOTE: The protocol scanner is *NOT* Chackmake! It is simply a file parser
 #       that scans the complete make protocol after the make for any
 #       matching patterns. If an 'errout' file is given for that make, the
 #       scanner will output into that file. Otherwise, it will output to
 #       STDOUT.
 #
 [PROTOCOL_SCAN]
 # Parts (patterns) to scan for:
 PSCAN_PARTS=PP_ERR,PP_INC,PP_WAR,PP_FAIL,PP_JTEST
 
 # ERRORS and unknown targets:
 PP_ERR_TITLE=Errors
 PP_ERR_ACTIVE=1
 PP_ERR_MATCH=(?i)\berror|unknown\starget
 PP_ERR_EXCLUDE=Change\s+\d+\s+on|dependency\serror|errors\sin\sdependencies|NO\sDETECTED\sERRORS|Error..pp|ErrorHandling|error.h|error.c|ignored|PP_ERR_|ERR_OUTFILE|XSERVER\sis\salready\srunning|\.\.none\.|No\serrors\sfound|MAIN\sEXIT|errors\.py|-\sSummary|SutStarter\sfinished|OOM_ERROR|Description:\s|FILTER-SUMMARY\sactive|QADB_DBI|QADB_HTTPSQL|ErrorAndMessages|_ErrorStack|error_|ErrorMakeMail|PP_JTEST
 
 # Warnings:
 PP_WAR_TITLE=Warnings
 PP_WAR_ACTIVE=1
 PP_WAR_MATCH=(?i)\bwarning
 PP_WAR_EXCLUDE=Change\s+\d+\s+on|different\sways\sof\swritings|LNK4033|LNK4089|LNK4070|LNK4006|C4786|PP_WAR_|LNK4224|productive\smake!|FILTER-SUMMARY\sactive|QADB_DBI|QADB_HTTPSQL|Could\snot\sread\sconfiguration|not\sall\srequired\senvironment
 
 # Include inconsistencies:
 PP_INC_TITLE=Include inconsistencies
 PP_INC_ACTIVE=1
 PP_INC_MATCH=but\snot\sincluded|not\sin\sinclude\slist
 PP_INC_EXCLUDE=nothing_to_exclude_yet_dummy
 
 # Failures:
 PP_FAIL_TITLE=Failures
 PP_FAIL_ACTIVE=1
 PP_FAIL_MATCH=(?i)failed
 PP_FAIL_EXCLUDE=Mismatch\s+FAILED\s+Extras|PP_FAIL_|\.cpp\sfast|\.lnk\sfast|sqldbc\/Failed|_Failed\S|FailedStmt|TARGETS\sFAILED\s+=\s0|Failed\S+\.lib|Failed\S+\.exp|0\sfail
 
 # JTest errors:
 PP_JTEST_TITLE=JTEST Errors
 PP_JTEST_ACTIVE=1
 PP_JTEST_MATCH=(?i)jtest--.\s+ERROR
 PP_JTEST_EXCLUDE=nothing_to_exclude_yet_dummy


=head1 COPYRIGHT

(c)2005, SAP AG


=cut

