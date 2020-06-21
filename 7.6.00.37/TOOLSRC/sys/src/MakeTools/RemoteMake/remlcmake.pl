#!/usr/bin/perl
#
# remlcmake.pl Script
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

use Env;

do "$ENV{'TOOL'}\\bin\\remuser_mach$ENV{'TOOLEXT'}";

$action="lcmake.pl";
my $special_hosttype = undef;
my $debug = undef;

while( $_ = $ARGV[ $i ], ( $i<=$maxi)  )
{
	if (/^-\?|-h$/)
	{
		usage(1);
	}
	if (/^-l$/)
	{
		list_hosts(1);
		exit;
	}
	$i++;
}

while ($_ = $ARGV[0], /^[-+]/) 
{	
	shift;
	if (/^-action=(.*)$/) 
	{
	    $action=$1;
	    unless ($action =~ /^(cleanup|xterm|dir|kill|lcprot\.pl)$/) 
	    {
      		print "Error: wrong syntax for -action option\n\n";
      		usage();
      	}
      	next;
    }
	if (/^-\?|-h$/)
	{
		usage();
	}
	if (/^-(.*)$/)
	{
		my $option = $1;
		if ($option =~ /^p/)
		{
			# make to productive make machine
			$special_hosttype = "prod";
		}
		elsif ($option =~ /^w$/)
		{
			$special_hosttype = "web";		
		}
		elsif ($option =~ /^d=(.*)$/)
		{
			$debug = $1;		
		}
		elsif ($option =~ /^v$/)
		{
			$debug = 1;		
		}
		else
		{
			print "Error: unknown option $_\n";
			usage();
		}
		next;
	}
}

if ($ENV{RELSTAT} =~ /^PROD$/i)
{
	if ( -f "$ENV{OWN}/syncinfo.dat")
	{
		open(SYNCINFO, "< $ENV{OWN}/syncinfo.dat") || die "Couldn't open $ENV{OWN}/syncinfo.dat for reading";
		$syncnumber = <SYNCINFO>;
		if ( $syncnumber =~ /<maxchangenr>(.*)<\/maxchangenr>/)
		{
			$ENV{LATEST_CHANGE_LIST}= $1;
		}
		close (SYNCINFO);
		print ("Current changelist is $ENV{LATEST_CHANGE_LIST}\n");
	}
	elsif (-f "$ENV{OWN}/changelist.log")
	{
		open(CHLST, "<$OWN/changelist.log");
		local $changelist=<CHLST>;
		chomp $changelist;
		$ENV{LATEST_CHANGE_LIST} = $changelist;
		close CHLST;
		print ("Current changelist is $ENV{LATEST_CHANGE_LIST}\n");
	}
	elsif ( $ENV{LATEST_CHANGE} )
	{
		$ENV{LATEST_CHANGE_LIST}=$ENV{LATEST_CHANGE};
	}
	else
	{
		print ("Waring: no changlist found \n     set LATEST_CHANGE_LIST to 111\n");
		$ENV{LATEST_CHANGE_LIST}="111";
	}
}
$ENV{LATEST_CHANGE}=$ENV{LATEST_CHANGE_LIST};

sub usage()
{
	my $list = shift;
	print STDERR "usage: remlcmake [-action=cleanup|xterm][<platform>] {<lcmake_parameter>}\n";
    print STDERR ("              64Bit:", $REMUSER_HOST{'64bit'}, "\n");
    print STDERR "     or              H64_hostname to work on a specific 64bit host\n";
    print STDERR "     or              all   to work on all platforms\n";
	print STDERR "<lcmake_parameter> passed to 'lcmake'\n\n";
	print STDERR "This is the remote version of lc_make.\n";
	print STDERR "Example: remlcmake aix ::COMBase/COMBase.dld recompiles kernel on ::COMBase/COMBase.dld platform\n\n";
	print STDERR "Environment: REMUSER_PLATFORM default for <platform>\n";
	print STDERR "             Used only if first parameter given is no valid platform!\n";
	
	list_hosts(0) if ($list);
	die;
}


sub list_hosts
{
	my $display_hosts= shift;
	require Remote::SDBRemInfo;
	import Remote::SDBRemInfo;
	printHostKeyList(\%REMUSER_HOST, \%REMUSER_ALIAS, \%REMUSER_HOSTMODE, $display_hosts);
}


sub getRemuserhost {
	my $Platform=$_;
	my $Release;

	if ( defined $ENV{IGME_VERSION} ) {
		$Release = substr( $ENV{IGME_VERSION}, 0, 3 );
		if ( defined $REMUSER_HOST{"${Platform}_${Release}"} ) {
			print "Using release specific host\n";
			return $REMUSER_HOST{"${Platform}_${Release}"};
		}
	} else {
		if ( defined($ENV{RELVER})
		  && defined($ENV{CORRECTION_LEVEL}) ) {
			$Release = "$ENV{RELVER}";
			$Release =~ s/R(\d)(\d+)/$1.$2/;
			if ( defined $REMUSER_HOST{"${Platform}_${Release}"} ) {
				print "Using release specific host\n";
				return $REMUSER_HOST{"${Platform}_${Release}"};
			}
		}

	}
	return $REMUSER_HOST{$Platform};
}

if ( @ARGV < 1 )
{
	usage();
	exit 1;
}

$platform = $ARGV[0];
shift;

