#!/usr/bin/perl
# imake           [vmake_options] targets
#
# @(#)imake.pm  1999-07-28
#
# make targets
#
# OWN           root directory for complete release
# TOOL          root directory for tools
# WRK           root directory for production
# DBROOT        root directory for distributable files
# PCFLAGS       optional pascal compiler flags
# CCFLAGS       optional c compiler flags
# ASFLAGS       optional assembler flags
# LDFLAGS       optional linker flags
# KEEPFLG       optional keep intermediate files flag
# VMAKE_OPTION  optional make options
# VMAKE_PATH    search path for root directories
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


#
# cba 1999/07/27 changes for wrk32- and wrk64-directories -> ADIS 1103467
# cba 2000/04/04 changes for wrk32- and wrk64-directories using $WRK_NAME
# cba 2000/11/06 -> PTS 1108201 using $^O for osname,
#                because we cannot use toolvars for running toolvars
#
package imake;

use Env;
use Getopt::Long;
use Exporter;
use FileHandle;
# use ICopy;

@ISA = ('Exporter');
@EXPORT = ('imake', 'imf', 'imq', 'ims');

push @EXPORT, ('imf32', 'imf64', 'imq32', 'imq64', 'ims32', 'ims64');

sub imf32 {
	unshift @_, "-32";
	return imf (@_);
}

sub imf64 {
	unshift @_, "-64";
	return imf (@_);
}

sub imq32 {
	unshift @_, "-32";
	return imq (@_);
}

sub imq64 {
	unshift @_, "-64";
	return imq (@_);
}

sub ims32 {
	unshift @_, "-32";
	return ims (@_);
}

sub ims64 {
	unshift @_, "-64";
	return ims (@_);
}

sub imf {
	splice @_, $_[0] eq "-t" ? 1 : 0 , 0, ( "-f", "--" );
	return imake (@_);
}

sub imq {
	$ENV{'DIAGNOSE'} = "DIAGNOSE";
	splice @_, $_[0] eq "-t" ? 1 : 0 , 0, ( "-q", "--" );
	return imake (@_);
}

sub ims {
	$ENV{'DIAGNOSE'} = "DIAGNOSE-RANGECHECK";
        splice @_, $_[0] eq "-t" ? 1 : 0 , 0, ( "-s", "--" );
	return imake (@_);
}

sub imake
{
   my %ENV_SAV = %ENV ;
   my $exeption = undef ;
   my $rc ;

   eval
   {
      $rc = unsafe_imake (@_) ;
   }; $exeption = $@ if $@ ;

   %ENV = %ENV_SAV ;

   die $exeption if ( defined $exeption ) ;

   return ( $rc ) ;
}


