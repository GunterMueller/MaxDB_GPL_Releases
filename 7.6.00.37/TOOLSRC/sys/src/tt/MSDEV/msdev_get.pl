#
# @(#)msdev_get    2000-12-01
#
# G. Arnold, SAP AG
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
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

package DevStudio;
require 5.000;
# use Tk;
use Env;
use File::Copy;


# collect options for im?
my @Options = ();
my $File ="";
my $Relase ="";

$my_OWN = shift;
@FILES=@ARGV;

$ENV{OWN}=$my_OWN;

# read release from Release.dat in $my_OWN
open(RELEASE_DAT,"< $my_OWN/Release.dat") or 
	die "Error: Can't open $my_OWN/Release.dat\n";
$Release=<RELEASE_DAT>;
close(RELEASE_DAT);


# set environment
local @ARGV = ("-c",$my_OWN, $Release);
do "Devstudio.pl"; 
if ( $@ ) { print "$@"; exit }
$Pid=$$;
copy ("$my_OWN/iprofile.bat", "$my_OWN/msdev_get$Pid.bat") or die $1;
open(DEV_GET,">>$my_OWN/msdev_get$Pid.bat") or 
	die "Error: Can't open $my_OWN/msdev_get$Pid.bat\n";
print DEV_GET "echo on\n";
print DEV_GET "pget.pl ";
print DEV_GET join " ", @FILES;
print DEV_GET "\n";
close(DEV_GET);

$ret= system("$my_OWN/msdev_get$Pid.bat");
unlink("$my_OWN/msdev_get$Pid.bat");

die ("Error while executing msdev_get !\n") if ( $ret != 0 ) ;




__DATA__


  USAGE:  msdev_get <own> <filenames>

  1. Start Environment of release
  3. Start pview for files

  