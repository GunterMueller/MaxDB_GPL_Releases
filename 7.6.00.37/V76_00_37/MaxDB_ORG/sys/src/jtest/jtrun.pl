#!/usr/bin/perl
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

use Getopt::Long;

# This script invokes the Java TestDriver.
#
# Before doing so, it performs various tests on the environment and command
# line arguments.
#
# Changes:
#	
# date	     version	author		comment
# 2001-11-09 0.1	Paul		Initial Version
# 2001-12-12 1.0	Dennis		build the correct classpath; in UNIX
#					we must use a ":" but in WIN a ";"
#					between two jar-files
# 2002-01-08 1.1	Dennis		add the java-property "env.own" to the
#					java call for a development system
# 2002-01-14 1.2	Dennis		add the lib - directory to the
#					classpath; use the right pathseparator
# 2002-01-25 1.3	Robert		add option -JT for JTEST_TOOL directory
# 2002-02-11 1.4	Dennis		append only "*.jar" files to the classpath
# 2002-02-20 1.5	Dennis		improve helpscreen, add existing testsets to
#					helpscreen, remove option "-R" from testDriver-call
# 2002-02-21 1.6	Robert		sort jars in /lib -> change sequence in class path
# 2002-03-28 1.7	Dennis		built JTEST_ROOT with $TEST and jtest dir; if existent
# 2002-04-05 1.8	Dennis		added more verbose output especially in "Build Classpath"
# 2002-07-01 1.9	Dennis		added property "-Denv.TZ" for the check of the right timezone
# 2002-07-01 2.0	Dennis		added new commandline parameter knldump
# 2002-09-03 2.1	Dennis		added new commandline parameter mailindividual
# 2002-10-07 2.2	Dennis		added LD_LIBRARY_PATH to environment for tests and removed it afterwards
# 2002-10-07 2.3	Dennis		added new cmd-parameter QA, LCP and CL
# 2002-10-07 2.4	Dennis		added new cmd-parameter MAKEKEY
# 2002-10-07 2.5	Dennis		removed cfg and seq dependency (enabled: use x.cfg with y.seq)
# 2003-01-03 2.6	Dennis		set and unset also SHLIB_PATH and LIBPATH for the correct libary path
# 2003-01-23 2.7	Dennis		changed the bug GetOptions(..); it was not possible to forward options (LCP, MAKEKEY, CL) with value 1
# 2003-03-10 2.8	Robert		use installed JDBC driver if available
# 2003-03-19 2.9	Dennis		quote all arguments, which are given to java; example "C:\Program Files\..."
# 2003-03-21 3.0	Robert		force a flush after every print to get the output
#					in the make details in correct order
# 2003-05-27 3.1	Dennis		added new options -SN and -SR
# 2003-05-28 3.2	Dennis		changed options -SN and -SR to -snode and -sroot; in verbose mode do not force a flush after every print
# 2003-06-23 3.3	Dennis		expand LD_LIBRARY_PATH for linux 32 bit
# 2003-06-23 3.4	Dennis		simplify LD_LIBRARY_PATH ; added port option
# 2003-06-23 3.5	Dennis		added -Xmx128m option to java call
# 2003-10-27 3.6	Dennis		don't ignore the case of the option arguments
# 2003-10-27 3.7	Dennis		introduced new option -pf (for a separate parameter file)
# 2004-05-10 4.0	Dennis		introduced new option -makekey2 (for a second makekey)
# 2003-06-23 4.1	Dennis		changed -Xmx128m option to -Xmx256m
# 2004-09-24 4.2	Dennis		expand LD_LIBRARY_PATH with INDEPLIB
# 2004-09-24 4.3	Dennis		added information for explicit db version in combination  with -G NONE
# 2004-12-09 4.4	Dennis		shifted the return code of the java test
# 2005-03-04 4.5	Dennis		expanded LD_LIBRARY_PATH
# 2005-03-04 4.6	Dennis		added missing '-c' for correct usage of cfg-file
# 2005-03-04 4.7	Dennis		added the java-property "-Denv.TOMCAT_PORT" to the java call for a development system
# 2005-06-22 4.8	Mecki		added java option -Xrs to solve TP2 user problem on Windows test server
# 2005-08-31 4.9	Dennis		added java option -classic  for alphaOSF only
# 2005-12-07 5.0	Dennis		enabled new option -env for new tests 
# 2005-12-07 5.1	Dennis		added some optimization for new -env file
# 2005-12-07 5.2	Dennis		added new option -INSTKEY <no>
# 2005-03-23 5.3	Dennis		added some output for the -env option and integrate some changes from release 7.6.00.DEV

