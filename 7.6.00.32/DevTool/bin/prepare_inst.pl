#!/usr/bin/perl

#
#	prepare_inst.pl
#
#	part of SAP DB development tools
#
#	create config directory /usr/spool/sql and entries in /etc/services
#


$^O =~ /mswin/i and print STDERR "use script for UNIX only\n" and exit -1;

$< != 0 and print STDERR "please start script as root\n" and exit -1;

$config_dir = "/usr/spool/sql";
$config2_dir = "/usr/spool/sql/config";
$fifo_dir = "/usr/spool/sql/fifo";

%services = ( 	'sql6' => '7210/tcp');
$sf = '/etc/services';



if(-d $config_dir){
	print "directory $config_dir already exists\n";
}
else{
	-e $config_dir && print STDERR "$config_dir is no directory\n" && exit -1;
	-d '/usr/spool' or (mkdir ('/usr/spool',0775) or print STDERR "cannot create directory /usr/spool: $!" and exit -1);
	mkdir $config_dir,0755 or print STDERR "cannot create directory $config_dir: $!" and exit -1;
	chmod(0777,$config_dir) or print STDERR "cannot chmod $config_dir\n" and exit -1;	
	mkdir $config2_dir,0755 or print STDERR "cannot create directory $config2_dir: $!" and exit -1;
	chmod(0777,$config2_dir) or print STDERR "cannot chmod $config2_dir\n" and exit -1;	
	print "directories $config_dir, $config2_dir are world writable now!\n";
	print "please change permissions to access with your develop/sap db users only\n";
}

if(-d $fifo_dir){
	print "directory $fifo_dir already exists\n";
}
else{
	mkdir $fifo_dir,0755 or print STDERR "cannot create directory $fifo_dir: $!" and exit -1;
	chmod(0777,$fifo_dir) or print STDERR "cannot chmod $fifo_dir\n" and exit -1;	
	print "directory $fifo_dir is world writable now!\n";
	print "please change permissions to access with your develop/sap db users only\n";
}


my @buf;

open(FD,$sf) or print STDERR "cannot open file $sf\n" and exit -1;
while(<FD>){
	chomp;
	push @buf,$_;	
}
close(FD);


foreach my $name (keys(%services)){
	$found = 0;
	foreach (@buf){
		my $pattern = '^\s*'.$name.'\s';
		/$pattern/ and $found = 1 and last;
	}
	$found and print "$name in $sf found\n" and next;
	print "append $name to $sf\n";
	open (FD,'>>'.$sf) or print STDERR "cannot open file $sf: $!\n" and exit -1;
	print FD "$name\t\t".$services{$name}."\t#SAP DB\n";
	close(FD);
}

unless(-d '/etc/opt'){
	mkdir('/etc/opt');
	chmod(0555,'/etc/opt');
}

unless(-f '/etc/opt/sdb'){
	open(FD,'>/etc/opt/sdb');
	print FD "[Globals]\n";
	print FD "IndepProgramPath=/opt/sdb/programs\n";
	print FD "IndepDataPath=/var/opt/sdb/data\n";
	print FD "SdbOwner=sdb\n";
	print FD "SdbGroup=sdba\n";
	close(FD);
	chmod(0444,'/etc/opt/sdb');
}

exit 0;