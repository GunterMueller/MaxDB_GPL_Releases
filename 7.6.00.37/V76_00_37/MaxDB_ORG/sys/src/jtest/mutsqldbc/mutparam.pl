#!/usr/bin/perl
# @(#)mutparam		2005-09-01
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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


use File::Basename;
use Carp;
use Env;

$SELF = $0;
$SELF =~ tr[\\][/] ;
$SELF = basename $SELF ;

$FAST_INSTALLED=0;
$QUICK_INSTALLED=0;
$SLOW_INSTALLED=0;
$INSTROOT=$DBROOT;
$SYSTEM_USER_PWD="";
$CONTUSER="";
$CONTPWD="";
$SYSUSER="";
$SYSPWRD="";
$DBNAME="";
$Node="";


#*******************************
#           M A I N            #
#*******************************

AnalyzeAndSetMutParamGlobals();

do "./mutprepare.pl";

do "./mutvar.all" ;
if ( $@ ) { print "$@"; exit }

if(!$INSTROOT){
	print "Error! Release installation path is not defined.\nCall the script with argument \"-R <dependent-path>\".\n";
	exit;
}

# Update path in order to reach dbmcli and other programs
&if $OSSPEC in [ WIN32 ]
$ENV{PATH}=$INSTROOT.'\bin;'.$INSTROOT.'\pgm;'.$ENV{PATH};
&else
$ENV{PATH}=$INSTROOT.'/bin:'.$INSTROOT.'/pgm:'.$ENV{PATH};
&endif
$ENV{DBROOT}=$INSTROOT;

DbStop();

print "used database is $DBNAME\n";
print "----------------------------------------------\n";

print "db_drop $DBNAME";
if ($Node)
{
	print " on host '$Node' ...\n";
}
else
{
	print " ...\n";
}
CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME db_drop", "");

print "db_create -R $INSTROOT $DBNAME";
if ($Node)
{
	print " on host '$Node' ...\n";
}
else
{
	print " ...\n";
}
CallDbmCli("-R $INSTROOT db_create $DBNAME $CTRL_USR,$CTRL_PWD $SYSTEM_USER_PWD",
           "db_create -f $DBNAME failed") ;

print "----------------------------------------------\n";
print "database will be initialized with the following parameters from $PUTPAR_SCRIPT\n";
print "print paramfile ...\n";
print "\n";
open (_PP, "<$PUTPAR_SCRIPT") || die "open scriptfile failed\n";
while (defined($outputline = <_PP> ))
{
	if ($outputline!~/^\s*$|^#/)
	{
		print $outputline;
	}
}
close _PP;
print "----------------------------------------------\n";
print "\n";
print "if you want to use another configuration, edit an existing config file or use your own config file\n";
print "for help: use '$SELF -h'\n";

CallDbmCli("-u $CTRL_USR,$CTRL_PWD -d $DBNAME -i $PUTPAR_SCRIPT","init param failed");


#***********************
sub AnalyzeAndSetMutParamGlobals{
#***********************
$Usage = "Usage: $SELF \n
	[-cfg <cfg file>]\n\t- name of config-file used for database initialization\n
	[-R <INSTROOT>]\n\t- path for INSTROOT\n
	[-d <dbname>]\n\t - name of the database\n
	[-n <node>]\n\t- name of host for remote tests\n
	[-l <system_user_pwd]\n\t- password of the systemuser\n
	[-dbm <dbm_user>,<dbm_password>]\n\t- username and password of dbm\n
	[-dba <dba_user>,<dba_password>]\n\t- username and password of dba\n
	\nATTENTION: if you use the option '-n' you have to use '-d', '-R' and '-l' too!\n" ;

my %options = ();
use Getopt::Long;
$Getopt::Long::ignorecase=0;

if ( !&GetOptions 
	(
		\%options, 'h|help|?', 'cfg=s', 'R=s', 'd=s', 'n=s', 'l=s', 'dbm=s', 'dba=s'
	)
)
{
	print("Run $SELF -h for help");
	exit(1);
}

if ( $options{'h'} )
{
	print $Usage;
	print "existing config files: \n";	
	listcfgfiles();
	exit(-1);
}

if ( $options{'cfg'} )
{
	if (-e $options{'cfg'})
	{
		$PUTPAR_SCRIPT = $options{'cfg'};
		print "used config file $PUTPAR_SCRIPT\n";
	}
	else
	{
		print "config file does not exist\n";
		print "use one of the following config files: \n";	
		listcfgfiles();
		exit (-1);
	}

}
else 
{
	$PUTPAR_SCRIPT="ascii_small.cfg";
}

if ( $options{'R'} )
{
	$INSTROOT =  $options{'R'};	
}

if ( $options{'d'} )
{
	$DBNAME = $options{'d'};
}

if ( $options{'l'} )
{
	$SYSTEM_USER_PWD = $options{'l'};
}

if ( $options{'dbm'} )
{
	($CONTUSER, $CONTPWD) = split /,/, $options{'dbm'};
}

if ( $options{'dba'} )
{
	($SYSUSER, $SYSPWRD) = split /,/, $options{'dba'};
}

if ( $options{'n'} ) {
 	if( $options{'d'} && $options{'l'} && $options{'R'} )
	{
		$Node = $options{'n'};
		$DBNAME = $options{'d'};
		$SYSTEM_USER_PWD = $options{'l'};
		$INSTROOT =  $options{'R'};	
	}
	else
	{
		print "if you want to use another database node with '-n', you have to specify also '-d', '-l' and '-R'\n";
		print $Usage;
		exit (-1);
	}
}

}
#***********************
#end sub AnalyzeAndSetMutParamGlobals
#***********************

#***********************
sub listcfgfiles{
#***********************
	opendir _DIR, "." || die "open scriptfile failed\n";
	@cfgfiles = grep /\.cfg$/, readdir _DIR;
	closedir _DIR;
	if (@cfgfiles > 0)
	{
		foreach $filename (@cfgfiles) 
		{
			print " - $filename\n";
		}
	}
	else
	{
		print "there is no config file ...\n";
	}
}
#***********************
#end sub listcfgfiles
#***********************
