#!/devtool/local/bin/perl
#
# createtoolprofile.pl Script 
# gar
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

use strict;
package build_tool;
use Getopt::Long;
use File::Copy;

my $devenv_initial;
my $devenv_root;
my $devenv_src;
my $dir_sep;
my $TOOLHOST = "pwdfm017";
my $tool_release;
my $tool_own;
my %Options;
my $rmdir_cmd;
my $perl_dir = undef;
my $vcvars = undef;
my $platform = undef;


# $TOOLHOST = "pwdfm019" if $ENV{'BIT64'};

if (!Getopt::Long::GetOptions( \%Options, 'own=s', 'rel=s', 'platform=s', 'batch', 'prod' ) || $Options{'h'} || @ARGV )
{
	print "createtoolprofile.pl [-h|-?] [-own <OWN>] [-rel <TOOLRELEASE>] [-platform <PLATFORM>] [-batch] [-prod]\n"; 
	print "\nWhere:\n";
	print "        -rel <TOOLRELEASE>\n";
	print "             sets the TOOL-Release\n";
	print "        -own <OWN_DIR>\n";
	print "             sets the own directory for TOOL build\n";
	print "        -platform <MACHINE_CONFIG>\n";
	print "             sets the MACHINE_CONFIG (for qadb handling)\n";
	print "        -prod\n";
	print "             enable productive make\n";
	print "        -batch\n";
	print "             run script in batch mode\n";
	
	
	
#	if($^O=~/.*win32.*/i)
#	{
#		print "        -vcvars\n";
#	    print "             batch file for compiler settings\n";
#		print "        -perldir\n";
#		print "             perl directory (without bin\\perl.exe)\n";
#	}
	exit;
}

# check for incompatible options
die ("Error: missing release name for make in batch mode\n")
	if ( defined $Options{'batch'} && ! defined $Options{'rel'} );




if($^O=~/.*win32.*/i)
{
	$dir_sep = "\\";
	# also UNC path possible
	$devenv_root  = "\\\\$TOOLHOST\\depot\\prodprojects\\vmake";
	$rmdir_cmd = "del /S /Q "; 
}
else
{
	$dir_sep = "/";
	$devenv_root = "/sapmnt/depot/prodprojects/vmake";
	$rmdir_cmd = "rm -rf "; 
}

# look for TOOLDIRS
opendir(DIR, "$devenv_root") || die "can't opendir $devenv_root: $!\n";
my @TOOLDIRS = grep ! /^\.\.?$/i, readdir(DIR);
@TOOLDIRS = reverse sort @TOOLDIRS;
my $count = 0;
if (defined $Options{'rel'})
{
	my $found_release = 0;
	foreach (@TOOLDIRS)
	{
		if ( $_ =~ /^$Options{'rel'}$/i)
		{
			$found_release=1;
			last;
		}
		$count++;
	}
	die "Error: can't find specified release ($Options{'rel'})\n" unless ($found_release);
}
else
{
	print "\nAvailable TOOL releases:\n\n";
	foreach (@TOOLDIRS)
	{
		$count++;
		my $outstr = sprintf("[%2u] = %s\n", $count, $_);
		print "$outstr";
	}
	
	print "\nPlease enter the TOOL release [1 - " . ($count ). "] : ";
	my $erg = <STDIN>;
	chomp $erg;
	while (($erg <= 0) || ($erg > $count))
	{
	    print "Input incorrect. Please use numbers between 1 - " . ($count) . ".\n";
	    print "\nPlease enter the TOOL release [1 - " . ($count). "] : ";
	    $erg = <STDIN>;
	    chomp $erg;
	}
	$count = $erg - 1;
}

$tool_release = $TOOLDIRS[$count];

$devenv_src = "${devenv_root}${dir_sep}${tool_release}${dir_sep}src${dir_sep}_tools${dir_sep}maketools";


die "Error: Can't find initial tool build in $devenv_src\n" unless ( -d $devenv_src );
	
if (defined $Options{'own'})
{
	$tool_own = $Options{'own'};
}
else
{
	my $default_tool_own = undef;
	
	if($^O=~/.*win32.*/i)
	{
		if ($ENV{'INDEP_TOOL'})
		{
			$default_tool_own = "$ENV{'INDEP_TOOL'}\\$tool_release";
		}
		elsif ($ENV{'TOOL'})
		{
			$default_tool_own = "$ENV{'TOOL'}\\$tool_release";
		}
	}
	else
	{
		$default_tool_own = "/devtool/_tools/$tool_release";
	}
	
	if (defined $Options{'batch'})
	{
		die "Error: Environment variable Tool not set\n" 
			if (($^O=~/.*win32.*/i) && ! defined $ENV{'INDEP_TOOL'} && ! defined $ENV{'TOOL'} );
		$tool_own = $default_tool_own;
	}
	else
	{
		print "\nPlease enter the TOOL own directory";
		print "\n[$default_tool_own]" if ($default_tool_own);
		print ":";
		my $own = <STDIN>;
		chomp $own;
		while (( ! $own ) &&  (! $default_tool_own))
		{
			print "\nPlease enter the TOOL own directory";
			print "\n[$default_tool_own]" if ($default_tool_own);
			print ":";
	    	$own = <STDIN>;
	    	chomp $own;
		}
		$tool_own = $own ? $own : $default_tool_own;
	}
}  

