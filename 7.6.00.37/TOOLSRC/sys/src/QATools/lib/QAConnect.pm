
package QAConnect;

use strict;
use Net::HTTP;
use HTTP::Status;
use File::Basename;
use XML::Simple;
use Time::Local;
use Net::Ping;

my $filepath = ($^O =~ /win32/i ? "\\\\production\\SCM\\MaxDB\\" : "/sapmnt/production/makeresults/SCM/MaxDB/");

my %backups;
my $DEBUG = 0;

my %Connect_Strings;

$backups{'QADB_DBI'} = 'DBI:Proxy:hostname=ls3001.wdf.sap.corp;port=51000;dsn=DBI:ODBC:pwdf2125.wdf.sap.corp:QADB';
$backups{'QADB_HTTPSQL'} = 'pwdf2125.wdf.sap.corp:1080:httpsql';

##############################################################################
# get_strings() - retrieves available connection strings.
##############################################################################
sub initialize_strings
{
	my $name = shift;
	my @strs;
	
	unless (-e "$filepath$name.qac")
	{
		print "WARNING: Could not read configuration from server! Using defaults...\n";
		if ($backups{$name})
		{
			push @{$Connect_Strings{$name}}, $backups{$name};
			return 1;
		} elsif ($name =~ /QADB_DBI/) {
			print "Trying to get information from webdav..";
			my $str = getConnectString();
			if ($str ne "")
			{
				print "..OK\n";
				push @{$Connect_Strings{$name}}, $str;
				return 1;
			} else {
				print "..FAILED!\n";
				return 0;
			}
		}
		print "ERROR: No suitable string found!\n";
		return 0;
	}
	unless (open (IN_FILE, "<$filepath$name.qac"))
	{
		print "ERROR: Configuration file '$filepath$name.qac' is not readable!\n";
		if ($backups{$name})
		{
			print "Using backup entry '$backups{$name}'!\n";
			push @{$Connect_Strings{$name}}, $backups{$name};
			return 1;
		}
		return 0;
	}
	while (<IN_FILE>)
	{
		my $line = $_;
		chomp $line;
		push @{$Connect_Strings{$name}}, $line;
	}
	close (IN_FILE);
	return 1;
}

sub get_strings
{
	my $name = shift;
	unless (defined @{$Connect_Strings{$name}}) {
		unless (initialize_strings($name))
		{
			throw_error("FATAL ERROR", "Can't get any connection string!");
			return undef;
		}
	}
	return (wantarray ? @{$Connect_Strings{$name}} : @{$Connect_Strings{$name}}[0]);
}

##############################################################################
# httpsql_request() - executes a SQL statement.
##############################################################################
sub httpsql_request
{
	my $type = shift;
	my $stmt = shift;
	my $fatal_errors = shift;
	my ($outbuf, $tmpbuf, $n) = ("", "", 0);
	my ($qah, $host, $port, $service);
	
	unless (defined @{$Connect_Strings{$type}}) {
		unless (initialize_strings($type))
		{
			throw_error("FATAL ERROR", "Can't get any connection string!", $fatal_errors);
			return (0, undef);
		}
	}
	# Create new HTTP handle for DB proxy connection:
	my $conn_errstr;
	for (my $i=0; $i<10; $i ++) {
		$conn_errstr = "";
		foreach my $str (@{$Connect_Strings{$type}}) {
			($host, $port, $service) = split(/:/, $str);
			$conn_errstr .= "Host: $host, Port: $port, Service: $service\n";
			$qah = Net::HTTP->new('Host' => $host, 'PeerPort' => $port);
			last if($qah);
			if ($DEBUG) {
				print "*** Could not connect to host $host:\n";
				my $pinger = Net::Ping->new();
				my ($ret, $duration, $ip) = $pinger->ping($host, 10);
				if ($ret) {	printf("***    Host $host [ip: $ip] is alive (pingtime %.2f ms)\n", 1000 * $duration); } 
				else      {	print  "***    Host $host is not reachable!\n"; }
			}
		}
        last if ($qah);
		print "*** Connection problem: running out of hosts...going to sleep for 60 secs.\n" if ($DEBUG);
		sleep (60);
	}
	unless ($qah) { $fatal_errors ? throw_error("HTTP CONNECTION ERROR", "Failed to get a connection to QADB over any of the machines after 10 retries:\n$conn_errstr\n", 1) : return (0, undef); }
	if ($qah->write_request('PUT', $service, 'User-Agent' => 'QADBHTTP/1.1', $stmt)) {
		my ($code, $mess, %headers) = $qah->read_response_headers();
		if (is_success($code)) {
			while($qah->read_entity_body($tmpbuf, 1000)) { $outbuf .= $tmpbuf; }
			if ($outbuf =~ /^OK$/i) { return (1, undef); }
			elsif ($outbuf =~ /^</) {
				my $res = eval { XMLin($outbuf, forcearray => ['Row']); };
				if (!$@) { return (1, $res); }
				else {throw_error("XML CONVERSION ERROR", "XML Converter reported an error:\n$@\nXML-Buffer:\n\n$outbuf", $fatal_errors); }
			} else 	{ throw_error("HTTP PROXY ERROR", "HTTP proxy responded with an error:\n$outbuf", $fatal_errors);	}
		} else 		{ throw_error("HTTP RESPONSE ERROR", "HTTP proxy reported error code '$code':\n$mess", $fatal_errors); }
	} else 			{ throw_error("HTTP SEND ERROR", "Error while sending HTTP command to server:\n$@\n$!", $fatal_errors);	}
	return (0, undef);
}


