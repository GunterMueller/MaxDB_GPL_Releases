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


$MUTDBANA_RUNNING="mutdbana.act" ;
$MUTDBANA_ABORT="mutdbana.abo";
$MUTDBANA_OUTPUT_DIR_ONLY="dbanaprt";
$MUTDBANA_OUTPUT_DIR="-o $MUTDBANA_OUTPUT_DIR_ONLY";

use Env;
use File::Basename;
use integer;

#*******************************
#           M A I N            #
#*******************************

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

$CYCLE = int (5 + ($ARGV[1]/10));

if ( $ARGV[0] eq "start" ) 
{
  if (is_sema_posted ($MUTDBANA_RUNNING) )
  {
    print "dbanalyzer already active.\n";
  }
  else
  {
    post_sema($MUTDBANA_RUNNING) ;

    ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    $DBAnaClearDate = sprintf("%4d%02d%02d", $year+1900, $mon+1, $mday > 2 ? $mday - 2 : $mday);

    print "dbanalyzer deletes protocols older $DBAnaClearDate ...\n";
    if ($Node eq "" || $Node eq "localhost")
    {
	call_system ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR -delete $DBAnaClearDate",
		"deleting dbanalyzer protocols failed");
    }
    else
    {
	call_system ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -n $Node -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR -delete $DBAnaClearDate",
		"deleting dbanalyzer protocols failed");
    }

    print "dbanalyzer writes all output in various *.csv-files in the outputDir $MUTDBANA_OUTPUT_DIR\n";
    print " see also $SELF.*.prt for usual window output\n";
    if ($Node eq "" || $Node eq "localhost")
    {
	call_system ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR",
		"starting dbanalyzer failed");
    }
    else
    {
	call_system ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -n $Node -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR",
		"starting dbanalyzer failed");
    }

    if ( !is_sema_posted ($MUTDBANA_ABORT) )
    {
      send_error_msg ("dbanalyzer finished unsuccessfully", $EXIT) ;
    }
    reset_sema($MUTDBANA_ABORT) ;
  }
}
else
{
  post_sema($MUTDBANA_ABORT) ;
  if (is_sema_posted ($MUTDBANA_RUNNING) )
  {
    reset_sema($MUTDBANA_RUNNING) ;
    if ($Node eq "" || $Node eq "localhost")
    {
	call_system2 ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR -stop ","stopping dbanalyzer failed");
    }
    else
    {
	call_system2 ("dbanalyzer -u SUPERU,SUPERU -d $DBNAME -n $Node -c 1 -t $CYCLE $MUTDBANA_OUTPUT_DIR -stop ","stopping dbanalyzer failed");
    }
  }
  else
  {
    print "No dbanalyzer is active.\n";
    send_error_msg ("dbanalyzer was not active", $EXIT) ;
  }
}
