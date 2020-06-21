#!/usr/bin/perl
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
use File::Basename;


#*******************************
#           M A I N            #
#*******************************

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

# Reads script arguments
$oldArg = '';
foreach $arg (@ARGV)
{
	if ( $oldArg eq '-R' ){
	  $INSTROOT = $arg ;
	}
	$oldArg = $arg;
}

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

if ( is_sema_posted($MUT_RUNNING) )
{
    print "$SELF aborting mut ...\n" ;
    my($SLEEP_TIME)=10 ;
    post_sema($MUT_USER_ABORT) ;

    if ( is_db_warm() )
    {
      @ARGV = ("stop");
      do "mutdbana.pl";
      
      post_sema($MUTSAVE_ABORT);
      while ( is_sema_posted ($MUTSAVE_RUNNING) )
      { 
        print "$SELF mutsave is active, waiting $SLEEP_TIME(s) for termination ...\n" ;
        sleep($SLEEP_TIME);
      }
    }

    DbStop() ;

    my($i)=5;
    while ( is_sema_posted ($MUT_RUNNING) && $i > 0 )
    { 
      print "$SELF mut is still active, waiting max ", $SLEEP_TIME*$i ,"(s) for termination ...\n" ;
      sleep($SLEEP_TIME);
      $i--;
    }
    reset_sema($MUT_RUNNING) ;
    reset_sema($MUT_USER_ABORT) ;
}
else
{
    DbStop() ;
    print "$SELF no mut active\n" ;
}
reset_sema($MUTSAVE_RUNNING) ;
