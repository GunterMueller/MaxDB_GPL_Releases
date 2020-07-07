#!/usr/bin/perl
#
# @(#)ils		1997-11-18
#
# G. Groﬂmann, SAP AG
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

package ils;
use Env;
use Getopt::Std;
use ICopy;

$Usage = "usage: ils [-h] [-b] [-l[0-9][x]] file\n";
die "" if !getopts('hbl:');
if (  @ARGV == 0 && !defined($opt_h))
{
	die $Usage;
}
if ( $opt_h )
{
	print <DATA>;
	exit 0;
}
if ( defined($opt_l) )
{
	die $Usage unless $opt_l =~ /^(\d)x?$/i;
	$level = $1; $exclusive = 1 if $opt_l =~ /x$/i;
}
$level = 0 unless defined($level);
$exclusive = 0 unless defined($exclusive);

if ( ICopy::_GetPathList($VMAKE_PATH) < $level+1 )
{
	die "The level value dont match! Use a lower level.\n";
}

$source = shift;
local @vmakepath = ICopy::_GetPathList($VMAKE_PATH);

if ( defined($opt_b) )
{
    local $abs_path, $rel_path, $output, $bpath, $found;

    $found = 0;
    $bpath = ICopy::_GetBackupPath($source);
    if ( defined($bpath))
    {
        if ( ICopy::_get_path_status($bpath) eq 'local' )
        {
	    if ($^O !~ /mswin32/i )
	    {
		$output = `ls $bpath/$source.* 2>&1`;
	    } else {
		$bpath =~ tr [/][\\];
		$output = `cmd /c dir /B $bpath\\$source.* 2>&1 `;
                @list = split (/\n/,$output);
                $output = "";
                foreach $element (@list) {
		    $output .= "$bpath/$element\n";
		}
	    }
        }
        else
        {
			local ($computer, $rpath) = $bpath =~ /([\w\.]+):(.*)/;
            $output = `$REMCL sh $computer ls $rpath/$source.*`;
        }
        $found = 1 if  $output !~ /no such file/i
    }

    if ( !$found)
    {
        warn "backup not found!\n";
    }
    else
    {
        $output =~ tr /\//\\/;
        print $output;
    }
    exit;
}

local ($start, $stop) = ($level, $exclusive ? $level : $#vmakepath);
local $curlevel = $start;
print "----------------------------------------------------------\n\n";
foreach $vmakepath ( @vmakepath[$start..$stop] )
{
	print "level $curlevel - $vmakepath :\n";
	if ( ($abs_path, $rel_path ) = ICopy::GetFilePath($source, $curlevel, 1, 1) )
    {
		# $source existiert auf $curlevel
		if ( ICopy::_get_path_status("$abs_path/$rel_path") eq 'local' )
        {
			local @statinfo = stat "$abs_path/$rel_path";
			local ( $r_size, $r_mtime ) = (\7, \9);
			print "@statinfo[$$r_size] ", scalar localtime(@statinfo[$$r_mtime]),  " $rel_path\n";
		}
		elsif (ICopy::IsUseSMB())
		{
			my @stat = SMB::stat (ICopy::make_smbfilename
				($abs_path."/".$rel_path));
			print @stat[7], " ", scalar localtime (@stat[9]), " ",
				$rel_path."\n";
		}
		else
        {
			local $path = "$abs_path/$rel_path";
			local ($computer, $rpath) = $path =~ /([\w\.]+):(.*)/;
			local $statinfo = `$REMCL sh $computer ls -l $rpath`; chop $statinfo;
			# PTS 1001753
			local @all_dates = split (/ +/,$statinfo);
            print "$all_dates[4]   $all_dates[5] $all_dates[6] $all_dates[7]\n";
		}
	}
	else
    {
		warn "$source NOT found!\n";
	}
	print "\n";
	$curlevel++;
}
print "----------------------------------------------------------\n";


__DATA__

usage: ils [-h] [-b] [-l level[x]] file

options:

 -b         : SAPDB internal option
              list all backup versions of the file

 -l<level>  : list existent files from the specified level down to
              the final level

 -l<level>x : list only the file at the specified level (exclusive)

