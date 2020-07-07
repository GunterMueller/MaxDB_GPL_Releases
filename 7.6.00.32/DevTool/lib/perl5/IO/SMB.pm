#!/usr/bin/perl

#
# allows smb access to nt-server
#

package IO::SMB;

use strict;
use vars qw($VERSION @EXPORT_OK @ISA);
use Symbol;
require SMB;

use Exporter;
@ISA = qw(Exporter);

sub logon {
	SMB::logon (@_);
}

sub new {
	my $self = gensym();
	
	if (defined $_[0]) {
		IO::SMB::sysopen ($self, $_[0]) || return undef;
	}

	bless $self;
	return $self;
}

sub DESTROY {
	my $self = shift;
	$self->close ();
}

sub sysopen {
	my ($self, $filename) = @_;
	return (SMB::sysopen ($self, $filename));
}

sub close {
	my $self = shift;
	return (SMB::close ($self));
}

sub sysread {
	my $self = shift;
	return (SMB::sysread ($self, @_));
}

sub binmode {
	my $self = shift;
	return (SMB::binmode ($self));
}

sub read {
	my $self = shift;
	return (SMB::read ($self, @_));
}

sub getline {
	my $self = shift;
	return (SMB::getline ($self));

}

1;

