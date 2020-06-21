#!/devtool/local/bin/perl
# comppers.pl
use Env;
use File::Basename;

do "$TOOLVARS";
if ( $@ ) { print "$@"; exit }

ToolTrace("comppers.pl called with @ARGV\n");
my %PERSINFO;

# options
while ($_ = $ARGV[0], /^[-+]/) 
{
    shift;
}

my $SOURCE=$ARGV[0];
my ($BASE, $SOURCEPATH, $EXT) = fileparse($SOURCE, '\..*');
my $TARGET="$BASE.pers";
my $lc_state= defined $ENV{'LC_STATE'} ? $ENV{'LC_STATE'} : "DEV";

open(VERSIONFILE_IN, "<$SOURCE") 
	or die ("can't open VERSIONS file $SOURCE for reading\n");
open(VERSIONFILE_OUT, ">$TARGET") 
	or die ("can't open VERSIONS file $TARGET for writing\n");

my $line = 1;
while ( <VERSIONFILE_IN>) 
{
	$line++;
	chomp $_;
	if (/^\s*(#.*)$/) # found comment
	{
		print VERSIONFILE_OUT "$1\n";
		next;
	}
	if (/^\s*(\S+)\s*:\s*(\S+)\s*=\s*(\S+)\s*/)
	{
		my $key = $2;
		my $value = $3;
		die "wrong LC_STATE in $SOURCE in line $line \n"
			unless ( $1 =~ /^$lc_state$/i );
		if ( $value =~ /^(\d)\.(\d)\.(\d\d?)\.(\d\d?)$/ )
		{
			my $corlevel = $3;
			my $build = $4;
			$value = "$1.$2.";
			$value .= "0" unless ( $corlevel =~ /^\d\d$/ );
			$value .= "$corlevel.";
			$value .= "0" unless ( $build =~ /^\d\d$/ );
			$value .= "$build";
			$PERSINFO{$key}=$value;
			print VERSIONFILE_OUT "$key=$PERSINFO{$key}\n";
		}
		else
		{	die "wrong version string in $SOURCE line $line\n($_)\n";	}
	}
	else
	{	die "wrong syntax in $SOURCE line $line\n($_)\n";	}
}
close (VERSIONFILE_IN);
close (VERSIONFILE_OUT);

if ( defined $ENV{'NOQUIET'} )
{
	if (defined %PERSINFO)
	{
		print "following persinfos found:\n";
		foreach (keys %PERSINFO)
		{
			print "$_=$PERSINFO{$_}\n";
		}
	}
	else
	{
		print "no persinfos found\n";
	}
}
