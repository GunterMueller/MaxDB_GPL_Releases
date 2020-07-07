#!/usr/bin/perl
# iprot.pm           [-old | -h{0-9}] target
#
# @(#)iprot.pm  		1999-07-27
#
# Displays protocol of "target".
#
# Changes:
# cba 1999/07/27
# ipf, ipq und ips im iprot.pm zusammengefasst
# -32 und -64 option unter UNIX, siehe ADIS CR 1103480
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


package iprot;
use Env;
use Carp;
use Exporter;

@ISA = ('Exporter');
@EXPORT = ('iprot','convertFileString');
$ERRORPROTOCOL=0;

#
# iprot ()
# usage: iprot ("ipf"|"ipq"|"ips", @arguments)
#
sub iprot {
    my $Prog = shift;
    my @argv = @_;
    my $History = "-h 0";
	my $Usage = "usage: ".$Prog." [ -old | -h{0-9}] [-a] [-e|-E] [-x|-X] target\n";

    SWITCH: {
        if ($Prog eq "ipf") {$Version = "fast"; last SWITCH}
        if ($Prog eq "lcprot") {$Version = "fast"; last SWITCH}
        if ($Prog eq "ipq") {$Version = "quick"; last SWITCH}
        if ($Prog eq "ips") {$Version = "slow"; last SWITCH}
    }


    @argv = setup_compilation_mode (@argv);

    while ($_ = $argv[0], /^-/) {
        shift @argv;
        last if /^--$/;
        if (/^-old$/)   { $History = "-h 1"; next }
		if (/^-e$/)		{ $ERRORPROTOCOL = 1; next;}
		if (/^-E$/)		{ $ERRORPROTOCOL = 2; next;}
		if (/^-x$/)		{ $EXTPROTOCOL = 1; next;}
		if (/^-X$/)		{ $EXTPROTOCOL = 2; next;}
		if (/^-a$/)		{ $ALLPROTOCOL = 1; next;}
		if (/^-h(\d)$/) { $History = "-h $1"; next }
        if (/^-.*/)     { print $Usage; exit 1 }
    }

    my $Target = $argv[0];
    require SDB::Make::Path;
	import SDB::Make::Path;
	$Target = SDB::Make::Path::GetQualifiedTargetName($Target);

    if ($Target eq "") { print $Usage; exit 1}
	my $ProtList="";

	if ((($ERRORPROTOCOL < 2) && ($EXTPROTOCOL < 2)) || $ALLPROTOCOL )
	{
		$ProtList .= `$TOOL/pgm/vmakprot $History $Version $Target`;
		chomp $ProtList;
		$ProtList = convertFileString ($ProtList);
		if ($ProtList =~ /^no protocol/i) {
			$ProtList = "";
			print "no protocol for $Target \n";
		}
	}
	if ( $EXTPROTOCOL || $ALLPROTOCOL )
	{
		$ProtList .= " " if ($ProtList ne "");
		$ProtList .= `$TOOL/pgm/vmakprot -x $History $Version $Target`;
		chomp $ProtList;
		$ProtList = convertFileString ($ProtList);
		if ($ProtList =~ /^ no protocol/i) {
			$ProtList = "";
			print "no extended protocol for $Target \n";
		}
    }

    if ( $ERRORPROTOCOL || $ALLPROTOCOL )
	{
		$ProtList .= " " if ($ProtList ne "");
		$ProtList .= `$TOOL/pgm/vmakprot -e $History $Version $Target`;
		chomp $ProtList;
		$ProtList = convertFileString ($ProtList);
		if ($ProtList =~ /^ no protocol/i) {
			$ProtList = "";
			print "no error protocol for $Target \n";
		}
	}

	$ProtList =~ s/vmake_path is NULL !!!.*\n//g;

    if ($ProtList ne "") {
	 @ARGV = ("-e", $EDITOR, "-r", reverse split / /, $ProtList);
	 do "$TOOL/bin/opendoc$TOOLEXT";
         die "Error while executing opendoc$TOOLEXT: $@" if ($@);
    }
}

#
# find out if we should compile 64-bit- or 32-bit- or traditional mode
#
sub setup_compilation_mode {
	my @argv = @_;
	my $compilation_mode = undef;
	my $idx = 0;

	foreach $arg (@argv) {
        	if ($arg eq "-32") {
                	$compilation_mode = "32";
			splice (@argv, $idx, 1);
               	 	next;
	        }
	        if ($arg eq "-64") {
        	        $compilation_mode = "64";
                	splice (@argv, $idx, 1);
                	next;
        	}
        	$idx++;
	}

	if ($compilation_mode eq "32") {
		$ENV{WRK_NAME} = "wrk32";
		delete $ENV{BIT64};
		if ($OSSPEC eq "HPUX" ) {
			$MACH = "PA11";
		} elsif ($MACH eq "ALPHA") {
			die "cannot create 32-bit executables on ALPHA\n";
		}
	} elsif ($compilation_mode eq "64") {
		$ENV{WRK_NAME} = "wrk64";
		$ENV{BIT64} = 1;
		if ($OSSPEC eq "HPUX" ) {
                        die "\$MACH not defined\n";
		}
	}

	return @argv;
}


1;

sub convertFileString
{
	local $filestring = shift;
	if($^O=~/.*win32.*/i)
	{
		$filestring =~ tr/\//\\/;
	}
	else
	{
		$filestring =~ tr/\\/\//;
	}
	return $filestring;
}


