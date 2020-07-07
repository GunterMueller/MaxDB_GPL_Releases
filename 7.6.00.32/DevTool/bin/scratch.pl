#!/usr/bin/perl
#
# @(#)scratch   6.2	  11-11-1997	G. Groﬂmann
#
# umbenennen von einer oder mehreren Dateien
#
# _file <--> __file
# file  <--> _file
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
use File::Basename;
use ICopy;
use Getopt::Std;

die "\n" if !getopts('qhuav:');

if ( ($opt_h) || ($opt_q && $opt_a) ) {
	print <DATA>;
	exit 0;
}
if ((defined $opt_v ) && ($opt_v < 1))
{	die ("Error: Version count lower then minimum 1\n");}

if ( @ARGV == 0 ) { die "usage: scratch [-u] [-v versions] file [[file] ... ]\n"}

FILELIST: foreach $File (@ARGV) {
	local $source;
	if ( $opt_u ) {
		# unscratch
		local @path = ICopy::_solve_path($File);
		local ( $found, $name, $path, $unscratchname );
		foreach (@path) {
			($name, $path) = fileparse($_);
			$unscratchname = "$OWN/$path"."_".$name;
			if ( -f "$OWN/$path$name" )
			{
				print "$OWN/$path$name already unscratched!\n" unless ($opt_q);
				if ( $opt_a )
				{
					$ANSW="x";
					until ( $ANSW =~ /^(y|n)/i )
					{
						print "Delete it (y/n) ?";
						$ANSW = <STDIN>;
						chop($ANSW);
					}
					if ($ANSW =~ /y/i)
					{
						unlink ("$OWN/$path$name");
					}
					else
					{
						next FILELIST;
					}
				}
				else
				{
					exit 0;
				}

			}
			if ( -f $unscratchname ) { $found = 1; last; }
		}
		$source = $unscratchname if $found;
	}
	else {
		$source = ICopy::GetFilePath($File, 0, 1, 0);
	}
    if  ( $source ) {
		local @ARGV = ( $source );
		do "$TOOL/bin/closedoc$TOOLEXT";
		if ( $@ ) { die "Error while executing closedoc$TOOLEXT: $@"}
		($name, $path) = fileparse($source);
		$opt_u ? unscratch($name, $path) : scratch($name, $path) ;
	    print $opt_u ? "rename $source --> $path".substr($name,1): "rename $source --> ${path}_$name\n" unless ($opt_q);
		caller() ? 0 : exit 0;
	}
    else {
         die "scratch: file $source not found!\n";
    }
}

sub scratch {
	local($name, $path) = @_;
	local ($scratchname) = "$path"."_$name";

	if ( -f "$scratchname")
	{
		# PTS 1105072
		$tmp_name="_$name";
		if (( defined $opt_v) && ($tmp_name =~ /^_{$opt_v,}/) )
		{ unlink "$scratchname" 	}
		else
		{	scratch("_$name", $path) }
	}
	rename("$path$name", $scratchname) || die "Can't rename $path$name to $scratchname\n: $!\n";
}

sub unscratch {
	local($name, $path) = @_;
	local $unscratchname = substr($name, 1);
	rename("$path$name", "$path$unscratchname" )
		|| die "Can't rename $path$name to $path$unscratchname\n: $!\n";
	if ( -f $path."_".$name ) {
		unscratch( "_$name", $path )
	}
}

__DATA__

usage: scratch [-u [-a]] [-q] file [[file] ... ]

Optionen:
     -u unscratch : rename _file -> file (filename without _ !)
	 -a answer:     anwer to delete orginal file without _ if existent (only with u-option)
     -q quiet:      quiet mode (not with a-option)
