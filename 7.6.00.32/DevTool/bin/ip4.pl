#  -----------------------------------------------------------------------------
#
#  module: ip4.pl
#
#  -----------------------------------------------------------------------------
#
#  special area: Development Environment
#
#  description:  perforce edit/sync/add/revert/delete for a given source
#
#  responsible:  Bernd Vorsprach
#
#  -----------------------------------------------------------------------------
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

use Env;
use Getopt::Std;
use ICopy;

#  -----------------------------------------------------------------------------
#  read arguments
#  -----------------------------------------------------------------------------
die "" if !getopts('he:');
if ( @ARGV != 2 && !defined($opt_h) ) {
  die <DATA>;
}

#  -----------------------------------------------------------------------------
#  help required?
#  -----------------------------------------------------------------------------
if ( $opt_h ) {
 print <DATA>;
 exit 0;
}

#  -----------------------------------------------------------------------------
#  editor
#  -----------------------------------------------------------------------------
if ( !defined($opt_e) ) {
	die "environment %EDITOR% not used!\n" unless defined($EDITOR);
	$editor = $EDITOR;
} else {
  $editor = $opt_e
}

($p4cmd, $source) = @ARGV;

#  -----------------------------------------------------------------------------
#  check command
#  -----------------------------------------------------------------------------
if ( $p4cmd !~ /^(add|sync|revert|edit|delete)$/) {
  die <DATA>;
}

#  -----------------------------------------------------------------------------
#  retrieve full source name
#  -----------------------------------------------------------------------------

( $vmakepath, $rel_source ) = ICopy::GetFilePath($source, 0, 1, 1);

if ( !defined($rel_source) ) {
  die "source not found!\n";
}

#  -----------------------------------------------------------------------------
#  p4 edit
#  -----------------------------------------------------------------------------
my($RC)=system("p4 $p4cmd $vmakepath/$rel_source");
if ($RC != 0) {
  die "perforce failed!\n";
}

#  -----------------------------------------------------------------------------
#  show file
#  -----------------------------------------------------------------------------
if ( $p4cmd =~ /^(add|sync|edit)$/) {
  @r_source = ($source);
  $rc = ICopy::iview(\@r_source, 0, 1, $editor, 0);
}

exit 0;



__DATA__

usage: usage: ip4 [-e editor] p4cmd source

p4cmd - add
        edit
        sync
        delete
        revert
