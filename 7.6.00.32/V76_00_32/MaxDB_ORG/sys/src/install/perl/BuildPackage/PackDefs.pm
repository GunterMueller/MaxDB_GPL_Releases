#!/usr/bin/perl
#
# PackDefs.pm
#
# $Header: //sapdb/V76/c_00/b_32/sys/src/install/perl/BuildPackage/PackDefs.pm#1 $
# $DateTime: 2006/07/06 16:50:32 $
# $Change: 130190 $
#

# added test diff

# Desc:	- define packages and profiles of SAP DB Packages
#		
#
#
#    ========== licence begin LGPL
#    Copyright (c) 2002-2004 SAP AG
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

my $os = $BuildPackage::Distrib::opt_os;

my $isWindows = 0;
if(defined $os){
	$isWindows = 1 if ($os =~ /WIN/); 	
}
else{
	$isWindows = 1 if ($^O =~ /mswin/i);
}


@ISA = ('Exporter');

@EXPORT = ('%profile_in_name','%profiles','@allpackages',
			'%profile_data','@installer_files','%ALL_RPMS',
			'%RPM_PROFILES','@alltestpackages',
			'%CD_MAP_LIBNAME','$self_extractor',
			$freeware_license_file);

#define profiles -> contain package names


%profiles=(
&if $OSSPEC != OSF1
	'Server' => ['Base','SAP Utilities',
				'Server Utilities',
				'Database Kernel','JDBC',"SQLDBC 76",'ODBC','DB Analyzer','Redist Python','Loader','Database Connectivity','Messages','Fastload API'],
	
	'APO LiveCache' => ['APO LC APPS'],

        # temporary exclude Perl interface until imf -32 is resolved
        'Script Interface' => ['Base','Python Interface','Messages'],

	'ODBC'   => ['Base','SAP Utilities','ODBC','Messages'],

	'JDBC'   => ['Base','SAP Utilities','JDBC','Messages'],

	'Loader' => ['Base','SAP Utilities','Loader','Redist Python','Messages'],

	'WebJTools' => ['Base','SAP Utilities','JDBC','WebSQL','WebDAV'],

	'Synchronization Manager' => ['Base','SAP Utilities','Redist Python','JDBC','Synchronization Manager'],

&endif
	'Runtime For SAP AS' => ['Base','SAP Utilities','JDBC','ODBC','Redist Python','Loader','Database Connectivity',"SQLDBC 76",'Messages','Fastload API'],

&if $OSSPEC != OSF1	
	'Webtools' => ['Base', 'ODBC', 'Webtools','Messages'],
	
	'DB Analyzer' => ['Base','SAP Utilities',$isWindows ? 'ODBC' : undef ,'DB Analyzer','Messages'],
&if $OS == WIN32 && defined BIT64 || $OSSPEC == LINUX && $MACH != I386	
	'Develop' => ['Base','SAP Utilities','ODBC','JDBC','Python Interface','Database Connectivity','Messages']
&else
	'Develop' => ['Base','SAP Utilities','ODBC','JDBC','Perl Interface','Python Interface','Database Connectivity','Messages']
&endif
&endif

);


my $name_COR = $COR;
$name_COR =~ s/^0*//g;
$name_COR = 0 unless $name_COR =~ /\S/;
my $name_BUILD = $BUILD;
$name_BUILD =~ s/^0*//g;
$name_BUILD = 0 unless $name_BUILD =~ /\S/;


$freeware_license_file = 'LICENSE';



#
#	additional profile data defs needed by SAP Software Center
#


my ($apo_rel_for_lca_package_name) = ($APO_SHORT_VERS =~ /^(.{2})/);
my ($apo_build_for_lca_package_name) = ($APO_BUILD =~ /(\d{2})$/);	# swc supports only 2 digits for apo com build
																	# ignore leading ones