if ( defined $REMUSER_ALIAS{$platform} )
{
	@platforms = split(/ /,$REMUSER_ALIAS{$platform});
}
else
{
	if ($platform =~ /,/)
	{
		my %platform_hash;
		foreach (split(/,/,$platform))
		{
			if ( defined $REMUSER_ALIAS{$_} )
			{
				foreach (split(/ /,$REMUSER_ALIAS{$_}))
				{
					$platform_hash{$_} = 1;
				}
			}
			else
			{
				$platform_hash{$_} = 1;
			}
		}
		@platforms = keys %platform_hash;
	}
	else
	{ @platforms= ( "$platform" ); }
}

@tmpargv=@ARGV;
@commonargv=();
push @commonargv, "-uselocaldev" if ( ! defined $IGNOREOWN );

if ( $action eq "xterm" )
{
    $ENV{REM_PARALLEL} = "Start_Xterm";
    $count = @platforms + 0;
    if ( $count > 1 )
    {
        print "Starting $count xterms, please wait ...\n";
    }
    else
    {
        print "Starting xterm, please wait ...";
    }
}
else
{
    if ( $ENV{"REM_FORCE_PARALLEL_MODE"} eq "ALWAYS" )
    {
        $ENV{REM_PARALLEL} = "Start_Parallel";
    }
    else
    {
        if ( $ENV{"REM_FORCE_PARALLEL_MODE"} eq "NEVER" )
        {
            undef $ENV{REM_PARALLEL};
        }
        else
        {
            if ( (@platforms > 1) && ($action ne "kill") )
            {
                $ENV{REM_PARALLEL} = "Start_Parallel";
            }
            else
            {
                undef $ENV{REM_PARALLEL};
            }
        }
    }
}
if ( $ENV{APOMAKE})
{
	%OS_to_Platform = (
	dec       =>  'alphaosf',
	hp64      =>  'hp_64',
	aix64     =>  'rs6000_64',
	aix5L64   =>  'rs6000_51_64',
    sun64     =>  'sun_64',
    ia64      =>  'NTia64',
    amd64     =>  'NTx86_64',
    hpia64    =>  'hpia64',
    ntia64    =>  'NTia64',
    win32     =>  'NTintel',
    linux     =>  'linuxia64',
    linuxia64 =>  'linuxia64',
    linuxia64_9 =>  'linuxia64sles9',
    linuxia64_icc =>  'linuxia64',
    linuxia64_icc9 =>  'linuxia64sles9',
    linuxx86_64 =>  'linuxx86_64',
    linuxx64_gcc4 =>  'linuxx86_64',
    linuxx64_icc =>  'linuxx86_64',
    linuxppc64 =>  'linuxppc64',
    sunx86_64 =>  'sunx86_64'
    );

    
   	%Maschine_to_Platform = (
	d    =>  'alphaosf',
	h    =>  'hp_64',
	i    =>  'rs6000_64',
    u    =>  'sun_64',
    l    =>  'linuxia64',
    );

}
	
	

foreach (@platforms) {

	local $platform = $_;
	undef $mapped_platform;
	
	print "Using platform $_\n";
	
	
		
	$host = getRemuserhost( "$_" );
	
	if ( defined $host )
	{
		$mapped_platform = $OS_to_Platform{$platform};
	}
	else
	{
		if ( /^H32_(\w+)/ )
		{
			$host = $1;
			undef $REMUSER_BIT64MODE{$_};
		}
		else
		{
			if ( /^H64_(\w+)/ )
			{
				$REMUSER_BIT64MODE{$_} = "BIT64";
				$host = $1;
				if ($ENV{APOMAKE})
				{
					if ($host =~ /^(.*)\((.*)\)/)
					{
						$host = $1;
						$mapped_platform = $2;
						if (defined $REMUSER_ALIAS{$mapped_platform})
						{ $mapped_platform = $OS_to_Platform{$REMUSER_ALIAS{$mapped_platform}};	}
					}		
					else
					{
						$mapped_platform = $Maschine_to_Platform{substr($host,0,1)};	
						# aix 4.3 <-> 5.1 for APO 3.10
						if ( ($ENV{APO_COM_SHORT} lt "4000") &&  ( $mapped_platform eq $OS_to_Platform{aix64} ) )
						{
							print STDERR "Error: missing platform define for aix machines for APO < 4.0\n";
							
							print STRERR "       Please use H64_$host(aix) or H64_$host(aix5) !\n";
							die;
						}
						
					}
				}
			}
			elsif ( /^(\w*)\[(\w*)\]$/ )
			{
				$host = $2;
				$mapped_platform = $1;
			}
		}
	}
	print STDERR "Error: Can't map to right platform !\n" unless ( defined $mapped_platform);
	die unless ( defined $mapped_platform);

	if ( ! defined $host )
	{
		print STDERR "no host for platform ${platform} given\n";
		usage();
	}

	@ARGV=@commonargv;

	if ( defined $REMUSER_BIT64MODE{$_} || ($mapped_platform =~ /64$/))
	{
		push @ARGV, "-bit64";
	}
	
	push @ARGV, "-platform", $mapped_platform if ($ENV{APOMAKE});
	push @ARGV, "-hosttype", $special_hosttype if (defined $special_hosttype);
	push @ARGV, "-debug", $debug if (defined $debug);
	push @ARGV, "-action", $action;
	push @ARGV, "-host", $host, "-e", $action; 
	
	
	push @ARGV, @tmpargv;
	

	if ( defined $DEBUGREMIM )
	{
		print "$ENV{'TOOL'}\\bin\\rem_vmake$ENV{'TOOLEXT'}";
	    foreach ( @ARGV ) { print " $_"; }
		print "\n";
	}
	else
	{
		do "$ENV{'TOOL'}\\bin\\rem_vmake$ENV{'TOOLEXT'}";
		
	}
}
