#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/PatchTar.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc: 	module maniputates tar header
#		needed to set unix file permissions inside tar archive packed by windows tar	
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


package BuildPackage::PatchTar;


$DEBUG = 0;


my %members = (
	'fd' => undef,
	'archive_name' => undef,
	'cur_header_data' => undef,
	'cur_pos' => undef,
	'cur_header_pos' => undef,
 	'patch_data' => undef,
);


sub readMagic{
	my ($self) = @_;
	$self->{cur_pos} == 0 or $self->reset;
	my $magic;
	my $read = sysread($self->{fd},$magic,8);
	unless($read == 8){
		print STDERR ("cannot read tar header\n");
		return undef;
	}
	unless($magic =~ /^CAR\s2\.00$/){
		print STDERR ("unknown magic\n");
		return undef;
	}
	$self->{cur_pos} += 8;
	return 1;
}

sub readHeader{
	my ($self) = @_;
	my $header;
	my %header_data;
	$DEBUG && print ">>>>>> current position = ".(tell($self->{fd}))."\n";
	my $read = sysread($self->{fd},$header,512);
	
	unless($read or $read == 512){
		print STDERR ("cannot read tar header\n");
		return undef;
	}
	
	$self->{cur_pos} += 512;
	
	$DEBUG && print ">>>> hearder = $header\n";

	my ($name,$mode,$uid,$gid,$size,$mtime,
		$chksum,$typeflag,$linkname,$magic,
		$version,$uname,$gname,$devmajor,
		$devminor,$prefix) = ($header =~ /^(.{100})(.{8})(.{8})(.{8})(.{12})(.{12})(.{8})(.)(.{100})(.{6})(..)(.{32})(.{32})(.{8})(.{8})(.{155})/) or
		die("invalid tar header at ".$self->{cur_pos}." \n");	

	
	return undef if (unpack(LS,$magic))[0] == 0;
	die("invalid magic $magic at ".$self->{cur_pos}."\n") unless $magic =~ /ustar/;
	$self->{cur_header} = $header;
	$header_data{name} = ($prefix =~ /[\w\d]/ ? "$prefix/$name" : $name);
	$header_data{name} =~ s/\0*$//;
	$header_data{mode} = oct $mode;
	$header_data{mode} &= 0777;
	$header_data{date} = oct $mtime;
	$header_data{type} = ($typeflag == 0 ? 'f' : 'd');
	$header_data{size} = oct $size;
	$DEBUG && print ">>>>>> uid: $uid\n";
	$DEBUG && print ">>>>>> gid: $gid\n";
	$DEBUG && print ">>>>>> typeflag: $typeflag\n";
	$DEBUG && print ">>>>>> chksum: $chksum\n";
	$DEBUG && print ">>>>>> mtime: $mtime\n";
	$DEBUG && print ">>>>>> size: $size\n";



	$DEBUG && print ">>>>>> name: ".$header_data{name}."\n";
	$DEBUG && print ">>>>>> mode: ".$header_data{mode}."\n";
	$DEBUG && print ">>>>>> date: ".(localtime($header_data{date}))."\n";
	$DEBUG && print ">>>>>> size: ".$header_data{size}."\n";

	return \%header_data;
}



sub new{
	my ($type,$file_name,$patch_data) = @_;
	my $class = ref($type) || $type;
	my $self ={
		%members
	};
	-f $file_name or die("tar archive \"$file_name\" not found\n");
	open(SARFD,'+<'.$file_name) or die("cannot open tar archive $file_name: $!");
	binmode(SARFD);
	bless $self,$class;
	$self->{fd} = \*SARFD;
	$self->{archive_name} = $file_name;
	$self->{patch_data} = $patch_data;
	$self->{cur_pos} = 0;
	return $self;
}


