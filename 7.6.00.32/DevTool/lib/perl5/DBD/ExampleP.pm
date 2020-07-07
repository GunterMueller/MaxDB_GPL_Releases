{
    package DBD::ExampleP;

    use Symbol;

    use DBI qw(:sql_types);

    @EXPORT = qw(); # Do NOT @EXPORT anything.
    $VERSION = sprintf("%d.%02d", q$Revision: 11.6 $ =~ /(\d+)\.(\d+)/o);

#   $Id: ExampleP.pm,v 11.6 2002/07/15 11:18:57 timbo Exp $
#
#   Copyright (c) 1994,1997,1998 Tim Bunce
#
#   You may distribute under the terms of either the GNU General Public
#   License or the Artistic License, as specified in the Perl README file.

    @statnames = qw(dev ino mode nlink
	uid gid rdev size
	atime mtime ctime
	blksize blocks name);
    @statnames{@statnames} = (0 .. @statnames-1);

    @stattypes = (SQL_INTEGER, SQL_INTEGER, SQL_INTEGER, SQL_INTEGER,
	SQL_INTEGER, SQL_INTEGER, SQL_INTEGER, SQL_INTEGER,
	SQL_INTEGER, SQL_INTEGER, SQL_INTEGER,
	SQL_INTEGER, SQL_INTEGER, SQL_VARCHAR);
    @stattypes{@statnames} = @stattypes;

    $drh = undef;	# holds driver handle once initialised
    $err = 0;		# The $DBI::err value
    #$gensym = "SYM000"; # used by st::execute() for filehandles

    sub driver{
	return $drh if $drh;
	my($class, $attr) = @_;
	$class .= "::dr";
	($drh) = DBI::_new_drh($class, {
	    'Name' => 'ExampleP',
	    'Version' => $VERSION,
	    'Attribution' => 'DBD Example Perl stub by Tim Bunce',
	    }, ['example implementors private data '.__PACKAGE__]);
	$drh;
    }

    sub default_user {
	return ('','');
    }
}


{   package DBD::ExampleP::dr; # ====== DRIVER ======
    $imp_data_size = 0;
    use strict;

    sub connect { # normally overridden, but a handy default
        my($drh, $dbname, $user, $auth)= @_;
        my($this) = DBI::_new_dbh($drh, {
	    'Name' => $dbname,
	    'User' => $user,
	    examplep_get_info => {},
	    });
		$this->STORE(Active => 1);
        $this;
    }

    sub data_sources {
	return ("dbi:ExampleP:dir=.");	# possibly usefully meaningless
    }

    sub disconnect_all {
	# we don't need to tidy up anything
    }
    sub DESTROY { undef }
}


