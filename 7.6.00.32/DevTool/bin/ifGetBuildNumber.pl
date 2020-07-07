# ifGetBuildNumber
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
#  Implementation-Version: "7.3      Build 000-000-071-212"

use Env;

sub FormatBuildNr {
  my $cmd ="$ENV{TOOL}/pgm/GetBuildNr.exe";
  if (-x $cmd) {
    $BuildNr = `$cmd`;
    if ($BuildNr !~ m!BuildNr: (\d+)!) {
      $Buildnr = 0;
      warn "caution: Can't get a buildnumber. Don't use buildnumber.\n";
    }
    else {
      $Buildnr = $1;
    }
  }
  else {
    $Buildnr = 0;
  }
  $Buildnr = "0" x (9-length($Buildnr)) . "$Buildnr";
  $Buildnr = join "-", grep /\d+/, split( /(\d\d\d)/, $Buildnr);
  $ENV{BUILDPRAEFIX} = 0 unless defined($ENV{BUILDPRAEFIX});
  $ENV{RELVER}=~ /R(\d)(\d+)/;
  return sprintf("%d.%d.%d    Build %.3d", $1, $2, $ENV{CORRECTION_LEVEL}, $ENV{BUILDPRAEFIX}) . "-". $Buildnr;
}

print FormatBuildNr;
