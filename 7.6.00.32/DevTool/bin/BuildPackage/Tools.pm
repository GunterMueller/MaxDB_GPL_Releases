#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/Tools.pm#19 $
# $DateTime: 2005/11/18 16:44:30 $
# $Change: 578700 $
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


package BuildPackage::Tools;

use File::Copy;
use Cwd;
use Exporter;
use Net::FTP;
use Net::Config;
use Time::Local;


@ISA =('Exporter');
@EXPORT = ('getMD5Sum','add_file','insert_file','remove_file',
			'do_cmd','makedir','find','deltree','copy','readini',
			'remoteGet','remoteLs','ftp_connect','ftp_release','getBuildInfo',
			'getDBMGuiVersion','getSQLStudioVersion');


sub setErrMsgHandler{
	my ($handler) = @_;
	*err = $handler;
}

sub setMsgHandler{
	my ($handler) = @_;
	*msg = $handler;
}

sub getSQLStudioVersion{
	my $infofile;
	my $sqlstd_list = $ENV{'WRK'}.'/fast/install/packages/sqlstudio/sqlstudio.lst';
	
	unless(-f $sqlstd_list){
		print "getSQLStudioVersion(): file list $sqlstd_list not found\n";
		return undef;
	}
	
	open(TFD,$sqlstd_list);
	my $exe_file;
	while(my $line = <TFD>){
		chomp $line;
		if($line =~ /\/SQL.*\.exe$/){
			$exe_file = $line;
			last;
		}
	}	
	close(TFD);

	unless(defined $exe_file){
		print "getSQLStudioVersion(): exe_file not found\n";
		return undef;
	}
	
	$info_file = $exe_file;
	$info_file =~ s/\.exe/\.info/;
	$info_file = $ENV{'INSTROOT'}.'/'.$info_file;
	
	unless(-f $info_file){
		print "getSQLStudioVersion(): info file $info_file not found\n";
		return undef;
	}
	
	my $version;
	open(TFD,$info_file);
	while(my $line = <TFD>){
		chomp $line;
		if($line =~ /SQL\s+Studio\s+\d{4}\s+Build\s+\d\d/){
			my ($maj,$min,$cor,$build) = ($line =~ /SQL\s+Studio\s+(\d)(\d)(\d\d)\s+Build\s+(\d\d)/);
			$version = "$maj.$min.$cor.$build";
			last;
		}
	}
	close(TFD);
	return $version;
}

sub getDBMGuiVersion{
	my $infofile;
	my $dbmgui_list = $ENV{'WRK'}.'/fast/install/packages/dbmgui/dbmgui.lst';
	unless(-f $dbmgui_list){
		print "getDBMGuiVersion(): $dbmgui_list not found\n";
		return undef;
	}
	
	open(TFD,$dbmgui_list);
	my $exe_file;
	while(my $line = <TFD>){
		chomp $line;
		if($line =~ /\/DBM.*\.exe$/){
			$exe_file = $line;
			last;
		}
	}	
	close(TFD);

	unless(defined $exe_file){
		print "getDBMGuiVersion(): exe_file not found\n";
		return undef;
	}
	
	$info_file = $exe_file;
	$info_file =~ s/\.exe/\.info/;
	$info_file = $ENV{'INSTROOT'}.'/'.$info_file;
	
	unless(-f $info_file){
		print "getDBMGuiVersion(): infofile $info_file not found\n";
		return undef;
	}
	
	my $version;
	open(TFD,$info_file);
	while(my $line = <TFD>){
		chomp;
		if($line =~ /Database\s+Manager\s+\d{4}\s+Build\s+\d\d/){
			my ($maj,$min,$cor,$build) = ($line =~ /Database\s+Manager\s+(\d)(\d)(\d\d)\s+Build\s+(\d\d)/);
			$version = "$maj.$min.$cor.$build";
			last;
		}
	}
	close(TFD);
	return $version;
}


