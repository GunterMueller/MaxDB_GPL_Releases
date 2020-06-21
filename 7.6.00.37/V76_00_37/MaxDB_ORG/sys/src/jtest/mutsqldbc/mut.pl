#!/usr/bin/perl
# @(#) mut		6.2	1998-01-28
# Calls mutrun, reads from mutvar
# set the correct values for DBNAME and User/Password in file mutvar
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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


do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

# set the number of rows and the maximum
# length of long columns (space consuming !)

AnalyzeAndSetMutArgGlobals();

if(!$INSTROOT){
	print "$SELF Error! Release installation path is not defined.\nCall the script with argument \"-R <dependent_path> \".\n";
	exit;
}

# Update path in order to reach dbmcli
&if $OSSPEC in [ WIN32 ]
$ENV{PATH}=$ENV{PATH}.';'.$INSTROOT.'\bin'.';'.$INSTROOT.'\pgm';
&else
$ENV{PATH}=$ENV{PATH}.':'.$INSTROOT.'/bin'.':'.$INSTROOT.'/pgm';
&endif
$ENV{INSTROOT}=$INSTROOT;

if ( $Restore == 1 )
{
	do "./mutrest.pl" ;
	if ( $@ ) { print "$@"; exit }
	$INIT_TABLES = 0;
}
else
{ 
	$INIT_TABLES = 1; 
}

if ( !is_db_warm() )
	{set_serverdb( "WARM" )}

if ( $Verbose )
{
	print "$SELF Restore = " , $Restore , "\n";
	print "$SELF Verbose = " , $Verbose , "\n";
	print "$SELF Durch   = " , $DURCH , "\n" ;
	print "$SELF Crash/Rest = ", $RestartAfterCheckpointAndCrash , "\n" ;
}

do "./mutrun.pl";
if ( $@ ) { print "$@"; exit }
