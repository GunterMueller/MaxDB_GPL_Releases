#!/usr/bin/perl
#
#    CheckOptFlags.pl - caller for optimization data.
#
#    @(#)CheckOptFlags.pl     2005-05-24
#
#    &gar, SAP AG
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

use Getopt::Long;
use SDB::Make::OptimizationInfo;
use strict;
use FileHandle;
use Cwd;

my $progname = $0;
$progname=~ s,(.*/)|(.*\\),,;

my %Opts;
my $config = undef; 
my $path_sep = ($^O=~/.*win32.*/i) ? "\\" : "/";

sub Usage
{
	print <<USAGE_END;
    
Usage:
  $progname [-config (fast|quick|slow)] [-groupby (target|platform)]
                   [-platform <platform|alias>[,<platform|alias>]] 
                   [-xml] [-outfile <filename>] [-silent] [target]
                   [-verbose] [-(h|help)]                   
                                       
Where:
     -config <version> or -c <version>
         default version is fast
     
     -groupby (target|platform) or -g (target|platform)
         output will be grouped by given parameter (default is platform)
     
     -platform <platformlist> or -p <platformlist>
         platformlist is a comma separated list of platforms and 
         platorm aliases (like remimf)
         (Only under windows available)
     
     -xml or -x
         write output in xml file 
     
     -outfile <filename> or -o <filename> 
         specify filename for xml output 
         (default is \$TMP/OptFlags_<target>_<platform>_<groupby>.xml)
     
     -silent or -s
         Suppress start of viewer for generated xml file.
     
     target
         default is all.mac
USAGE_END


}     


if (!GetOptions( \%Opts,
                'config=s',
                'groupby|g=s',
                'xml|x',
                'outfile|o=s',
                'platform|p=s',
                'silent|s',
                'verbose|v',
                'h|help') || $Opts{'h'})
{
    Usage();
    exit(0);
}
my $fxml = undef;
my $target = shift;
defined $target or $target = "all.mac"; 
my %COF_Options = ();
my %OptHash = ();
my $filename = $Opts{'outfile'};

my $groupby = "platform";
my @platforms_to_check = ("local");
if (defined $Opts{'platform'})
{
	if ($^O!~/.*win32.*/i)
	{
		print "Error: option -platform only under WIN32 allowed\n";
		Ussage();
		exit; 
	}
	@platforms_to_check =  split ",", $Opts{'platform'};
}
 
(defined $filename) or 
		$filename = "$ENV{TMP}/OptFlags_${target}_". (join "_", @platforms_to_check) . "_$groupby.xml";

if (defined $Opts{'platform'})
{	substitute_platforms (\@platforms_to_check);}

$COF_Options{'config'} = (defined $Opts{'config'}) ? $Opts{'config'} : "fast";

if (defined $Opts{'groupby'})
{
	if ($Opts{'groupby'} !~ /^platform|target$/)
	{
		Usage();
		exit (-1)
	} 
	else
	{ $groupby = $Opts{'groupby'}; }
}

if ( defined $Opts{'xml'})
{
	 $fxml = open_xml_file($target, $groupby, \@platforms_to_check, $filename );
}
(scalar(@platforms_to_check) > 1) and 
	print("Run check on following platforms: ". (join ",",  @platforms_to_check ) . "\n"); 

foreach my $platform (@platforms_to_check)
{
	print ("Looking for modified optimization " . (($platform =~  /^local$/) ? "(local) ...\n" : "on $platform ...\n"));
	$COF_Options{'platform'} = $platform;
	my $count;
	my $TargetOptHash;
	($count, $TargetOptHash) = get_modified_optimization($target, \%COF_Options);
	if ($count > 0)
	{
		if (($groupby =~ /platform/) && defined $Opts{'xml'})
		{
			$fxml->print("<os Platform=\"$platform\">\n");
		}
		foreach (keys %$TargetOptHash)
		{	
			if ($groupby =~ /target/)
			{ $OptHash{$_}->{$platform}=$TargetOptHash->{$_};	}
			elsif (defined $Opts{'xml'})
			{
				$fxml->print("<trg Target=\"$_\">\n");
				$fxml->print("<Options>$TargetOptHash->{$_}</Options>\n");
				$fxml->print("</trg>\n");
				
			}
			else
			{	print "$_ :$TargetOptHash->{$_}\n"; }
		}
		if (($groupby =~ /platform/) && defined $Opts{'xml'})
		{	 $fxml->print("</os>\n"); }
	}
		
	if ($count < 0)
	{   print ("\nERROR while collecting!\n"); }
	elsif ($count == 0)
	{	print ("No \"manipulated\" targets found\n"); }
	elsif ($count == 1)
	{ print ("1 \"manipulated\" target found\n"); }
	else
	{ print ("$count \"manipulated\" targets found\n"); }
}

