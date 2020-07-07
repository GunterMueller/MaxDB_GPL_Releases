#!/usr/bin/perl
# Buildinfo.pm

use strict;
package Buildinfo;

my %PERSINFO;
my $persinfo_file="$ENV{'INSTROOT'}/etc/PERSINFO";
my $versions_file="$ENV{'OWN'}/sys/wrk/obj/config/Buildinfo/VERSIONS";
my $msgversion_file=$ENV{'OWN'}.'/usr/msg/version.txt';
my %SUBSTITUTE;
my $linecount = 0;

sub load_persinfo
{
	open(VERSIONFILE, "<$persinfo_file") 
		or die ("can't open VERSIONS file $persinfo_file for reading\n");
	while ( <VERSIONFILE>) 
	{
		next if (/^\s*#/); # found comment
		if (/^\s*(\S*)\s*=\s*(\S*)\s*/)
		{
			$PERSINFO{$1}=$2;
		}
		else
		{
			die "wrong syntax in $persinfo_file\n   in '$_'";
		}
	}
	close (VERSIONFILE);
}

sub get_persinfo 
{
	my $perstype = shift;
	unless ( defined %PERSINFO )
	{
		load_persinfo();
	}
	if ( defined $perstype)
	{
		defined $PERSINFO{$perstype} || die "Can't find persistent type '$perstype' in $persinfo_file";
		return ($PERSINFO{$perstype});
	}
	else
	{
		return (%PERSINFO);
	}
}

sub get_msgversion{
	my $msg_version;
	open(MSGVER, "<$msgversion_file") 
		or die ("can't open message version file $msgversion_file for reading\n");
	while(<MSGVER>){
		chomp;
		next unless /\S/;
		$msg_version = $_;
	}	
	close(MSGVER);
	return $msg_version;
}

sub setbuildinfo
{
	my $FILE = shift;
	my $additionalValues = shift; # hash of values
	my $TEMP_FILE = "$FILE".".convtmp";
	my @my_localtime = localtime();
	my $make_year    = $my_localtime[5]+1900;
	my $make_month   = $my_localtime[4]+1;
	$make_month      = "0$make_month" if ( $make_month < 10 );
	my $make_day     = $my_localtime[3];
	$make_day        = "0$make_day" if ( $make_day < 10 );
	%SUBSTITUTE = ();
	$linecount = 0;
		
	if ( ! defined $FILE || ( $FILE =~ /-(h|\?)/i ))
	{

		die "USAGE: Buildinfo::setbuildinfo <file>\n".
      "   replace in file following strings in <...>\n".
      "      SAPDB_PURPOSE               - LC|OLTP|ALL\n".
      "      RELEASE                     - Release info : 7.4\n".
      "      RELVER                      - R74\n".
      "      MAJOR_VERSION               - 7\n".
      "      MINOR_VERSION               - 4\n".
      "      CORRECTION_LEVEL            - unfomated (one digit if < 10)\n".
      "      FORMATED_CORRECTION_LEVEL   - formated with 2 digits\n".
      "      BUILD_PREFIX                - unfomated (one digit if < 10)\n".
      "      FORMATED_BUILD_PREFIX       - formated with 2 digits\n". 
      "      FORMATED3_BUILD_PREFIX      - formated with 3 digits\n". 
      "      FORMATED_BUILD_VERSION      - 7.4.03.09\n".
      "      LC_STATE                    - DEV|COR|RAMP\n".
      "      BUILD_NUMBER_STRING         - 123-0123-456\n".
      "      BUILD_NUMBER_EX_STRING      - extended buildnumber (009-123-0123-456)\n".
      "      RELSTAT                     - DEV|TEST|PROD\n".
      "      TOOL                        - Tool directory\n".
      "      OWN                         - OWN directory\n".
      "      VMAKE_PATH                  - VMAKE_PATH\n".
      "      MAKE_DATE                   - date of make\n".
      "      MAKE_DAY                    - day of make\n".
      "      MAKE_MONTH                  - month of make\n".
      "      MAKE_YEAR                   - year of make\n".
      "      PATCH_DATE                  - date of set of patchlevel \n".
      "                                    (makedate if not defined)\n".
      "      PATCH_DAY                   - day of set of patchlevel \n".
      "      PATCH_MONH                  - month of set of patchlevel \n".
      "      PATCH_YEAR                  - year of set of patchlevel \n".
      "      COMPANY_NAME                - SAP AG\n".
      "      PRODUCT_NAME                - SAP DB\n".
      "      COPYRIGHT_STRING            - Copyright 2000-2003 by SAP AG\n".
      "      COPYRIGHT_RESOURCE_STRING   - Copyright \\251 2000-2003 by SAP AG\n".
      "      COPYRIGHT_COMPANY_NAME      - SAP AG\n".
      "      COPYRIGHT_DATE              - 2000-2003\n".
      "      CHANGELIST                  - changelist\n".
      "      MAKEID                      - ID of the make\n".
      "      MSG_VERSION                 - latest version number of message files\n".
      "      PERSINFO:<perstype>         - buildinfo (buildprefix) of <perstype>\n".
      "                                    perstypes are defined \n".
      "                                    in :config/Persinfo/VERSIONS\n"; 
	}
	
	if ($ENV{'MAKE_DATE'})
	{
	  $SUBSTITUTE{'MAKE_DATE'} =  $ENV{'MAKE_DATE'};
	}
	else
	{
	  $SUBSTITUTE{'MAKE_DATE'}  = "$make_year-$make_month-$make_day";
	}
	
    $SUBSTITUTE{'PATCH_DATE'} = $ENV{'PATCH_DATE'} ? $ENV{'PATCH_DATE'} : $SUBSTITUTE{'MAKE_DATE'};
    
    if ($SUBSTITUTE{'PATCH_DATE'} =~ /^(\d\d\d\d)-(\d\d)-(\d\d)$/)
    {
    	$SUBSTITUTE{'PATCH_YEAR'}= "$1";
    	$SUBSTITUTE{'PATCH_MONTH'}= "$2";
    	$SUBSTITUTE{'PATCH_DAY'}= "$3";
    }
    
    if ($SUBSTITUTE{'MAKE_DATE'} =~ /^(\d\d\d\d)-(\d\d)-(\d\d)$/)
    {
    	$SUBSTITUTE{'MAKE_YEAR'}= "$1";
    	$SUBSTITUTE{'MAKE_MONTH'}= "$2";
    	$SUBSTITUTE{'MAKE_DAY'}= "$3";
    }
    
	
	$SUBSTITUTE{'SAPDB_PURPOSE'} = $ENV{'SAPDB_PURPOSE'} ? $ENV{'SAPDB_PURPOSE'} : "ALL";
	
	if ($ENV{'RELVER'} =~ /^R(\d)(\d)$/)
	{   
		$SUBSTITUTE{'MAJOR_VERSION'} = "$1" ;
		$SUBSTITUTE{'MINOR_VERSION'} = "$2" ;
		$SUBSTITUTE{'RELEASE'}       = "$1.$2" ;
	
	
		
	
		$SUBSTITUTE{'RELVER'}  = $ENV{'RELVER'};
		$SUBSTITUTE{'CORRECTION_LEVEL'}  = $ENV{'CORRECTION_LEVEL'} ? $ENV{'CORRECTION_LEVEL'} : "0";
		$SUBSTITUTE{'CORRECTION_LEVEL'} = $1 if ($SUBSTITUTE{'CORRECTION_LEVEL'} =~ /^0(\d)$/);
		
		$SUBSTITUTE{'FORMATED_CORRECTION_LEVEL'} = ( $SUBSTITUTE{'CORRECTION_LEVEL'} =~ /^\d\d$/ ) ?
		                                            $SUBSTITUTE{'CORRECTION_LEVEL'} : "0"."$SUBSTITUTE{'CORRECTION_LEVEL'}";
		
		$SUBSTITUTE{'BUILD_PREFIX'}  = $ENV{'BUILDPRAEFIX'} ? $ENV{'BUILDPRAEFIX'} : "0";
		$SUBSTITUTE{'BUILD_PREFIX'} = $1 if ($SUBSTITUTE{'BUILD_PREFIX'} =~ /^0(\d)$/);
		
		$SUBSTITUTE{'FORMATED_BUILD_PREFIX'} = ( $SUBSTITUTE{'BUILD_PREFIX'} =~ /^\d\d$/ ) ?
		                                        $SUBSTITUTE{'BUILD_PREFIX'} : "0"."$SUBSTITUTE{'BUILD_PREFIX'}";
		
		$SUBSTITUTE{'FORMATED3_BUILD_PREFIX'} = "0".$SUBSTITUTE{'FORMATED_BUILD_PREFIX'};
		
		$SUBSTITUTE{'FORMATED_BUILD_VERSION'} = join ".", ("$SUBSTITUTE{'MAJOR_VERSION'}","$SUBSTITUTE{'MINOR_VERSION'}",
		                                                   $SUBSTITUTE{'FORMATED_CORRECTION_LEVEL'}, $SUBSTITUTE{'FORMATED_BUILD_PREFIX'});
		                                                   
		check_VERSIONS_info();	                                                   
		
	}
	elsif ($ENV{'RELVER'} !~ /^RTOOL$/)
	{	die "wrong environment variable RELVER\n";}
	

	                                                   
	$SUBSTITUTE{'LC_STATE'}  = $ENV{'LC_STATE'} ? $ENV{'LC_STATE'} : "DEV";
	
	$SUBSTITUTE{'VMAKE_PATH'}  = $ENV{'VMAKE_PATH'} ? $ENV{'VMAKE_PATH'} : $ENV{'OWN'};
	
	$SUBSTITUTE{'PRODUCT_NAME'}  = $ENV{'PRODUCT_NAME'} ? $ENV{'PRODUCT_NAME'} : "SAP DB";
	
	$SUBSTITUTE{'COMPANY_NAME'}  = $ENV{'COMPANY_NAME'} ? $ENV{'COMPANY_NAME'} : "SAP AG";
	
	$SUBSTITUTE{'COPYRIGHT_COMPANY_NAME'}  = $ENV{'COPYRIGHT_COMPANY_NAME'} ? $ENV{'COPYRIGHT_COMPANY_NAME'} : $SUBSTITUTE{'COMPANY_NAME'};
	
	$SUBSTITUTE{'COPYRIGHT_DATE'}  = $ENV{'COPYRIGHT_DATE'} ? $ENV{'COPYRIGHT_DATE'} : "2000-$make_year";
	
	if ($ENV{'COPYRIGHT_STRING'})
	{
	  $SUBSTITUTE{'COPYRIGHT_STRING'} = $ENV{'COPYRIGHT_STRING'};
	}
	else
	{ 
	  $SUBSTITUTE{'COPYRIGHT_STRING'} = "Copyright $SUBSTITUTE{'COPYRIGHT_DATE'} by $SUBSTITUTE{'COPYRIGHT_COMPANY_NAME'}";
	}	
	
	if ($ENV{'COPYRIGHT_RESOURCE_STRING'})
	{
		$SUBSTITUTE{'COPYRIGHT_RESOURCE_STRING'} = $ENV{'COPYRIGHT_RESOURCE_STRING'};
	}
	else
	{
		$SUBSTITUTE{'COPYRIGHT_RESOURCE_STRING'} = $SUBSTITUTE{'COPYRIGHT_STRING'};
		$SUBSTITUTE{'COPYRIGHT_RESOURCE_STRING'} =~ s/^Copyright (.*)$/Copyright \\251 $1/;
	}
		
	
	if ( $ENV{'BUILDSTRING'} )
	{
		$SUBSTITUTE{'BUILD_NUMBER_STRING'}  = $ENV{'BUILDSTRING'};	
	}
	else
	{
		$SUBSTITUTE{'BUILD_NUMBER_STRING'}= "$SUBSTITUTE{'FORMATED3_BUILD_PREFIX'}-000-000-000";
		warn "Missing environment variable BUILDSTRING. Set it to $SUBSTITUTE{'BUILD_NUMBER_STRING'}\n";
	}
	
	$SUBSTITUTE{'BUILD_NUMBER_EX_STRING'} = "$SUBSTITUTE{'FORMATED3_BUILD_PREFIX'}-$SUBSTITUTE{'BUILD_NUMBER_STRING'}";
	
	if ( $ENV{'LATEST_CHANGE_LIST'} )
	{
		$SUBSTITUTE{'CHANGELIST'}  = $ENV{'LATEST_CHANGE_LIST'};	
	}
	else
	{
		$SUBSTITUTE{'CHANGELIST'}= "00000";
		warn "Missing environment variable LATEST_CHANGE_LIST. Set it to 00000\n";
	}
	if ( defined $ENV{'TOOL'} )
	{	$SUBSTITUTE{'TOOL'}  = $ENV{'TOOL'};}
	else
	{	die "Missing environtment variable TOOL! \n";	}
	
	if ( defined $ENV{'RELSTAT'} )
	{	$SUBSTITUTE{'RELSTAT'}  = $ENV{'RELSTAT'};	}
	else
	{	$SUBSTITUTE{'RELSTAT'}  = $ENV{'TEST'};	}	
	
	$SUBSTITUTE{'OWN'} = $ENV{'OWN'};
	
	if ( defined $ENV{'MAKEID'} )
	{	$SUBSTITUTE{'MAKEID'} = $ENV{'MAKEID'};	 }
	else
	{	$SUBSTITUTE{'MAKEID'} = "0000"; 	}

	open(FILE_IN, "<$FILE") || die "Can't open $FILE (input): $!";
	open(FILE_OUT, ">$TEMP_FILE") || die "Can't open $TEMP_FILE (output): $!";
	
	my $search_string = undef;
	
	foreach (keys %SUBSTITUTE)
	{
		if (defined $search_string)
		{ $search_string .= "|$_" }
		else
		{ $search_string = "$_" }
	}
		
	while(<FILE_IN>) 
	{
		$linecount++; 
		s/<\s*($search_string)\s*>/$SUBSTITUTE{$1}/g;
		s/<\s*SDB_SUBST:\s*($search_string)\s*>/$SUBSTITUTE{$1}/g;
		my $line = $_;
		while ($line =~/<\s*PERSINFO:\s*([^>]+)\s*>/)
		{
			my $Persinfo=$1;
			my $Perstype = $1;
			my $Perspart = undef;
			my $Perssubst = undef;
			if ($Persinfo =~ /^(\S+)\s*:\s*(\S+)\s*$/)
			{
				$Perstype = $1;
				$Perspart = $2;	
			}
			$Perssubst = get_persinfo($Perstype);
			if (defined $Perspart)
			{
				if ( $Perssubst =~ /(\d)\.(\d)\.(\d)(\d)\.(\d)(\d)/)
				{
					my %Persparts;
					$Persparts{'MAJOR_VERSION'} = $1;
					$Persparts{'MINOR_VERSION'} = $2;
					$Persparts{'CORRECTION_LEVEL'} = (($3 > 0) ? "$3" : "")."$4";
					$Persparts{'BUILD_PREFIX'} =  (($5 > 0) ? "$5" : "")."$6";
					$Persparts{'COMMA_FORMATED'} = "$1, $2, $Persparts{'CORRECTION_LEVEL'}, $Persparts{'BUILD_PREFIX'}";
					if (defined $Persparts{$Perspart})
					{
						$Perssubst = $Persparts{$Perspart};
					}
					else
					{
						die ("Error: $FILE($linecount) Persitence info $Persinfo not defined\n");
					}
				}
			}
			$line =~ s/<\s*PERSINFO:\s*$Persinfo\s*>/$Perssubst/;
		}
		while ($line =~/<\s*SDB_SUBST_ENV:\s*([^>]+)\s*>/)
		{
			my $SubstString = $1;
			my $Envinfo = $SubstString;
			my $EnvString = undef;
			my $option = undef;
			if ($SubstString =~ /^\s*([^\(]*)\s*\((\S*)\)/)
			{
				$Envinfo = $1;
				$option = $2;
				$SubstString =~ s/([\(\)])/\\$1/g;
			}
			if (defined $ENV{$Envinfo})
			{
				$EnvString = $ENV{$Envinfo};
				
				if ($option =~ /^c?path$/i)
				{
					if ($^O =~ /mswin/i)
					{ 
						$EnvString =~ s/\//\\/g;
						if ($option =~ /^cpath$/i)
						{
							$EnvString =~ s/([^\\])\\([^\\])/$1\\\\$2/g;
						}						
					}
				}
				elsif ($option =~ /^unixpath$/i)
				{
					$EnvString =~ s/\\/\//g;
				}
				elsif (defined $option)
				{
					die ("Error: $FILE(line $linecount) unknown environment option ($option)\n       only 'path' and 'cpath' suported\n");
				}
				$line =~ s/<\s*SDB_SUBST_ENV:\s*$SubstString\s*>/$EnvString/eg;
			}
			else
			{
				die ("Error: $FILE(line $linecount) environment variable '$Envinfo'\n");
			}
		}
		while($line =~/<\s*SDB_SUBST:\s*MSG_VERSION\s*>/){
			my $msg_version = get_msgversion();
			if(defined $msg_version){
				$line =~ s/<\s*SDB_SUBST:\s*MSG_VERSION\s*>/$msg_version/;
			}
		}		
		print FILE_OUT $line;
	}
	
	close(FILE_OUT); 
	close(FILE_IN);
	
	unlink ("$FILE");
	rename ("$TEMP_FILE", "$FILE");
}

#
sub check_VERSIONS_info
{
	my $vers_file = "$ENV{'OWN'}/sys/wrk/obj/config/Buildinfo/VERSIONS";
	my ($relver,$correction_level,$buildprefix,$purpose);
	if ( open(VERSIONFILE, "<$vers_file") )
	{
		while ( <VERSIONFILE>) 
		{ last if ( /\[kernel\]/);}
		while ( <VERSIONFILE> ) 
		{
			last if ( /^\s*\[/) ; # read untill the next entry (precompiler) 
			if (/^\s*version=(\d)\.(\d).(\d\d).(\d\d)\s*$/)
			{
				$relver= "R$1$2";
				$correction_level = "$3";
				$buildprefix = "$4";
				$correction_level = "0$1" if ( $correction_level =~ /^(\d)$/ );
				$buildprefix = "0$1" if ( $buildprefix =~ /^(\d)$/ );
			}
			elsif (/^\s*purpose=(\S*)\s*$/)
			{
				$purpose = "$1";    
			}
		}
		close (VERSIONFILE);
	
		my @err_messages=();
		
		push @err_messages, "RELVER between environment ($ENV{'RELVER'}) and versions file ($relver)" 
			if ($relver ne $ENV{'RELVER'}) ;
		push @err_messages, "CORRECTION_LEVEL between environment ($SUBSTITUTE{'FORMATED_CORRECTION_LEVEL'}) and versions file ($correction_level)" 
			if ($correction_level ne $SUBSTITUTE{'FORMATED_CORRECTION_LEVEL'}) ;
		push @err_messages, "BUILDPRAEFIX between environment ($SUBSTITUTE{'FORMATED_BUILD_PREFIX'}) and versions file ($buildprefix)" 
			if ($buildprefix ne $SUBSTITUTE{'FORMATED_BUILD_PREFIX'}) ;
		if ($#err_messages > -1)
		{
			print ("Error: following differences between environment and version file found\n");
			foreach (@err_messages)
			{
				print "         $_\n";
			}
			if ($^O=~/.*win32.*/i)
			{
				print "-> execute setlcenv.bat to update environment\n\n";
			}
			else
			{
				print "-> execute \". setlcenv\" to update environment\n\n";
			}
			die;
		}
	}
}	

#############################################################
#
#  get_buildinfo_from_gsp100 
# 
# - read build infos from $WRK/incl/gsp100.h
#
# ----------------------------------------------------------
#
# return:
#   if wantarray:
#     - list of values of the keys given by the parameters
#   else
#     - hash of all build infos (all keys in upper case)
#
#############################################################
sub get_buildinfo_from_gsp100
{
	my %gsp100_info;
	my @keys = @_;
	
	$gsp100_info{'PURPOSE'} = "ALL";
	
	my $gsp100_file = "$ENV{'OWN'}/sys/wrk/incl/gsp100.h";
	
	unless ( -f "$gsp100_file" )
	{
		print "Error: can't find gsp100.h  ($gsp100_file)\n";
		return undef;
	}
		
	unless (open(VERSIONFILE, "<$gsp100_file"))
	{
		print "Error: can't open gsp100.h for reading ($gsp100_file)\n";
		return undef;
	}
	while ( <VERSIONFILE>) 
	{
		if ( /^\s*#\s*define\s*MAJOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
		{	$gsp100_info{'MAJORVERSION'}=$1;	}
		elsif ( /^\s*#\s*define\s*MINOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
		{   $gsp100_info{'MINORVERSION'}=$1;	}
		elsif ( /^\s*#\s*define\s*CORRECTION_LEVEL_STR_SP100\s*"(.*)"/ )
		{	
			$gsp100_info{'CORRECTIONLEVEL'}=$1;		
			if ($gsp100_info{'CORRECTIONLEVEL'} =~ /^\d$/)
			{	$gsp100_info{'FORMATEDCORRECTIONLEVEL'}	= "0$gsp100_info{'CORRECTIONLEVEL'}";	}
			else
			{	$gsp100_info{'FORMATEDCORRECTIONLEVEL'}	= $gsp100_info{'CORRECTIONLEVEL'};	}
		}
		elsif ( /^\s*#\s*define\s*BUILD_PREFIX_STR_SP100\s*"(.*)"/ )
		{	
			$gsp100_info{'BUILDPREFIX'}=$1;	
			if ($gsp100_info{'BUILDPREFIX'} =~ /^0*(\d+)$/)
			{
				$gsp100_info{'BUILDPREFIX'} = $1;
			}
			if ($gsp100_info{'BUILDPREFIX'} =~ /^\d$/)
			{	$gsp100_info{'FORMATEDBUILDPREFIX'}	= "0$gsp100_info{'BUILDPREFIX'}";	}
			else
			{	$gsp100_info{'FORMATEDBUILDPREFIX'}	= $gsp100_info{'BUILDPREFIX'};	}
		}
		elsif ( /^\s*#\s*define\s*DATE_SP100\s*"Date\s*(\S*)"/ )
		{	$gsp100_info{'PATCHDATE'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*PROD_NAME_SP100\s*"(.*)"/ )
		{	$gsp100_info{'PRODUCTNAME'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*COMPANY_NAME_SP100\s*"(.*)"/ )
		{	$gsp100_info{'COMPANYNAME'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*COPYRIGHT_SP100\s*"(.*)"/ )
		{	$gsp100_info{'COPYRIGHTSTRING'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*SAPDB_PURPOSE_SP100\s*"(.*)"/ )
		{	$gsp100_info{'PURPOSE'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*SAPDB_BUILD_STATE_SP100\s*"(.*)"/ )
		{	$gsp100_info{'LC_STATE'} = "$1";  } 
		elsif ( /^\s*#\s*define\s*SAPDB_RELSTAT_SP100\s*"(.*)"/ )
		{	$gsp100_info{'RELSTAT'} = "$1";  } 
	}
	close(VERSIONFILE);
	
	if (wantarray)
	{
		my @infoarray=();
		foreach (@keys)
		{
			tr/a-z/A-Z/;
			if (defined $gsp100_info{$_})
			{
				push @infoarray, $gsp100_info{$_};
			}
			else
			{
				print ("Warning: can't get info for $_ in  gsp100.h\n");
				push @infoarray, undef;
			}
		}
		return @infoarray;
	}
	else
	{
		return \%gsp100_info;
	}
}

1;
	

 
