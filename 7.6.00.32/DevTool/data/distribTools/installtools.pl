#!/devtool/local/bin/perl
#
#
#	installtools.pl
#
#	install MaxDB development tools
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



package SAPDB::DevTools::Install;
use Cwd;
$editor;
$curDir=cwd();

%replace;

sub testSubDirs{
	my ($dir) = @_;
	my $ready=1;
	foreach my $subdir ('bin','incl','lib','pgm'){
		-d "$dir/$subdir" or print "something wrong with directory structure: \"$dir/$subdir\" not found\n" and $ready = 0;
	}
	return $ready;
}

sub jmpDir{
	my ($dir)=@_;
	$dir =~ s/\\/\//g;
	my $scriptdir=$0;
	$scriptdir=~s/\\/\//g;
	$scriptdir=~s/\/[^\/]*$//;
	if($^O=~/mswin/i){
		$scriptdir=~/^$dir$/i or chdir($scriptdir);
	}
	else{
		$scriptdir=~/^$dir$/ or chdir($scriptdir);
	}
}

sub which{
# search for file PATH- variable
	my ($name)= @_;
	my $pathvar=$ENV{'PATH'};
	my @path;
	if($^O=~/mswin/i){
		@path = split(';',$pathvar);
	}
	else{
		@path = split(':',$pathvar);
	}
	if($name=~/^\// or $name=~/^[a-z]:/i){
		my $executable = $name;
		$executable=~s/\\/\//g;
		$_= $executable;	
		if($^O=~/mswin/i){
			if(/\.exe$/i or /\.com$/i or /\.bat/i or /\.cmd$/i){
				-f $executable and return $executable;
			}
			else{
				foreach my $ext ('.exe','.com','.bat','.cmd'){
					-f "$executable$ext" and return "$executable$ext";
				}
			}
		}
		else{
			-x $executable and return $executable;
		}
	}
	else{
		foreach my $path (@path){
			my $executable= "$path/$name";
			$executable=~s/\\/\//g;
			$_=$executable;
			if($^O=~/mswin/i){
				if(/\.exe$/i or /\.com$/i or /\.bat/i or /\.cmd$/i){
					-f $executable and return $executable;
				}
				else{
					foreach my $ext ('.exe','.com','.bat','.cmd'){
						-f "$executable$ext" and return "$executable$ext";
					}
				}
			}
			else{
				-x $executable and return $executable;
			}
		}
	}
	print "$name not in path\n";
	return 0;	
}


sub getPerlPath{
	use Config;

	if($^O=~/mswin/i){
		my $perlexe=$^X;
		$perlexe=~/^(.*)\\bin\\perl.exe$/i or print "cannot get perl path\n" and return 0;
		return $1;
	}
	else{
		return 1;
	}
}


sub getPythonPath{
	if($^O=~/mswin/i){
		# python path with python executable
		my $pythonexe=which('python');
		$pythonexe=~s/\\/\//g;
		$pythonexe=~/^(.*)\/python.exe$/i;
		return $1;
	}
    else {
		my $pythonexe=which('python');
		$pythonexe=~/^(.*)\/python$/i;
		return $1;
	}
}
	

