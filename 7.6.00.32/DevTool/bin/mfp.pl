#!/usr/bin/perl
# mfp           *** internal script (called by vmake !)
#
# @(#)mfp		1997-11-18
#
# module frame to pascal source
#
# TOOL          directory for tools
# KEEPFLG       optional keep intermediate files flag
# VMAKE_VERSION optional module version
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

package mfp;
use Env;
use File::Basename;
use Carp;
do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }
ToolTrace("mfp called with @ARGV\n");

$USAGE  = "usage: mfp [-f|-q|-s] [-trace=<procedure>] [-check=no]";
$USAGE .= " [-<symbol>=<value>] [-k] source [includes]\n";

do "$TOOL/bin/mf_opts$TOOLEXT";
if ( $@ ) { print "$@"; exit }

if ( @ARGV < 1 ) { print $USAGE; exit 1 }

$SOURCE = shift;
$BASE = basename($SOURCE, ( ".p" ));

my(@INCLUDES) = "";
$CONS = "";
$TYPS =	"";

foreach $ARG (@ARGV) {
    $_ = $ARG;

	CASE: {

    if(/.*\.con/) { $CONS .= " $ARG"; last CASE }

    if(/.*\.typ/) { $TYPS .= " $ARG"; last CASE }

    push @INCLUDES ,$ARG;

    } #CASE
}
#
# Dieses mfconcom koennte eventuell entfallen.
# Fraglich ist jedoch, ob dadurch bei den folgenden Schritten
# irgendwo Probleme auftauchen.
# Es muesste einmal getestet werden. KWe 04-10-92
# IST 26.10.92: Der Scanner koennte die conditionalen Zeilen
# nicht erkennen und wuerde auch Elemente aus eigentlich
# ausgeklammerten Code-Segmenten beruecksichtigen!
#
@CONCOMOPTS;

system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, $SOURCE, "$BASE.mfp", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
	or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $SOURCE $BASE.mfp $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	"message: $!\n";

system("$TOOL/pgm/mfspl $BASE.mfp $BASE.def $BASE.use $BASE.cod") == 0
	or croak "Error while executing \"$TOOL/pgm/mfspl $BASE.mfp $BASE.def $BASE.use $BASE.cod\"\n".
	"message: $!\n";

if ( $MFPNAME eq "Y" ) {
    # Append "_" to pascal procedure and function names.

	@ARGV = ( "$BASE.def", "$BASE.cod" );
	do "$TOOL/bin/mfpname$TOOLEXT";
	if ( $@ ) { warn "Error while executing mfpname:\n", "$@"; exit }
}


system("$TOOL/pgm/mfdef $BASE.def $BASE.glv $BASE.glp") == 0
	or croak "Error while executing \"$TOOL/pgm/mfdef $BASE.def $BASE.glv $BASE.glp\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfuse $BASE.use $BASE.exv $BASE.exp") == 0
	or croak "Error while executing \"$TOOL/pgm/mfuse $BASE.use $BASE.exv $BASE.exp\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfcod $BASE.cod $BASE.con $BASE.typ $BASE.var $BASE.stm") == 0
	or croak "Error while executing \"$TOOL/pgm/mfcod $BASE.cod $BASE.con $BASE.typ $BASE.var $BASE.stm\"\n".
	"message: $!\n";

system("$TOOL/pgm/mfglp $BASE.glp $BASE.gpg $BASE.gpc") == 0
	or croak "Error while executing \"$TOOL/pgm/mfglp $BASE.glp $BASE.gpg $BASE.gpc\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfexp $BASE.exp $BASE.epc \"EXTERNAL\"") == 0
	or croak "Error while executing \"$TOOL/pgm/mfexp $BASE.exp $BASE.epc \"EXTERNAL\"\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfstm $BASE.stm $BASE.fwd $BASE.pra") == 0
	or croak "Error while executing \"$TOOL/pgm/mfstm $BASE.stm $BASE.fwd $BASE.pra\"\n".
	"message: $!\n";

if ( $CHECK eq "yes" ) { $CHKNAM =$BASE }
else { $CHKNAM = "" }

system("$TOOL/pgm/mfchk $BASE.gpc $BASE.fwd $CHKNAM") == 0
	or croak "Error while executing \"$TOOL/pgm/mfchk $BASE.gpc $BASE.fwd $CHKNAM\"\n".
	"message: $!\n";

unlink "$BASE.xxv";
rename "$BASE.exv", "$BASE.xxv";

