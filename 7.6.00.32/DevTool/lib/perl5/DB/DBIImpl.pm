package DB::DBIImpl;

use strict;
use Base::Module;
use Base::Error;
use DB::Connect;
use DBI;

sub new {
    my ($proto, $href) = @_;
    my $class = ref($proto) || $proto;
    my $data  = {};

    bless($data, $class);
	
	$data->{'HAVE_SESSIONS'} = 1;
	foreach my $key (keys %{$href}) { $data->{$key} = $href->{$key}; }
	push @{$data->{'CONNECT_STRINGS'}}, DB::Connect::get_strings($data->{'NAME'}, 'DBI');
	unshift @{$data->{'CONNECT_STRINGS'}}, $href->{'CONNECT_STRING'} if ($href->{'CONNECT_STRING'});
	if ($data->connect()) {
		my $ok = $data->send('SELECT count(*) from TABLES');
		$data->disconnect();
		return $data if ($ok);
	}
	return undef;
}

sub connect {
	my $self = shift;
	return undef unless (ref($self));
	foreach my $conn_str (@{$self->{'CONNECT_STRINGS'}}) {
		$self->{'dbh'} = DBI->connect($conn_str, $self->{'USER'}, $self->{'PASSWORD'});
		last if ($self->{'dbh'});
	}
	if ($self->{'dbh'}) {
		$self->autocommit($self->{'AUTO_COMMIT'}) if (defined $self->{'AUTO_COMMIT'});
		return 1;
	} else {
		return 0;
	}
}

sub disconnect {
	my $self = shift;
	return undef unless (ref($self));
	if ($self->{'dbh'}) {
		$self->{'dbh'}->disconnect();
		delete $self->{'dbh'};
	}
	return 1;
}

sub commit {
	my $self = shift;
	return undef unless (ref($self));
	if ($self->{'dbh'}) {
		return $self->{'dbh'}->commit();
	}
	return 0;
}

sub autocommit {
	my ($self, $auto) = @_;
	if (defined $auto) {
		$self->{'AUTO_COMMIT'} = ($auto ? 1 : 0);
		$self->{'dbh'}->{'AutoCommit'} = $self->{'AUTO_COMMIT'} if ($self->{'dbh'});
	}
	return $self->{'AUTO_COMMIT'};
}

sub rollback {
	my $self = shift;
	return undef unless (ref($self));
	if ($self->{'dbh'}) {
		return $self->{'dbh'}->rollback();
	}
	return 0;
}

sub send {
	my ($self, $command, $fatal_errors) = @_;
	return undef unless (ref($self) && $self->{'dbh'});
	return undef unless ($self->{'dbh'});
	return $self->{'dbh'}->do($command);
}

sub fetch {
	my ($self, $command, $fatal_errors) = @_;
	my $res;
	my $row = 0;
	my $first = 1;
	return undef unless (ref($self) && $self->{'dbh'});
	$res->{'Statement'} = $command;
	my $sth = $self->{'dbh'}->prepare($command);
	$sth->execute();
	my $fetch_ref;
	while ($fetch_ref = $sth->fetchrow_hashref()) {
		if ($first) {
			$first = 0;
			my $colcount = 1;
			foreach my $key (keys %{$fetch_ref}) {
				$res->{'ResultDescription'}->{'Col' . $colcount}->{'Name'} = $key;
				$colcount ++;
			}
		}
		foreach my $fetchkey (keys %{$fetch_ref}) {
			$res->{'Rows'}->{'Row'}[$row]->{$fetchkey} = $fetch_ref->{$fetchkey};
		}
		$row ++;
	}
	$res->{'Rowcount'} = $row + 1;
	$sth->finish();
	return $res;
}


1;