
package Process::Tree;
use strict;
use Base::Module;

use vars qw($Verbose @ISA);

$Verbose = 0;

my $useVersion = (Module::Use('SAPDB::Install::ProcState') == 1 ? 'ProcState' : (Module::Use('Process::TreePurePerlImpl') == 1 ? 'PurePerl' : 'none'));

sub new
{
	my $self = _getTree();
	return undef unless ($self);
	bless $self;
	return $self;
}

sub Kill {
	my ($self, $pid) = @_;
	return undef unless (ref($self));
	return kill(9, $pid);
}

sub WaitTerminate {
	my ($self, $pid, $seconds) = @_;
	return undef unless (ref($self));
	my $found = 0;

	while ($seconds) {
		$seconds --;
		$found = 0;
		my $tmp_tree = _getTree();
		foreach my $test_proc (@{$tmp_tree->{'procs'}}) {
			if ($test_proc->{'pid'} == $pid) {
				$found = 1;
				last;
			}
		}
		last unless ($found);
		sleep(1);
	}
	return ($found ? 0 : 1);
}

sub TerminateGroup {
	my ($self, $pid) = @_;
	return undef unless (ref($self));
	return $self->_stopGroup($pid, 0);
}

sub KillGroup {
	my ($self, $pid) = @_;
	return undef unless (ref($self));
	return $self->_stopGroup($pid, 1);
}

sub _getTree {
	if ($useVersion eq 'ProcState') {
		@ISA = qw(SAPDB::Install::ProcState);
		return SAPDB::Install::ProcState->new();
	} elsif ($useVersion eq 'PurePerl') {
		@ISA = qw(Process::TreePurePerlImpl);
		return Process::TreePurePerlImpl->new();
	} else {
		return undef;
	}
}

sub _stopGroup {
	my ($self, $pid, $use_kill, $ppid) = @_;
	return undef unless (ref($self) && $pid);
	my $find_counter = 0;
	my $kill_counter = 0;
	my @kids;
	my $tree = _getTree();
	
	foreach my $proc (@{$tree->{'procs'}}) {
		if ($proc->{'pid'} == $pid) {
			@kids = grep {$_->{'ppid'} == $pid} @{$tree->{'procs'}};
			my $killword = ($use_kill ? "Killing" : "Terminating");
			my $kill_ok;
			print ($ppid ? "$killword child process $pid '$proc->{'cmd'}' (Parent: $ppid).." : "$killword process $pid '$proc->{'cmd'}'..") if ($Verbose);
			if ($use_kill) {
				$kill_ok = $self->Kill($pid);
			} else {
				$self->Terminate($pid);
				$kill_ok = $self->WaitTerminate($pid, 3);
			}
			if ($kill_ok) {
				print "..OK\n" if ($Verbose);
				$kill_counter ++;
			} else {
				print "..FAILED\n" if ($Verbose);
			}
			$find_counter ++;
			last;
		}
	}
	
	foreach my $kid (@kids) {
		my ($kill_cnt, $find_cnt) = $self->_stopGroup($kid->{'pid'}, $use_kill, $pid);
		$kill_counter += $kill_cnt;
		$find_counter += $find_cnt;
	}
	
	unless ($ppid) {
		$tree = _getTree();
		undef @{$self->{'procs'}};
		push @{$self->{'procs'}}, @{$tree->{'procs'}};
	}
	
	return (wantarray ? ($kill_counter, $find_counter) : ($find_counter - $kill_counter));
}

1;

__END__

=head1 NAME

Process::Tree - Module for reading / killing process trees.

