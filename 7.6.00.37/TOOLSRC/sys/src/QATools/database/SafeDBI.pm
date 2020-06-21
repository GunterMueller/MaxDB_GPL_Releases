
package SafeDBI;

use strict;
use DBI;

use vars qw($AUTOLOAD);

sub connect {
    my ($proto, $data_source, $username, $auth, $attr) = @_;
    my $class = ref($proto) || $proto;
    my $data  = {};
    $data->{'data_source'} = $data_source;
    $data->{'username'} = $username;
    $data->{'auth'} = $auth;
    $data->{'attr'} = $attr;
    bless($data, $class);
    $data->_reconnect();
	return $data;
}

sub _reconnect {
    my $self = shift;
    my $count = 0;
    while ($count < 360) {
        $self->{'dbh'} = DBI->connect($self->{'data_source'}, $self->{'username'}, $self->{'auth'}, $self->{'attr'});
        return 1 if ($self->{'dbh'});
        sleep (10);
        $count ++;
    }
    return 0;
}

sub AUTOLOAD {
    my $self = shift;
    my $name = $AUTOLOAD;
    $name =~ s/.*://;   # strip fully-qualified portion
    $self->_reconnect() unless ($self->{'dbh'});
    return $self->{'dbh'}->$name(@_);
}

1;

