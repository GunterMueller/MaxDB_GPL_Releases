#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/PatchSar.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc: 	module maniputates sar header
#		needed to set unix file permissions inside sar archive packed by windows SAPCAR	
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


package BuildPackage::PatchSar;


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
		print STDERR ("cannot read sar header\n");
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
	$DEBUG && print ">>>>>> current position = ".$self->{cur_pos}."\n";
	my $read = sysread($self->{fd},$header,34);
	unless($read == 34){
		print STDERR ("cannot read sar header\n");
		return undef;
	}
	$self->{cur_pos} += 34;
	$DEBUG && print ">>>>> len header = ".(length($header))."\n";
	$DEBUG && print (join('|',unpack("C34",$header)))."\n";
	
	my ($type,$mode,$reserve1,$size,$reserve2,$reserve3,$date,
		$reserve4,$reserve5,$reserve6,$len_name,$reserve7) = 
		unpack('SSSLLLLLLSCC',$header) or die("invalid sar header\n"); 
		
	my $name;
	$type = ($type == 0x4752 ? 'f' : ($type == 0x5244 ? 'd' : undef));
	die("invalid sar header: unkown type\n") unless(defined $type);
	$DEBUG && print ">>>>> len name = $len_name\n";
	$DEBUG && print ">>>>size = $size\n";
	$mode = $mode & 0777;
	$DEBUG && print ">>>>>type = $type\n";
	$DEBUG && print ">>>>>mode = ".sprintf("%o",$mode)."\n";	
	$DEBUG && print ">>>>>date = ".localtime($date)."\n";	
	if($type eq 'd'){
		$read = sysread($self->{fd},$name,$len_name);
		unless ($read == $len_name){
			$self->{cur_pos} += $read;
			print STDERR ("cannot read sar header\n");
			return undef;
		}
		$self->{cur_pos} += $len_name;	
		$DEBUG && print ">>>>> name = $name\n";
	}
	else{
		$read = sysread($self->{fd},$name,$len_name);
		unless ($read == $len_name){
			print STDERR ("cannot read sar header\n");
			$self->{cur_pos} += $read;
			return undef;
		}
		$self->{cur_pos} += $len_name;	
		$DEBUG && print ">>>>> name = $name\n";
	}
	$header_data{'name'} = $name;
	$header_data{'type'} = $type;
	$header_data{'size'} = $size;
	$header_data{'mode'} = $mode;
	$header_data{'date'} = $date;
	return \%header_data;
}



sub findPos{
	my ($self,$page,$pagesize) = @_;
	my $pos;
	my $index = 0;
	my @page = unpack("C$pagesize",$page);
	foreach my $char (@page){
		#$DEBUG && print ">>>>>> $index:".(sprintf("%x",$char))."\n"; 	
		if($char == 0x44 && $page[$index + 1] == 0x52 && $page[$index + 4] == 0 && $page[$index + 5] == 0){
			# found directory hearder
			$pos = $index;
			last;	
		}
		elsif($char == 0x52 && $page[$index + 1] == 0x47 && $page[$index + 4] == 0 && $page[$index + 5] == 0){
			# found file hearder
			$DEBUG && print ">>>>>> found header at offset".(sprintf("hex: %x | dec: %d",$index,$index))."\n"; 	
			$pos = $index;
			last;	
		}		
		$index++;
	}	
	return $pos;
}



sub findNextHeader{
	my ($self) = @_;
	my $startpos;
	my $pagesize = 8 * 1024;
	my $cur_pos = $self->{cur_pos};
	while(1){ 
		my $page;
		#$DEBUG && print "current position (real) = ".tell($self->{fd})."\n";
		#$DEBUG && print "current position (own calculated) = ".$self->{cur_pos}."\n";
		
		my $read = sysread($self->{fd},$page,$pagesize);
		
		unless($read == ($pagesize)){
			$DEBUG && print ">>>>>> could not read $pagesize bytes\n";
			$DEBUG && print ">>>>>> start position = $cur_pos\n";
			$DEBUG && print ">>>>>> totally $read bytes read\n";
			$read or return undef;
		}
		
		$self->{cur_pos} += $read;	
		
		my $offset = $self->findPos($page,$read);
		#if($page =~ m/(DR..\0\0)|(RG..\0\0)/gc){
			#my $offset = (pos $page) - 6;
			#$DEBUG && print "$1 | $2 at $offset\n";
		if($offset){
			$DEBUG && print "page adress = $cur_pos\n";	
			$DEBUG && print "found header\n";
			$DEBUG && print "header start position = ".($cur_pos + $offset)."\n";
			return $cur_pos + $offset;
		}
		$cur_pos += $pagesize;
	}
	return undef;
}





sub new{
	my ($type,$file_name,$patch_data) = @_;
	my $class = ref($type) || $type;
	my $self ={
		%members
	};
	-f $file_name or die("sar archive \"$file_name\" not found\n");
	open(SARFD,'+<'.$file_name) or die("cannot open sar archive $file_name: $!");
	binmode(SARFD);
	bless $self,$class;
	$self->{fd} = \*SARFD;
	$self->{archive_name} = $file_name;
	$self->{patch_data} = $patch_data;
	$self->{cur_pos} = 0;
	$self->readMagic or die("unsupported format\n");
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
		my $position = $self->findNextHeader or return undef;
		sysseek($self->{fd},$position,0);
		$self->{cur_pos} = $position;
	}
	$self->{cur_header_pos} = $self->{cur_pos};	
	my $data = $self->readHeader or return undef;
	$self->{cur_header_data} = $data;
	return $data;
}


sub patchPermissions{
	my ($self,$patch_data) = @_;
	#$self->reset;
	while(1){
		my $data = $self->next or last;
		last if(ref($data) ne 'HASH');
		my %data = %$data;
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
					sysseek($self->{fd},$self->{cur_header_pos} + 2,0);
					my $written = syswrite($self->{fd},pack('S',($data{type} eq 'f' ? 0100000 : 040000) | $patch_data->{$key}),2);
					unless($written == 2){
						print STDERR ("cannot write\n");
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






