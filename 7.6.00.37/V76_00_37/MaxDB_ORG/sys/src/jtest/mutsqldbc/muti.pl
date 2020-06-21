#!/usr/bin/perl
# @(#) muti		6.2     1998-01-28
# Initializes, then calls mutrun, reads from mutvar
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


#***********************
sub remove_saves {
#***********************
    my($SAVES) = @_ ;
    if ( $SAVES ne "" )
      {
        my($FULL_SV_PATH) = make_full_sv_path($SAVES) ;
        print "$SELF removing $FULL_SV_PATH* ...\n" ;
        unlink <$FULL_SV_PATH*> ;
      }
}


#***********************
sub remove_all_saves {
#***********************
    remove_saves ( $SAVEDATA_FILE ) ;
    remove_saves ( $SAVEPAGES_FILES ) ;
    remove_saves ( $SAVELOG_FILES ) ;
}


#*******************************
#           M A I N            #
#*******************************

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

print "Start of test at ", scalar localtime(),"\n";
$start_time=time();
@tmp_ARGV=@ARGV;
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

if(!$TOOLVARS){
	$TOOLVARS="toolvars.pl";
}

$BIG_DURCH = $DURCH;

if ( $Restore == 1 )
{
  do "./mutrest.pl" ;
  die "!!! No more test after restore failed !!!\n" if $@ ;

  if ( is_db_warm() )
    { 
      $DURCH       = 1;
      $INIT_TABLES = 0;
      do "./mutrun.pl";
      die "!!! No more test after mut $DURCH failed !!!\n" if $@ ;
    }
}

set_serverdb("COLD");

print "$SELF activate serverdb ...\n";
CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_activate $SYSUSR,$SYSPWD","db_activate failed");

remove_all_saves() ;

print "$SELF loading system tables...\n";
CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME load_systab -ud dom","can't load system tables failed");
sleep(3);
print "$SELF loading system tables again...\n";
CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME load_systab -ud dom","2nd load system tables failed");

$DURCH      = $BIG_DURCH;
$INIT_TABLES = 1;
@ARGV = ("$start_time", "@tmp_ARGV");
do "./mutrun.pl";