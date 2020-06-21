
package Process::TreePurePerlImpl;
use strict;
use Base::Module;

sub new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $data  = {};

    bless($data, $class);

	push @{$data->{'procs'}}, ($^O =~ /win32/i ? _GetProcListWindows() : _GetProcListUNIX());
	return $data;
}

sub DESTROY {
	return undef;
}

sub WhoUsesModule {
	my ($self, $modname) = @_;
	return undef unless(ref($self));
	return undef;
}

sub Terminate {
	my ($self, $pid) = @_;
	return undef unless(ref($self));
	return kill 9, $pid;
}

sub GetPPid {
	my ($self, $pid) = @_;
	return undef unless (ref($self));
	foreach my $proc (@{$self->{'procs'}}) {
		return $proc->{'ppid'} if ($proc->{'pid'} == $pid);
	}
	return undef;
}

sub GetUid {
	my ($self, $pid) = @_;
	return undef unless (ref($self));
	foreach my $proc (@{$self->{'procs'}}) {
		return $proc->{'uid'} if ($proc->{'pid'} == $pid);
	}
	return undef;
}

sub GetCmd {
	my ($self, $pid) = @_;
	return undef unless(ref($self));
	foreach my $proc (@{$self->{'procs'}}) {
		return $proc->{'cmd'} if ($proc->{'pid'} == $pid);
	}
	return undef;
}

sub _GetProcListWindows {
	my @procs;
	my @ppid;
	$ppid[0] = 0;
	
	open (CMD_IN, "tlist -t |") or return undef;
	while (<CMD_IN>) {
		if (/^(\s*)(\S.+)\s\((\d+)\).*$/) {
			my $level = length($1) / 2;
			my $proc = {};
			$proc->{'pid'} = $3;
			$proc->{'ppid'} = $ppid[$level];
			$proc->{'uid'} = undef;
			$proc->{'cmd'} = $2;
			$ppid[$level + 1] = $3;
			push @procs, $proc;
		}
	}
	close (CMD_IN);
	return @procs;
}

sub _GetProcListUNIX {
	my @procs;
	
	open (CMD_IN, "ps -ef |") or return undef;
	while (<CMD_IN>) {
		if (/^\s*(\S+)\s+(\d+)\s+(\d+)\s+\d+\s+\S+\s+\S+\s+\S+\s+(.*)$/) {
			my $proc = {};
			$proc->{'pid'} = $2;
			$proc->{'ppid'} = $3;
			$proc->{'uid'} = $1;
			$proc->{'cmd'} = $4;
			push @procs, $proc;
		}
	}
	close (CMD_IN);
	return @procs;
}

1;