##############################################################################
# throw_error() - Throws an error message and eventually exits.
##############################################################################
sub throw_error
{
    my ($errname, $err_text, $rc) = @_;
	my ($oldpkg, $oldfile, $errstr) = (0, "", "", "");

	for (my $i=0; (caller($i))[0]; $i++)
	{
		if (((caller($i))[0] ne $oldpkg) || ((caller($i))[1] ne $oldfile))
		{
			$errstr .= "\n  Package '" . (caller($i))[0] . "' in file '" . (caller($i))[1] . "':\n\n";
			$oldpkg = (caller($i))[0];
			$oldfile = (caller($i))[1];
		}
		$errstr .= "    Func '" . (caller($i))[3] . "' called in line " . (caller($i))[2] . "\n";
   	}
	my $component = basename($oldfile);
	print "\n**********************************************************\n";
	print "\n$errname: $component reported an error:\n\n";
	print "Info:\n\n$err_text\n\n" if ($err_text);
	print "Function trace:\n";
	print "$errstr\n";
	print "\n**********************************************************\n\n";
	if ($rc)
	{
		print "Exiting...\n";
		exit ($rc ? $rc : 10);
	}
}

sub getConnectString {
	
	my $httpmachine = "pts.wdf.sap.corp";
	my $httpport    = 1080;
	my $webroot     = "webdav/lcmakes/cs.txt";
	my $buf         = "";
	my %head_content;
	
	$head_content{'User-Agent'} = "LCqaCLI/1.0";
	
	my $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) or return undef;
	$dav_req->write_request('GET', $webroot, %head_content,);
	my ($code, $mess, %headers) = $dav_req->read_response_headers();
	return undef unless (is_success($code));
	$dav_req->read_entity_body($buf, 1000);
	chomp($buf);
	chomp($buf);
	chomp($buf);
	return $buf;
}

##############################################################################
# dbgout() - prints out debug output.
##############################################################################
sub dbgout
{
	return 1 unless ($DEBUG);
	my $str = shift;
	my $comp = (caller(1))[3];
	$comp = $comp ? $comp : "main";
	$comp = $1 if ($comp =~ /\S+::(.*)/);
	$comp .= "(" . (caller(0))[2] . ")";
	print ">>DEBUG - $comp: $str\n";
}

sub setdbg
{
	$DEBUG = shift;
}

##############################################################################
# getisotime() - Converts a long to TimeStamp-Format
##############################################################################
sub getisotime {
	my @tlist = localtime(shift);
	return sprintf("'%04d-%02d-%02d %02d:%02d:%02d.000'", $tlist[5] + 1900, $tlist[4] + 1, $tlist[3], $tlist[2], $tlist[1], $tlist[0]);
}

sub isotime2long
{
	my $isotime = shift;
	return timelocal($6, $5, $4, $3, $2 - 1, $1 - 1900) if ($isotime =~ /(\d{4})-(\d{2})-(\d{2})\s(\d{2}):(\d{2}):(\d{2})/);
}


1;