sub main{
	print "\n\n\t\tInstallation Of MaxDB Development Tools\n";
	print "\t\t****************************************\n\n";
	my @ARGV=@_;
	my $ready=1;
	jmpDir($curDir);
	$curDir=cwd();
	print "your ToolDir is: $curDir\n";
	my $errtext= "\nMaxDB development tools not installed\n",
	testSubDirs($curDir) or $ready=0;
	SAPDB::DevTools::Install::TestPrograms::Perl() or $ready=0;
	$perlpath=getPerlPath() or $ready=0;
	die $errtext unless $ready;
	$pythonpath=getPythonPath();
	SAPDB::DevTools::Install::TestPrograms::Python($pythonpath) or $ready=0;
	die $errtext unless $ready;
	#gar
	unless($^O=~/mswin/i){
		SAPDB::DevTools::Install::TestPrograms::Bison() or die $errtext;
		SAPDB::DevTools::Install::TestPrograms::Flex() or die $errtext;
	}
	SAPDB::DevTools::Install::TestPrograms::Editor() and print "your editor is ok\n" or $ready=0;
	unless($^O=~/mswin/i){
		if ($^O =~ /linux/i){
			SAPDB::DevTools::Install::TestPrograms::linkPosixTools() or die $errtext;
		}
		else{
			-l "$curDir/Posix" or symlink('/usr/bin',"$curDir/Posix") or print "cannot create symbolic link \"$curDir/Posix -> /usr/bin\"; please do it manually $!\n";
		}
	}
	die $errtext unless $ready;
        unless($^O=~/mswin/i){
            system ("perl", "$curDir/bin/instperltools.pl", "$curDir/bin");
        }
	$SAPDB::DevTools::Install::Registry::values{'ToolPath'}=$curDir;
	$SAPDB::DevTools::Install::Registry::values{'Editor'}=$editor;
	if($^O=~/mswin/i){
		$curDir=~s/\//\\/g;
		$editor=~s/\//\\/g;
		$perlpath=~s/\//\\/g;	
		$pythonpath=~s/\//\\/g;
	}
	%replace=(
		'§DEVTOOL' => $curDir,
		'§EDITOR' => $editor,
		'§PERL' => $perlpath,
		'§PYTHON' => $pythonpath,
	);
	SAPDB::DevTools::Install::IProfile::replace(%replace) or $ready=0;
	SAPDB::DevTools::Install::Registry::write() or print "could not write in registry - during every installation of sources you have to type path of devtools :-(\n";
	die $errtext unless $ready;
	print "\nMaxDB development tools successfully installed\n";
	exit 0;
}

package SAPDB::DevTools::Install::TestPrograms;
use Cwd;
$curDir=cwd();


%rc=(
				'SUCCEED' => 0,
				'UNKNOWN_ERR' => -1,
				'PIPE_ERR' => -2,
				'EXEC_ERR' => -3,
				'TEST_FAILED' => -4,
				'WRONG_VERSION' => -5,
				'NOT_FOUND' => -6,	
	);

sub callsystem{
	my ($commandline,$testfunc)=@_;
	*checkfunc=$testfunc; # function reference to test stout of system call
    $!="";
    open OUT, "$commandline 2>&1 |" or return $rc{'PIPE_ERR'};
    my $outtext;
	while($line=<OUT>){
		$outtext=$outtext.$line."\n";
    }
	if($testfunc){
		return checkfunc($outtext);
	}
	else{
		unless($?){
			$!="";
		}
		if($!){
			return $rc{'UNKNOWN_ERR'};
		}
		close OUT or return $rc{'EXEC_ERR'};
	}
	return $rc{'SUCCEED'};
}


sub testPython{
	($_)=@_;
	/^([0-9a-f]*)\s*$/ or return $rc{'NOT_FOUND'};
	my $hexversion=$1;
	if(hex($hexversion) < hex('10502f0')){
		return $RC{'WRONG_VERSION'};
	}
	return $rc{'SUCCEED'};
}

sub testEditor{
	# search for file PATH- variable
	my ($name)= @_;
	my $pathvar=$ENV{'PATH'};
	my @path;
	if($^O=~/mswin/i){
		@path = split(';',$pathvar);
	}
	else{
		@path = split(':',$pathvar);
	}
	if($name=~/^\// or $name=~/^[a-z]:/i){
		my $editor = $name;
		if($^O=~/mswin/i){
			if(/\.exe$/i or /\.com$/i or /\.bat/i or /\.cmd$/i){
				-f $editor and return 1;
			}
			else{
				foreach my $ext ('.exe','.com','.bat','.cmd'){
					-f "$editor$ext" and return 1;
				}
			}
		}
		else{
			-x $editor and return 1;
		}
	}
	else{
		foreach my $path (@path){
			my $editor= "$path/$name";
			$editor=~s/\\/\//g;
			$_=$editor;
			if($^O=~/mswin/i){
				if(/\.exe$/i or /\.com$/i or /\.bat/i or /\.cmd$/i){
					-f $editor and return 1;
				}
				else{
					foreach my $ext ('.exe','.com','.bat','.cmd'){
						-f "$editor$ext" and return 1;
					}
				}
			}
			else{
				-x $editor and return 1;
			}
		}
	}
	return 0;	
}


sub Editor{
	while(1){
		if($^O=~/mswin/i) {
            $defaultEditor = 'notepad.exe';
        }
        else {
            $defaultEditor = 'vi';
        }
		print "please enter your favourite editor (must be in PATH) [$defaultEditor]: ";
		chomp(my $editor = <STDIN>);
        if ($editor eq '') {
            $editor = $defaultEditor;
        }
		testEditor($editor) and $SAPDB::DevTools::Install::editor = $editor and return 1;
		print "editor \"$editor\" not found\n";
		print "abort installation (y/n)? ";
		chomp($_=<STDIN>);
		/^y$/i and return 0;	
	}
}


