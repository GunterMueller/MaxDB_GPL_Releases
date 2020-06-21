#!/usr/bin/perl
# @(#) mutrun		7.1		1998-01-22
# Called by mut
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

$ROWS = (3000 * $MUT_SIZE_FACTOR) * $SizeMultiplier ;
$MAXLCOL = 19000 * $MUT_SIZE_FACTOR;

&if $OS in [ WIN32 ]
$TPUT = "cls";
&else
$TPUT = "clear";
&endif

#***********************
sub start_mut {
#***********************
#if ( is_sema_posted($MUT_RUNNING) )
# { stop_mut();
#   print "$SELF another mut is active, please stop it and try again...\n" ;
#   exit(1);
# }
#else
 {
   stop_mut();
   post_sema($MUT_RUNNING) ;
   unlink "mutsave.pl.prt";
 }
}

#***********************
sub stop_mut {
#***********************
	if ( is_sema_posted ($MUTSAVE_RUNNING) ){
		post_sema($MUTSAVE_ABORT);
	}

    reset_sema($MUT_RUNNING) ;
    reset_sema($MUT_USER_ABORT) ;
}

#***********************
sub DbmCliCreateUsers {
#***********************
    my($OK)=1;

    print "$SELF drop old and create new users ...";
    # Drop all users created in CREAT1.cmd and CREAT2.cmd
    open (_TMP, ">$DBMCLI_CMD_SCRIPT" ) || die "open drop / create user scriptfile failed\n";
    print _TMP "sql_connect $SYSUSR,$SYSPWD\n" ;
    print _TMP "sql_execute drop user SUPERU\n" ;
    print _TMP "sql_execute drop user A1\n" ;
    print _TMP "sql_execute drop user A2\n" ;
    print _TMP "sql_execute drop user A3\n" ;
    print _TMP "sql_execute drop user A4\n" ;
    print _TMP "sql_execute drop user A5\n" ;
    print _TMP "sql_execute drop user A6\n" ;
    print _TMP "sql_execute drop user A7\n" ;
    print _TMP "sql_execute drop user A8\n" ;
    print _TMP "sql_execute drop user A9\n" ;
    print _TMP "sql_release\n" ;
    close _TMP ;
    CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME -i $DBMCLI_CMD_SCRIPT", "");

    open (_TMP, ">$DBMCLI_CMD_SCRIPT" ) || die "open drop / create user scriptfile failed\n";
    print _TMP "sql_connect $SYSUSR,$SYSPWD\n" ;
    print _TMP "sql_execute create user SUPERU password SUPERU dba not exclusive\n" ;
    print _TMP "sql_execute create user A1 password B1 dba\n" ;
    print _TMP "sql_execute create user A2 password B2 dba\n" ;
    print _TMP "sql_execute create user A3 password B3 dba\n" ;
    print _TMP "sql_execute create user A4 password B4 dba\n" ;
    print _TMP "sql_execute create user A5 password B5 dba\n" ;
    print _TMP "sql_execute create user A6 password B6 dba\n" ;
    print _TMP "sql_execute create user A7 password B7 dba\n" ;
    print _TMP "sql_execute create user A8 password B8 dba\n" ;
    print _TMP "sql_execute create user A9 password B9 dba\n" ;
    print _TMP "sql_release\n" ;
    close _TMP ;
    CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME -i $DBMCLI_CMD_SCRIPT", "");
    print " done\n";

    return ($OK) ;
}

#***********************
sub init_tables {
#***********************
    my($OK)=1;

    system("$TPUT");

    DbmCliCreateUsers() ;

    post_sema("initu.act");
    push @actfiles, "initu.act";

    print "$SELF Initialize tables ...\n";
    system("initu -rows $ROWS -maxlongraw $MAXLCOL -use_cluster $USE_CLUSTER -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD 2> initu.prot");

    $OK = is_db_warm();

    if ( $OK )
    {
      system("$TPUT");
      foreach $i ( 1, 2, 3, 4, 5, 6 ) 
      {
	post_sema("iendlosA".$i.".act");
	push @actfiles, "iendlosA".$i.".act";
	system("iendlos -user A$i,B$i -rows $ROWS -use_cluster $USE_CLUSTER -database $DBNAME -node $Node -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD >nul");
      } 
      $OK = is_db_warm();
    }
    return($OK);
}

