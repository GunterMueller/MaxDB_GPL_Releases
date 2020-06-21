#! /usr/bin/perl
#
#    @(#) build_sdb_quantify.pl (&gar) - 2005-08-29
#
#    Performs a quantify make whith logging to qadb
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

unshift @INC, "$ENV{TOOL}/lib/Perl";

use strict;

use qadb;
use FileHandle;
use File::Path ;

my $qah;
my $qa_ok;
my $qadb_error_count++;
my $lc_own = undef;
my ($sec, $min, $hour, $mday, $mon, $year) = localtime();
# format the dates
$year += 1900;
$mon += 1;
$mon =~ s/\s+//g;
my $makedate = "$year.$mon.$mday-$hour.$min.$sec";

my $ProtDir="$ENV{OWN}/prot" ;
my $MakeRelease="buildqfy_$makedate";
my $ProtName="buildqfy_${makedate}.prot";
my $MakeProt="$ProtDir/$ProtName";
mkpath ( [ "$ProtDir" ], 0, 0775 ) ;
my $rc = 0;
my $fprot = undef;
my $make_id = undef;
my $debug = undef;
my $prot_text = "";
my $qfycmd = "$ENV{'TOOLSHELL'} -S imf.pl sdbqfy.mac";
my $error_filename = "$ENV{'WRK'}/fast/prot/sdbqfy.mac.e0";
my $error_file_link = undef;
my $lcversion;

sub connect_qadb
{
   # Create a new QADB handle:
   my $id = shift;
    
   if (defined $id)
   {
   		$qah = qadb->new( {'ID' => $id } );
   		if ( $qah -> { error_code } != 0 )
		{
		  $qa_ok = 0 ;
		  $qadb_error_count++;
		  print STDERR "ERROR: ".$qah->{error_text}."\n" ;
		}
   }
   else
   {
		die "Error need MAKEID of liveCache make\n";
   }
   return ($qah);
}

# always the last is the right 
# in future: get this infomation from qadb
sub get_lc_own
{
	unless (defined $lc_own)
	{
		$lcversion = "";
		
		if ($ENV{'RELVER'} =~  /^R(\d)(\d)$/)
		{ $lcversion = "${1}.${2}.";	}
		else
		{ die "Wrong RELVER variable $ENV{'RELVER'}\n"; }
		if ($ENV{'CORRECTION_LEVEL'} =~ /^\d\d?/)
		{ $lcversion .= ( $ENV{'CORRECTION_LEVEL'} =~ /^\d\d$/ ) ? $ENV{'CORRECTION_LEVEL'} : "0$ENV{'CORRECTION_LEVEL'}"; }
		else
		{ die "Wrong CORRECTION_LEVEL variable $ENV{'CORRECTION_LEVEL'}\n"; }
		$lcversion .= ".";
		if ($ENV{'LC_STATE'} =~ /DEV|COR/i)
		{
			$lcversion .= "$ENV{'LC_STATE'}";
		}	 
		elsif ($ENV{'BUILDPRAEFIX'} =~ /^\d\d?/)
		{ $lcversion .= ($ENV{'BUILDPRAEFIX'} =~ /^\d\d$/ ) ? $ENV{'BUILDPRAEFIX'} : "0$ENV{'BUILDPRAEFIX'}"; }
		else
		{ die "Wrong BUILDPRAEFIX variable $ENV{'BUILDPRAEFIX'}\n"; }
		local @ARGV = ("-lcversion", $lcversion, "-release", "dev");
		defined ($debug) and push @ARGV, "-debugX";  
		do "$ENV{TOOL}/bin/vmakeEnv.pl";
		$lc_own = vmakeEnv::getLC_OWN();
	}
	$ENV{'LC_OWN'} = $lc_own;
	return $lc_own;
}


sub write_makeid
{
	$make_id = vmakeEnv::getLCMAKEID();
	if ( open ( MAKEID_FILE , ">$ENV{OWN}/make.id" ))
	{  print MAKEID_FILE $make_id ;  }
	else
	{  	print "WARNING: can\'t open $ENV{OWN}/make.id!\n" ;  }
	close ( MAKEID_FILE ) ;
	return ($make_id);
}


sub writelog
{
    my $text = shift;       
    $qah->write_log($text);
}

sub print_double
{
	my $text = shift;
	unless (defined $fprot)
	{
		$fprot = new FileHandle;
		if ( ! $fprot->open($MakeProt, "w") )
		{
			die "Can't open $MakeProt (output) :$!\n";
		}
	}
	print ("$text");
	$fprot->print ("$text");
	$prot_text .= "$text";
}

