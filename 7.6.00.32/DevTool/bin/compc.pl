#!/usr/bin/perl
# compc         *** internal script (called by vmake !)
#
# @(#)compc			1997-11-18
#
# compile C / C++ / CLE-Pascal / Fortran / lpipascal / SINIX Pascal-XT
#
# CCFLAGS       optional c compiler flags
# KEEPFLG       optional keep intermediate files flag
#
# 1997-02-06 GG: Output-Extension wird auf .cpp gemappt, falls .cpp Input ist
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

package compc;
use Env;
use File::Basename;
use File::Path;
use File::Copy;
use Carp;
use Cwd;
use ICopy;
# Get CC, CCFLAGS, OPTIMIZE, CCMODEANSI, CCMODELINT, CCMODENORM
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("compc called with @ARGV\n");

$USAGE="usage: compc [cc-flags] source\n";

# Assume C-compiler
$EXT    = ".c";
$AEXT   = ".s";
@CCMODE = @CCMODENORM;
$FOR_DEBUG = 0; # 1 - debug , 2 - debug (optimzed)
$NO     = "";
$NY     = "";
$NC = "";
$Y      = "";
$NOWHATSTRING = 0;
# ignore warnings CTS 1102757
$CC_IGNORE = "";
my($cc) =""; # CTS 1103181
@INCLUDES=();
$filename_without_variant = "";
@OPTADD=();
@OPTSUB=();
my $NOTHREAD=0;
my $USE_THREAD = 0;
my $DEFAULT_NOTHREAD = 0;
my $SAVED_SOURCES_DIR = defined $ENV{SAPDB_SAVE_SOURCES} ? "../src" : undef;
my $SUPPRESS_VARIANT_NAME=0;
my $USE_PDB_FILES=0;
my $COMPILEFLAG="-c";

