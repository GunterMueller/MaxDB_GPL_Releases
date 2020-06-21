#!/usr/bin/perl
#
# module:    runtests.pl
#
# responsible: D039759
#
# special area: Script for running all Interfaces tests
# description:  
#
# see:
# 
# change history:
#
#    ========== licence begin  GPL
#    Copyright (c) 2001-2005 SAP AG
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

package runtests;

use Env;
use PCTestTools;
use Purify;
use Getopt::Long;

sub RunTests;

@tests = ( 
          'typestest',
          'lobtypestest',
          'Resultset',
          'InsStmtExe',
          'StmtExecute',
          'StmtExecuteU',
          'NullVal',
          'FailedStmt',
          'FailedPreparedStmt',
          'QueryStmt',
          'SimpleNavigation',
          'MassNavigation',
          'FetchTypes',
          'FetchPrevious',
          'connecttest',
          'batchtest',
          'illegalparam',
          'invdata',
          'pspytest',
          'applshared',
          'applinfo',
          'getrow',
          'applparamtest',
           'mttest -mt',
          'Blobtst001',
          'lobselectinto',
          'getobject',
          'dataexecute',
          'prepstmtmetadata',
          'resultsetmetadata',
          'allsqldbc',
          'allsqldbc_c',
          'rowsetnav',
          'unterm',
          'emptystring',
          'datatrunc',
          'convertUCS2',
          'updinsert',
          'upddelete',
          'updupdate',
          'allresultset',
          'helloWorld',
          'SQLStmt -p STATEMENTCACHESIZE=100',
          'sqlmode',
          'ParamBind',
          'piecewise',
          'blobsingleinsert',
	  'rowsaffected',
	  'multiplefetch',
	  'resultSetGetObject',
	  'resultSetBindColumn',
	  'errorCheck',
	  'typesCheck',
	  'conProp',
	  'PreparedStatement',
      'loblocatortest',
      'longmix',
      'ptsmisc -p SQLMODE=ORACLE',
      'lastserial',
#     'oomtest', (taken out because local databases have too less connections configured).
	  'canceltest -mt',
	  'commandinfo',
      'signalaction',
      'typesindicator',
      'paramtypes',
      'odbcbinhex',
# all tests before dropTables !!
          'droptables'
         );

do "$TOOLVARS";

$nodename=$ENV{'SERVERNODE'};
$dbname=$ENV{'SERVERDB'};
$userpwd=$ENV{'SUPERUID'} . "," . $ENV{'SUPERPWD'};

$ENV{'LD_LIBRARY_PATH'}=$ENV{'INSTLIB'};

@properties;
$help=0;
$purify_on=0;

$program_prefix=$ENV{OWN} . "/test/sqldbc/";

GetOptions( 'node|n=s' => \$nodename,
            'database|d=s' => \$dbname,
            'user|u=s' => \$userpwd,
            'purify' => \$purify_on,
            'valgrind' => \$valgrind_on,
            'property|p=s' => \@properties,
            'help|h|?' => $help) or usage();
if( $help || 
   ($dbname eq '') ||
    ($userpwd eq '') ||
    ($userpwd eq ',')
  ) {
  usage();
}


$NUM_TESTS=$#tests + 1;

$optionline="";
if($nodename ne '') {
  if(!gethostbyname($nodename)) {
    print "ERROR:Host name $nodename cannot be found.\n";
    print "INFO: 0 tests have SUCCEEDED.\n";
    print "INFO: 0 tests have FAILED.\n";
    exit(1);
    }
  $optionline .= " -n " . $nodename;
  $old_optionline .= " " . $nodename . " ";
} else {
  $old_optionline .= " localhost ";
}

$optionline .= " -d " . $dbname;
$old_optionline .= $dbname;
$optionline .= " -u " . $userpwd;

($user, $password) = split /,/, $userpwd;
$old_optionline .= " " . uc($user) . " " . uc($password) . " ";