%profile_data = (
				'Server' => {
				
					'inner_subdir' => "maxdb-server-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDB${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDB%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-server-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'
				},
				'APO LiveCache' => {
					'inner_subdir' => "lca${APO_SHORT_VERS}_livecache-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "LC${MAJ}${MIN}${COR}${BUILD}A${apo_rel_for_lca_package_name}${apo_build_for_lca_package_name}",
					'inner_subdir_tmpl' => 'lca%APO_SHORT_VERS_livecache-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD',
					'archive_name_tmpl' => 'LC%MAJ%MIN%COR%BUILDA%APO_REL%APO_BUILD'					
				},
				'ODBC' => {
					'inner_subdir' => "maxdb-odbc-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXODBC${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXODBC%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-odbc-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'					
				},
				'JDBC' => {
					'inner_subdir' => "maxdb-jdbc-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXJDBC${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXJDBC%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-jdbc-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'					
				},
				'Script Interface' => {
					'inner_subdir' => "maxdb-scriptif-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDBSIF${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDBSIF%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-scriptif-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'					
				},
				'Runtime For SAP AS' => {
					'inner_subdir' => "maxdb-as_runtime-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDBART${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDBART%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-as_runtime-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD',
				},
				'Webtools' => {
					'inner_subdir' => "maxdb-webtools-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDBWEB${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDBWEB%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-webtools-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'
					
				},
				'DB Analyzer' => {
					'inner_subdir' => "maxdb-analyzer-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDBANA${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDBANA%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-analyzer-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'
					
				},
				'Loader' => {
					'inner_subdir' => "maxdb-loader-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXDBLD${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXDBLD%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-loader-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'
					
				},
				'WebJTools' => {
					'inner_subdir' => "maxdb-webjtools-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXWEBJ${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXWEBJ%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-webjtools-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'
					
				},
				'Synchronization Manager' => {
					'inner_subdir' => "maxdb-syncman-$OS-".$BIT."bit-$ARCH-".$MAJ.'_'.$MIN.'_'.$COR.'_'.$BUILD,
					'archive_name' => "MAXSYNC${MAJ}${MIN}${COR}_${BUILD}",
					'archive_name_tmpl' => 'MAXSYNC%MAJ%MIN%COR_%BUILD',				
					'inner_subdir_tmpl' => 'maxdb-syncman-%OS-%BITbit-%ARCH-%MAJ_%MIN_%COR_%BUILD'					
				}
);


my $exe_ext = $isWindows ? '.exe' : '';

#
# files of sapdb installer (without SDBUPD - only needed for Server / APO LiveCache)
#

@installer_files = ('SDBINST'.$exe_ext, 'SDBINST.TGZ','README');

&if $OS == WIN32
push @installer_files,'perl58.dll','sdbrun.dll';
$self_extractor = 'extractor.exe';
push @installer_files,'SDBSETUP.exe','WXPERL.TGZ','RESOURCES.TGZ','wxmsw24.dll','Wx.dll','Grid.dll';
&else
push @installer_files,'WXPERL.TGZ','RESOURCES.TGZ','SDBSETUP','SDBRUN';
&if $OSSPEC == HPUX && $MACH != HP_IA64
push @installer_files,'libperl.sl','Wx.sl','Grid.sl','libwx_gtk-2.4.sl';
&else 
push @installer_files,'Wx.so','Grid.so';
&if $OSSPEC == AIX
push @installer_files,'libwx_gtk-2.4.a','libperl.a';
&else
push @installer_files,'libperl.so';
&if $OSSPEC == LINUX 
push @installer_files,'libwx_gtk2-2.4.so';
&else
push @installer_files,'libwx_gtk-2.4.so';
&endif
&endif
&endif
&endif

&if $OS == WIN32 && $_MSC_VER == 13 && not defined BIT64
&if $_MSC_MIN != 00
push @installer_files,'msvcr71.dll';
&else
push @installer_files,'msvcr70.dll';
&endif
&endif

