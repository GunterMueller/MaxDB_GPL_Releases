#!/usr/bin/perl
# @(#)mutprepare.pl		7.4		2002-05-06
#
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


use Env;
&if $OS in [ WIN32 ]
use Win32;
use Win32::IPC;
use Win32::Process;
&endif
use integer;

#*******************************
#           M A I N            #
#*******************************

# Setting identification
$VARFILE='mutvar';

$ENV{RELVER} =~ /R(\d\d)/;
$VERSION=$1;

$CORREC=$ENV{CORRECTION_LEVEL};
if($CORREC < 10){
	$CORREC = "0$CORREC";
}

$BUILD=$ENV{BUILDPRAEFIX};
if($BUILD < 10){
	$BUILD = "0$BUILD";
}

$TMPDBNAME="MU$VERSION$CORREC$BUILD";
$TMPSYSUSER="DBA";
$TMPSYSPWRD="DBA";
$TMPCONTUSER="DBM";
$TMPCONTPWD="DBM";
$rewrite_varfile=0;

if(-f "$VARFILE")
{
	open(FILE_IN, "<./$VARFILE") or die "Can't open ./$VARFILE: $!";
	chomp($_ = <FILE_IN>); 
	close FILE_IN;
	($TMPDBNAME, $TMPSYSUSER, $TMPSYSPWRD, $TMPCONTUSER, $TMPCONTPWD, $TMPNode) = split / /, $_;

	if ($DBNAME ne "")
	{
		$TMPDBNAME = $DBNAME;
		$rewrite_varfile=1;
	}
	if ($SYSUSER ne "" && $SYSPWRD ne "")
	{
		$TMPSYSUSER = $SYSUSER;
		$TMPSYSPWRD = $SYSPWRD;
		$rewrite_varfile=1;
	}
	if ($CONTUSER ne "" && $CONTPWD ne "")
	{
		$TMPCONTUSER = $CONTUSER;
		$TMPCONTPWD = $CONTPWD;
		$rewrite_varfile=1;
	}
	if ($Node ne "")
	{
		$TMPNode = $Node;
		$rewrite_varfile=1;
	}
}
else
{
	$rewrite_varfile=1;
	if ($DBNAME ne "")
	{
		$TMPDBNAME = $DBNAME;
	}
	if ($SYSUSER ne "" || $SYSPWRD ne "")
	{
		$TMPSYSUSER = $SYSUSER;
		$TMPSYSPWRD = $SYSPWRD;
	}
	if ($CONTUSER ne "" || $CONTPWD ne "")
	{
		$TMPCONTUSER = $CONTUSER;
		$TMPCONTPWD = $CONTPWD;
	}
	if ($Node ne "")
	{
		$TMPNode = $Node;
	}
}

if ($rewrite_varfile)
{
	open(MUTVAR,">$VARFILE");
	print MUTVAR "$TMPDBNAME $TMPSYSUSER $TMPSYSPWRD $TMPCONTUSER $TMPCONTPWD $TMPNode";
	close MUTVAR;
}

&if $OS=WIN32 # but this is for the 64 bit case ...
# In case of imf running on IA64:
if($ENV{MACHINE}){
	if($ENV{MACHINE} eq "IA64"){
		$ENV{CPU}="IA64";	
	}
}
&endif
