#!/usr/bin/perl
#
# igme.pl
# cba 2000/07/14
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

use igme;

sub main {
	local @ARGV = @_;

	my $outfile = undef;
	my $skelfile = undef;
	my $os = $^O;
	my $private = undef;
	my $target = undef;
	my $ignoreSourceServer = undef;
	my $noSceleton = undef;

	@_ = ();

	while ($#ARGV >= 0) {
		$_ = shift @ARGV;
		if ($_ =~ /^-/) {
			if ($_ eq "-l") {
				$target = shift @ARGV;
				if (igme::list ($target) == 0) {
					print "could not list release names\n";
				}
				return;
			} elsif ($_ eq "-c") {
				$target = shift @ARGV;
				if (igme::cat ($target) == 0) {
					print "unknown target ".$target."\n"
				}
				return;
			} elsif ($_ eq "-p") {
				$private = shift @ARGV;
			} elsif ($_ eq "-x") {
				$ignoreSourceServer = "wanted";
			} elsif ($_ eq "-f") {
				$outfile = shift @ARGV;
			} elsif ($_ eq "-s") {
				$skelfile = shift @ARGV;
			} elsif ($_ eq "-o") {
				$os = shift @ARGV;
			} elsif ($_ eq "-nos") {
				$noSceleton = "WANTED";
			} else {
				shift;
			}
		} else {
			$target = $_;
		}
	}

	return unless (defined $ENV{OWN});
	return if ($ENV{OWN} eq "");

	$outfile = $ENV{OWN}."/.iprofile" if (!defined $outfile);

	unless (defined $noSceleton ) {
		$skelfile = $ENV{OWN}."/.iprofile.skel" if (!defined $skelfile);
	}

	push @_, "-o", $os;
	push @_, "-f", $outfile if (defined $outfile);
	push @_, "-s", $skelfile if (defined $skelfile);
	push @_, "-p", $private if (defined $private);
	push @_, "-x" if (defined $ignoreSourceServer);
	push @_, $target if (defined $target);

	print "could not set vmake path for ".$target."\n"
		if (igme::igme (@_) == 0);
}

main (@ARGV);