%profile_in_name = (

	'Runtime For SAP AS' => 'as_runtime',
	'APO LiveCache' => "apo${APO_SHORT_VERS}_livecache",
	'Script Interface' => 'scriptif'

);

	
	
#define packages
@allpackages=(
		{
			'name' => 'Base',
			'subdir' => 'base',
			'script' => 'instbase',
			'archive' => 'SDBBAS.TGZ',
			'filelist' => 'base.lst',
&if $OS == WIN32			
			'DBG_PACKAGE' => {'files' => ['*.dbg','*.pdb','*.map'],
							  'name' => 'SDBBAS.SDB'},
&endif							  
&if $OS == WIN32 && $_MSC_VER == 13 && not defined BIT64
			,'add_files' =>
&if $_MSC_MIN != 00
			['msvcr70.dll','mfc70.dll','mfc70u.dll','msvcr71.dll','mfc71.dll','mfc71u.dll']	
&else
			['msvcr70.dll','mfc70.dll','mfc70u.dll','msvcr71.dll']	
&endif
&endif
		},
		{
			'name' => 'SAP Utilities',
			'subdir' => 'saputl',
			'script' => 'instsaputl',
			'archive' => 'SAPUTL.TGZ',
			'filelist' => 'saputl.lst',
			'distribution' => 'SAP ONLY'
		},	
		{
			'name' => 'Messages',
			'subdir' => 'msg',
			'script' => 'instmsg',
			'archive' => 'SDBMSG.TGZ',
			'filelist' => 'msg.lst'	
		},
&if $OSSPEC != OSF1
		{
			'name' => 'Server Utilities',
			'subdir' => 'utils',
			'script' => 'instutils',
			'archive' => 'SDBUTL.TGZ',
			'filelist' => 'utils.lst',
&if $OS == WIN32			
			'DBG_PACKAGE' => {'files' => ['*.dbg','*.pdb','*.map'],
							  'name' => 'SDBUTL.SDB'}	
&endif	
		},
		{
			'name' => 'Database Kernel',
			'subdir' => 'kernel',
			'script' => 'instsrv',
			'archive' => 'SDBKRN.TGZ',
			'filelist' => 'kernel.lst',
			'pmask' => 0227,
			'additional_installer_files' => ['SDBUPD'.$exe_ext],
			'README' => 'README',
			'variant' =>	{'commercial' =>
								{
									'files' =>
										{		
&if $OS == WIN32							
											'pgm/dbmsrv.exe' => 'commercial/dbmsrv.exe',
											'Support/Mapfiles/dbmsrv.map' => 'commercial/dbmsrv.map',
											'symbols/dbmsrv.pdb' => 'commercial/dbmsrv.pdb',
											'cluster/dbmsrv_clu.exe' => 'commercial/dbmsrv_clu.exe',
											'Support/Mapfiles/dbmsrv_clu.map' => 'commercial/dbmsrv_clu.map',
											'symbols/dbmsrv_clu.pdb' => 'commercial/dbmsrv_clu.pdb'  	
&else
											'pgm/dbmsrv' => 'commercial/dbmsrv'				
&endif
										}
								}
							},
			'DBG_PACKAGE' => {'files' => ['*.dbg','*.pdb','*.map','*slowknl*'],
							  'name' => 'SDBKRN.SDB'}				
		},
&endif
&if $OSSPEC != OSF1		
		{
			'name' => 'DB Analyzer',
			'subdir' => 'dbana',
			'script' => 'instdbana',
			'archive' => 'SDBANA.TGZ',
			'filelist' => 'dbana.lst'	
		},
		{
			'name' => 'Webtools',
			'subdir' => 'web',
			'script' => 'instweb',
			'archive' => 'SDBWEB.TGZ',
			'filelist' => 'web.lst',
			'distribution' => 'SAP ONLY'
		},
&endif
		{
			'name' => 'ODBC',
			'subdir' => 'odbc',
			'script' => 'instodbc',
			'archive' => 'SDBODBC.TGZ',
			'filelist' => 'odbc.lst'	
		},	
		{
			'name' => 'JDBC',
			'subdir' => 'jdbc',
			'script' => 'instjdbc',
			'archive' => 'SDBJDBC.TGZ',
			'filelist' => 'jdbc.lst'
			#'FileMappings' => $isWindows ? {"$RUNTIME_PATH/pgm/sapdbcbin.dll" => 'pgm/sapdbcbin.dll','runtime/jar/sapdbc.jar' => 'jar/sapdbc.jar'} : {'runtime/jar/sapdbc.jar' => 'jar/sapdbc.jar'} 	
		},
&if not ($OS == WIN32 && defined BIT64 || $OSSPEC == LINUX && $MACH != I386)
#		{
#			'name' => 'Perl Interface',
#			'subdir' => 'perl',
#			'script' => 'instperl',
#			'archive' => 'SDBPL.TGZ',
#			'filelist' => 'perl.lst'
#		},
&endif
		#{
		#	'name' => 'Python Interface',
		#	'subdir' => 'python',
		#	'script' => 'instpy',
		#	'archive' => 'SDBPY.TGZ',
		#	'filelist' => 'py.lst'	
		#},
		{
			'name' => 'APO LC APPS',
			'subdir' => 'apocom',
			'script' => 'instapocom',
			'archive' => 'APOLCA.TGZ',
			'filelist' => 'apocom.lst',
			'distribution' => 'SAP ONLY'
		},
		{
			'name' => 'Loader',
			'subdir' => 'loader',
			'script' => 'instloader',
			'archive' => 'SDBLD.TGZ',
			'filelist' => 'loader.lst'
		},
# &if $OS == WIN32 && not defined BIT64
# 		{
# 			'name' => 'Database Manager',
# 			'subdir' => 'dbmgui',
# 			'script' => 'instdbmgui',
# 			'archive' => 'SDBDBM.TGZ',
# 			'filelist' => 'dbmgui.lst',
# 		},
# 		{
# 			'name' => 'SQL Studio',
# 			'subdir' => 'sqlstudio',
# 			'script' => 'instsqlstudio',
# 			'archive' => 'SDBSQLST.TGZ',
# 			'filelist' => 'sqlstudio.lst',
# 		},
# &endif
		{
			'name' => 'Database Connectivity',
			'subdir' => 'dbc',
			'script' => 'instdbc',
			'archive' => 'SDBC.TGZ',
			'filelist' => 'dbc.lst',
		},
		{
			'name' => "SQLDBC 76",
			'subdir' => 'dbc_dep',
			'script' => 'instdbc',
			'archive' => 'SQLDBC76.TGZ',
			'filelist' => 'dbc.lst',
			'distribution' => 'SAP ONLY'
		},
		{
			'name' => 'WebSQL',
			'subdir' => 'websql_j',
			'script' => 'instwebsql_j',
			'archive' => 'SDBWEBSQL.TGZ',
			'filelist' => 'websql_j.lst',
			'distribution' => 'SAP ONLY'
		},
		{
			'name' => 'WebDAV',
			'subdir' => 'webdavservlet',
			'script' => 'instwebdavservlet',
			'archive' => 'SDBWEBDAV.TGZ',
			'filelist' => 'webdavservlet.lst',
			'distribution' => 'SAP ONLY'
		},
		{
			'name' => 'Synchronization Manager',
			'subdir' => 'syncman',
			'script' => 'instsyncman',
			'archive' => 'SYNCMAN.TGZ',
			'filelist' => 'syncman.lst'
		},
		{
			'name' => 'Fastload API',
			'subdir' => 'fastload',
			'script' => 'instfastload',
			'archive' => 'FASTLOAD.TGZ',
			'filelist' => 'fastload.lst',
			'distribution' => 'SAP ONLY'
		},

&if $OSSPEC != OSF1
		{
			'name' => 'Redist Python',
			'subdir' => 'rdpy',
			'script' => 'instrdpy',
			'archive' => 'SDBRDPY.TGZ',
			'filelist' => 'rdpy.lst'	
		}
&endif
);

