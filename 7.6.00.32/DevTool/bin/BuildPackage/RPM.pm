#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/RPM.pm#15 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc: helper functions for BuildPackage
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


package BuildPackage::RPM;

use BuildPackage::Tools;
use BuildPackage::Vars;
use Exporter;

@ISA =('Exporter');
@EXPORT = ('packRPM');

my $rpm_all_list = "$WRK/fast/$INSTALL_SUB_PATH/rpm/rpm_all_cp.lst";
my $wa_documents_list = "$INSTROOT/etc/waresources.lst";
my $buildroot = "$WRK/fast/$INSTALL_SUB_PATH/rpm/buildroot";
my $rpmrc = "$buildroot/rpmrc";
my $rpmmacros = "$buildroot/rpmmacros";
my $spec = "$buildroot/sapdb.spec";

my $rel = $ENV{'RELVER'};
$rel =~ s/^R//;


unless($^O =~ /linux/){
	print STDERR ("WRN: module BuildPackage::RPM is for linux use only\n");
}

sub setErrMsgHandler{
	my ($handler) = @_;
	*err = $handler;
}

sub setMsgHandler{
	my ($handler) = @_;
	*msg = $handler;
}



sub GenerateFileList{
	-f $rpm_all_list or err("RPM file list not found\n");
	-f $wa_documents_list or err("WA Documents file list not found\n");
	open(FD,$rpm_all_list) or err("cannot open RPM file list $rpm_all_list: $!\n");
	my %files = ();
	while(<FD>){	
		chomp;
		/^#/ and next;
		my ($value,$key) = (/(\S+);(\S+)/);
		$files{$key} = $value;
	}
	close(FD);
	open(FD,$wa_documents_list) or err("cannot open WA Documents file list $wa_documents_list: $!\n");
	while(<FD>){
		chomp;
		exists $files{$_} and next;
		$files{'opt/sapdb/web/'.$_} = $_;
	}
	close(FD);
	
	return %files
}

sub GenerateRPMrc{
	open(FD,">$rpmmacros");
	print FD "\%_tmppath /tmp\n";
	print FD "\%_rpmdir $WRK/fast/$INSTALL_SUB_PATH\n";
	print FD "\%_rpmfilename \%\%\{NAME\}-\%\%\{VERSION\}-\%\%\{RELEASE\}.\%\%\{ARCH\}.rpm\n";
	close(FD);
	my @filebuf;
	open (FD,'/usr/lib/rpm/rpmrc') || err("cannot open /usr/lib/rpm/rpmrc: $!\n");
	while(<FD>){
		push (@filebuf,$_) unless /^macrofiles/;
	}	
	close(FD);
	open (FD,">$rpmrc") || err("cannot create $rpmrc: $!\n");
	foreach my $line (@filebuf){
		print FD $line;
	}
	print FD "macrofiles:  /usr/lib/rpm/macros:/usr/lib/rpm/\%\{_target\}/macros:/etc/rpm/macros:/etc/rpm/\%\{_target\}/macros:~/.rpmmacros:$rpmmacros\n";
	close(FD);
	return 1;
}


