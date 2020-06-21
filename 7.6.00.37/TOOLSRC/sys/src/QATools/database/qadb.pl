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

use strict;

use Getopt::Long;

require qadb; 	# As long this module throws errors on windows machines, it has
import qadb;  	# to be imported rather than used to avoid editor compile errors.

my $Program  	= "QADB";
my $Version 	= "1.00";
my $Date     	= "07/10/03";
my $IDString	= "$Program v$Version ($Date)"; 

my $Cmd = shift @ARGV; # Command to be executed.
my %Opts;		# Takes all global program options.
my $qah;
my $write_mode  = 0;
my $str_data    = "";

if (!GetOptions( \%Opts,
                'rel=i',
                'build=i',
                'id=i',
                'state=s',
                'string=s',
                'file=s',
                'linkname=s',
                'linktext=s',
                'cl=i',
                'toolcl=i',
                'purpose=s',
                'idfile=s',
                'platform=s',
                'v|verbose',
                'status=i',
                'testprofile=s',
                'h|help|?') || $Opts{'h'})
{
    Usage();
}

print "$IDString\n\n" if ($Opts{'v'});

if ($Cmd eq "new")
{
    # Check for mandatory options:
    MandatoryOpt($Opts{'rel'}, "-rel");
    MandatoryOpt($Opts{'build'}, "-build");
    
    # Correct any unset option:
    $Opts{'state'}   = "DEV" unless ($Opts{'state'});
    $Opts{'purpose'} = "ALL" unless ($Opts{'purpose'});
    $Opts{'cl'}      = "0"   unless ($Opts{'cl'});
    $Opts{'toolcl'}  = "0"   unless ($Opts{'toolcl'});

    if ($Opts{'platform'})
    {
	    $qah = qadb->new( {'VERSION'          => "$Opts{'rel'}" ,
	                       'BUILDPFX'         => "$Opts{'build'}" ,
	                       'QASTATUS'         => "$Opts{'state'}" ,
	                       'CHANGELIST'       => "$Opts{'cl'}" ,
	                       'PLATFORM'         => "$Opts{'platform'}",
	                       'BUILD_PURPOSE'    => "$Opts{'purpose'}" });        
    }
    else
    {
	    $qah = qadb->new( {'VERSION'          => "$Opts{'rel'}" ,
	                       'BUILDPFX'         => "$Opts{'build'}" ,
	                       'QASTATUS'         => "$Opts{'state'}" ,
	                       'CHANGELIST'       => "$Opts{'cl'}" ,
	                       'BUILD_PURPOSE'    => "$Opts{'purpose'}" });        
    }  
    
    if ($qah->{'error_code'} == 0)
    {
        print "OK\nQA-ID=$qah->{'ID'}\n";
    }
    else
    {
        print "ERROR\nCreating new QADB entry failed:\n$qah->{'error_string'}\n";
        exit(1);
    }  
}
elsif (($Cmd eq "write") or ($Cmd eq "writelog") or ($Cmd eq "writeprot"))
{
    # Again, check the mandatory options:
    MandatoryOpt($Opts{'id'}, "-id");
    MandatoryOpt(($Opts{'string'} || $Opts{'file'}), "-string or -file");
    
    $qah = qadb->new ( {'ID' => "$Opts{'id'}"} );
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nCould not connect to QADB with ID '$Opts{'id'}'!\n$qah->{'error_string'}\n";
    }
    
    $str_data = "";

    if ($Opts{'string'})
    {
        $str_data = $Opts{'string'};
    }
    
    if ($Opts{'file'})
    {
        open (IN_FILE, "< $Opts{'file'}") or die "ERROR\nCan't open file '$Opts{'file'}' for reading!\n$!\n";
        $str_data .= <IN_FILE>;
        close (IN_FILE);
    }
    
    if (length($str_data) > 1023)
    {
        $write_mode = ($Cmd ne "writelog") ? 2 : die "ERROR\nData too long to be written as log file! Use write prot instead.\n";
    }
    elsif (length($str_data) == 0)
    {
        die "ERROR\nNo Data to write! Please, give either a -string or a -file.\n";
    }
    else
    {
        $write_mode = ($Cmd eq "writeprot") ? 2 : 1;
    }
}
elsif ($Cmd eq "setstate")
{
    MandatoryOpt($Opts{'id'}, "-id");
    MandatoryOpt($Opts{'status'}, "-status");
    
    $qah = qadb->new ( {'ID' => "$Opts{'id'}"} );
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nCould not connect to QADB with ID '$Opts{'id'}'!\n$qah->{'error_string'}\n";
    }
    
    $qah->update_columns({'IDOBJSTATUS' => "$Opts{'status'}"});
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nCould not update column IDOBJSTATUS:\n$qah->{'error_string'}\n";
    }   
    else
    {
        print "OK\n";
    }
}
elsif ($Cmd eq "setprofile")
{
    MandatoryOpt($Opts{'id'}, "-id");
    MandatoryOpt($Opts{'testprofile'}, "-testprofile");
    
    $qah = qadb->new ( {'ID' => "$Opts{'id'}"} );
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nCould not connect to QADB with ID '$Opts{'id'}'!\n$qah->{'error_string'}\n";
    }
    
    $qah->create_testprofile($Opts{'testprofile'});
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nCould not create test profile:\n$qah->{'error_string'}\n";
    }   
    else
    {
        print "OK\n";
    }
}
else 
{
    print "ERROR\nCommand '$Cmd' not found!\n";
    Usage();
}

