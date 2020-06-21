#!/usr/bin/perl5
package createProfile;

undef $appendfile;
local @args=();

while ($_ = $ARGV[0]) 
{
	shift;
	if (/^-append/)
	{
		$appendfile = shift;
		next;
	}
	push @args, $_;
}

push @args, "-bit", "64" if (($ENV{CPU} =~ /^IA64$/i) || ($ENV{BIT64} eq "1"));
@ARGV=@args;

if ($ENV{BRANCH_INFO})
{
	$release=$ENV{BRANCH_INFO};
	$release=~ s/\.//;
	push @ARGV, "-release", "$release";
}

do "vmakeEnv.pl";
@profile_args="-all";
if (defined $appendfile)
{
	push @profile_args, "-append", $appendfile;
}
vmakeEnv::createProfile(@profile_args);

