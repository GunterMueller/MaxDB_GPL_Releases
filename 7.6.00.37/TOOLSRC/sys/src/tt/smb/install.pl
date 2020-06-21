#!/usr/bin/perl
#
# :tt/smb/install.pl
# installation script for WIN32
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


use File::Copy;

sub main {
	my ($InstBin, $InstLib) = get_inst_dirs (get_tool_dir ());
	my $Mode = 0775;

	mkdir ($InstLib."\\IO", $Mode);
	mkdir ($InstLib."\\SMB", $Mode);
	mkdir ($InstLib."\\SMB\\File", $Mode);

	copy ("SMBNT.pm", $InstLib."\\SMB.pm");
	copy ("IO\\SMB.pm", $InstLib."\\IO\\SMB.pm");
	copy ("SMB\\File\\Copy.pm", $InstLib."\\SMB\\File\\Copy.pm");
	return 0;
}

sub get_inst_dirs {
	my $InstDir = shift;
	my $InstBin = $InstDir."\\bin";
	my $InstLib = $InstDir."\\lib\\Perl";
	return ($InstBin, $InstLib);
}

sub get_tool_dir {
	my $TOOL = $ENV{"TOOL"};
	unless (defined $TOOL && $TOOL ne "") {
		die "\$TOOL undefined, please set \$TOOL\n";
	}
	return $TOOL;
}

main (@ARGV);