# Do not change the number format of the version string! Keep the format x.y!
$version = "5.3";

###########################################################
# print version
###########################################################

print "\njtrun $version\n\n";

###############################################################################
# Build the correct separator for the classpath (if WIN then ";" else ":"),
# the path separator (if WIN the "\" else "/") and
# set jtest_root
###############################################################################

$classpathsep = $^O =~ /mswin/i ? ";" : ":";
$pathsep = $^O =~ /mswin/i ? "\\" : "/";
$jtest_root = $ENV{"JTEST_ROOT"};
$env_tz = $ENV{"TZ"};
$env_tomcat_port = $ENV{"TOMCAT_PORT"};

###############################################################################
# Parse commandline options.
###############################################################################
%options = ();
$Getopt::Long::ignorecase = 0; # do not ignore the case of the arguments ( -g != -G )
if(! &GetOptions(	\%options,
			'h|help|?',
			'd=s',	
			'u=s', 	
			'uc=s',
			'n=s', 	
			'c=s', 	
			'G=s', 	
			'k=s', 	
			'mail=s',
			'm=s',	
			'sr', 			
			'restart=s', 		
			'nowebdav', 		
			'noknldiag', 		
			'novtrace', 		
			'bits=i', 		
			'scheduled',		
			'monitor', 		
			'nomonitor',
			'mailindividual',
			'knldump',
			'R=s',			
			'JR=s',			
			'JT=s',			
			'jtrun-verbose|jv',
			'port=i',
			'k=s',			
			'o',			
			'os',			
			'p=s',
			'pf=s',
			'l=s',			
			'g=s',			
			'I=s',			
			'O=s',			
			'S=s',
			'sroot=s',
			'snode=s',
			'sport=s',
			'P=s',			
			'T',
			'QA=s',
			'LCP=s',
			'CL=s',
			'MAKEKEY=s',
			'MAKEKEY2=s',
			'INSTKEY=s',
			'env=s'
		) ) {
			
		shortHelp();
		exit(1);						
}

###############################################################################
# Handle -?. long help
###############################################################################
if($options{"h"}) {
	longHelp();
}

###############################################################################
# Check if -jtrun-verbose has been set.
###############################################################################

$verbose = $options{'jtrun-verbose'} eq '1';


###############################################################################
# force a flush after every write or print only if no verbose is activated
###############################################################################
if (!$verbose) {
	close STDERR;            # put STDERR and STDOUT on the same handle
	open STDERR, ">&STDOUT"; # -> output stays in correct order
	$| = 1;                  # enable flushing
}

###############################################################################
# Check whether we are running in a development or customer environment,
# handle $OWN and $TOOL appropriate.
###############################################################################

$develop = ($ENV{"OWN"} ne '');

if($develop) {
	
	# Development environment.
	
		
	$owndir = $ENV{"OWN"};	
	
	if($options{'JT'} ne '') {
		$tooldir = $options{'JT'};
	} else {
		if($ENV{"JTEST_TOOL"} eq '') {
			
			if($ENV{"TOOL"} eq '') {
			
				print 	"Neither \$JTEST_TOOL nor \$TOOL contains the directory of the " .
					"tool distribution.\n";			
				exit(1);								
			} else {
				$tooldir = $ENV{"TOOL"};							
			}												
		
		} else {
			
			$tooldir = $ENV{"JTEST_TOOL"};					
		}
	}
			
} else {
		
	# Customer environment.
	
	if($options{'JT'} ne '') {
		$tooldir = $options{'JT'};
	} else {
		if( ($ENV{"JTEST_TOOL"} eq '') && ($ENV{"TOOL"} eq '') ) {
			
			print 	"Environment variable JTEST_TOOL is missing\n\n" .			
				"This variable points to the directory which contains binaries needed\n" .
				"to perform certain tests.\n\n" .
				"If you don't want to call external programs from your testcases, set\n" .
				"JTEST_TOOL to any existing directory (e.g./tmp). This will cause no harm.\n\n" ;												
			exit(1);
			
		} else {
			if ($ENV{"TOOL"} eq '') {
				$tooldir = $ENV{"JTEST_TOOL"};
			} else {
				$tooldir = $ENV{"TOOL"};
			}
			
		}	
	}		
}

###############################################################################
# Check for $JTEST_ROOT.
###############################################################################

if($options{'JR'} ne '') {
	$jtest_root = $options{'JR'};
}