sub getBuildInfo{
	my ($file) = @_;
	return undef if(exists $ENV{'XCOMP'});
	my %rc;
	return undef unless -f $file;
	my $cmd = $ENV{'WRK'}.'/fast/'.$BuildPackage::Vars::INSTALL_SUB_PATH.'/SDBINST --main SAPDB::Install::BuildInfo::main '.$file;
	$cmd =~ s/\//\\/g if $^O =~ /mswin/;
	$cmd = 'umask 007 && '.$cmd if $^O =~ /aix/i;
	my $outfile;
	if($^O =~ /mswin/i){
		$outfile = $ENV{'WRK'}.'/fast/install/BuildInfo.out';
		$outfile =~ s/\\/\//g;
		$cmd .= ' '.$outfile;
	}
	
	my $txt_out;
	{
		local %ENV = %ENV;
		delete $ENV{'DBROOT'};
		delete $ENV{'SAPDB_NO_INDEPPATH'};	
		$txt_out = `$cmd`;
	}
	
	return undef unless($? == 0);
	
	if($^O =~ /mswin/i){
		open(MYFD,$outfile) or return undef;
		while(<MYFD>){
			$txt_out .= $_;
		}
		close(MYFD);
		unlink($outfile);		
	}
	
	foreach (split("\n",$txt_out)){
		my ($key,$value) = (/(\S+):\s+(\S.*)/);
		next unless $key =~ /\S/;
		$rc{$key} = $value;
	}
	return \%rc;
}



sub getMD5Sum{
	my ($file) = @_;
	-f $file || err("cannot get md5sum of $file: $!\n") && return undef;
	my $md5sum = `md5sum $file 2>&1`;
	chomp($md5sum);
      my $output = $md5sum;
	$md5sum =~ s/\s+\S+//;
      $md5sum =~ /^[a-f|0-9]{32}$/ || err ("cannot get md5sum of $file: $output\n") && return undef;
	return $md5sum;
}


sub copy{
	my ($src,$dst,$config)=@_;
	$src=~s/\\/\//g;
	$dst=~s/\\/\//g;
	-f $src or err("ERR: System::copy(): file \"$src\" not found\n");
	my @statbuf = stat($src);
	open(SRC,$src) or err("ERR: System::copy(): cannot open file \"$src\": $!\n");
	$config->{'binmode'} && binmode(SRC);
	my $dst_name;
	if(-d $dst){
		$src=~/([^\/]*)$/ and my $file=$1;
		my $dperm_changed = 0; 
		my @dstatbuf;
		if($^O !~ /mswin/i and $< != 0){
			@dstatbuf = stat($dst);
			if($< == $dstatbuf[4]){
				if(($dstatbuf[2] & 0200) == 0){
					chmod($dstatbuf[2] | 0200,$dst);
					$dperm_changed = 1;
				}
			}
		}
		open(DST,">$dst/$file") or err("ERR: System::copy(): cannot create $dst/$file: $!\n");
		chmod($dstatbuf[2],$dst) if $dperm_changed;
		$dst_name = "$dst/$file";
	}
	elsif(-f $dst){
		if($^O !~ /mswin/i and $< != 0){
			my @dstatbuf = stat($dst);
			if($< == $dstatbuf[4]){
				if(($dstatbuf[2] & 0200) == 0){
					chmod($dstatbuf[2] | 0200,$dst);
				}
			}
		}
		open(DST,">$dst") or err("ERR: System::copy(): cannot create $dst\n");
		$dst_name = $dst;
	}
	else{
		my $dir = $dst;
		$dir=~s/[^\/]*$//;
		$dst_name = $dst;
		unless(-d $dir){
			if($config->{'createdir'}){
				my $perm = 0775; #default
				$perm = $config->{'dir_perm'} if defined $config->{'dir_perm'}; 
				makedir($dir,$perm) or err("ERR: Tools::copy(): cannot create directory $dir: $!\n");
			}
			else{
				err("ERR: Tools::copy(): cannot create $dst - parent dir missing\n");
			}
		}  
		my $dperm_changed = 0; 
		my @dstatbuf;
		if($^O !~ /mswin/i and $< != 0){
			@dstatbuf = stat($dir);
			if($< == $dstatbuf[4]){
				if(($dstatbuf[2] & 0200) == 0){
					chmod($dstatbuf[2] | 0200,$dir);
					$dperm_changed = 1;
				}
			}
		}
		open(DST,">$dst") or err("ERR: System::copy(): cannot create $dst: $!\n");
		chmod($dstatbuf[2],$dir) if $dperm_changed;
	}
	$config->{'binmode'} && binmode(DST);
	my $bufSize=(stat SRC)[11] || 32768;
	while(my $len = sysread(SRC,$buf,$bufSize)){
		unless(defined $len){
			next if $! =~ /^Interrupted/;
			err("ERR: Tools::copy(): read failure: $!\n");
			return 0;
		} 
		my $offset=0;
		while($len){
			$written=syswrite(DST,$buf,$len,$offset);
			defined $written or err("ERR: Tools::copy(): write failure: $!\n");
			$len-=$written;
			$offset+=$written;
		}	
	}
	close(SRC);
	close(DST);
	
	utime ($statbuf[8],$statbuf[9],$dst_name) or err("cannot set time of copied file \"$dst_name\": $!\n");

	unless($^O =~ /mswin/i){
		$statbuf[2] &= 0777;
		chmod($statbuf[2],$dst_name) or err("cannot set mode of copied file \"$dst_name\": $!\n");
		unless($config->{'nochown'}){
			chown($statbuf[4],$statbuf[5],$dst_name) or err("cannot set uid/gid of copied file \"$dst_name\": $!\n"); 
		}
	}
	
	#msg("copied file \"$src\" to \"$dst\"\n");
	return 1;
}



