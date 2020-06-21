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

local($SAVE_END)=0;
local($DEMO_LOG)=0;

local($NUM_NEW_LOGS)=0;
local($MUT_CHECK_SAVE)=1;


#***********************
sub del_sysupdstatlog {
#***********************
    print "\n$SELF deleting table SYSUPDSTATLOG ...";
    CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME -uSQL $SYSUSR,$SYSPWD -c sql_execute delete SYSUPDSTATLOG", "");
    print " done\n";
}

#***********************
sub diag_parseid_monitor_check {
#***********************
    print "\n$SELF diagnose monitoring active ...";
    DbUtilExecute("diagnose parseid on");
    sleep 10; 
    DbUtilExecute("diagnose parseid off");
    sleep 10; 
    DbUtilExecute("diagnose monitor selectivity 100");
    sleep 10; 
    DbUtilExecute("diagnose monitor time 0");
    sleep 10; 
    DbUtilExecute("diagnose monitor off");
    CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME sql_execute delete sysparseid","");
    print " finished\n";
}

#***********************
sub check_media {
#***********************
     my($DB,$MED_ID, $MED, $SAVE_TYPE, $OVERWR) = @_ ;
#    print "$SELF checking for media definition of $MED_ID... \n" ; 
#    if ( CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DB backup_media_get $MED_ID","") ne 0 ) 
#    {
	print "$SELF creating media definition for $MED_ID for $DB\n" ; 
	my($BACKUP_BLOCK_CNT);
	GetParam("BACKUP_BLOCK_CNT", \$BACKUP_BLOCK_CNT);
	CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DB backup_media_put $MED_ID $MED FILE $SAVE_TYPE 0 $BACKUP_BLOCK_CNT $OVERWR",
		"backup_media_put failed!") ;
#    }
}

#***********************
sub enough_new_logsegments {
#***********************
	( $NUM_NEW_SINCE_LAST_SV_PAG ) = @_ ;
	my($FULL_SV_PATH) = make_full_sv_path($SAVELOG_FILES) ;
	my($NUM_SAVES)=1;
	while(<$FULL_SV_PATH*>)
	{
		$NUM_SAVES++;
	}
	$$NUM_NEW_SINCE_LAST_SV_PAG = $NUM_SAVES - $NUM_NEW_LOGS ;
	my($IS_ENOUGH_LOG) = $$NUM_NEW_SINCE_LAST_SV_PAG >= $NUM_LOG_SEGMENTS ? 1 : 0 ;
	if ( $IS_ENOUGH_LOG != 0 )
	{
		$NUM_NEW_LOGS = $NUM_SAVES;
	}
	return($IS_ENOUGH_LOG);
}

#***********************
sub remove_logsaves {
#***********************
	if ( $SAVELOG_FILES ne "" )
	{
		my($FULL_SV_PATH) = make_full_sv_path($SAVEDATA_FILE) ;
		if ( -f "$FULL_SV_PATH" )
		{ 
			my($SV_DATA_LPNO) = get_lpno($FULL_SV_PATH, "f");
			$FULL_SV_PATH = make_full_sv_path($SAVELOG_FILES) ;
			while(<$FULL_SV_PATH*>)
			{
				$FULL_SV_PATH = make_full_sv_path($_) ;
				my($LAST_LPNO) = get_lpno($FULL_SV_PATH,"l") ;
				if ( $LAST_LPNO < $SV_DATA_LPNO )
				{
					print "$SELF removing '$FULL_SV_PATH' ( $SV_DATA_LPNO > $LAST_LPNO) \n" ;
					unlink $FULL_SV_PATH;
				}
			}
		}
		$NUM_NEW_LOGS = 0;
	}
}
 
#***********************
sub remove_saved_pages {
#***********************
	if ( $SAVEPAGES_FILES ne "" )
	{
		my($FULL_SV_PATH) = make_full_sv_path($SAVEPAGES_FILES) ;
		while(<$FULL_SV_PATH*>)
		{
			$FULL_SV_PATH = make_full_sv_path($_) ;
			print "$SELF removing '$FULL_SV_PATH'\n " ;
			unlink $FULL_SV_PATH;
		} 
	}
}

#***********************
sub is_logmode_demo {
#***********************
	# Have to be selected from system tables
    return (0) ;
}

