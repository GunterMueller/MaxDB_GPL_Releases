#!/usr/bin/perl
# @(#) knl-tcc	1995-08-01  Torsten Strahl
#
# Type-Check fuer alle Module, die von der Kerngruppe gepflegt
# werden.
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

use strict;
use Getopt::Long;
use File::Basename;
use Mail::Automail;

# Disable output buffers STDERR and STDOUT:
select (STDERR);
$| = 1;
select (STDOUT);
$| = 1;

my %Opts;
my $VersionString = "tccrun v2.01 (2004-01-19)";
my @cmd;
my $RC = 0;
my %FileList;
my $BASEDIR  = "$ENV{'WRK'}/fast/prot";
my $tccerr = "";

print "\n$VersionString\n\n";

if (!GetOptions( \%Opts, 'l', 'v', 'h|help|?') || $Opts{'h'})
{
    Usage(1);
}

print "Loading TOOLVARS.." if ($Opts{'v'});
do "$ENV{'TOOLVARS'}";
CheckRes(!$@, 1, !$Opts{'v'}, "Loading TOOLVARS failed!\n$@");

# Let's do some DOS housekeeping:
chdir ("$ENV{'WRK'}/fast/prot") || die "Can't change to directory $ENV{'WRK'}/fast/prot: $!";
unlink("$ENV{'TMP'}/all");

print "Preparing file lists.." if ($Opts{'v'});

if (@ARGV != 0) {
    foreach my $target (@ARGV)
    {
        print "." if ($Opts{'v'});
        $RC = system ("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/imf$ENV{'TOOLEXT'} -m $target >> $ENV{'TMP'}/all 2>&1");
        CheckRes (!$RC, $RC, 1, "imf -m $target failed!\n$!");
    }
}
else {
    print "." if ($Opts{'v'});
    $RC = system ("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/imf$ENV{'TOOLEXT'} -m all >> $ENV{'TMP'}/all 2>&1");
    CheckRes (!$RC, $RC, 1, "imf -m all failed!\n$!");

    if ($ENV{'RELVER'} le "R75") {
        print ".";
        $RC = system ("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/ims$ENV{'TOOLEXT'} -m quickknl >> $ENV{'TMP'}/all 2>&1");
        CheckRes (!$RC, $RC, 1, "ims -m quickknl failed!\n$!");
    }
    
    print ".";
    $RC = system ("$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/ims$ENV{'TOOLEXT'} -m slowknl >> $ENV{'TMP'}/all 2>&1");
    CheckRes (!$RC, $RC, 1, "ims -m slowknl failed!\n$!");
}

print "..OK\n\n" if ($Opts{'v'});

print "Fetching results.." if ($Opts{'v'});
CheckRes(open(FILE_IN, "<$ENV{'TMP'}/all"), 3, 1, "Can't open file $ENV{'TMP'}/all for reading!"); 
while(<FILE_IN>) { chop; $FileList{$_} += 1 if (/v[a-z][a-z]\d{2,}[^acx]{0,1}$/); }
close(FILE_IN);
print "..OK\n\n" if ($Opts{'v'});
unlink ("$ENV{'TMP'}/all");

my $TCCRC = 0;
my $PTOCRC = 0;
my $Failed = 0;

