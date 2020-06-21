#!/usr/bin/perl
# odbctest 
#
# @(#)odbctest  6.2  1997-10-15 
#
# Generate and start all ODBC Testfiles
# 
# Burkhard Diesing
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

package odbctest;
use Env;
use PCTestTools; # needed for including the right PCTest.pm
use Purify;
sub RunTest;
sub GetDSNFromInclude;
sub RegisterDriver;

do $TOOLVARS;

if ( !defined $OWN ) {
    $OWN = PCTest::GetOWN;
}

$TESTSCRIPT="ODBC Tests";
$ENV{SQLOPT}="";
$OPT="";
$MAIL="Yes";
$PARFILE="$OWN/test/jtest/tests/parameters/Odbc.par";

#regular expression list of servers who will send mails to the reciever
@sender = ( ".*", "P26325", "P26326", "P26327" );

#list of users who will receive a mail about the testresults
@receiver = ( "D025086" );

$USAGE ="USAGE: odbctest [options]\n";
$USAGE.="   \noptions  :\n";
$USAGE.="      -cleanup          : Cleans the output from previous runs.\n";
$USAGE.="      -nocore           : Switch to the debugger in case of a core.\n";
$USAGE.="      -purify           : runs all programs under Purify.\n";
$USAGE.="      -debug            : Include debug information.\n";
$USAGE.="      -keep             : Keeps all intermediate file.\n";
$USAGE.="      -nomail           : Do not send a mail with the testresults to @receiver.\n";
$USAGE.="      -parfile \"<file>\" : Use parameter file <file> instead of default file.\n";
$USAGE.="                          Default parameter file is '$PARFILE'.\n";


$i=0;
foreach $_ (@ARGV) {
    $i++;
    if ( /-cleanup/ ) { 
	$OPT.=$_;
	$MAIL="No";
    }
    if ( /-nocore/ ) { 
	$OPT.=$_;
    }
    if ( /-debug/ ) { 
	$OPT.=$_;
    }
    if ( /-norun/ ) {
	$OPT.=$_;
    }
    if ( /-purify/ ) {
	$OPT.=$_."-debug";
    }
    if ( /-keep/ ) { 
	$OPT.=$_;
    }
    if ( /-nomail/ ) { 
	$MAIL="No";
    }
    if ( /-\?|-h/ ) {
	print $USAGE;
	exit;
    }
    if ( /-parfile/ ) {
        $PARFLAG = "INIT";        
    } elsif (defined $PARFLAG && $PARFLAG eq "INIT") {
        $PARFLAG="YES";
        $PARFILE = $_;
    }
    next;
}

if ( ! -e $PARFILE ) {
  PCTest::WriteProt(PCTest::date()." $PARFILE not found.");
    exit -1;
}

 PCTest::WriteProt(PCTest::date()." Start $TESTSCRIPT");


# set LD_LIBRARY_PATH for ssl-libraries
if (!defined $ENV{ISWDFNACHT}) {
  $ENV{LD_LIBRARY_PATH}=$ENV{LD_LIBRARY_PATH} . ":$OWN/usr/lib";
} else {
  $ENV{LD_LIBRARY_PATH}=$ENV{LD_LIBRARY_PATH} . ":$INDEPLIB";
}

 odbctest::GetDSNFromInclude ( "$TEST/pc/incl/winhead.h" );
 odbctest::RegisterDriver ( "$DSN" );
 odbctest::RunTest;

sub RunTest
{
    local($DSN) = $_[0];
    
    if ($OPT =~ /purify/)  {
&if $OSSPEC in [ WIN32 ]
      Purify::WriteResultHeader ();
&else
      Purify::WriteResultValgrindHeader ();
&endif
    }

    open (PARF, $PARFILE);
    
    PCTest::pctest ( "-X", "odbc/initodbc", $OPT);
    while (<PARF>) {
        s/\/\/.*|\#.*//g;
        if (/odbc\/\w+/) {
            PCTest::pctest ($`, $&, (split ' ', $'), $OPT);
        }
    }    
    
    close (PARF);

    if ($OPT =~ /purify/)  {
&if $OSSPEC in [ WIN32 ]
      Purify::WriteResultFooter ();
&else
      Purify::WriteResultValgrindFooter ();
&endif
    }

    Purify::Rename ("odbc");
}

if ( $MAIL =~ /Yes/ ) {
    local( $subject, $SendFile, $cmd );
    local( $receiver, $sender);

    $SendFile = PCTest::GetPCProtFile;
    $subject  = $TESTSCRIPT;

    LOOP: foreach $sender (@sender){
	if ( $COMPUTERNAME =~ /$sender/ ) {
	    foreach $receiver (@receiver) {
		if ( ! (uc($USERNAME) =~ /$receiver/) ) {
		    do "imail$TOOLEXT -u $receiver -s \"$subject\" $SendFile";
		}
	    }
	    last LOOP;
	}
    }	
}

sub GetDSNFromInclude
{
    $DSN="";
    $incfile=@_[0];
    open(FILE_IN, "$incfile") || die "Error: '$incfile' not found.";
    LINE: while(<FILE_IN>) {
	if ( /^#\s*define\s+DATASOURCENAME\s+"(.*)"/ ) {
	    $DSN=$1;
	    last LINE;
	}
    }
    close(FILE_IN);    
}

sub RegisterDriver
{
    $SERVERDB=$ENV{SERVERDB};
    $SERVERNODE=$ENV{SERVERNODE};
    local ($XX) = "";
    $XX="$XX SERVERDB" if (length $SERVERDB == 0);
    die "Env $XX not set.\n" if ( length $XX );
    
    local ($TMP)="\"$DSN\" Driver \"$DBROOT\\pgm\\sqlod32.dll\"";
  PCTest::WriteProt(PCTest::date()." Register ODBC Driver $TMP");
    system("upddsn $TMP");

    local ($TMP)="\"$DSN\" Setup \"$DBROOT\\pgm\\sqlsp32.dll\"";
    system("upddsn $TMP");

    local ($TMP)="\"$DSN\" ServerDB \"$SERVERDB\"";
    system("upddsn $TMP");

    local ($TMP)="\"$DSN\" ServerNode \"$SERVERNODE\"";
    system("upddsn $TMP");
}

__END__
