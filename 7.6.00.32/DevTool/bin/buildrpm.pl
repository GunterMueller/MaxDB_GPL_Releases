#!/usr/bin/perl
#
#	$TOOL/bin/buildRPM.pl
#
#
#	script generates RPM packages:
#
#				sapdb-ind-version-release.arch.rpm
#				sapdb-srv-version-release.arch.rpm
#				sapdb-testdb-version-release.arch.rpm
#				sapdb-webtools-version-release.arch.rpm
#				sapdb-precompiler-version-release.arch.rpm
#				sapdb-callif-version-release.arch.rpm
#				sapdb-scriptif-version-release.arch.rpm
#				sapdb-version.tgz -> include files of all rpm packages
#
#	targets in $WRK/fast/install
#
#
#
####################################################
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


$^O=~/linux/i or die("tool for linux only\n");
#$< == 0 or die("restart as root\n");

my $wrk=$ENV{'WRK'};
my $own=$ENV{'OWN'};
my $relver=$ENV{'RELVER'};
my $correction_level=$ENV{'CORRECTION_LEVEL'};
my $buildpraefix=$ENV{'BUILDPRAEFIX'};
my $release = $relver;
$release =~ s/R(\d)(\d)/$1\.$2/;
$release="$release.$correction_level.$buildpraefix";
my $buildroot="$wrk/fast/install/buildrpm";
my $filelist="$wrk/fast/install/rpm_all_cp.lst";
my $install = "$wrk/fast/install";
my $temp= "$install/tmp";
-d $temp || `mkdir $temp`;
my @specs=('sapdb_ind.spec',
		'sapdb_srv.spec',
		'sapdb_testdb.spec',
		'sapdb_web.spec',
		'sapdb_precompiler.spec',
		'sapdb_callif.spec',
		'sapdb_scriptif.spec'
		);

print "test needed files\n";
foreach my $file (@specs){
	-f "$install/$file" or die("file \"$install/$file\" not found");
	open FD, "$install/$file" || die ("cannot open \"$install/$file\"\n");
	open OUT, ">$temp/$file" || die ("cannot create $temp/$file");
	while(<FD>){
		s/�RELEASE/$release/;
		s/�BUILDROOT/$buildroot/;
		print OUT $_;
	}
	close(OUT);
	close(FD);
}
-f $filelist or die("filelist \"$filelist not found\"\n");



print "remove old existing buildroot\n";
`rm -rf $buildroot`;
`mkdir -p $buildroot`;
-d $buildroot || die ("cannot create dir \"$buildroot\"\n");


print "read filelist\n";
open (FD,$filelist) || die("cannot open filelist $filelist\n");
my @filebuf;
while(<FD>){
	push @filebuf,$_;
}
close(FD);


print "copy files from filelist\n";
foreach my $line (@filebuf){
	chomp($line);
	$line =~ /^#/ && next;
	my $src = $line;
	$src=~s/;.*$//;
	$src="$own/usr/$src";
	my $dest = $line;
	$dest=~s/^.*;//;
	$dest="$buildroot/$dest";
	my $dir = $dest;
	$dir=~s/\/[^\/]*$//;
	-d $dir or `mkdir -p $dir`;
	-f $src or die("file \"$src\" not found\n");
	`cp -p $src $dest`;
	-f $dest || die ("cannot copy file \"$src\"\n");
	my @statbuf = stat($dest);
	$statbuf[2] &= ~022;
	chmod($statbuf[2],$dest);
}

# special for web
print "copy WebAgent73.ini, change \$OWN32 -> /opt/sapdb/web and copy sapdbweb startscript\n";
`mkdir -p $buildroot/var/spool/sql/ini/`;
-d "$buildroot/var/spool/sql/ini" || die "cannot create $buildroot/var/spool/sql/ini\n";
open (FD,"$buildroot/opt/sapdb/web/config/WebAgent73.ini") || die ("cannot open $buildroot/opt/sapdb/web/config/WebAgent73.ini");
open (OUT,">$buildroot/var/spool/sql/ini/WebAgent73.ini") 	 || die ("cannot create $buildroot/var/spool/sql/ini/WebAgent73.ini");
while(<FD>){
	s/\$OWN32/\/opt\/sapdb\/web/;
	print OUT $_;
}
close(OUT);
close(FD);
-d "$buildroot/etc/init.d" || `mkdir -p $buildroot/etc/init.d`;
`cp -p $install/startscripts/sapdbweb $buildroot/etc/init.d/sapdbweb`;
-f "$buildroot/etc/init.d/sapdbweb" or die ("cannot copy $install/startscripts/sapdbweb\n");


# special for independent
print "create indep_data/wrk directory\n";
`mkdir -p $buildroot/var/opt/sapdb/indep_data/wrk`;
-d "$buildroot/var/opt/sapdb/indep_data/wrk" || die ("cannot create dir \"$buildroot/var/opt/sapdb/indep_data/wrk\"\n");
chmod(0775,"$buildroot/var/opt/sapdb/indep_data/wrk") || die("cannot change mode of file \"$buildroot/var/opt/sapdb/indep_data/wrk\"\n");
print "create indep_data/config directory\n";
`mkdir -p $buildroot/var/opt/sapdb/indep_data/config`;
-d "$buildroot/var/opt/sapdb/indep_data/config" || die ("cannot create dir \"$buildroot/var/opt/sapdb/indep_data/config\"\n");

