#!/usr/bin/perl
#
# @(#)link_opts		1998-05-09
#
# common link options
#
# Gert Groﬂmann (SAP AG, Basis Entwicklung Berlin)
#
# 2000-10-12 JoergMensing $TOOL/lib replaced by $TOOL_LIB, $INSTROOT/lib by $INSTLIB
#
# no package name, because it should run in the name space of the caller
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

use File::Basename;

$USE_THREAD = 0;
$DEFAULT_NOTHREAD = 0;
$NO_SYMBOL_FILES = 0;
@DENIES = ();

my( $i, $maxi ) = ( 0, $#ARGV );
while( $_ = $ARGV[ $i ], ( /^[-\+]/ && $i<=$maxi ) )
{
    last if /^--$/;
    local $toolopt = undef;
    if (/^-tooloption=(.*)/) {
        $toolopt = $1;
	}
	if (/^-toolopt=(.*)/) {
	    $toolopt =$1;
	}
	if (defined $toolopt) {
        if      ( $toolopt =~ /^CXX$/ )
        {
            $LD = $CXX;
            $LDDLL_TYPE = "LDDLL_CXX";
			splice @ARGV, $i, 1; next;
        }
        if      ( $toolopt =~ /^CC$/ )  #PTS 1002110
        {
			$LD = $CC;
			$LDDLL_TYPE = "LDDLL_C";
			splice @ARGV, $i, 1; next;
        }

		if      ( $toolopt =~ /^CXX_CC$/ )  #CTS 1107045
        {
			$LD = $CC;
			$LDDLL_TYPE = "LDDLL_CXX_C";
			splice @ARGV, $i, 1; next;
        }
        elsif ( $toolopt =~ /^nolinkdebug/ )
        {
            $NOLINKDEBUG = 1;
        }
        elsif( $toolopt =~ /^BuildNrFunction=(.*)/i)
        {
            ToolLib::SetBuildNrModule($1);
        }
        else
        {
            warn "unknown tool option $toolopt \n";
        }
        splice @ARGV, $i, 1; next;
    }

    if ( /^-linkoption=(.*)/)
    {
		local $linkopt = $1;
		if ( $linkopt =~ /^lib=(.+)/ )
		{
	        local $libname = $1.$ARCH_TYPE;
	        if ( $libname =~ /^ptocnts/i || $libname =~ /^ptocntr/i )
	        {
	          @LIBPC = ( "$TOOL_LIB/$libname" );
	          ToolTrace("linklnk: link with @LIBPC (temporary)\n");
	        }
	        else
	        {
	            push @Lib,$libname;
	        }
		}
		else
		{
			push @OFLAGS, $1;
		}
        splice @ARGV, $i, 1; next;
    }

    if ( /^-k$/ )
    {
        @KEEPFLG = "-k";
        splice @ARGV, $i, 1; next;
    }
    elsif ((/^-g$/) || (/^-optdebug/ ))
    {
        $DEBUG = "y";
        warn "Warning: make with debug flag, but it's a productive make!\n"
            if $ENV{RELSTAT} =~ /^prod/i;
        splice @ARGV, $i, 1; next;
    }
    elsif(/^-BuildNrFunction\s*=\s*(.*)/i)
    {
        ToolLib::SetBuildNrModule($1);
        splice @ARGV, $i, 1; next;
    }
    elsif(/^-generate_persinfo/i)
    {
        ToolLib::generate_persinfo();
        splice @ARGV, $i, 1; next;
    }
    elsif (/^-o$/)
    {
        $OUTFILE = $ARGV[ $i + 1 ];
        $OUTFILE =~ /(.*)\.[fqs]/, $PROG = $1 ne "" ? $1 : $OUTFILE ;
        splice @ARGV, $i, 2; next;
    }
    elsif(/^-p$/)
    {
        splice @ARGV, $i, 1; next;
	}
    elsif(/^-Y$/)
    {
        # ignore -Y
        splice @ARGV, $i, 1; next;
    }
    elsif (/^-x=(.*)/)
    {
        $SCRIPT = $1;
        splice @ARGV, $i, 1; next;
    }
    if (/^-noscrlib$/)
    {
        @LIBSCR = ();
        splice @ARGV, $i, 1; next;
    }
	elsif(/^-lib=(.*)/ && defined( $1 ))
    {
        my $lib = $1;
        my $dir = dirname($lib);
		local $basename = basename($lib, '.a');
		local $libname = ($dir ne "." ? $dir."/" : "").basename($lib, '.a' ).".a";
		local $slibname = ($dir ne "." ? $dir."/" : "").basename($lib, '.a' ).".$DLL_TYPE";

        if ( $libname =~ /^ptocnts/i || $libname =~ /^ptocntr/i )
        {
          @LIBPC = ( "$TOOL_LIB/$libname" );
          ToolTrace("linklnk: link with @LIBPC (temporary)\n");
        }
        else
        {
			push @Lib,$libname;
			if ( $basename =~ /^lib([^\/]*)$/)
            {
            	if ( -f "$slibname" )
            	{
					push @SLIBS, "-L$dir", "-l$1" ;
				}
				else
				{
					die "$slibname not found \n";
				}
            }
            else
            {
                print "WARNING: try to link lib with irregular name \"$basename.$DLL_TYPE\"\n";
				print "         ( library name must begin with \"lib...\" )\n";
				print "         Option: \"-lib=$lib\" ignored\n";
            }
        }
        splice @ARGV, $i, 1; next;
	}
    elsif(/^-plib=(.*)/ && defined( $1 ))
    {
        my $platformLib = $plibMapping{$1};
        if ($platformLib) {
            if ($^O eq 'MSWin32') {
                push @SLIBS, $platformLib;
            }
            else {
                push @SLIBS, '-l' . $platformLib;
            }
        }
        splice @ARGV, $i, 1; next;

    }

	if (/^-use_thread/ )
	{
		$USE_THREAD = 1;
		splice @ARGV, $i, 1;
		next;
	}

	if (/^-default_nothread/ )
	{
		$DEFAULT_NOTHREAD = 1;
		splice @ARGV, $i, 1;
		next;
	}

	if (/^-nothread/ ) {
		$NOTHREAD=1;
		splice @ARGV, $i, 1;
		next;
	}

	if(/^-nowhatstring/)
    {
    	$NOWHATSTRING=1;
   	    splice @ARGV, $i, 1; next;
    }

    if(/^-append=(.+)$/)
    {
    	push @APPENDFLAGS, split(/&/, $1);
   	    splice @ARGV, $i, 1; next;
    }

    if(/^-deny=(.+)$/)
    {
    	push @DENIES, split(/,/, $1);
   	    splice @ARGV, $i, 1; next;
    }

    if (/^-no_symbol_files/)
    {
    	$NO_SYMBOL_FILES = 1;
    	splice @ARGV, $i, 1; next;
    }

    if(/^-keep_symbols/)
    {
    	$KEEP_SYMBOLS=1;
   	    splice @ARGV, $i, 1; next;
    }

    if(/^-symbol_dir=(.+)$/)
    {
    	$SYMBOL_DIR=$1;
   	    splice @ARGV, $i, 1; next;
    }

    if(/^-map_dir=(.+)$/)
    {
    	$MAP_DIR=$1;
   	    splice @ARGV, $i, 1; next;
    }

    if(/^-force_pdb_files=(.+)$/)
    {
    	$USE_PDB_FILES = 1;
    	splice @ARGV, $i, 1; next;
    }

    if(/^-quantify/)
    {
    	$QUANTIFY=1;
    	splice @ARGV, $i, 1; next;
    }

    $i++;
}

if ( $NOTHREAD || ($DEFAULT_NOTHREAD && ( $USE_THREAD == 0 )))
{
	$NOTHREAD=1;
	@THREAD_LDFLAGS=();
	@LIB_PTHREAD=();
}            #  CTS 1110865

