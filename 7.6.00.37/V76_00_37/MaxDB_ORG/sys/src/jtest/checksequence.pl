#!/usr/bin/perl
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

# This script invokes 'TestGUI'. It is based on the stedi.pl script.
#
# Before doing so, it performs various tests on the environment and command 
# line arguments. 
# 
# Changes:
#	
# date	     version	author		comment
# 2005-10-21 1.0	Andreas		first Version of the script
# 2005-10-28 1.1	Andreas		added command line parameters (sysout, sequence file to open)

use Getopt::Long;
use File::Basename;
use Cwd;
use File::Spec;

$version = "1.0";

print "checksequence.pl $version\n";

$help = '';
GetOptions ('h|help|?' => \$help);

if ($help) { 
	
	print <<EOF; 

	Usage: 
	checksequence.pl [-help] <jar file> <sequence file>
	
	multiple jar and sequence files are accepted in any order
	seperated by spaces.

	-help      this helpful help screen
	
EOF
	exit(0);
}

$classpathsep = $^O =~ /mswin/i ? ";" : ":";
$pathsep = $^O =~ /mswin/i ? "\\" : "/";

$owndir = $ENV{"OWN"};
$jtest_root = $ENV{"JTEST_ROOT"};	
	
	
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
	


@jtest_dirs = ( $jtest_root, "$jtest_root" . $pathsep . "frame", "$jtest_root" . $pathsep . "tests", "$jtest_root" . $pathsep . "protocols", "$jtest_root" . $pathsep . "lib" );

$classpath = 	"$jtest_root" . $pathsep . "tests" . $classpathsep .
		"$jtest_root" . $pathsep . "frame" . $classpathsep .
		"$jtest_root" . $pathsep . "lib";
		
# Append all *.jar files in the jtest/tests directory.		
		
opendir TESTDIR, "$jtest_root" . $pathsep . "tests";
@testdir_files = grep /.*jar/, readdir TESTDIR;
closedir TESTDIR;

foreach $jar (@testdir_files) {
	$classpath .= $classpathsep . "$jtest_root" . $pathsep . "tests" . $pathsep . "$jar";		
}

# Append all *.jar files in the jtest/frame directory.

opendir FRAMEDIR, "$jtest_root" . $pathsep . "frame";
@framedir_files = grep /.*jar/, readdir FRAMEDIR;
closedir FRAMEDIR;

foreach $jar (@framedir_files) {
	$classpath .= $classpathsep . "$jtest_root" . $pathsep . "frame" . $pathsep . "$jar";			
}

# Append all *.jar files in the jtest/lib directory.

opendir LIBDIR, "$jtest_root" . $pathsep . "lib";
@libdir_files = grep /.*jar/, readdir LIBDIR;
closedir LIBDIR;


foreach $jar (@libdir_files) {
	$classpath .= $classpathsep . "$jtest_root" . $pathsep . "lib" . $pathsep . "$jar";			
}


# Append the swt .dll file  in the jtest/lib directory.
opendir LIBDIR, "$jtest_root" . $pathsep . "lib";
@libdir_files_dll = grep /.*dll/, readdir LIBDIR;
closedir LIBDIR;




# Build list of properties to be passed to the Java VM via -D.
$javaproperties =  	"-Djava.library.path=$jtest_root$pathsep" . "lib$pathsep " . 
			"-Djtest.root=\"$jtest_root\" " .
			"-Djava.compiler=\"NONE\" " .
			"-Denv.tool=\"$tooldir\" " .
			"-Denv.own=\"$owndir\" " .
			"-Ddb.insttype=\"develop\" ";

#set sysout enabled
if ($sysout) {
	$javaproperties = $javaproperties . "-Dtg.sysout=\"TRUE\" ";
	print $javaproperties;
}

$seq = "";
$jar = "";

# Set the sequence file to open at startup
foreach $ARGV (@ARGV) {
			if (-e $ARGV ){
			}elsif ( -e "tests".$pathsep.$ARGV){
				$ARGV = "tests".$pathsep.$ARGV;
			}else{
				print "File $ARGV could not be found!";
				exit(2);
			}
			$_ =$ARGV;
			if ( m/.*\.seq/) {
				$seq = $seq." ".$ARGV;
			}elsif ( m/.*\.jar/) {
				$jar = $jar." ".$ARGV;
				$classpath = $classpath . $classpathsep . $ARGV;
			}
}
if ($seq eq ""){
	print "Sequence file is missing.";
	exit 0;
}
if ($jar eq ""){
	print "Jar file is missing.";
	exit 0;
}


# Main class of this application.
$mainclass = "com.sap.sapdb.testframe.utilities.JarTest";

# Command line to execute.
$cmd = "java -classpath $classpath $javaproperties $mainclass $jar $seq";

if ($sysout) {
	print $cmd."\n";
}

$rc = system($cmd); 


