#!/usr/bin/perl
#
# build_lcapps_runner.pl Script
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2002 SAP AG
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
################################################################################

use BuildPackage::Vars;
use lcmake ;
use File::Path ;
use Getopt::Long;
use POSIX;
use File::Copy;
use qadblca;
use LCAMakePreCheck;
use Cwd;
use strict;


################################################################################
################## main ########################################################
################################################################################

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

my $ProtDir;
my $MakeProtFile;
my $MakeProt;
my @args_rmtree;
my $CHANGELIST_NO;
my $TOOL_CHANGELIST_NO;
my $qahlca;
my $qalca_id;
my $qalca_ok = 1;
my $found_dsp2vmake_description = 0;
my $ret;
my $SCRATCH;
my $SCRATCH1;
my $HELP;
my $QASTATUS;
my $TYPE;
my $PRECHECK = undef;
my $LCPOOL_ID="0";
my $idobjstat = 0;
my $prot_text="";
my $COPY_SRC = undef;

# objstat defines 
my $WRITE_TOOLCL=50;
my $WRITE_BRANCH_INFO=45;
my $WRITE_PATCHLEVEL=40;
my $MAKE_SKIPPED=7;
my $START_DSP2VMAKE=601;
my $ERROR_DSP2VMAKE=609;
my $SUCCESS_DSP2VMAKE=610;
my $START_MAKE_AND_BUILD=611;
my $ERROR_MAKE_AND_BUILD=619;
my $SUCCESS_MAKE_AND_BUILD=620;
my $WRITE_LCPOOL_ID=950;
my $WRITE_LCPOOL_ID=950;
my $ERROR_ALL=999;
my $SUCCESS_ALL=1000;
my $qadb_error_count=0;
my @BUILTTARGETS = ();
my $zip_mode = 0;
my $MAKE_CMD = "$ENV{'TOOLSHELL'} $ENV{'TOOL'}/bin/lcmake.pl -i --report_percent lcapps_factory.mac";
($^O =~ /MSWin32/i) and $MAKE_CMD =~ s/\//\\/g;

#------------------------------------------------------------------------------#
#   get some machine informations
#------------------------------------------------------------------------------#

prot_parts ( "uname" ) ;
my ( $sysname, $nodename, $release, $version, $machine ) = POSIX::uname();

#------------------------------------------------------------------------------#
#   create protocol name of the make
#------------------------------------------------------------------------------#

prot_parts ( "build prot name" ) ;


my ($sec, $min, $hour, $mday, $mon, $year) = localtime();
my $makedate;
# format the dates
$year += 1900;
$mon += 1;
$mon =~ s/\s+//g;
$makedate = "$year.$mon.$mday-$hour.$min.$sec";

$ProtDir="$ENV{OWN}/prot" ;
$MakeProtFile="NACHT_LCAPPS_$ENV{APO_COM_SHORT}_${nodename}_${makedate}.prot";
$MakeProt="$ProtDir/$MakeProtFile";
mkpath ( [ "$ProtDir" ], 0, 0775 ) ;


print "OUTPUT_FILTER: OPEN FILE $MakeProt\n";


#------------------------------------------------------------------------------#
#   read options
#------------------------------------------------------------------------------#

Getopt::Long::Configure ("no_auto_abbrev");
prot_parts ( "read and check options" ) ;
if ( GetOptions ( "force_rm"    => \$SCRATCH,
                  "scratch"     => \$SCRATCH1,
                  "precheck=s"    => \$PRECHECK,
                  "copy_src"     => \$COPY_SRC,
                  "type=s"      => \$TYPE,
                  "help"        => \$HELP,
                  "h"           => \$HELP )  and not  $HELP )
{	$SCRATCH = 1 if ( defined $SCRATCH || defined $SCRATCH1 );}
else
{
	print_usage () ;
	$qalca_ok = 0;
	finish_make ();
	caller () ? return ( 1 ) : exit ( 1 ) ;
}

if ($TYPE)
{
	$ENV{VMAKE_DEFAULT_VARIANT} = ($TYPE =~ /dbg/i) ? "Debug" : "Release"; 
}
else
{	
	$TYPE = ( $ENV{VMAKE_DEFAULT_VARIANT} =~ /Debug/i ) ? "dbg" : "opt"; 
}

