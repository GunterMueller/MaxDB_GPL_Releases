#!/usr/bin/perl
# mf_opts       (NOT called by vmake !)
#
# @(#)mf_opts	1997-11-18
#
# included by mf? and mf?inc
#
# kein package name, da im Namespace des aufrufenden Scripts ausgeführt
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


$KIND      = "";
$TRACE     = "";
$CHECK     = "yes";
$TRACEPROC = "none";
$NO_TRACE  = 0;
$unpack    = "";
$concom    = "";
$checkinc  = "";
$MFPNAME   = "";
$MKEXEC    = "";
$LAYER     = "";
$PREPROC   = 0 ;
$NOEXTENSION = 0;
$NOWHATSTRING = 0;
@INCLUDE_DIRS=split /$PATH_SEPARATOR/, $ENV{INCLUDE};
@DEFINES = ();
$SCRIPT = "";
$SETBUILDINFO = 0;
@ADDITIONAL_COMPILE_FLAGS = ();
$INC_COMPILECHECK = ($ENV{INCLUDE_COMPILE_CHECK} =~ /^force$/i) ? 1 : 0;
$INC_COMPILETYPE=defined $ENV{INCLUDE_COMPILE_TYPE} ? $ENV{INCLUDE_COMPILE_TYPE} : "cpp";;


while ($_ = $ARGV[0], /^-/) {
    shift;
    last if /^--$/;

	if(/^-([-\+].*)$/)
	{
		my $Option = $1;
		if ($Option =~ /^-I(.*)$/i)
		{
			push @INCLUDE_DIRS, $1;
		}
		if ($Option =~ /^-D/)
		{ push @DEFINES, $Option; }
		elsif ($Option =~ /^-nowhatstring/)
		{ $NOWHATSTRING=1; }
		else
		{push @ADDITIONAL_COMPILE_FLAGS, $Option; }
		next;

	}

    if(/^-f$/) { $KIND = "FAST"; next }

    if(/^-q$/) { $KIND = "QUICK"; next }

    if(/^-s$/) { $KIND = "SLOW"; next }

	if(/^-n$/) { $KIND = ""; next }

	if(/^-ascii$/){
		# initialize to defauls of ascii (concom,unpack,checkinc=no)
		$unpack = "no" if ($unpack eq "");
		$concom = "no" if ($concom eq "");
		$checkinc = "no" if ($checkinc eq "");
		next
	}

	if(/^-preprocess/){
		$SCRIPT="preprocess.pl";
		next
	}

	if(/^-execscript=(.*)/)
	{
		$SCRIPT = $1;
		next
	}

	if(/^-inctype=(.*)/)
	{
		my $inctype= $1;
		if ($inctype =~ /^(c|cpp)$/i)
		{
			$INC_COMPILETYPE = $1;
			$INC_COMPILETYPE =~ tr/A-Z/a-z/;
		}
		else
		{
			die "wrong inctype '$inctype': $_\n";
		}
		next;
	}

	if(/^-checkinc=(.*)/)
	{
		$checkinc = $1 eq "" ? $_ : $1; next}

    if(/^-exec=(.*)/) {
       $MKEXEC=  $1 eq "" ? $_ : $1; next}

    if(/^-unpack=(.*)/) {
       $unpack =  $1 eq "" ? $_ : $1; next}

    if(/^-concom=(.*)/) {
       $concom =  $1 eq "" ? $_ : $1; next}


    if(/^-trace=(.*)/) {
       $TRACEPROC =  $1 eq "" ? $_ : $1; next}

	if (/^-no_trace/) { $NO_TRACE = 1; next }

    if (/^-trace_step_out=1/) {
        if (! defined($TRACE_STEP_OUT)) { $TRACE_STEP_OUT = "-o"; }
        next
    }

    if (/^-no_trace_step_out=1/) { $TRACE_STEP_OUT = ""; next }

    if(/^-check=(.*)/) {
       $CHECK =  $1 eq "" ? $_ : $1; next}

    if(/^-language=(.*)/) {
       $LANG =  $1 eq "" ? undef : $1; next}

    if(/^-MACH=(.*)/) {
       $MACH =  $1 eq "" ? $_ : $1; next}

    if(/^-KIND=(.*)/) {
       $KIND =  $1 eq "" ? $_ : $1; next}

    if(/^-TRACE=(.*)/) {
       $TRACE =  $1 eq "" ? $_ : $1; next}

    if(/^-COMP=(.*)/) {
       $COMP =  $1 eq "" ? $_ : $1; next}

    if(/^-MFPNAME=(.*)/) {
       $MFPNAME =  $1 eq "" ? $_ : $1; next}

	# &gar PTS 1002135
	if(/^-debug=(.*)/) {
       $DEBUG =  $1; push @CONCOMOPTS, "-debug=$DEBUG"; next}

	if (/^-optdebug/) {
       $DEBUG =  1; push @CONCOMOPTS, "-debug=$DEBUG"; next}

	if (/^-layer=(.*)/) { $LAYER=$1; next	}

	if (/^-noextension/)  { $NOEXTENSION = 1; next } # CTS 1112700

	if (/^-setbuildinfo/) { $SETBUILDINFO = 1; next }

	if (/^-compilecheck(=.+)?$/)
	{
		if ($1 =~ /=no/i)
		{  $INC_COMPILECHECK = 0; }
		elsif ( $ENV{INCLUDE_COMPILE_CHECK} )
		{  $INC_COMPILECHECK = 1;		}
		next
	}

    if(/^-.*=.*/) {
    				next if /^-LC_STATE=/;
    				push @CONCOMOPTS, $_; next
    				}

    if(/^-k$/) { @KEEPFLG = "-k"; next }

    if(/^-k$/) { @KEEPFLG = "-K"; next }

    if(/^-.*/) { print $USAGE; exit 1; }

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

if ( $TRACE eq "J" || defined( $ENV{'BETA'} ) )
{
    push @CONCOMOPTS, "-BETA=1";
}

if ($NO_TRACE)
{
	$TRACEPROC = "none";
}