if ( (!$jtest_root) && ($develop) ) {
	# usually in the development environment $TEST is set and beyond this a directory jtest with the testframe
	if ($ENV{"TEST"} ne '') {
		$test_dir = $ENV{"TEST"};
		if ( -d ($test_dir . $pathsep . "jtest") ) {
			$jtest_root = $test_dir . $pathsep . "jtest";
			print	"WARNING! \n\nEnvironment variable JTEST_ROOT is missing.\n\n" .	
				" Found directory \"jtest\" beyond <$test_dir> and \n built JTEST_ROOT with it.\n".
				"\n JTEST_ROOT -> " . $test_dir . $pathsep . "jtest" ."\n\n";
		}
	}

}
if(!$jtest_root) {	
	print	"Environment variable JTEST_ROOT is missing.\n\n" .	
		"JTEST_ROOT should point to the base directory of the testframe environment.\n" .
		"This is the place where 'jtrun.pl' is located.\n" .
		"Set this variable in the environment or use the option -JR.";
	exit(1);	
}

###############################################################################
# Check if jtest root-directory exists.
###############################################################################

@jtest_dirs = ( $jtest_root, "$jtest_root" . $pathsep . "frame", "$jtest_root" . $pathsep . "tests", "$jtest_root" . $pathsep . "protocols", "$jtest_root" . $pathsep . "lib" );

$verbose && print STDERR "Check if jtest_root directory contains typical subdirectories...\n";

foreach $dir (@jtest_dirs) {
	
	$verbose && print STDERR " $dir ";
	
	if( ! -d $dir) {
		print "\nJava TestFrame root directory <$dir> does not exist.\n";						
		exit(1);				
	}
	
	$verbose && print STDERR "exists.\n";			
}

###############################################################################
# Check if tool-directory exists.
###############################################################################

if( ! -d $tooldir ) {

	print 	"The tool-directory <$tooldir> does not exist.\n";
	exit(1);
				
}
																		
###############################################################################
# Debug output
###############################################################################

if($verbose) {
	print STDERR "Collection of all neccessary directories finished. \n\nHere we go: \n";		
	print STDERR "\tJTest root directory: $jtest_root\n";
	print STDERR "\tTool directory      : $tooldir\n";
	print STDERR "\tClasspathseparator  : $classpathsep\n";
	print STDERR "\tPathseparator       : $pathsep \n";
	print STDERR "\tused Javaproperties : -Djava.compiler=NONE " .
			"-Denv.TZ=\"$env_tz\" " .
			"-Denv.TOMCAT_PORT=\"$env_tomcat_port\" " .
			"-Denv.tool=\"$tooldir\" " .
			($develop ? "-Denv.own=\"$owndir\" " : "") .
			"-Djtest.root=\"$jtest_root\" " .			
			"-Ddb.insttype=" . ($develop ? "develop" : "customer") . " ".
			"-Dcom.sap.sapdb.testframe.DBIPERL=". (defined($ENV{DBIPERL}) ? "$ENV{DBIPERL} " :"perl ")." ".
			"-Dcom.sap.sapdb.testframe.SQLDBCSDK=". (defined($ENV{SQLDBCSDK}) ? "\"$ENV{SQLDBCSDK}\"" :"\"$ENV{OWN}".$pathsep."usr\"")." ";
	print STDERR "\n";
}

###############################################################################
# Check if given configfile exists.
###############################################################################

if( $options{"c"} ) {
	if($verbose) {
		print STDERR "Check if given configfile exists... ";
	}

	if ( ! -f $options{"c"} )  {

		if( ! -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$options{c}" ) {							
			print	"The configuration-file <$options{c}> does not exist.\n";					
			exit(1);			
		} else {			
			$cfgfile = " -c \"$jtest_root" . $pathsep . "tests" . $pathsep . "$options{c}\"";		
		}		
	} else {
		$cfgfile = " -c $options{c}";		
	}
	if($verbose) {
		print STDERR "yes; use: $cfgfile\n";
	}
}

###############################################################################
# Build classpath
###############################################################################

$verbose && print STDERR "Building classpath...\n";
$verbose && print STDERR " adding directory $jtest_root" . $pathsep . "tests" . "\n";
$verbose && print STDERR " adding directory $jtest_root" . $pathsep . "frame" . "\n";
$verbose && print STDERR " adding directory $jtest_root" . $pathsep . "lib" . "\n\n";

$classpath = 	"\"$jtest_root" . $pathsep . "tests\"" . $classpathsep .
		"\"$jtest_root" . $pathsep . "frame\"" . $classpathsep .
		"\"$jtest_root" . $pathsep . "lib\"";
		
# Append all *.jar files in the jtest/tests directory.		
		