if ($write_mode == 1)
{
    $qah->write_log($str_data);
    
    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nWrite log failed:\n$qah->{'error_string'}\n";
    }
    else
    {
        print "OK\n"; 
    }
}
elsif ($write_mode == 2)
{
    my $fname = ($Opts{'linkname'} ? $Opts{'linkname'} : ($Opts{'file'} ? $Opts{'file'} : die "ERROR\nWhen writing a protocol, you have to give either a -file or a -linkname!\n"));

    if ($Opts{'linktext'})
    {
        $qah->write_prot ($fname, $str_data, $Opts{'linktext'});
    }
    else
    {
        $qah->write_prot ($fname, $str_data);
    }

    if ($qah->{'error_code'} != 0)
    {
        die "ERROR\nWrite prot failed:\n$qah->{'error_string'}\n";
    }
    else
    {
        print "OK\n";
    }
}

if ($Opts{'idfile'})
{
    if (open(OUT_FILE, "> $Opts{'idfile'}"))
    {
        print OUT_FILE "$qah->{'ID'}";
        close (OUT_FILE);
        print "QADB: ID $qah->{'ID'} stored successfully in file '$Opts{'idfile'}'\n" if ($Opts{'v'});    
    }
    else
    {
        die "QADB: Couldn't store ID $qah->{'ID'} in file '$Opts{'idfile'}'!\n$!\n";
    }
}

################################################################################
# MandatoryOpt() - Checks wether a mandatory option is set or not.
################################################################################
sub MandatoryOpt
{
    my ($val, $name) = @_;
    
    unless ($val) 
    {
        print "ERROR\nMandatory parameter '$name' is not set!\n";
        Usage();
    }
}

################################################################################
# Usage() - Shows program usage and then exits.
################################################################################
sub Usage
{
	print <<USAGE_END;
	
Usage:  qadb <cmd> [-build <i>] [-cl <i>] [-file <s>] [-(h|help|?)] [-id <i>]
                   [-idfile <s>] [-linkname <s>] [-linktext <s>]
                   [-platform <s>] [-purpose <s>] [-rel <i>] [-state <s>]
                   [-string <s>] [-testprofile <s>] [-toolcl <i>]
                   [-(v|verbose)]

Where:

    <cmd>
        This is the command to be executed. Known commands are (mandatory
        parameters are listed behind command):
        
        o 'new' (-rel <i> -build <i>)
        o 'setstate' (-id <i> -status <i>)
        o 'write' (-id <i> ((-string <s> and -linkname <s>) or -file <s>))
        o 'writeprot' (-id <i> ((-string <s> and -linkname <s>) or -file <s>))
        o 'writelog' (-id <i> (-string <s> or -file <s>))
        
    -build <i>
        Set the actual build prefix to be used. This can be a one- or two-
        digit number.
    
    -cl <i>
        Sets the number of the change list used.
    
    -file <s>
        Here you can give a file to be inserted into qadb. This file will be
        inserted _AFTER_ any -string <s>.
        
    -h or -help or -?
        Will show this help file and then exit the program.
        
    -id <i>
        Sets the QA-ID to be used. Must be a valid QA-ID.
        
    -idfile <s>
        If this option is given, the QA-ID used will be stored in the given
        file name.
        
    -linkname <s>
        If a linkname is given it will be shown as protocol file name rather
        than the orifinal one.
        
    -linktext <s>
        If this option is given, it will be shown as link text to a protocol.
        
    -platform <s>
        If this option is given, the platform name will be used rather than
        the one calculated by qadb itself.
        
    -purpose <s>
        Sets the build purpose. If not given, 'ALL' will be assumed.
        
    -rel <i> 
        Sets the release version including the correction level, i.e. '7500'.
      
    -state <s>
        Defines the LC_STATE for the release. If not given, 'DEV' will be 
        assumed.
        
    -string <s>
        If given, the string will inserted into qadb. This string will be
        inserted _BEFORE_ any -file <s>.
        
    -testprofile <s>
        Defines a test profile string.
        
    -toolcl <i>
        Sets the number of the tool change list used.
        
    -v or -verbose
        If this option is set, the program will produce a more detailed output.
	
Note:

    <i> = Positive integer number.
    <s> = String value. If it contains spaces, it has to be written in 
          double quotes ("c:\i have spaces\me too.dat").
	
USAGE_END

    exit(0);
}
