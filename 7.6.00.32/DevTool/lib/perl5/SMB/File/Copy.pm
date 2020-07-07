#!/usr/bin/perl

package SMB::File::Copy;

use strict;
use vars qw($VERSION @ISA @EXPORT);

use Exporter;
use SMB;

@ISA = qw (Exporter);
@EXPORT = qw (copy);

sub copy {
	return SMB::copy_from_smb (@_);
}

1;

