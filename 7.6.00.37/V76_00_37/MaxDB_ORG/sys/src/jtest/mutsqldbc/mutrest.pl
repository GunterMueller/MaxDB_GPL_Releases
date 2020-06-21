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

local($RUNDIR)="" ;
local($LAST_LPNO)=0 ;

 
#***********************
sub auto_recover {
#***********************
  print "automatic recovery in progress ..." ;
  CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uUTL -d $DBNAME autorecover","autorecover failed");
  print "finished \n" ;
  return(1);
}

#*******************************
#           M A I N            #
#*******************************


do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

DbStop() ;
DbStart() ;
&if $OS in [ WIN32 ]        
        DbUtilExecute("SET PARAMETER CHECK_TREE='YES'"); # Check Index Pages 
&else
        DbUtilExecute("SET PARAMETER CHECK_TREE=\\'YES\\'"); # Check Index Pages 
&endif

die "!!! restore failed !!!\n" if !auto_recover() || !is_db_warm() ;
check_data("check data extended");

print "$SELF loading system tables...\n";
CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME load_systab -ud dom","can't load system tables failed");