=head1 SYNOPSIS

 use Base::Module;

 my $include_ok = Module::Use('Process::Tree') or die "Can't include process tree module!\n";

 $Process::Tree::Verbose = 1; # To see console output for each task

 my $proctree = Process::Tree->new() or die "Can't get process tree!\n";

 # To list all processes:
 foreach my $process (@{$proctree->{'procs'}}) {
     print "PID: $process->{'pid'}, NAME: $process->{'cmd'}, PARENT: $process->{'ppid'}\n";
 }

 # To terminate a process softly:
 $proctree->Terminate($pid);

 # To kill a complete process group (hard):
 $proctree->KillGroup($start_pid);

 # To release tree:
 undef $proctree;

=head1 DESCRIPTION

The Process::Tree module implements a unified interface to handle process trees
directly from perl. In addition to its underlaying base modules, it also supports
(soft and hard) killing of complete process groups (which is normally not supported
by windows). To use this module, you have to install either the pure-perl
implementation for process trees (Process::TreePurePerlImpl) or the native version
(SAPDB::Install::ProcState). Either of these modules is then loaded dynamically
when creating an instance of this module (SAPDB::Install::ProcState is default). 

=head1 CLASS METHODS / VARIABLES

=over 4

=item B<new()>

This is the constructor for a new Process::Tree object. It takes no parameters
and instantly builds up a snapshot of the actual process tree.

=item B<$Verbose [1|0]>

With this global varaible, one can (de-)activate verbose shell output. When
activated will give detailed information about every process when terminating
a process group.

=back

=head1 INSTANCE METHODS

=over 4

=item B<Terminate(PID)>

Softly terminates a single process via TERM signal. This function is only fully
functional when using the SAPDB::Install::ProcState module. In pure perl mode,
this function will be mapped to the Kill() function.

NOTE: Since this function only uses system signals to terminate a process, there
will be no appropriate return code to check. In order to do this, one will have
to re-create a new process tre and look, if the terminated task is missing.

=item B<Kill(PID)>

Kills a single process the hard way by sending the KILL signal. This function is
supported by ALL implementations / platforms.

Returns the number of successfully killed processes.

=item B<TerminateGroup(PID)>

Terminates a complete process group including all child processes, starting off
with the given PID. This function anternally calls the Terminate() function
to terminate the single processes.

Returns an array with two values: The number of actually killed processes and
the number of overall found processes to kill. If these two values differ from
each other, some processes could not be killed.

If called in scalar context, it simply returns the number of processes that
could not be killed (ideally 0).

=item B<KillGroup(PID)>

Same function as above with the only difference that it uses the Kill() function
instead of Terminate(). In other words: It kills a complete process group the
hard way.

Also returns either an array ($killed_procs, $total_found_procs) or a scalar
$failed_to_kill.

=item B<WaitTerminate(PID, SECONDS)>

Waits for a process to vanish for the given number of seconds. If a process
terminated within the given time, this function returns a '1', otherwise '0'.

=item B<WhoUsesModule(MOD_NAME)>

Retruns the process PID's that use a given module.

NOTE: This function only works UNIX systems with the SAPDB::Install::ProcState
extension in place. For Windows systems, this functionality ist diabled since
there's still a problem with freezing cygwin-processes.

=item B<GetPPid(PID)>

Returns the parent PID of a given process.

=item B<GetUid(PID)>

Returns the user id for a given PID.

NOTE: In order to get this work under Windows, you have to have the
SAPDB::Install::ProcState extension installed.

=item B<GetCmd(PID)>

Returns the command string for a given PID.


=back

=head1 INSTANCE VARIABLES

The only viewable object variable is '$obj->{'procs'}'. This is actually a list
of process hash entries. Each process entry itself has four scalar entries:
'pid', 'ppid', 'uid' and 'cmd'. For a better explanation, tak a look at the
example below:

 # This example dumps all running processes like unix 'ps' would do:
 
 my $proctree = Process::Tree->new();
 
 foreach my $proc (@{$proctree->{'procs'}}) {
	print "$proc->{'uid'} $proc->{'pid'} $proc->{'ppid'} $proc->{'cmd'}\n";
 }

=head1 COPYRIGHT

Copyright (c) 2004, SAP AG