opendir TESTDIR, "$jtest_root" . $pathsep . "tests";
@testdir_files = grep /.*jar$/, readdir TESTDIR;
closedir TESTDIR;
$verbose && print STDERR " adding files from $jtest_root" . $pathsep . "tests \n";

foreach $jar (sort @testdir_files) {
	$classpath .= $classpathsep . "\"$jtest_root" . $pathsep . "tests" . $pathsep . "$jar\"";
	$verbose && print STDERR "   $jar \n";
}

# Append installed JDBC driver if it exists

$JDBCinstalled = 0;
if ($ENV{"INDEPPATH"} ne '') {
	$indeppath = $ENV{"INDEPPATH"};	
	if (-f "$indeppath" . $pathsep . "runtime" . $pathsep . "jar" . $pathsep . "sapdbc.jar") {
		$classpath .= $classpathsep . "\"$indeppath" . $pathsep . "runtime" . $pathsep . "jar" . $pathsep . "sapdbc.jar\"";
		$verbose && print STDERR " adding installed JDBC driver: " . "$indeppath" . $pathsep . "runtime" . $pathsep . "jar" . $pathsep . "sapdbc.jar\n";
		$JDBCinstalled = 1;
	}
}

# Append all *.jar files in the jtest/frame directory.

opendir FRAMEDIR, "$jtest_root" . $pathsep . "frame";
@framedir_files = grep /.*jar$/, readdir FRAMEDIR;
closedir FRAMEDIR;
$verbose && print STDERR " adding files from $jtest_root" . $pathsep . "frame \n";

foreach $jar (@framedir_files) {
if (($jar ne "sapdbc.jar") || ($JDBCinstalled == 0)) {
		$classpath .= $classpathsep . "\"$jtest_root" . $pathsep . "frame" . $pathsep . "$jar\"";
		$verbose && print STDERR "   $jar\n";
	}
}

# Append all *.jar files in the jtest/lib directory.

opendir LIBDIR, "$jtest_root" . $pathsep . "lib";
@libdir_files = grep /.*jar$/, readdir LIBDIR;
closedir LIBDIR;
$verbose && print STDERR " adding files from $jtest_root" . $pathsep . "lib \n";

foreach $jar (sort @libdir_files) {
	$classpath .= $classpathsep . "\"$jtest_root" . $pathsep . "lib" . $pathsep . "$jar\"";			
	$verbose && print STDERR "   $jar\n";
}

if ($JDBCinstalled == 1) {
	print "jtrun--> Using installed JDBC driver: " . "$indeppath" . $pathsep . "runtime" . $pathsep . "jar" . $pathsep . "sapdbc.jar\n";
} else {
	print "jtrun--> No installed JDBC driver found, using internal driver.\n";
}