sub GenerateBuildRoot{
	my (%files) = @_;
	makedir($buildroot,0775) or err("cannot create buildroot\n"); 
	foreach my $file (keys(%files)){
		copy("$INSTROOT/".$files{$file},"$buildroot/$file",{'binmode' => 1,'createdir' => 1,'nochown' => 1,'dir_perm' => 0755}) or err("cannot copy $INSTROOT/".$files{$file}."\n");
		my @statbuf = stat("$buildroot/$file");
		$statbuf[2] &= ~022; # remove write permissions of group and world
		
		if($file =~ /\/dbmsrv$|\/lserver$/){
			# set setuid bit       
			$statbuf[2] |= 04000;
		}
		chmod($statbuf[2],"$buildroot/$file");
	}
	
	foreach my $dir ("$buildroot/var/opt/sapdb/indep_data/wrk",
				"$buildroot/var/opt/sapdb/indep_data/fifo",
				"$buildroot/var/opt/sapdb/indep_data/ipc",
				"$buildroot/var/opt/sapdb/indep_data/ppid",
				"$buildroot/var/opt/sapdb/indep_data/diag",
				"$buildroot/var/opt/sapdb/indep_data/dbspeed",
				"$buildroot/var/opt/sapdb/indep_data/pid/serverpids",
				"$buildroot/var/opt/sapdb/indep_data/config",
				"$buildroot/var/spool/sql/ini",
				"$buildroot/var/log/sapdb",
				"$buildroot/opt/sapdb/depend$rel/wrk",
				"$buildroot/opt/sapdb/testdb$rel"){
		makedir($dir,0775) or err("cannot create $dir\n");
	}
	
	chmod(0755,"$buildroot/opt/sapdb/depend$rel","$buildroot/opt/sapdb/testdb$rel");

	copy("$WRK/fast/$INSTALL_SUB_PATH/rpm/sapdb","$buildroot/etc/init.d/sapdb$rel",{'binmode' => 1,'createdir' => 1,'nochown' => 1});
	copy("$WRK/fast/$INSTALL_SUB_PATH/rpm/sapdbweb","$buildroot/etc/init.d/sapdbweb",{'binmode' => 1,'createdir' => 1,'nochown' => 1});
	
	# copy demodb scripts
	
	my %replacements = ('TST' => 'TST'.$rel,
				'\$IND' => '/opt/sapdb/indep_prog',
				'\$DEP' => '/opt/sapdb/depend'.$rel);
	foreach my $file ('create_demo_db.sh','drop_demo_db.sh'){
		my $src_dir = "$INSTROOT/misc";
		my $dest_dir =  "$buildroot/opt/sapdb/testdb$rel";
		my @filebuf=();
		open(FD,"$src_dir/$file") or err("cannot open $src_dir/$file: $!\n");
		while(<FD>){
			foreach my $repl (keys(%replacements)){
				s/$repl/$replacements{$repl}/g;
			}
			push @filebuf,$_;
		} 
		close(FD);
		open(FD,">$dest_dir/$file") or err("cannot create $dest_dir/$file: $!\n");
		foreach my $line (@filebuf){
			print FD $line;
		}
		close(FD);
	}
	
	# copy webagent and xie ini
	%replacements = ( '\$OWN32' => '/opt/sapdb/web',
				'\$OWN64\$*' => '/opt/sapdb/web',
				'\$LIB\$*' => '/opt/sapdb/web/lib',
				'\$VERSION\$*' => "$MAJ.$MIN.$COR.$BUILD",
				'\$INSTROOT\$*' => '/opt/sapdb/web',
				'\$LOG\$*' => '/var/log/sapdb'
	);
	
	foreach my $ini ('WebAgent'.$rel.'.ini','sapdbxie.ini.org'){	
		-f "$INSTROOT/config/$ini" or next;
		@filebuf = (); #init
		open(FD,"$INSTROOT/config/$ini") or err("cannot open $INSTROOT/config/$ini: $!\n");
		while(<FD>){
			foreach my $repl (keys(%replacements)){
				s/$repl/$replacements{$repl}/g;
			}
			push @filebuf,$_;
		}
		close(FD);
		my $new_ini = $ini;
		$new_ini =~ s/\.org$//;		
		open(FD,">$buildroot/var/spool/sql/ini/$new_ini")
			or err("cannot create $buildroot/var/spool/sql/ini/$new_ini: $!\n");
		foreach my $line (@filebuf){
				print FD $line;
		}
		close(FD);
	}
	return 1;
}


sub GenerateSpec{
	my $src_spec = "$WRK/fast/$INSTALL_SUB_PATH/rpm/sapdb.spec";
	my @filebuf = ();
	$cor = $COR;
	$cor =~ s/^0//;
	$build = $BUILD;
	$build =~ s/^0//;
	my $release = "$MAJ.$MIN.$cor.$build";
	%replacements = ( '§RELEASE' => $release,
				'§BUILDROOT' => $buildroot
	);
	open(FD,$src_spec) or err("cannot open $srv_spec: $!\n");
	while(<FD>){
		foreach my $repl (keys(%replacements)){
			s/$repl/$replacements{$repl}/g;
		}
		push @filebuf,$_;
	}	
	close(FD);
	open(FD,">$spec") or err("cannot create $spec: $!\n");
	foreach my $line (@filebuf){
		print FD $line;
	}
	close(FD);
	return 1;	
}



sub packRPM{
	-d $buildroot and (deltree($buildroot) or err("cannot delete old buildroot\n"));
	my %files = GenerateFileList();
	GenerateBuildRoot(%files);
	GenerateRPMrc();
	GenerateSpec();
	my $out_text = `rpm --rcfile $rpmrc -bb $spec 2>&1`;
	unless($? == 0){
		err("error during rpm call: $out_text\n");
	}
	my @rpms = ();
	foreach (split("\n",$out_text)){
		my $pattern = "$WRK/fast/$INSTALL_SUB_PATH/([^/]+\.rpm)";
		$pattern =~ s/\//\\\//g;
		my ($archive) = (/$pattern/);
		push @rpms,$archive if defined $archive;
	} 	
	-d $buildroot and (deltree($buildroot) or err("cannot delete buildroot\n"));
	return @rpms;
}



1;






