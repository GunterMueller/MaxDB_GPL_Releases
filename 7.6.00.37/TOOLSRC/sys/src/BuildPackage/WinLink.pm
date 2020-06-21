#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/WinLink.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc:
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

package WinLink;

use Exporter;

@ISA = ('Exporter');

@EXPORT = ('createLink','readLink');

sub createLink{
	my ($src,$link) = @_;
	$^O =~ /mswin/i or $! = "use it for windows only" and return 0;
	-e $src or return 0;
	-e $link and $! = "$link already exist\n" and return 0; 
	open myFD, ">$link" or $! = "cannot create $link: $!" and return 0;
	print myFD "$src\n";
	close myFD;
	return 1;
}

sub readLink{
	my ($link) = @_;
	$^O =~ /mswin/i or $! = "use it for windows only" and return undef;
	-f $link or return undef;
	open (myFD,$link) or $! = "cannot open $link: $!" and return undef;
	my $count = 0;
	my $deref = '';
	while(<myFD>){
		chomp;
		$deref = $_;
		$count++;		
	}
	close(myFD);
	unless($count == 1 || $deref =~ /^[\w|\d|\/|\\|\ ]+$/){
		$! = "invalid link format";
		return undef;
	}
	return $deref;	
}

1;