sub Python{
	my ($path) = @_;
	-d $path and $path=$path.'/';
	$path=~s/\//\\/g if($^O=~/mswin/i);
	my $returned=callsystem("python -c \"import sys; print '%x' % sys.hexversion\"",\&testPython);
	SWITCH: {
		($returned == $rc{'PIPE_ERR'}) && do {print "something wrong with your system: cannot open pipe\n"; last SWITCH;};
		($returned == $rc{'EXEC_ERR'}) && do {print "no python installed or in not in PATH\n"; last SWITCH;};
		($returned == $rc{'UNKNOWN_ERR'}) && do {print "unknown error while testing python, make sure that python is correct installed\n"; last SWITCH;};
		($returned == $rc{'NOT_FOUND'}) && do {print "python not installed\n"; last SWITCH;};
		($returned == $rc{'WRONG_VERSION'}) && do {print "wrong python version installed\n"; last SWITCH;};
		($returned == $rc{'SUCCEED'}) && do {print "your python is ok\n"; return 1;};			
	}
	return 0; #error
}

sub Perl{
		if($]<5.00503){
			print "wrong perl verion, please update to min version 5.005 patch 03\n";
			return 0;
		}
		print "your perl is ok\n";
		return 1;
}

sub Flex{
	if($^O=~/mswin/i){
		my $flexexe=SAPDB::DevTools::Install::which('flex');
		if ( ! $flexexe ){
			print "cannot found flex.exe in the path\n";
			return 0;
		}
	}
	else
	{
		unless ( -l "$curDir/gnu/bin/flex")
		{
			
			my $flexexe=getExecutablePath('flex');
			return 0 unless ( $flexexe );
			-d "$curDir/gnu" or mkdir ("$curDir/gnu", 0775) or print "cannot create directory $curDir/gnu" and return 0;	
			-d "$curDir/gnu/bin" or mkdir ("$curDir/gnu/bin", 0775) or print "cannot create directory $curDir/gnu/bin" and return 0;	
			symlink("$flexexe" ,"$curDir/gnu/bin/flex") or
				print "cannot create symbolic link \"$curDir/gnu/bin/flex -> $flexexe\": $!\n" and return 0;
		}
	}
	return 1;
}


sub Bison{
	if($^O=~/mswin/i){
		my $bisonexe=SAPDB::DevTools::Install::which('bison');
		if ( ! $bisonexe ){
			print "cannot found bison.exe in the path\n";
			return 0;
		}
	}
	else{
		unless ( -x "$curDir/gnu/bin/bison"){
			my $bisonexe=getExecutablePath('bison');
			return 0 unless ($bisonexe);
			-d "$curDir/gnu" or mkdir ("$curDir/gnu", 0775) or print "cannot create directory $curDir/gnu" and return 0;	
			-d "$curDir/gnu/bin" or mkdir ("$curDir/gnu/bin", 0775) or print "cannot create directory $curDir/gnu/bin" and return 0;	
			symlink("$bisonexe" ,"$curDir/gnu/bin/bison") or
				print "cannot create symbolic link \"$curDir/gnu/bin/bison -> $bisonexe\": $!\n" and return 0;
		}
	}
	return 1;
}

# only for linux necessary
sub linkPosixTools
{
	local @PosixTools = ("bash", "cat", "cp", "diff", "echo",
	                     "egrep", "fgrep", "find", "grep", "gzip", "ln", "ls",
				         "make", "mkdir", "mv", "ps", "rm", "rmdir", "sed",
				         "sh", "sort", "tar", "touch", "uniq", "wc");
	local $PTool, $PPath;
	
	# create Posix directory
	-l "$curDir/Posix" and (unlink "$curDir/Posix" or print "cannot unlink link $curDir/Posix" and return 0 );	
	-d "$curDir/Posix" or ( mkdir ("$curDir/Posix", 0775) or print "cannot create directory $curDir/gnu" and return 0);	

	# link the ptools
	foreach $PTool (@PosixTools)
	{
		$PPath=getExecutablePath($PTool);
		return undef unless ( $PPath );
		unless (-l "$curDir/Posix/$PTool")
		{
			symlink("$PPath", "$curDir/Posix/$PTool") or
				print "cannot create symbolic link \"$curDir/Posix/$PTool -> $PPath\": $!\n" and return 0;
		}
	}
	return 1; # all	ok
}


