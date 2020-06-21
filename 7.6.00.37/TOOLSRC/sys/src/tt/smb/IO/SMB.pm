#!/usr/bin/perl
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


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

