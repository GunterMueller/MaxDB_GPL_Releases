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

#***********************
sub GetActualNumOfDataVols {
#***********************
    my($NumOfDataVols);
    $SqlCmd="sql_execute \"select NUMERIC_VALUE from SYSDBA.CONFIGURATION WHERE DESCRIPTION=\'No of data volumes\'\"";
    open(TMP, "$DBMCLI_PROG -u $CTRL_USR,$CTRL_PWD -d $DBNAME $SqlCmd|") || 
               die "'$DBMCLI_PROG -u $CTRL_USR,* -d $DBNAME $SqlCmd' failed\n";
    chomp($RC = <TMP>);
    chomp($Muell = <TMP>);
    chomp($NumOfDataVols = <TMP>);
    close TMP;
    die "$SqlCmd failed: RC=$RC" if ( $RC ne "OK");
    return($NumOfDataVols);
}


#*******************************
#           M A I N            #
#*******************************

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

$CYCLE = $ARGV[0];

my($i)=0  ;
my($End)=0;

# be sure, that all data volumes are in use
print "This script drops and adds different data volumes for $DBNAME.\n\n";

if ( ($Devsp=GetActualNumOfDataVols()) < $NUM_DATA_DEVS )
{
  print "Volumes currently in use: $Devsp, wanted: $NUM_DATA_DEVS \n";
  for ( $Devsp++ ; $Devsp <= $NUM_DATA_DEVS ; $Devsp++ )
  {
    print "add data volume $Devsp ...\n";
    CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_addvolume DATA", "");
  }
}

srand();

while ( ! $End )
{
  $Add=0;
  $i++;
  $VolToDel = $i % $NUM_DATA_DEVS + 1 ;

  if ($i == 1)
  {
	print "- dropping a volume waits a specified time depending on number of cycles\n";
	print "  and is incremented with each loop of mutAddDelVol\n";
	print "- adding a volume waits a randomized time between 30 and 90 seconds\n\n";
	print "- after a drop volume an add volume will be performed always\n\n";
  	print "waiting 30 seconds for first action\n\n";
	sleep(30);
  }

  if ($Node eq "" || $Node eq "localhost")
  {
	print "$i. call of 'x_cons $DBNAME show all'\n";
	call_x_cons();
  }

  $wait = ($i * $CYCLE) + ($i * $i);
  $counter = 0;
  print "waiting $wait seconds for next drop volume\n";
  $End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
  while ((!$End) && ($counter < $wait))
  {
	sleep(1);
	++$counter;
	$End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
  }
  $End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
  if (!$End) 
  {
  	print "	delete data volume $VolToDel ...";
  	CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_deletevolume $VolToDel", "Could not delete volume $VolToDel");
	$Add=1;
	print " done\n";
  }
  
  $wait = 30 + int(rand(60));
  $counter = 0;  
  print " waiting $wait seconds for next add volume\n";
  $End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
  while ((!$End) && ($counter < $wait))
  {
	sleep(1);
	++$counter;
	$End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
  }

  if ($Add==1)
  {
	print "	add data volume ...";
	CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_addvolume DATA", "Add volume failed");
	print " done\n\n";
  }

  $End = ((is_sema_posted($MUT_USER_ABORT)) || (!is_sema_posted($MUT_RUNNING))) ;
}