{   package DBD::ExampleP::db; # ====== DATABASE ======
    $imp_data_size = 0;
    use strict;

    sub prepare {
	my($dbh, $statement)= @_;

	my($fields, $dir)
		= $statement =~ m/^\s*select\s+(.*?)\s+from\s+(\S*)/i;
	return $dbh->DBI::set_err(1, "Syntax error in select statement (\"$statement\")")
		unless defined $fields and defined $dir;

	my ($outer, $inner) = DBI::_new_sth($dbh, {
	    'Statement'     => $statement,
	}, ['example implementors private data '.__PACKAGE__]);

	my @fields = ($fields eq '*')
			? keys %DBD::ExampleP::statnames
			: split(/\s*,\s*/, $fields);

	my @bad = map {
	    defined $DBD::ExampleP::statnames{$_} ? () : $_
	} @fields;
	return $dbh->DBI::set_err(1, "Unknown field names: @bad")
		if @bad;

	$inner->{dbd_param} = [];
	@{ $inner->{'dbd_param'} } = ($dir) if $dir !~ /\?/;

	$outer->STORE('NAME' => \@fields);
	$outer->STORE('NULLABLE' => [ (0) x @fields ]);
	$outer->STORE('NUM_OF_FIELDS' => scalar(@fields));
	$outer->STORE('NUM_OF_PARAMS' => ($dir !~ /\?/) ? 0 : 1);
	# should do better here:
	$outer->STORE('SCALE'     => undef);
	$outer->STORE('PRECISION' => undef);

	$outer;
    }


    sub table_info {
	my $dbh = shift;
	my ($catalog, $schema, $table, $type) = @_;

	my @types = split(/["']*,["']/, $type || 'TABLE');
	my %types = map { $_=>$_ } @types;

	# Return a list of all subdirectories
	my $dh = Symbol::gensym(); # "DBD::ExampleP::".++$DBD::ExampleP::gensym;
	my $haveFileSpec = eval { require File::Spec };
	my $dir = $haveFileSpec ? File::Spec->curdir() : ".";
	my @list;
	if ($types{VIEW}) {	# for use by test harness
	    push @list, [ undef, "schema",  "table",  'VIEW', undef ];
	    push @list, [ undef, "sch-ema", "table",  'VIEW', undef ];
	    push @list, [ undef, "schema",  "ta-ble", 'VIEW', undef ];
	    push @list, [ undef, "sch ema", "table",  'VIEW', undef ];
	    push @list, [ undef, "schema",  "ta ble", 'VIEW', undef ];
	}
	if ($types{TABLE}) {
	    no strict 'refs';
	    opendir($dh, $dir)
		or return $dbh->DBI::set_err(int($!),
					"Failed to open directory $dir: $!");
	    while (defined(my $file = readdir($dh))) {
		next unless -d $file;
		my($dev, $ino, $mode, $nlink, $uid) = lstat($file);
		my $pwnam = undef; # eval { scalar(getpwnam($uid)) } || $uid;
		push @list, [ $dir, $pwnam, $file, 'TABLE', undef ];
	    }
	    close($dh);
	}
	# We would like to simply do a DBI->connect() here. However,
	# this is wrong if we are in a subclass like DBI::ProxyServer.
	$dbh->{'dbd_sponge_dbh'} ||= DBI->connect("DBI:Sponge:", '','')
	    or return $dbh->DBI::set_err($DBI::err,
			"Failed to connect to DBI::Sponge: $DBI::errstr");

	my $attr = {
	    'rows' => \@list,
	    'NUM_OF_FIELDS' => 5,
	    'NAME' => ['TABLE_CAT', 'TABLE_SCHEM', 'TABLE_NAME',
		    'TABLE_TYPE', 'REMARKS'],
	    'TYPE' => [DBI::SQL_VARCHAR(), DBI::SQL_VARCHAR(),
		    DBI::SQL_VARCHAR(), DBI::SQL_VARCHAR(), DBI::SQL_VARCHAR() ],
	    'NULLABLE' => [1, 1, 1, 1, 1]
	};
	my $sdbh = $dbh->{'dbd_sponge_dbh'};
	my $sth = $sdbh->prepare("SHOW TABLES FROM $dir", $attr)
	    or return $dbh->DBI::set_err($sdbh->err(), $sdbh->errstr());
	$sth;
    }


    sub type_info_all {
	my ($dbh) = @_;
	my $ti = [
	    {	TYPE_NAME	=> 0,
		DATA_TYPE	=> 1,
		COLUMN_SIZE	=> 2,
		LITERAL_PREFIX	=> 3,
		LITERAL_SUFFIX	=> 4,
		CREATE_PARAMS	=> 5,
		NULLABLE	=> 6,
		CASE_SENSITIVE	=> 7,
		SEARCHABLE	=> 8,
		UNSIGNED_ATTRIBUTE=> 9,
		FIXED_PREC_SCALE=> 10,
		AUTO_UNIQUE_VALUE => 11,
		LOCAL_TYPE_NAME	=> 12,
		MINIMUM_SCALE	=> 13,
		MAXIMUM_SCALE	=> 14,
	    },
	    [ 'VARCHAR', DBI::SQL_VARCHAR, 1024, "'","'", undef, 0, 1, 1, 0, 0,0,undef,0,0 ],
	    [ 'INTEGER', DBI::SQL_INTEGER,   10, "","",   undef, 0, 0, 1, 0, 0,0,undef,0,0 ],
	];
	return $ti;
    }


    sub disconnect {
	shift->STORE(Active => 0);
	return 1;
    }


    sub get_info {
	my ($dbh, $info_type) = @_;
	return $dbh->{examplep_get_info}->{$info_type};
    }


    sub FETCH {
	my ($dbh, $attrib) = @_;
	# In reality this would interrogate the database engine to
	# either return dynamic values that cannot be precomputed
	# or fetch and cache attribute values too expensive to prefetch.
	# else pass up to DBI to handle
	return $INC{"DBD/ExampleP.pm"} if $attrib eq 'example_driver_path';
	return $dbh->SUPER::FETCH($attrib);
    }


    sub STORE {
	my ($dbh, $attrib, $value) = @_;
	# would normally validate and only store known attributes
	# else pass up to DBI to handle
	if ($attrib eq 'AutoCommit') {
	    # convert AutoCommit values to magic ones to let DBI
	    # know that the driver has 'handled' the AutoCommit attribute
	    $value = ($value) ? -901 : -900;
	}
	return $dbh->SUPER::STORE($attrib, $value);
    }

    sub DESTROY {
	my $dbh = shift;
	$dbh->disconnect if $dbh->FETCH('Active');
	undef
    }


    # This is an example to demonstrate the use of driver-specific
    # methods via $dbh->func().
    # Use it as follows:
    #   my @tables = $dbh->func($re, 'examplep_tables');
    #
    # Returns all the tables that match the regular expression $re.
    sub examplep_tables {
	my $dbh = shift; my $re = shift;
	grep { $_ =~ /$re/ } $dbh->tables();
    }
}


{   package DBD::ExampleP::st; # ====== STATEMENT ======
    $imp_data_size = 0;
    use strict; no strict 'refs'; # cause problems with filehandles

    my $haveFileSpec = eval { require File::Spec };

    sub bind_param {
	my($sth, $param, $value, $attribs) = @_;
	$sth->{'dbd_param'}->[$param-1] = $value;
	return 1;
    }


    sub execute {
	my($sth, @dir) = @_;
	my $dir;

	if (@dir) {
	    $sth->bind_param($_, $dir[$_-1]) or return
		foreach (1..@dir);
	}

	my $dbd_param = $sth->{'dbd_param'} || [];
	return $sth->DBI::set_err(2, @$dbd_param." values bound when 1 expected")
	    unless @$dbd_param == 1;

	$dir = $dbd_param->[0];
	return $sth->DBI::set_err(2, "No bind parameter supplied")
	    unless defined $dir;

	$sth->finish;

	#
	# If the users asks for directory "long_list_4532", then we fake a
	# directory with files "file4351", "file4350", ..., "file0".
	# This is a special case used for testing, especially DBD::Proxy.
	#
	if ($dir =~ /^long_list_(\d+)$/) {
	    $sth->{dbd_dir} = [ $1 ];	# array ref indicates special mode
	    $sth->{dbd_datahandle} = undef;
	}
	else {
	    $sth->{dbd_dir} = $dir;
	    $sth->{dbd_datahandle} = Symbol::gensym(); # "DBD::ExampleP::".++$DBD::ExampleP::gensym;
	    opendir($sth->{dbd_datahandle}, $dir)
		or return $sth->DBI::set_err(2, "opendir($dir): $!");
	}
	$sth->STORE(Active => 1);
	return 1;
    }


    sub fetch {
	my $sth = shift;
	my $dh  = $sth->{dbd_datahandle};
	my $dir = $sth->{dbd_dir};
	my %s;

	if (ref $dir) {		# special fake-data test mode
	    my $num = $dir->[0]--;
	    unless ($num > 0) {
		$sth->finish();
		return;
	    }
	    my $time = time;
	    @s{@DBD::ExampleP::statnames} =
		( 2051, 1000+$num, 0644, 2, $>, $), 0, 1024,
	          $time, $time, $time, 512, 2, "file$num")
	}
	else {			# normal mode
	    my $f = readdir($dh);
	    unless ($f) {
		$sth->finish;
		return;
	    }
	    my $file = $haveFileSpec
		? File::Spec->catfile($dir, $f) : "$dir/$f";
	    # put in all the data fields
	    @s{ @DBD::ExampleP::statnames } = (lstat($file), $f);
	}

	# return just what fields the query asks for
	my @new = @s{ @{$sth->{NAME}} };

	return $sth->_set_fbav(\@new);
    }
    *fetchrow_arrayref = \&fetch;


    sub finish {
	my $sth = shift;
	closedir($sth->{dbd_datahandle}) if $sth->{dbd_datahandle};
	$sth->{dbd_datahandle} = undef;
	$sth->{dbd_dir} = undef;
	$sth->SUPER::finish();
	return 1;
    }


    sub FETCH {
	my ($sth, $attrib) = @_;
	# In reality this would interrogate the database engine to
	# either return dynamic values that cannot be precomputed
	# or fetch and cache attribute values too expensive to prefetch.
	if ($attrib eq 'TYPE'){
	    my @t = @DBD::ExampleP::stattypes{ @{ $sth->{NAME} } };
	    return \@t;
	}
	elsif ($attrib eq 'ParamValues') {
	    my $dbd_param = $sth->{dbd_param} || [];
	    my %pv = map { $_ => $dbd_param->[$_-1] } 1..@$dbd_param;
	    return \%pv;
	}
	# else pass up to DBI to handle
	return $sth->SUPER::FETCH($attrib);
    }


    sub STORE {
	my ($sth, $attrib, $value) = @_;
	# would normally validate and only store known attributes
	# else pass up to DBI to handle
	return $sth->{$attrib} = $value
	    if $attrib eq 'NAME' or $attrib eq 'NULLABLE' or $attrib eq 'SCALE' or $attrib eq 'PRECISION';
	return $sth->SUPER::STORE($attrib, $value);
    }

    sub DESTROY { undef }
}

1;