#------------------------------------------------------------------------------#
#   check for lcapps settings
#------------------------------------------------------------------------------#
prot_parts ( "check for lcapps settings" ) ;
unless ( $ENV{LCMAKEID} )
{
  print "WARNING: can\'t find LCMAKEID in environment!\n" ;
  $ENV{LCMAKEID} = 1;
}
unless ( $ENV{APO_COM_SHORT} )
{
  print "WARNING: can\'t find APO_COM_SHORT in environment!\n" ;
  $ENV{APO_COM_SHORT} = "0";
}
if ( $ENV{BRANCH_INFO} )
{
	if ($ENV{BRANCH_INFO} =~ /(COR|DEV|HOT)$/)
	{	$QASTATUS = $1;	}
	elsif ($ENV{BRANCH_INFO} =~ /^CONS$/i)
	{	$QASTATUS = "COR";	}
	else
	{	$QASTATUS = "RAMP";	}
}
else
{
  print "WARNING: can\'t find BRANCH_INFO in environment!\n" ;
  $QASTATUS = "NONE";
}

#------------------------------------------------------------------------------#
#   read changelist number
#------------------------------------------------------------------------------#
prot_parts ( "read changelist number" ) ;
if ( open ( CHANGELIST_LOG , "$ENV{OWN}/changelist.log" ))
{
   if ( $CHANGELIST_NO = <CHANGELIST_LOG> )
   {      chomp ( $CHANGELIST_NO ) ;   }
   close ( CHANGELIST_LOG ) ;
}
elsif ( $ENV{LATEST_CHANGE} )
{
	$CHANGELIST_NO=$ENV{LATEST_CHANGE};
}

unless ($CHANGELIST_NO)
{
  print "WARNING: can\'t read changelist number!\n" ;
  $CHANGELIST_NO = 0;
}


#------------------------------------------------------------------------------#
#   read tool changelist number
#------------------------------------------------------------------------------#

prot_parts ( "read tool changelist number" ) ;

print ( "\nInfo: using tool directory $ENV{'TOOL'}\n" ) ;

if ( open ( TOOL_CHANGELIST_LOG , "$ENV{TOOL}/data/changelist.log" ))
{
   if ( $TOOL_CHANGELIST_NO = <TOOL_CHANGELIST_LOG> )
   {  chomp ( $TOOL_CHANGELIST_NO ) ;  }
   else
   {  
      print "WARNING: can\'t read tool changelist number!\n" ;
      $TOOL_CHANGELIST_NO = 0;
   }
}
else
{
   print "WARNING: can\'t open $ENV{TOOL}/data/changelist.log!\n" ;
   $TOOL_CHANGELIST_NO = 0;
}
close ( TOOL_CHANGELIST_LOG ) ;

#------------------------------------------------------------------------------#
#   create a new qadblca entry
#------------------------------------------------------------------------------#

prot_parts ( "create new qadblca entry" ) ;

my %makeinfo = ( 'IDMAKE'     => "$ENV{LCMAKEID}" ,
				 'VERSION'    => "$ENV{APO_COM_SHORT}" ,
				 'TYPE'       => "$TYPE",
				 'QASTATUS'   => "$QASTATUS" ,
				 'CHANGELIST' => "$CHANGELIST_NO"								 
				);
				
$qahlca = qadblca -> new ( \%makeinfo);
						