sub add_file{
        my ($file,$target_dir,$target,$list) = @_;
        -f $file or return undef;
        -d $target_dest || return undef;
        $target =~/^\// and return undef;
        $file =~ s/\\/\//g;
        $target_dir =~ s/\\/\//g;
        $target_dir =~ s/\/$//g;
        $file =~ s/\\/\//g;
        my ($dir) = ($target =~ /^(.*)\/\w+$/);
        if ($dir =~ /\S/){
                makedir($target_dir.'/'.$dir) || return undef;
        }
        copy($file,$target_dir.'/'.$target) || return undef;
         my $md5sum = `md5sum $file`;
        $md5sum =~ s/\s+\S+//;
        $md5sum =~ /^[a-f|0-9]{32}$/ || return undef;
        my $list_file = $target_dir.'/'.$list;
        my @list_buf;
        open (LST,$list_file) || return undef;
        while(<LST>){
                chomp;
                /\S/ or next;
                /^"$target"/ and next;
                push @list_buf,$_;
        }
        close(LST);
        push @list_buf,"\"$target\" $md5sum";
        open (LST,'>'.$list_file) || return undef;
        foreach (sort @list_buf){
                print LST "$_\n";
        }
        close(LST);
        return 1;
}

sub insert_file {
        my ($buildroot,$file,$list)=@_;
        -d $buildroot || return undef;
        -f "$buildroot/$file" || return undef;
        -f "$buildroot/$list" || return undef;
        my @list_buf;
        open (LST,"$buildroot/$list") || return undef;
        while(<LST>){
                chomp;
                /\S/ or next;
                /^"$file"/ and next;
                push @list_buf,$_;
        }
        close(LST);
        my $md5sum = `md5sum $buildroot/$file`;
        $md5sum =~ s/\s+\S+//;
        $md5sum =~ /^[a-f|0-9]{32}$/ || return undef;
        push @list_buf,"\"$file\" $md5sum";
        open (LST,">$buildroot/$list") || return undef;
        foreach (sort @list_buf){
                print LST "$_\n";
        }
        close(LST);
        return 1;
}

