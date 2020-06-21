#!/devtool/local/bin/perl
#
#    SDB::Make::ExecScript.pm - module to execute scripts activated by execscript=
#    
#    @(#)SDB::Make::ExecScript.pm     2005-08-03
#
#    &gar, SAP AG
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

package SDB::Make::ExecScript;

use strict;


sub ExecScript
{
	my $scriptcall = shift;
	my $file = shift;
	my @script_args = ();
	if ($scriptcall	=~ /^(.*)\((.*)\)$/)
	{
		$scriptcall = $1;
		push @script_args, (split ",", $2); 
	}
	else
	{
		push @script_args, $file;
	}
	local @ARGV = @script_args;
	do "$scriptcall"; 
	if ( $@ ) { die "$@";   }
}
1;