#***********************
sub enable_autosave_log {
#***********************
    if ( $SAVELOG_FILES ne "" )
    {
       remove_logsaves();
       check_media( $DBNAME, $MEDIA_ID_SL, $SAVELOG_FILES, "AUTO", "YES" );
       print "$SELF enable autolog ...\n" ; 
#       CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uUTL -d $DBNAME autosave_on $MEDIA_ID_SL","can't enable AUTOSAVE LOG");
       CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME autolog_on $MEDIA_ID_SL","can't enable AUTOLOG");
    }
}


#***********************
sub disable_autosave_log {
#***********************
    if ( is_db_warm() )
    {
       print "$SELF disable autolog ...\n" ; 
#       CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uUTL -d $DBNAME autosave_off","");
       CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME autolog_off","");
    }
}

#***********************
sub check_save {
#***********************
	if($ENV{SAPDB_INTERNAL})
	{
		if ( defined $MUT_CHECK_SAVE )
		{
			my($MEDIA_ID, $BACKUP_TYPE)  = @_ ;
			print "$SELF checking save SAVE $BACKUP_TYPE ..." ;
			CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uSRV -d $DBNAME backup_restore_check $MEDIA_ID $BACKUP_TYPE",
					 "CHECK SAVE $BACKUP_TYPE failed");
			print " finished \n" ;

			if ($BACKUP_TYPE eq "DATA")
			{
				open(_DBMCLI_PRT, "$DBMCLI_PROT" ) || die "Could not open dbmcli.prt\n" ;
				while (<_DBMCLI_PRT>)
				{
					if($_ =~ /^Page Count/)
					{
						$PAGECOUNT=$_;
						break;
					}
				}
				close _DBMCLI_PRT;
				$PAGECOUNT =~ s/.*?(\d+)\s*/$1/;
print "PAGECOUNT from backup_restore_check = $PAGECOUNT\n";
				CallDbmCli ("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_execute \"select (sum(treeleavessize) + sum(treeindexsize) + sum(lobsize))/8 from files where type <> 'SHORT COLUMN FILE' and fileid < x'8000000000000000'\"",
						"select error");
				open(_DBMCLI_PRT, "$DBMCLI_PROT" ) || die "Could not open dbmcli.prt\n" ;
				chomp($RC = <_DBMCLI_PRT>);
				chomp($RC2 = <_DBMCLI_PRT>);
				chomp($PAGECOUNT2 = <_DBMCLI_PRT>);
				close _DBMCLI_PRT;
print "PAGECOUNT from 'select from files' = $PAGECOUNT2\n";
				if (($PAGECOUNT/$PAGECOUNT2 > 2) || ($PAGECOUNT2/$PAGECOUNT > 2))
				{
					print "WARNING: PAGECOUNT from backup_restore_check = $PAGECOUNT and PAGECOUNT from 'select from files' = $PAGECOUNT2\n";
					stop_mutsave();
					send_error_msg ("To much difference in PAGECOUNT value between database and backup\nbackup_restore_check = $PAGECOUNT and PAGECOUNT from 'select from files' = $PAGECOUNT2", $EXIT) ;
				}
			}
		}
		else 
		{
			print "$SELF NOT checking save SAVE $BACKUP_TYPE...! Set $MUT_CHECK_SAVE to reenable\n";
		}
	}
}

#***********************
sub save_pages {
#***********************
  if ( $SAVEPAGES_FILES ne "" )
  {
     print "$SELF save pages ..." ; 
     CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uUTL -d $DBNAME backup_save $MEDIA_ID_SP PAGES","SAVE PAGES failed");
     print " finished \n" ; 
     check_save($MEDIA_ID_SP, "PAGES");
     my($VERSION_NO) = @_ ;
     my($FULL_SV_PATH) = make_full_sv_path($SAVEPAGES_FILES) ;
     my($VERSION_PATH) = $FULL_SV_PATH ;
     $VERSION_PATH .= ".";
     $VERSION_PATH .= $VERSION_NO;
     print "$SELF renaming $FULL_SV_PATH --> $VERSION_PATH\n" ;
     rename($FULL_SV_PATH, $VERSION_PATH) ;
  }
}

