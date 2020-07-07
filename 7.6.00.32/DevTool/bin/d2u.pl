#!/usr/bin/perl
#
# @(#)d2u  6.2  1997-10-01
#
# G. Gro�mann, SAP AG
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

package d2u;

$USAGE = "usage: d2u files\n";
if ( @ARGV == 0 ) { print $USAGE; exit 1 };

$SIG{'INT'} = 'IGNORE';
$tmpfile = "d2u.tmp";
foreach $file (@ARGV) {

	if ( -f $file && -T $file) {
		open(FILE_IN, "<$file") or do { print STDERR "Can't open $file\n"; next };
		binmode FILE_IN;
		open(FILE_OUT, "+>$tmpfile") or die "Can't open $tmpfile\n";
		binmode FILE_OUT;
		my($char1, $char2);
		while(<FILE_IN>) {
			$char1 = chop; $char2 = chop;
			if ( ord($char1) != 10 or ord($char2) != 13) {
				$_ .= $char2.$char1;
			} else {
				$_ .= chr(10) }
			print FILE_OUT $_;
		}
		close FILE_IN; close FILE_OUT;
		unlink $file; rename $tmpfile, $file;
	}
	else { print STDERR "$file seems to be a binary or doesn't exist!\n" }
}

END { unlink $tmpfile }


__END__
