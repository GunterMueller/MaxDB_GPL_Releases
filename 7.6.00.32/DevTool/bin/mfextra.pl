#!/usr/bin/perl
# mfextra       *** internal script (called by vmake !)
#
# @(#)mfextra		1997-11-18
#
# module frame to extra file
#
# TOOL          directory for tools
# KEEPFLG       optional keep intermediate files flag
#
#
# includes currently unused but syntactically allowed
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

package mfextra;
use Env;
use File::Basename;
use File::Copy;
use ToolLib; # CTS 1108093
use File::Path;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfextra called with @ARGV\n");

$USAGE = "usage: mfextra [-<symbol>=<value>] [-k] infile outfile [includes]\n";

# mf_opts muß hier eingefügt sein, da ein
# imf -U mf_opts.pl.tt ein vorhandenes
# %tool%\bin\mf_opts.pl weglöscht, welches hier benötigt
# wird
#
# ---------------- start mf_opts.pl --------------------

$KIND      = "";
$TRACE     = "";
$CHECK     = "yes";
$TRACEPROC = "none";
$unpack    = "";
$concom    = "";
$MFPNAME   = "";
$MKEXEC    = "";
$SETBUILDINFO = 0;

while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

    if(/^-f$/)           { $KIND = "FAST"; next }

    if(/^-q$/)           { $KIND = "QUICK"; next }

    if(/^-s$/)           { $KIND = "SLOW"; next }

    if(/^-exec=(.*)/)    { $MKEXEC=  $1 eq "" ? $_ : $1; next}

    if(/^-unpack=(.*)/)  { $unpack =  $1 eq "" ? $_ : $1; next}

    if(/^-concom=(.*)/)  { $concom =  $1 eq "" ? $_ : $1; next}

    if(/^-trace=(.*)/)   { $TRACEPROC =  $1 eq "" ? $_ : $1; next}

    if(/^-check=(.*)/)   { $CHECK =  $1 eq "" ? $_ : $1; next}

    if(/^-MACH=(.*)/)    { $MACH =  $1 eq "" ? $_ : $1; next}

    if(/^-KIND=(.*)/)    { $KIND =  $1 eq "" ? $_ : $1; next}

    if(/^-TRACE=(.*)/)   { $TRACE =  $1 eq "" ? $_ : $1; next}

    if(/^-COMP=(.*)/)    { $COMP =  $1 eq "" ? $_ : $1; next}

    if(/^-MFPNAME=(.*)/) { $MFPNAME =  $1 eq "" ? $_ : $1; next}

    if(/^-optdebug/)     { push @CONCOMOPTS, "-debug=1" ; next }

    if(/^-setbuildinfo/) { $SETBUILDINFO=1 ; next }

    if(/^-.*=.*/)        { push @CONCOMOPTS, $_; next}

    if(/^-k$/)           { @KEEPFLG = "-k"; next }

    if(/^-.*/)           { print $USAGE; exit 1; }

} #while

if ( $KIND eq "" ) {

	CASE: {
	   if ($VMAKE_VERSION =~ /^f.*/i) { $KIND = "FAST";  last CASE }

	   if ($VMAKE_VERSION =~ /^q.*/i) { $KIND = "QUICK";	last CASE }

	   if ($VMAKE_VERSION =~ /^s.*/i) { $KIND = "SLOW";	last CASE }

       print $USAGE;
       print "please specify VMAKE_VERSION\n";
       exit 1
    } #CASE
}

if ( $TRACE eq "" ) {

    $TRACE = "N";
	if ($KIND =~ /^s.*/i) { $TRACE = "J"; }

}

# ----------- end mf_opts.pl ---------------

if ( @ARGV < 2 ) { print $USAGE; exit 1 }

$SOURCE = shift;
$BASE = basename($SOURCE);
$DEST = shift;

$TMP1 = "mfextra_tmp1.$$";
$TMP2 = "mfextra_tmp2.$$";

unlink $TMP1, $TMP2;

$DESTDIR = dirname($DEST);

if ( ! -d $DESTDIR ) {
   File::Path::mkpath($DESTDIR, 0,0775);
}

if ( $unpack ne "no" ) {
    system("$TOOL/pgm/mfspl $SOURCE $NULL $NULL $TMP1") == 0
       or die "Error while executing \"$TOOL/pgm/mfspl $SOURCE $NULL $NULL $TMP1\"\n".
	   "message: $!\n";
}
else { ToolLib::SrcCopy ($SOURCE, $TMP1) || die "Source copy failed : $!\n"; } # CTS 1108093

@CONCOMOPTS;

if ( $concom ne "no" ) {
    system ("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, $TMP1, $TMP2, $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
       or die "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $TMP1 $TMP2 $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	   "message: $!\n";
}
else { copy $TMP1, $TMP2 }

if ($SETBUILDINFO)
{
	require Buildinfo;
	import Buildinfo;
	Buildinfo::setbuildinfo("$TMP2");
}

if ( @KEEPFLG == 0 ) {
    unlink $TMP1, $DEST;
	move $TMP2, $DEST;
}
else {
    unlink $DEST;
    copy $TMP2, $DEST;
}

if ( $MKEXEC eq "1" ) {
	# als ausführbar kennzeichnen falls nicht in NT
	if ( $^O !~ /mswin32/i) { `chmod +x $DEST` }
}

__END__

Hier können lange Kommentare stehen
