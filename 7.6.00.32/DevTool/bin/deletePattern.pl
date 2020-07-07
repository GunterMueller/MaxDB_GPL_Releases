#!/usr/bin/perl
# delvpa00
#
# @(#)delvpa00    2002-04-09
#
# deletes all "vpa00" in a file (used for vpa00sql.h -> sql.h etc.)
#
# TTK
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

# reads file "in", replaces $pattern with $replace and writes file "out"
sub delete_pattern
{
  local($in)      = $_[0];
  local($out)     = $_[1];
  local($pattern) = $_[2];
  local($replace) = $_[3];

  if ($in eq $out)
  {
  	my @lines=();
  	open (INPUT, $in)       || die "could not open ".$in;
  	while (<INPUT>)
  	{
	  s/$pattern/$replace/g;
	  push @lines, $_;
	}
	close (INPUT);
	open (OUTPUT, ">".$out) || die "could not open ".$out;
	foreach (@lines)
	{
	  printf OUTPUT;
	}
  	close (OUTPUT);
  }
  else
  {
	  open (INPUT, $in)       || die "could not open ".$in;
	  open (OUTPUT, ">".$out) || die "could not open ".$out;

	  while (<INPUT>)  {
	    s/$pattern/$replace/g;
	    printf OUTPUT;
	  }
	  close (INPUT);
	  close (OUTPUT);
  }
}

delete_pattern($ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3]);

