#!/usr/bin/perl
#
#    sapdbc2depot - updates sapdbc depot files, if neccessary.
#
#    @(#)sapdbc2depot     2005-05-04
#
#    UliJ, SAP AG
#
#    ========== licence begin GPL
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
##############################################################################

use strict;
use Getopt::Long;
use File::Copy;
use File::Path;
use Mail::Sendmail;

select (STDOUT);
$| = 1;

print "\nsapdbc2depot v0.10a (2005-05-04)\n\n";

my %opts;

unless (GetOptions(\%opts, 'deliver', 'file=s', 'java=s', 'p4=s', 'email=s')) {
    Usage();
    CleanExit(2);
}

print "Checking pre-requisites:\n\n";

# Check the JAR-File:
print "Local jar file...................";
if (!$opts{'file'} && !$ENV{'OWN'}) {
    print "..\$OWN not defined! Please use '-file' option!\n";
    Usage();
    CleanExit(2);
}
$opts{'file'} = ($opts{'file'} ? $opts{'file'} : "$ENV{'OWN'}/usr/runtime/jar/sapdbc.jar");
unless (-e $opts{'file'}) {
    print "..FAILED! (File does not exist!)\n";
    CleanExit(1);
}
print "..OK\n";

#Check Java executable:
print "Java executable..................";
my $test_ok = 0;
$opts{'java'} = "java" unless ($opts{'java'});
system("$opts{'java'} -version > javaversion.txt 2>&1");
open (FILE_IN, "< javaversion.txt") or CleanExit(2);
while (<FILE_IN>) {
    $test_ok = 1 if (/Runtime\sEnvironment/);
}
close (FILE_IN);
unlink("javaversion.txt");
if ($test_ok) {
    print "..OK\n";
} else {
    print "..FAILED!\n";
    CleanExit(2);
}

#Check p4 executable:
print "P4 executable....................";
$test_ok = 0;
$opts{'p4'} = "p4" unless ($opts{'p4'});
open (CMD_OUT, "$opts{'p4'} |") or CleanExit(2);
while (<CMD_OUT>) {
    $test_ok = 1 if (/Perforce/);
}
close (CMD_OUT);
if ($test_ok) {
    print "..OK\n\n";
} else {
    print "..FAILED!\n\n";
    CleanExit(2);
}
print "Preparing local side:\n\n";
my $DestDir  = ($ENV{'TMP'} ? "$ENV{'TMP'}/sapdbc2depot" : ($ENV{'OWN'} ? "$ENV{'OWN'}/tmp/sapdbc2depot" : "/tmp/sapdbc2depot")); 
my $P4Port   = "perforce5000.wdf.sap.corp:5000";
my $P4User   = "d031096";
my $P4Client = mktemp();
my $SvrPath  = "//db/java/sapdb";
my @view     = ("$SvrPath/... //$P4Client/...");
my $file_open = 0;
my $client_created = 0;

my $local_version = "";
my $depot_version = "";
my $new_string = "";
my $local_fullver = "";
my $depot_fullver = "";
my $act_ver = "";
my $mail_sent = 0;

mkdir ("$DestDir", 0777);
print "Reading sapdbc local version.....";
unless (open(CMD_OUT, "$opts{'java'} -jar $opts{'file'} --version |")) {
    print "FAILED! (Can't execute java command!)\n";
    CleanExit(2);
}
while (<CMD_OUT>) {
    $new_string = $1 if (/(MySQL.*)$/);
    $local_fullver = ToVerStr($_) if (/MaxDB,.*\S\s*$/);
    $local_version = "$1$2$3$4$5$6$7" if (/MaxDB,\s+(\d+)\.(\d+)\.(\d+)\s+Build\s+(\d+)-(\d+)-(\d+)-(\d+)$/);
}
close (CMD_OUT);
if ($local_version) {
    print "..OK ($local_fullver)\n";
} else {
    print "..FAILED! (java string format error?)\n";
    CleanExit(2);
}

print "Creating Perforce client.........";

if (P4SetClient($P4Port, $P4Client, "$DestDir", $P4User, undef, undef, undef, \@view))
{
    $client_created = 1;
    print "..OK ($P4Client)\n";
}
else
{
    print "..FAILED ($P4Client)!\n";
    CleanExit(2);
}

if ($opts{'deliver'}) {
    CheckVersion("deliver");
    CheckVersion("misc");
} else {
    CheckVersion("misc");
}

