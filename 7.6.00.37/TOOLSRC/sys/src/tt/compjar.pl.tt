#!/usr/bin/perl
# compjar
#
# @(#)compjar			1999-03-23
#
# extract jar-file with Java sources and compile to jar-file
#       with class files
#
# KEEPFLG       optional keep intermediate files flag
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

package compjar;
use Env;
use File::Basename;
use File::Copy;
use Cwd;
use ICopy;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("compjar called with @ARGV\n");

$USAGE= <<EOTEXT;
usage: compjar [options] source

    -f              fast (optimized)
    -q              quick (debug)
    -s              slow = quick
    -outfile <file> name of output jar
    -resume         no unpack, (compile and pack) only
    -build <nr>     set build number
    anything else is passed to javac
EOTEXT

# set defaults

@JAVACFLAGS = ('-d .');
$debugFlagSet = 0;
$CURR_VERSION = 'FAST';
$wrkdir = 'fast';
$resume = 0;
$outjar = "";
$build = undef;

# parse command line

while ($_ = $ARGV[0], /^[-]/) {
    shift;
    last if /^--$/;

    if (/^-[h?]/) {
        print $USAGE;
        exit;
    }
    if (/^-f$/) {
        $CURR_VERSION = 'FAST';
        $wrkdir = 'fast';
        next;
    }
    if (/^-q$/) {
        $CURR_VERSION = 'QUICK';
        $wrkdir = 'quick';
        next;
    }
    if (/^-s$/) {
        $CURR_VERSION = 'SLOW';
        $wrkdir = 'slow';
        next;
    }
    if (/^-resume$/) {
        $resume = 1;
        next;
    }
    if (/^-build$/) {
        $build = $ARGV[0];
        shift;
        next;
    }
    if (/^-outfile$/) {
        $outjar = $ARGV[0];
        shift;
        next;
    }

   	if (/^-g/) {
        $debugFlagSet = 1;
    }
    push @JAVACFLAGS, $_;
    next;

}

if ( @ARGV != 1 ) { print $USAGE;  exit 1; }

$SOURCE=$ARGV[0];

$CURR_VERSION = uc($CURR_VERSION);
if ($CURR_VERSION eq 'SLOW') {
    push @JAVACFLAGS, '-O';
}
else {
    if (($CURR_VERSION eq 'SLOW') || ($CURR_VERSION eq 'QUICK')) {
        if (!$debugFlagSet) {
            push @JAVACFLAGS, '-g:lines';
        }
    }
}

# create Build string

sub createBuild ()
{
    if (!defined ($build)) {
        $build = `GetBuildNr`;
    }
    if ($build =~ /BuildNr: ([0-9]+)/) {
        $build = $1;
    }
    my $twelveDigits = sprintf ("%012d", $build);
    my $result = substr ($twelveDigits, 0, 3) . "-"
            . substr ($twelveDigits, 3, 3) . "-"
            . substr ($twelveDigits, 6, 3) . "-"
            . substr ($twelveDigits, 9, 3);
    return $result;
}

sub writeManifest ()
{
    my ($outfile, $manifestList) = @_;
    my $buildString = &createBuild ();
    my $line;
    open GLOBALMANIFEST, ">$outfile";
    foreach $infile (@$manifestList) {
        open INFILE, "<$infile";
        if ($infile =~ '.*(com/sap/.*)$') {
            open COPYMANIFEST, ">$1";
        }
        else {
            open COPYMANIFEST, ">dummy.mf";
        }
        while ($line = <INFILE>) {
            if ($line =~ /(Implementation-Version:.*)(" *$)/) {
                $line = $1 . $buildString . $2 . "\n";
            }
            print GLOBALMANIFEST $line;
            print COPYMANIFEST $line;
        }
        close COPYMANIFEST;
        close INFILE;
    }
    close GLOBALMANIFEST;
}

# change to right tmp
$wrkdir = "$WRK/$wrkdir/tmp";
chdir $wrkdir;

($BASE, $SOURCEPATH, $EXT) = fileparse($SOURCE, '\..*');
$JARNAME="$BASE$EXT";
$LISTFILE = "$BASE.lst";
$MANIFESTFILE = 'MANIFEST.MF';
%srcDirs = ();

if (!$resume) {
    # get source jar
    ICopy::icp ($SOURCE, $wrkdir, 0, 0, 1) || die $!;
    # unpack
    $listtext = `jar xvf $JARNAME`;

    # build list
    @manifests = ();
    @list = split /\n/, $listtext;
    open LISTFILE, ">$LISTFILE";
    foreach $line (@list) {
        if ($line =~ '(.*?)/') {
            $srcDirs{$1} = 1;
        }
        if ($line =~ / *extracted: *(.*\.java)$/) {
            print LISTFILE "$1\n";
            next;
        }
        if ($line =~ '.*/$') {
            # ignore directories
            next;
        }
        if ($line =~ m'extracted: (.*com/sap/.*)MANIFEST.MF$'i) {
            push @manifests, $1 . $MANIFESTFILE;
            next;
        }
        if ($line =~ 'extracted: (.*(com/sap/.*))$') {
            # copy file to place in result tree
            print "copying resource $1\n";
            copy ($1, $2);
        }
    }
    close LISTFILE;
    # generate manifest file
    if (@manifests > 0) {
        &writeManifest ($MANIFESTFILE, \@manifests);
    }
}
# compile
$javacLine = "javac @JAVACFLAGS " . '@' . "$LISTFILE";
#print "compiling: $javacLine\n";
system ($javacLine) && die "Compile error";

# pack
if (!$outjar) {
    $outjar = "$BASE.jar";
}
#print "generating jar $outjar\n";
if (-r $MANIFESTFILE) {
    $jarCmd = "jar cfm $outjar $MANIFESTFILE com";
}
else {
    $jarCmd = "jar cf $outjar com";
}
system ($jarCmd) && die "jar failed";

# copy to SRC
copy ($outjar, "$SOURCEPATH/$outjar");

# remove tmps
if ( @KEEPFLG == 0 ) {
    @dirlist = keys %srcDirs;
    $rmCmd = "rm -rf $JARNAME $LISTFILE $outjar @dirlist com META-INF $MANIFESTFILE";
    system ($rmCmd);
}
