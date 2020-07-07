#!/usr/bin/perl
#
# BuildPackage/Items.pm
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/LCPool/Items.pm#4 $
# $DateTime: 2005/06/14 13:02:22 $
# $Change: 563403 $
#

# Desc:	- define packages and profiles of SAP DB Packages
#		
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

		
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

@EXPORT = ('%lcitems'); 

my $liboms = ($^O =~ /mswin/i) ? 'lib/liboms.lib' : "$LIBPATH/liboms.$SOEXT";

my $dynliboms = ($^O =~ /mswin/i) ? 'pgm/liboms.dll' : undef;

%lcitems = (
	
	"$OWN/test" => {'items' => [ 'jtest/jtinstall.pl',
								'jtest/ir_testframe_lib.sar',
								'jtest/jr_testframe_exe.sar',
								'jtest/jr_testframe_dep.sar',
			 					'jtest/jr_testframe_ind.sar'
								]
					},
	
	"$OWN/usr/etc" => {'items' => ['VERSIONS']},

	
	"$OWN/usr" => {'items' => [	'incl',
								"sap/niping$EXEEXT",
								$liboms,
								$dynliboms
							  ],
					'subdir' => 'sys/src/lcsrc'
				},

	"$WRK/fast/$INSTALL_SUB_PATH" => {'items' => ['packages/apocom'],
							'subdir' => 'sys/src/lcsrc'
							}			
);

if ($ENV{RELVER} lt "R77") {
	push @{$lcitems{"$OWN/usr"}->{'items'}} , ("pgm/idl2xml$EXEEXT", "pgm/xml2ispc$EXEEXT");
}

if (-f "$OWN/test/support/tp2/tp2install.tar.gz"){
	$lcitems{"$OWN/test/support"} = {'items' =>  [ 'tp2/tp2install.tar.gz']};
}


if ($^O =~ /mswin/i){
	$lcitems{"$OWN/usr/pgm"} = {'items' => ['dbmcli.exe']};
}
else{
	$lcitems{"$OWN/usr/bin"} = {'items' => ['dbmcli']};
}

1;


