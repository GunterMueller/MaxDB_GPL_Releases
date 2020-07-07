#!/usr/bin/perl
#
# @(#)ToolLib       1998-04-14
#
# Collection of useful tools
#
# G. Großmann, SAP AG
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

package ToolLib;
require 5.000;
use Env;
@EXPORT_OK = qw(CreateBuildNrModule GetBuildNrModule SetBuildNrModule ToolTrace SrcCopy getWhatModule $TraceFile setIdentPrefix);

use Carp;
use FileHandle;
use SDBM_File;

#require "sutvars$ENV{TOOLEXT}";
do "$TOOL/bin/sutvars$TOOLEXT";

$use_ident_directive = 0;
if ($^O =~ /svr4/i)
{$use_ident_directive=1; }

my $ident_prefix;
my @IdentList=();

my $BuildNrFunction;
my $PersinfoString=undef;

if ( $RELVER eq "R62" )
{
    $BuildNrFunction="s98linknumber";
}
else
{
    $BuildNrFunction="s100buildnumber";
}
return( 1 );


###############################################################

sub CreateBuildNrModule
{
	local @Flags = @_;
	local $BuildModule;
	local $BuildObj;
    if ( $ENV{RELVER} ne "R61" && $ENV{RELVER} ne "TOOL" && $ENV{RELVER} ne "RTOOL")
    {
		unless ( $ENV{APOMAKE} )
		{
			return() if ( ! $BuildNrFunction );
			$BuildObj = GetBuildNrModule().".o";
			return( $BuildObj ) if ( -f $BuildObj );
			@Flags = @_;
			$BuildModule = GetBuildNrModule().".c";
			local $fd = new FileHandle;
			if ( ! $fd->open($BuildModule, "w") )
			{
				die "Can't open $BuildModule (output) :$!\n";
			}
			else
			{
				# PTS 1105044
				if ( $RELVER ge "R72" )
				{
					$fd->print("#define RESOURCE_FILE\n");
					$fd->print("#include \"gsp100.h\"\n");
					$fd->print("char * $BuildNrFunction(){return \"$ENV{BUILDSTRING}\";}\n");
					# $fd->print("char * $BuildNrFunction(){return BUILD_PREFIX_STR_SP100\"-$ENV{BUILDSTRING}\";}\n");
					$fd->print("char * ${BuildNrFunction}Ex(unsigned char * buildpraefix, unsigned long * buildnr)\n");
					$fd->print("{\n\tif ( buildpraefix ) * buildpraefix = BUILD_PREFIX_SP100;\n");
					$fd->print("\tif ( buildnr ) * buildnr = $ENV{BUILDNUMBER};\n");
					# $fd->print("\treturn BUILD_PREFIX_STR_SP100\"-$ENV{BUILDSTRING}\";\n}\n");
					$fd->print("\treturn \"$ENV{BUILDSTRING}\";\n}\n");
				}
				else
				{
					if (not defined($ENV{BUILDPRAEFIX})) { $ENV{BUILDPRAEFIX} = 0 }
					$fd->print("char * $BuildNrFunction(){return \"$ENV{BUILDSTRING}\";}\n");
					$fd->print("char * ${BuildNrFunction}Ex(unsigned char * buildpraefix, unsigned long * buildnr)\n");
					$fd->print("{\n\tif ( buildpraefix ) * buildpraefix = $ENV{BUILDPRAEFIX};\n");
					$fd->print("\tif ( buildnr ) * buildnr = $ENV{BUILDNUMBER};\n");
					$fd->print("\treturn \"$ENV{BUILDSTRING}\";\n}\n");
				}

			}

			if ( $RELVER eq "R62" or $RELVER eq "R71" )
			{
				do "$TOOLVARS";
				if ( $@ ) { print "$@"; exit }

				if ($ENV{RELVER} eq "RTOOL")
				{
					$REL_STRING = "Tool";
					$PERS_STRING = "Tool";
				} else {
					($FOO) = $RELVER =~ /^.(.)/i;
					($BAR) = $RELVER =~ /^..(.)/i;
					$REL_STRING = "Rel. ".$FOO.".".$BAR.".".$ENV{CORRECTION_LEVEL};
					$PERS_STRING = "downto ".$FOO.".".$BAR.".".$ENV{CORRECTION_LEVEL};

				}


				# PTS 1105044
				if ( $RELVER ge "R72" )
				{
					$fd->print("$MF_ID \"@(#)$REL_STRING Build: \"BUILD_PREFIX_STR_SP100\"-$ENV{BUILDSTRING}\" $ID_END\n");
				}
				else
				{
					$fd->print("$MF_ID \"@(#)$REL_STRING Build: $ENV{BUILDSTRING} \"$ID_END\n");
				}
			}
			$fd->close;
		}
		else
		{
			$BuildModule ="what_$ENV{BUILDPID}.cpp";
			$BuildObj = "what_$ENV{BUILDPID}.o";
			local  @ARGV = ( $BuildModule );
			do "make_what.pl";

			do "$TOOLVARS";
			if ( $@ ) { print "$@"; exit }
			push @Flags, "-nowhatstring";
			open (BUILDMOD, ">>$BuildModule") or
				die "Can't open $BuildModule: $?\n";

			print BUILDMOD scalar (GetOSRTEStrings ());
			print BUILDMOD scalar (GetSysInfoStrings ());
			close BUILDMOD;
		}

        {
            local  @ARGV = ( @Flags, "-nowhatstring", $BuildModule );
            do "$ENV{TOOL}/bin/compc$ENV{TOOLEXT}";
            if ( $@ ) { die "$@"; exit }
        }
        return( $BuildObj );
    }
	else
	{
        return ();
    }
}

