#!/usr/bin/perl
#
# $SRC/extern/libunwind/build_libunwind.pl
#
# This script is used to extract,configure and make libunwind from source package
#
# Modify these values if using a different libunwind package
#
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

$LIBUNWINDVERSION="libunwind-0.98.3";
$LIBUNWINDTAR=$LIBUNWINDVERSION.".tar.gz";
$EXTERNDIR=$ENV{WRK}."/extern";
$EXTERNINCLUDEDIR=$ENV{WRK}."/incl/extern";
$LIBUNWINDMAKEDIR=$EXTERNDIR."/".$LIBUNWINDVERSION;
$LIBUNWINDCFLAGS=$CCFLAGS_LIBUNWIND." -ip -O3";
$LIBUNWINDCOMPILERCONFIG=($ENV{'COMPILER'} =~ /ICC/i) ? "CC=icc CFLAGS=\"$LIBUNWINDCFLAGS\" CXX=icpc CCAS=gcc CCASFLAGS=-g LD=icc LDXX=icpc" : "";
$LIBUNWINDCONFIG="--exec_prefix=$EXTERNDIR --prefix=$EXTERNINCLUDEDIR ".$LIBUNWINDCOMPILERCONFIG;

print "build_libunwind called extracting sources\n";
system("cd $EXTERNDIR;        tar -xzf $LIBUNWINDTAR")          && die "Extracting $LIBUNWINDTAR failed";
print "configure $LIBUNWINDCONFIG called\n";
system("cd $LIBUNWINDMAKEDIR; ./configure $LIBUNWINDCONFIG")   && die "Configuring $LIBUNWINDVERSION failed";
print "make AM_FLAGS=-fPIC called\n";
system("cd $LIBUNWINDMAKEDIR; make AM_CFLAGS=-fPIC")             && die "Making $LIBUNWINDVERSION failed";
print "make AM_FLAGS=-fPIC install-includeHEADERS install-nodist_includeHEADERS install-exec called\n";
system("cd $LIBUNWINDMAKEDIR; make AM_CFLAGS=-fPIC install-includeHEADERS install-nodist_includeHEADERS install-exec")  && die "Making $LIBUNWINDVERSION failed";
