#!/devtool/local/bin/perl
use Getopt::Long;
use strict;
use lcmake;
use IO::DelTree;
use File::Copy;
use File::Path;

my $progname = $0;
$progname=~ s,(.*/)|(.*\\),,;
my %Opts;

if (!GetOptions( \%Opts,
                's|slow',
                'q|quick',
                'o|output:s',
                'c|checkheader',
                'd|directory:s',                
                'm|move',
                'n|nodep',
                'h|help'
                ) || $Opts{'h'})
{
		print <<USAGE_END; 
Usage: $progname [-s|slow] [-q|quick] [-c|checkheader] [-n|nodep] [-d <dir>]
                  [-m|move] [-o|output <file>] <target> [<target> ...]
            
USAGE_END
    exit(0);
}

my @TARGETS = @ARGV;

if ($Opts{'q'} && $Opts{'s'})
{
	print "You are too stupidly to work with this tool!\n";
	print "What's wrong with your options ?\n";
	exit (0);
}

my $config = $Opts{'q'} ? "quick" : ( $Opts{'s'} ? "slow" : "fast" );
my $list_cmd = "$ENV{TOOLSHELL} -S lcmake.pl -c --config=$config ";
my $error_messages = "";
my $error_count = 0;


if ($Opts{'o'})
{
	if ( ! open (ERRORFILE, ">$Opts{'o'}")) 
	{	die "can't open protfile $Opts{'o'}\n";	}
}
    		
    
my %all_errors;
my %tested;

foreach my $target (@TARGETS)
{
	my @filelist = ();
	if ($Opts{'n'})
	{
		push @filelist, $target;
	}
	else
	{
		my $line = undef;
		print "Collect module list of $target ...";
		open(LIST,"$list_cmd $target 2>&1 |");
	    while(<LIST>)
	    {
	    	chomp;	
	    	if (/^module/)
	    	{
	    		my %module_info = ();
	    		
	    		while (<LIST>)
	    		{
	    			(/^\s+callers/) && last;
	    			(/^\s+(name|layer|version)\s+\'(.*)\'/) and $module_info{$1}=$2;
	    		}
	    		push @filelist, \%module_info;
	    	}
	    }
	    print "OK\n"; 
	}
	
    my $allcount = scalar(@filelist);
    print "Found $allcount modules\n";
    my $count = 0; 
    my $rc = 0;
        
    
    foreach my $file (@filelist)
    {
       	if (defined $tested{$file->{'layer'}}->{$file->{'name'}}->{$file->{'version'}})
    	{
    		print "Info: $file->{'layer'}/$file->{'name'}($file->{'version'}) already tested by another target - ignore it\n";
    	}
    	else
    	{
    		$tested{$file->{'layer'}}->{$file->{'name'}}->{$file->{'version'}} = 0;
        	$count++;
        	print ("Clear $ENV{INSTROOT}/incl directory ...\n");
	    	DelTree("$ENV{INSTROOT}".(($^O=~/win32/i) ? "\\" : "/")."incl");
	    	print ("Clear wrk directory ...\n");
	    	DelTree("$ENV{'WRK'}");
	    	print "Build $file->{'layer'}/$file->{'name'}($file->{'version'}) ($count/$allcount of $target):\n";
	    
	    	my $file_path = (defined $file->{'layer'}) ? ":$file->{'layer'}/" : "";
	    	my $config = ($file->{'version'} =~ /f/) ? 'fast' : (($file->{'version'} =~ /q/) ? 'quick' : 'slow');
	    	$file_path .= $file->{'name'};
	    	(defined $Opts{'c'}) and ($file =~ /\.hp{0,2}$/) and $file_path .= "//compilecheck";
	    	$rc = lcmake::lcmake ("lcmake.pl", "--config=$config", "-i", $file_path);
	    	$tested{$file->{'layer'}}->{$file->{'name'}}->{$file->{'version'}} = 1;
	    	if ($rc != 0)
	    	{
	    		$all_errors{"$file->{'layer'}/$file->{'name'}($file->{'version'})"} = $rc;
	    		$tested{$file->{'layer'}}->{$file->{'name'}}->{$file->{'version'}} = -1;
	    		$error_count++;
	    		print "Error in make !\n";
	    		my $error_text = "";
	    		my ($protfile) = ($file_path =~ /^:(.*)$/);
	    			    		
	    		unless ( -f "$ENV{OWN}/sys/wrk/$config/prot/$protfile.e0" )
	    		{
	    			$protfile =~ /\/([^\/]*)$/;
	    			$protfile = $1;
	    		}
	    		
	    		if ( open (PROTFILE, "$ENV{OWN}/sys/wrk/$config/prot/$protfile.e0")) 
	    		{
	    			$error_text .= "\n". ("*" x 70 ). "\n";	
		    		$error_text .= "\MAKEERROR in $file->{'layer'}/$file->{'name'} ($file->{'version'}) -> \n";
		    		$error_text .= "\n". ("*" x 70 ). "\n";	
					while(<PROTFILE>) { $error_text .= $_; }
					$error_text .= "\n". ("+" x 70 ). "\n";	
				}
			    close PROTFILE;
			    if ($Opts{'o'})
			    {
			    	print ERRORFILE "$error_text\n";
			    }
			    if ($Opts{'d'})
	    		{ 
	    		    	my $cp_file = $file_path;
	    		    	$cp_file =~ s/\://g; 
			    	(-d "$Opts{'d'}/$cp_file") or File::Path::mkpath("$Opts{'d'}/$config/$cp_file", 0, 0777); 
			    	File::Copy::copy("$ENV{OWN}/sys/wrk/$config/prot/$protfile.e0", "$Opts{'d'}/$config/$cp_file");
			    	File::Copy::copy("$ENV{OWN}/sys/wrk/$config/prot/$protfile.p0", "$Opts{'d'}/$config/$cp_file");
			    	File::Copy::copy("$ENV{OWN}/sys/wrk/$config/prot/$protfile.x0", "$Opts{'d'}/$config/$cp_file");
  			    }
			}
		}   
    }    
}
if ($Opts{'o'})
{
	close ERRORFILE;
}

if ($error_count)
{
	if (scalar (keys %all_errors))
	{
		print "Errors occured in: \n";
		foreach (keys %all_errors)
		{
			print " $_\n";
		}
	}
}
else
{ print "No errors found\n" } ;
	

