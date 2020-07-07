#!/usr/bin/perl

#----------------------------------------------------------------------
# (C) Copyright SAP AG, Berlin 2005
#----------------------------------------------------------------------
# asyncstart.pl
#
# script for starting scripts asynchronously on client hosts.
#
# Prerequisites:
#   Win32 TaskScheduler (Windows only)
#   Ability to use 'at' command (UNIX only)
#----------------------------------------------------------------------

BEGIN {
	if ($^O =~ /win32/i) {
		require Win32;
		import  Win32;
		require Win32::TaskScheduler;
		import  Win32::TaskScheduler;
	}
}

use strict;

use Getopt::Long;
use File::Temp;

my %opts;

if(!GetOptions(\%opts, 'startdir=s', 'user=s', 'password=s', 'taskname=s') || (!@ARGV)) {
print <<HELP_END;

asyncstart v1.00 (c)2005, SAP AG

Usage: asyncstart [-startdir <starting directory>] [-user <username>]
                  [-password <password>] [-taskname <taskname>]
                  command [arguments]

Where:
    -startdir <starting directory>
     Set a start directory where the task is executed in.

    -user <username>
     Sets the user name to be used as task owner (Windows only).
		
    -password <password>
     Sets the password for the user defined above (Windows only).
		
    -taskname <taskname>
     Sets a special name for the task shown in task scheduler (Windows only).

HELP_END
	exit(1);
}

if ($^O =~ /win32/i) {

	############################################################################
	## Windows Part
	############################################################################

	my $scheduler 	= Win32::TaskScheduler->New() or die "Can't create scheduler object!\n";
	my $task 		= ($opts{'taskname'} ? $opts{'taskname'} : substr(File::Temp::tempnam("", 'ASYNCSTART_'), 1));
	my %trigger 	= (	'BeginYear' 	=> (localtime(time))[5] + 1900,
						'BeginMonth'	=> (localtime(time))[4] + 1,
						'BeginDay'		=> (localtime(time))[3],
						'StartHour'		=> (localtime(time))[2],
						'StartMinute'	=> (localtime(time))[1],
						'TriggerType'	=> $scheduler->TASK_TIME_TRIGGER_ONCE);
	my $user = ($opts{'user'} ? $opts{'user'} : getlogin());
	my $fulluser = "SAP_ALL\\$user";
	my $passwd = "";
	if($opts{'password'}) {
		$passwd = $opts{'password'};
	} else {
		my $pwfile = "\\\\production\\info\\make\\$user\\loginpw";
		if (-r $pwfile) {
			if (open(IN_FILE, "<$pwfile")) {
				while(<IN_FILE>) {
					chomp $_;
					$passwd = $_ if($_);
				}
				close (IN_FILE);
			}
		}
	}	
	die "Can't read password for user $user!\n" unless ($passwd);
	
	$scheduler->NewWorkItem($task, \%trigger) or die "Can't create job for task '$task'!\n";
	$scheduler->SetApplicationName(shift @ARGV) or die "Can't set Application!\n";
	$scheduler->SetParameters(join(' ', @ARGV)) or die "Can't set application parameters for task!\n";
	($scheduler->SetWorkingDirectory($opts{'startdir'}) or die "Can't set start directory!\n") if ($opts{'startdir'});
	$scheduler->SetFlags($scheduler->TASK_FLAG_DELETE_WHEN_DONE) or die "Can't set delete flag!\n";
	$scheduler->SetMaxRunTime($scheduler->INFINITE) or die "Can't set maximum run time!\n";
	$scheduler->SetAccountInformation($fulluser, $passwd) or die "Can't set user account information!\n";
	if ($scheduler->Save()) {
		$scheduler->Activate($task) or die "Can't select saved task '$task'!\n";
		$scheduler->Run() or die "Can't run task '$task'!\n";
	} else {
		my $status = 0;
		$scheduler->Activate($task) or die "Can't select saved task '$task'!\n";
		$scheduler->GetStatus($status);
		print "\nTask error:\n" . Win32::FormatMessage($status) . "\n";
		$scheduler->Delete($task) or warn "Warning: Could not delete task '$task'!\n";
	}
	$scheduler->End();
	
} else {

	############################################################################
	## UNIX Part
	############################################################################

	my $task_file = File::Temp::tempnam("/tmp", 'ASYNCSTART_');
	open(OUT_FILE, ">$task_file") or die "Can't open task file '$task_file' for writing!\n";
	print OUT_FILE "cd $opts{'startdir'}; " if ($opts{'startdir'});
	print OUT_FILE "@ARGV\n";
	close OUT_FILE;
	system("at -f $task_file now; rm -f $task_file");
}

exit(0);
