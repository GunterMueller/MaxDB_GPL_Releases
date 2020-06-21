#!/usr/local/bin/perl

BEGIN {
	unshift @INC, "$ENV{'TOOL'}/lib/perl5";
	$ENV{'PATH'} .= ";$ENV{'TOOL'}\\pgm" if ($^O =~ /win32/i);
}

use strict;
use Base::Module;
use Getopt::Long;

my %opts;

if(!GetOptions(\%opts, 'm', 'quiet', 'f', 'h', 'lockfile') || !@ARGV || $opts{'h'}) {
	print "\nUsage: killpstree [-m] [-quiet] [-lockfile] <PID> [<PID> ...]\n\n";
	print "Where:     -m     shows verbose module loading information\n";
	print "           -quiet suppresses any console output\n";
	print "           -f     force a hard kill instead of soft terminate\n";
	print "           -lockfile Use a lockfile name instead of a PID\n\n";
	print "NOTE: The return code corresponds to the number of processes failed to kill!\n\n";
	exit(0);
}

print "Killpstree v1.00a, (c)2004 SAP AG\n\n" unless ($opts{'quiet'});

$Module::Verbose = 1 if ($opts{'m'} && !$opts{'quiet'});

my ($useKillTree, $err) = Module::Use('Process::Tree');
print STDERR "Error:$err" if ($err);

$Process::Tree::Verbose = 1 unless ($opts{'quiet'});

my ($killed, $found) = (0,0);
my $islast = 0;
if ($opts{'lockfile'}) {
	if ($^O =~ /win32/i) {
		foreach my $file (@ARGV) {
			$file =~ tr/\//\\/;
			print "Terminating pending processes for file '$file':\n" unless ($opts{'quiet'});
			my @pidlist = getPIDListFromFH($file);
			foreach my $pid (@pidlist) {
				my $proctree = Process::Tree->new();
				my ($tmpkilled, $tmpfound) = (0,0);
				if ($proctree) {
					print "\n" unless ($opts{'quiet'});
				} else {
					print STDERR "Error building process tree!\n\nExiting...\n";
					exit(1);
				}
			
				print ($opts{'f'} ? "Killing tree for PID $pid:\n\n" : "Terminating tree for PID $pid:\n\n") unless ($opts{'quiet'});
				if ($opts{'f'}) {
					($tmpkilled, $tmpfound) = $proctree->KillGroup($pid);
					$killed += $tmpkilled;
					$found += $tmpfound;
				} else {
					($tmpkilled, $tmpfound) = $proctree->TerminateGroup($pid);
					$killed += $tmpkilled;
					$found += $tmpfound;
				}
			}
		}
		
	} else {
        my $rc = 0;
		foreach my $file (@ARGV) {
			$file =~ tr/\\/\//;
			$rc = system("fuser -k $file");
		}
		exit($rc >> 8);
	}
} else {
	foreach my $pid (@ARGV) {
		my $proctree = Process::Tree->new();
		my ($tmpkilled, $tmpfound) = (0,0);
		if ($proctree) {
			print "\n" unless ($opts{'quiet'});
		} else {
			print STDERR "Error building process tree!\n\nExiting...\n";
			exit(1);
		}
	
		print ($opts{'f'} ? "Killing tree for PID $pid:\n\n" : "Terminating tree for PID $pid:\n\n") unless ($opts{'quiet'});
		if ($opts{'f'}) {
			($tmpkilled, $tmpfound) = $proctree->KillGroup($pid);
			$killed += $tmpkilled;
			$found += $tmpfound;
		} else {
			($tmpkilled, $tmpfound) = $proctree->TerminateGroup($pid);
			$killed += $tmpkilled;
			$found += $tmpfound;
		}
	
	}
}

print "\nFound Processes: $found\n" unless($opts{'quiet'});
print "Successfully " . ($opts{'f'} ? "killed" : "terminated") . ": $killed\n\n" unless($opts{'quiet'});

caller () ? return ( $found - $killed ) : exit ( $found - $killed );


sub getPIDListFromFH {
	my $fname = shift;
	my %pids;
	if ($^O =~ /win32/i) {
		open (CMD_OUT, "nthandle $fname |") or (print STDERR "Error opening command 'nthandle'!\n\nExiting...\n" and exit(1));
		while (<CMD_OUT>) {
			if (/pid:\s+(\d+)\s+/) {
				$pids{$1} = 1;
			}
		}
		my @plist;
		push @plist, keys %pids;
		return @plist;
	}
}