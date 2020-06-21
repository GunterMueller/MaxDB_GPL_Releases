#! /usr/bin/perl
#
#    make_env.pl - A snmall tool to create make environments for UNIX.
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

use strict;
use Getopt::Long;
use File::Path;

##############################################################################

my $ProgramName     = "make_env";
my $Version         = "0";
my $Revision        = "02";
my $Date            = "2003-07-10";
my %Opts            = ();
my $Glob            = {};

##############################################################################

print "\n$ProgramName v$Version.$Revision ($Date), (c)2003 SAP AG\n\n";

if (!GetOptions( \%Opts, 'h|help|?', 'own=s', 'tool=s', 'src=s', 'bit=i', 'rel=s', 'dbg') || $Opts{'h'} || @ARGV)
{
    Usage();
}

$Glob->{'RELEASE'}  = ($Opts{'rel'} ? $Opts{'rel'} : RequestInput("The Release is not set!\n\nRelease (eg. '7404DEV') : ", "", ".*", "Input incorrect!\n"));

if ($Glob->{'RELEASE'} =~ /^(\d)\.?(\d)\.?(\d\d)[\._]?(DEV|COR|RAMP)$/i)
{
    $Glob->{'REL'} = "$1$2";
    $Glob->{'CORR'} = "$3";
    $Glob->{'STATE'} = lc ($4);
}
else
{
    print "\nThe release you have given has an incorrect format!\n\n";
    Usage();
}

$Glob->{'BIT'}  = ($Opts{'bit'} ? $Opts{'bit'} : "64");
$Glob->{'OWN'}  = ($Opts{'own'} ? $Opts{'own'} : RequestInput("\nThe \$OWN directory is not set!\n\nOWN-Directory : ", "", ".+", "Input incorrect!\n"));
$Glob->{'DBG'}  = ($Opts{'dbg'} ? 1 : 0);
$Glob->{'TOOL'} = ($Opts{'tool'} ? $Opts{'tool'} : ($ENV{'TOOL'} ? $ENV{'TOOL'} : "/devtool/TOOL/tool"));

unless (-d $Glob->{'TOOL'})
{
    print "\nThe tool path '$Glob->{'TOOL'}' is not valid!\n\n";
    $Glob->{'TOOL'} = RequestInput("Please select a tool path.\n\nTool path : ", "", ".+", "Input incorrect!\n");

    unless (-d $Glob->{'TOOL'})
    {
        print "\nThe given tool path '$Glob->{'TOOL'}' is still invalid!\nCan't continue without tools...Exiting.\n";
        exit(1);
    }
}

if ($Opts{'src'})
{
    $Glob->{'SRC'} = $Opts{'src'};
}
else
{
    $Glob->{'SRC'} = "/bas/SAP_DB/$Glob->{'REL'}$Glob->{'CORR'}/src/" . ($Glob->{'STATE'} eq "ramp" ? "\${BN}" : ($Glob->{'STATE'} eq "dev" ? "develop" : "$Glob->{'STATE'}"));
}

print "Using settings:\n\n";
print "Release version  = R$Glob->{'REL'}\n";
print "Correction level = $Glob->{'CORR'}\n";
print "Release own      = $Glob->{'OWN'}\n";
print "LC state         = $Glob->{'STATE'}\n";
print "Tool path        = $Glob->{'TOOL'}\n";
print "Source path      = $Glob->{'SRC'}\n";
print "Bit's used       = $Glob->{'BIT'}\n";
print "Debug mode       = " . ($Glob->{'DBG'} ? "yes\n" : "no\n");
print "\n";

print "Making paths...\n\n";

File::Path::mkpath (["$Glob->{'OWN'}"], 0, 0775);

unless (-d $Glob->{'OWN'})
{
    print "Creating directory $Glob->{'OWN'} failed!\nExiting...\n";
    exit(1);
}

print "Creating go-Script...\n\n";

my $profile_name  = "go$Glob->{'REL'}$Glob->{'CORR'}";
   $profile_name .="w" if ($Glob->{'BIT'} eq "64");
   $profile_name .= $Glob->{'STATE'} if ($Glob->{'STATE'} ne "ramp");
   $profile_name .= "dbg" if ($Glob->{'DBG'});