#***********************
sub start_superu_1 {
#***********************

    post_sema("endloss1.act");
    push @actfiles, "endloss1.act";

&if $OS in [ WIN32 ]
    Win32::Process::Create($Proc,
	    			 "$SYSTEMROOT\\system32\\cmd.exe",
					 "/c endloss.exe -line 2 -maxcycle $DURCH3 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -checklongrows -maxruntime $MaxRuntime -superusernumber 1 -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD",
					 1,
					 CREATE_DEFAULT_ERROR_MODE,
					 ".") || die Win32::FormatMessage(Win32::GetLastError());
    $Proc->SetPriorityClass(NORMAL_PRIORITY_CLASS);
&else
    FORK: {
	    if ( $pid = fork() ) {
		    #parent
	    }
	    elsif ( defined($pid) ) {
		    # child
		    my($RC) = system("endloss -line 2 -maxcycle $DURCH3 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -checklongrows -maxruntime $MaxRuntime -superusernumber 1 -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD") ;
            if ( $RC != 0 )
		{ die "Error while executing 'endloss -line 2 -maxcycle $DURCH3 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -checklongrows -maxruntime $MaxRuntime -superusernumber 1 -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD': $!\n" ; }
            else
		{ exit ; }
	    }
#	    elsif ( $! =~ /No more proccess/ ) () {
#		    #EAGAIN, supposedly recoverable fork error
#    		sleep 5;
#	    	redo FORK;
#	    }
 	    else {
 		    #weird fork error
 		    die "Can't for: $!\n";
	    }
    }
&endif
}


#***********************
sub start_a1_a6 {
#***********************

    $LNO = 9;		# Each user will write 2 lines onto screen, starting with $LNO

    # Users A[1-6]
    my($i) = 0;
    foreach $Process ( Proc1, Proc2, Proc3, Proc4, Proc5, Proc6 ) 
    {
	$i++;
        post_sema("rendlosA".$i.".act");
	push @actfiles, "rendlosA".$i.".act";

&if $OS in [ WIN32 ]
    	#Create the process object.

#ATTENTION: edit mut.h in check_params(), if you edit the call from the pl-script for rendlos.exe with more than two "-ignore"-values

	    Win32::Process::Create($$Process,
						 "$SYSTEMROOT\\system32\\cmd.exe",
						 "/c rendlos.exe -database $DBNAME -node $Node -user A$i,B$i -line $LNO -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -maxcycle $DURCH -maxruntime $MaxRuntime -ignore 807,9807 -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD",
						 1,
	         			 CREATE_DEFAULT_ERROR_MODE,
 						 ".") || die Win32::FormatMessage(Win32::GetLastError());
    	$$Process->SetPriorityClass(NORMAL_PRIORITY_CLASS);
&else
        FORK: {
	        if ( $pid = fork() ) {
		        #parent
	        }
	        elsif ( defined($pid) ) {
		       # child

#ATTENTION: edit mut.h in check_params(), if you edit the call from the pl-script for rendlos.exe with more than two "-ignore"-values
 	          my($RC) = system("rendlos -database $DBNAME -node $Node -user A$i,B$i -line $LNO -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -maxcycle $DURCH -maxruntime $MaxRuntime -ignore 807,9807 -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD") ;
              if ( $RC ) 
              { die "Error while executing 'rendlos -database $DBNAME -node $Node -user A$i,B$i -line $LNO -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -maxcycle $DURCH -maxruntime $MaxRuntime -ignore 807,9807 -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD': $!\n"; }
              else
              { exit ; }
	        }
#	        elsif ( $! =~ /No more proccess/ ) () {
#		        #EAGAIN, supposedly recoverable fork error
#    		    sleep 5;
#	    	    redo FORK;
#	        }
    	    else {
	    	    #weird fork error
		        die "Can't for: $!\n";
	        }
        }
&endif
		sleep (2) ;
        $LNO = $LNO + 2;
    }
}

