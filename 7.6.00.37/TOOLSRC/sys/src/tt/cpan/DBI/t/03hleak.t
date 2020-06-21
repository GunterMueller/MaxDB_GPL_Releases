#!perl -w

use strict;
use Test;

BEGIN { plan tests => 10 }

use DBI;

my $driver = "ExampleP";

my $drh = DBI->install_driver($driver);
ok($drh);

sub work {
    my (%args) = @_;

    my $dbh = DBI->connect("dbi:$driver:", '', '');

    ok(ref $dbh->{Driver}) if $args{Driver};

    my $sth = $dbh->prepare_cached("select name from ?");

    ok(ref $sth->{Database}) if $args{Database};

    # both handles should be freed here
}

ok($drh->{Kids}, 0);

foreach my $args (
	{},
	{ Driver => 1 },
	{ Database => 1 },
	{ Driver => 1, Database => 1 },
) {

    work( %$args );

    ok($drh->{Kids}, 0);
}


exit 0;