sub remove_file{
        my ($buildroot,$file,$list) = @_;
        $buildroot =~ s/\\/\//g;
        $buildroot =~ s/\/$//;
        $file =~ s/\\/\//g;
        my $list_file = $buildroot.'/'.$list;
        -f $list_file || return undef;
        my $path = $buildroot.'/'.$file;
        -f $path || return undef;
        unlink($path) || return undef;
        open (LST,$list_file) || return undef;
        my $found = 0;
        my @list_buf;
        while(<LST>){
                chomp;
                /\S/ or next;
                unless(/"$file"/){
					$found = 1;
					next;
                }
                push @list_buf,$_;
        }
        close(LST);
        return undef unless $found;
        open (LST,'>'.$list_file) || return undef;
        foreach ( @list_buf){
                print LST "$_\n";
        }
        close(LST);
        return 1;
}

sub do_cmd{
        my ($cmd) = @_;
        msg("processing $cmd\n");
        my $outtxt = `$cmd 2>&1`;
        if($? != 0){
                err("$cmd: $outtxt\n");
                return;
        }
        return 1;
}

sub deltree{
        my ($dir) = @_;
        -d $dir || return 0;
        opendir(DH,$dir) || return 0;
        my @content = readdir(DH);
        closedir(DH);

        my @files = grep { -f "$dir/$_" } @content;
        my @dirs = grep { -d "$dir/$_" && ! /^\.{1,2}$/} @content;

        
		#
		#  enable write permission in directory to delete entries
		#

		
		unless($^O =~ /mswin/i){
			if($#files > -1 || $#dirs > -1){
				my @statbuf = stat($dir);
				my $mask;
				if($> == $statbuf[4]){
					$mask = 0200;
				}
				else{
					my $found;
					foreach my $gid (split(' ',$) )){
						if($gid == $statbuf[5]){
							$found = 1;
							last;
						} 
					}
					if($found){
						$mask = 020;
					}
					else{
						$mask = 002;
					}
				}
				if(($statbuf[2] & $mask) == 0){
					chmod($statbuf[2] | $mask,$dir) || err("cannot enable write permissions for $dir\n");
				}
			}
		}

		foreach (@dirs){
                -l "$dir/$_" and next;
                deltree("$dir/$_") || return 0;
        }
        foreach (@files){
                -l "$dir/$_" and next;
		    -f "$dir/$_" and (unlink("$dir/$_") or err("cannot delete $dir/$_\n") and return 0);
        }
        rmdir($dir) or err("cannot delete $dir\n") and return 0;
        return 1;
}


sub makedir{
        my ($dir,$mode,$ref_uid,$ref_gid) = @_;
        $dir =~ s/\\/\//g;
        $dir =~ s/\/$//;
        -d $dir && return 1;
		my ($parent) = ($dir =~ /^(.*)\/[^\/]+$/);
	    
		if($dir !~ /\// || -d $parent){
				
				my $pperm_changed = 0;
				my @pstatbuf;

				if($parent =~ /\S/){
					if($^O !~ /mswin/i and $< != 0){
						@pstatbuf = stat ($parent);
						my $dirmode = $pstatbuf[2] & 0777;	
						if($pstatbuf[4] == $<){
							if(($pstatbuf[2] & 0200) == 0){
								$pperm_changed = 1;
								chmod($pstatbuf[2] | 0200,$parent);
							}
						}
					}
				}

				mkdir($dir,$mode) || return 0;
                unless($^O =~ /mswin/i){
					#
					# workaround for perl bug
					#
					my @statbuf = stat ($dir);
					my $filemode = $statbuf[2] & 0777;
					if ($filemode != $mode) {
						chmod ($mode, $dir) || return 0;
					}
					if(defined $$ref_uid || defined $$ref_gid){
						my ($uid,$gid) = ($statbuf[4],$statbuf[5]);
						defined $$ref_uid and $uid = $$ref_uid;
						defined $$ref_gid and $gid = $$ref_gid;
						chown($uid,$gid,$dir) or $! = "cannot change ownership of directory $dir: $!" and return 0; 
					}	
				}
				chmod($pstatbuf[2],$parent) if ($pperm_changed);
				return 1;
        }
        makedir($parent,$mode) || return 0;
 		
		my $pperm_changed = 0;
		my @pstatbuf;

		if($parent =~ /\S/){
			if($^O !~ /mswin/i and $< != 0){
				@pstatbuf = stat ($parent);
				my $dirmode = $pstatbuf[2] & 0777;	
				if($pstatbuf[4] == $<){
					if(($pstatbuf[2] & 0200) == 0){
						$pperm_changed = 1;
						chmod($pstatbuf[2] | 0200,$parent);
					}
				}
			}

		}
		
		mkdir($dir,$mode) || return 0;
		unless($^O =~ /mswin/i){
			#
			# workaround for perl bug
			#
			my @statbuf = stat ($dir);
			my $filemode = $statbuf[2] & 0777;
			if ($filemode != $mode) {
				chmod ($mode, $dir) || return 0;
			}
		}
		chmod($pstatbuf[2],$parent) if ($pperm_changed);
		return 1;
}