#***********************
sub start_superu_2 {
#***********************

    post_sema("endloss2.act");
    push @actfiles, "endloss2.act";

&if $OS in [ WIN32 ]
    #Create the process object.
    Win32::Process::Create($Proc7,
	   				 "$SYSTEMROOT\\system32\\cmd.exe",
					 "/c endloss.exe -line 4 -maxcycle $DURCH2 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 2 -maxruntime $MaxRuntime -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD",
					 1,
					 CREATE_DEFAULT_ERROR_MODE,
					 ".") || die Win32::FormatMessage(Win32::GetLastError()); #current dir.
    #Set the process priority
    $Proc7->SetPriorityClass(NORMAL_PRIORITY_CLASS);
&else
    FORK: 
    {
	    if ( $pid = fork() ) 
        {
		    #parent
	    }
	    elsif ( defined($pid) ) 
        {
		    # child
		    my($RC)=system("endloss -line 4 -maxcycle $DURCH2 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 2 -maxruntime $MaxRuntime -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD") ;
            if ( $RC ) 
            { die "Error while executing 'endloss.exe -line 4 -maxcycle $DURCH2 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 1 -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD': $!\n" ; }
            else
            { exit ; }
	    }
#	elsif ( $! =~ /No more proccess/ ) () {
#		#EAGAIN, supposedly recoverable fork error
#		sleep 5;
#		redo FORK;
#	    }
	    else {
	  	    #weird fork error
		    die "Can't for: $!\n";
	    }
    }	
&endif
}

#***********************
sub print_FPROT {
#***********************

	if ( $Verbose == 1 )
	{
		my ($outtext) = @_ ;
		my ($sec,$min,$h,$d,$m,$y,$w,$doy,$Ist) ;
		($sec,$min,$h,$d,$m,$y,$w,$doy,$Ist) = localtime (time) ;

		printf LPROT "%02d:%02d:%02d  %s \n", $h,$min,$sec,$outtext ;
		#printf "%02d:%02d:%02d  %s\n", $h,$min,$sec,$outtext ;
	}
}

#***********************
sub start_superu_3 {
#***********************

    post_sema("endloss3.act");
    push @actfiles, "endloss3.act";

&if $OS in [ WIN32 ]
    #Create the process object.
    Win32::Process::Create($Proc8,
	   				 "$SYSTEMROOT\\system32\\cmd.exe",
					 "/c endloss.exe -line 6 -maxcycle $DURCH4 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 3 -maxruntime $MaxRuntime -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD",
					 1,
					 CREATE_DEFAULT_ERROR_MODE,
					 ".") || die Win32::FormatMessage(Win32::GetLastError()); #current dir.
    #Set the process priority
    $Proc7->SetPriorityClass(NORMAL_PRIORITY_CLASS);
&else
    FORK: 
    {
	    if ( $pid = fork() ) 
        {
		    #parent
	    }
	    elsif ( defined($pid) ) 
        {
		    # child
		    my($RC)=system("endloss -line 6 -maxcycle $DURCH4 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 3 -maxruntime $MaxRuntime -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD") ;
            if ( $RC ) 
            { die "Error while executing 'endloss.exe -line 6 -maxcycle $DURCH4 -rows $ROWS -use_cluster $USE_CLUSTER -use_random $use_random -superusernumber 3 -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD': $!\n" ; }
            else
            { exit ; }
	    }
#	elsif ( $! =~ /No more proccess/ ) () {
#		#EAGAIN, supposedly recoverable fork error
#		sleep 5;
#		redo FORK;
#	    }
	    else {
	  	    #weird fork error
		    die "Can't for: $!\n";
	    }
    }
&endif
}

