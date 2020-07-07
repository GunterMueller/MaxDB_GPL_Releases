#!/usr/bin/perl
#
#	retrieve message text for message number
#
#    ========== licence begin  GPL
#    Copyright (C) 2003 SAP AG
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
use File::Basename;

if ( $RELVER eq "R61" ) {
   $BASEDIR="$DBROOT/env";
} else {
   $BASEDIR="$DBROOT/env/en";
}

if  ( @ARGV != 1 ) {
     print "Usage: err <msgno>";
     exit 100;
}

$no=$ARGV[0];
chdir( $BASEDIR );
opendir(DIR, "$BASEDIR");

if ( $RELVER eq "R61" ) {
   @Files = grep /.enm$/, readdir(DIR);
} else {
   @Files = grep /M\.en$/, readdir(DIR);
}
closedir(DIR);

foreach $File (@Files) {
    open (INFILE, "<$File");
    while (<INFILE>) {
        if ( /(DEU|ENG)\s+$no/ ) {
            @list = split / +/, $_;
            foreach $field (@list) {
                if ( $field == $no ) {
                    print "$File: $_";
                    close (INFILE);
                    exit;
                }
            }
        }
    }
    close ($INFILE);
}

# look in $OWN/sys/src/SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.cpp
if ( $RELVER ge "R75" ) {
    $File = "$OWN/sys/src/SAPDB/SQLManager/ErrorHandling/SQLManErr_Instance.cpp";
    open (INFILE, "<$File");
    while (<INFILE>) {
	    if ( /{.*,($no),(".*")/ ) {
            print basename($File), ": ", $1, " ", $2;
            close (INFILE);
            exit;
        }
    }
    close (INFILE);
}
print "msgno is not defined";


