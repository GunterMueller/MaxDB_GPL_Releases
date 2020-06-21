package createLC_OK;

use Cwd;

undef $platform;
undef $bit;
undef $changelist;
undef $source;
undef $lc_pool_path;

$USAGE="usage: createLCPoolLink -platform <platform> [-path <path>] [-cl <changelist>] <target> [<source>]\n if <source> not defined LastBuild is used";

while ($_ = $ARGV[0], /^[-]/) 
{
    shift;
	if (/^-bit/)
	{
		$bit = shift;
		next;
	}
	if (/^-platform/)
	{
		$platform = shift;
		next;
	}
	if (/^-cl/)
	{
		$changelist = shift;
		next;
	}
	if (/^-path/)
	{
		$lc_pool_path = shift;
		next;
	}
	die "$USAGE";
}

$target = shift;
die ("Error: <target> not defined\n$USAGE\n") unless (defined $target);

$source = shift;
die ("Error: platforn not defined\n$USAGE\n") unless (( defined $platform ) || ($^O =~ /mswin/i));

$source = "LastBuild" unless ( defined $source );

unless (defined $lc_pool_path)
{
	if($^O =~ /mswin/i)
	{	$lc_pool_path = "\\pwdfm017\LC_POOL\LC_7403_".((defined $bit) ? "$bit": "32")."_DEV"; }
	else
	{   $lc_pool_path = "/bas/SAP_DB/7403/pgk/platform/LC_7403_64_DEV"; }
}

# if link found substitute it to number
if ( $source !~ /\d\d\d/ )
{
	if($^O =~ /mswin/i)
	{
		open(LB, "< $lc_pool_path/$source") || die "Couldn't open $lc_pool_path/$source for reading";
		$source=<LB>;
		chomp $source;
		close(LB);
	}
	else
	{
		$source = readlink "$lc_pool_path/$source";
		if ( $source =~ /(\d\d\d)$/ )
		{	$source = $1;	} 
		else
		{   die "Could not substitute link \n"; }
	}
}

if ( -d "$lc_pool_path/$source" )
{
	if ( defined $changelist )		
	{
		open(CL, "< $lc_pool_path/$source/changelist.log") || die "Couldn't open $lc_pool_path/$source/changelist.log for reading";
		$read_changelist = <CL>;
		chomp $read_changelist;
		close (CL);
		die "Different changelists ( $changelist <-> $read_changelist )\n" if ( $changelist != $read_changelist);
	}
}
else
{
	die "$lc_pool_path/$source is not a directory\n"
}

if($^O =~ /mswin/i)
{
	open(LB, "> $lc_pool_path/$target") || die "Couldn't open $lc_pool_path/$target for writing";
	print LB "$source";
	close (LB);
}
else
{
	my $cwd = getcwd();
	chdir($lc_pool_path) or die("cannot change into directory $lc_pool_path: $!\n");
	if( -e $target )
	{
		unlink($target) or die("cannot remove symlink $lc_pool_path/$target: $!\n"); 
	}
	symlink($source,$target) or die("cannot create link \"$target\" to $source\n"); 
	chdir($cwd) or die("cannot change into directory $cwd: $!\n");
}	
