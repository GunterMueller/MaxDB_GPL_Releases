
package DB::Simple;
use strict;
use Base::Module;
use Base::Error;

use vars qw($Verbose @ISA);

$Verbose = 0;

my $canUseHTTP = Module::Use('DB::HTTPSQLImpl');
my $canUseDBI  = Module::Use('DB::DBIImpl');

sub new
{
    my ($proto, $href) = @_;
	my $self;
	
	if ($href->{'USE'} =~ /DBI/) {
		$canUseHTTP = 0;
	} elsif ($href->{'USE'} =~ /HTTP/) {
		$canUseDBI = 0;
	}
 	
	unless ($canUseDBI || $canUseHTTP) {
		Error::throw("DATABASE MODULE ERROR", "There is no appropriate module istalled!", $href->{'FATAL_ERRORS'});
		return undef;
	}
	
	if ($canUseDBI) {
		$self = DB::DBIImpl->new($href);
		@ISA = qw(DB::DBIImpl);
	}
	if (!$self && $canUseHTTP) {
		$self = DB::HTTPSQLImpl->new($href);
		@ISA = qw(DB::HTTPSQLImpl);
	}
	if ($self) {
		bless($self);
		return $self;
	} else {
		Error::throw("DATABASE MODULE ERROR", "Can't connect to database '$href->{'NAME'}'!\nPlease check Database NAME, USER, PASSWORD and installed drivers.", $href->{'FATAL_ERRORS'});
		return undef;
	}
}


1;

__END__

=head1 NAME

DB::Simple - An abstract interface for DB calls.

=head1 SYNOPSIS

use strict;
use Base::Module;
use Base::Error;
use Dumpvalue;

$Module::Verbose = 1; # Just to see, which DB-implementation is actually loaded.
my ($canUseDBSimple, $err) = Module::Use('DB::Simple');

die "Can't use DB!\n$err\n" unless ($canUseDBSimple);

my $dbh = DB::Simple->new({'NAME' => 'QADB', 'FATAL_ERRORS' => 0, 'USER' => 'TESTER', 'PASSWORD' => '******'});

$dbh->connect();

my $result = $dbh->fetch('select ID, HOST from SERVERS');

$dbh->disconnect();

my $dumper = new Dumpvalue;
$dumper->dumpValue($result);

=head1 DESCRIPTION

The DB::Abtract module implements an abstract interface for DB connections and
data transfer. It utilizes one of the following implementations: DBI (any
method - internally used are: DBD::MaxDB and DBD::Proxy), HTTPSQL. Additionally,
it utilizes the DB::Connect class, which prvodes predefined connect strings for
default databases (only QADB at the moment).

=head1 CLASS METHODS / VARIABLES

=over 4

=item B<new(HREF)>

This is the constructor for a new DB::Simple object. It takes a hash reference
HREF as single parameter. The usable keys for this hash are described in the 
section "HASH KEYS".

=item B<$Verbose [1|0]>

With this global varaible, one can (de-)activate verbose shell output. 

=back

=head1 INSTANCE METHODS

=over 4

=item B<connect()>

Connects to the database. Takes no parameters. Has no effect, when an
implementation is used that doesn't support sessions.

=item B<disconnect()>

Disconnects from database. Takes no parameters. Has no effect, when an
implementation is used that doesn't support sessions.

=item B<commit()>

Commits all yet uncommited commands. Takes no parameters. Has no effect, when
autocommit() has turned on or an implementation is used that doesn't support
sessions (which implicates autocommit()).

=item B<autocommit(VALUE)>

Steers autocommit function. If a value is given, autocommit will be set to this
value (should be 1 or 0). If no value is given, this function returns the actual
value for autocommit.

=item B<rollback()>

Rolls back all cammands since last commit(). Has no effect when either
autocommit() has been turned on or the implmentation doesn't support sessions.

=item B<send(STATEMENT, FATAL_ERRORS)>

Sends a STATEMENT to the database and doesn't expect a result (equivalent to
DBI::do()). Normally used for DML commands like INSERT or UPDATE.
If FATAL_ERRORS is true, the program will die, if this command fails. If not,
this function will just print out an error message.

NOTE: FATAL_ERRORS has no effect on DBI implementations, jet. 

=item B<fetch(STATEMENT, FATAL_ERRORS)>

Sends a statement to the database and returnes a hash reference which contains
any result rows.

 The result href looks like this:
 
	'ResultDescription' => HASH(0x1bed96c)
		'Col1' => HASH(0x1bed978)
			'Name' => 'ID'
		'Col2' => HASH(0x1bed9fc)
			'Name' => 'HOST'
		'Rowcount' => 4
		'Rows' => HASH(0x1beda20)
			'Row' => ARRAY(0x1beda38)
				0  HASH(0x1beda50)
					'HOST' => 'ds0012'
					'ID' => 145
				1  HASH(0x1beda80)
					'HOST' => 'ds0111'
					'ID' => 53
				2  HASH(0x1bed9c0)
					'HOST' => 'ds0113'
					'ID' => 54
				3  HASH(0x1bedad4)
					'HOST' => 'ds0115'
					'ID' => 1
		'Statement' => 'select ID, HOST from SERVERS'

 Eg.: $result->{'Rows'}->{'Row'}[1]->{'ID'} equals '53'.

=back

=head1 INSTANCE VARIABLES

The only interesting instance variable at the moment would be 'dbh' when using
a DBI implementation. Reference: $handle->{'dbh'} contains a direct reference
to the dbi handle.

=head1 HASH KEYS

=over 4

=item B<USE>

Can be used to force the abstract interface to use a specific implementation.
Values at this moment are: 'DBI' or 'HTTPSQL'.

=item B<NAME>

Describes an abstract database name. The only abstract defined at this moment
is 'QADB'. It actually presents a predefined preset of connection methods and
host name strings.

=item B<NEED_SESSIONS>

This value can either be 1 or 0 and indicated whether you need session or not.
Default value is '0'.

=item B<HAVE_SESSIONS>

This is a read only entry that indicated whether you actually have session
support or not (mostly yes).

=item B<CONNECT_STRINGS>

This is a read only array of connection strings determined from NAME.

=item B<CONNECT_STRING>

Here you can set your own connection string when you're not using a predefined
database (namely QADB). (Should be set a object creation time.)

=item B<USER>

Defines the user to be used when connection to database. Not needed when using
HTML implementation.

=item B<PASSWORD>

...Guess what.

=item B<FATAL_ERRORS>

Sets the global FATAL_ERRORS option. Can be either 1 or 0 - '1' means that the
module will die when an error occours.

=back

=head1 COPYRIGHT

Copyright (c) 2004, SAP AG

