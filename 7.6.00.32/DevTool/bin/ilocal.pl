#!/usr/bin/perl
# ilocal
#
# @(#)ilocal  6.2  1998-01-09
#
# stolen from knl-tcc.pl
#
# Scans $OWN to find local files from the specified component
#
# Burkhard Diesing
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



package pcxuser;
use Env;
use Getopt::Std;
use ICopy;
$Trace=1; # Switch to 1 to enable trace

if (!getopts ('hlue:n') || $opt_h) {
    print <DATA>;
    exit;
}
if ( $opt_n && !$opt_e ) {
    print STDERR "ERROR: option -n only allowed with option -e.";
    exit;
}
if (@ARGV != 0) {
    @desc = @ARGV;
}
else {
    @desc = "all";
}

print "ilocal scans @desc";

open(SAVEOUT, ">&STDOUT");
open(STDOUT, ">$TMP/all");

foreach $desc (@desc) {
    @ARGV = ("-m", $desc);
    do "$TOOL/bin/imf$TOOLEXT";
    if ( $@ ) { die "Error while executing ims$TOOLEXT: $@" }
}


close(STDOUT);
open(STDOUT, ">&SAVEOUT");

open(FILE_IN, "<$TMP/all");
LINE:
while(<FILE_IN>) {
    chop;
    if (/^.*\.dll$/) {
        s/^.*\n?//;
        redo LINE if $_;
        next LINE;
    }
    if (/^.*\.idl$/) {
        s/^.*\n?//;
        redo LINE if $_;
        next LINE;
    }
    if (/^unknown target.*/) {
	die "\nERROR: collecting @desc\n";
    }
    if (/VMAKE_PATH/) {
        s/^.*\n?//;
        redo LINE if $_;
        next LINE;
    }
	push @FILE_OUT, $_."\n";
}
close(FILE_IN);
@FILE_OUT = sort @FILE_OUT;
open(FILE_OUT, ">$TMP/all1");
undef($prevline);
foreach $out (@FILE_OUT) {
	next if $prevline eq $out;
	$prevline = $out;
	print FILE_OUT $out;
}
close(FILE_OUT);

print " ... ";
open(FILE_IN, "<$TMP/all1"); local @count = <FILE_IN>;
print "check ", scalar(@count),  " files ...\n";
seek FILE_IN, 0, 0;
$lockedfiles = 0;
$notlockedfiles = 0;
$localfiles = 0;
while(<FILE_IN>) {
    chop;
    $Local = "$OWN/sys/$_";
    if ( -f $Local ) {
	$localfiles += 1;
	if ($opt_l || $opt_u) {
	    open(SAVEOUT, ">&STDOUT");
	    open(STDOUT, ">$TMP/all2");
	    @ARGV = ("-s", "-f $Local" );
	    do "$TOOL/bin/istat$TOOLEXT";
	    close(STDOUT);
	    open(STDOUT, ">&SAVEOUT");
	    close(SAVEOUT);
	    open(FILE_IN2, "<$TMP/all2");
	    while(<FILE_IN2>) {
		if ($opt_l && /^.*(locked by .*)$/) {
		    $catch = $1;
		    $lockedfiles += 1;
		}
		if ($opt_u && /^.*(not locked).*$/) {
		    $catch = $1;
		    $notlockedfiles += 1;
		}
		if ($catch) {
		    undef($catch);
		    print "$Local";
		    if ($opt_u && $opt_l) {
			print " $1";
		    }
		    print "\n";
		}
	    }
	    close(FILE_IN2);
	}
	else {
	    $found += 1;
	    print "$Local\n";
	}
	if ($opt_e) {
	    $opt_e =~ s/(.*)%d(.*)/$1directory$2/;
	    $opt_e =~ s/(.*)%p(.*)/$1path$2/;
	    $opt_e =~ s/(.*)%f(.*)/$1file$2/;
	    print "$opt_e\n"
	}
    }
}
if ($localfiles > 0) {
    if ($lockedfiles > 0 || $notlockedfiles > 0) {
	if ($lockedfiles > 0) {
	    print "$lockedfiles locked";
	}
	if ($notlockedfiles > 0) {
	    if ($lockedfiles > 0) {
		print " and ";
	    }
	    print "$notlockedfiles not locked";
	}
	print " files found.\n";
    }
    else {
	print "$localfiles local files found.";
    }
}
else {
    print "No local files found.";
}

close(FILE_IN);
unlink("$TMP/all", "$TMP/all1");

__DATA__

 USAGE: ilocal [-hlue:n] | [component ...]

        Shows all local modules and description from the specified component.

 OPTIONS:    -h       Prints this help.
             -l       Displays locked files.
             -u       Displays not locked files.

             -e <cmd> Executes the formatted <cmd> argument. Following
                      options will format the <cmd> argument:
                            %f the filename,
                            %d the directory and
                            %p both file and directory.

             -n       No execution of <cmd>.