CleanExit(0);

sub CheckVersion {
    my $version = shift;
    $act_ver = $version;
    $depot_fullver = "";
    print "\nChecking sapdbc version ($version):\n\n";
    
    print "Getting change list..............";
    
    my $chlist = P4GetChangeList($P4Port, $P4User, "$SvrPath/$version/...");
    if ($chlist)
    {
        print "..OK ($chlist)\n";
    }
    else
    {
        print "..FAILED!\n";
        CleanExit(2);
    }
    
    print "Retrieving sapdbc depot version..";
    
    if (P4Sync($P4Port, $P4User, $P4Client, "$SvrPath/$version/...", $chlist))
    {
        print "..OK\n";
    }
    else
    {
        print "..FAILED!\n";
        CleanExit(2);
    }
    
    print "Reading sapdbc depot version.....";
    unless (open(CMD_OUT, "$opts{'java'} -jar $DestDir/$version/sapdbc.jar --version |")) {
        print "FAILED! (Can't execute java command!)\n";
        CleanExit(2);
    }
    while (<CMD_OUT>) {
        $depot_fullver = ToVerStr($_) if (/MaxDB,.*\S\s*$/);
        $depot_version = "$1$2$3$4$5$6$7" if (/MaxDB,\s+(\d+)\.(\d+)\.(\d+)\s+Build\s+(\d+)-(\d+)-(\d+)-(\d+)$/);
    }
    close (CMD_OUT);
    if ($depot_version) {
        print "..OK ($depot_fullver)\n";
    } else {
        print "..FAILED! (java string format error?)\n";
        CleanExit(2);
    }
    
    if ($local_version <= $depot_version)
    {
        print "\nsapdbc depot version ($version) is up-to-date!\n\n";
        return;
    }
    
    print "Checking out depot file..........";
    
    if (P4Edit($P4Port, $P4User, $P4Client, "$SvrPath/$version/sapdbc.jar"))
    {
        print "..OK\n";
        $file_open = 1;
    }
    else
    {
        print "..FAILED!\n";
        CleanExit(2);
    }
    
    print "Switching files..................";
    
    unless (unlink("$DestDir/$version/sapdbc.jar"))
    {
        print "..FAILED (delete)!\n";
        CleanExit(2);
    }
    
    unless (copy("$opts{'file'}","$DestDir/$version/sapdbc.jar"))
    {
        print "..FAILED (copy)!\n";
        CleanExit(2);
    }
    
    print "..OK\n";
    
    print "Submitting new file..............";
    
    if (P4Submit($P4Port, $P4User, $P4Client, "$SvrPath/$version/sapdbc.jar"))
    {
        print "..OK\n";
        $file_open = 0;
    }
    else
    {
        print "..FAILED!\n";
        CleanExit(2, "Unable to submit new version!");
    }
    
    SendMail (($version eq 'deliver' ? 1 : 0), $depot_fullver, $local_fullver) unless ($mail_sent);
    
}

#############################################################################

sub Usage {
print <<USAGE_END;

Usage: 

sapdbc2depot [-deliver] [-file <JAR-FILE>] [-java <JAVA-EXECUTABLE>]
             [-p4 <PERFORCE-EXECUTABLE>] [-email <EMAIL-ADRESSES>]
             
Where:
             -deliver
              Activates 'delivery mode'.
             
             -file <JAR-FILE>
              Uses a specific jar file. If omitted, then 
              '<OWN>/usr/runtime/jar/sapdb.jar' is used.
             
             -java <JAVA-EXECUTABLE>
              Uses a specific java executable. May only be needed, if 'java'
              is not accessible without path.
              
             -p4 <PERFORCE-EXECUTABLE>
              Same as with java...
              
             -email <EMAIL-ADRESSES>
              Uses specific email adresses (comma separated without space)
              instead the default lists.

USAGE_END
}

sub CleanExit {
    my $rc = shift;
    
    if ($file_open)
    {
	    print "Reverting change.................";
	    
	    if (P4Revert($P4Port, $P4User, $P4Client, "$DestDir/$act_ver/sapdbc.jar"))
	    {
	        print "..OK\n";
	    }
	    else
	    {
	        print "..FAILED!\n";
	    }
    }
    
    if ($client_created) {    
        print "Removing P4Client................";
        if (P4RemClient($P4Port, $P4User, $P4Client))
        {
            print "..OK\n";
        }
        else
        {
            print "..FAILED!\n";
        }
    }
    
    if ($DestDir) {
        print "Removing temp directories........";
        rmtree($DestDir);
        if (-d $DestDir) {
            print "..FAILED ($DestDir)\n";
        } else {
            print "..OK\n";
        }
    }
    exit($rc);
}

