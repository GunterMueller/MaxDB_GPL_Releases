#!/devtool/local/bin/perl
# createprofile.pl

use strict;
use Getopt::Long;
use Cwd;

my ($relver,$correction_level,$buildprefix,$purpose);
my $vers_target = ":config/Buildinfo/VERSIONS";
my $gsp100_target = "gsp100.h";
my $line       = undef;
my $ret        = undef
my $own        = undef;
my $release    = undef;
my $tool       = undef;
my $src        = undef;
my $vmake_path = undef;
my $lc_state   = undef;
my $prod       = undef;
my $bit64      = undef;
my $useenv     = undef;
my $HELP       = undef;
my $path_del   = undef;
my $patch_date = undef;
my $path_sep   = undef;
my $temp_lcstate = 0;
my $profile_target;


if($^O=~/.*win32.*/i)
{   
  $profile_target = ":config/profiles/iprofile.bat"; 
  $path_del =  ";";
  $path_sep = "\\";
}
else
{   
  $profile_target = ":config/profiles/.iprofile"; 
  $path_del =  ":";
  $path_sep = "/";
}


sub my_die
{
	my $message = shift;
	delete $ENV{'IGNORE_CCVERS'};
	die "$message\n";	
} 

sub get_callers_path 
{
	my $callers_path = (caller())[1];
	if ($^O=~/.*win32.*/i)
	{ $callers_path  =~ tr/\//\\/;}
	return $callers_path;
}

sub check_for_absolute_path
{
	my $ref_path = shift;
	if ($^O=~/.*win32.*/i)
	{
		($$ref_path =~ /^(\\|.*:)/) and return;
	}
	else
	{
		($$ref_path =~ /^(\/|.*:)/) and return;
	}
	$$ref_path = cwd() . "$path_sep$$ref_path";
	if ($^O=~/.*win32.*/i)
	{ $$ref_path  =~ tr/\//\\/;}
}

unless ( GetOptions ( "own=s"         => \$own,
                  "release=s"     => \$release,
                  "tool=s"        => \$tool,
                  "lc_state=s"    => \$lc_state,
                  "bit64"         => \$bit64,                  
                  "prod"          => \$prod,
                  "useenv"        => \$useenv,
                  "vmake_path=s"  => \$vmake_path,
                  "src=s"         => \$src,
                  "help"          => \$HELP,
                  "h"             => \$HELP )  and not  $HELP )
{
	print "createlcprofile.pl [-h|-?] [-own <OWN>] [-release <RELEASE>] [-tool <TOOLDIR>]\n";
	print "                   [-lc_state <LC_STATE>] [-bit64] [-prod] [-useenv]\n";
	print "                   [-src <SOURCEDIR> | -vmake_path <VMAKE_PATH>]\n"; 
	print "Where:\n";
	print "        -own <OWN_DIR>\n";
	print "             set the own directory for build\n";
	print "        -release <RELEASE>\n";
	print "             set the Release\n";
	print "        -tool <TOOLDIR>\n";
	print "             set the tool directory\n";
	print "        -lc_state <LC_STATE>\n";
	print "             set LC_STATE (DEV|COR|RAMP|HOT)\n";
	print "        -bit64\n";
	print "             set 64 bit mode\n";
	print "        -prod\n";
	print "             enable productive make\n";
	print "        -useenv\n";
	print "             use current environment (all other options will be ignored)\n";
	print "        -src <SRCDIR>\n";
	print "             set a second path behind own in vmake_path\n";
	print "        -vmake_path <VMAKE_PATH>\n";
	print "             set the VMAKE_PATH\n";
	exit ;
}

unless (defined $useenv)
{
	if (defined $tool)
	{
		check_for_absolute_path (\$tool);
		$ENV{'TOOL'} = $tool;
	}
	else
	{
		my $callers_path = get_callers_path ();
		if ($callers_path =~ /^(.*)[\\\/]bin[\\\/]createlcprofile.*/)
		{ $ENV{'TOOL'} = $1; }
	}
	
	my_die("Error: TOOL is not defined !\n") unless ( defined $ENV{'TOOL'} );
	
	$own = $ENV{'OWN'} unless (defined $own);
	check_for_absolute_path (\$own);
	my_die("Error: OWN is not defined !\n") unless (defined $own);
	
	$ENV{'OWN'} = $own;
	unshift @INC,  "$ENV{'TOOL'}/lib/Perl" if ( -d "$ENV{'TOOL'}/lib/Perl");
	unshift @INC, "$ENV{'TOOL'}/bin", "$ENV{'TOOL'}/lib/perl5";

	if ( defined $release)
	{
		if ( $release =~ /(\d)(\d)(\d*)/)
		{
			$ENV{'RELVER'} = "R$1$2";
			$ENV{'CORRECTION_LEVEL'} = "$3";	
			$ENV{'CORRECTION_LEVEL'} = "$1" if ( $ENV{'CORRECTION_LEVEL'} =~ /0(\d)/ );
		}
	}
	else
	{
		unless ( defined $ENV{'RELVER'} && defined $ENV{'CORRECTION_LEVEL'} )
		{
			# defaults
			$ENV{'RELVER'} = "R74";
			$ENV{'CORRECTION_LEVEL'} = "4";
		}
	}
	
	$ENV{'BUILDPRAEFIX'} = "0" unless ( defined $ENV{'BUILDPRAEFIX'});
	
	$ENV{'WRK'}          = "$ENV{'OWN'}/sys/wrk";
	$ENV{'INSTROOT'}     = "$ENV{'OWN'}/usr";
	$ENV{'VMAKE_OPTION'} = "aDISOhvWMLRgbX";
	if  (defined $vmake_path)
	{
		$ENV{'VMAKE_PATH'} = $vmake_path;	
	}
	else
	{
		$ENV{'VMAKE_PATH'}   = "$ENV{'OWN'}";
		if (defined $src)
		{
			check_for_absolute_path (\$src);
			$ENV{'VMAKE_PATH'}  .= ",$src"; 
			$ENV{'VMAKE_PATH'} .= "//" unless ($ENV{'VMAKE_PATH'} =~ /\/\/$/);
		}
	}
	$ENV{'TOOLVARS'}     = "$ENV{'TOOL'}/bin/toolvars.pl";
	$ENV{'TOOLSHELL'}    = "perl";
	$ENV{'TOOLEXT'}      = ".pl";
	
	if (defined $prod)
	{	$ENV{'RELSTAT'}      = "PROD"; }
	elsif ( ! defined $ENV{'RELSTAT'} )
	{ $ENV{'RELSTAT'}      = "TEST"; }
	
    $ENV{'PERL5LIB'}       = "$ENV{'TOOL'}/bin${path_del}$ENV{'TOOL'}/lib/perl5".
  								(( -d "$ENV{'TOOL'}/lib/Perl") ? "${path_del}$ENV{'TOOL'}/lib/": "").
                               ((defined $ENV{'PERL5LIB'})?"{path_del}$ENV{'PERL5LIB'}":"");

	if  (defined $lc_state)
	{	
		$lc_state =~ tr/a-z/A-Z/;
		$ENV{'LC_STATE'} = $lc_state;	
	}
	elsif ( ! defined $ENV{'LC_STATE'} )
	{	
		$ENV{'LC_STATE'} = "DEV";	
		$temp_lcstate = 1;
	}
	
	$ENV{'BIT64'} = "1" if ( defined $bit64 );
	
	# createlcprofile don't need compiler version
	# path to compiler is set later
	$ENV{'IGNORE_CCVERS'}="YES";
}

require	lcmake;
#import lcmake;
my $vers_file;

# create version file
eval
{ $ret = lcmake::lcmake ( "imf", "--force_run", "$vers_target" ) ;};
if ($@)
{  my_die("ERROR: calling imf $vers_target!\n$@\n");}
if ($ret == 0)
{ 
	$vers_file = "$ENV{'OWN'}/usr/etc/VERSIONS";
	# read version file
	my_die("missing VERSIONS file $vers_file\n") unless ( -f "$vers_file" );
	
	open(VERSIONFILE, "<$vers_file") or my_die ("can't open VERSIONS file $vers_file for reading\n");
	while ( <VERSIONFILE>) 
	{ last if ( /\[kernel\]/);}
	while ( <VERSIONFILE> ) 
	{
	  last if ( /^\s*\[/) ; # next entry (precompiler) 
	  if (/^\s*version=(\d)\.(\d).(\d\d).(\d\d)\s*$/)
	  {
	    $relver= "R$1$2";
	    $correction_level = "$3";
	    $buildprefix = "$4";
	    $correction_level = "$1" if ( $correction_level =~ /0(\d)/ );
	    $buildprefix = "$1" if ( $buildprefix =~ /0(\d)/ );
	  }
	  elsif (/^\s*purpose=(\S*)\s*$/)
	  {
	    $purpose = "$1";    
	  }
	  elsif (/^\s*date=(\S*)\s*$/)
	  {
	    $patch_date = "$1";    
	  }
	}
	while (<VERSIONFILE>) 
	{ last if ( /\[perforce\]/);}
	#<VERSIONFILE>;
	
	if (<VERSIONFILE> =~ /^\s*file=\$File:\s*(\S*)\s*\$/)
	{
		my $full_perforce_path = $1;
	  	if ($full_perforce_path =~ /\/\/sapdb\/(.*)\/sys\/src\/config\/Buildinfo\/VERSIONS/)
	  	{
	  		my $perforce_rel = $1;
	  		my $new_lcstate = ( $perforce_rel =~ /develop$/ ) ? "DEV" :
	  		                  (( $perforce_rel =~ /cor$/ ) ? "COR" : "RAMP");
	  		
		  	if ($temp_lcstate || not defined $ENV{'LC_STATE'})
		  	{
		  		$ENV{'LC_STATE'} = $new_lcstate;
		  	}
		  	elsif ($ENV{'LC_STATE'} ne "$new_lcstate")
		  	{
		  		print "Warning: different LC_STATE infos beetwen environment ($ENV{'LC_STATE'}) and VERSIONS ($new_lcstate)\n";
		  	}
		 }
	}
	close VERSIONFILE;
}
else
{
	my ($MajorVersion, $MinorVersion);
	$ret = lcmake::lcmake ( "imf", "--force_run", "-d", "$gsp100_target" ); 
	if ($@)
	{  my_die("ERROR: calling imf $vers_target!\n$@\n");}
	elsif ($ret != 0)
	{  my_die("ERROR: imf $vers_target returned with an error!\n$!\n");  }
	$vers_file = "$ENV{'OWN'}/sys/wrk/incl/gsp100.h";

	my_die("missing gsp100.h file $vers_file\n") unless ( -f "$vers_file" );
	
	open(VERSIONFILE, "<$vers_file") or my_die ("can't open VERSIONS file $vers_file for reading\n");
	while ( <VERSIONFILE>) 
	{
		if ( /^\s*#\s*define\s*MAJOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
		{	$MajorVersion=$1;	}
		if ( /^\s*#\s*define\s*MINOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
		{   $MinorVersion=$1;	}
		if ( /^\s*#\s*define\s*CORRECTION_LEVEL_STR_SP100\s*"(.*)"/ )
		{	$correction_level=$1;		}
		if ( /^\s*#\s*define\s*BUILD_PREFIX_STR_SP100\s*"(.*)"/ )
		{	$buildprefix=$1;		}
		if ( /^\s*#\s*define\s*DATE_SP100\s*"Date\s*(\S*)"/ )
		{	$patch_date = "$1";  } 
	}
	$relver= "R$MajorVersion$MinorVersion";
	$correction_level = "$1" if ( $correction_level =~ /0(\d+)$/ );
	$buildprefix = "$1" if ( $buildprefix =~ /0(\d+)$/ );
	$purpose = "ALL";
	
	$profile_target  = "$profile_target ascii nocomfile concom=yes unpack=no setbuildinfo exec CLEVEL=$correction_level ->\$OWN/ <-\$TOOL/profiles/fallback/";
	$profile_target .= "fallback/" if ( $relver < "R74" );
	if($^O=~/.*win32.*/i)
	{
		$profile_target = "\"$profile_target\"";
	}
}

my_die("wrong version info in $vers_file")  unless (defined $relver);
my_die("wrong purpose info in $vers_file")  unless (defined $purpose);

close (VERSIONFILE);

# set environment
$ENV{'RELVER'}           = $relver;
$ENV{'CORRECTION_LEVEL'} = $correction_level;
$ENV{'BUILDPRAEFIX'}     = $buildprefix;
$ENV{'SAPDB_PURPOSE'}    = $purpose;

# if date not defined use current date
unless (defined $patch_date)
{
  print ("Warning: missing date entry in VERSIONS (use current date)\n");
  my @my_localtime = localtime();
  $patch_date = ($my_localtime[5] + 1900) . "-";
  $patch_date .= "0" if ($my_localtime[4] + 1 < 10);
  $patch_date .= ($my_localtime[4] + 1 ) . "-";
  $patch_date .= "0" if ($my_localtime[3] < 10);
  $patch_date .= $my_localtime[3];
}
$ENV{'PATCH_DATE'}       = $patch_date;

# create profile (call createprofile.mac)
eval
{ $ret = lcmake::lcmake ( "imf", "-u", "--force_run", "$profile_target" ) ;};
if ($@)
{  my_die("ERROR: calling imf $profile_target!\n$@\n");}
elsif ($ret != 0)
{  my_die  ("ERROR: imf $profile_target returned with an error!\n$!\n");  }

delete $ENV{'IGNORE_CCVERS'};

0;
	

 