#***********************
sub start_backround_process {
#***********************
    $PROCESS_NAME = $_[0] ;
    @MYARG = split / /, $PROCESS_NAME;
    $PROCESS_NAME_FILE = $MYARG[0] ;

&if $OSSPEC in [WIN32]
&if $RELSTAT!=PROD
    system("start $PROCESS_NAME");
&else
    system("start /B $PROCESS_NAME >> $PROCESS_NAME_FILE.prt 2>&1");
&endif
&else
    system("nohup " . ($ENV{'TOOLSHELL'} ? "$ENV{'TOOLSHELL'}" : "perl") . " $PROCESS_NAME >> $PROCESS_NAME_FILE.prt 2>&1 &");
&endif

}

#*******************************
#           M A I N            #
#*******************************

$start_time=$ARGV[0];
@tmp_ARGV=$ARGV[1];

reset_sema( $MUTSAVE_STARTED ) ;
sleep 1;
start_mut();

open(FILE_OUT, ">.prot"); close FILE_OUT; 	# do NOT remove - may be a symbolic link !!
unlink "initu.prot", <endloss?.prot>, <rendlosA?.prot>, <iendlosA?.prot>, <dump.prt>;
unlink <util*.prot>, <ci*.prot>, <pid*.p*t>;

if ( $Node eq "" || $Node eq "localhost") 
{ system ("x_server" ) ; }

DbUtilExecute("diagnose vtrace default optimize update on");

&if $OS in [ WIN32 ]
DbUtilExecute("SET PARAMETER CHECK_TREE='YES'"); # TS 1999-06-03 Check Index Pages 
DbUtilExecute("SET PARAMETER CHECK_TREE_LOCKS='YES'");  # TS 2003-10-01 Check Prim Page References
&else
DbUtilExecute("SET PARAMETER CHECK_TREE=\\'YES\\'"); # TS 1999-06-03 Check Index Pages 
DbUtilExecute("SET PARAMETER CHECK_TREE_LOCKS=\\'YES\\'");  # TS 2003-10-01 Check Prim Page References
&endif
DbUtilExecute("SET PARAMETER TRACE_SHAREDSQL=3"); 

@perlscripts=();

if ($use_mutsave)
{
	start_backround_process ("mutsave.pl");
	push @perlscripts, "mutsave";
	print "$SELF awaiting start of 'save data' to avoid LOG FULL problem ...\n";

	my $counter = 0;
	do
	{
	        sleep 2;
	        $counter += 1;
	}
	while ( !is_sema_posted ($MUTSAVE_STARTED) && ($counter < 100)) ;

	if (!is_sema_posted ($MUTSAVE_STARTED))
	{
    		print "ERROR! mutsave could not be started!\n";
		goto ABORT_LABEL;
	}
	reset_sema( $MUTSAVE_STARTED ) ;
}
else
{
	print "$SELF without mutsave.pl - 'set log auto overwrite on' to avoid LOG FULL problem ...\n";
	CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_execute set log auto overwrite on", "'set log auto overwrite on' failed");
}

$ENV{HOME} = $CWD;

if ($Node eq "")
{
	$Node = "emptynode";		#if there is no node set, this 'emptynode' is necessary to check in cpp-files
}