if ($groupby =~ /target/)
{
	foreach my $target (sort keys %OptHash)
	{
		if ( defined $Opts{'xml'}) 
		{ $fxml->print("<trg Target=\"$target\">\n");}
		else
		{		print "$target\n"; }
		foreach (sort keys %{$OptHash{$target}})
		{	
			if ( defined $Opts{'xml'})
			{ 
				$fxml->print("<os Platform=\"$_\">\n");
				$fxml->print("<Options>$OptHash{$target}->{$_}</Options>\n");
				$fxml->print("</os>\n");
			}
			else
			{	print "    $_: $OptHash{$target}->{$_}\n"; }
		}
		( defined $Opts{'xml'}) and $fxml->print("</trg>\n");
	}
}

if ( defined $Opts{'xml'}) 
{
	close_xml_file($fxml);
	check_for_absolute_path (\$filename);
	if (! $Opts{'silent'}) 
	{ 
		start_ie ($filename);
	}
	else
	{ print "XML file: $filename\n"; 	}
}

sub substitute_platforms
{
	my %tmphash = ();
	my $platforms_to_check = shift;
	do "$ENV{'TOOL'}\\bin\\remuser_mach$ENV{'TOOLEXT'}";
	my %REMUSER_ALIAS = get_REMUSER_ALIAS();
	my %REMUSER_HOST = get_REMUSER_HOST();
	$REMUSER_ALIAS{'all'} .= ' local';
	
	foreach (@$platforms_to_check)
	{
		if ( defined $REMUSER_ALIAS{$_} )
		{
			my @tmpplatforms = split(/ /,$REMUSER_ALIAS{$_});
			foreach (@tmpplatforms)
			{	$tmphash{$_} = 1; }
		}
		else
		{	$tmphash{$_} = 1;	}
	}
	@$platforms_to_check = sort keys %tmphash;
}


sub open_xml_file
{
	my $target = shift;
	my $groupby = shift;
	my $lplatforms = shift;
	my $filename = shift;
	
	my $fxml =  new FileHandle;
	if ( ! $fxml->open($filename, "w") )
	{	die "Can't open $filename (output) :$!\n";	}
	$fxml->print("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<OptimizerOptions>\n");
	$fxml->print("<Metadata target=\"${target}\" groupby=\"${groupby}\" platforms=\"".(join ",", @$lplatforms). "\" />\n") ;
	return 	($fxml);
}

sub close_xml_file
{
	$fxml->print("</OptimizerOptions>\n");
	$fxml->close();
}

sub start_ie
{
	my $filename = shift;
	if ($^O=~/.*win32.*/i)
	{
		require OLE;
		import OLE;
		print STDERR "Start Internet Explorer with \"$filename\" ...";
		my $explorer = CreateObject OLE 'InternetExplorer.Application'
						or die "Couldn't create new instance of Explorer App!!";
		$explorer->Navigate("$filename");
		$explorer->{'Visible'} = 1;
		$explorer->{'Active'}  = 1;
		# theatermode to switch to foreground
		$explorer->{'TheaterMode'}  = 1;
		$explorer->{'TheaterMode'}  = 0;
	}
	else
	{
		system ("cat $filename");
	}
}

