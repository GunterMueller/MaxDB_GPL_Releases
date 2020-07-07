#!/usr/bin/perl
# @(#) kernprot   6.2     1995-01-18   kern   Juergen
#
#     call of $DBROOT/bin/x_diagnose -d KERNPROT
#
#  1999-04-23 Burkhard Diesing (D025086)
#             Auswertung der Argumente mittels getopts (hd:u:c:f:)
#             anstelle von getopt
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
use FileHandle;
require "$TOOL/bin/sutvars$TOOLEXT";
use DBTools;
use Getopt::Std;


my $PROG = $X_DIAGNOSE;
if ($RELVER eq "R61")
{
	$X_VTRACE = "$DBROOT\\bin\\x_vtrace.exe";
	$DIAG_CMD = "$TMP\\kernprot61.cmd";
}

if ( @ARGV == 0 ) { system ("$PROG -d KERNPROT"); exit }

getopts('hd:u:c:f:');
if ( $opt_h )
{
    print <DATA>;
    exit 0;
}

$opt_d = $ENV{'SERVERDB'} if ( !$opt_d && defined($ENV{'SERVERDB'}) );

if ( $opt_d )
{
    $PROT_PREFIX = $opt_d;
    $RUNDIR = DBTools::GetParam($PROT_PREFIX, "RUNDIRECTORY", $opt_u);
	if ( $RELVER eq "R61" )
    {
        $TRACE_FN  = DBTools::GetParam($PROT_PREFIX, "KERNELTRACEFILE");
	}
	else
    {
        $TRACE_FN  = DBTools::GetParam($PROT_PREFIX, "_KERNELTRACEFILE", $opt_u);
	}
    $IN_FN       = "$RUNDIR/$TRACE_FN";
    $OUT_FN      = "$PROT_PREFIX.prt";
}
else
{
	if ( $opt_f )
    {
	    $IN_FN  = $opt_f;
	    $OUT_FN = "$IN_FN.prt";
	}
}

if ( -f $IN_FN )
{
    if ( $opt_c )
    {
	    system ("$PROG -d COPYTRACE $IN_FN $opt_c");
	}
	else
    {
	    print "KERNPROT: \n";
		if ($RELVER eq "R61")
        {
			system ("$X_VTRACE -f $IN_FN");
            my $fh;
			if ( $fh = new FileHandle $DIAG_CMD, "w" )
            {
                $fh->print("$OUT_FN\n");
                $fh->print("vtrace.dat\n");
                $fh->print("ALL\n");
                $fh->print("$ARGV[0]\n");
                $fh->print("EXIT\n");
                $fh->close();
            }
            system ("$PROG -d KERNPROT -b $DIAG_CMD");
			unlink ("$DIAG_CMD");
		}
		else
        {
 			print "reading  $IN_FN\n";
            $unsortedtrace = 0;
            if ( $RELVER ge "R72" ) {
                if ( $RELVER eq "R72" ) {
                    if ( $CORRECTION_LEVEL >= 5 ) {
                        $unsortedtrace = 1
                    }
                }
                else {
                    $unsortedtrace = 1
                }
            }
            if ( $unsortedtrace ) {
                print "sorting trace\n";
                $IN_FN_SORT .= "$IN_FN.sorted";
                system( "$INSTROOT/pgm/tracesort $IN_FN $IN_FN_SORT");
                $IN_FN = $IN_FN_SORT;
            }
			print "creating $OUT_FN\n";
            system ("$PROG -d KERNPROT $OUT_FN $IN_FN $ARGV[0]");
            if ( $unsortedtrace ) {
                unlink( $IN_FN_SORT );
            }
		}
	}

}
else
{
    die "*** INPUT FILE NOT FOUND: $IN_FN\n";
}

__DATA__

usage:
    kernprot -d <serverdb> [-u dbcolduser,pwd] <trace output>
  | kernprot -f <trace file name> [-u dbcolduser,pwd] <trace output>
  | kernprot -d <serverdb> [-u dbcolduser,pwd] -c <pages per trace file>

  <trace output> ::= {a, b, c, k, m, n, x, e}

    a: AK                          n: net (distribution)
    b: BD                          s: strategy
    k: KB                          t: time
    m: message block, sql_packet   x: switch output (slow_kernel)
    e: no entrypos output

  Example: kernprot -d dbsite1 akn

  if option '-u' is not used the user "superdba,colduser" is used.