while ($_ = $ARGV[0], /^[-+]/) {
    shift;


    last if /^--$/;

	if (/^-k&r$/)        { @CCMODE = @CCMODEKR; next; }

    if (/^-ansi$/)       { @CCMODE = @CCMODEANSI; next; }

	if (/^-compile_as_cpp$/) {
        push @CCFLAGS, "-x c++";
        next; }

	if (/^-lint$/)       { @CCMODE = @CCMODELINT; next; }


	if (/^-except/ )   { push @CCFLAGS, @EXCEPT_FLAG ;next}     # PTS 1000166

	if (/^-rtti/ )   { push @CCFLAGS, @RTTI_FLAG ;next}

	if (/^-nothread/ ) { $NOTHREAD = 1;	next; }

	if (/^-use_thread/ ) { $USE_THREAD = 1; next; 	}

	if (/^-compileflag=(.*)/ )   { $COMPILEFLAG=$1; next }

	if (/^-default_nothread/ )	{ $DEFAULT_NOTHREAD = 1; next; }

    if (/^-K.*$|^-NORANG$/ ) { next }     # ignored

    if (/^-language=(.*)/ )   { $language=$1; next }

    if (/^-c$/)          { next }     # used implicitly

    if (/^-D$/)          { next }     # catch define-option without values

	if (/^-Mx=(.*)/) { $LINKMODE = $1; next }

	if (/^-MD/)    { $LINKMODE = "-MD"; next } # temp

	if (/^-G/)    { next } # ignored

    if (/^-Y$/)    { $Y='-Y'; next }

    if (/^-NY$/)   { $NY='-NY'; next }

    if(/^-NC$/) { $NC = "-NC"; next }  # 'No range Check' for this module

	if(/^-range_check/) { next; }  # range checks ignored

    if (/^-NO$/)   { @OPTIMIZE = @NO_FLAGS; next }            # CTS 1108431

    if (/^-IN$/)   { push @CCFLAGS, "-I${WRK}/incl"; next }

	# &gar CTS 1102757
    if (/^-IGNORE=(.*)/)
    {
        $CC_IGNORE.="|" unless ($CC_IGNORE eq "");
        $CC_IGNORE.= $1;
        # change ',' to '|' ( for RegExpr-String )
        $CC_IGNORE =~ s/,/\|/g;
        next;
    }

    if (/^-OPT=(.*)$/) { @OPTIMIZE = getOptFlags($1) unless ($FOR_DEBUG == 1) ; next }

	if (/^-OPTADD=(.*)$/) { @OPTADD = getOptFlags($1); next }

	if (/^-OPTSUB=(.*)$/) { @OPTSUB = getOptFlags($1); next }


    if (/^-x=(.*)/) { $SCRIPT = $1;  print "\$SCRIPT = $SCRIPT\n"; next}

    if (/^-X=(.*)/ ) { # execute command
          $CMD = $1;
          print "$CMD\n";
		  system($CMD) == 0
		     or croak "Error while executing \"$CMD\"";
		  next;
	}

    if (/^-CC=(.*)/ ) { $CC = $cc = $1; next }

	# CTS 1103249
	if (/^-CFLAGS=(.*)/ ) { push @CCFLAGS, (split ",", $1); next }


    if (/^-k$/)  { @KEEPFLG = "-k"; next }

    if (/^-g$/) {
		@OPTIMIZE=(); # CTS 1108431
		$FOR_DEBUG=1;
		# CTS 1103440
		push @CCFLAGS, @NO_FLAGS;
        warn "Warning: compile with debug flag, but it's a productive make!\n"
            if $ENV{RELSTAT} =~ /^prod/i;
        next;
    }
    # CTS 1110663
    if (/^-optdebug/ )
    {
        $FOR_DEBUG=2;
        warn "Warning: compile with debug flag, but it's a productive make!\n"
            if $ENV{RELSTAT} =~ /^prod/i;
        next;
    }

    if (/^-p$/) { push @CCFLAGS, @PRF_FLAGS;
					warn "Option for Profiling on this platform is not implemented\n"
						if ($#PRF_FLAGS==-1);
                next }
    if (/^-preproc/) { $preprocessing = 1; next; }

    if (/^-cc_oversteer\((.*)\)/) { push @CCFLAGS_OVERSTEER, $1; next; }

    if (/^-rm_ccflag\((.*)\)/) { push @CCFLAGS_RM, $1; next; }

    if (/^-I(.*)$/)   { push @INCLUDES, "-I$1"; next }

    if (/^-append=(.+)$/)   { push @CCFLAGS_OVERSTEER, $1; next; }

	if (/^-nowhatstring/) { $NOWHATSTRING=1; next}

	if (/^-save_sources/)
	{
		if (/^-save_sources=(.*)$/)
		{
			$SAVED_SOURCES_DIR=$1;
		}
		else
		{	$SAVED_SOURCES_DIR="../src";}
		next;
	}

	if (/^-suppress_variant_name$/)
	{
		$SUPPRESS_VARIANT_NAME = 1;
		next;
	}

	if (/^-pdbfile=(.+)$/)   { $PDBFILE="$1"; $PDBFILE =~ tr/\//\\\\/; next; }

	if (/^-force_pdb_files$/)
	{
		$USE_PDB_FILES = 1;
		next;
	}

	if (/^-use_local_lcinc$/) { @LC_INC_FLAGS = (); next; }

  if (/^-.*/ ) { push @CCFLAGS, $_; next }

	# some options on HP/UX starting with '+',
	# caused by a vmake feature/bug we get '=1' at the end, but we do not want this at all
	if (/^\+(.*)=1/ ) { push @CCFLAGS, $1; next }
	# &gar: options with + without '=1' (since vmake 9.5.6 without automatic '=1')
	if (/^\+.*/ ) { push @CCFLAGS, $_; next }

} #while


if ( $NOTHREAD || ($DEFAULT_NOTHREAD && ( $USE_THREAD == 0 )))
{
	$NOTHREAD=1;
	@THREAD_CCFLAGS=();
	$LINKMODE = "-MD";
}

if (scalar(@LC_INC_FLAGS) > 0)
{
	unshift @CCFLAGS, @LC_INC_FLAGS;
}

# mix optimze flags
push @OPTIMIZE, @OPTADD;
foreach $optsup (@OPTSUB)
{
	my( $i, $maxi ) = ( 0, $#OPTIMIZE );
	while( $_ = $OPTIMIZE[ $i ], ( $i<=$maxi ) )
	{
		if ( $optsup eq $_)
		{
			splice @OPTIMIZE, $i, 1; next;
		}
		$i++;
	}
}


if ( @ARGV != 1 ) { print $USAGE;  exit 1; }

$SOURCE=$ARGV[0];
($BASE, $SOURCEPATH, $EXT) = fileparse($SOURCE, '\..*');
# for full path for MS DevStudio
# $SOURCEPATH=cwd()."\\" if ( $SOURCEPATH =~ /\.\\/);

#
# jrg 9.9.1999
# Always force ANSI compilation for C++ modules
#
if ($EXT eq ".cpp" || $language eq 'x')
{
	@CCMODE = @CCMODEANSI;
}

unshift @CFLAGS, @CCMODE;

unshift @CCFLAGS, @INCLUDES;

if ($RELVER ge "R80") {
    if ($EXT eq ".cpp" || $language eq 'x') { push @CCFLAGS, @CXXFLAGS; }
}

# $CURR_VERSION wird von 'VMAKE' bereitgestellt und darf in
# der Umgebung nicht gesetzt sein
$CURR_VERSION = uc($CURR_VERSION);

# CTS 1109050
if (($RELVER lt "R74") || ($RELVER eq "RTOOL"))
{
push @CCFLAGS, "-D$CURR_VERSION=$CURR_VERSION";
push @CCFLAGS, "-DQUICK=QUICK" if ( $CURR_VERSION eq "SLOW");
}

push @CCFLAGS, "-DSAPDB_$CURR_VERSION";
push @CCFLAGS, "-DSAPDB_QUICK" if ($CURR_VERSION eq "SLOW");

if ($FOR_DEBUG)
{	push @CCFLAGS, @DEBUG_FLAGS; }

if ( ! $CC ) {
	    ToolTrace( "This machine has no C compiler defined\n" );
		my $OutFile = $$;
        open(FILE_OUT, ">$OutFile$EXT") or croak "Can't open $OutFile$EXT (append output): $!\n";
        print FILE_OUT  "#include <stdio.h>\n";
        close FILE_OUT;
        system("cc $COMPILEFLAG $OutFile$EXT") == 0
		   or croak "Error while executing \"cc $COMPILEFLAG $OutFile$EXT\"";
		unlink "$BASE.o"; rename "$OutFile.o", "$BASE.o";
        unlink "$OutFile$EXT";
        exit 0;
}

if ( $Y and !$NY ) { push @CCFLAGS, "-DPTOC_Y"; }

push @CCFLAGS, @THREAD_CCFLAGS;   # CTS 1110865

push @CCFLAGS, @OPTIMIZE ;  # CTS 1108431

if ( $SCRIPT ) {
	local $script = "$SCRIPT$TOOLEXT" unless $SCRIPT =~ /.+\..+/;
	do "$TOOL/bin/$script";
	if ( $@ ) { warn "Error execute $TOOL/bin/$script: $@"; exit 2}
    #
    # IST 1994-05-19
    # If the script is 'tr_cpc' then the precompiled source
    # containes '#include'-statements which refer to $DBROOT/incl.
    #
	local $dbroot = $DBROOT; $dbroot =~ tr!\\!/!;
    push @CCFLAGS, "-I$dbroot/incl";
	die "Source \"$SOURCE\" after running script \"$SCRIPT$TOOLEXT\" not found !\n"
		unless ( -f "$SOURCE" );
}


if ( $SOURCE ne "$BASE$EXT" ) { copy $SOURCE, "$BASE$EXT" }

unlink "$BASE.save"; rename "$BASE$EXT", "$BASE.save";
copy "$BASE.save", "$BASE$EXT";
$filename_without_variant = "$BASE$EXT";

if ( $CPP ) {
    open(PIPE_OUT, ">$BASE.i") or croak "Can't open $BASE.i (output): $!\n";
    open(PIPE_IN, "$CPP @CCFLAGS $BASE$EXT |") or croak "Can't pipe (output): $!\n";
	while(<PIPE_IN>) { print PIPE_OUT unless /^#pragma/}
	close PIPE_IN; close PIPE_OUT;
    unlink "$BASE$EXT"; rename "$BASE.i", "$BASE$EXT";
}

if ( $CC_ID ) {
    #
    # C-compiler
    #
    local($incl) = "$TOOL/incl";
    $incl =~ s!\\!/!g;
    push @CCFLAGS, "-I$incl";

	open(FILE_OUT, ">>$BASE$EXT") or croak "Can't open $BASE$EXT (append output): $!\n";
@CXXFLAGS;
@CFLAGS;
	if ( $cc eq "" )  # no compiler in com-description (CTS 1103181)
	{
		if ($EXT eq ".c" && $language ne 'x')
		{
			$cc = $CC;
		} elsif ($EXT eq ".cpp" || $language eq 'x')
		{
			$cc = $CXX;
		} else
		{
			die "unknown extention $EXT\n";
		}
	}
	# if cpp and c-compiler => <filename>.c
	elsif ( ($cc ne $CXX) && ($EXT eq ".cpp"))
	{
		unlink "$BASE.c"; copy "$BASE$EXT", "$BASE.c";
		$filename_without_variant=~ s/\.cpp$/.c/;
		$EXT=".c";
	}
	@CCMD = $cc;
	my @cxflags = ();
	if ( $cc eq $CXX )
	{
		@cxflags = @CXXFLAGS;
        rm_CC_flags(\@CCFLAGS, \@CCFLAGS_RM);
        rm_CC_flags(\@CXXFLAGS, \@CCFLAGS_RM);
		push @CCMD, @CCFLAGS, @CXXFLAGS;
	}
	else
	{
		@cxflags = @CFLAGS;
        rm_CC_flags(\@CCFLAGS, \@CCFLAGS_RM);
        rm_CC_flags(\@CFLAGS, \@CCFLAGS_RM);
		push @CCMD, @CCFLAGS, @CFLAGS;
	}
	$cc =~ tr![\\\"]![/']!;
	# &gar PTS 1002137
	my(@ccflags)=@CCFLAGS;
	for ($i=0;$i<=$#ccflags;$i++)
	{
		$ccflags[$i] =~ tr![\\\"]![/']!;
	}
	for ($i=0;$i<=$#cxflags;$i++)
	{
		$cxflags[$i] =~ tr![\\\"]![/']!;
	}
	print FILE_OUT "$CC_ID \"\@(#)$BASE $cc @ccflags @cxflags\"$ID_END\n" unless ($NOWHATSTRING);
	$tmpBASE = $BASE; $tmpBASE =~ s/-/_/g; $tmpBASE =~ s/\+/_/g;
	print FILE_OUT "extern char *${tmpBASE}_force_whatinfo () { return( ident_cc ); }\n" unless ($NOWHATSTRING);
	close FILE_OUT;


    push @CCMD, @CCFLAGS_OVERSTEER;
	if ( ! $preprocessing ) {
        push @CCMD, "$COMPILEFLAG", $BASE.$EXT;
    } else {
        push @CCMD, "-P", $BASE.$EXT;
    }
    print "@CCMD\n" if ($ENV{NOQUIET});
    ToolTrace( "Calling @CCMD\n" );
    system(@CCMD) == 0
	   or croak "Error while executing \"@CCMD\"\n".
	   "message: $!\n";
}
else
{   #
    # NOT C-compiler
    #
     my $MF_ID_LOCAL = $MF_ID;
	 # alle außer Zeilen die mit $MF_ID beginnen nach *.i kopieren
     $MF_ID_LOCAL =~ s/\[/\\[/g; #falls $MF_ID das Metazeichen '[' enthält, dieses schützen
     open(FILE_IN, "<$BASE$EXT") or croak "Can't open $BASE$EXT (input): $!\n";
     open(FILE_OUT, ">$BASE.i") or croak "Can't open $BASE.i (output): $!\n";
	 while(<FILE_IN>) { print FILE_OUT unless /^$MF_ID_LOCAL/ }
     close FILE_IN; close FILE_OUT;
     unlink "$BASE$EXT"; rename "$BASE.i", "$BASE$EXT";
     push @CCFLAGS, @CCFLAGS_OVERSTEER;
     ToolTrace( "$CC called with: @CCFLAGS $BASE$EXT\n" );
	 print "$CC @CCFLAGS $BASE$EXT\n" if ($ENV{NOQUIET});
     system("$CC @CCFLAGS $BASE$EXT") == 0
	   or croak "Error while executing $CC @CCFLAGS $BASE$EXT\n".
	   "message: $!\n";
}

if ( ! -d "../debug" ) { mkdir("../debug", 0775); }

if ( $FOR_DEBUG ) {
	if ( -f "$BASE.dbg" ) {
		unlink("../debug/$BASE$EXT");
		rename("$BASE.dbg", "../debug/$BASE$EXT");
	}
	if ( -f "$BASE.p"   ) {
      rename("$BASE.p", "../debug/$BASE.p");
	}
}
else {
    unlink "$BASE.dbg", "../debug/$BASE$EXT", "../debug/$BASE.p";
}


if (defined $SAVED_SOURCES_DIR)
{
	File::Path::mkpath("$SAVED_SOURCES_DIR", 0, 0777) if ( ! -d "$SAVED_SOURCES_DIR" );
	unlink("$SAVED_SOURCES_DIR/$filename_without_variant");
	copy("$filename_without_variant", "$SAVED_SOURCES_DIR/$filename_without_variant");
}
else
{  unlink("$SAVED_SOURCES_DIR/$BASE$EXT"); }

unlink "$BASE$EXT"; rename "$BASE.save", "$BASE$EXT";
unlink "$filename_without_variant" if ($filename_without_variant ne "$BASE$EXT");

if ( @KEEPFLG == 0 ) {
	unlink "$BASE$EXT" if ($FOR_DEBUG);
    if ( ! $preprocessing )
    {
        unlink "$BASE.i";
    }
    unlink "$BASE.S", "$BASE$AEXT";

    # Junk created by SINIX Pascal-XT compiler
    unlink "$BASE.spec", "$BASE.bo.l", "$BASE.t";
}

sub rm_CC_flags {
    my $r_CCFLAGS = shift;
    my $r_remove = shift;
    foreach $flag (@$r_remove) {
        @$r_CCFLAGS = grep { !/^${flag}$/ } @$r_CCFLAGS;
    }
}


sub getOptFlags
{
	my @OptArray =();
	my $OptString=shift;
	while ( $OptString =~ /([^\/]*)\/(.*)/)
	{
		push @OptArray, $1;
		$OptString = $2;
	}
	push @OptArray, $OptString if ($OptString);
	return @OptArray;
}


__END__

Hier können lange Kommentare stehen