if($^O=~/.*win32.*/i)
{
	my $perl_dir_default = undef;
	my $vcvars_default = undef;
	
	# set defaults 
	if ( $Options{'perldir'} )
	{ $perl_dir_default = $Options{'perldir'}; }
	elsif ( $ENV{'PERL'} )
	{ $perl_dir_default = $ENV{'PERL'}; }
	elsif ( defined $ENV{'INDEP_TOOL'} && -f "$ENV{'INDEP_TOOL'}\Perl" )
	{ $perl_dir_default = "$ENV{'INDEP_TOOL'}\Perl"; }
	elsif ( -f "$ENV{'TOOL'}\Perl" )
	{ $perl_dir_default = "$ENV{'TOOL'}\Perl"; }
	
	if ( $Options{'vcvars'} )
	{ $vcvars_default = $Options{'vcvars'}; }
	elsif ( $ENV{'VCVARS'} )
	{ $vcvars_default = $ENV{'VCVARS'}; }

	if ( defined $Options{'batch'})
	{
		#die "Error: Environment variable PERL not set\n" if (($^O=~/.*win32.*/i) && ! defined ($perl_dir_default));
		#$perl_dir = $perl_dir_default;
		#die "Error: $perl_dir not found !\n" unless ( -d "$perl_dir" );
		#if ( $Options{'vcvars'} )
		#{
		#	$vcvars = $Options{'vcvars'};
		#	die "Error: vcvars file $vcvars not found !\n" unless ( -f "$vcvars" );
		#}
	}
	else
	{
		# check perl directory
		while ( ! defined $perl_dir ) 
		{
			unless ( $Options{'perldir'} )
			{
				print "\nPlease enter the PERL directory";
				print "\n[$perl_dir_default]" if (defined $perl_dir_default);
				print ":";
				$perl_dir = <STDIN>;
				chomp $perl_dir;
			}
			while (( ! $perl_dir ) &&  (! defined $perl_dir_default))		
			{
				print "\nPlease enter the PERL directory";
				print "\n[$perl_dir_default]" if ($perl_dir_default);
				print ":";
				$perl_dir = <STDIN>;
				chomp $perl_dir;
			}
			$perl_dir = $perl_dir ? $perl_dir : $perl_dir_default;
			unless ( -f "$perl_dir\\bin\\perl.exe" )
			{
				print "Error: $perl_dir\\bin\\perl.exe not found !\n";
				$perl_dir = undef;
				$perl_dir_default = undef;
			}	
		}
		
		# check for vcvars.bat
		my $ANSW =  $Options{'vcvars'} ? "y":"x";
		until ($ANSW =~ /^(y|n)/i)
		{
			print ("Set a special comiler environment before make (y/n) ?");
			$ANSW = <STDIN>;
			chop $ANSW;
			lc $ANSW;
		}
		if ( $ANSW =~ /^y/i)
		{
			while ( ! defined $vcvars ) 
			{
				unless ( $Options{'vcvars'} )
				{
					print "\nPlease enter the batchfile to set comiler environment";
					if (defined $vcvars_default)
					{	print "\n[$vcvars_default]:" ; }
					else
					{	print ":"; }
					
					$vcvars = <STDIN>;
					chomp $vcvars;
				}
				while (( ! $vcvars ) &&  (! defined $vcvars_default))		
				{
					print "\nPlease enter the batchfile to set comiler environment";
					if (defined $vcvars_default)
					{	print "\n[$vcvars_default]:" ; }
					else
					{	print ":"; }
					
					$vcvars = <STDIN>;
					chomp $vcvars;
				}
				$vcvars = $vcvars ? $vcvars : $vcvars;
				unless ( -f "$vcvars" )
				{
					print "Error: vcvars file $vcvars not found !\n";
					$vcvars = undef;
					$vcvars_default = undef;
				}	
			}
		}
	}	
}

checkdir("$tool_own/");
	
