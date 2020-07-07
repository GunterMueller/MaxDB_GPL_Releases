#!/usr/bin/perl
# ipf           [-old | -h{0-9}] target
#
# @(#)ipf  		1999-07-27
#
# Displays protocol of "target".
#
# changes:
# cba 199/07/27
# body moved to iprot.pm ADIS CR 1103480
#
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

use Env;
use Carp;
use iprot;

$VMAKE_VARIANT=$ENV{VMAKE_DEFAULT_VARIANT};

$VMAKE_VARIANT="Release" unless ($VMAKE_VARIANT);
# default is not sync 
$sync = -1;
@NEWARGV=();
$found_config = undef;	
$ENV{APOMAKE}=1;

while ($_ = $ARGV[0], /^-/) {
      shift @ARGV;
      last if /^--$/;

      if(/^-?-config=(.*)$/) 
	  {
         $found_config = $1 ; 
		 next; 
	  }

	# add all not matched options 
      push @NEWARGV, $_;
	  next;
}

# ckeck for default or modified Configuration and changelists
if ( $found_config )
{
	$VMAKE_VARIANT=$found_config;
}

while ($_ = $ARGV[0]) {
      shift @ARGV;
	  if (/^(.*)(\.[^.]*)$/)
	  {
		$target=$1."+$VMAKE_VARIANT".$2;
	  }
	  else
	  {
		$target=$_."+$VMAKE_VARIANT";
	  }
	  push @NEWARGV, $target;
}
		
iprot ("lcprot", @NEWARGV);