sub readini{
	my ($file) = @_;
	my %returnvalue;
	open(FH,$file) or return undef;
	my $section = 'main';
	while(<FH>){
		chomp;
		if(/^\s*\[.+\]\s*$/){
			($section) = (/^\s*\[(.+)\]\s*$/);	
		}
		elsif(/^.+=.+$/){
			my ($key,$value) = (/^(.+)=(.+)$/);
			$key =~ s/^\s*//;
			$key =~ s/\s*$//;
			$value =~ s/^\s*//;
			$value =~ s/\s*$//;
			if(exists $returnvalue{$section} && ref($returnvalue{$section}) eq  'HASH'){
				$returnvalue{$section}->{$key} = $value;		
			}
			else{
				$returnvalue{$section} = {$key => $value}; 	
			}
		}
	}
	close(FH);
	return %returnvalue;
}



sub find {
        my ($dir,$type) = @_;
        my @returnvalue;
        -d $dir or return undef;
        opendir(DH,$dir) || return undef;
        my @content = readdir(DH);
        closedir(DH);

        my @files = grep { -f "$dir/$_" } @content;
        my @dirs = grep { -d "$dir/$_" && ! /^\.{1,2}$/} @content;


        foreach(@dirs){
                -l "$dir/$_" and next;
                my @result = find("$dir/$_",$type);
                $#result > -1 && push @returnvalue,@result;
        }

        if($type !~ /^d$/i){
                foreach (@files){
                        push @returnvalue,"$dir/$_";
                }
        }
        if($type !~ /^f$/i){
                foreach (@dirs){
                        push @returnvalue,"$dir/$_";
                }
        }
        return @returnvalue;
}


sub ftp_connect{
	my ($host,$user,$passwd) = @_;
	my $ftp = Net::FTP->new($host)
			or err("cannot get ftp connection to host $host\n");
	
	if($user){
		$ftp->login($user,$passwd) or err('FTP: login to host '.$host.' failed: '.$ftp->message . "\n");				
	}	
	else{
		unless($ftp->login('remuser','Remuser')){
			$ftp->login('anonymous') or err('FTP: login to host '.$host.' failed: '.$ftp->message . "\n");
		}
	}
	return $ftp;	
}

sub ftp_release{
	my ($ftp) = @_;
	$ftp->quit or err('FTP: '.$ftp->message."\n");
	return 1;
}




