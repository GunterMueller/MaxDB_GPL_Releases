#!/usr/bin/perl
#
# ifaconf.pl
#
# helper script to call irconf with $INSTRUNTIME
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


package ifaconf;

sub main {
	my ($path) = @_;

	# remove trailing '/' if found
	$path =~ s/\/$//;


	# remove trailing '/pgm64' if found
	$path =~ s/\/pgm64$//;

	# remove trailing '/lib64' if found
	$path =~ s/\/lib64$//;

	# remove trailing '/pgm' if found
	$path =~ s/\/pgm$//;

	# remove trailing '/lib' if found
	$path =~ s/\/lib$//;

	# convert slashes on win32
	$path =~ s/\//\\/g if ($ENV{OSSPEC} eq 'WIN32');

	# get INSTROOT as value for SAPDBINSTKEY
	my $INSTROOT = $ENV{'INSTROOT'};

	my @cmd = ('irconf', '-i', '-p', $path, '-k', $INSTROOT);
	my $rc = system (@cmd);
	print "error while \'".join (" ", @cmd)."\' : ".$!."\n" if ($rc != 0);
	return ($rc);
}

main (@ARGV);