if ( $qahlca -> { error_code } != 0 )
{
  $qalca_ok = 0 ;
  $qadb_error_count++;
  print STDERR "ERROR: ".$qahlca->{error_text}."\n" ;
}
else
{
	if ( $ENV{'BRANCH_INFO'} =~ /^(DEV|CONS)/i)
	{
		print ("Found additional BRANCH_INFO for QADB: $1\n");		
		if ( $qahlca -> update_columns ( {'BRANCHINFO'     => $ENV{'BRANCH_INFO'},
										  'IDOBJSTATUS' => "$WRITE_BRANCH_INFO"										
										} ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update TOOLCL: $errtxt\n" ;
		}
	}

	if ( $TOOL_CHANGELIST_NO )
	{
		if ( $qahlca -> update_columns ( {'TOOLCL'     => "$TOOL_CHANGELIST_NO",
										  'IDOBJSTATUS' => "$WRITE_TOOLCL"
										 } ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update TOOLCL: $errtxt\n" ;
		}
	}
}

#######################

if ( $qahlca -> { error_code } != 0 )
{
  $qalca_ok = 0 ;
  $qadb_error_count++;
  print STDERR "ERROR: ".$qahlca->{error_text}."\n" ;
}
else
{
		if ( $qahlca -> update_columns ( {'PATCHLEVEL'     => "$ENV{'APO_PATCH_LEVEL'}",
										  'IDOBJSTATUS' => "$WRITE_PATCHLEVEL"
										 } ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update PATCHLEVEL: $errtxt\n" ;
		}
}


#------------------------------------------------------------------------------#
#   storing PID into $OWN/prot/build.pid
#------------------------------------------------------------------------------#

prot_parts ( "write build process id to $ProtDir/build.pid" ) ;
if ( open ( PID_FILE , ">$ProtDir/build.pid" ))
{  print PID_FILE "$$\n" ;  }
else
{  	print "WARNING: can\'t open $ProtDir/build.pid!\n" ;  }
close ( PID_FILE ) ;

#------------------------------------------------------------------------------#
#   write link to DTM protocol
#------------------------------------------------------------------------------#
prot_parts ( "write link to dtm protocol" ) ;
if (defined $ENV{'DTM_TASKLOGFILE'})
{
	my $logfilepath = $ENV{'DTM_TASKLOGFILE'};
	$logfilepath =~ tr/\\/\//;
	($logfilepath) = ($logfilepath =~ /(production\/info\/logs.*)$/);
	write_log("Info: <a href=\"http://ls3105.wdf.sap.corp:1080/BAS/$logfilepath\" target=\"DTMLogFileWindow\">DTM Logfile</a>");
}
else
{  print "WARNING: DTM_TASKLOGFILE not defined\n";  }

#-----------------------------------------------------------------------------#
# precheck: check for necessity of make 
#-----------------------------------------------------------------------------#
if ( $PRECHECK )
{
	prot_parts ( "check necessity of make ($PRECHECK)") ;
	my $to_make = 0;
	my $PCReturn = 0;
	my $PCMessage = ""; 
	my $PCLog="PreCheck:\n";
	foreach my $precheck (split(/[,]/, $PRECHECK)) 
	{	
		($PCReturn,$PCMessage) = eval "LCAMakePreCheck::precheck_$precheck()";
		if ($@)
		{
			print "Error: can't call precheck function 'precheck_$precheck':\n$@\n";
			print_usage () ;
			finish_make ();
		    caller () ? return ( 1 ) : exit ( 1 ) ;
		}
		$PCLog .= ( $PCReturn < 0 ) ? " * " : (( $PCReturn == 0 ) ? " - " : " + "); 
		$PCLog .= "$PCMessage\n";
		if ( $PCReturn < 0 )
		{
			$to_make = $PCReturn;
			last;
		}
		$to_make += $PCReturn;
	}
	$PCLog =~ s/\(makeid=(\d*)\)/(<a href="LCAMake_Details.jsp?id=$1">$1<\/a>)/g;
	$PCLog =~ s/\(lcmakeid=(\d*)\)/(<a href="Make_Details.jsp?id=$1">$1<\/a>)/g;
	if ( $to_make < 1 )
	{
		write_log($PCLog);
		skip_make();
		unlink "$MakeProtFile";
		caller () ? return ( 0 ) : exit ( 0 ) ; 
	}	 
	else
	{
		write_log($PCLog);
	}
}

#------------------------------------------------------------------------------#
#   preparation of scratch make
#------------------------------------------------------------------------------#
if ( $SCRATCH )
{
	prot_parts ( "prepare scratch make" ) ;
	if($^O=~/.*win32.*/i)
	{   $ret = system ("del /S /Q usr sys\\wrk sys\\src") ;}
	else
	{   $ret = system ("rm -rf usr sys/wrk sys/src") ; }
	rmtree ("usr", 1, 1);
}

if ( $COPY_SRC )
{
	(-d "$ENV{OWN}/sys/src") &&	system("rm -r $ENV{OWN}/sys/src");
	(-d "$ENV{OWN}/sys") || mkpath ("$ENV{OWN}/sys", 0775);
	prot_parts ( "Copy src directory $ENV{APO_SRC}/sys/src to $ENV{OWN}/sys") ;
	my $rc = system("cp -r $ENV{APO_SRC}/sys/src $ENV{OWN}/sys"); 
	print ("Copying ". ($rc ? "finished successfully" : "failed"). "\n");
}


#------------------------------------------------------------------------------#
#   storing MAKE ID into $OWN/make.id
#------------------------------------------------------------------------------#

prot_parts ( "store make id" ) ;
$qalca_id = $qahlca -> {'ID'} ;
if ( $qahlca -> { error_code } != 0 )
{
	$qalca_ok = 0 ;
	print STDERR "ERROR: ".$qahlca->{error_text}."\n" ;
}
else
{
	if ( open ( MAKEID_FILE , ">$ENV{OWN}/make.id" ))
	{  print MAKEID_FILE $qalca_id ;  }
	else
	{  	print "WARNING: can\'t open $ENV{OWN}/make.id!\n" ;  }
	close ( MAKEID_FILE ) ;
}

#------------------------------------------------------------------------------#
#   call dsp2vmake
#------------------------------------------------------------------------------#

prot_parts ( "create vmake descriptions" ) ;

update_idobjstat ( $START_DSP2VMAKE ) ;

if (-f "$ENV{APO_SRC}/sys/src/descriptions.mac" )
{
	$found_dsp2vmake_description = 1;
	$ret = lcmake ("lcmake.pl", "-i", "descriptions.mac"); 
	push @BUILTTARGETS, "descriptions.mac";
}
else
{
	if($^O=~/.*win32.*/i)
	{	$ret = system("python $ENV{TOOL}\\bin\\dsp2vmake.py -inDEV $ENV{APO_SRC}\\sys\\src"); }
	else
	{	$ret = system("python $ENV{TOOL}/bin/dsp2vmake.py -inDEV $ENV{APO_SRC}/sys/src"); }
}

if ($ret != 0)
{
	update_idobjstat ( $ERROR_DSP2VMAKE ) ;
#	if ($found_dsp2vmake_description)
#	{	checkmake::check_lca_make("descriptions.mac");}
#	else
#	{	checkmake::lca_make_message("Error while creating vmake descriptions!\n"); 	}
}
else
{
	update_idobjstat ( $SUCCESS_DSP2VMAKE ) ;
	
	#------------------------------------------------------------------------------#
	# make and build
	#------------------------------------------------------------------------------#
	prot_parts ( "make and build ..." ) ;
	update_idobjstat ( $START_MAKE_AND_BUILD );
	
    if (open (MAKE_OUT, "$MAKE_CMD |")) {
    	# insert_percent();
        while (<MAKE_OUT>) {
            if (/VMAKE\sREPORT:\s+(\d+) %\s+targets finished/) {
            	#update_percent ($1);
            } 
            print "$_";
        }
        close(MAKE_OUT);
        $ret = $? >> 8;
    }
    else
    {
    	print ("Error while open pipe for '$MAKE_CMD'\n");
    	$ret = 1;
    }    
        
	update_idobjstat ( $ret==0 ? $SUCCESS_MAKE_AND_BUILD : $ERROR_MAKE_AND_BUILD  ) ;
	push @BUILTTARGETS, "lcapps_factory.mac"; 
	
	if ($ret != 0)
	{	
		print "make failed\n";
#		checkmake::check_lca_make("lcapps_factory.mac"); 
	}
	else
	{
		#------------------------------------------------------------------------------#
		# write lcpool id to qadblca
		#------------------------------------------------------------------------------#
		if($^O=~/.*win32.*/i)
		{
			require WinLink;
			import WinLink;
			$LCPOOL_ID = readLink ("$APO_POOL_DIR/LastBuild");
		}
		else
		{	$LCPOOL_ID = readlink "$APO_POOL_DIR/LastBuild"; }
	}
}
		
if ( $qalca_ok )
{
	my $LC_POOL_PATH="$APO_POOL_DIR/$LCPOOL_ID";
	if ( $qahlca -> update_columns ( {'LCPOOLID' => "$LCPOOL_ID",
									  'IDOBJSTATUS' => "$WRITE_LCPOOL_ID"
									 } ) != 0 )
	{
		my $errtxt = $qahlca -> { error_text } ;
		print STDERR "ERROR: can\'t update LCPOOLID: $errtxt\n" ;
	}
	if ( $ret == 0 )
	{
		copy ( "$OWN/make.id", "$LC_POOL_PATH/make.id" ) 
			unless ( -f "$LC_POOL_PATH/make.id" );
		
		if($^O=~/.*win32.*/i)
		{
			$LC_POOL_PATH =~ tr/\//\\\\/;
		}
		write_log ("LC_POOL: $LC_POOL_PATH");
		my $packdistrib_options = lcapoolpath2packdistribopts($LC_POOL_PATH);
		write_log ("Package command:\n packdistrib $packdistrib_options\n");
	}
}
else
{	print "WARNING: don't update LCPOOLID because of an open error\n" ;	}

prot_parts ( "end make" ) ;

#------------------------------------------------------------------------------#
#   send mail
#------------------------------------------------------------------------------#

prot_parts ( "send mail" ) ;
my $return ;
my @arg_checkmake = ( "$ENV{TOOLSHELL}", "$ENV{'TOOL'}/bin/checklcmake.pl" ) ;
push @arg_checkmake, "-makeid=$qalca_id" if ( $qalca_ok );
push @arg_checkmake , @BUILTTARGETS ;

$return = system ( @arg_checkmake ) ;

if (( !defined $return ) and ( $return != 0 ))
{
  print STDERR "ERROR: file \"checkmake.pl\" failed\n";
}

unlink "$MakeProtFile" unless ($qadb_error_count);



#------------------------------------------------------------------------------#
#   only protocol informations
#------------------------------------------------------------------------------#

print "\n\nwrite protocoll file \"$MakeProtFile\" into qadb\n" ;

if ( $ret == 0 )
{
   print "\nEXIT: Congratulations!!! No make errors occured!\n" ;
   update_idobjstat ( $SUCCESS_ALL ) ;

}
else
{
   update_idobjstat ( $ERROR_ALL ) ;
   print "\nEXIT: ATTENTION---ERRORS---ATTENTION---ERRORS---ATTENTION\n" ;
   print "EXIT: see above\n" ;
}

finish_make ();


#------------------------------------------------------------------------------#
#   END, return MAIN exit code
#------------------------------------------------------------------------------#

caller () ? return ( $ret ) : exit ( $ret ) ;

################################################################################
################## subroutines #################################################
################################################################################

sub print_usage
{
   print "usage: $0\n\n";
   print "       [-force_rm|-scratch]\n" ;
   print "          initialise a scratch make\n" ;
   print "       [-type] dbg|opt\n";
   print "          maketype\n";
   print "       [-help | -h]\n" ;
   print "          show this help message\n" ;
   print "       [-precheck] <precheck profile>\n";
   print "          precheck for necessity of make\n\n";
   LCAMakePreCheck::Usage();  
}

################################################################################
# - read the make protocol
# - save protocol in $prot_text
# - compress it if > 8 K
# - wrie to qadb
################################################################################
sub finish_make
{
	my $zip_mode = shift;
	my $protname = $MakeProtFile;
		
	$prot_text = "";
	
	print "OUTPUT_FILTER: CLOSE FILE\n";	
	sleep (2);
	
	# read protfile
	unless (open (PROT_FILE, "<$MakeProt"))
    {
        print STDERR "ERROR: Can't open file '$MakeProt'!\n" ;
        return -1;
    }

    while (<PROT_FILE>)
    {
        chomp $_;
        $prot_text .= "$_\n";
    }

    close (PROT) or print STDERR "ERROR: Can't close file '$MakeProt'!\n$!\n";

	if (length ($prot_text) > 8000)
	{
		eval "require SDB::Protocol::Tools";
		if ($@)
		{	print STDERR "Warning: Error while loading SDB::Protocol::Tools\n($@)\nCompressing file failed - using uncompressed file\n" ;	}
		else
		{
			import SDB::Protocol::Tools;
			my $ref_to_zip;
			my $zip_mode = 0;
			   		
			eval '($ref_to_zip, $zip_mode) = SDB::Protocol::Tools::PackScalar(\$prot_text, $MakeProtFile , {\'toDOS\' => 1});';
			if ($@) 
			{	print STDERR "Warning: Error while compress protocol\n($@)\nUsing uncompressed file\n" ;	}
			else
			{	$zip_mode and $protname .= ".zip"; }
		}
	}
	if ( $qalca_ok )
	{
		my $prot_link = $qahlca->write_prot ( "$protname" , $prot_text, "make protocol");
	    if ($qahlca->{'error_code'} != 0)
	    {
			print STDERR "ERROR: can\'t write prot to DB: $qahlca->{error_text}\n" ;
			$qadb_error_count++;
	    }
	}
	else
	{
		print "WARNING: don't write prot because of an open error\n" ;
	}
	
	return (0);
	 	
}

################################################################################

sub prot_parts
{
   my $prot_part = shift ;

   print "\n\n===========================================================\n" ;
   print "=   $prot_part\n" ;
   print "===========================================================\n\n" ;
}


################################################################################

sub update_idobjstat
{
	$idobjstat = shift ;
	
	if ( $qalca_ok )
	{
		if ( $qahlca -> update_columns ( {'IDOBJSTATUS' => "$idobjstat"} ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update IDOBJSTATUS: $errtxt\n" ;
		}
	}
	else
	{
		print "WARNING: don't update IDOBJSTATUS because of an open error\n" ;
	}
}

################################################################################

sub update_idobjstat
{
	$idobjstat = shift ;
	
	if ( $qalca_ok )
	{
		if ( $qahlca -> update_columns ( {'IDOBJSTATUS' => "$idobjstat"} ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update IDOBJSTATUS: $errtxt\n" ;
		}
	}
	else
	{
		print "WARNING: don't update IDOBJSTATUS because of an open error\n" ;
	}
}

##################################################################################

sub skip_make
{
	write_log("Make skipped");
	print "\nMake skipped \n\n";	
	if ( $qalca_ok )
	{
		if ( $qahlca -> update_columns ( {'LCPOOLID' => "skip",
										  'IDOBJSTATUS' => "$MAKE_SKIPPED"
										 } ) != 0 )
		{
			my $errtxt = $qahlca -> { error_text } ;
			print STDERR "ERROR: can\'t update LCPOOLID: $errtxt\n" ;
		}
	}
	
	finish_make ();
	#$zip_mode = pack_prot_text(\$prot_text);
	
}

###################################################################################

sub write_log
{
	my $logtext = shift;
	
	if ( $qalca_ok )
	{
		$qahlca->write_log($logtext);
		if ($qahlca->{'error_code'} != 0)
	    {
			print STDERR "ERROR: can\'t write prot to DB: $qahlca->{error_text}\n" ;
			$qadb_error_count++;
	    }
	}	
}

##################################################################################

sub lcapoolpath2packdistribopts{
	my ($path) = @_;
	$path =~ s/\\/\//g;	
	my ($version,$state,$buildselect,$lca_version,$lca_variant,$lca_state,$lca_buildselect)
		= ($path =~ /LC_(\d{4,6})_[2346]{2}_([A-Z]+)\/(\w+)\/LCAPPS_(\d+)_(\w+)_(\w+)\/(\w+)/);
	
	my $opt_state = " -state $state";
	
	if(length($version) >4 && $state eq 'RAMP' ){
		$opt_state = '';
	}
	
	require BuildPackage::SysName;
	import BuildPackage::SysName;
	my $sys = getsysinfo();
	my $opt_os = "";
	
	if(defined $sys)
	{	$opt_os .= "-os $sys->{'bp_name'} ";	}
	else
	{	write_log ("Warning: Can't determine platform tag for packdistrib command\n");	}

	
	my $command_line = "${opt_os}-profile \"APO LiveCache\" ".
			       "-version $version$opt_state -buildselect $buildselect ".
			       "-lca_version $lca_version -lca_variant $lca_variant ".
			      "-lca_state $lca_state -lca_buildselect $lca_buildselect";

	
	return $command_line;
}