#***********************
sub save_data {
#***********************
    my($LAST_SD_FILE) = make_full_sv_path($SAVEDATA_FILE) ;
    my($TMP_SD_FILE)  = make_full_sv_path("sv_dat.tmp") ;
    unlink($TMP_SD_FILE);
    rename($LAST_SD_FILE,$TMP_SD_FILE);
    print "$SELF save data ..." ; 
    my($RC) = CallDbmCli("-u $CTRL_USR,$CTRL_PWD -uUTL -d $DBNAME backup_save $MEDIA_ID_SD DATA","");
    if ($RC != 0)
    {
      print " aborted \n" ; 
      print "$SELF -u $CTRL_USR,* -uUTL -d $DBNAME backup_save $MEDIA_ID_SD DATA returned with code $RC\n" ;
      print "$SELF renaming $TMP_SD_FILE --> $LAST_SD_FILE\n" ;
	  unlink($LAST_SD_FILE);
      rename($TMP_SD_FILE,$LAST_SD_FILE);
      stop_mutsave();
      send_error_msg ("SAVE DATA failed", $EXIT) ;
    }
  else
    {
      print " finished \n" ; 
      check_save($MEDIA_ID_SD, "DATA");
      remove_logsaves();
      remove_saved_pages();
    }
}


#***********************
sub start_mutsave {
#***********************
    if ( is_sema_posted ($MUTSAVE_RUNNING) )
    { 
      post_sema($MUTSAVE_ABORT);
      my($SLEEP_TIME)=$SAVEPAGES_INTERVAL + 10 ;
      print "$SELF another mutsave is active, waiting $SLEEP_TIME(s) ...\n" ;
      sleep($SLEEP_TIME);
    }
    stop_mutsave();
    post_sema( $MUTSAVE_RUNNING ) ;
}

#***********************
sub stop_mutsave {
#***********************
    if ( !$DEMO_LOG  )
    {
#       disable_autosave_log() ;
    }
    reset_sema($MUTSAVE_RUNNING) ;
    reset_sema($MUTSAVE_ABORT) ;
}


#*******************************
#           M A I N            #
#*******************************

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

if ( $Node ne "" && $Node ne "localhost" )
{
	print "ATTENTION: mutsave is active, but no logfile will be deleted, beware of a 'not enough disk space'-problem\n\n";
}

$DEMO_LOG=is_logmode_demo() ;

start_mutsave();

sleep 1;

check_media( $DBNAME, $MEDIA_ID_SD, $SAVEDATA_FILE, "DATA" ,"YES" );
save_data();      # !! MUST BE BEFORE FIRST LOGSAVE 

if ( !$DEMO_LOG  )
{
  enable_autosave_log();
}

if ( $NUM_SAVE_PAGES == 0 )
{
  print "$SELF warning: save pages not tested ...\n";
}
else
{
  check_media( $DBNAME, $MEDIA_ID_SP, $SAVEPAGES_FILES, "PAGES", "YES" );
}

$SAVE_END=is_sema_posted($MUTSAVE_ABORT) ;

if ( $SAVE_END == 0 )
{
  post_sema( $MUTSAVE_STARTED ) ;
}

while ( $SAVE_END == 0 )
{    
  for ( $i=1; $SAVE_END == 0 && $i <= $NUM_SAVE_PAGES ;  )
  { 
    $SAVE_END=is_sema_posted($MUTSAVE_ABORT);
    if ( $SAVE_END == 0 )
    { 
      my($NEW_LOGS) ; 
      if ( $DEMO_LOG || enough_new_logsegments( \$NEW_LOGS ) ) 
      { 
        sleep $SAVEPAGES_INTERVAL ;
        check_data("check data extended");
        save_pages($i++);
        $i++;
        del_sysupdstatlog();
      }
      else
      { 
        print "$SELF only $NEW_LOGS logsegments detected, save pages ignored ...\n";
        check_data("check data except index");
        sleep 20 ;
      }
    }
  }

  $SAVE_END=is_sema_posted($MUTSAVE_ABORT);

  if ( $SAVE_END == 0 )
  {
    sleep $SAVEPAGES_INTERVAL ;
    check_data("check data extended");
    save_data();
    diag_parseid_monitor_check(); 
&if $OS in [ WIN32 ]
    call_system ("$DBMCLI_PROG -o ALLOC_STAT -u $CTRL_USR,$CTRL_PWD -d $DBNAME -uSQL superu,superu sql_execute select * from $SYSUSR.ALLOCATORSTATISTIC", 
	             " select from $SYSUSR.ALLOCATORSTATISTIC failed");
&else
    call_system ("$DBMCLI_PROG -o ALLOC_STAT -u $CTRL_USR,$CTRL_PWD -d $DBNAME -uSQL superu,superu sql_execute select \\* from $SYSUSR.ALLOCATORSTATISTIC", 
	             " select from $SYSUSR.ALLOCATORSTATISTIC failed");
&endif
  }
}

stop_mutsave();
