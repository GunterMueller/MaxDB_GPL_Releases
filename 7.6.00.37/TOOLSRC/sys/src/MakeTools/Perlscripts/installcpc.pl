#!/usr/bin/perl
#    installcpc  - install precompiler versions from lc_pool
#
#    installcpc.pl     2004-02-10
#
#    &gar, SAP AG
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

use File::Path;
use Getopt::Long;

use strict;
my $bit = $ENV{BIT64} ? "64" : "32";
my $substituted_lcversion = undef;
my %Options= undef;
my $lc_own= undef;
my $dir_sep = "/";

if (!Getopt::Long::GetOptions( \%Options, 'h|?', 'bit=i' ) || $Options{'h'} )
{
	die "Usage: installcpc.pl [-bit 32|64] <lcversion> <instdir>\n";
}

my $lcversion = shift;
my $instdir = shift;

(defined $lcversion && defined $instdir) || 
	die "Usage: installcpc.pl [-bit 32|64] <lcversion> <instdir>\n";

$bit = $Options{'bit'} if (defined $Options{'bit'});

# format directory windows like
if ($^O =~ /win32/i)
{
	$dir_sep = "\\";
	if ($instdir =~ /\$/)
	{
		# allow $ for environment variables
		$instdir =~ s/\$(\w*)/$ENV{"$1"}/g;
	} 
	$instdir =~ tr /\//\\/;	
}

# get lcown of precomiler sdk and substituted lcversion (dev,cor,ramp)
do "$ENV{'TOOL'}/bin/vmakeEnv.pl";
if ( $@ ) { die "can't execute $ENV{'TOOL'}/bin/vmakeEnv.pl\n:message:$@"; }
# -lcpool_count LastBuild needed for dev (else default to Last Build)
$lc_own = vmakeEnv::getLC_OWN("-lcversion", "$lcversion", "-bit", "$bit", "-lcpool_count", "LastBuild" );
print ("Start installation of $lcversion $bit-bit precompiler:\n\n");
print ("  SRC : $lc_own\n");
print ("  DEST: $instdir\n\n");

unless ( -d $instdir )
{
	mkpath($instdir, 0, 0775) || die "can't mkdir $instdir : $!";
}

my $rc = system ("cd $instdir && SAPCAR -xvf $lc_own${dir_sep}SAPDBCSDK.SAR");
if ($rc == 0)
{ 	print "\nInstallation finished successfully\n"; 	exit 0; }
else
{	print "\nInstallation failed\n"; exit -1; }

