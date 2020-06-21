#!perl -w

use strict;
use Test;

use DBI;
use DBI::Profile;

BEGIN {

    if ($DBI::PurePerl) {
	print "1..0 # Skipped: profiling not supported for DBI::PurePerl\n";
	exit 0;
    }

    plan tests => 6;
}

#
#	XXX These tests are very poor!
#

use Data::Dumper;
$Data::Dumper::Indent = 1;
$Data::Dumper::Terse = 1;

my $dbh = DBI->connect("dbi:ExampleP:", '', '', { RaiseError=>1, AutoCommit=>1, });
ok($dbh);

ok(!$dbh->{Profile} && !$ENV{DBI_PROFILE});

$dbh->{Profile} = "4";
my $profile = $dbh->{Profile};

ok(ref $dbh->{Profile}, "DBI::Profile");
ok(ref $profile->{Data}, 'HASH');
ok(ref $profile->{Path}, 'ARRAY');

#warn Dumper($profile);

my $sth = $dbh->prepare("select mode,size,name from ?");
$sth->execute(".");
#$profile->{Path} = [ "foo", DBIprofile_Statement, DBIprofile_MethodName ];
while ( my $hash = $sth->fetchrow_hashref ) {
}
#warn Dumper($profile);

my $logfile = "profile.log";
unlink $logfile;
DBI->trace(0, $logfile);

# --- force DESTROY to force output of profile results
undef $profile;
undef $sth;
undef $dbh;
DBI->trace(0, undef); # force close of log file for Windows

ok(-s $logfile);
unlink $logfile;

exit 0;
