#!/usr/bin/perl
#
# $Header: //sapdb/TOOLSRC/develop/sys/src/tt/cpan/SAPDB/Install/PipeExec.pl#2 $
# $DateTime: 2002/04/25 11:10:12 $  
# $Change: 19899 $
#
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


package SAPDB::Install::PipeExec;

sub new {
	my $self = {};
	bless $self;

	$self->{'sess'} = 0;
	$self->{'errtext'} = undef;
	$self->{'sess'} = newXS ();
	return ($self);
}

sub DESTROY {
	my ($self) = @_;

	return unless (defined $self->{'sess'});
	return if ($self->{'sess'} == 0);
	CloseXS ($self->{'sess'});
	$self->{'sess'} = 0;
	delete $self->{'errtext'};
	return;
}

sub Open {
	my ($self, $cmdline) = @_;
	
	unless (defined OpenXS ($self->{'sess'}, $cmdline)) {
		$self->{'errtext'} = GetErrorXS ($self->{'sess'});
		return undef;
	} 
	return 0;
}

sub Close {
	my ($self) = @_;
	my $rc;	

	return undef unless (defined $self->{'sess'});
	return 0 if ($self->{'sess'} == 0);

	$rc = CloseXS ($self->{'sess'});

	$self->{'errtext'} = undef;
	$self->{'sess'} = 0;

	return $rc;
}

sub Readline {
	my ($self) = @_;
	
	return undef unless (defined $self->{'sess'});
	return 0 if ($self->{'sess'} == 0);

	my $buffer = ReadlineXS ($self->{'sess'});
	$self->{'errtext'} = GetErrorXS ($self->{'sess'});
	return undef if (defined $self->{'errtext'});

	return ($buffer)
}

sub GetError {
	my ($self) = @_;
	return ($self->{'errtext'});
}

#
# internal helper for unix cmd line splitting
#
sub PrepareArgv {
	my ($cmd) = @_;
	my @argv = SplitArgv ($cmd);
	my $file = shift (@argv);

	if ($file =~ /\//) {
		return ($file.': '.$!) unless (-x $file);
		return ('', $file, @argv);
	}

	foreach my $path (split (':', $ENV{'PATH'})) {
		if (-x $path.'/'.$file) {
			$file = $path.'/'.$file;
			return ('', $file, @argv);
		}
	}
	return $file.': No such command';
}

sub SplitArgv {
	my ($cmdline, @argv) = @_;

	if (not defined $cmdline || $cmdline eq "") {
		return (@argv);
	}

	# doublequoted argument
	if ($cmdline =~ s/^\s*\"(.*?)\"//) {
		push @argv, $1;
		return (SplitArgv ($cmdline, @argv));
	}

	# argument surrunded with withespaces
	if ($cmdline =~ s/^\s*(.+?)\s+//) {
		push @argv, $1;
		return (SplitArgv ($cmdline, @argv));
	}

	# last argument 
	if ($cmdline =~ s/^\s*(.+)//) {
		push @argv, $1;
		return (SplitArgv ($cmdline, @argv));
	}

	return (@argv);
}
