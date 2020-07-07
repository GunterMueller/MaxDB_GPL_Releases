#!/usr/bin/perl
# @(#) sutsingle   6.1.1   1994-10-27   kern   admin
#
# Eine Liste von Teststromnamen wird vom Standard-Input
# eingelesen und fuer jedes Element das gewuenschte Programm
# aufgerufen. Das entstehende Protokoll mit einem als korrekt
# festgeschriebenen Protokoll verglichen.
#
# Aenderungen:
#
use Env;
use File::Basename;
use File::Copy;
use Carp;
use Cwd;
use ToolLib;
require "hostname.pl";
do "$TOOL/bin/sutvars$TOOLEXT";
warn "sutstack obsolete - use sutsingle with option '-stack'";

if ($RELVER eq "R61"){
	$X_PROG = $X_UTIL;
}
else{
	$X_PROG = $X_DIAGNOSE;
}

$USAGE = "usage: sutsingle version program [dbname [dbremote]]  teststream.vdnts, ... \n";

if ( $ARGV[$#ARGV] !~ /.+\.vdnts/ ) { die "teststreams have to named with the extension '.vdnts'" }

if  ( $SUT ne "" ) {
	local $cwd = cwd(); $cwd =~ tr[\\][/];
	$SUT =~ tr[\\][/];
	if  ( $cwd !~ /$SUT/i ) {
		print "cd $SUT";
		chdir($SUT) || die "Can't change to directrory $SUT: $!";
	}
}

if ( $^O =~ /mswin32/i )  {
	$DrWtsnFile = "$WINDIR/drwtsn32.log";
	unlink "$WINDIR/drwtsn32.bak"; rename("$WINDIR/drwtsn32.log", "$WINDIR/drwtsn32.bak");
}

open(FILE_OUT, ">debuginput");
$DEBUG = "sdb";
print FILE_OUT "t\nx\nq\n";
close(FILE_OUT);

if  ( @ARGV < 2 ) { print "$USAGE"; exit 1 }

my $VERSION = shift;
my $PROGRAM = shift;

if ( $ARGV[0] !~ /.+\.vdnts/i ) { $DBNAME = shift }
if ( $ARGV[0] !~ /.+\.vdnts/i ) { $DBREMOTE = shift }

if ( ! $DBNAME ) {
    croak "ERROR: name of the database is not defined!\n";
}

$_ = $VERSION;
CASE: {
	if (/^fast$/i) {
		$DBPROG = "$DBROOT/pgm/kernel$PROG_EXT";
		$SUTOPTION = "-fast";
		last CASE
	}

	if (/^slow$/i) {
		$DBPROG = "$DBROOT/pgm/slowknl$PROG_EXT";
		$SUTOPTION = "-slow";
		last CASE
	}

	if (/^quick$/i) {
		$DBPROG = "$DBROOT/pgm/quickknl$PROG_EXT";
		$SUTOPTION = "-quick";
		last CASE
	}

	print "$USAGE";
	print "version: fast|quick|slow\n";
	exit 1;
} # CASE

$ENV{SUTOPTION} = $SUTOPTION;
my $PVERS = substr($VERSION, 0, 1);
my $PROG = "$DBROOT/pgm/$PROGRAM";

if ( $PROGRAM =~ /^sqlterm/i ) { $Sqlterm  = 1;  } else { $Sqlterm  = 0; }
if ( $DBBIGCMD )       { $Dbbigcmd = 1;  } else { $Dbbigcmd = 0; }

if  ( ! -x $PROG ) { print "$USAGE"; die "program $PROG not executable\n" }
if  ( ! -f $DBPROG ) { die "can't find $DBPROG\n" }

$ENV{SERVERDB} = $DBNAME;
if  ( ! -r "$PARAMDIR/$DBNAME" ) {
    print "$USAGE";
    croak "param file $PARAMDIR/$DBNAME not readable\n";
}
system("x_stop $DBNAME >$NULL 2>&1");

if ( $DBREMOTE ) {

    $ENV{DBREMOTE} = $DBREMOTE;
    if ( $DBREMOTE ne "" ) {
		do "$TOOL/bin/xinitdst$TOOLEXT";
		if ( $@ ) { die "Error while executing xinitdst:\n", "$@"; exit }
		$HASCORE = "init";
	}
    if ( ! -r "$PARAMDIR/$DBREMOTE" ) {
        print "$USAGE";
        croak "param file $PARAMDIR/$DBREMOTE not readable\n";
	}
    system("x_stop $DBREMOTE >$NULL 2>&1");

}
#	Ermittle ob die Variable DBUNICODE gesetzt ist
#	und setze danach den XPARAM Parameter.

$DEF_CODE = "ASCII";

if ($RELVER ne "R61"){
	if ( ! $DBUNICODE ) {
		system("$DBROOT/pgm/putparam $DBNAME _UNICODE NO") == 0
			or croak "Error while executing \"$DBROOT/pgm/putparam $DBNAME _UNICODE NO\"\n".
			"message: $!\n";
	}
	else {
	    	system("$DBROOT/pgm/putparam $DBNAME _UNICODE YES") == 0
		    	or croak "Error while executing \"$DBROOT/pgm/putparam $DBNAME _UNICODE YES\"\n".
			    "message: $!\n";
		if ( $DEFCODE eq "" ) { $DEF_CODE = "ASCII" } else { $DEF_CODE = $DEFCODE }
	}
}

#	Ermittle den Namen des Protokolls durch Aufruf des Programmes
#	und auflisten der dadurch erzeugten Datei.

$CI_PROT = ToolLib::ProtokollName($PROG);

if ( $TMP eq "" ) { $TMPDIR = "/tmp" } else { $TMPDIR = $TMP }

# Variablen fuer DBNAME
$DBPARAM = "$DBROOT/pgm/getparam $DBNAME";
$DBPARAM = "$DBROOT/pgm/getparam $DBNAME";

if ($RELVER eq "R61"){
	$RUNDIR  = `$DBPARAM RUNDIRECTORY`;		chomp $RUNDIR;
	$OPMSG3  = `$DBPARAM DIAGFILE`;			chomp $OPMSG3;
	$KDUMP   = `$DBPARAM KERNELDUMPFILE`;	chomp $KDUMP;
	$KNLDIAG = `$DBPARAM DIAGFILE`;			chomp $KNLDIAG;
}
else{
	$RUNDIR  = `$DBPARAM RUNDIRECTORY`;		chomp $RUNDIR;
	$OPMSG3  = `$DBPARAM _KERNELDIAGFILE`;	chomp $OPMSG3;
	$KDUMP   = `$DBPARAM  _KERNELDUMPFILE`; chomp $KDUMP;
	$KNLDIAG = `$DBPARAM _KERNELDIAGFILE`;	chomp $KNLDIAG;
	$RDUMP   = `$DBPARAM  _RTEDUMPFILE`;	chomp $RDUMP;
}

if ( $DBREMOTE ) {
    # Variablen fuer DBREMOTE
    $RMDBPARAM= "$DBROOT/pgm/getparam $DBREMOTE";
    $RMRUNDIR = `$RMDBPARAM RUNDIRECTORY`;			chomp $RMRUNDIR;
    $RMOPMSG3 = `$RMDBPARAM _KERNELDIAGFILE`;		chomp $RMOPMSG3;
    $RMKDUMP  = `$RMDBPARAM  _KERNELDUMPFILE`;		chomp $RMKDUMP;
    $RMRDUMP  = `$RMDBPARAM  _RTEDUMPFILE`;			chomp $RMRDUMP;
}

$dir = dirname($KNLDIAG);
if ( $dir eq "." or $dir eq ".." ) { $DIAG = "$RUNDIR/$KNLDIAG" } else { $DIAG = $KNLDIAG }
$dir = dirname($KDUMP);
if ( $dir eq "." or $dir eq ".." ) { $DUMP = "$RUNDIR/$KDUMP" } else { $DUMP = $KDUMP }
$dir = dirname($RDUMP);
if ( $dir eq "." or $dir eq ".." ) { $RDMP = "$RUNDIR/$RDUMP" } else { $RDMP = $RDUMP }

if ( $DBREMOTE )  {
	$dir = dirname($RMOPMSG3);
	if ( $dir eq "." or $dir eq ".." ) { $RMDIAG = "$RMRUNDIR/$RMOPMSG3" } else { $RMDIAG = $RMOPMSG3 }
	$dir = dirname($RMKDUMP);
	if ( $dir eq "." or $dir eq ".." ) { $RMDUMP = "$RMRUNDIR/$RMKDUMP" } else { $RMDUMP = $RMKDUMP }
	$dir = dirname($RMDUMP);
	if ( $dir eq "." or $dir eq ".." ) { $RMRDMP = "$RMRUNDIR/$RMRDUMP" } else { $RDMP = $RDUMP }
}

# Aktualisierung des Paramfiles! Durch den Vergleich der Kernel-
# version des Paramfiles und des Kerns beim Hochfahren musste
# vor Beginn der Testlaeufe immer das Paramfile von Hand erzeugt
# werden, wenn der Kern gestempelt wurde. Dies entfaellt jetzt.

system("x_param -u superdba,colduser -d $DBNAME -v BCHECK") == 0
	or croak "Error while executing \"x_param -u superdba,colduser -d $DBNAME -v BCHECK\"\n".
	"message: $!\n";
$Returncode = $?;
if ( $Returncode != 0 ) {
   print "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n\n";
   print "ERROR BY EXECUTING XPARAM FOR $DBNAME !\n";
   print "Returncode is $Returncode\n";
   print "For further informations see xparam.prot and xpcheck.prot!\n\n";
   print "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n";

   print "One more try! x_param BCHECK ...\n";
   system("x_param -u superdba,colduser -d $DBNAME -v BCHECK") == 0
		or croak "Error while executing \"x_param -u superdba,colduser -d $DBNAME -v BCHECK\"\n".
		"message: $!\n";
}

$Returncode = $?;
if ( $Returncode != 0 ) { print "Error by executing xparam!\n"; exit 1 }

# Note, that it is important to rename old style xuser files, too...
if ( -f "$HOME\\.XUSER"    ) { unlink "$HOME/.XUSER.sav"; rename("$HOME/.XUSER",    "$HOME/.XUSER.sav") }
if ( -f "$HOME\\.XUSER.62" ) { unlink "$HOME/.XUS62.sav"; rename("$HOME/.XUSER.62", "$HOME/.XUS62.sav") }

unlink "lock";
$DBHIF = $TERM;

print scalar localtime, "\n";
if ( $DBREMOTE ) { print " DBNAME = $DBNAME DBREMOTE = $DBREMOTE\n" } else { print " DBNAME = $DBNAME\n" }

if ( ! $REKURS ) {
	open(FILE_OUT, ">$AllProt"); print FILE_OUT scalar localtime, "\n"; close(FILE_OUT);
	open(FILE_OUT, ">$AllCore"); close(FILE_OUT);
	open(FILE_OUT, ">$AllDiff"); close(FILE_OUT);
	open(FILE_OUT, ">$AllOk");   close(FILE_OUT);
}

#===================================================================
# Abarbeiten der Teststroeme
#===================================================================
# Die SUTs laufen mit einem vorgefertigten Paramfile

# transsize: 100 datasize: 400 code: ASCII archivesize: 200
install_paramfile( $DEF_CODE );
@TFNS = @ARGV;
foreach $TFN ( @TFNS ) {

	$TFN =~ s/\.vdnts//i;
    while ( -f "lock" ) { sleep 10 }

    # fuer eine gleichmaessige Ausgabe in 'allsut'
	my $PTEST = $TFN." "x(8-length($TFN));
	open(FILE_IN, "<CONNECT.cmd");
	while(<FILE_IN>) {
		if ( /^CONNECT &1 IDENTIFIED BY &2 &3 &4 ISOLATION LEVEL (\d+).*/i ) {
			$LEVEL = $1; last;
		}
	}
	close(FILE_IN);
	open(FILE_OUT, ">>$AllProt");
    print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST\n"; close(FILE_OUT);

    if ( ! -f "$TFN.vdnts" ) {
		open(FILE_OUT, ">>$AllProt");
        print FILE_OUT "NOT FOUND: $TFN.vdnts\n"; close(FILE_OUT);
        print "NOT FOUND: $TFN.vdnts\n";
        next;
	}

    $ProtDir = "$SUT/stack";
    # Ueberbleibsel des alten Testlaufs entfernen
	unlink <$RUNDIR/core*>, <*host*>, "shmcore", <*.HOST*>, $CI_PROT;
    unlink "$ProtDir/$TFN.core", "$ProtDir/$TFN.dump", "$ProtDir/$TFN.rtedump", "$ProtDir/$TFN.prot", "$ProtDir/$TFN.pdiff";
	unlink <$RMRUNDIR/core*>;

    # Da einige Teststroeme ein spezielles Paramfile benoetigen
    # wird vor dem eigentlichen Lauf das entsprechende Paramfile
    # kopiert.

	CASE: {
	if ( $TFN =~ /JOIN2TAB|LINKDDL|NIST3|USERTEST/i) {
           install_paramfile( EBCDIC );
		   last CASE; }
	if ( $TFN =~ /STAV|STAU|UPDSTAT/i) {
           install_paramfile( ASCII );
		   last CASE; }
	if ( $TFN =~ /SELFET1|SELFET2/i) {
           copy("$RUNDIR/xinitdb.sav", "$RUNDIR/xinitdb.sav2");
           system("$DBROOT/pgm/putparam $DBNAME _MAXSERVERDB 1") == 0
				or croak "Error while executing \"$DBROOT/pgm/putparam $DBNAME _MAXSERVERDB 1\"\n".
				"message: $!\n";
		   $REKURS = "X"; $ENV{'REKURS'} = $REKURS;
		   local @ARGV = ($VERSION, "slowci$PROG_EXT", $DBNAME, "XINITDB.vdnts");
			do "$TOOL/bin/sutsingle$TOOLEXT";
			if ( $@ ) { die "Error while executing sutsingle:\n", "$@"; exit }
		   undef($REKURS);
		   last CASE; }
	} #CASE


	CASE: {
		if ( $TFN =~ /^Y.*/ ) { $RESTORE = "YRESTORE"; last CASE; }
		if ( $TFN =~ /^W.*/ ) { $RESTORE = "WRESTORE"; last CASE; }
		$RESTORE = "XRESTORE";
	}

    if ( $DBREMOTE ) {
		copy("$RESTORE.cmd", "$RESTORE.sav");
		copy("XRESTDST.cmd", "$RESTORE.cmd");
    }

    if ( $Dbbigcmd != 0 ) {
        # Um groessere SQL Kommandos zu erhalten werden durch
        # das XCI Kommando "BIGCMD <offset>" Blanks vor jedes
        # SQL  Kommando geschrieben.
		open(FILE_OUT, ">>$RESTORE.cmd");
        print FILE_OUT "BIGCMD $DBBIGCMD !\n"; close(FILE_OUT);
    }

    if ( $DBUNICODE =~ /^xci/i ) {
		if ( $DBLANG ne "" ) {
			# Da der XCI hier garantiert nur Zeichen aus dem ASCII-Subset der
			# entsprechenden Kodierung schickt, wird ihm mit TERMINAL ASCII
			# mitgeteilt, dass er bei der Ausgabe von UNICODE-Spalten nur
			# halb soviel Platz benoetigt, wie der Kern ihm mitteilt.
			open(FILE_OUT, ">>$RESTORE.cmd");
			print FILE_OUT "TERMINAL ASCII!\n"; close(FILE_OUT);
		}
		else {
			# Diese Kombination der Umgebungsvariablen sagt aus, dass
			# der XCI die Auftragsschnittstelle als Unicode-Client
			# bedienen soll. Das wird ihm durch Aufruf des Kommandos
			# TERMINAL UNICODE am Anfang eines Testlaufs mitgeteilt.
			open(FILE_OUT, ">>$RESTORE.cmd");
			print FILE_OUT "TERMINAL UNICODE!\n"; close(FILE_OUT);
		}
    }
    if ( $DBUPDSTAT ) {
		# Hier wird jetzt das Diagnose-Kommando abgesetzt, das den Kern
		# dazu bringt, beim UPDATE STATISTICS immer eine grosse Zahl als
		# Page-Anzahl einzutragen. Dadurch werden dann in der Strategie-
		# suche auch mal einige Indizes genommen statt immer nur Table Scan...
		local $TMP = "$::TMP/REST$$";
		copy("$RESTORE.cmd", $TMP);
		open(FILE_OUT, ">$RESTORE.cmd"); open(FILE_IN, "<$TMP");
		while(<FILE_IN>) {
			print FILE_OUT $_;
			print FILE_OUT "DIAGNOSE OPTIMIZE UPDSTAT ON\n" if /autosave on/;
		}
		close(FILE_IN); close(FILE_OUT);
		unlink($TMP);
    }

    if ( $TFN =~ /^ENTRY/i ) {
	    unlink "$TFN.ori"; rename("$TFN.vdnts", "$TFN.ori") || warn "Can't rename $TFN.vdnts to $TFN.ori: $!";
	    #system("$TOOL/Posix/sed -e \"s/<serverdb>/$DBNAME/g\" < $TFN.ori > $TFN.vdnts");
		open(FILE_IN, "<$TFN.ori" ); open(FILE_OUT, ">$TFN.vdnts" );
		while(<FILE_IN>) {
			s/<serverdb>/$DBNAME/g;
			print FILE_OUT $_;
		}
		close(FILE_IN); close(FILE_OUT);
    }

    if ($RELVER eq "R61"){
		# Specify the kernel version in the registry
		$main::HKEY_LOCAL_MACHINE->Open("SYSTEM\\CurrentControlSet\\Services\\ADABAS-$DBNAME", $RegADA)
			|| die "Cannot open registry key!\n";

		$RegADA->SetValueEx('ImagePath', 0, REG_EXPAND_SZ, "$DBPROG")
			|| die "Cannot register kernel version $VERSION in the registry!\n";
	}

    if ( $DBREMOTE ) {
	# - Weil die verteilten Tests nicht jedesmal ein neues INIT CONFIG
	#   machen, laufen Dinge wie Stamps, Session, etc. auseinander.
	#   Es gibt Testlaeufe, wo solche Dinge explizit selektiert werden.
	# - Nach einem Crash (oder einem Work rolled back beim Drop)
	#   existieren hoechstwahrscheinlich noch ueberfluessige Tabellen.
	# In obigen Faellen wird daher vorm Start xinitdst aufgerufen...
		$initNeeded = 0; #false
		if ( $HASCORE ne "" ) {
			if ( $HASCORE ne "init" ) { $initNeeded = 1; }
			$HASCORE = "";
		}
		else {
			if ( $TFN =~ /^(ACCOUNT|AK03TEST|DIAGNOSE|FD|H01SAVE|H02ALT|LOCKNORM|N67UPD|ORACLE|P02CREVI|S05STATI|S07REST2|STAMP|SUBTRANS|SYSDBA)/i )
			{$initNeeded = 1 }
		}

		if ( $initNeeded ) {
			print "Initialize distributed configuration ... ";
			do "$TOOL/bin/xinitdst$TOOLEXT";
			if ( $@ ) { die "Error while executing xinitdst:\n", "$@"; exit }
			print "done\n";
			print " $PVERS $LEVEL $PTEST ... ";
		}
		else {
			print " $PVERS $LEVEL $PTEST ... ";
			system("x_start $SUTOPTION $DBNAME   > $NULL") == 0
				or croak "Error while executing \"x_start $SUTOPTION $DBNAME   > $NULL\"\n".
				"message: $!\n";
			system("x_start $SUTOPTION $DBREMOTE > $NULL") == 0
				or croak "Error while executing \"x_start $SUTOPTION $DBREMOTE > $NULL\"\n".
				"message: $!\n";
		}
		system("$X_PROG -d $DBNAME -u superdba,colduser restart") == 0
			or croak "Error while executing \"$X_PROG -d $DBNAME -u superdba,colduser restart\"\n".
			"message: $!\n";
  		if ( $DBUPDSTAT ne "" ) {
	    system("$X_PROG -d $DBNAME -u superdba,colduser diagnose optimize updstat on") == 0
			or croak "Error while executing \"$X_PROG -d $DBNAME -u superdba,colduser diagnose optimize updstat on\"\n".
			"message: $!\n";
		}
  }
	else {
		print " $PVERS $LEVEL $PTEST ... ";

			if ($RELVER eq "R61"){
		system("x_start $DBNAME > $NULL") == 0
			or croak "Error while executing \"x_start $DBNAME > $NULL\"\n".
			"message: $!\n";
			}
			else{
		system("x_start $SUTOPTION $DBNAME > $NULL") == 0
			or croak "Error while executing \"x_start $SUTOPTION $DBNAME > $NULL\"\n".
			"message: $!\n";
            }
    }

	print scalar localtime;

    # Soll anstatt des slowci sqlterm verwendet werden, dann muss
    # vorher ein restore durchgefuehrt werden.

    if ( $Sqlterm != 0 ) {
       system("$DBROOT/pgm/slowci -b XRESTORE.cmd > $NULL") == 0
			or croak "Error while executing \"$DBROOT/pgm/slowci -b XRESTORE.cmd > $NULL\"\n".
			"message: $!\n";
       $SQLOPT = "-X";
	   system("$DBROOT/pgm/sqlterm $TFN.vdnts > $NULL") == 0
			or croak "Error while executing \"$DBROOT/pgm/sqlterm $TFN.vdnts > $NULL\"\n".
			"message: $!\n";
	   unlink $CI_PROT;
       rename("$ProtDir/$TFN.prot", $CI_PROT) || warn "Can't rename $TFN.prot to $CI_PROT: $!";
    }
	else {
		local $core = 1;
		system("$PROG -d $DBNAME -b $TFN.vdnts > $NULL");
		open(FILE_IN, "$CI_PROT") || die "Can't open $CI_PROT (input): $!\n";
		seek(FILE_IN, -50, 2) || warn "seek-error in $CI_PROT: $!\n";
		while(<FILE_IN>) {
			$core = 0 if /vdnexit/i;
		}
		close(FILE_IN);
    }

    # Ausgabe der '...' zwischen 'slowci' und 'diff'
    print "... ";

	open(FILE_OUT, ">>$CI_PROT");
    print FILE_OUT " \n";  # .pibm have empty line beyond end
	close(FILE_OUT);

    if ( $DBREMOTE ) {
		system("$X_PROG -d $DBNAME -u superdba,colduser shutdown > $NULL 2>&1") == 0
			or croak "Error while executing \"$X_PROG -d $DBNAME -u superdba,colduser shutdown > $NULL 2>&1\"\n".
			"message: $!\n";
		system("x_stop $DBREMOTE > $NULL 2>&1");
    }

    system("x_stop  $DBNAME >$NULL 2>&1");


	@ARGV = ( "-d", $DBNAME, "ax" );
    do "$TOOL/bin/kernprot$TOOLEXT";
	if ( $@ ) { die "Error while executing kernprot:\n", "$@"; exit }

    unlink "$ProtDir/$TFN.vprot", "$ProtDir/$TFN.s2", "$ProtDir/$TFN.s3", "$ProtDir/$TFN.sort";
    rename("$DBNAME.prt", "$ProtDir/$TFN.vprot");
	open(FILE_OUT, ">$ProtDir/$TFN.s2"); open(FILE_IN, "$ProtDir/$TFN.vprot");
	while(<FILE_IN>) {

         if ( /maxstack\s*:\s*(\d+)/ ) {

		    $help = $1;
            $i = $1;
		    if ( length($help) eq 5 )
                { $i = "0$1" }
		    else
                { if ( length($help) eq 4 )
                   { $i = "00$1" } }

    		print FILE_OUT $i, " ", "$TFN:", $.;
        }


	}
	close(FILE_IN); close(FILE_OUT);

    system("$TOOL/Posix/sort -o $ProtDir/$TFN.s3 -r $ProtDir/$TFN.s2");
    unlink "$ProtDir/$TFN.s2";

	open(FILE_IN, "$ProtDir/$TFN.s3"); open(FILE_OUT, ">$ProtDir/$TFN.sort");
	while(<FILE_IN>) {
        chomp;
		if (41...eof()) { next }
		print FILE_OUT $_ ;
	}
	close(FILE_IN); close(FILE_OUT);
    unlink "$ProtDir/$TFN.s3";
    unlink "$ProtDir/$TFN.s2";

    $\ = "";
	@ARGV = ( $CI_PROT );
    do "$TOOL/bin/sed_stack$TOOLEXT";
	if ( $@ ) { die "Error while executing sed_stack:\n", "$@"; exit }


	#===================================================================
	# Zuruecksetzen des Paramfiles
	#===================================================================

	install_paramfile( $DEF_CODE );


    #  Absturz des lokalen DB-Kerns:
	@CORE = <$RUNDIR/core*>;
    foreach $CORE (@CORE) {

		if ( ! -f $CORE ) { last }
        if ( $DBREMOTE ne "" ) { $local = " (local)" }
		open(FILE_OUT, ">>$AllSut");
        print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST core$local\n"; close(FILE_OUT);
		open(FILE_OUT, ">>$AllCore");
        print FILE_OUT "$TFN.prot\n"; close(FILE_OUT);
		open(FILE_OUT, ">>$CI_PROT");
        print FILE_OUT "$TFN\n"; close(FILE_OUT);

		open(FILE_OUT, ">>$CI_PROT");
        print FILE_OUT "$DEBUG $DBPROG $CORE $WRK/$VERSION/tmp\n";
        system("$DEBUG $DBPROG $CORE $WRK/$VERSION/tmp <debuginput >>$CI_PROT 2>&1");

        if  ( -r $DIAG ) {
            print FILE_OUT "save $DIAG\n";
			open(DIAG, "$DIAG");
			while(<DIAG>) { print FILE_OUT $_ unless / +$/ }
			close(DIAG);
        }
		close(FILE_OUT);

        if ( ! $ERASE ) {
			unlink "$ProtDir/$TFN.core", "$ProtDir/$TFN.dump", "$ProtDir/$TFN.rtedump";
           rename($CORE, "$ProtDir/$TFN.core") || warn "Can't rename $CORE to $ProtDir/$TFN.core: $!";
           rename($DUMP, "$ProtDir/$TFN.dump") || warn "Can't rename $DUMP to $ProtDir/$TFN.dump: $!";
           rename($RDMP, "$ProtDir/$TFN.rtedump") || warn "Can't rename $RDMP to $ProtDir/$TFN.rtedump: $!";
        }
		else {
           unlink $CORE, $DUMP, $RDMP;

           # Touchen eines dummys, der ein schnelleres erkennen
           # eines cores ermoeglicht!
           system("$TOOL/Posix/touch $ProtDir/$TFN.core");
           system("$TOOL/Posix/touch $ProtDir/$TFN.dump");
           system("$TOOL/Posix/touch $ProtDir/$TFN.rtedump");
        }
        $HASCORE = "local";
    } #foreach

    if ( $DBREMOTE ) {
		@CORE = <$RMRUNDIR/core*>;
		foreach $CORE (@CORE) {
			if ( ! -f $CORE ) { last }
			if ( $DBREMOTE ne "" ) { $remote = " (remote)" }
			open(FILE_OUT, ">>$AllSut");
			print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST core$remote\n"; close(FILE_OUT);
			open(FILE_OUT, ">>$AllCore");
			print FILE_OUT "$TFN.prot\n"; close(FILE_OUT);
			open(FILE_OUT, ">>$CI_PROT");
			print FILE_OUT "$TFN\n" ; close(FILE_OUT);

			open(FILE_OUT, ">>$CI_PROT");
			print FILE_OUT "$DEBUG $DBPROG $CORE $WRK/$VERSION/tmp\n";
			system("$DEBUG $DBPROG $CORE $WRK/$VERSION/tmp <debuginput >>$CI_PROT 2>&1") == 0
				 or croak "Error while executing \"$DEBUG $DBPROG $CORE $WRK/$VERSION/tmp <debuginput >>$CI_PROT 2>&1\"\n".
				 "message: $!\n";

			if  ( -r $RMDIAG ) {
				print FILE_OUT "save $RMDIAG\n";
				open(DIAG, "$RMDIAG");
				while(<RMDIAG>) { print FILE_OUT $_ unless / +$/ }
				close(DIAG);
			}
			close(FILE_OUT);

			if ( ! $ERASE ) {
				unlink "$ProtDir/$TFN.core", "$ProtDir/$TFN.dump", "$ProtDir/$TFN.rtedump";
			   rename($CORE, "$ProtDir/$TFN.core") || warn "Can't rename $CORE to $ProtDir/$TFN.core: $!";
			   rename($RMDUMP, "$ProtDir/$TFN.dump") || warn "Can't rename $RMDUMP to $ProtDir/$TFN.dump: $!";
			   rename($RMRDMP, "$ProtDir/$TFN.rtedump") || warn "Can't rename $RMRDMP to $ProtDir/$TFN.rtedump: $!";
			}
			else {
				unlink $CORE, $DUMP, $RDMP;

				# Touchen eines dummys, der ein schnelleres erkennen
				# eines cores ermoeglicht!
				system("$TOOL/Posix/touch $ProtDir/$TFN.core");
				system("$TOOL/Posix/touch $ProtDir/$TFN.dump");
				system("$TOOL/Posix/touch $ProtDir/$TFN.rtedump");
			}
			$HASCORE = "remote";
		} #foreach
    }


    #  Absturz des Programmes (slowci o.ae.):
	@CORE = <core*>;
	foreach $CORE (@CORE) {
		if ( ! -f $CORE ) { last }
		print FILE_OUT "$DEBUG $PROG $CORE $WRK/$VERSION/tmp\n";
		system("$DEBUG $PROG $CORE $WRK/$VERSION/tmp <debuginput >>$CI_PROT 2>&1") == 0
			 or croak "Error while executing \"$DEBUG $PROG $CORE $WRK/$VERSION/tmp <debuginput >>$CI_PROT 2>&1\"\n".
			 "message: $!\n";
		unlink $CORE;
		$HASCORE = $PROG;
     }

    if ( $Dbbigcmd != 0 or $$DBUPDSTAT ) {
        # jetzt wrid das XCI Kommando "BIGCMD <offset>"
        # wieder entfernt
		unlink "$RESTORE.tmp";
        rename("$RESTORE.cmd", "$RESTORE.tmp") || warn "Can't rename $RESTORE.cmd to $RESTORE.tmp: $!";
        #system("$TOOL/Posix/sed -e '/^BIGCMD/'d -e '/^TERMINAL/'d -e 's/\* if rc/if rc/' < $RESTORE.tmp > $RESTORE.cmd");
		open(FILE_IN, "<$RESTORE.tmp" ); open(FILE_OUT, ">$RESTORE.cmd" );
		while(<FILE_IN>) {
			next if /^BIGCMD/ || /^TERMINAL/ || / UPDSTAT ON/;
			s/\* if rc/if rc/;
			print FILE_OUT $_;
		}
		close(FILE_IN); close(FILE_OUT);
        unlink  "$RESTORE.tmp";
	}
	elsif ( $DBREMOTE ) {
		unlink "$RESTORE.cmd";
		rename("$RESTORE.sav", "$RESTORE.cmd") || warn "Can't rename $RESTORE.sav to $RESTORE.cmd: $!";
    }


    #===============================================================
    #-------jetzt wird das Protokoll so verbogen, dass moeglichst viele
    #	der laestigen 'ewigen' Differenzen wegfallen (Datum, Zeit, ...)
    #===============================================================

    # We do this in a subshell to leave DBNAME undisturbed...
	if ( $DBREMOTE ) {
		$DBLOCAL = $DBNAME;
		$ENV{DBLOCAL} = $DBLOCAL;
		local $DBNAME = $DBREMOTE;
		# neues $DBNAME wird in sed_sut genutzt!
		@ARGV = ( "$CI_PROT", "$TMPDIR/$TFN.prot" );
		do "$TOOL/bin/sed_sut$TOOLEXT";
		if ( $@ ) { die "Error while executing sed_sut:\n", "$@"; exit }
	} else {
		@ARGV = ( "$CI_PROT", "$TMPDIR/$TFN.prot" );
		do "$TOOL/bin/sed_sut$TOOLEXT";
		if ( $@ ) { die "Error while executing sed_sut:\n", "$@"; exit }
	}

    if  ( $LEVEL != 1 ) {
        #  Aendern des Isolation-Levels auf 1; der urspruengliche I.L.
        #  ist nur noch anhand der Zeit der letzten Aenderung bzw. an
        #  der Directory, in der das Protokoll steht, zu erkennen.
        #  Diese Loesung war leider noetig, da 'diff' kein 'ignore'
        #  kennt.
		@ARGV = ( "$LEVEL", "$TMPDIR/$TFN.prot" );
        do "$TOOL/bin/sed_sut-lvl$TOOLEXT";
		if ( $@ ) { die "Error while executing sed_sut-lvl:\n", "$@"; exit }
    }

    if ( $TFN =~ /ENTRY/ ) {
	    unlink "$TFN.vdnts";
	    rename "$TFN.ori", "$TFN.vdnts";
	}

    if ( $DBUNICODE ) {
        # Aendern der Ausgaben, die aufgrund der nur 9 Byte langen
        # Identifier im 'UNICODE'-Lauf anders sind als im normalen
        # Lauf
		@ARGV = ( "$TMPDIR/$TFN.prot" );
        do "$TOOL/bin/sed_sut-uni$TOOLEXT";
		if ( $@ ) { die "Error while executing sed_sut-uni:\n", "$@"; exit }
    }

    unlink $CI_PROT;

    # Die Fehlerpositionen werden aus dem Protokoll entfernt.
    if ( $Sqlterm or $Dbbigcmd ) {
		@ARGC = ( "$TMPDIR/$TFN.prot", "$TMPDIR/PROT" );
		do "$TOOL/bin/sed_term$TOOLEXT";
		if ( $@ ) { die "Error while executing sed_term:\n", "$@"; exit }
    }

    #	echo "analyze differences"

    $DIFFEXT = "punix";
    if ( $VERSION =~ /fast|quick/ ) { if ( -s "$TFN.tpunx" ) { $DIFFEXT = "tpunx" } }

    if ( $DBUNICODE =~ /^xci/i and  $DBLANG eq "" and -s "$TFN.cunix" ) {
		# Unicode-Auftragsschnittstelle mit eigenem Referenzprotokoll...
        $DIFFEXT = "cunix";
    }
	elsif ( $DBREMOTE and -s "$TFN.vunix" ) {
		# # Strategie-Testlauf mit eigenem Referenzprotokoll...
		$DIFFEXT = "sunix";
    }
	elsif ( $DBREMOTE and -s "$TFN.vunix" ) {
		# Verteilungs-Testlauf mit eigenem Referenzprotokoll...
		$DIFFEXT = "vunix";
    }
	else {
        if ( $DBUNICODE ) {
            if ( $DEF_CODE eq UNICODE ) {
                if         ( -s "$TFN.upnix" ) { $DIFFEXT = "upnix" }
				else { if  ( -s "$TFN.uunix" ) { $DIFFEXT = "uunix" } }
            }
			else { if  ( -s "$TFN.uunix" ) { $DIFFEXT = "uunix" } }
        }
    }

    if  ( -s "$TFN.$DIFFEXT" ) {
        $DIFFRC = 0;
        if ( $Sqlterm or $Dbbigcmd ) {
			@ARGV = ( "$TFN.$DIFFEXT", "$TMPDIR/PUNIX" );
			do "$TOOL/bin/sed_term$TOOLEXT";
			if ( $@ ) { die "Error while executing sed_term:\n", "$@"; exit }
			system("$DIFF $TMPDIR/PUNIX $TMPDIR/PROT > $ProtDir/$TFN.pdiff");
			$DIFFRC = $?;
			unlink "$TMPDIR/PUNIX", "$TMPDIR/PROT";
    	}
		 else {
    	       $DIFFRC = system("$DIFF $TFN.$DIFFEXT $TMPDIR/$TFN.prot > $ProtDir/$TFN.pdiff");
    	 }
        if ( $DIFFRC == 2 ) {
			open(FILE_OUT, ">>$ProtDir/$TFN.pdiff");
           print FILE_OUT "DIFF-ERROR HAPPENED\n";
        }
    }
	else {
		open(FILE_OUT, ">$ProtDir/$TFN.pdiff");
        # kein Protokoll zum Vergleichen da !
        print FILE_OUT "new test without protocol: $TFN\n";
    }

	close(FILE_OUT);
	if ( $DIFFEXT =~ /punix|tpunx/) { $DIFFEXT = "" }

    if ( $DBREMOTE ) {
		if ( -s "$ProtDir/$TFN.pdiff" ) {
			$diagnostic = `checkDst $TMPDIR/$TFN.prot $TFN.punix $ProtDir/$TFN.pdiff`;
			if ( $diagnostic ne "" ) {
				unlink "$ProtDir/$TFN.pdiff";
				$DIFFEXT = "* ($diagnostic)";
			}

		}
		# In LOCKNORM the -8026 doesnt get into the pdiff, since it is
		# ok and written into the vunix. Nevertheless we must init the db.
		if ( $TFN eq LOCKNORM ) { $HASCORE = "taskLimit" }
    }


    if  ( -s "$ProtDir/$TFN.pdiff" ) {
        $CHARS = `$TOOL/Posix/wc -c < $ProtDir/$TFN.pdiff`; chomp $CHARS;
		open(FILE_OUT, ">>$AllSut");
        print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST * $CHARS $DIFFEXT\n";
        if ( -f "$ProtDir/$TFN.core" ) {
			if ( $DBREMOTE ne "" ) {
				print "core ($HASCORE)\n";
			}
			else {
				print "core\n";
			}
        }
		else {
           print "differences  $CHARS\n";
        }

	# For a distributed test we have to reset the serverdbs to a clean
	# state not only after a core, but also if the server shuts down
	# due to a full arch log or error 600/650 because of a full trans log.
		if ( $DBREMOTE ne "" and $HASCORE eq "" ) {
			open(FILE_IN, "<$ProtDir/$TFN.pdiff");
			while ( <FILE_IN> ) {
				$HASHCORE = "tooFull" if /ERROR    6[50]0 / || /ERROR -8026/ ||/NOT ACCESSIBLE, connection broken/;
			}
			close(FILE_IN);
	    }
		open(FILE_OUT, ">>$AllDiff");
        print FILE_OUT "$ProtDir/$TFN.pdiff";
		unlink "$ProtDir/$TFN.prot";
        rename("$TMPDIR/$TFN.prot", "$ProtDir/$TFN.prot") || warn "Can't rename $TMPDIR/$TFN.prot to $ProtDir/$TFN.prot: $!";
    }
	else {
		open(FILE_OUT, ">>$AllSut");
        print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST $DIFFEXT\n";
        if ( -f "$ProtDir/$TFN.core" ) {
		    if ( $DBREMOTE ne "" ){ print "core ($HASCORE)\n" } else { print "core\n" }
        }
		else { print "ok\n" }

		open(FILE_OUT, ">>$AllOk");
        print FILE_OUT ToolLib::Timestamp(1), "\t$PVERS $LEVEL $PTEST";
        unlink "$ProtDir/$TFN.pdiff", "$TMPDIR/$TFN.prot";
    }
	if ( $TFN =~ /SELFET1|SELFET2/ ) {
           unlink("$RUNDIR/xinitdb.sav");
           rename("$RUNDIR/xinitdb.sav2", "$RUNDIR/xinitdb.sav");
           system("$DBROOT/pgm/putparam $DBNAME _MAXSERVERDB 2") == 0
				 or croak "Error while executing \"$DBROOT/pgm/putparam $DBNAME _MAXSERVERDB 2\"\n".
				 "message: $!\n";
	}


} #while

open(FILE_OUT, ">>$AllProt"); print FILE_OUT scalar localtime, "\n";
print scalar localtime, "\n";

if ( -f "$HOME/.XUSER.sav" ) { unlink "$HOME/.XUSER"   ; rename("$HOME/.XUSER.sav", "$HOME/.XUSER")    }
if ( -f "$HOME/.XUS62.sav" ) { unlink "$HOME/.XUSER.62"; rename("$HOME/.XUS62.sav", "$HOME/.XUSER.62") }

undef($SUTOPTION);
close(FILE_OUT);

sub install_paramfile {
if ($RELVER ne "R61"){
      system("$DBROOT/pgm/putparam $DBNAME DEFAULT_CODE $_[0]");
}
}


__END__