sub getExecutablePath
{
	local($executable) = @_;
	local $epath=0;

	my $epath = SAPDB::DevTools::Install::which("$executable");
	return $epath if ($epath);
	while (1){
		print "please enter $executable path: ";
		chomp($_=<STDIN>);	
		$_=~s/\\/\//g;
		$epath=$_;
		-f $epath and return $epath;
		-f "$epath/$executable" and return "$epath/$executable";
		print "$executable executable not found\n" ;
		print "abort installation (y/n)? ";
		chomp($_=<STDIN>);
		/^y$/i and return undef;
	}
}


package SAPDB::DevTools::Install::Registry;

$ini_file=$ENV{'HOME'} . ".sdbdev";
$ini_key='software\SAP\DB Tech\Open Source';
%values;

sub writeUX{
	-f "$ini_file" and ( unlink "$ini_file" or  print "cannot unlink file \"$ini_file\"($!)\n" and return 0 );
	open(FD,">$ini_file") or print "cannot create file \"$ini_file\" ($!)\n" and return 0;
        print FD "# registry for MaxDB build tools\n";
	foreach my $key (keys(%values)){
		print FD "$key = $values{$key}\n";
	}
	close(FD);	
}

sub writeWin{
	#use Win32API::Registry 0.13 qw( :ALL );
	require Win32API::Registry;
	import Win32API::Registry qw( :ALL );
	sub createKey{
		# sub create key in registry recursively
		my $hkey = $_[0]; # HKEY_*
		my $key = $_[1];  # keyname
		unless($hkey){
			$hkey=&HKEY_CURRENT_USER;
		}
		$key=~ /^[^\\]*$/ and return 1; # cant create first level key
		my $tmpkey=$key;
		$tmpkey=~s/^(.*)\\[^\\]{1,}$/$1/;
                my $newkey;
                if (RegOpenKeyEx(&HKEY_CURRENT_USER, $key, 0, &KEY_ALL_ACCESS, $newkey )) {
                    RegCloseKey($newkey) or print("cannot close new subkey \"$key\": $^E\n");
                    return 1;
                }
                createKey($hkey,$tmpkey) or return 0;
                RegCreateKeyEx($hkey,$key,0,"",&REG_OPTION_NON_VOLATILE ,&KEY_CREATE_SUB_KEY,0,$newkey,$piDisp) or print("cannot create subkey \"$key\"\n") and return 0;
                RegCloseKey($newkey) or print("cannot close new subkey \"$key\": $^E\n");
		return 1;
	}
	unless(RegOpenKeyEx( &HKEY_CURRENT_USER, $ini_key, 0, &KEY_ALL_ACCESS, $key )){
		createKey( &HKEY_CURRENT_USER, $ini_key) or print "cannot create registry key\n" and return 0;
		RegOpenKeyEx(&HKEY_CURRENT_USER, $ini_key, 0, &KEY_ALL_ACCESS, $key ) or print "cannot open created key: $^E\n" and return 0;
	}
	foreach my $valuename (keys(%values)){
		RegSetValueEx( $key, $valuename, $iZero, &REG_SZ , $values{$valuename}, length($values{$valuename})) or print "cannot set \$TOOL in registry: $^E\n" and return 0;
	}
	RegCloseKey( $key );
    return 1;	
}

*write =        $^O =~ /^mswin/i ? \&writeWin :
	\&writeUX;



package SAPDB::DevTools::Install::IProfile;

$filename = "iprofile";

sub replace{
	my (%replace) = @_;
	my @filebuffer;
	open(FD,$filename) or print "cannot open file \"$filename\"\n" and return 0;
	while(<FD>){
		chomp($_);
		push @filebuffer,$_;
	}
	close(FD);
	open(FD,">$filename.tmp") or print "cannot create file \"$filename.tmp\"\n" and return 0;
	foreach my $line (@filebuffer){
		foreach my $var (keys(%replace)){
			$line =~ s/$var/$replace{$var}/g;
		}
		print FD "$line\n";
	}
	close(FD);
	return 1
}





SAPDB::DevTools::Install::main(@ARGV);