&if $OSSPEC != OSF1

@alltestpackages=(
		{
			'name' => 'OMS Test Kernel',
			'pmask' => 0227,
			'subdir' => 'tstknl',
			'script' => 'insttstknl',
			'archive' => 'TSTKNL.TGZ',
			'filelist' => 'tstknl.lst'
		},
		{
			'name' => 'LC Simulator',
			'pmask' => 0227,
			'subdir' => 'lcsim',
			'script' => 'instlcsim',
			'archive' => 'LCSIMUL.TGZ',
			'filelist' => 'lcsim.lst'
		},
		{
			'name' => 'Test Apps',
			'pmask' => 0227,
			'subdir' => 'tstapps',
			'script' => 'insttstapps',
			'archive' => 'TSTAPPS.TGZ',
			'filelist' => 'tstapps.lst'
		},
		{
			'name' => 'SAP CRYPTOLIB',
			'subdir' => 'sapcrypto',
			'script' => 'instsapcrypto',
			'archive' => 'SAPCRYPT.TGZ',
			'filelist' => 'sapcrypto.lst',
			'add_files' => ['sec/SDBSSLA.pse','sec/SDBSSLS.pse','sec/ticket']
		},
		{
			'name' => 'WebSQL',
			'subdir' => 'websql_j',
			'script' => 'instwebsql_j',
			'archive' => 'SDBWEBSQL.TGZ',
			'filelist' => 'websql_j.lst',
		},
		{
			'name' => 'WebDAV',
			'subdir' => 'webdavservlet',
			'script' => 'instwebdavservlet',
			'archive' => 'SDBWEBDAV.TGZ',
			'filelist' => 'webdavservlet.lst',
		}
);

