#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/PackDefs.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
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

		
package BuildPackage::PackDefs;

use Exporter;
use BuildPackage::Vars;

@ISA = ('Exporter');

@EXPORT = ('%profile_in_name','%profiles','@allpackages','%profile_data','@installer_files');

#define profiles -> contain package names

my @add_to_server_profile = ();

if(($MAJ == 7 && $MIN > 4) || 
	($MAJ == 7 && $MIN == 4 && $COR > 3) ||
	($MAJ == 7 && $MIN == 4 && $COR == 3 && $BUILD > 0) ||
	($MAJ == 7 && $MIN == 4 && $COR == 2 && $BUILD > 7)){
	push @add_to_server_profile,'DB Analyzer';
	if($^O =~ /mswin/i){
		push @add_to_server_profile,'ODBC';
	}
}

#print join (' ',@add)


%profiles=(
	'Server' => ['Base','Server Utilities','Database Kernel',"PCR $RUNTIME_VERSION",@add_to_server_profile],
	
	#'APO LiveCache' => ['Base','Server Utilities','Database Kernel',"PCR $RUNTIME_VERSION",'APO COM'],

	'APO LiveCache' => ['APO COM'],

	#'Client' =>  ['Base',"PCR $RUNTIME_VERSION"],

	'Runtime For SAP AS' => ['Base',"PCR $RUNTIME_VERSION"],
	
	#'Runtime For SAP AS' => ['Base',"PCR $RUNTIME_VERSION",'JDBC'],
	
	'Web' => ['Base','Webtools'],
	
	'DB Analyzer' => ['Base',$^O =~ /mswin/i ? 'ODBC' : undef ,'DB Analyzer'],
	
	'Develop' => ['Base',"PCR $RUNTIME_VERSION","SDK $RUNTIME_VERSION",'ODBC','JDBC','Perl Driver','Python Driver']
);

my $name_COR = $COR;
$name_COR =~ s/^0*//g;
$name_COR = 0 unless $name_COR =~ /\S/; 
my $name_BUILD = $BUILD;
$name_BUILD =~ s/^0*//g;
$name_BUILD = 0 unless $name_BUILD =~ /\S/; 






#
#	additional profile data defs needed by SAP Software Center
#

my ($apo_rel_for_lca_package_name) = ($APO_SHORT_VERS =~ /^(.{2})/);
my ($apo_build_for_lca_package_name) = ($APO_BUILD =~ /(\d{2})$/);	# swc supports only 2 digits for apo com build
																	# ignore leading ones 

%profile_data = (
				'Server' => {
				
					'inner_subdir' => "sapdb-server-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$name_COR.'_'.$name_BUILD,
					'archive_name' => "SAPDB${MAJ}${MIN}${COR}_${BUILD}"				
				
				},
				'APO LiveCache' => {
					'inner_subdir' => "sapdb-apo${APO_SHORT_VERS}_livecache-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$name_COR.'_'.$name_BUILD,
					'archive_name' => "LC${MAJ}${MIN}${COR}${BUILD}A${apo_rel_for_lca_package_name}_${apo_build_for_lca_package_name}"				
				
				} 
);


my $exe_ext = $^O =~ /mswin/i ? '.exe' : '';

#
# files of sapdb installer (without SDBUPD - only needed for Server / APO LiveCache)
#

@installer_files = ('SDBINST'.$exe_ext,'SDBRUN'.$exe_ext,'SDBINST.TGZ');
push @installer_files,'perl56.dll' if $^O =~ /mswin/i;





%profile_in_name = (

	'Runtime For SAP AS' => 'as_runtime',
	'APO LiveCache' => "apo${APO_SHORT_VERS}_livecache"

);

	
	
#define packages
@allpackages=(
		{
			'name' => 'Base',
			'subdir' => 'base',
			'script' => 'instbase',
			'archive' => 'SAPDBBAS.TGZ',
			'filelist' => 'base.lst'	
		},
		{
			'name' => 'Server Utilities',
			'subdir' => 'utils',
			'script' => 'instutils',
			'archive' => 'SAPDBUTL.TGZ',
			'filelist' => 'utils.lst'	
		},
		{
			'name' => 'Database Kernel',
			'subdir' => 'kernel',
			'script' => 'instsrv',
			'archive' => 'SAPDBKRN.TGZ',
			'filelist' => 'kernel.lst',
			'additional_installer_files' => ['SDBUPD'.$exe_ext]	
		},
		{
			'name' => "PCR $RUNTIME_VERSION",
			'subdir' => 'pcr',
			'script' => 'instpcr',
			'archive' => "PCR$MAJ$MIN$COR.TGZ",
			'filelist' => 'pcr.lst',
			'specialFunc' => \&BuildPackage::New::copyPCR
		},
		
		{
			'name' => 'DB Analyzer',
			'subdir' => 'dbana',
			'script' => 'instdbana',
			'archive' => 'SAPDBANA.TGZ',
			'filelist' => 'dbana.lst'	
		},
		{
			'name' => "SDK $RUNTIME_VERSION",
			'subdir' => 'sdk',
			'script' => 'instsdk',
			'archive' => "SDK$RUNTIME_VERSION.TGZ",
			'filelist' => 'sdk.lst'	
		},
		{
			'name' => 'Webtools',
			'subdir' => 'web',
			'script' => 'instweb',
			'archive' => 'SAPDBWEB.TGZ',
			'filelist' => 'web.lst'	
		},
		{
			'name' => 'ODBC',
			'subdir' => 'odbc',
			'script' => 'instodbc',
			'archive' => 'SAPODBC.TGZ',
			'filelist' => 'odbc.lst'	
		},	
		{
			'name' => 'JDBC',
			'subdir' => 'jdbc',
			'script' => 'instjdbc',
			'archive' => 'SAPJDBC.TGZ',
			'filelist' => 'jdbc.lst',
			'FileMappings' => $^O =~ /mswin/i ? {"$RUNTIME_PATH/pgm/sapdbcbin.dll" => 'pgm/sapdbcbin.dll','runtime/jar/sapdbc.jar' => 'misc/sapdbc.jar'} : {'runtime/jar/sapdbc.jar' => 'misc/sapdbc.jar'} 	
		},
		{
			'name' => 'Perl Driver',
			'subdir' => 'perl',
			'script' => 'instperl',
			'archive' => 'SAPDBPL.TGZ',
			'filelist' => 'perl.lst'
		},
		{
			'name' => 'Python Driver',
			'subdir' => 'python',
			'script' => 'instpy',
			'archive' => 'SAPDBPY.TGZ',
			'filelist' => 'python.lst'	
		},
		{
			'name' => 'APO COM',
			'subdir' => 'apocom',
			'script' => 'instapocom',
			'archive' => 'APOCOM.TGZ',
			'filelist' => 'apocom.lst'
		}
);





1;