sub sendMail
{
	require Mail::Sendmail;
	import Mail::Sendmail;

	my $receiver = shift;
	my $cc = shift;
	my $sub =shift;
	my $msg =shift;
	
#	my $hostname =  `hostname`;
#	$hostname =~ s/\n$//;

	my %mail = ( To      => "$receiver",
              From    => 'quantifymake@sap.corp',
              Message => "$msg",
              Cc      => "$cc",
              Smtp    => 'mail.sap.corp',
              Subject => "$sub" );

	print "Send mail to $receiver\n";
	
    if ( ! sendmail(%mail) ) {
       print "FATAL: sendmail error $Mail::Sendmail::error\n";
	}
}


####################
# MAIN
####################

get_lc_own();
print_double("LC_OWN=$lc_own\n");
write_makeid();
print_double("MAKEID=$make_id\n");
if (connect_qadb($make_id))
{
	print_double("Connect to qadb successfully\n");	
	
}
else
{
	print_double ("Connect to qadb with make id failed\n");
	exit -2;
}	

writelog ("START: Make of quantify kernel (sdbqfy.mac)");
if (open (QMAKE, "$ENV{'TOOLSHELL'} -S imf.pl sdbqfy.mac |")) 
{
	while (<QMAKE>) 
	{
		print_double ("$_");
    }
    close(QMAKE);
    
    $rc = $? >> 8;
    if ($rc == 0)
    {
    	writelog ("SUCCESS: Make of quantify kernel successfully finished");
    }
    else
    {
    	writelog ("ERROR: Make of quantify kernel failed");
    	my $error_file_text = "";
    	my $ferror = new FileHandle;
    	if ( ! $ferror->open("$error_filename", "r") )
	    {
	        print_double ("Can't open $error_filename (input) :$!\n");
	    }
	    else
    	{
    		while (<$ferror>)
    		{
    			$error_file_text .= $_;
    		}
    		$ferror->close();
    	}
    	$error_file_link = $qah->write_prot ( "${MakeRelease}Error" , $error_file_text, "error protocol (quantify)");
    }
}
else
{
	writelog ("ERROR: Start of quantify kernel make failed");
	$rc = 2;
}

my $protname = "${MakeRelease}.prot";
#if (length ($prot_text) > 8000)
if (length ($prot_text) > 8)
{
	eval "require SDB::Protocol::Tools";
	if ($@)
	{	print_double ("Warning: Error while loading SDB::Protocol::Tools\n($@)\nCompressing file failed - using uncompressed file\n") ;	}
	else
	{
		import SDB::Protocol::Tools;
		my $ref_to_zip;
		my $zip_mode = 0;
		   		
		eval '($ref_to_zip, $zip_mode) = SDB::Protocol::Tools::PackScalar(\$prot_text, $ProtName , {\'toDOS\' => 1});';
		if ($@) 
		{	print STDERR "Warning: Error while compress protocol\n($@)\nUsing uncompressed file\n" ;	}
		else
		{	$zip_mode and $protname .= ".zip"; }
	}
}

my $prot_link = $qah->write_prot ( "$protname" , $prot_text, "make protocol (quantify)");
my $msg;
my $subject = "";
my $receiver = "";
my $cc = "";
my $package_name = $lcversion;
$package_name =~ tr/\.//d;

if ($rc == 0)
{
	$subject = "Neues Quantify-Paket fuer $lcversion";
	$msg = "Hallo Stephan,\n".
	       "unter $lc_own solltest Du jetzt ein neues Quantify-Paket finden.\n\n".
	       "  Details:        http://pts:1081/TestMonitor/Make_Details.jsp?id=$make_id\n".
	       "\nViele Gruesse\n". 
	       "Der automatische Axel\n";	
	$receiver = 'Stephan.Weber@sap.com';
	$cc = 'Gerald.Arnold@sap.com,Axel.Mosle@sap.com';
}
else
{	
	$subject = "Fehler beim Quantify-Make $lcversion";
	
	$msg = "Hallo Axel,\n".
		   (( $rc == 0 ) ? "das SAR-Paket fehlt ...\n\n" : "da ging wohl was schief beim Quantify-Make.\n\n").
	       "  Details:        http://pts:1081/TestMonitor/Make_Details.jsp?id=$make_id\n".
	       "  error protocol: $error_file_link\n".
	       "\nViele Gruesse\n". 
	       "Der automatische Quantify-Make\n";	
		
	$receiver = 'Axel.Mosle@sap.com';
	$cc = 'Gerald.Arnold@sap.com';
}
sendMail($receiver, $cc, $subject, $msg); 

caller () ? return ( $rc ) : exit ( $rc );
