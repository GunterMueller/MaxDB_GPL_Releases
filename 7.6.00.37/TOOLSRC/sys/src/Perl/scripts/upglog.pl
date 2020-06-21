#!/usr/bin/perl
#
#    sutcheck.pl
#
#    @(#)sutcheck 2006-04-13
#
#    Marco Baar, SAP AG
#
#    ========== licence begin LGPL
#    Copyright (C) 2006 SAP AG
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
    }

    unshift @INC, "$ENV{'TOOL'}/lib/perl5";
    unshift @INC, "$ENV{'TOOL'}/bin";
    
    if (-e "/devtool/local/bin")
    {
        $ENV{'PATH'} = "/devtool/local/bin:$ENV{'PATH'}";
    }
    
}


use strict;
use IO::ZipFile;
use IO::SimpleCfg;
use IO::FilePath;
use IO::DelTree;
use File::Path;
use File::Copy;
use File::Compare;
use Getopt::Long;
use Perforce::P4Info;
use Perforce::P4Admin;
use Perforce::P4Sync;
use Perforce::P4File;
use Tool::RequestInput;
use Tool::HashRef;
use Tool::SQLCli;
use Net::HTTP;
use HTTP::Status;

###############################################################################
# Global Variables:
###############################################################################
my $ProgramName       = "upglog";
my $Version           = "1";
my $Revision          = "09";
my $Date              = "2006-05-08"; 
my %Opts;
my $Cfg               = {};
my $CfgFile           = "$ENV{'TOOL'}/config/upglog.cfg";
my $upgFile	      = shift;
my $unpack        = "C:\\TEMP\\upgradetest_upgs";

###############################################################################
# Program initialisation:
###############################################################################
print "\n$ProgramName v$Version.$Revision ($Date), (c)2004 SAP AG\n\n";

print "param: $upgFile\n\n";

if (!GetOptions( \%Opts,
                'h|help|?',
                'register',
                'unregister',
                'showcfg') || $Opts{'h'})
{
    Usage();
}

LoadCfg();




###############################################################################
# Windows registration handling:
###############################################################################
if ($Opts{'register'} || $Opts{'unregister'})
{
    if ($^O =~ /MsWin32/i)
    {
        if ($Opts{'register'})
        {
           RegisterFileExt();
        }
        else
        {
            UnregisterFileExt();
        }
    }
    else
    {
        print "Sorry! This function is only available for Windows.\n";
    }
    exit(0);
}


###############################################################################
# Show config and exit:
###############################################################################
if ($Opts{'showcfg'})
{
    ShowHashRef($Cfg);
    exit(0);
}



###############################################################################
# unzip UPG-File):
###############################################################################
    

print "\nUnzipping file...\n";
print "Upgfile '$upgFile'\n";
print "UNPACKDIR '$unpack'\n";
unless (UnzipFile($upgFile, $unpack))
{
	print "Error - Abording\n";
	return 0;
}
print "OK\n\n";

###############################################################################
# start index.html in main-browser
###############################################################################
    

system("$unpack\\index.html");

sleep (10);
exit(0);



#---------------------------------  METHODS ------------------------------------

###############################################################################
# LoadCfg() - Loads the config file and then overloads cmdline params:
###############################################################################

sub LoadCfg
{
    if (-e "$ENV{'TOOL'}/config/upglog.cfg")
    {
	print "Config file exists at: $ENV{'TOOL'}/config/upglog.cfg\n";
	print "Reading config file ...";

      $Cfg = LoadSimpleCfg($CfgFile);

	$unpack = $Cfg->{'UNPACKDIR'};      
	print " OK\n";
	print "Following information read:\n";

    }
    else{
	print ">> config does not exist at: $ENV{'TOOL'}/config/upglog.cfg\n";
    }
	print "UNPACKDIR = '$unpack'\n";

#    $Cfg->{'BROWSER'} = (defined $Opts{'browser'} ? $Opts{'browser'} : (defined $Cfg->{'BROWSER'} ? $Cfg->{'BROWSER'} : 0));
#    $Cfg->{'INDEX'}   = (defined $Opts{'index'}   ? $Opts{'index'}   : (defined $Cfg->{'INDEX'}   ? $Cfg->{'INDEX'}   : 0));
#    $Cfg->{'TMPDIR'}  = (defined $Opts{'tmpdir'}  ? $Opts{'tmpdir'}  : (defined $Cfg->{'TMPDIR'}  ? $Cfg->{'TMPDIR'}  : 0));
#    
}


###############################################################################
# RegisterFileExt() - Registers file extension for .upg files
###############################################################################
sub RegisterFileExt
{
    require Win32::TieRegistry;
    import  Win32::TieRegistry ( ArrayValues=>1 );
    
    print "Registering file extension .upg ...";
    
    my $Reg = Win32::TieRegistry->new("Classes")->TiedRef();
    $Reg->Delimiter("/");
    $Reg->{".upg/"} = {"/" => "upg_auto_ext"};
    $Reg->{"upg_auto_ext/"} = { "shell/" => { "open/" => { "command/" => { "/" => ($ENV{'TOOLSHELL'} ? "$ENV{'TOOLSHELL'}" : ($ENV{'PERL'} ? "$ENV{'PERL'}\\bin\\perl.exe" : "$ENV{'TOOL'}\\Perl\\bin\\perl.exe")) . " $ENV{'TOOL'}\\bin\\upglog.pl \"%1\"" } } } };
    print "..OK\n";    
}

###############################################################################
# UnregisterFileExt() - Unregisters .upg file extension again
###############################################################################
sub UnregisterFileExt
{
    require Win32::TieRegistry;
    import  Win32::TieRegistry ( Delimiter=>"/", ArrayValues=>1 );
    
    print "Unregistering file extension .upg ...";
    
    my $file_ext = "upg";
    my $Reg = Win32::TieRegistry->new("Classes")->TiedRef();
    $Reg->Delimiter("/");
    
    my $keyname = $file_ext . "_auto_ext";
    
    delete $Reg->{".upg/"};
    delete $Reg->{"upg_auto_ext/shell/open/command/"};
    delete $Reg->{"upg_auto_ext/shell/open/"};
    delete $Reg->{"upg_auto_ext/shell/"};
    delete $Reg->{"upg_auto_ext/"};
    print "..OK\n";    
}

###############################################################################
# Usage() - Shows program usage and exits program.
###############################################################################
sub Usage
{
    print <<USAGE_END;
Usage: upglog [-options] upgradeLogfile.upg

Options:
    -all <0|1>     Check all users file, if set to 1.
    -ed [Editor]   Editor to view diff files. Use 'diffview' to use diffview.
    -h | help      Shows this help.
    -p4p <PORT>    Sets a Perforce port other than 'perforce:3006'.
    -p4u <USER>    Sets a Perforce user other than '-user'.
    -p4pw          Sets the environmant variable %P4PASSWD%.
    -register      Registers file extension '.sck' to be a sutcheck file.
    -save          Save program options under %TOOL%\\config\\shutcheck.cfg
    -unregister    Removes file extension registration for '.sck' again.
    -user <User>   Take given D-User number and check only these files.
    -wrkdir <Dir>  Use given directory as working directory.
    -showcfg       Show config used and exit.
    -sqlcli <File> Uses <File> to add entries into PTS database.
    
USAGE_END
    #sleep (5);
    exit(0);
}
