#!/devtool/local/bin/perl
# convert old types in ptoc.h to new types
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2004 SAP AG
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


use strict;


my $PTOCBASE_H = shift;
my $TEMP_FILE = "$PTOCBASE_H".".trunc_ptoc_tmp";
my $line = undef;

print "Truncate ptoc.h for ptoc_base.h\n";

open(FILE_IN, "<$PTOCBASE_H") || die "Can't open $PTOCBASE_H (input): $!";
open(FILE_OUT, ">$TEMP_FILE") || die "Can't open $TEMP_FILE (output): $!";

while($line = <FILE_IN>) 
{
	if ($line =~ /^#\s*define\s+(s35|vbegexcl).*/)
	{
		while ($line && ($line =~ /\\\s*$/))
		{
			$line = <FILE_IN>;
		}
		next;
	}
	print FILE_OUT $line;
}

close(FILE_OUT); 
close(FILE_IN);

unlink ("$PTOCBASE_H");
rename ("$TEMP_FILE", "$PTOCBASE_H");
0;
	

 