if($^O=~/.*win32.*/i)
{
	
	print ("Create profile ...");
	system ("attrib -R $tool_own\\iprofile.bat");
	open(PROFILE, ">$tool_own\\iprofile.bat") || die "Couldn't open $tool_own\\iprofile.bat for writing";
	print PROFILE "rem --- profile was generated by createtoolprofile.pl ---\n\n";
	print PROFILE "set OWN=$tool_own\n";
	print PROFILE "if exist %OWN%\\ipreprof.bat call %OWN%\\ipreprof.bat\n";
	if ( defined $ENV{'INDEP_TOOL'} )
	{	print PROFILE "set INDEP_TOOL=$ENV{'INDEP_TOOL'}\n"; }
	else
	{	print PROFILE "set INDEP_TOOL=$tool_own\\tool\n"; }
	print PROFILE "set TOOL=%OWN%\\tool\n";
	print PROFILE "set VMAKE_PATH=%OWN%,$devenv_src//\n";
	print PROFILE "set RELVER=TOOL\n";
	print PROFILE "set TOOLREL=$tool_release\n";
	print PROFILE "set CORRECTION_LEVEL=\n";
	print PROFILE "set BUILDPRAEFIX=0\n";
	print PROFILE "set RELSTAT=PROD\n" if (defined $Options{'prod'});
	if (defined $Options{'platform'})
	{
		print PROFILE "set MACHINE_CONFIG=" . (($Options{'platform'} =~ /^nt(.*)$/i) ? "NT$1" : "$Options{'platform'}") . "\n";
	}
	print PROFILE "call %OWN%\\iprofile_skel.bat\n";
	close PROFILE;	
	# save iprofile.bat (don't overwrite by devstudio.pl)
	system ("attrib +R $tool_own\\iprofile.bat");
	print (" OK\nProfile created successfully\n");
}
else
{
	print ("OK\nCreate profile ...");
	open(PROFILE, ">$tool_own/.iprofile") || die "Couldn't open $tool_own/.iprofile for writing";
	print PROFILE "#!/bin/sh\n";
	print PROFILE "# --- profile was generated by createtoolprofile.pl ---\n\n";
	print PROFILE "OWN=$tool_own  export OWN\n";
	print PROFILE "TOOL=\$OWN/tool export TOOL\n";
	print PROFILE "VMAKE_PATH=\$OWN,$devenv_src// export VMAKE_PATH\n";
	print PROFILE "RELVER=RTOOL export RELVER\n";
	print PROFILE "TOOLREL=$tool_release export TOOLREL\n";
	print PROFILE "unset CORRECTION_LEVEL\n";
	print PROFILE "BUILDPRAEFIX=0 export BUILDPRAEFIX\n";
	print PROFILE "RELSTAT=PROD export RELSTAT\n" if (defined $Options{'prod'});
	print PROFILE "MACHINE_CONFIG=$Options{'platform'} export MACHINE_CONFIG\n" if (defined $Options{'platform'});
	
	if ( -f "$tool_own/.iprofile.skel")
	{
		open(PROFILE_SKEL, "< $tool_own/.iprofile.skel") || 
	    	die "Couldn't open $tool_own/.iprofile.skel for reading";
		print PROFILE "\n# ----------------------------------\n";
		print PROFILE   "# --- import from .iprofile.skel ---\n";
		print PROFILE   "# ----------------------------------\n\n";
		while (<PROFILE_SKEL>)
		{	print PROFILE $_;	}
		close PROFILE_SKEL;
	}
	close PROFILE;
	
	print (" OK\nProfile created successfully\n");
}


sub checkdir
{
	my $path = shift;
	my $dir_path = "";
	my $mode = 0775;
	# convert \ to /
	$path =~ s/\\/\//g;
	# search for directories
	while ($path =~ /([^\/]*\/)/g)
    {
		$dir_path.=$1;
	    if ( ! -d $dir_path )
		{
			die "Error: Can't create directory $dir_path\n" if ( mkdir($dir_path, $mode) == 0 );
			unless($^O =~ /mswin/i)
			{
				#
				# workaround for perl bug
				#
				my @statbuf = stat ($dir_path);
				my $filemode = $statbuf[2] & 0777;
				if ($filemode != $mode) 
				{ die "Error: Can't change permissions of directory $dir_path\n" if (chmod ($mode, $dir_path)== 0); }
			}
		}
	}
}


sub makeWriteable
{
	my $file = shift;
	my $recursive = shift;
	my $chmod_cmd;
	
	if($^O=~/.*win32.*/i)
	{	
		$chmod_cmd ="attrib -R ";	
		$chmod_cmd.="/S " if ($recursive);
		$chmod_cmd.="$file";
	}
	else
	{	
		if ($recursive)
		{
			$chmod_cmd ="find $file -type f -exec chmod +w {} \\;";	
		}
		else
		{
			$chmod_cmd ="chmod +w $file";	
		}
	}
	system ("$chmod_cmd") == 0 
			or die "\nError: Error while executing '$chmod_cmd'\n";
	
}

