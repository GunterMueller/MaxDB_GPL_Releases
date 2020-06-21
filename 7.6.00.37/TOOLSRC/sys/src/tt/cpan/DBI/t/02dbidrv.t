#!perl -w

use DBI;
use Test;
use strict;

BEGIN {
	plan tests => 22;
}

$|=1;
$^W=1;
my $drh;

{   package DBD::Test;
    use strict;

    $drh = undef;	# holds driver handle once initialised

    sub driver{
	return $drh if $drh;
	main::ok(1);		# just getting here is enough!
	my($class, $attr) = @_;
	$class .= "::dr";
	($drh) = DBI::_new_drh($class, {
		'Name' => 'Test',
		'Version' => '$Revision: 11.5 $',
	    },
	    77	# 'implementors data'
	    );
	main::ok($drh);
	$drh;
    }
}

{   package DBD::Test::dr;
    use strict;
    $DBD::Test::dr::imp_data_size = 0;
    $DBD::Test::dr::imp_data_size = 0;	# avoid typo warning

    sub disconnect_all { undef }
    sub DESTROY { undef }

    sub data_sources {	# just used to run tests 'inside' a driver
	my $h = shift;
	print "DBD::_::dr internals\n";
	main::ok($h);
	main::ok(!tied $h);
    }
}

{   package DBD::Test::db;
    use strict;
    $DBD::Test::db::imp_data_size = 0;
    $DBD::Test::db::imp_data_size = 0;	# avoid typo warning

    sub DESTROY { undef }

    sub do {	# just used to run tests 'inside' a driver
	my $h = shift;
	print "DBD::_::db internals\n";

	main::ok($h);
	main::ok(!tied $h);

	#$h->trace(9);

	print "Driver for inner handles needs to be the Drivers inner handle\n";
	my $drh_i = $h->{Driver};
	main::ok($drh_i);
	main::ok(ref $drh_i);
	main::ok(!tied %$drh_i);

	print "Driver for outer handles needs to be the Drivers outer handle\n";
	my $drh_o = $h->FETCH('Driver');
	main::ok($drh_o);
	main::ok(ref $drh_o);
	main::ok(tied %$drh_o) unless $DBI::PurePerl && main::ok(1);

	#$h->trace(0);
    }
}

$INC{'DBD/Test.pm'} = 'dummy';	# fool require in install_driver()

# Note that install_driver should *not* normally be called directly.
# This test does so only because it's a test of install_driver!
$drh = DBI->install_driver('Test');
ok($drh);

ok(DBI::_get_imp_data($drh), 77);

$drh->data_sources;	# trigger driver internal tests above

my $dbh = $drh->connect;

$dbh->do('dummy');		# trigger more driver internal tests above

$drh->set_err("99", "foo");
ok($DBI::err, 99);
ok($DBI::errstr, "foo");

$drh->set_err(0, "00000");
ok($DBI::state, "");

$drh->set_err(1, "test error");
ok($DBI::state, "S1000");

$drh->set_err(1, "test error", "IM999");
ok($DBI::state, "IM999");

eval { $DBI::rows = 1 };
ok($@ =~ m/Can't modify/);

ok($drh->{FetchHashKeyName}, 'NAME');
$drh->{FetchHashKeyName} = 'NAME_lc';
ok($drh->{FetchHashKeyName}, 'NAME_lc');


exit 0;