sub mktemp
{
    my @time = localtime(time);
    
    return "SDBC_$time[0]$time[1]$time[2]$time[3]$time[4]$time[5]";
}

##############################################################################
# P4SetClient() - Adds / edits a perforce client.
##############################################################################

sub P4SetClient
{
    my ($p4port, $p4client, $rootdir, $owner, $host, $opts, $desc_ref, $view_ref) = @_;

    unless ($p4port)   { die "P4SetClient: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4SetClient: ERROR! Second parameter (p4client) is not set!\n" }

    if ($desc_ref && !ref($desc_ref)) { die "P4SetClient: ERROR! Seventh parameter (desc_ref) is set but is no reference! Please use a reference like \\\@desc here." }
    if ($view_ref && !ref($view_ref)) { die "P4SetClient: ERROR! Eigth parameter (view_ref) is set but is no reference! Please use a reference like \\\@views here." }

    $| = 1;

    my $href = P4GetClientInfo($p4port, $p4client);

    $href->{'Client'}       = $p4client;
    $href->{'Owner'}        = $owner if ($owner);
    $href->{'Host'}         = $host if ($host);
    $href->{'Description'}  = $desc_ref if ($desc_ref);
    $href->{'Root'}         = $rootdir if ($rootdir);
    $href->{'Options'}      = $opts if ($opts);
    $href->{'View'}         = $view_ref if ($view_ref);

    if (open CLIENT_IN, "| $opts{'p4'} -p $p4port client -i > p4out.tmp")
    {
        print CLIENT_IN "Client:\t$href->{'Client'}\n";
        print CLIENT_IN "Owner:\t$href->{'Owner'}\n";
        print CLIENT_IN "Host:\t$href->{'Host'}\n";
        print CLIENT_IN "Description:\n";
        foreach my $desc (@{$href->{'Description'}}) { print CLIENT_IN "\t$desc\n" }
        print CLIENT_IN "Root:\t$href->{'Root'}\n";
        print CLIENT_IN "Options:\t$href->{'Options'}\n";
        print CLIENT_IN "View:\n";
        foreach my $view (@{$href->{'View'}}) { print CLIENT_IN "\t$view\n" }

        unless (close(CLIENT_IN))
        {
            open (ERROUT, "< p4out.tmp");
            while (<ERROUT>) { print "$_"; }
            close (ERROUT);
            unlink "p4out.tmp";
            return 0;
        }

        unlink "p4out.tmp";
    }
    else
    {
        print STDERR "P4SetClient: Can't open 'p4 -p $p4port client -i'!\n$!\n";
        return 0;
    }

    $ENV{'P4CLIENT'} = $p4client;

    return 1;
}

##############################################################################
# P4GetClientInfo() - Retrieves information about a client.
##############################################################################

sub P4GetClientInfo
{
    my ($p4port, $p4client) = @_;

    unless ($p4port)   { die "P4GetClientInfo: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4GetClientInfo: ERROR! Second parameter (p4client) is not set!\n" }

    $| = 1;

    my $href = {};
    my $refname;
    my @tmparr;

    open CLIENT_OUT, "$opts{'p4'} -p $p4port client -o $p4client |" or print STDERR "Can't open 'p4 client -o $p4client'!\n$!\n";
    while (<CLIENT_OUT>)
    {
        if (/^(\S+):\s+(\S.*)$/)
        {
            $href->{$1} = $2;
        }
        elsif (/^(\S+):$/)
        {
            $refname = $1;
        }
        elsif (/^$/)
        {
            if ($refname)
            {
                @{$href->{$refname}} = @tmparr;
                $refname = undef;
                undef @tmparr;
            }
        }
        elsif (/^\s+(\S.*)$/)
        {
            push @tmparr, $1;
        }
    }
    close CLIENT_OUT or print STDERR "Can't close 'p4 client -i $p4client'!\n$!\n";

    return $href;
}

##############################################################################
# P4GetChangeList() - Retrieves the latest change number for a branch.
##############################################################################

