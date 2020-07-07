#!/usr/bin/perl
# 
# SMB.pm
#
package SMB;

require Exporter;
require DynaLoader;
use Symbol;

@ISA = ("Exporter", "DynaLoader");

bootstrap SMB;

sub logon {
	my ($user, $pass, $domain) = @_;

	if ($user =~ /^\\\\(w+)\\(w+)$/g) {
		$domain = $1;
		$user = $2;
	}

	return (SMB::logon_to_smb ($user, $pass, $domain))
		unless ($pass eq "*");

	my @logon_param = get_passwd ($user);
	return (SMB::logon_to_smb (@logon_param))
		if (@logon_param == 2 || @logon_param == 3);

	return 0;
}

sub get_passwd {
	my $user = shift;
	my $smbpass_filename = $ENV{"HOME"}."/.smbpass";	
	my @pass_ent;

	my $fh = gensym ();
	die ("SMB::logon(): could not open smbpass file\n")
		unless (open ($fh, $smbpass_filename));

	while (<$fh>) {
		@pass_ent = split (' ', $_);
		if ($pass_ent[0] =~ /\\\\(\w+)\\(\w+)/g) {
			if ($2 eq $user) {
				close $fh;
				return ($user, $pass_ent[1], $1);
			}
		} elsif ($pass_ent[0] =~ /(\w+)/g) {
			if ($1 eq $user) {
				close $fh;
				return ($user, $pass_ent[1]);
			}
		}
	}

	close $fh;
	return ();
}

1;
