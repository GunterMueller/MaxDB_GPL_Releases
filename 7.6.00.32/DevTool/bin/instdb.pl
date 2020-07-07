#!/usr/bin/perl
#  -----------------------------------------------------------------------------
#
#  module: instdb.pl
#
#  -----------------------------------------------------------------------------
#
#  responsible:  Bernd Vorsprach (D025588 - Bernd.Vorsprach@SAP-AG.de)
#
#  special area: DBMServer
#
#  description:  installation of database via DBMServer
#
#  prerequisite: Please install the DBM Srcipting Interface.
#                (for instance with "imf scriptdbm" in the 7.2
#                 development tree)
#
#  remarks:      For including a private configuration this script
#                looks for a file named "myparams.dbm" in the current
#                directory. In this file you can provide private
#                definitions of parameters and devspaces.
#
#                Lines starting with "param_put" contains a valid
#                "param_put" DBMServer command.
#                This lines will be used in the "Create param file..."
#                section of this script.
#
#                Lines starting with "param_adddevspace" contains a
#                a valid "param_adddevspace" DBMServer command.                definition. Use format from the DBMServer command
#                This lines will be used in the "Add devspaces..."
#                section of this script.
#
#                All other lines will be ignored.
#
#  -----------------------------------------------------------------------------
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
use dbm;
use repman;

$INSTDB_PROT  ="instdb.prt";

if (!getopts('hd:u:n:R:a:s:i:t:lr')) {
  print <DATA>;
  exit (1);
}

$serverdb   = $opt_d ? "$opt_d" : "testdb";
$userpwd    = $opt_u ? "$opt_u" : "dbm,dbm";
$dbroot     = $opt_R ? "$opt_R" : $INSTROOT;
$servernode = $opt_n ? "$opt_n" : "";
$sysdba     = $opt_a ? $opt_a   : "dba,dba";
$systemusr  = $opt_s ? "$opt_s" : "";
$paramfile  = $opt_i ? $opt_i   : "myparams.dbm";
$instance   = $opt_t ? $opt_t   : "";

if ($opt_h          ||
    $serverdb eq "" ||
    $userpwd  eq ""    ) {
    print <DATA>;
    exit (1);
}

# clear protocol file
open (_PP, ">$INSTDB_PROT");
close _PP;

$session = new DBM ($servernode, '', $dbroot);

print "Checking version...           ";
$version = dbmcmd($session, "version");
split (/^/, $version);
split (/,/, @_[1]);
split (/\./, substr(@_[0], 1, length(@_[0]) - 2));
$version = (@_[0] * 10000) + (@_[1] * 100) + @_[2];
if ($version < 70103) {
  print "ERR  Sorry, no support for version @_[0].@_[1].@_[2].\n                                    (Please use 7.1.3. and higher!)";
  exit (1);
} # end if
if ($version < 70202 && $instance != "") {
  print "ERR  Sorry, no support for instancetype in version @_[0].@_[1].@_[2].\n                    (Please use 7.2.2. and higher!)";
  exit (1);
} # end if
print "OK\n";

if ( $opt_r ) {

    $session = new DBM ($servernode, $serverdb, $dbroot, $userpwd);

    print "Set serverdb offline...       ";
    dbmcmd($session, "db_offline");
    print "OK\n";

    print "Drop serverdb...              ";
    dbmcmd($session, "db_drop");
    print "OK\n";

} else {

    print "Create registry entry...      ";
    dbmcmd($session, "db_create $serverdb $userpwd $systemusr");
    print "OK\n";

    if ($version < 70200) {
      $session = new DBM ('', $serverdb, '', $userpwd);
    } # end if

    print "Create param file...          ";
    dbmcmd($session, "param_startsession");
    dbmcmd($session, "param_init $instance");
    dbmcmd($session, "param_put MAXUSERTASKS  10");

    # using myparams.dbm
    if (open(FF, $paramfile)){
      while (<FF>) {
            if (/^param_put/) {
          dbmcmd($session, $_);
        } # end if
      } # end while
    } # end if

    dbmcmd($session, "param_checkall");
    dbmcmd($session, "param_commitsession");
    print "OK\n";

    print "Add devspaces...              ";
    if ($version < 70400) {
      dbmcmd($session, "param_adddevspace 1 SYS  sys_001  F       ");
    } # end if
    dbmcmd($session, "param_adddevspace 1 LOG  LOG_001  F  2000 ");
    dbmcmd($session, "param_adddevspace 1 DATA DAT_001  F 10000 ");

    # using myparams.dbm
    if (open(FF, $paramfile)){
      while (<FF>) {
            if (/^param_adddevspace/) {
          dbmcmd($session, $_);
        } # end if
      } # end while
    } # end if

    print "OK\n";

    print "Start cold...                 ";
    dbmcmd($session, "db_cold");
    print "OK\n";

    print "Init config...                ";
    dbmcmd($session, "util_connect");
    dbmcmd($session, "util_execute INIT CONFIG");
    print "OK\n";

    print "Activate serverdb...          ";
    dbmcmd($session, "util_activate $sysdba");
    dbmcmd($session, "util_release");
    print "OK\n";

    print "Load system tables...         ";
    dbmcmd($session, "load_systab -ud domp");
    print "OK\n";

    if ($version < 70200) {
      print "Restart serverdb...           ";
      dbmcmd($session, "db_restart");
      print "OK\n";
    } # end if

    if ($opt_l) {

        print "Load demo...                  ";
        chdir("$TOOL/bin/demodb") || die "Can't change to directory $TOOL/bin/demodb: $!";
        system ("xload -d $serverdb -u $sysdba -b demo.ins") && die "load demo faild\n";
        print "OK\n";
    }
}


# *** The End ***

sub dbmcmd {
    my($session, $command) = @_ ;

    open (_PP, ">>$INSTDB_PROT") || die "open instdb protocol failed\n";

    print _PP "DBMServer-Command $command\n";
    eval {
      $answer = $session->cmd ($command);
    }; # end eval
    if ($@) {
      print _PP $@;
      close _PP;
      split (/ /,$command);
      die "ERR  at DBMServer command @_[0]!\n(See $INSTDB_PROT for more information.)\n";
    } else {
      print _PP $answer;
      close _PP;
    } # end if

    return ( $answer ) ;

} # end sub dbmcmd

__DATA__

 USAGE: instdb -h
               -l
               -r
               -d <serverdb>
               -u <dbmusr>,<dbmpwd>
               -n <servernode>
               -R <instroot>
               -a <sysdba>,<pwd>
               -s <osusr>,<pwd>
               -i <inputfile>
               -t <instancetype>

        Creates a database

 OPTIONS:
       -h provides this help information
       -l loads demo schema
       -r removes database
       -d specifies serverdb to create (default testdb)
       -u specifies DBMServer user (default dbm,dbm)
       -n specifies servernode
       -R specifies INSTROOT path
       -a specifies SYSDBA user (default dba,dba)
       -s specifies operating system user
       -i specifies a private param file (default myparams.dbm)
       -t specifies the type of the instance to create
