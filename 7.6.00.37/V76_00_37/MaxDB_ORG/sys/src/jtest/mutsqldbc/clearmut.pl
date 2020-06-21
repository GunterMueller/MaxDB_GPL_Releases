#!/usr/bin/perl
# @(#) clearmut		2005-10-07
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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


use Env;
use File::Path;

print "Delete all mut files, that are not necessary (old prot's etc.)\n";

unlink <*.prot>, <*.prt>, <*.pct>, <*.exe.f>, <*.exe.s>, <sv_*>, <*.act>, <*.abo>, <*.inp>, <dbmcli.*>, <*.vtrace.txt>, <*.knldiag.txt>;
unlink <*.ilk>, <*.obj>, <*.pdb>;

unlink "mut.err", "param.chk", "ALLOC_STAT", ".prot", "trace_show.txt";

rmtree(dbanaprt, 1, 1);

print "... done\n";