&endif



%ALL_RPMS = (
	'ind' => 'maxdb-ind-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	'srv76' => 'maxdb-srv76-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	'web' => 'maxdb-web-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	'callif' => 'maxdb-callif-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	'scriptif' => 'maxdb-scriptif-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	'testdb76' => 'maxdb-testdb76-%MAJ.%MIN.%cor.%build-1.i386.rpm',
	#'sqldbc' => 'maxdb-sqldbc-%MAJ.%MIN.%cor.%build-1.i386.rpm'
);

%RPM_PROFILES = (
	'Server' => ['ind','srv76','testdb76'],
	'Runtime For SAP AS' => ['ind'],
	'DB Analyzer'  => ['ind'],
	'DB Instance'  => ['ind'],
	'Webtools' => ['callif','web'],
	'ODBC' => ['callif'],
	'JDBC' => ['callif'],
	'SDK' => ['callif','scriptif'], #,'sqldbc']	
);

%CD_MAP_LIBNAME = (
	'libperl.so' => 'LIBPERL.SO',
	'libperl.sl' => 'LIBPERL.SO',
	'libperl.a' => 'LIBPERL.SO',
	'Grid.so' => 'GRID.SO',
	'Grid.sl' => 'GRID.SO',
	'Grid.a' => 'GRID.SO',
	'libwx_gtk2-2.4.so' => 'WXWIN.SO',
	'libwx_gtk-2.4.so' => 'WXWIN.SO',
	'libwx_gtk-2.4.a' => 'WXWIN.SO',
	'libwx_gtk-2.4.sl' => 'WXWIN.SO',
	'libwx_x11univ-2.4.so' => 'WXWIN.SO',
	'libwx_x11univ-2.4.sl' => 'WXWIN.SO',
	'libwx_x11univ-2.4.a' => 'WXWIN.SO',
	'Wx.so' => 'WX.SO',
	'Wx.sl' => 'WX.SO',
	'Wx.a' => 'WX.SO'
);


1;