# add all entries in the special test file given via -env option
if( $options{"env"} ) {
	print STDERR "jtrun--> Check if given file with special classpath entries exists... ";
	if ( ! -f $options{"env"} )  {

		if( ! -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$options{env}" ) {							
			print	"The special file <$options{env}> does not exist.\n";					
			exit(1);			
		} else {			
			$envfile = "$jtest_root" . $pathsep . "tests" . $pathsep . "$options{env}";		
		}		
	} else {
		$envfile = "$options{env}";		
	}
	print STDERR "yes;\njtrun--> use: $envfile for path expansion\n";

	if ( $options{"R"} ) {
		$INSTROOT = $options{"R"}; 
	} else {
		$INSTROOT = $ENV{"INSTROOT"};
	}
	$JTEST_ROOT=$jtest_root;
	$INDEPPATH = $ENV{"INDEPPATH"};
	
	$extclasspath = "";
	$extpath = "";
	
	open (IN_FILE, "<$envfile");
	while (<IN_FILE>)
	{
 		next if (/^\s*#/ || /^\s*$/);
 		my $variable;
 		my $content;
 	
 		if (/^\s*(\S+)\s*=\s*(\S+)\s*$/) {
 			 my $variable = $1;
 			 my $content = $2;
 			 print "jtrun-->  change '$content' to \n";
 			 $content =~ s/\\/\\\\/g;
 			 eval "\$content = \"$content\";";
 			 print "jtrun-->         '$content' for a new entry in $variable\n";
 		 	if ($variable =~ /^classpath$/i) {
 		 		$extclasspath .= "\"$content\"" . $classpathsep;
 		 	}
 		 	if ($variable =~ /^path$/i) {
 		 		$extpath .= "\"$content\"" . $classpathsep;
 		 	}
 		 	
 		}
	}
	close (IN_FILE);
	
	# change to the correct file seperator
	$extclasspath =~ s/\\/$pathsep/g;
	$extclasspath =~ s/\//$pathsep/g;
	$extpath =~ s/\\/$pathsep/g;
	$extpath =~ s/\//$pathsep/g;

	
	chop $extclasspath; # remove the last ;
	chop $extpath; # remove the last ;
	
	# expand classpath and path
	$classpath .= $classpathsep . $extclasspath;
	$ENV{PATH} =  $ENV{PATH} . $classpathsep . $extpath;
	
}


$verbose && print STDERR "\nbuilt complete; Java Classpath:\n$classpath\nEnd Java Classpath\n\n";

###############################################################################
# Extend PATH.
###############################################################################

$ENV{PATH} = $ENV{PATH} . $classpathsep . "$jtest_root" . $pathsep . "tp2";

###############################################################################
# Process non-option arguments: seq file or java classnames
###############################################################################

if(!@ARGV) {
	print	"Expecting sequence-file or testnames on the command line.\n\n";	
	shortHelp();
}

$verbose && print STDERR "Options evaluated. Processing list of tests (sequence-file or classname) to run\n";

foreach $arg (@ARGV) {	
	$cleanarg = $arg;
	$cleanarg =~ s/\.seq//;
			
	if( -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.seq" ) {
		# .seq exists
	    	if($cfgfile eq '') {
	    		if( -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.cfg" ) {
	    			$cfgfile = " -c \"$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.cfg\"";
			
			} else {
				print	"A corresponding configuration-file <$cleanarg.cfg> does not exist.\n";					
				exit(1);			
	    		}
	    	}
	    		    		    	
	    	$tests .= "\"$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.seq\" ";	    		    		    		    	
	    	
	    	$verbose && print STDERR "adding seq-file <$cleanarg>\n";
	    		    	
	} else {
	
		# this is a class name.
				
		$tests .= "$arg ";

	    	$verbose && print STDERR " adding java class <$arg>\n";
						
		$arg =~ s/\./\//g;
		$javasrc = "$jtest_root" . $pathsep . "tests" . $pathsep . "$arg.java";	
		
		$classfile = $javasrc;				
		$classfile =~ s/\.java/.class/;
				
		if( -f $javasrc ) {
				
			print "\nSource $arg.java found... ";			
																	
			@stmp = stat $javasrc;
			$mtime_src = $stmp[9];
									
			@stmp = stat $classfile;
			$mtime_class = $stmp[9];
												
			if ( $mtime_src > $mtime_class ) {
			
				print "compiling... ";
														
				if (!$verbose) {
					$cmd = "javac -classpath $classpath $javasrc " . ($^O =~ /.*win.*/i ? "> NUL 2> NUL" : "2>&1 > /dev/null");
				} else {
					$cmd = "javac -classpath $classpath $javasrc ";
				}
				$rc = system($cmd);
				
				if($rc == 0) {
										
					print "done.\n";					

				} else {
					!$verbose && print " error ($rc).\nUse option \"-jtrun-verbose\" for more information. ";
					$verbose && print " error ($rc).\n\n" .
						"--- Executed command ---\n$cmd";
					
					exit(1);																
				}																					
			} else {
				print "classfiles up to date.\n\n";								
			}																		
		}												
	}
}

###############################################################################
# Set special environment for tests
###############################################################################

if ( !($^O =~ /mswin/i) && ($ENV{"INSTROOT"} ne '') ) {
# set for unix systems some environment variables to the lib directory of sap db
	$verbose && print "Setting environment variable\n";
	if ($^O =~ /aix/i) {
	# aix
		if ($ENV{"LIBPATH"} eq '') {
			if ($ENV{"INDEPLIB"} ne '') {
				$ENV{LIBPATH} = $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib" . $classpathsep . $ENV{"INDEPLIB"};
			} else {
				$ENV{LIBPATH} = $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib";
			}
			if ($ENV{"INDEPPATH"} ne '') {
				$ENV{LIBPATH} = $ENV{"LIBPATH"} . $classpathsep . $ENV{"INDEPPATH"} . "/lib"
			}
			$verbose && print "LIBPATH to " . $ENV{"LIBPATH"} . "\n";
		} else {
			$libpath = $ENV{"LIBPATH"};
			if ($ENV{"INDEPLIB"} ne '') {
				$ENV{LIBPATH} = $libpath . $classpathsep . $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib" . $classpathsep . $ENV{"INDEPLIB"};
			} else {
				$ENV{LIBPATH} = $libpath . $classpathsep . $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib";
			}
			if ($ENV{"INDEPPATH"} ne '') {
				$ENV{LIBPATH} = $ENV{"LIBPATH"} . $classpathsep . $ENV{"INDEPPATH"} . "/lib"
			}
			$verbose && print "LIBPATH to " . $ENV{"LIBPATH"} . "\n";
		}
	} else {
	# all other unix
		if ($ENV{"LD_LIBRARY_PATH"} eq '') {
			if ($ENV{"INDEPLIB"} ne '') {
				$ENV{LD_LIBRARY_PATH} = $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib" . $classpathsep . $ENV{"INDEPLIB"};			
			} else {
				$ENV{LD_LIBRARY_PATH} = $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib";			
			}
			if ($ENV{"INDEPPATH"} ne '') {
				$ENV{LD_LIBRARY_PATH} = $ENV{"LD_LIBRARY_PATH"} . $classpathsep . $ENV{"INDEPPATH"} . "/lib"
			}			
			$verbose && print "LD_LIBRARY_PATH to " . $ENV{"LD_LIBRARY_PATH"} . "\n";
		} else {
			$libpath = $ENV{"LD_LIBRARY_PATH"};
			if ($ENV{"INDEPLIB"} ne '') {
				$ENV{LD_LIBRARY_PATH} = $libpath . $classpathsep . $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib" . $classpathsep . $ENV{"INDEPLIB"};
			} else {
				$ENV{LD_LIBRARY_PATH} = $libpath . $classpathsep . $ENV{"INSTROOT"} . "/lib/lib64" . $classpathsep . $ENV{"INSTROOT"} . "/lib" ;
			}
			if ($ENV{"INDEPPATH"} ne '') {
				$ENV{LD_LIBRARY_PATH} = $ENV{"LD_LIBRARY_PATH"} . $classpathsep . $ENV{"INDEPPATH"} . "/lib"
			}
			
			$verbose && print "LD_LIBRARY_PATH to " . $ENV{"LD_LIBRARY_PATH"} . "\n";
		}
	}
}

###############################################################################
# Build option list for TestDriver.
###############################################################################

%skipuseroptions = (	'h' => 1,
			'JT' => 1,
			'jtrun-verbose' => 1,
			'JR' => 1,
			'env' => 1, # the env file is handled some steps below and not necessary for the test frame
			'c' => 1); # the cfg file is handled some steps below and is registered in the variable $cfgfile
	
foreach $option (keys %options) {
	
	if($skipuseroptions{$option}) { next; }

	$useroptions .= ' -' . $option;
	
	if( ($options{$option} != 1) || ($option eq "LCP") || ($option eq "CL") || ($option eq "MAKEKEY")  || ($option eq "MAKEKEY2") || ($option eq "INSTKEY")){
		$useroptions .= ' "' . $options{$option} . '"';			
	}						
	
}

if ($ENV{PURIFY}) {
    if (! $options{d}) {
        die "must specify database name when using Purify";
    }
    $useroptions .= ' -G OFF -restart false ';
    if (! -d 'protocols/purify') {
        mkdir 'protocols/purify', 0777;
    }
    &startPurifyKernel ($options{d}, "protocols/purify/" . $ARGV[0] . ".purlog");
}

$fixedoptions = "-o \"$jtest_root" . $pathsep . "protocols\" -p \"$jtest_root" . $pathsep . "tests" . $pathsep . "parameters\"" . (defined $cfgfile ? $cfgfile : '');
$javaproperties = 	"-Djava.compiler=NONE " .
			"-Denv.TZ=\"$env_tz\" " .
			"-Denv.TOMCAT_PORT=\"$env_tomcat_port\" " .
			"-Denv.tool=\"$tooldir\" " .
			($develop ? "-Denv.own=\"$owndir\" " : "") .
			"-Djtest.root=\"$jtest_root\" " .			
			"-Ddb.insttype=" . ($develop ? "develop" : "customer") . " ".
			"-Dcom.sap.sapdb.testframe.DBIPERL=". (defined($ENV{DBIPERL}) ? "$ENV{DBIPERL} " :"perl ")." ".
			"-Dcom.sap.sapdb.testframe.SQLDBCSDK=". (defined($ENV{SQLDBCSDK}) ? "\"$ENV{SQLDBCSDK}\"" :"\"$ENV{OWN}".$pathsep."usr\"")." ";
			
$mainclass = "com.sap.sapdb.testframe.driver.TestDriver";

if ( ($^O =~ /dec/i) ) {
	# java option -classic  for alphaOSF only
	$javaproperties .= " -classic ";
}

$cmd = "java -Xmx256m -Xrs -classpath $classpath $javaproperties $mainclass $fixedoptions $useroptions $tests";

###############################################################################
# Run the TestDriver.
###############################################################################

$verbose && print STDERR "\nStarting TestDriver.\n\n--- Begin Command line ---\n$cmd\n--- End Command line ---\n\n";

$rc = system($cmd);

###############################################################################
# Unset special environment for tests
###############################################################################

if ( !($^O =~ /mswin/i) && ($ENV{"INSTROOT"} ne '') ) {
	$verbose && print "Unsetting environment variable \n";
	if ( ($^O =~ /dec/i) || ($^O =~ /solaris/i) || ($^O =~ /hpux/i) || ($^O =~ /linux/i) ) {
	# dec
		if ($libpath eq '') {
			$verbose && print "Setting LD_LIBRARY_PATH from " . $ENV{LD_LIBRARY_PATH} . " back to \"\"\n";
			$ENV{LD_LIBRARY_PATH} = undef;
			$verbose && print "LD_LIBRARY_PATH is set back to \"" . $ENV{LD_LIBRARY_PATH} . "\"\n";
		} else {
			$verbose && print "Setting LD_LIBRARY_PATH from " . $ENV{LD_LIBRARY_PATH} . " back to $libpath\n";
			$ENV{LD_LIBRARY_PATH} = $libpath;
			$verbose && print "LD_LIBRARY_PATH is set back to " . $ENV{LD_LIBRARY_PATH} . "\n";
		}
	} # dec

	if ($^O =~ /aix/i) {
	# aix
		if ($libpath eq '') {
			$verbose && print "Setting LIBPATH from " . $ENV{LIBPATH} . " back to \"\"\n";
			$ENV{LIBPATH} = undef;
			$verbose && print "LIBPATH is set back to \"" . $ENV{LIBPATH} . "\"\n";
		} else {
			$verbose && print "Setting LIBPATH from " . $ENV{LIBPATH} . " back to $libpath\n";
			$ENV{LIBPATH} = $libpath;
			$verbose && print "LIBPATH is set back to " . $ENV{LIBPATH} . "\n";
		}
	} # aix
}

if ($rc != 0) {
	$rc >>= 8;
}
exit($rc);

###############################################################################
# short help
###############################################################################
sub longHelp() {
	print <<EOF;	

USAGE: jtrun.pl [-d <database>] [-uc <name,pwd>] [<options>] testname/testset
  <options>:=

   -h | -? | -help            display this help

   -d <database>              name of the database to be tested

   -u <name,password>         DBA user (admin)

   -uc <name,password>        DBM user (colduser)

   -n <node>                  host where db is running; default: localhost

   -port <port>               <db port> for database communication

   -c <driver config file>    name of the test driver config file.
                              This file has to be located in the
                              folder jtest_root/tests

   -pf <par file>             name of an explicit parameter file in the
                              parameters directory for the test; overwrites
                              entries in the seq file

   -G [FORCE|OFF|SHORT|NONE],
      [DROP|KEEP],
      [ATTACH],
      [OFFLINE,COLD,ADMIN,
       WARM,ONLINE]           define the database creation mode
                               FORCE   - Always generate the database
                               OFF     - Use only an existing database
                               NONE    - no database; NONE(version)
                                          no database, but use the given
                                          version for the testmonitor
                               SHORT   - generate a database if required
                                          wont use: load_systab, backup,
                                          no DB-restart between tests
                               DROP    - Always drop the database
                                         after the tests
                               KEEP    - Never drop the database after
                                         the tests
                               ATTACH  - before the test will be executed,
                                         you get the chance to attach
                                         to the kernel with a debugger
                               OFFLINE - if the database should be kept,
                                         keep it in OFFLINE - mode
                               COLD    - keep the database in COLD - mode
                               WARM    - keep it in WARM - mode

   -k [SLOW|FAST|TEST]   sets the database speed (kernel mode)

   -mail [<emailadress>|OFF]  send an email to <emailadress> or not (OFF)
                              overwrites the mail-entry in the config file

   -sr                        store the testresults in external database;
                              see configfile

   -restart [true|false]      specify whether the database should be restarted
                              between each test.

   -nowebdav                  disable upload of files to WebDAV server

   -noknldiag                 disable uploading or attaching of knldiag
                              and knldiag.err files

   -novtrace                  disable generating of vtrace file

   -bits [32|64|n]            architecture of the OS where the db is running

   -LCP <no>                  lcpool number; which software package was used

   -QA [D|C|R|H]              QA level; means DEV, RAMP, COR or HOT

   -CL <no>                   last changelist number

   -INSTKEY <no>              installation identifier

   -MAKEKEY <no>              make identifier

   -MAKEKEY2 <no>             second make identifier (like CPC Make id)

   -scheduled                 indicates, that this is a scheduled test which
                              should be included in the generated report

   -monitor                   make this test visible in the TestMonitor

   -nomonitor                 don't send any information to the TestMonitor

   -R <db_root>               database root directory. Either remote or local.

   -sroot <db_root>           a second database root

   -snode <db_node>           a second database node resp. host

   -sport <db_port>           a second database port for jdbc communication

   -JR                        root directory of the Java TestFrame environment.
                              This overwrites the contents of the JTEST_ROOT
                              environment variable.

   -JT                        directory for binaries required by tests.
                              This overwrites the contents of the JTEST_TOOL
                              environment variable.

   -T                         enable additional testframe trace

   -jtrun-verbose | -jv       display information about the testdriver-call

   -env <file>                File with environment changes for jtrun:
                               CLASSPATH=<jar file> append this archive to CP
                               PATH=<directory> append this directory to PATH
                               Several CLASSPATH/PATH entries are possible.
                               
  testname                    testset/Java class to be executed
                              Testsets : lctest_night | lctest_weekend |
                                         lctest_multi | oltptest


EOF
	createTestSets();
	exit(1);	

}

###############################################################################
# short help
###############################################################################
sub shortHelp() {
	print <<EOF;	
	
USAGE: jtrun.pl [-d <database>] [-uc <name,pwd>] [<options>] testname/testset

jtrun.pl -? for detailed help

EOF
	createTestSets();
	exit(1);	
}


###############################################################################
# build and print all seq+cfg pairs from the $JTEST_ROOT/tests directory
###############################################################################
sub createTestSets() {
print " EXAMPLES:\n";
print "    jtrun.pl lctest_night\n";
print "        Run the liveCache test suite with default parameters\n";
print "    jtrun.pl -d MYTEST -u admin,admin -uc superdba,colduser lctest_night\n";
print "        Run the liveCache test suite on a different database\n";
print "    jtrun.pl -c lctest_night.cfg com.sap.sapdb.lctest.PrimaryConsistent1\n";
print "        Run only one specific test with a given config file\n\n";


	opendir TESTDIR, "$jtest_root" . $pathsep . "tests";
	@testseq_files = grep /.*seq/, readdir TESTDIR;
	closedir TESTDIR;
	
	print "\n Existing testsets (cfg+seq file) in this directory: \n\n    ";	

	$linecounter = 0; # four testsets per line
	$total_tests = 0;
	foreach $seq (@testseq_files) {
		$cleanarg = $seq;
		$cleanarg =~ s/\.seq//;
		if( -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.seq" && -f "$jtest_root" . $pathsep . "tests" . $pathsep . "$cleanarg.cfg" ) {
		
			if ($linecounter ne 0) {
				print " | ";
			}
			print $cleanarg;
			$total_tests++ ;
			$linecounter++;
			if ($linecounter eq 4) {
				$linecounter = 0;
				print "\n    ";
			}
		}
	}
	if ($total_tests eq 0) {
		print "NONE";
	}
	print "\n";
}

###############################################################################
sub startPurifyKernel {
    my ($dbname, $protocolFile, $userInfo) = @_;
    if (! defined ($userInfo)) {
        $userInfo = 'DBM,DBM';
    }
    my $cmdline = " /SaveTextData=$protocolFile $ENV{INSTROOT}/pgm/kernel_reloc.exe $dbname";
    # print $ENV{'PURIFY'} . " $cmdline\n";
    if ($^O =~ /win32/i) {
        my $spawn = \&Win32::Spawn;;
        &$spawn ($ENV{'PURIFY'}, $cmdline, $PID);
    }
    else {
        die "support for purified only on Windows";
    }
    my $dbmcli = "dbmcli -d $dbname -u $userInfo ";
    &waitForAdminState ($dbmcli . ' db_state');
    system ($dbmcli . ' db_online');
}

#################################################################
# Funktion : waitForAdminState
#
# Arguments:
#      1. the full dbmcli command
#
sub waitForAdminState {
    my ($dbmcli) = @_;
    my ($targetTime) = time + 120;

    while (time < $targetTime) {
        my $remaining = $targetTime - time;
        open (PIPE, $dbmcli . " |") or die;
        while (<PIPE>) {
            if (/.*ADMIN/) {
                return 1;
            }
        }
        close PIPE;
        sleep 5;
    }
    die "database didn\'t reach ADMIN state";
}
