#! /usr/bin/perl
#
# $Header: //sapdb/V76/c_00/b_37/sys/src/install/perl/BuildPackage/RPM.pm#1 $
# $DateTime: 2007/04/16 17:59:45 $
# $Change: 149747 $
#
# Desc: helper functions for BuildPackage
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


package BuildPackage::RPM;

use BuildPackage::Tools;
use BuildPackage::Vars;
use Exporter;

@ISA =('Exporter');
@EXPORT = ('packRPM');

my $rpm_all_list = "$WRK/fast/install/rpm/rpm_all_cp.lst";
my $wa_documents_list = "$INSTROOT/etc/waresources.lst";
my $buildroot = "$WRK/fast/install/rpm/buildroot";
my $rpmrc = "$buildroot/rpmrc";
my $rpmmacros = "$buildroot/rpmmacros";
my $spec = "$buildroot/maxdb.spec";
my $has_rpmbuild;

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


sub has_rpmbuild{
	return $has_rpmbuild if defined $has_rpmbuild;
	$has_rpmbuild = 0;
	foreach my $path (split(':',$ENV{'PATH'})){
		if(-x "$path/rpmbuild"){
			$has_rpmbuild  = 1;		
			last;
		}
	}
	return $has_rpmbuild;	
}


sub GenerateFileList{
	-f $rpm_all_list or err("RPM file list not found\n");
	-f $wa_documents_list or err("WA Documents file list not found\n");
	open(FD,$rpm_all_list) or err("cannot open RPM file list $rpm_all_list: $!\n");
	my %files = ();
	while(<FD>){	
		chomp;
		/^#/ and next;
		/\S/ or next;
		my ($value,$key) = (/(\S+);(\S+)/);
		$files{$key} = $value;
	}
	close(FD);
	open(FD,$wa_documents_list) or err("cannot open WA Documents file list $wa_documents_list: $!\n");
	while(<FD>){
		chomp;
		exists $files{$_} and next;
		$files{'opt/sdb/web/'.$_} = $_;
	}
	close(FD);
	
	return %files
}

sub GenerateRPMrc{
	open(FD,">$rpmmacros");
	print FD "\%_tmppath /tmp\n";
	print FD "\%_rpmdir $WRK/fast/install\n";
	print FD "\%_rpmfilename \%\%\{NAME\}-\%\%\{VERSION\}-\%\%\{RELEASE\}.\%\%\{ARCH\}.rpm\n";
	if(has_rpmbuild()){
		print FD "\%_unpackaged_files_terminate_build 0\n";
	}
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
		copy("$INSTROOT/".$files{$file},"$buildroot/$file",{'binmode' => 1,'createdir' => 1,'nochown' => 1,'dir_perm' => 0555}) or err("cannot copy $INSTROOT/".$files{$file}."\n");
		my @statbuf = stat("$buildroot/$file");
		$statbuf[2] &= ~0222; # remove write permissions of owner, group and world
		if($file =~ /^opt\/sdb\/\d\d\d\d\//){
			
			#
			#	remove world permissions
			#	of db kernel files ("dependent path")
			#

			$statbuf[2] &= ~07;
		}
		chmod($statbuf[2],"$buildroot/$file");
	}
	
	foreach my $dir (
				["$buildroot/var/opt/sdb/data",0775],
				["$buildroot/var/opt/sdb/data/wrk",0770],
				["$buildroot/var/opt/sdb/data/fifo",0775],
				["$buildroot/var/opt/sdb/data/ipc",0775],
				["$buildroot/var/opt/sdb/data/ppid",0775],
				["$buildroot/var/opt/sdb/data/diag",0770],
				["$buildroot/var/opt/sdb/data/dbspeed",0770],
				["$buildroot/var/opt/sdb/data/pid",0775],
				["$buildroot/var/opt/sdb/data/pid/serverpids",0777],
				["$buildroot/var/opt/sdb/data/config",0775],
				["$buildroot/var/spool/sql/ini",0775],
				["$buildroot/var/log/sdb",0775],
				["$buildroot/opt/sdb/testdb$rel",0555]){
		makedir($dir->[0],$dir->[1]) or err("cannot create ".$dir->[0]."\n");
	}
	
	chmod(0755,"$buildroot/opt/sdb/$rel","$buildroot/opt/sdb/testdb$rel");

	copy("$WRK/fast/install/rpm/maxdb","$buildroot/etc/init.d/maxdb$rel",{'binmode' => 1,'createdir' => 1,'nochown' => 1});
	copy("$WRK/fast/install/rpm/maxdbweb","$buildroot/etc/init.d/maxdbweb",{'binmode' => 1,'createdir' => 1,'nochown' => 1});
	
	# copy demodb scripts
	
	my %replacements = ('TST' => 'TST'.$rel,
				'\$IND' => '/opt/sdb/programs',
				'\$DEP' => '/opt/sdb/'.$rel.$COR);
	foreach my $file ('create_demo_db.sh','drop_demo_db.sh'){
		my $src_dir = "$INSTROOT/misc";
		my $dest_dir =  "$buildroot/opt/sdb/testdb$rel";
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
	%replacements = ( '\$OWN32\$*' => '/opt/sdb/web',
				'\$OWN64\$*' => '/opt/sdb/web',
				'\$LIB\$*' => '/opt/sdb/web/lib',
				'\$VERSION\$*' => "$MAJ.$MIN.$COR.$BUILD",
				'\$INSTROOT\$*' => '/opt/sdb/web',
				'\$LOG\$*' => '/var/log/sdb'
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
	my $src_spec = "$WRK/fast/install/rpm/maxdb.spec";
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
	my $out_text;
	if(has_rpmbuild()){
		$out_text = `rpmbuild --rcfile $rpmrc -bb $spec 2>&1`;
	}
	else{
		$out_text = `rpm --rcfile $rpmrc -bb $spec 2>&1`;
	}
	unless($? == 0){
		err("error during rpmbuild call: $out_text\n");
	}
	my @rpms = ();
	foreach (split("\n",$out_text)){
		my $pattern = "$WRK/fast/install/([^/]+\.rpm)";
		$pattern =~ s/\//\\\//g;
		my ($archive) = (/$pattern/);
		push @rpms,$archive if defined $archive;
	} 	
	-d $buildroot and (deltree($buildroot) or err("cannot delete buildroot\n"));
	return @rpms;
}



1;






