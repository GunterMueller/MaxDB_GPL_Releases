#!/usr/bin/perl
# @(#) mutnight		7.x		2001-06-22
# Called by mut
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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
use Win32;

###############
sub PrintUsage{
###############
  print "Usage: $SELF [-b|-c] [-o] [-r] \n" ;
  print "              -b : sync and build new release (imf all) before\n" ;
  print "              -c : copy new release from server ($GRP) before\n" ;
  print "              -r : restore database before\n" ;
  print "              -s : using slowknl\n" ;
  print "              -q : using quickknl\n" ;
}

##############################
sub CopyExecutablesFromServer{
##############################
  my($LOCKFILE)="$GRP\\.compile-end" ;
  printf "waiting for creating of $LOCKFILE ...\n" ;
  while ( ! -f "$LOCKFILE" )
  {  
    my($SLEEP_TIME)=20 ;
    sleep($SLEEP_TIME);
  }

  #
  # copy new release from server 
  #

  print "copy files from $GRP/usr $OWN ...";
  sleep 2;
  system ("cp -ru $GRP/usr/bin $OWN/usr") ;
  system ("cp -ru $GRP/usr/env $OWN/usr") ;
  system ("cp -ru $GRP/usr/etc/ $OWN/usr") ;
  system ("cp -ru $GRP/usr/incl $OWN/usr") ;
  system ("cp -ru $GRP/usr/lib $OWN/usr") ;
  system ("cp -ru $GRP/usr/pgm $OWN/usr") ;
  system ("cp -ru $GRP/usr/misc $OWN/usr") ;
  system ("cp -ru $GRP/usr/runtime $OWN/usr") ;
  system ("cp -ru $GRP/usr/sap $OWN/usr") ;
  system ("cp -ru $GRP/usr/support $OWN/usr") ;
  system ("cp -ru $GRP/usr/sap $OWN/usr") ;
  system ("cp -ru $GRP/usr/symbols $OWN/usr") ;
}

##############################
sub BuildUpNewRelease{
##############################
  my($DBSpeed) = @_ ;
  @ARGV = ("all");
  do "imf.pl";
  if ( $DBSpeed eq "q" )
  {
    @ARGV = ("quickknl");
    do "imq.pl";
  }
  if ( $DBSpeed eq "s" )
  {
    @ARGV = ("slowknl");
    do "ims.pl";
  }
}

#sceduling the mut via NT scheduler: !!! %MUT% has to be expanded by the user !!! 
#
# %windir%\system32\CMD.EXE /c "%TOOL%\perl\bin\perl.exe %TOOL%\bin\devstudio.pl %OWN% %RELEASE% %MUT%\mutnight.bat"
#
# example:
# C:\WINNT\system32\CMD.EXE /c "%TOOL%\perl\bin\perl.exe %TOOL%\bin\devstudio.pl d:\V74 7.4 D:\V74\test\port\mut\mutnight.pl"


#set SEND_SCRIPT=send_mut_err_to fr.hoffmann@sap.com
#set MUT_SIZE_FACTOR=2

#checking parameters ...
$BuildNewRelease = 0 ;
$CopyFromServer  = 0 ;
$WithRestore     = 0 ;
$DBSpeed         = "f" ;

for ( $i = 0 ; $i < @ARGV ; $i++ )
{
	$argv0 = substr ( $ARGV[$i] , 0 , 1 ) ;
	if ( $argv0 eq "-" )
	{
		$argv1 = substr ( $ARGV[$i] , 1 , 1 ) ;
		if ( $argv1 eq "r" )
	    	{ $WithRestore = 1 }
		if ( $argv1 eq "c" )
	    	{ $CopyFromServer = 1 }
		if ( $argv1 eq "b" )
	    	{ $BuildNewRelease = 1 }
		if ( $argv1 eq "q" )
	    	{ $DBSpeed = "q" }
		if ( $argv1 eq "s" )
	    	{ $DBSpeed = "s" }
		if ( $argv1 eq "h" )
	    	{ print PrintUsage();
              exit;
            }
    }
}

#
# abort a proberbly running MUT 
#
chdir "$MUT" ;

@ARGV = ("100");
do "./mutabort.pl";


#
# cleanup old kernel to avoid that the test is running with an old kernel  
#
if  ( $CopyFromServer || $BuildNewRelease )
{
  system("rm $INSTROOT/pgm/_*");
  system("rm $INSTROOT/pgm/kernel.*");
  system("rm $INSTROOT/pgm/quickknl.*");
  system("rm $INSTROOT/pgm/slowknl.*");
}

if  ( $CopyFromServer )
{  CopyExecutablesFromServer(); }
else
{
  if  ( $BuildNewRelease )
  { BuildUpNewRelease($DBSpeed); }
}

#
# make new mut  
#
@ARGV = ("mutsqldbc");
do "imf.pl";

#
# and start it 
#

@ARGV = ();
push @ARGV, "-$DBSpeed" ;

# - with recovery ?? --
if ( $WithRestore == 1 )
{ push @ARGV, "-r"; }

# - 1000 Rounds ?? --
push @ARGV, "1000" ;

do "./muti.pl";