if ( $KIND eq "SLOW" && $TRACEPROC ne "none" ) {

	local $found;
	foreach $I ("$BASE.gpc", "$BASE.epc") {
		open(FILE_IN, "<$I") or croak "Can't open $I (input): $!\n";
		while (<FILE_IN>) { $found = 1 if /$TRACEPROC/}
		close FILE_IN;
	}
	if ( $found != 1) {
		open(FILE_OUT, ">>$BASE.epc") or croak "Can't open $BASE.epc (output): $!\n";
		print FILE_OUT "PROCEDURE $TRACEPROC(tt:$TRACETYPE);EXTERNAL;\n\n";
		close FILE_OUT;
	}
    system("$TOOL/pgm/mftrc $TRACE_STEP_OUT -t $BASE.pra $BASE.prc \"$TRACEPROC\" $BASE 16") == 0
		or croak "Error while executing \"$TOOL/pgm/mftrc -t $BASE.pra $BASE.prc \"$TRACEPROC\" $BASE 16\"\n".
		"message: $!\n";
}
else { unlink "$BASE.prc"; rename "$BASE.pra", "$BASE.prc" }

system("$TOOL/pgm/mftop $BASE.gpg \"\" $BASE.top $BASE.bot") == 0
	or croak "Error while executing \"$TOOL/pgm/mftop $BASE.gpg \"\" $BASE.top $BASE.bot\"\n".
	"message: $!\n";

system("$TOOL/pgm/mfcat $BASE.cns CONST $CONS $BASE.con") == 0
	or croak "Error while executing \"$TOOL/pgm/mfcat $BASE.cns CONST $CONS $BASE.con\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfcat $BASE.tps TYPE $PASTYP $TYPS $BASE.typ") == 0
	or croak "Error while executing \"$TOOL/pgm/mfcat $BASE.tps TYPE $PASTYP $TYPS $BASE.typ\"\n".
	"message: $!\n";
system("$TOOL/pgm/mfcat $BASE.vrs VAR $BASE.xxv $BASE.glv $BASE.var") == 0
	or croak "Error while executing \"$TOOL/pgm/mfcat $BASE.vrs VAR $BASE.xxv $BASE.glv $BASE.var\"\n".
	"message: $!\n";

open(FILE_OUT, "+>$BASE.puc") or croak "Can't open $BASE.puc (output): $!\n";
foreach $I ("$PASOPT", "$BASE.top", @INCLUDES, "$BASE.cns", "$BASE.tps", "$BASE.vrs",
            "$BASE.epc", "$BASE.fwd", "$BASE.prc", "$BASE.bot") {
	if ( -f $I ) {
		open(FILE_IN, "<$I") or croak "Can't open $I (input): $!\n";
		print FILE_OUT <FILE_IN>;
		close FILE_IN;
	}
}
close FILE_OUT;

system("$TOOL/pgm/mfconcom", "-n", @CONCOMOPTS, "$BASE.puc", "$BASE.p", $MACH, $KIND, $TRACE, $COMP, $OS, $OSSPEC) == 0
	or croak "Error while executing \"$TOOL/pgm/mfconcom -n @CONCOMOPTS $BASE.puc $BASE.p $MACH $KIND $TRACE $COMP $OS $OSSPEC\"\n".
	"message: $!\n";

if ($SETBUILDINFO)
{
	require Buildinfo;
	import Buildinfo;
	Buildinfo::setbuildinfo("$BASE.p");
}

if ($SCRIPT ne "")
{
	require SDB::Make::ExecScript;
	import SDB::Make::ExecScript;

	SDB::Make::ExecScript::ExecScript("$SCRIPT", "$BASE.p");
}

open(FILE_OUT, ">>$BASE.p") or croak "Can't open $BASE.p (output): $!\n";
print FILE_OUT "\n";
print FILE_OUT "(*\n";

# 'MOD_DAT' is exported by 'vmake' !!
#print FILE_OUT "$SR_ID \"@(#)$MOD_DAT\"$ID_END\n";
#
#&if $OS in [ UNIX ]
#$tmpBASE = $BASE; $tmpBASE =~ s/-/_/; $tmpBASE =~ s/\+/_/;
#print FILE_OUT "extern char *${tmpBASE}_force_data () { return( ident_sr ); }\n";
# This function forces a reference into the data section, so the AIX, HPUX
# compiler cannot ignore that section, so we ensure that the 'what' strings
# become part of the object.
# CAUTION: This works only for the "static char" type of 'what' string!
#&endif

splice(@CONCOMOPTS, 0, 0, $KIND, $MACH, $OS, $OSSPEC, $COMP );
print FILE_OUT "$MF_ID \"@(#)$BASE @CONCOMOPTS\"$ID_END\n";
print FILE_OUT "*)\n";
close FILE_OUT;



if ( @KEEPFLG == 0 ) {
    unlink "$BASE.mfp", "$BASE.def", "$BASE.use", "$BASE.cod",
        "$BASE.glv", "$BASE.glp", "$BASE.exv", "$BASE.xxv", "$BASE.exp",
        "$BASE.con", "$BASE.typ", "$BASE.var", "$BASE.stm",
        "$BASE.gpg", "$BASE.gpc", "$BASE.epc", "$BASE.fwd",
        "$BASE.pra", "$BASE.prb", "$BASE.prc",
        "$BASE.top", "$BASE.bot",
        "$BASE.cns", "$BASE.tps", "$BASE.vrs", "$BASE.puc";
}

__END__

Hier kann ein langer Kommentar stehen!