if(-f "$own/FirstSteps.tgz"){
        print "create directory indep_prog/demo\n";
        `mkdir -p $buildroot/opt/sapdb/indep_prog/demo`;
        -d "$buildroot/opt/sapdb/indep_prog/demo" or die("cannot create $buildroot/opt/sapdb/indep_prog/demo\n");
        print "extract FirstSteps\n";
        chdir("$buildroot/opt/sapdb/indep_prog/demo") or die("cannot change dir\n");
        `tar -xvpzf $own/FirstSteps.tgz`;
        $? != 0 && die ("tar error\n");
}
else{
	print "NO FIRSTSTEPS - $own/FirstSteps.tgz missing\n";
}








#special for server
print "copy sapdb start script\n";
-d "$buildroot/etc/init.d" || `mkdir -p $buildroot/etc/init.d`;
`cp -p $install/startscripts/sapdb $buildroot/etc/init.d/sapdb`;
-f "$buildroot/etc/init.d/sapdb" or die ("cannot copy $install/startscripts/sapdb\n");
print "create depend/wrk directory\n";
`mkdir -p $buildroot/opt/sapdb/depend/wrk`;
-d "$buildroot/opt/sapdb/depend/wrk" || die ("cannot create dir \"$buildroot/opt/sapdb/depend/wrk\"\n");



#special for testdb
print "copy files for testdb\n";
-d "$buildroot/opt/sapdb/testdb" || `mkdir -p $buildroot/opt/sapdb/testdb`;
#`cp -p $ENV{OWN}/usr/misc/create_demo_db.sh $buildroot/opt/sapdb/testdb/create_demo_db.sh`;
#-f "$buildroot/opt/sapdb/testdb/create_demo_db.sh" or die ("cannot copy $install/demodb/create_demo_db.sh\n");
#`cp -p $ENV{OWN}/usr/misc/drop_demo_db.sh $buildroot/opt/sapdb/testdb/drop_demo_db.sh`;
#-f "$buildroot/opt/sapdb/testdb/drop_demo_db.sh" or die ("cannot copy $install/demodb/drop_demo_db.sh\n");

# replace vars
my %replace = (
		'\$DEP' => '/opt/sapdb/depend',
		'\$IND' => '/opt/sapdb/indep_prog'
	);

foreach my $file ('create_demo_db.sh','drop_demo_db.sh'){
	open (IN,"$ENV{OWN}/usr/misc/".$file) or die("cannot open file \"$ENV{OWN}/usr/misc/$file\"\n");
	open (OUT,">$buildroot/opt/sapdb/testdb/$file") or die("cannot create file \"$buildroot/opt/sapdb/testdb/$file\"\n");
	while(<IN>){
		chomp;
		foreach my $key (keys(%replace)){
			s/$key/$replace{$key}/g;
		}
		print OUT $_."\n";
	}
	close (IN);
	close (OUT);
}






#generate rpm rcscript
#needed to create RPMs in special directory
##########################################################
print "generate rpm rcscript and macros\n";
open(FD,">$temp/rpmmacros");
print FD "\%_tmppath /tmp\n";
print FD "\%_rpmdir $install\n";
print FD "\%_rpmfilename \%\%\{NAME\}-\%\%\{VERSION\}-\%\%\{RELEASE\}.\%\%\{ARCH\}.rpm\n";
close(FD);
my @filebuf;
open (FD,'/usr/lib/rpm/rpmrc') || die("cannot open /usr/lib/rpm/rpmrc\n");
while(<FD>){
	push (@filebuf,$_) unless /^macrofiles/;
}
close(FD);

open (FD,">$temp/rpmrc") || die ("cannot create $temp/rpmrc");
	foreach my $line (@filebuf){
		print FD $line;
	}
	print FD "macrofiles:  /usr/lib/rpm/macros:/usr/lib/rpm/\%\{_target\}/macros:/etc/rpm/macros:/etc/rpm/\%\{_target\}/macros:~/.rpmmacros:$temp/rpmmacros\n";
close(FD);



#build rpm
#######################################################
foreach my $spec (@specs){
	print "build rpm for $spec\n";
	`rpm --rcfile $temp/rpmrc -bb $temp/$spec`;
	$? != 0 && die("rpm error\n");
}


#build tgz archive for SuSE
##############################################
print "build tgz package for SuSE: $install/sapdb-$release.tgz\n";
chdir("$buildroot") || die ("cannot change directory\n");
`tar -cvpzf $install/sapdb-$release.tgz *`;
$? != 0 && die ("tar error\n");

#clean
########################################################
print "cleaning buildroot and temporary files\n";

`rm -rf $buildroot`;
-d $buildroot && print "cannot delete buildroot\n";
foreach my $spec (@specs){
	unlink("$temp/$spec") || print "cannot delete file $temp/$spec\n";
}
unlink("$temp/rpmrc") || print "cannot delete file $temp/rpmrc\n";
unlink("$temp/rpmmacros") || print "cannot delete file $temp/rpmmacros\n";