if ( ! $INIT_TABLES || init_tables () ) 
{
	system("$TPUT");
	# make the superusers terminate closer to the others
	local($DURCH2) = int ( $DURCH * 75 / 100 + 1 );
	local($DURCH3) = int ( $DURCH * 50 / 100 + 1 );
	local($DURCH4) = int ( $DURCH * 83 / 100 + 1 );
  
	if ($use_dbana)
	{
		start_backround_process ("mutdbana.pl start $DURCH");
		push @perlscripts, "dbana";
	}
	if ($use_adddelvol)
	{
		start_backround_process ("mutAddDelVol.pl $DURCH");
		push @perlscripts, "adddelvol";
	}

	if ($DURCH > 80)
	{
		$use_superu_3 = 1;
	}
	else
	{
		$use_superu_3 = 0;
	}

	if ( $RestartAfterCheckpointAndCrash == 0 )
	{
		start_superu_1() ;
		start_a1_a6() ;
		start_superu_2() ;
		if ($use_superu_3 == 1)
		{
			start_superu_3();
		}
	}
	else
	{
		srand () ;
		$MaxStopAndStart = 10 ;
		for ($StopAndStart = 0 ; $StopAndStart < $MaxStopAndStart ; $StopAndStart++ )
		{	  
			srand () ;
			unlink ('dump.prt') ;
			unlink <pid*.c*t> ;
    
			start_superu_1() ;
			start_a1_a6() ;
			start_superu_2() ;
			if ($use_superu_3 == 1)
			{
				start_superu_3();
			}
  
			if ( $RestartAfterCheckpointAndCrash && ( $StopAndStart < $MaxStopAndStart) && ( ! ( -e 'dump.prt' ) ) )
			{  
				sleep (30+30*$StopAndStart) ;
				if ( -e 'dump.prt' ) {goto ONERROR;} 

#				DbUtilExecute("force checkpoint");
#				sleep (30*$StopAndStart) ;
#				if ( -e 'dump.prt' ) {goto ONERROR;} 
#				system ("x_diag -u $CTRL_USR,$CTRL_PWD -d $DBNAME shutdown quick") ;
#				sleep (10) ;
				system ("x_server stop") ;
				DbStop() ;
				print "$SELF Wait 30 seconds ...\n" ;
				sleep (30) ;
				GetParam("RUNDIRECTORY",\$RUNDIR) ;
				unlink ("$RUNDIR/knltrace.mutold") ;
				rename ("$RUNDIR/knltrace" ,"$RUNDIR/knltrace.mutold") ;
		
				open(FILE_OUT, ">endlos.protold");
				foreach $file ( "initu.prot", <endloss?.prot>, <rendlosA?.prot>, <iendlosA?.prot> ) 
				{
					open(FILE_IN, "<$file");
					print FILE_OUT <FILE_IN>;
					close(FILE_IN);
				}
				close(FILE_OUT);
	    
				open(FILE_OUT, ">endlos.pctold");
				foreach $file ( "<pid*.p*t>" ) 
				{
					open(FILE_IN, "<$file");
					print FILE_OUT <FILE_IN>;
					close(FILE_IN);
					unlink $file ;
				}
				close(FILE_OUT);

				system ("$TPUT") ;
				print "$SELF Start serverdb ...\n" ;
				DbStart() ;
				print "$SELF Start x_server ... \n" ;
				system ("x_server") ;
				sleep (30) ;

				DbUtilExecute("diagnose vtrace default optimize update object on");
&if $OS in [ WIN32 ]        
				DbUtilExecute("SET PARAMETER CHECK_TREE='YES'"); # TS 1999-06-03 Check Index Pages 
				DbUtilExecute("SET PARAMETER CHECK_TREE_LOCKS='YES'");  # TS 2003-10-01 Check Prim Page References
&else
				DbUtilExecute("SET PARAMETER CHECK_TREE=\\'YES\\'"); # TS 1999-06-03 Check Index Pages 
				DbUtilExecute("SET PARAMETER CHECK_TREE_LOCKS=\\'YES\\'");  # TS 2003-10-01 Check Prim Page References
&endif

				print "$SELF Restart serverdb ...\n" ;
				DbUtilExecute("restart");
				unlink ('dump.prt') ;

				if ( int ( rand (4) ) == 0 )
				{
					print_FPROT ( "init tables" ) ;
					init_tables () ;
					if ( -e 'dump.prt' ) {goto ONERROR;}

					print "$SELF Wait 10 seconds again ...\n" ;
					for ( $t = 10 ; $t > 0 ; $t-- )
					{
						sleep (1) ; 
						print $t , " " ;
					}
					print "\n" ;

					system ("$TPUT") ;
				}
			}
		}
		ONERROR:
	}

	sleep (20);
	print "In the background MUT has started - ";
	foreach $perlscript (@perlscripts)
	{
		print "$perlscript - ";
	}
	print "to support the MUT\n";
#	print "\t[except in case of explicit 'no<one-of-the-tool>'-option]\n";
&if $RELSTAT==PROD
	print "Please see corresponding '*.prt'-files.\n";
&else
	print "Please see other windows.\n";
&endif

	if (is_sema_posted($STOPONERROR))
	{
		print "In case of any error with one of the perl scripts MUT will stop the database immediately!\n";
	}

&if $OS in [ WIN32 ]
	if ($use_superu_3 == 0)
	{
		Win32::IPC::WaitForMultipleObjects([ $Proc, $Proc1, $Proc2, $Proc3, $Proc4, $Proc5, $Proc6, $Proc7 ], 1, INFINITE);
	}
	else
	{
		Win32::IPC::WaitForMultipleObjects([ $Proc, $Proc1, $Proc2, $Proc3, $Proc4, $Proc5, $Proc6, $Proc7, $Proc8 ], 1, INFINITE);
	}
&else
	do
	{
		wait;
	}
	while ( $? != -1 ) ;
&endif

	if ( is_db_warm() )
	{
		post_sema("checklo.act");
		push @actfiles, "checklo.act";
		system("checklo -database $DBNAME -node $Node -user SUPERU,SUPERU -dbm $CTRL_USR,$CTRL_PWD -dba $SYSUSR,$SYSPWD > checklo.prot");
	}

	print "\n\n\n\nNow MUT will try to stop all background tasks.\n\n";
}

