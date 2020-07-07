package DB::Connect;

use strict;
use Net::HTTP;
use HTTP::Status;

my $BackupData = {	'QADB' => {
						'DBI_SQLDBC' => 'dbi:MaxDB:pts.wdf.sap.corp/QADB',
						'DBI_PROXY1' => 'DBI:Proxy:hostname=ls3001.wdf.sap.corp;port=6666;dsn=DBI:ODBC:pts.wdf.sap.corp:QADB',
						'HTTP_QADB'  => 'pts.wdf.sap.corp:1080:httpsql'
					}
};


sub get_strings {
	my ($name, $type) = @_;
	
	my $content = "";
	
	# Try to read file from production share
	$content = _read_file_by_direct_access(($^O =~ /win32/i ? "\\\\production\\SCM\\MaxDB\\" : "/sapmnt/production/makeresults/SCM/MaxDB/") . "$name.conn.txt");
	# Try to read file from webdav
	$content = _read_file_by_webdav($name) unless ($content);
	# Now try to find a local copy:
	$content = _read_file_by_direct_access("$ENV{'TOOL'}/config/$name.conn.txt") unless ($content);
	# Finally, take backup strings:
	$content = _get_static_backup_data($name) unless ($content);
	
	my @lines = split("\n", $content);
	my @outlines;
	
	foreach my $line (@lines) {
		push @outlines, $1 if ($line =~ /^\[.*$type.*\]=(.*)$/);
	}
	return @outlines;
}

sub _read_file_by_direct_access {
	
	my $fname = shift;
	my $content = "";
	return undef unless (-e $fname);
	if (open(FILE_IN, "< $fname")) {
		while (<FILE_IN>) { $content .= $_; }
	}
	close (FILE_IN);
	return $content;
}

sub _read_file_by_webdav {
	my $name = shift;
	my $webroot     = "webdav/lcmakes/$name.conn.txt";
	my $buf         = "";
	my %head_content;
	
	$head_content{'User-Agent'} = "LCqaCLI/1.0";
	my $dav_req = Net::HTTP->new('Host' => 'pts.wdf.sap.corp', 'PeerPort' => 1080) or return undef;
	$dav_req->write_request('GET', $webroot, %head_content,);
	my ($code, $mess, %headers) = $dav_req->read_response_headers();
	return undef unless (is_success($code));
	$dav_req->read_entity_body($buf, 1000);
	chomp($buf);
	chomp($buf);
	chomp($buf);
	return $buf;
}

sub _get_static_backup_data {
	my $name = shift;
	my $content = "";
	
	foreach my $key (keys %{$BackupData->{$name}}) {
		$content .= "[$key]=$BackupData->{$name}->{$key}\n";
	}
	return $content;
}

1;