my $profile  = "";
   $profile  = "#!/bin/sh\n#\n# /usr/local/bin/$profile_name\n# created: " . (scalar localtime) . "\n#\n\n";
   $profile .= "OWN=$Glob->{'OWN'}\nexport OWN\n";
   $profile .= "TOOL=$Glob->{'TOOL'}\nexport TOOL\n";
   $profile .= "TOOLVARS=\$TOOL/bin/toolvars.pl\nexport TOOLVARS\n\n";
   $profile .= "PATH=\$TOOL/bin:\$PATH\nexport PATH\n\n";
   $profile .= "if [ -d \$TOOL ]\n";
   $profile .= "then\n";
   if ($Glob->{'STATE'} eq "ramp")
   {
        $profile .= "    PORT=perforce3006.wdf.sap.corp:3006\n";
        $profile .= "    BNCFG=//sapdb/V$Glob->{'REL'}/c_$Glob->{'CORR'}/ramp/buildnumber.cfg\n";
        $profile .= "    BN=`p4  -p \$PORT print \$BNCFG | grep build= | cut -c7-10`\n";
   }
   $profile .= "    perl \$TOOL/bin/createlcprofile.pl -own $Glob->{'OWN'} -tool $Glob->{'TOOL'} -prod -lc_state " . uc($Glob->{'STATE'}) . " -src $Glob->{'SRC'}";
   if ($Glob->{'BIT'} eq "64")
   {
        $profile .= " -bit64\n";
   }
   else
   {
        $profile .= "\n";
   }
   $profile .= "    cd \$OWN\n";
   $profile .= "    . \$OWN/.iprofile\n";
   $profile .= "else\n";
   $profile .= "    echo \"ERROR! TOOL does not exist!\"\n";
   $profile .= "fi\n\n";

unless (open(OUT_FILE, "> /devtool/local/bin/$profile_name"))
{
    print "ERROR! Cannot write to '/devtool/local/bin'! Writing to \$OWN instead.\n\n";
    open(OUT_FILE, "> $Glob->{'OWN'}/$profile_name") or die "Can't write profile!\n\n";
}

print OUT_FILE $profile;

close (OUT_FILE);

print "ALL done!\n\n";

##############################################################################
# Usage() - Shows the program usage and exits the script.
##############################################################################

sub Usage()
{
    print <<USAGE_END;

Usage: make_env [-(h|help|?)] [-own <OWN-DIR>] [-tool <TOOL-DIR>]
                [-src <SRC-PATH>] [-bit <32|64>] [-rel <RELEASE>]
Where:
        -h or -help or -?
            Displays this help file and ends program execution.

        -own <OWN-DIR> *
            Sets the new OWN directory to be used.

        -tool <TOOL-DIR>
            Sets the tool directory to be used. If not given, the program
            will try to read \$TOOL instead. If that fails, the program
            will try to use '/devtool/TOOL/tool' as default. If that also
            fails, you'll be asked for a path.

        -src <SRC-PATH>
            Specifies the path where sources can be found. If no path is
            given here, the default path '/bas/SAP_DB/<REL_NUM>/src/<STATE>'
            will be taken.

        -bit <32|64>
            Defines the bits to be used. Can be either 32 or 64. If no
            bit information is given, bit=64 will be assumed.

        -rel <RELEASE> *
            Defines the release. The <RELEASE> string must fit into the
            following mask: '\d\.?\d\.?\d\d[\._]?[DEV|COR|RAMP]', eg.
            '7404DEV', '7.4.04.DEV', '7404_DEV' or '74.04_DEV'

        -dbg
            Produces a go script for debug makes.

        All options marked with (*) are mandatory. If not given, the
        program will ask you for.

USAGE_END

    exit(1);
}

##############################################################################
# RequestInput() - Asks for a user input
##############################################################################

sub RequestInput
{
    my ($outstr,$defval,$inpmask,$errstr) = @_;
    my $tempval;

    if ($outstr)
    {
        print $outstr;
    }

    unless ((!$defval) || ($defval =~ /^$inpmask$/))
    {
        print STDERR "\nRequestInput: The input mask doesn't seem to be appropriate! ($inpmask)\n";
        exit(20);
    }

    $tempval = <STDIN>;
    chomp $tempval;

    until (($tempval =~ /^$inpmask$/) or ($defval and ($tempval =~ /^$/)))
    {
        print $errstr.$outstr;
        $tempval = <STDIN>;
        chomp $tempval;
    }

    if ($tempval =~ /^$/)
    {
        $tempval = $defval;
    }

    return $tempval;
}