########################################

sub getWhatModule
{
	my ($target, $linkcmd, @Flags) = @_;
	my $whatobj;
	my $whatfile=$target;
	my $build_prefix_str = $ENV{BUILDPRAEFIX};
	$build_prefix_str = "0" x (3-length($build_prefix_str)) . "$build_prefix_str";
	$vmake_path = $ENV{VMAKE_PATH};
	$vmake_path =~ tr![\\]![/]!;
	$whatfile =~ tr/\-\+\./_/;
	$whatfile.= "_$ENV{BUILDPID}" unless ($ENV{POGO});
	$ident_prefix = "$whatfile";
	$whatfile = "what_$whatfile";
	$whatobj  = $whatfile.".o";
	$whatfile.= ".c";
	$linkcmd =~ tr![\\\"]![/']!;
	do "$TOOLVARS";
	if ( $@ ) { print "$@"; exit }
	if ($ENV{RELVER} eq "RTOOL")
	{   $REL_STRING = "Tool";	}
	else
	{
		($FOO) = $RELVER =~ /^.(.)/i;
		($BAR) = $RELVER =~ /^..(.)/i;
		$REL_STRING = $FOO.".".$BAR.".".$ENV{CORRECTION_LEVEL};
	}
	local $fd = new FileHandle;
	if ( ! $fd->open("$whatfile", "w") )
	{
		die "Can't open $whatfile (output) :$!\n";
	}
	if ( $RELVER ge "R72" )
	{
		$fd->print("#define RESOURCE_FILE\n");
	}

	# empty IdentList for AIX
	@IdentList =();
	$fd->print(GetIdentString("LinkCmd", $target, $linkcmd)."\n");
	if ( caller() !~ /linkrel/i )
	{
		$fd->print(GetIdentString("Build", "Rel. "."$REL_STRING Build",
				( $RELVER ge "R72" ) ? "$build_prefix_str-$ENV{BUILDSTRING}" : $ENV{BUILDSTRING})."\n");
		$fd->print (scalar (GetOSRTEStrings ()));
		$fd->print (scalar (GetSysInfoStrings ()));
		$fd->print (GetIdentString ("makeuser",($^O =~ /win32/i) ? $ENV{USERNAME}:$ENV{USER})."\n" );
		$fd->print (GetIdentString ("makeid", $ENV{MAKEID})."\n");
		$fd->print (GetIdentString ("makedate", Timestamp(1))."\n");
		$fd->print (GetIdentString ("makepath", $vmake_path )."\n");
		$fd->print (GetIdentString ("makestate", $ENV{LC_STATE})."\n");
		$fd->print (GetIdentString ("purpose", $ENV{SAPDB_PURPOSE})."\n");
		$fd->print (GetIdentString ("toolchangelist", $ENV{TOOL_CHANGELIST})."\n");
	}
	if (defined $PersinfoString)
	{
		$fd->print(GetIdentString("downto","$PersinfoString")."\n");
	}

	if ($^O =~ /svr4|aix|win32|linux/i)
	{
		$fd->print ("char * dummy_ident_$ident_prefix"."[] = {");
		while (scalar(@IdentList))
		{
			$fd->print ( shift @IdentList );
			$fd->print (", ") if (scalar(@IdentList) > 0);
		}
		$fd->print ("};\n");
	}
	$fd->close;
    {
        local  @ARGV = ( @Flags, "-nowhatstring", "$whatfile" );
        do "$ENV{TOOL}/bin/compc$ENV{TOOLEXT}";
        if ( $@ ) { die "$@"; exit }
    }
    if ($^O =~ /aix/i)
    {
    	if ( ! $fd->open("whatinfo.exp", "w") )
		{
			die "Can't open whatinfo.exp (output) :$!\n";
		}
		$fd->print ( "dummy_ident_$ident_prefix\n" );
		$fd->close;
    	$whatobj .= " -bE:whatinfo.exp" if (caller() !~ /linkdll|linkrel/);
    }
	return ($whatobj);
}



########################################

sub GetSysInfoStrings
{
	#
	# this is only to make changes
	# in SysInfo possible after tools will
	# be frozen on final assembly machines
	#
	my @myINC = @INC;
	local @INC = @myINC;

        # This is done so that Open Source builds don't require
        # the Perl extensions imported below.
        # This will break any attempt to create an installer
        # from this build. DanielD
	return "" unless (exists $ENV{'SAPDB_INTERNAL'});

	if (-r $ENV{OWN}.'/tool/lib/perl5/SAPDB/Install/SysInfo.pm') {
		unshift @INC, $ENV{OWN}.'/tool/lib/perl5';
	}

	require SAPDB::Install::SysInfo;
	import SAPDB::Install::SysInfo;

	my $info = SAPDB::Install::SysInfo::GetSystemInfo ();

	my @out = ();
	while (my ($key, $val) = each %{$info})
	{	push @out, GetIdentString ("sysinfo_$key", "sysinfo.$key", $val);	}

	return @out if (wantarray);

	return (join ("\n", @out)."\n");
}

########################################

sub GetOSRTEStrings
{
	#
	# this is only to make changes
	# in OSRTE possible after tools will
	# be frozen on final assembly machines
	#
	my @myINC = @INC;
	local @INC = @myINC;

	if (-r $ENV{OWN}.'/tool/lib/perl5/OSRTE.pm') {
		unshift @INC, $ENV{OWN}.'/tool/lib/perl5';
	}

	require OSRTE;
	import OSRTE;

	my @out = ();
	while (my ($key, $val) = each %OSRTE)
	{
		my $ident =	$key eq 'OS'        ? 'Operating System' :
					$key eq 'ARCH'      ? 'Architecture' :
					$key eq 'CC'        ? 'C Compiler' :
					$key eq 'CPLUSPLUS' ? 'C++ Compiler' :
					$key;

		push @out, GetIdentString ("osrte_$key", $ident, $val);
	}
	return @out if (wantarray);

	return (join ("\n", @out)."\n");
}

#####################################

sub GetIdentString
{
	my ($key,$ident,$string) = @_;
	my $identstring;
	# key = ident if only two parameters
	($string,$ident) = ($ident,$key) unless ($string);
	$key = "${ident_prefix}_$key" if ($ident_prefix);
	$key = "ident_$key";

#	if ($use_ident_directive)
#	{ $identstring = "#ident \"@(#)"; }
#	elsif ($ident_prefix)
#	{ $identstring ="char ident_${ident_prefix}_${key}"."[] = \"@(#)"}
#	else
#	{ $identstring ="static char ident_${key}"."[] = \"@(#)"}
	$identstring ="static char ${key}"."[] = \"@(#)";

	# avoid C2026 error on MS platforms by inserting several "" in the string
	my $elch =  "$ident: $string";
	my $i = 0;
	while ($i < length($elch))
	{
	  $identstring .= "\"\"" if ($i > 0);
	  $identstring .= substr($elch, $i, 2000);
	  $i += 2000;
	}

#	$identstring .= $use_ident_directive ? "\"" : "\";";
	$identstring .= "\";";
	push @IdentList, $key;
	return "$identstring";
}

###############################################################

sub GetBuildNrModule
{
    if ($ENV{POGO})
    {
        return($BuildNrFunction);
    }
    else
    {
        return( $BuildNrFunction."_".$ENV{BUILDPID} );
    }
}

###############################################################

sub setIdentPrefix
{
	$ident_prefix=shift;
	$ident_prefix =~ tr/\-\+\./_/;
}


###############################################################

sub SetBuildNrModule
{
    local $func = shift;
    if ( defined($func) )
    {
        $BuildNrFunction = $func;
    }
    else
    {
        undef $BuildNrFunction;
    }
}

###############################################################

sub generate_persinfo
{
	require Buildinfo;
	import Buildinfo;

	my %persinfo = Buildinfo::get_persinfo();
	my $PersinfoNr=undef;

	foreach (keys %persinfo)
	{
		if ((! defined $PersinfoNr) || ($PersinfoNr > $persinfo{$_}))
		{ $PersinfoNr = $persinfo{$_}; }
	}

	if ($PersinfoNr =~ /^(\d\.\d\.\d\d)\.(\d\d)$/)
	{
		$PersinfoString = "$1 Build: 0$2";
	}

	return ($PersinfoString);
}

###############################################################

sub CheckIncludes
{
    my( $r_Includes, $File, $LAYER ) = @_;
    my %FileVariants;
    my @all_lines = ();

    local $fd = new FileHandle;
    if ( ! $fd->open($File, "r") )
    {
        warn "Can't open $File (input) :$!\n";
    }
    else
    {
        # check includes in $SOURCE with %INCLUDES
        my ($incname, $inc, $slowlevel, $lookup, %INCLUDES);
        $lookup = "";
        $lookup = "(QUICK_MSP00|SLOW_MSP00|QUICK_MEO00|SLOW_MEO00)" if $ENV{'CURR_VERSION'} =~ /^f/i;
        $lookup = "(SLOW_MSP00|SLOW_MEO00)" if $ENV{'CURR_VERSION'} =~ /^q/i;
        my $wrk = $ENV{'WRK'};
        $wrk =~ tr[\\][/];
        foreach $inc ( @$r_Includes )
        {
            $inc =~ tr[\\][/];
            my $ind = index($inc, "$wrk/incl/");
            if ( $ind == 0 )
            {
                $incname = substr( $inc, length("$wrk/incl/") );
                ( $incname ) = $incname =~ m![/]?(.+)!;
            }
            else { $incname = $inc }
			if ( $incname =~ /^(.*)(\(noextension\)|\.)$/)
			{	$INCLUDES{$1} = "";}
			else
			{
				$incname .= ".h" unless $incname =~ /\.(h|hpp|c|cpp)$/;
				$INCLUDES{$incname} = "";
			}
        }
        local $maxlines = 1200;
        local $noVariantFile=$File;
        if ($ENV{APOMAKE})
        {
       		$maxlines = 12000 ;
       		if ($File =~ /^(.*)(\+|-)[^.]*(\..*)$/)
       		{
	      		$noVariantFile = "$1$3";
	      	}
	    }

        while ( <$fd> ) {
            if ($. > $maxlines)
            {
	            push @all_lines, $_;
	            last;
	        }

			if (( $ENV{'RELVER'} lt  "R74" ) ||
			     (($ENV{'RELVER'} eq  "R74" ) &&
			       (($ENV{'CORRECTION_LEVEL'} < 3 ) || (($ENV{'CORRECTION_LEVEL'} == 3) && ($ENV{'BUILDPRAEFIX'} < 12)))
			     )
			   )
			{
	            if ( ($ENV{'CURR_VERSION'} !~ /^s/i) &&
	                 /#\s*if\s+(COMPILEMODE_MSP00|COMPILEMODE_MEO00).+$lookup/ )
	            {
	                while ( !/#\s*endif/ and !eof ) { $_ = <$fd> }
	            }
	        }

    		if ($ENV{APOMAKE})
    		{
		        while ( /^\s*#\s*pragma\s*(message|APO_MESSAGE)/ )
	            {
	            	$_ = <$fd>;
	       			warn "WARNING: Missing empty line after pragma definition in $noVariantFile (".($.-1).")\n"
	       				unless ( /^(\s*#\s*pragma\s*(message|APO_MESSAGE)|\s*$|\s*#\s*endif)/ );
	       			last unless ($_);
	            }
	        }


			# CTS 1107626
			if ( /^\s*\/\*\s* set maxlines\s*=\s*(\d*)\s*\*\//i )
	        {
				$maxlines = $1;
			}

            #   if ( /^\s*#\s*include\s*"(.*)"(?!.*no\s*check)/ )
		    if ( /^\s*#\s*include\s*"(.*)"(.*)$/ )
            {
				$incname = $1;
				my $comment = $2;
				$is_nocheck = ( $comment =~ /(.*no\s*check|.*dep=)/ );
				if ( $comment =~ /\/\*\s*&variant\s*(\+|-)(.*\S)\s*\*\// )
				{
					my $op = $1;
					my @variants = split /,\s*/, $2;
					unless (%FileVariants)
					{
						%FileVariants = GetFileVariants($File);
					}
					if ( $op =~ /\+/)
					{
						$is_nocheck = 1;
						foreach ( @variants )
						{
							if ( defined $FileVariants{$_})
							{
								$is_nocheck = 0;
								last;
							}
						}
					}
					else
					{
						$is_nocheck = 0;
						foreach ( @variants )
						{
							if ( defined $FileVariants{$_})
							{
								$is_nocheck = 1;
								last;
							}
						}
					}
				}
				#unless ($is_nocheck)
				#{
				    if ( defined( $INCLUDES{$incname} ) )
	                {
						$INCLUDES{$incname} = "1";
					}
					elsif ( defined( $INCLUDES{"SAPDB/".$incname} ) )  # CTS 1105037
					{
						$INCLUDES{"SAPDB/".$incname} = "1";
	                }
					# for STL (not right description implementation of STL) ->
					elsif ( defined( $INCLUDES{"SAPDB/STLport/stlport/".$incname} ) )
					{
						$INCLUDES{"SAPDB/STLport/stlport/".$incname} = "1";
	                }
					elsif ( ($LAYER ne "") && defined( $INCLUDES{"$LAYER/$incname"} ))
					{
						$INCLUDES{"$LAYER/$incname"}="1";
					}
					elsif ( ($LAYER ne "") && defined( $INCLUDES{"$WRK/incl/$LAYER/$incname"} ))
					{
						$INCLUDES{"$WRK/incl/$LAYER/$incname"}="1";
					}
	                else
	                {
	                    warn "$File($.): Include $incname not in include list\n($maxlines lines examined)\n"
							unless ($is_nocheck);
	                }
	            #}
	            $incname =~ tr/a-z/A-Z/;
	            $incname =~ s/\//_/g;
	            $incname =~ s/\//_/g;
	            $incname =~ s/-/_/g;
	            $incname =~ s/^(.+)\.(.*)$/__INC__${2}__$1/;
	            push @all_lines, "#ifndef $incname\n";
	            push @all_lines, "#define $incname 1\n";
	            push @all_lines, "#line $.\n";
	            push @all_lines, $_;
	            # found comment behind include
	            if (/\/\*(.*)$/)
	            {
	            	my $next_line = $1;

	            	while ($next_line)
	            	{
	            		if ($next_line =~ /\*\/(.*)$/)
	            		{
	            			$next_line = $1;
	            			if ($next_line =~ /\/\*(.*)$/)
	            			{
	            				$next_line = $1;
	            				next;
	            			}
	            			last;
	            		}
	            		$next_line = <$fd>;
	            		push @all_lines, $next_line;

	            	}
	            }
	            push @all_lines, "#endif\n";
	            push @all_lines, "#line ". (int($.)+1) . "\n";
			}
			else
			{
				push @all_lines, $_;
			}
        }
        foreach $key ( keys %INCLUDES )
        {
            warn "$key in include list for $File but not included\n($maxlines lines examined)\n"
                if $INCLUDES{$key} ne "1";
        }
        if  ($ENV{'FORCE_INC_DEFINES'})
        {
	        while ( <$fd> )
	        { push @all_lines, $_; }
	        $fd->close();

	        if ( ! $fd->open($File, "w") )
		    {
		        warn "Can't open $File (input) :$!\n";
		    }
		    else
		    {
		    	foreach (@all_lines)
		    	{ $fd->print ($_); }
		    	$fd->close();
		    }
		}

    }
    $fd->close();
}

###############################################################

sub ProtokollName
{
    my $PROG = shift;

    my $fh = new FileHandle "dummy.dbcmd", "w";
    #open(FILE_OUT, "+>dummy.dbcmd");
    #print FILE_OUT "vdnexit!\n";
    #close FILE_OUT;
    $fh->print("vdnexit!\n");
    $fh->close;

    system("$PROG -b dummy.dbcmd >$NULL");

    my $CMD = "ls -1tr c*.prt c*.prot";
    my $CI_PROT;
    open(PIPE_IN, "$CMD |");
    while(<PIPE_IN>) { chomp; $CI_PROT = $_ }
    close PIPE_IN;

    unlink("dummy.dbcmd");
    return $CI_PROT;
}

###############################################################

sub ToolTrace
{
	# usage ToolTrace($string, \@daten)
    $TraceFile = "$WRK/$VMAKE_VERSION/prot/trace.prot";
	if ( $ENV{'TOOLTRACE'})
    {
		local( $string, $r_daten ) = @_;
		open( TOOLTRACE, ">>$TraceFile") || die "Can't open $TraceFile (append): $!\n";
		print TOOLTRACE sprintf($string, @$r_daten);
		close(TOOLTRACE);
	}
}

###############################################################

sub Timestamp
{
    my $no_linefeed = shift;
    local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
	$year = $year - 100 if ( $year > 99 ); # PTS 1105199
	$mon++; # adapt month
    foreach ( "mon", "mday", "hour", "min", "sec" )
    {
	    $$_ = "0$$_" if length($$_) == 1;
    }
    my $timestamp = "$year-$mon-$mday-$hour.$min.$sec";
	$timestamp="0".$timestamp if ( $year < 10 );  # PTS 1105199
    $timestamp .= "\n" unless $no_linefeed;
    return($timestamp);
}

##################################################################

sub GetConnectLevel
{
    my $fh;
    local $rc;
    if ( $fh = new FileHandle "CONNECT.cmd", "r" )
    {
        while(<$fh>)
        {
            if ( /^CONNECT &1 IDENTIFIED BY &2 &3 &4 ISOLATION LEVEL (\d+).*/i )
            {
                $rc = $1; last;
            }
        }
        $fh->close;
    }
    return $rc;
}

##################################################################

sub GetProfilePath
{

    my $Release   = shift;
    my $ProfileDB = "\\\\P26326\\ToolSrc\\profiles";
    my %ProfileDB;
    my $ProfilePath;

    dbmopen(%ProfileDB, $ProfileDB, 0) ||
	warn "Can't open Profile-Database!: $!\n";

    $ProfilePath = %ProfileDB->{$Release};

    dbmclose(%ProfileDB);

    return($ProfilePath);

}

###################################################################

sub GetFileVariants
{
	my $filename = shift;
	my %Variants;
	# cut extension
	if ($filename =~ /^(.*)\.([^\.]*)$/)
	{	$filename = $1; }
	while ($filename =~ /^(.*)[\+-]([^\+-]*)$/)
	{
		$Variants{$2} = 1;
		$filename = $1;
	}
	return %Variants;
}

##################################################################

sub GetAllProfiles
{

    my $ProfileDB = "\\\\P26326\\ToolSrc\\profiles";
    my %ProfileDB;
    my @ProfileKeys;

    if ( dbmopen(%ProfileDB, $ProfileDB, 0) ) {

	@ProfileKeys = keys %ProfileDB;

	dbmclose(%ProfileDB);
    }

    return (@ProfileKeys);
}

########################################################################
# &gar  (CTS 1108093)                                                  #
########################################################################
# Funktion: SrcCopy($source, $target)
#
# src_copy kopiert eine Datei in ein Verzeichnis und hebt,
# wenn vorhanden, die Schreibsperre auf.
#
# $source  : absoluter Dateipfad
# $target  : absoluter Zielpfad
# Rückgabe : 1 bei Erfolg, 0 sonst, $! wird gesetzt
#
#########################################################################
sub SrcCopy
{
	local($source, $target) = @_;
	local $rc;

	if (-f $target)
	{
		chmod 0664, "$target";
		unlink ($target);
	}

	$rc = File::Copy::copy($source, $target) ;
	return 0 unless ( $rc );

     chmod 0664, "$target" if (-f $target);

	return 1;
}
