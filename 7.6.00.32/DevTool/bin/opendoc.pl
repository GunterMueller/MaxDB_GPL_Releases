#!/usr/bin/perl
#
#       @(#)opendoc     Version:6.2.8           30-10-1997
#
#       Gert Großmann, SAP AG
#
# usage: opendoc [-e editorname] [-r] [-s] file [ file ... ]
#	-e editorname	: name of the used editor
#   -r				: open as ReadOnly file
#	-s				: split the given files in the editor
#
# open files with the given editor and mode
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

package OpenDoc;
use Env;
use Getopt::Std;

undef $opt_e; undef $opt_s; undef $opt_r;

if ( getopts('e:rs') && @ARGV == 0 ) { die "usage: opendoc [-e editorname] [-r] [-s] file [ file ... ]\n" }

if ( !defined($opt_e) ) {
	die "environment %EDITOR% not used!\n" unless defined($EDITOR);
	$opt_e = $EDITOR;
}
else {
    $opt_e_set = 1;
}

    @ED_CMD = ();
    if (defined ($opt_e_set)) {
        push @ED_CMD, $opt_e;
    }
	elsif ($opt_r != 0) {
		if (-x "/usr/local/bin/less") {
			push @ED_CMD, "/usr/local/bin/less";
		}
		# PTS 1103330
		elsif (-x "/usr/bin/less") {
			push @ED_CMD, "/usr/bin/less";
		}
		elsif ($opt_e  eq "vi") {
			push @ED_CMD, "vi", "-R";
		}
	}
	# &gar for local read -> not read only
	else {
		push @ED_CMD, $opt_e;
	}

	push @ED_CMD, @ARGV;

	system (@ED_CMD) == 0
		or warn "Can't execute \"@ED_CMD\": $!\n";