foreach my $p (@properties) {
  $optionline .= " -p " . $p;
}

print "INFO:Running ".$NUM_TESTS." test programs.\n";
print "######################################################################\n";
$starttime=time();
$NUM_SUCCESSFUL=0;
$NUM_FAILED=0;
$FAILED_TEST="";

runtests::RunTests;

#----------------------------------------------------------

sub RunTests
{

if ($purify_on != 0) {
  Purify::WriteResultHeader ();
}
if ($valgrind_on != 0) {
  Purify::WriteResultValgrindHeader ();
}

foreach my $test (@tests) {
  $command = $program_prefix . $test . $optionline . " 2>&1";  

  if ($purify_on != 0) {
    $command = "purify /SaveTextData /ExcludeSystemDir=yes " . $program_prefix . $test . $optionline . " 2>&1";  
  }
  if ($valgrind_on != 0) {
    $command = "~/ttk/opt/bin/valgrind --tool=memcheck --leak-check=yes --num-callers=10 " . $program_prefix . $test . $optionline . " 2>&1";  
  }

  print "INFO:Running $command\n";
  $singletest_starttime = time();
  $output = qx{$command};
  print $output;
  if($?) {
    $NUM_FAILED += 1;
    $FAILED_TEST .= "$test\n";
  } else {
    $NUM_SUCCESSFUL += 1;
  }

  $singletest_endtime=time();
  $singletest_difftime=$singletest_endtime - $singletest_starttime;
  
  print "INFO: Test $test did run $singletest_difftime seconds.\n";

  if ($purify_on != 0) {
    rename ($test.".txt", $TEST."\\pc\\wrk\\".$test.".txt");
    Purify::EvaluateSingleResults ("$test");
  }
  if ($valgrind_on != 0) {
    open (TTK, ">"."TTK");
    print TTK $output;
    close (TTK);

#   cut out the program name if parameters are given
    $ltest = $test;
    $start = index ($ltest, " ");
    if ($start > 0) {
	$ltest = substr ($ltest, 0, $start);
    }

    system ("grep \"^==\" TTK > $ltest.val");
    Purify::EvaluateSingleValgrindResults ("$ltest");
  }
}
print "######################################################################\n";
print "INFO: $NUM_SUCCESSFUL tests have SUCCEEDED.\n";
print "INFO: $NUM_FAILED tests have FAILED.\n";

$endtime=time();
$difftime=$endtime - $starttime;

print "INFO: Tests did run $difftime seconds.\n";

if ($purify_on != 0) {
#  Purify::WriteFailedTests ($NUM_FAILED);
  Purify::WriteResultFooter ();
  Purify::Rename ("ifr");
}

if ($valgrind_on != 0) {
  Purify::WriteResultValgrindFooter ();
  Purify::Rename ("sqldbc");
  rename ($TEST."/pc/wrk/purifysqldbc.html", $TEST."/sqldbc/purifysqldbc.html");
}

if($NUM_FAILED > 0) {
    print "INFO: List of failed test:\n$FAILED_TEST\n";
    exit(1);
}

}

sub usage {
  print <<end;
runtests.pl

runtests.pl - run the tests of the Interface Runtime 8.0

SYNOPSIS

\truntests.pl [-n node] [-d dbname] [-u username,password] [-p property=value]

DESCRIPTION
\tRuns the tests of the SAP DB interface runtime 8.0. If not specified at command
\tline, the environment variables SERVERDB, SERVERNODE, SUPERUID and SUPERPWD 
\tare inspected to get values for node, database name, user and password.

\t-n <node>
\t\tRuns the tests using a database running on the host <node>.

\t-d <dbname>
\t\tRuns the tests using a database named <dbname>.

\t-u <username>,<password>
\t\tRuns the tests using an user <username> having a password
\t\t<password>.

\t-p <property>=<value>
\t\tSets a connect property. You may specify this option multiple times.
end

  exit(1);
}

  