sub check_for_absolute_path
{
	my $ref_path = shift;
	if ($^O=~/.*win32.*/i)
	{
		($$ref_path =~ /^(\\|.*:)/) and return;
	}
	else
	{
		($$ref_path =~ /^(\/|.*:)/) and return;
	}
	$$ref_path = cwd() . "$path_sep$$ref_path";
	if ($^O=~/.*win32.*/i)
	{ $$ref_path  =~ tr/\//\\/;}
}

__END__ 

=head1 NAME

CheckOptFlags - a script to show optimizer options set by descriptions

=head1 ABSTRACT

This script shows the modified optimizer options in the descriptions set by following options:

=over 4

 -OPT=<options>
 -OPTSUB=<options>
 -OPTADD=<options>
 -NO

=back

It supports remote checks on other platforms too (only under windows).

It is possible to generate all data to xml formated file which can be viewed by IE.

=head1 SYNOPSIS

 CheckOptFlags [-(config|c) (fast|quick|slow)] [-(groupby|g) (target|platform)]
               [-(platform|p) <platform|alias>[,<platform|alias>]] 
               [-(xml|x)] [-(outfile|o) <filename>] [-(show|s)] [<target>]
               [-(verbose|v)] [-(h|help)]           

=head1 OPTIONS

=over 3

=item B<-(c|config) <version>>

Set vmake version (fast|quick|slow) - default version is fast.

=item B<-(g|groupby) (target|platform)> 

Output will be grouped by given parameter (default is platform).

=item B<-(p|platform) <platformlist>> 

<platformlist> is a comma separated list of platforms and platorm aliases (like remimf).
if the platform alias all is used the local machine will by added to the list automatically.
This option is only under windows available. For the platform name "local" the local machine will be used.

=item B<-(o|outfile) <filename>> 

Specify filename for xml output (default is $TMP/OptFlags_<target>_<platform>_<groupby>.xml). 

=item B<-(x|xml)>

Write output in xml formate to a file and show it with the IE (Only under Windows).

=item B<-(s|silent)>

Suppress start of viewer for generated xml file.

=item B<-<target>>    

Name of the target to check for optimizer options (default is all.mac).

=back

=head1 EXAMPLES

Here you can find some usage examples for CheckOptFlags. 

B<First simple Scenario: I want to check optimizer flags for a special target for one platform:>

Execute on the host (locally)

  C:\>CheckOptFlags.pl ak1lib.mac 
 
  Looking for modified optimization (local) ...
  ak/vak17 :-NO
  ak/vak16 :-NO
  ak/vak11 :-NO
  3 "manipulated" targets found

or execute remote from local development machine under windows

  C:\>CheckOptFlags.pl -platform sun ak2lib
  
  Looking for modified optimization on sun64 ...
  ak/vak37 :-NO
  ak/vak42 :-NO
  ak/vak22 :-NO
  3 "manipulated" targets found

B<Scenario: I want to check optimizer flags for a special target on a list of some platforms>

  C:\>CheckOptFlags.pl -platform sun64,hp64,aix ak2lib
  Run check on following platforms: aix64,hp64,sun64
  Looking for modified optimization on aix64 ...
  ak/vak42 :-NO
  1 "manipulated" target found
  Looking for modified optimization on hp64 ...
  no "manipulated" targets found
  Looking for modified optimization on sun64 ...
  ak/vak37 :-NO
  ak/vak42 :-NO
  ak/vak22 :-NO
  3 "manipulated" targets found

B<Scenario: I want output the data of the previous scenario grouped by modules>

  C:\>CheckOptFlags.pl -platform sun64,hp64,aix,local -groupby target ak2lib
  Run check on following platforms: aix64,hp64,local,sun64
  Looking for modified optimization on aix64 ...
  1 "manipulated" target found
  Looking for modified optimization on hp64 ...
  No "manipulated" targets found
  Looking for modified optimization (local) ...
  No "manipulated" targets found
  Looking for modified optimization on sun64 ...
  3 "manipulated" targets found
  ak/vak22
      sun64: -NO
  ak/vak37
      sun64: -NO
  ak/vak42
      aix64: -NO
      sun64: -NO

B<Scenario: I want to check optimizer flags for a special target on all platforms>

  C:\>CheckOptFlags.pl -platform all -groupby target bdlib
  Run check on following platforms: aix64,dec,hp64,hpia64,ia64,linux,linux_9,linuxia64_9,linuxx86_64,local,ppc64,sun64
  Looking for modified optimization on aix64 ...
  No "manipulated" targets found
  Looking for modified optimization on dec ...
  3 "manipulated" targets found
  Looking for modified optimization on hp64 ...
  No "manipulated" targets found
  Looking for modified optimization on hpia64 ...
  1 "manipulated" target found
  Looking for modified optimization on ia64 ...
  No "manipulated" targets found
  Looking for modified optimization on linux ...
  2 "manipulated" targets found
  Looking for modified optimization on linux_9 ...
  2 "manipulated" targets found
  Looking for modified optimization on linuxia64_9 ...
  No "manipulated" targets found
  Looking for modified optimization on linuxx86_64 ...
  No "manipulated" targets found
  Looking for modified optimization (local) ...
  No "manipulated" targets found
  Looking for modified optimization on ppc64 ...
  6 "manipulated" targets found
  Looking for modified optimization on sun64 ...
  No "manipulated" targets found
  bd/vbd02
      dec: -OPT=-O1
  bd/vbd13.cpp
      ppc64: -OPT=-O1
  bd/vbd20
      dec: -OPT=-O1
      ppc64: -OPT=-O1
  bd/vbd30
      dec: -OPT=-O1
      ppc64: -OPT=-O0
  bd/vbd405.cpp
      ppc64: -OPT=-O1
  bd/vbd495.cpp
      linux: -OPT=-O2
      linux_9: -OPT=-O2
  bd/vbd50
      ppc64: -OPT=-O0
  bd/vbd52
      ppc64: -OPT=-O0
  bd/vbd92.cpp
      hpia64: -OPT=+O0
  bd/vbd93.cpp
      linux: -OPT=-O3
      linux_9: -OPT=-O3  

B<Scenario: I want to view modified optimizer flags for a special target on all platforms in XML format>

  C:\CheckOptFlags.pl -platform all -groupby target bdlib -xml
  ...

=head1 COPYRIGHT

Copyright 2005 SAP AG