foreach my $file (sort keys %FileList)
{
	$TCCRC = 0;
	$PTOCRC = 0;
	$RC = 0;
    my $FName = basename ($file);
    
    my $OName = $FName =~ /(.*)\./ ? $1 : $FName;
    
    print "Type check $file.." if ($Opts{'v'});
    unlink("$file.check");
    unlink("$BASEDIR/$OName.chk");

    my $cmdopts = "$FName $Opts{'l'} -TY -O UNIX -C PTOC -M I386 -E SCO";
    
    if (($file =~ /src\/ak\/vak260/i) || ($file =~ /src\/ak\/vak36/i) || ($file =~ /^dg.*/i))
    {
        $cmdopts .= " -DINLINK";
        #print "Added inlink...\n";
    }
    
    $TCCRC = system("$ENV{'TOOL'}/pgm/tcc", $cmdopts);
    $PTOCRC = system("$ENV{'PYTHON'}/python $ENV{'TOOL'}/bin/ptocTcc.py $OName" );
    
    if (($TCCRC == 0) && ($PTOCRC == 0))
    {
        print "..OK\n" if ($Opts{'v'});
    }
    else
    {
        if ($TCCRC)
        {
            $RC = $TCCRC;
            $tccerr .= "TCC Error - " if ($Opts{'v'});
        }
        else
        {
            $RC = $PTOCRC;
            $tccerr .= "PTOC Error - " if ($Opts{'v'});
        }
        
        $RC = $RC >> 8;
        $Failed += $RC;
        
        if (-f "$OName.check")
        {
            $tccerr .= scalar localtime((stat("$OName.check"))[9]) . " $file" . ($Opts{'v'} ? "(RC = $RC):\n" : "\n");
            print ($Opts{'v'} ? "..ERROR, TCC result = $TCCRC, PTOC result = $PTOCRC:\n" : "ERROR ($file), TCC result = $TCCRC, PTOC result = $PTOCRC:\n");
            open (IN_FILE, "<$OName.check");
            while (<IN_FILE>) 
            {
                $tccerr .= "$_" if ($Opts{'v'}); 
                print "$_"; 
            }
            close (IN_FILE);
            $tccerr .= "\n" if ($Opts{'v'});
        }
        else
        {
            $tccerr .= "$OName" . ($Opts{'v'} ? "(RC = $RC)\n\n" : "\n");
            print "..ERROR! (RC = $RC, No information available)\n";
        }
    }
    unlink ("$OName.check");
}

if ($Failed)
{
    SendTCCMail();
}

exit 0;

sub SendTCCMail
{
    my $relstr = "V$1" if ($ENV{'RELVER'} =~ /R(\d+)$/);
    my $TccSubject = "Typencheck Probleme in $relstr CL $ENV{'CORRECTION_LEVEL'}!";
    my @PoorTccGroup;
    my $TccMsg = scalar localtime() . "\n\n$TccSubject\n\n$tccerr\n\nGruß Admin.\n\nNote: Mail generated by $VersionString\n";

    get_mail_list ("$ENV{'TOOL'}/bin/tccmail", \@PoorTccGroup);

    map {
        $_ .= '@sap.com'
    } @PoorTccGroup;

    my $TccTo = join ',', @PoorTccGroup;

   	my %mail = ( 'To'    => $TccTo,
                 'Message' => $TccMsg,
                 'Category' => 'Make',
                 'Type'     => 'Error',
                 'Infotext' => 'Typencheck Probleme');

		my $mail_ok = 0;
	
	for (1..45)
	{
    	if (Mail::Automail::Send(%mail))
    	{
    		$mail_ok = 1;
    		last;
    	}
    	sleep (60);
    }
    
    warn $Mail::Sendmail::error unless ($mail_ok);
}

sub get_mail_list {

    my $inputfile  = shift;
    my $r_maillist = shift;

    open (MAILLIST,"<$inputfile") or warn "can't open $inputfile";
    @$r_maillist = <MAILLIST>;
    close MAILLIST;
    map {
        chomp;
        s/(^.*?)#.*/$1/;
    } @$r_maillist;
    @$r_maillist = grep !/^\s*$/, @$r_maillist;

}

sub Usage
{
    my $rc = shift;
    
    print <<USAGE_END;
    
Usage: tccrun [-l] [-v] [-(h|help|?)] [Module list]

Where:
        -l
            Uses local option.
        
        -v
            Runs in verbose mode.
            
        -h, -help or -?
            Displays this help.
            
        Module List
            A list of modules to be tested. If this option is obmitted,
            a default list of kernel modules will be taken.
    
USAGE_END
    exit ($rc);
}

sub CheckRes
{
    my ($cond, $rc, $silent, $output) = @_;
    
    if ($cond)
    {
        print "..OK\n\n" unless ($silent);
    }
    else
    {
        print ($output ? "..ERROR:\n$output\n\n" : "..ERROR!\n\n");
        $rc = ($rc >> 8) if ($rc > 255);
        exit ($rc) if ($rc >= 0);
    }
    
    return (-$rc);
}