sub remoteGet{
	my ($host,$source_file,$destination,$binary)=@_;		
	
	#-d $destination_dir or err ("directory \"$destination_dir\" not found\n");
	
	my $ftp;
	my $new_connection = 0;
	
	$DEBUG && print ">>>>> host / ftp obj = $host / ".ref($host)."\n";
	
	if(ref($host) eq 'Net::FTP'){
		$ftp = $host;
	}
	else{
		$new_connection = 1;
		$ftp= Net::FTP->new($host)
			or err("cannot get ftp connection to host $host\n");
	
		unless($ftp->login('remuser','Remuser')){
			 $ftp->login('anonymous') or err('FTP: '.$ftp->message . "\n");
		}
	}
		
	if($binary){
		$ftp->binary() or err('FTP: '.$ftp->message."\n");
	}
		
	my($remoteDir,$remoteFile) = ($FTPSRC =~ /^(.*)\/([^\/]+)$/);  
	
	$ftp->get($source_file,$destination) or err('FTP: '.$ftp->message."\n");
	if($new_connection){
		$ftp->quit or err('FTP: '.$ftp->message."\n");
	}
	return 1;
}

sub modeStr2Num{
	my ($modstr) = @_;
	$modstr =~ /^[rwxs-]{9}$/ or err("mode string \"$modstr\" has wrong format\n"); 
	$modstr =~ s/[wrx]/1/g;
	$modstr =~ s/-/0/g;
	return unpack("N", pack("B32", substr("0" x 32 . $modstr, -32)));
}


$DEBUG = 0;

sub remoteLs{
	my ($host,$source_dir)=@_;		
	my %entries; 
	my $ftp;
	my $new_connection = 0;
	 
	$DEBUG && print ">>>>> host / ftp obj = $host / ".ref($host)."\n";
	
	if(ref($host) eq 'Net::FTP'){
		$ftp = $host;
	}
	else{
		$new_connection = 1;
		$ftp= Net::FTP->new($host)
			or err("cannot get ftp connection to host $host\n");
	
		unless($ftp->login('remuser','Remuser')){
			$ftp->login('anonymous') or err('FTP: '.$ftp->message . "\n");
		}
	}
	#$ftp->chdir($source_dir) or err('FTP: '.$ftp->message . "\n");

	my @list = $ftp->dir($source_dir) or err("FTP: cannot access path $source_dir\n");
	
	foreach my $line (@list){
		$DEBUG && print ">>>>$line\n";
		my %entry_data;
		my ($perm,$size,$month,$day,$time_year,$name) = ($line =~ /^(\S+)\s+\S+\s+\S+\s+\S*\s+(\d+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S.*$)/) or next;
		(my $type,$perm) = ($perm =~ /^([ld-])(.+)/);
		$type = 'f' if $type eq '-';
		#($type =~ /^[dfl]$/) or next;
		my $dest;
		$entry_data{'type'} = $type;
		if ($type eq 'l'){
			($name,$dest) = ($name =~ /^(.+)\s+->\s+(.+)$/);
			$entry_data{'dest'} = $dest;
		}
		
		my $mindex = 0;
		foreach my $mon (Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec){
			$month =~ /$mon/i and last;
			$mindex++;
		}
		$mindex > 12 and err("cannot get month\n");
		$month = $mindex;
		my $year;
		my $sec = 0;
		my $min;
		my $hours;
		if($time_year =~ /:/){
			$year = (localtime(time))[5] + 1900;
			if($mindex > (localtime(time))[4]){
				$year--;
			}
			($hours,$min) = ($time_year =~ /^(\d+):(\d+)$/);
		}
		else{
			$min = 0;
			$hours = 0;
			$year = $time_year;
		}
		
		my $time = timelocal($sec,$min,$hours,$day,$month,$year);
		$entry_data{'time'} = $time;
		my $time_str = localtime($time);
		$DEBUG && print ">>>>time = $time_str\n";
		$DEBUG && print ">>>>> name => $name\n";
		$DEBUG && print ">>>>> resolved => $dest\n" if defined $dest;
		$DEBUG && print ">>>>> type => $type\n";
		my $mode =modeStr2Num($perm);
		$entry_data{'mode'} = $mode;
		$DEBUG && print ">>>>> mode = ".(sprintf('%o',$mode))."\n";
		$entries{$name} = \%entry_data;
	}
	if($new_connection){	
		$ftp->quit or err('FTP: '.$ftp->message."\n");
	}
	return %entries;
}





1;