open(FILE_OUT, ">endlos.prot");

foreach $file ( "initu.prot", <iendlosA?.prot>, <endloss?.prot>, <rendlosA?.prot>, "checklo.prot" ) 
{
	open(FILE_IN, "<$file");
	print FILE_OUT <FILE_IN>;
	close(FILE_IN);
}
close(FILE_OUT);

if ($use_dbana)
{
	@ARGV = ("stop");
	do "mutdbana.pl";
}

ABORT_LABEL:

stop_mut();

reset_sema($STOPONERROR);

call_traceshow();

$db_online=is_db_warm();
if ( !$db_online )
{
	send_error_msg ("mut $DBNAME finished unsuccessfully", $NO_EXIT) ;
	if ( $Node eq "" || $Node eq "localhost") 
	{
		call_xkernprot();
	}
	exit(1);
}
else
{
	my $cnt = 0;
	my $wait_time = 20;
	while(is_sema_posted($MUTSAVE_RUNNING))
	{
		sleep $wait_time;
		$cnt++;
		my $total = $wait_time * $cnt;
		print "\r$SELF Waiting for mutsave to shutdown ... (since $total s)";
	}
	print "\n";

	$count_actfiles=0;
	foreach $actfile (@actfiles)
	{
		if (-e $actfile) 
		{
			print "-- '$actfile' still exists; there is an irregular finished application --\n";
			$count_actfiles++;
		}
	}

	$end_time=time();
	$duration=($end_time-$start_time)/60;
	print "$SELF was started with arguments '@tmp_ARGV'\n";
	printf "$SELF ran '%01d' minutes.\n", $duration;
	print "End of test at ", scalar localtime(),"\n";

	if ($count_actfiles > 0)
	{
		exit($count_actfiles);
	}

# exit only, if a regular mut has finished. No exit after the validation run after restore (muti -r)
        if ( $INIT_TABLES )
        {
	  exit(0);
        }
}
__END__
