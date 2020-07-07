package DB::HTTPSQLImpl;

use strict;
use Base::Module;
use Base::Error;
use DB::Connect;

use Net::HTTP;
use HTTP::Status;
use XML::Simple;

sub new {
    my ($proto, $href) = @_;
    my $class = ref($proto) || $proto;
    my $data  = {};

    bless($data, $class);
	
	if ($href) {
		return undef if ($href->{'NEED_SESSIONS'});
	}
	$data->{'HAVE_SESSIONS'} = 0;
	foreach my $key (keys %{$href}) { $data->{$key} = $href->{$key}; }
	push @{$data->{'CONNECT_STRINGS'}}, DB::Connect::get_strings($data->{'NAME'}, 'HTTP');
	unshift @{$data->{'CONNECT_STRINGS'}}, $href->{'CONNECT_STRING'} if ($href->{'CONNECT_STRING'});
	if ($data->send('select count(*) from TABLES', 0)) {
		return $data;
	} else {
		return undef;
	}
}

sub connect {
	return 1;
}

sub disconnect {
	return 1;
}

sub commit {
	return 1;
}

sub autocommit {
	return 1;
}

sub rollback {
	return 0;
}

sub send {
	my ($self, $command, $fatal_errors) = @_;
	return undef unless (ref($self));
	my ($rc, $dummy) = $self->_do_request($command, $fatal_errors);
	return $rc;
}

sub fetch {
	my ($self, $command, $fatal_errors) = @_;
	return undef unless (ref($self));
	my ($rc, $result) = $self->_do_request($command, $fatal_errors);
	return ($rc ? $result : undef);	
}

sub _do_request
{
	my ($self, $stmt, $fatal_errors) = @_;
	return undef unless (ref($self));
	my ($outbuf, $tmpbuf, $n) = ("", "", 0);
	my ($qah, $host, $port, $service);
	
	$fatal_errors = (defined $fatal_errors ? $fatal_errors : ((defined $self->{'FATAL_ERRORS'}) ? $self->{'FATAL_ERRORS'} : 0));
	
	unless (@{$self->{'CONNECT_STRINGS'}}) {
			Error::throw("FATAL ERROR", "Can't get any connection string!", $fatal_errors);
			return (0, undef);
	}
	# Create new HTTP handle for DB proxy connection:
	my $conn_errstr = "";
	foreach my $str (@{$self->{'CONNECT_STRINGS'}}) {
		($host, $port, $service) = split(/:/, $str);
		$conn_errstr .= "Host: $host, Port: $port, Service: $service\n";
		$qah = Net::HTTP->new('Host' => $host, 'PeerPort' => $port);
		last if ($qah);
	}
	Error::throw("HTTP CONNECTION ERROR", "Failed to connect to any of these machines:\n$conn_errstr", $fatal_errors) unless ($qah);
	if ($qah->write_request('PUT', $service, 'User-Agent' => 'MAXDBHTTP/1.1', $stmt)) {
		my ($code, $mess, %headers) = $qah->read_response_headers();
		if (is_success($code)) {
			while($qah->read_entity_body($tmpbuf, 1024)) { $outbuf .= $tmpbuf; }
			if ($outbuf =~ /^OK$/i) { return (1, undef); }
			elsif ($outbuf =~ /^</) {
				my $res = eval { XMLin($outbuf, forcearray => ['Row']); };
				if (!$@) { return (1, $res); }
				else {Error::throw("XML CONVERSION ERROR", "XML Converter reported an error:\n$@\nXML-Buffer:\n\n$outbuf", $fatal_errors); }
			} else 	{ Error::throw("HTTP PROXY ERROR", "HTTP proxy responded with an error:\n$outbuf", $fatal_errors);	}
		} else 		{ Error::throw("HTTP RESPONSE ERROR", "HTTP proxy reported error code '$code':\n$mess", $fatal_errors); }
	} else 			{ Error::throw("HTTP SEND ERROR", "Error while sending HTTP command to server:\n$@\n$!", $fatal_errors);	}
	return (0, undef);
}

1;