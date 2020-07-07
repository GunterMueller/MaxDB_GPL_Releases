#!/usr/bin/perl
#
# @(#)iclear     1998-04-01
#
# G. Groﬂmann, SAP AG
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

use Env;
use Getopt::Std;

if ( !getopts('pw') ) {
    print "usage: iclear [-p] [-w]";
}

@Dirs = ("$OWN/tmp" );
push @Dirs, ( "$WRK/fast/tmp", "$WRK/quick/tmp", "$WRK/slow/tmp") if ! $opt_w;
if ( $opt_p && ! $opt_w )
{
    push @Dirs, ( "$WRK/prot", "$WRK/fast/prot", "$WRK/quick/prot", "$WRK/slow/prot");
}
if ( $opt_w )
{
    push @Dirs, "$WRK";
}
for $i ( @Dirs )
{
    if ( -d $i ) {
        print "remove all files from $i\n";
        remdir( $i );
    }
    #unlink <$i/*.*>;
}

sub remdir() {
    my $dir = shift;
    opendir(DIR, $dir) || die "iclear: can't opendir $dir: $!";
    my @files = readdir( DIR );
    closedir( DIR );
    my $file;
    for $file ( @files )
    {
        if ( $file ne "." and $file ne ".." )
        {
            if ( -d "$dir/$file" )
            {
                remdir( "$dir/$file" );
                rmdir( "$dir/$file" ) || warn "can't remove $dir/$file: $!";
            }
            else
            {
                unlink( "$dir/$file" );
            }
        }
    }
}