sub P4GetChangeList
{
    my ($p4port, $p4user, $p4branch) = @_;

    unless ($p4port)   { die "P4GetChangeList: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4GetChangeList: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4branch) { die "P4GetChangeList: ERROR! Third parameter (p4branch) is not set!\n" }

    unless ($p4branch =~ /.*\/\.\.\./) { $p4branch .= "/..." }

    my $chlist = 0;

    open CHLIST_OUT, "$opts{'p4'} -p $p4port -u $p4user changes -m 1 -s submitted $p4branch |" or print STDERR "Can't open 'p4 -p $p4port -u $p4user changes -m 1 -s submitted $p4branch'.\n$!\n";
    while(<CHLIST_OUT>)
    {
        if (/^Change\s*(\d+)\s/)
        {
            $chlist = $1;
        }
    }
    close CHLIST_OUT;

    return $chlist;
}

##############################################################################
# P4Sync() - Synchronizes a perforce branch
##############################################################################

sub P4Sync
{
    my ($p4port, $p4user, $p4client, $p4branch, $chlist, $force, $verbose, @flist) = @_;

    unless ($p4port)   { die "P4Sync: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Sync: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Sync: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4branch) { die "P4Sync: ERROR! Fourth parameter (p4branch) is not set!\n" }
    unless ($chlist)   { $chlist = 0 }

    my $cmdparam = "";

    if (@flist)
    {
        if ($p4branch =~ /(.*)\/\.\.\./) { $p4branch = "$1" }
        foreach my $file (@flist)
        {
            $cmdparam .= " $p4branch/$file" . ($chlist ? ($chlist ne "none" ? "\@$chlist" : "#none") : "");
        }
    }
    else
    {
        unless ($p4branch =~ /.*\/\.\.\./) { $p4branch .= "/..." }
        $cmdparam = ($chlist ? ($chlist ne "none" ? "$p4branch\@$chlist" : "$p4branch#none") : "$p4branch");
    }

    my $rc = 0;

    if ($force)
    {
        $cmdparam = "-f " . $cmdparam;
    }

    $| = 1;

    open CMD_OUT, "$opts{'p4'} -s -p $p4port -u $p4user -c $p4client sync $cmdparam 2>&1 |" or print STDERR "Can't open p4 sync $cmdparam!\n$!\n";

    while (<CMD_OUT>)
    {
        if    (/^(.*):\s(.*)\s-\sadded/)       { print "Add     : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\srefreshing/)  { print "Refresh : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\supdating/)    { print "Update  : $2\n" if ($verbose) }
        elsif (/^(.*):\s(.*)\s-\sdeleted/)     { print "Deleted : $2\n" if ($verbose) }
        elsif (/^(.*):.*up-to-date/)           { print "Info    : All files are up-to-date.\n" if ($verbose) }
        elsif (/^error:\s(.*)/)                { print "Error   : $1\n" if ($verbose) }
        elsif (/^info:\s(.*)/)                 { print "Info    : $1\n" if ($verbose) }
        elsif (/^exit:\s(\d+)/)                { $rc = $1;                 }
        else                                   { print "$_" if ($verbose) }
    }
    close CMD_OUT or $rc = 40;

    return ($rc ? 0 : 1);
}

sub P4Revert
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;
    my $rc = 0;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    open (CMD_IN, "$opts{'p4'} -p $p4port -u $p4user -c $p4client revert $p4file 2>&1 |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/.*reverted$/)
        {
            $rc = 1;
        }
    }
    close (CMD_IN);
    return $rc;   

}

##############################################################################
# P4SetClient() - Adds / edits a perforce client.
##############################################################################

sub P4RemClient
{
    my ($p4port, $p4user, $p4client) = @_;
    my $rem_OK = 0;

    unless ($p4port)   { die "P4RemClient: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4client) { die "P4RemClient: ERROR! Second parameter (p4client) is not set!\n" }

    $| = 1;

    if (open CLIENT_OUT, "$opts{'p4'} -p $p4port -u $p4user client -d $p4client |")
    {
        while(<CLIENT_OUT>)
        {
            $rem_OK = 1 if(/^.*deleted\.$/);
        }
        close CLIENT_OUT;
    }
    else
    {
        print STDERR "P4RemClient: Can't open 'p4 -p $p4port client -d -f $p4client'!\n$!\n";
    }

    $ENV{'P4CLIENT'} = undef if ($rem_OK);

    return $rem_OK;
}

sub P4Edit
{
    my ($p4port, $p4user, $p4client, $p4file) = @_;
    my $rc = 0;

    unless ($p4port)   { die "P4Edit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Edit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Edit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)    { die "P4Edit: ERROR! Fourth parameter (flist) is not set!\n" }

    open (CMD_IN, "$opts{'p4'} -p $p4port -u $p4user -c $p4client edit $p4file |") or return 0;
    
    while (<CMD_IN>)
    {
        if (/.*opened\sfor\sedit$/)
        {
            $rc = 1;
        }
    }
    close (CMD_IN);
    return $rc;   
}

sub P4Submit
{
    my ($p4port, $p4user, $p4client, $p4file, $p4desc) = @_;
    my $rc = 0;
    my $clist = 0;

    unless ($p4port)   { die "P4Submit: ERROR! First parameter (p4port) is not set!\n" }
    unless ($p4user)   { die "P4Submit: ERROR! Second parameter (p4user) is not set!\n" }
    unless ($p4client) { die "P4Submit: ERROR! Third parameter (p4client) is not set!\n" }
    unless ($p4file)   { die "P4Submit: ERROR! Fourth parameter (flist) is not set!\n" }
    unless ($p4desc)   { $p4desc = "P4Submit: Implicit changelist created for file '$p4file'" }

    unless (open (CMD_OUT, "| $opts{'p4'} -p $p4port -u $p4user -c $p4client submit -i > tmpout.txt")) { return 0; }

    print CMD_OUT "Change: new\n\n";
    print CMD_OUT "Description: $p4desc\n\n";
    print CMD_OUT "Files:\n";
    print CMD_OUT "\t$p4file\n";
    
    close (CMD_OUT);

    open (FILE_IN, "< tmpout.txt") or return 0;

    while (<FILE_IN>)
    {
        $rc = 1 if (/.*submitted\.$/);
    }
    
    close (FILE_IN);
    unlink ("tmpout.txt");
    
    return $rc;   
}

sub ToVerStr {
    my $verstr = shift;
    return "$1 Build: $2" if ($verstr =~ /MaxDB,\s+(\S+)\s+Build\s+(\S+)\s*/);
}

sub SendMail {
    my ($deliver, $old_ver, $new_ver) = @_;
    my %mail;
    
    $mail{'From'}   = 'automail@maxdb.sap.corp';
    $mail{'To'}     = ($opts{'email'} ? $opts{'email'} : ($deliver ? 'DL_NWDI_SERVICES@exchange.sap.corp,marco.paskamp@sap.com,alexander.schroeder@sap.com,thomas.elvers@sap.com' : 'marco.paskamp@sap.com,alexander.schroeder@sap.com,frank.strassenburg@sap.com,axel.mosle@sap.com'));
    $mail{'Cc'}     = 'gerald.arnold@sap.com';
    $mail{'Bcc'}    = '';
    $mail{'Smtp'}   = 'mail.sap.corp';
    $mail{'Subject'}= "MaxDB - New " . ($deliver ? '' : 'DEVELOPMENT ') . "version of JDBC driver available $new_ver";
    $mail{'Message'} = "A new " . ($deliver ? 'official' : 'development') . " version of MaxDB JDBC driver is available at\n\n";
    $mail{'Message'}.= "Perforce    : perforce5000.wdf.sap.corp:5000\n";
    $mail{'Message'}.= "File        : //db/java/sapdb/" . ($deliver ? 'deliver' : 'misc') . "/sapdbc.jar\n";
    $mail{'Message'}.= "New version : $new_ver\n";
    $mail{'Message'}.= "Old version : $old_ver\n\n";
    $mail{'Message'}.= "Regards,\nTeam MaxDB\n\n";
    $mail{'Message'}.= "--------------------------------------------------------------------\n\n";
    $mail{'Message'}.= "Please note: This mal has been sent automatically to inform you about a new version of the JDBC driver of MaxDB. Please send a note to mailto:marco.paskamp\@sap.com if you or a colleague of you is to be included/excluded.\n";
    print "Sending mail.....................";
    if (sendmail(%mail)) {
        print "..OK\n";
        $mail_sent = 1;
    } else {
        print "..FAILED! ($Mail::Sendmail::error)\n";
    }
    return;
}