sub reset{
	my ($self) = @_;
	$self->{cur_pos} = 0;
	$self->{cur_header_data} = undef;
	unless(sysseek($self->{fd},0,0)){
		print STDERR ("cannot seek file ".$self->{archive_name}.": $!\n");
		$self->{cur_pos} = undef;
		return undef;
	}
	return 1;
}

sub next{
	my ($self) = @_;
	if(defined $self->{cur_header_data} && $self->{cur_header_data}->{type} eq 'f'){
		my $position = $self->{cur_pos} + $self->{cur_header_data}->{size} + 512 - ($self->{cur_header_data}->{size} % 512) - (($self->{cur_header_data}->{size} % 512) == 0 ? 512 : 0);
		sysseek($self->{fd},$position,0) or return undef;
		$self->{cur_pos} = $position;
	}
	$self->{cur_header_pos} = $self->{cur_pos};	
	my $data = $self->readHeader or return undef;
	$self->{cur_header_data} = $data;
	return $data;
}


sub tar_checksum {
	$h = $_[0];
	local($i, $s);
	$s = 0;
	for($i = 0; $i < 512; $i++) {
		$s += unpack('C', substr($h, $i, 1));
	}
	return $s;
}




sub patchPermissions{
	my ($self,$patch_data) = @_;
	#$self->reset;
	while(1){
		my $data = $self->next or last;
		#print ">>>> 1\n";
		last if(ref($data) ne 'HASH');
		#print ">>>> 2\n";	
		my %data = %$data;
		#print ">>>>> test $data{name}";
		foreach my $key (keys(%$patch_data)){
			my $pattern = $key;
			$pattern =~ s/\*/\.\*/g;
			$pattern =~ s/\\/\//g;
			$pattern =~ s/\//\\\//g;
			$pattern =~ s/\?/\./g;
			$pattern = '^'.$pattern.'$';
			#print ">>>>> test $data{name} -> $pattern\n";

			if($data{name} =~ /$pattern/){
				#print ">>>>> test\n";
				if($data{mode} == $patch_data->{$key}){
					print "permission of $data{name} is already ".sprintf("%o",$data{mode})."\n";
				}
				else{
					print "set permissions of $data{name} from ".sprintf("%o",$data{mode})." to ".sprintf("%o",$patch_data->{$key})."\n"; 
					
					#
					#	write new permissions in tar header
					#
					
					sysseek($self->{fd},$self->{cur_header_pos} + 100,0);
					my @packs = ();

					foreach my $char (0,$data{type} eq 'f' ? 1 : 0,$data{type} eq 'd' ? 4 : 0,0,(split('',sprintf("%o",$patch_data->{$key})))){
						 push @packs,ord($char);
					}
					push @packs,0;
					my $written = syswrite($self->{fd},pack('C8',@packs),8);
					unless($written == 8){
						print STDERR ("cannot write new mode\n");
						return undef;
					}
			
					#
					#	write new tar header checksum
					#

					my $tmp_header = $self->{cur_header};
					substr($tmp_header,148,8) = pack('A8',"");
					substr($tmp_header,100,8) = pack('C8',@packs);
					my $checksum = sprintf("%6o\0", unpack("%16C*",$tmp_header));
					$checksum =~ s/\s/0/;
					sysseek($self->{fd},$self->{cur_header_pos} + 148,0);
					$written = syswrite($self->{fd},pack(A6,$checksum),6);
					unless($written == 6){
						print STDERR ("cannot write new checksum\n");
						print STDERR ("total written: $written\n");
						return undef;
					}
					sysseek($self->{fd},$self->{cur_pos},0);
				}	
				last;
			}
			
		}
		#print "$data{type} $data{size} byte ".sprintf("%o",$data{mode})." ".localtime($data{date})."$data{name}\n";
	}
	
}





sub DESTROY{
	my ($self) = @_;
	$DEBUG && print ">>>>> destroy\n";
	$DEBUG && ">>>>> last position = ".$self->{cur_pos}."\n";
	close($self->{fd});
}




1;