sub unsafe_imake {


	# temporary for check SAPDB_INTERNAL (CTS 1109249)
	# die "SAPDB_INTERNAL not set\n" if ( ! $ENV{SAPDB_INTERNAL});

    my @SavedArgv = @ARGV;
    @ARGV = @_;
    my $Target = "@ARGV";
	my $rc = 0;
	my $useOldBuildNr = 0;
    %optctl = ();
    GetOptions (\%optctl, "f", "q", "s", "t");

    $USAGE = "usage : imake -{f|q|s} [-t] -- [vmake_options] target \n";

    if ($optctl{f}) {
        # fast
        $ENV{'VMAKE_VERSION'} = "fast";
        $ENV{'VMAKE_DEFAULT'} = "fast";
    } elsif ($optctl{q}) {
        # quick
        $ENV{'VMAKE_VERSION'} = "quick";
        $ENV{'VMAKE_DEFAULT'} = "quick";
    } elsif ($optctl{s}) {
        # slow
        $ENV{'VMAKE_VERSION'} = "slow";
        $ENV{'VMAKE_DEFAULT'} = "quick";
    }

    die $USAGE
        unless (defined($ENV{'VMAKE_VERSION'}) && @ARGV > 0);

    # warn "\ncaution: usage of local profile!\n\n"
    #    if ! defined ($GLOBAL_PROFILE);

	$compilationmode = undef;
    ($compilationmode, @ARGV) = setup_compilation_mode (@ARGV);

	$ENV{SHELL}="/bin/sh" if ($^O eq "linux");


	#PTS 1105673
	if ($ENV{WRK_NAME})
	{
		$ENV{WRK}="$OWN/sys/$ENV{WRK_NAME}";
	}

	# use TOOLOUT for make tests of tools
	# if not set use $TOOL
	if (! $ENV{TOOLOUT})
	{
		$ENV{TOOLOUT}=$ENV{TOOL};
	}

    my @VMakeArgs = @ARGV;


	$ENV{'BUILDPID'} = "PID".$$;
    my $BuildNr = 0;
    my ($IsSource, $FirstTarget, $Options, $LastTargetPart);
    ($FirstTarget) = $Target =~ /\s+([\w.\/]+(-[\w.\/]+)?)/;
    if ($FirstTarget) {
        $Options = $`;
        if ($Options =~ /c|n|m/) {
            $noBuildNr = 1;
        }
        $LastTargetPart = $';

        $IsSingleModule =
        ($FirstTarget =~ m!^[vhgi]([a-z][a-z]+)\d\d+-?.*!i  ||
        $FirstTarget =~ m!\.(cpp|c|p|rc|ico|bmp|h|hpp|idl)! ) &&
        ($LastTargetPart =~ /^\s*$/) ? 1 : 0;

        if (!$IsSingleModule &&
        $FirstTarget =~ /^(sut|mut|.+\.(cmd|vdnts))$/i) {
            $noBuildNr = 1;
        }
    } else {
        $IsSingleModule = 1;
    }

	$ModuleFilename="";
	#no BuildNumber necessary if create filelist only (-F)
	foreach $VArg (@VMakeArgs)
	{
		if ( $VArg =~ /^-\w*F/ )
		{
			$noBuildNr = 1;
		}
	}

	# CTS SAPDBEnv
	#$rc = setDevEnv();
	system("chmod -R a+x $WRK/incl 2>/dev/null > /dev/null") if (-d "$WRK/incl"); # only for bugfix
	if ($ENV{RELVER} ne "TOOL" && $ENV{RELVER} ne "R61" && !$IsSingleModule && ! $ENV{APOMAKE} &&
    ( $VMakeArgs[0] !~ /gsp100\.h/ ))
	{
		#CTS 1109250
		if ($ENV{RELVER} ge "R72" && $ENV{RELVER} lt "R80" && $ENV{RELVER} ne "TOOL" && $ENV{RELVER} ne "RTOOL" &&
		     ! (($ENV{RELSTAT} =~ /TEST/i) &&  (-f "$ENV{WRK}/incl/gsp100.h")) )
		{

			local @gsp100_make = ();
			if ($ENV{'VMAKE_VERSION'} =~ /^(f|q|s)/)
			{ push @gsp100_make, "-$1"; }
			else
			{ die "unknown VMAKE_VERSION\n"; }
			push @gsp100_make, "--", "-$compilationmode" if (defined $compilationmode);
			push @gsp100_make, "gsp100.h";

			$gsp100_made=0;
			$checkedBuild=0;

			unless ( ( $VMakeArgs[0] =~ /gsp100\.h/ ) )
			{
				print ("Create gsp100.h first:\n");
				$rc = unsafe_imake (@gsp100_make);
				if ( rc != 0 )
				{
					die "can't create gsp100.h \n";
				}
				$gsp100_made=1;
			}

			# check BUILDPRAEFIX, CORRECTION_LEVEL, RELVER
			# if not existent save in evironment
			die "Error: CORRECTION_LEVEL must be set !\n" unless (defined $ENV{CORRECTION_LEVEL});
			die "Error: RELVER must be set !\n" unless (defined $ENV{RELVER});

			local $fd_gsp100 = new FileHandle;
			do
			{
				$MajorVersion = $MinorVersion = $CorrLevel = $BuildPref = -1;
				if ( ! $fd_gsp100->open("$ENV{WRK}/incl/gsp100.h", "r") )
				{
					die "Can't open $ENV{WRK}/incl/gsp100.h (input) :$!\n";
				}
				else
				{
				   while ( <$fd_gsp100> )
				   {
						if ( /^\s*#\s*define\s*MAJOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
						{	$MajorVersion=$1;	}
						if ( /^\s*#\s*define\s*MINOR_VERSION_NO_STR_SP100\s*"(.*)"/ )
						{   $MinorVersion=$1;	}
						if ( /^\s*#\s*define\s*CORRECTION_LEVEL_STR_SP100\s*"(.*)"/ )
						{	$CorrLevel=$1;		}
						if ( /^\s*#\s*define\s*BUILD_PREFIX_STR_SP100\s*"(.*)"/ )
						{	$BuildPref=$1;		}
						if (! defined $ENV{SAPDB_PURPOSE} && /^\s*#\s*define\s*SAPDB_PURPOSE_SP100\s*"(.*)"/ )
						{	$ENV{SAPDB_PURPOSE}=$1;	}
					}
					$fd_gsp100->close;
					$checkedBuild=1;
				}

				if ( ((($MajorVersion < 0) || ($MinorVersion < 0) || ($ENV{RELVER} ne "R$MajorVersion$MinorVersion") ||
					 ($CorrLevel < 0) || ( $ENV{CORRECTION_LEVEL} != $CorrLevel ) ||
					 ( ($ENV{BUILDPRAEFIX}) && ($ENV{BUILDPRAEFIX} != $BuildPref) )))
					 && ( ! $gsp100_made )
				   )
				{
					$rc = unsafe_imake (@gsp100_make);
					if ( rc != 0 )
					{
						die "can't create gsp100.h \n";
					}
					$gsp100_made=1;
					$checkedBuild=0;
				}
			}
			while (! $checkedBuild);

			die "Inconsistent build information between \$RELVER($ENV{RELVER}) and gsp100.h (R$MajorVersion$MinorVersion)\n"
					if ( ($MajorVersion < 0) || ($MinorVersion < 0) || ( $ENV{RELVER} ne "R$MajorVersion$MinorVersion" ) );

			die "Inconsistent build information between \$CORRECTION_LEVEL($ENV{CORRECTION_LEVEL}) and gsp100.h($CorrLevel)\n"
					if ( ($CorrLevel < 0) || ( $ENV{CORRECTION_LEVEL} != $CorrLevel ) );

			# only check if defined
			$BuildPref+=0;
			if ($ENV{BUILDPRAEFIX})
			{
				die "Inconsistent build information between \$BUILDPRAEFIX($ENV{BUILDPRAEFIX}) and gsp100.h($BuildPref)\n"
					if ( ($BuildPref < 0) || ( $ENV{BUILDPRAEFIX} != $BuildPref ) );

			}
			else
			{
				$ENV{BUILDPRAEFIX}=$BuildPref;
			}
		}

		if (( ($ENV{'RELSTAT'} !~ /TEST/i && !$noBuildNr && ($ENV{'RELVER'} lt "R74" ) ) || !$ENV{'SAPDB_INTERNAL'} )  && ( ! -f "$OWN/changelist.info"))
		{
			$BuildNr = 0;
		}
		else
		{
			$BuildNr = 0;
			if ( -f  "$ENV{OWN}/changelist.log")
			{
				open(CLLOG, "< $ENV{OWN}/changelist.log") || die "Couldn't open $ENV{OWN}/changelist for reading";
				$BuildNr = <CLLOG>;
				chomp $BuildNr;
				$BuildNr =~ s/\s//g;
				close (CLLOG);
			}
			elsif ( (-f  "$ENV{OWN}/changelist.info") && ( $ENV{'RELSTAT'} =~ /PROD/i ))
			{
				open(CLLOG, "< $ENV{OWN}/changelist.info") || die "Couldn't open $ENV{OWN}/changelist.info for reading";
				$BuildNr = <CLLOG>;
				chomp $BuildNr;
				if ( $BuildNr =~ /\$Change: (\d*) \$/)
				{
					$BuildNr = $1;
				}
				my $checkbuild=<CLLOG>;
				chomp $checkbuild;
				if ($checkbuild =~ /Build: (\d*)\s*$/)
				{
					$checkbuild = $1;
					die "build difference between environment ($ENV{BUILDPRAEFIX}) and changelist.info($checkbuild)\n"
					 	if ($checkbuild != $ENV{BUILDPRAEFIX});
				}
				close (CLLOG);
			}
			elsif (-f "$ENV{OWN}/syncinfo.dat")
			{
				open(SYNCINFO, "< $ENV{OWN}/syncinfo.dat") || die "Couldn't open $ENV{OWN}/syncinfo.dat for reading";
				$syncnumber = <SYNCINFO>;
				$BuildNr = $1 if ( $syncnumber =~ /<maxchangenr>(.*)<\/maxchangenr>/);
				close (SYNCINFO);
			}
			print "Warning: Can't find changelist information\n"
				if (( $ENV{RELSTAT} =~ /PROD/i ) && ! $BuildNr );

			$BuildNr = "0" x (6-length($BuildNr)) . "$BuildNr";
			my $build_info = ($ENV{RELSTAT} =~/PROD/i) ? "1":"0";
			$build_info.= ($ENV{LC_STATE} =~ /ANA/i) ? "4" : ($ENV{LC_STATE} =~ /HOT/i) ? "3" : ($ENV{LC_STATE} =~ /RAMP/i) ? "2" : ($ENV{LC_STATE} =~ /COR/i) ? "1" : "0";
			$build_info.=($ENV{SAPDB_PURPOSE}=~/BOTH|ALL/i) ? "3":($ENV{SAPDB_PURPOSE}=~/LC/i) ? "2" :
			                  ($ENV{SAPDB_PURPOSE}=~/OLTP/i) ? "1": "0";
			$BuildNr = "$build_info$BuildNr" ;
			$BuildNr = $1 if ($BuildNr =~ /^0*([^0].*)$/);
			print "Build: ", $BuildNr, "\n";
		}
    }
    elsif ( $ENV{APOMAKE} )
	{
		$BuildNr = $ENV{LATEST_CHANGE_LIST};
		$BuildNr = "0" x (6-length($BuildNr)) . "$BuildNr";
		$BuildNr = $ENV{APO_PATCH_LEVEL}.$BuildNr if ( $ENV{APO_PATCH_LEVEL} );
		$BuildNr = "0" x (9-length($BuildNr)) . "$BuildNr";
		$BuildNr = $ENV{APO_COM_BUILD}.$BuildNr if ( $ENV{APO_COM_BUILD} );

	}

    $ENV{'BUILDNUMBER'} = $BuildNr;
    $ENV{'BUILDSTRING'} = FormatBuildNr($BuildNr);

	# print "Buildnumber: $ENV{'BUILDNUMBER'}\n";
    # print "Buildstring: $ENV{'BUILDSTRING'}\n";

    # Sicherstellen, dass ein altes Build-Modul entfernt wird
    unlink "$WRK/fast/tmp/*_$ENV{'BUILDPID'}.*";
    unlink "$WRK/quick/tmp/*_$ENV{'BUILDPID'}.*";
    unlink "$WRK/slow/tmp/*_$ENV{'BUILDPID'}.*";

    # cba : 2000-10-20
    # for some reason I do not understand $TOOLVARS was undefined during remimf
	# on Windows for IA64
	#
    $TOOLVARS = $ENV{'TOOLVARS'};
    do "$TOOLVARS";
    die $@ if $@;


    if (defined ($optctl{t})) {
        $ENV{'TOOLTRACE'} = "on";
        @ARGV = ($TraceFile);
        do "$ENV{'TOOL'}/bin/closedoc$TOOLEXT";
        unlink $TraceFile;
    }

  	if (-f "$TOOL/data/changelist.log" )
	{
		open(CHLST, "<$TOOL/data/changelist.log");
		my $changelist=<CHLST>;
		chomp $changelist;
		$ENV{TOOL_CHANGELIST} = $changelist;
		close CHLST;
		print ("Current TOOL changelist is $ENV{TOOL_CHANGELIST}\n") if ($ENV{RELSTAT} =~ /PROD/i);
	}


    ToolTrace (scalar localtime ()."\timf @VMakeArgs\n\n");

	my $VMakeRc;
	# to debug vmake look for the $VMAKE_LOCATION
	if ($ENV{VMAKE_LOCATION})
	{
		print ("WARNING: use vmake in the directory $ENV{VMAKE_LOCATION}\n");
		$VMakeRc = system ($ENV{VMAKE_LOCATION}."/vmake", @VMakeArgs);
	}
	else
	{
		$VMakeRc = system ($ENV{TOOL}."/pgm/vmake", @VMakeArgs);
	}

	if (defined ($optctl{t})) {
        @ARGV = ("-r", $TraceFile);
        do "$TOOL/bin/opendoc$TOOLEXT";
        delete $ENV{'TOOLTRACE'};
    }

    # Sicherstellen, dass ein altes Build-Modul entfernt wird
    unless ($Options =~ /K/) {
        unlink <$WRK/fast/tmp/*_$ENV{'BUILDPID'}.*>;
        unlink <$WRK/quick/tmp/*_$ENV{'BUILDPID'}.*>;
        unlink <$WRK/slow/tmp/*_$ENV{'BUILDPID'}.*>;
    }

	$protname="$WRK/prot/Modules.$ENV{BUILDPID}.m0";
	$protname="$WRK/prot/Modules.m0" unless ( -f "$protname" );

    if ($useOldBuildNr)
    {
        my $cmd =
        "$TOOL/pgm/PutBuildPrt $ENV{'BUILDNUMBER'} $WRK/prot/Modules.$ENV{BUILDPID}.m0";
        if (system ($cmd) < 0) {
            warn "caution: Can't put protocol.\n";
        }
    }

	# set back to default return 0 if no errors occurs
	$VMakeRc /= 256;
	$VMakeRc =0 if ( $VMakeRc == 1 );

    @ARGV = @SavedArgv;
    return $VMakeRc;
}

##############################################################################
sub FormatBuildNr {
    local $Buildnr = $_[0];
    $Buildnr = "0" x (($ENV{APOMAKE} ? 12 : 9)-length($Buildnr)) . "$Buildnr";
    $Buildnr = join "-", grep /\d+/, split( /(\d\d\d)/, $Buildnr);

    if ( $RELVER eq "R62" or $RELVER eq "R71" ) {
	$ENV{BUILDPRAEFIX} = 0 unless defined($ENV{BUILDPRAEFIX});
	return sprintf("%.3d", $ENV{BUILDPRAEFIX}) . "-". $Buildnr;
    }

    return $Buildnr;
}


sub setup_compilation_mode {
	my @argv = @_;
	my $compilation_mode = undef;
	my $idx = 0;

	foreach $arg (@argv) {
        if ($arg eq "-32") {
            $compilation_mode = "32";
            splice (@argv, $idx, 1);
            next;
	    } elsif ($arg eq "-64") {
            $compilation_mode = "64";
            splice (@argv, $idx, 1);
            next;
        }
        $idx++;
	}

	if ($compilation_mode eq "32") {
		$ENV{WRK_NAME} = "wrk32";
		delete $ENV{BIT64};
		if ($^O =~ /hpux/i ) {
			$MACH = "PA11";
		} elsif ($MACH eq "ALPHA") {
			die "cannot create 32-bit executables on ALPHA\n";
		}
	} elsif ($compilation_mode eq "64") {
		$ENV{WRK_NAME} = "wrk64";
		$ENV{BIT64} = 1;
		if ($^O =~ /hpux/i ) {
                        die "\$MACH not defined\n";
		}
	} else {
               delete $ENV{WRK_NAME} ;
	}

	return ($compilation_mode, @argv);
}


#sub setDevEnv()
#{
#	local $SAPDBENV = ICopy::GetFilePath( "SAPDBEnv", 0, 0, 1 );
#
#	unless ( defined($SAPDBENV) )
#   {
#		print "Use standard SAPDB environment\n";
#		return 0;
#	}
#	else
#	{
#		if ( ICopy::_get_path_status( $SAPDBENV ) eq 'remote' )
#		{
#			ICopy::copy( $SAPDB_ENV, "$TMP/SAPDBEnv", 1 );
#			$$APDBENV = "$TMP/SAPDBEnv";
#		}
#		if ( open(SAPDBENV) )
#		{
#			while(<SAPDBENV>)
#			{
#				if ( /^([\w\d]+)\s*=\s*([\w\d]+)/ )
#				{ $ENV{$1}=$2; }
#			}
#			close(SAPDBDEV);
#		}
#		else
#		{
#			warn "Can't get SAPDB environment info!\n";
#			return -1;
#		}
#	}
#	return 1;
#
# }
