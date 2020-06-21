#!/usr/bin/perl
#
# BuildPackage/Items.pm
#
# $Header: //sapdb/V76/c_00/b_37/sys/src/install/perl/BuildPackage/LCPool/Items.pm#1 $
# $DateTime: 2007/04/16 17:59:45 $
# $Change: 149747 $
#

# Desc:	- define packages and profiles of SAP DB Packages
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

		
#
# data struct items describes which files have to be copied to LCPool
# structure of %items: 	
#	<source_path> => {'items' => [<file 1>,<directory 1>,<file 2>,... ]
#					  'subdir' => <destination_subdir>  	
#					}	
#
#	destination directory = <LC_POOL directory>/<destination_subdir>
#
#



package BuildPackage::LCPool::Items;

use BuildPackage::Vars;
use Exporter;

@ISA = ('Exporter');

@EXPORT = ('%lcitems','@add_pool_files');

my @liboms =
($^O =~ /mswin/i) ? ('lib/liboms.lib','pgm/liboms.dll','bin/sqlwhat.exe') :
                    ("$LIBPATH/liboms.$SOEXT");


@simulator_list = ($^O =~ /mswin/i) ?
	# Windows
	('lib/libomssimul.lib','pgm/libomssimul.dll',
	 'pgm/test/liboms.dll','pgm/test/liboms.lib',
	 'pgm/libmemdbg.dll',
	 'pgm/libtestsimul_comobj.dll','lib/libtestsimul_comobj.lib',
	 'pgm/omststknl.exe',
	 'pgm/kernel_reloc.exe',
	 'Support/Mapfiles/omststknl.map',
	 'Support/Mapfiles/libmemdbg.map',
	 'Support/Mapfiles/liboms-omstst.map',
	 'Support/Mapfiles/libomssimul.map',
	 'Support/Mapfiles/lctest.map',
	 'Support/Mapfiles/kernel_reloc.map',
&if $OS == WIN32 && $_MSC_VER in [ 13, 14 ]					
	 'symbols/omststknl.pdb',
	 'symbols/liboms-omstst.pdb',
	 'symbols/libmemdbg.pdb',
	 'symbols/liboms.pdb',
	 'symbols/libomssimul.pdb',
	 'symbols/kernel_reloc.pdb'	 
&else					
	 'symbols/omststknl.dbg',
	 'symbols/libmemdbg.dbg',
	 'symbols/liboms-omstst.dbg',
	 'symbols/liboms.dbg',
	 'symbols/libomssimul.dbg',
	 'symbols/kernel_reloc.dbg'
&endif	
# Files for MultiSessionSimulator; only WIN32 on IA-32
&if $OS == WIN32 && $MACH == I386 && not defined BIT64
	 ,'pgm/libmssim.dll'
	 ,'symbols/libmssim.pdb'
	 ,'Support/Mapfiles/libmssim.map'
&endif
	) :
	# Unix
	("$LIBPATH/libomssimul.$SOEXT",
	 "$LIBPATH/libmemdbg.$SOEXT",	
	 "$LIBPATH/libtestsimul_comobj.$SOEXT",
	 "$LIBPATH/test/liboms.$SOEXT",
	 "pgm/omststknl");


my @extra_sdk;

if($^O =~ /mswin/i and not exists $ENV{'BIT64'}){
	push @extra_sdk,"lib/libsqlcls.lib"; # needed by Win32 TRex
}
elsif($^O !~ /mswin/i){
	push @extra_sdk,"$LIBPATH/libsqlcls.a"; # needed by TRex
}



%lcitems = (
	"$OWN" => {'items' => ['test/jtest/alltestpkg.sar','changelist.log',
							'TESTSIMUL.SAR','make.id']
			  },
	
	"$OWN/usr/etc" => {'items' => ['VERSIONS']},

	"$OWN/usr" => { 'items' => ['incl',
								"pgm/idl2xml$EXEEXT",
  								"pgm/xml2ispc$EXEEXT",
								"sap/niping$EXEEXT",
								@liboms,
								@extra_sdk,
								@simulator_list
							  ],
				    'subdir' => 'sys/src/lcsrc'
				  },
				
	"$WRK" => { 'items' => ['lcsrc'],
				'subdir' => 'sys/src'
			  },

	"$WRK/fast/install" => {'items' => ['packages/apocom'],
							'subdir' => 'sys/src/lcsrc'
						   }			
);


if ($^O =~ /mswin/i){
	$lcitems{"$OWN/usr/pgm"} = {'items' => ['dbmcli.exe']};
}
else{
	$lcitems{"$OWN/usr/bin"} = {'items' => ['dbmcli']};
}

@add_pool_files = ();

@add_pool_files = (
	["$WRK/fast/install/SQLDBC-SDK.TGZ",
	 'archives/maxdb-sqldbc-'.$OS.'-'.$BIT.'bit-'.$ARCH.'-'.$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD.'.tgz'],
	 ["$WRK/fast/install/LICENSE",
	 'SAPDB_COMPONENTS/LICENSE']
);

&if $OS == WIN32 && not defined BIT64
push @add_pool_files,(
						["$WRK/fast/install/maxdb-php4-win-32bit-${MAJ}_${MIN}_${COR}_${BUILD}.tgz",
							"archives/maxdb-php4-win-32bit-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"],
						["$WRK/fast/install/maxdb-php5-win-32bit-${MAJ}_${MIN}_${COR}_${BUILD}.tgz",
							"archives/maxdb-php5-win-32bit-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"],
						["$OWN/usr/runtime/jar/sapdbc.src.jar",
							"archives/sapdbc-source-${MAJ}_${MIN}_${COR}_${BUILD}.jar"],
						["$OWN/usr/misc/DBM76.exe",
						   "Setup/DBM76.exe"],
						["$OWN/usr/misc/DBM76.info",
						   "Setup/DBM76.info"],
						["$OWN/usr/misc/SQLSTD76.exe",
						   "Setup/SQLSTD76.exe"],
						["$OWN/usr/misc/SQLSTD76.info",
						   "Setup/SQLSTD76.info"]);
&elif $OSSPEC == LINUX && $MACH == I386
push @add_pool_files,(
			["$WRK/fast/install/DBD-MaxDB-${MAJ}_${MIN}_${COR}_${BUILD}.tar.gz",
						   "archives/DBD-MaxDB-${MAJ}_${MIN}_${COR}_${BUILD}.tar.gz"],
			["$WRK/fast/install/SQLDBC-MAN.TGZ",
				 "archives/maxdb-sqldbc-docu-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"],
			["$WRK/fast/install/maxdb-php-source-${MAJ}_${MIN}_${COR}_${BUILD}.tgz",
				   "archives/maxdb-php-source-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"],
			["$WRK/fast/install/maxdb-php-xmldocu-${MAJ}_${MIN}_${COR}_${BUILD}.tgz",
				   "archives/maxdb-php-xmldocu-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"],   
			["$WRK/fast/install/maxdb-php-docu-${MAJ}_${MIN}_${COR}_${BUILD}.tgz",
				   "archives/maxdb-php-docu-${MAJ}_${MIN}_${COR}_${BUILD}.tgz"]);												
&endif

&if $OS == WIN32
push @add_pool_files, (["$WRK/fast/install/extractor.exe",
			 'SAPDB_COMPONENTS/extractor.exe']);
&endif


1;


