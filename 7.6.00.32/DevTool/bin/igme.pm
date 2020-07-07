#!/usr/bin/perl
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
# igme.pm
#
# cba 2000-07-14 : build in support for igme::cat
#
# cba 2000-10-18 : build in support for igme::igme in array context
#                  all exported subs are now in package igme, internal subs
#                  in subpackages are called via wrappers
#
#

package igme;

use Exporter;

@ISA = ('Exporter');
@EXPORT = ('igme', 'list', 'cat');

sub igme {
	return igme::igme::igme (@_);
}

sub list {
	return igme::list::list (@_);
}

sub cat {
	return igme::cat::cat (@_);
}

package igme::api;

use Getopt::Long;

sub new {
	my $self = {};
	bless $self;

	my $tree = igme::common::ParseXMLFile ();
	return undef if ($tree == 0);

	$self->{'tree'} = $tree;
	return $self;
}

sub cat {
	my ($self, $target) = @_;
	return undef unless (defined $self);

	my $tree = $self->{'tree'};
	return undef unless (defined $tree && $tree != 0);

	$target = '*.*.*.*' unless defined ($target);

	my $txt =
	igme::cat::GetVmakePath ($tree, igme::util::SplitTarget ($target));

	return ($txt) unless (wantarray ());
	return (split ('\n', $txt));
}

sub list {
	my ($self, $target) = @_;
	return undef unless (defined $self);

	my $tree = $self->{'tree'};
	return undef unless (defined $tree && $tree != 0);

	my @names =
	igme::list::GetReleaseNames ($tree, $target);

	return (@names) if (wantarray ());
	return (join ("\n", @names));
}

sub igme {
	my $self = shift;
	return undef unless (defined $self);

	my $tree = $self->{'tree'};
	return undef unless (defined $tree && $tree != 0);

	my $ignore_server = undef;
	my $os = undef;
	my $private_path = undef;
	my $output_file = undef;
	my $skel_file = undef;

	Getopt::Long::Configure ('pass_through');
	Getopt::Long::Configure ('no_auto_abbrev');

	local @ARGV = @_;
	GetOptions ('x!', \$ignore_server, 'o:s', \$os, 'p:s', \$private_path,
	            'f:s', \$output_file, 's:s', \$skel_file);

	$output_file = $ENV{OWN}."/.iprofile" unless defined ($output_file);
	$skel_file = $ENV{OWN}."/.iprofile.skel" unless defined ($skel_file);
	$os = $^O unless defined ($os);

	my @arg = ("-o", $os);
	push @arg, "-f", $output_file if (defined $output_file);
	push @arg, "-s", $skel_file if (defined $skel_file);
	push @arg, "-p", $private_path if (defined $private_path);
	push @arg, "-x" if (defined $ignore_server);
	push @arg, @ARGV;

	($output_file, $skel_file, @arg) = igme::igme::GetArguments (@arg);

	#
	# ia64-windows wants array
	#
	return (igme::igme::GetVmakePath ($tree, @arg)) if (wantarray ());

	#
	# unix-wants an output file
	#
	my $vmake_path = igme::igme::GetVmakePath ($tree, @arg);

	return -1 unless (defined $vmake_path || $vmake_path eq "");

	igme::igme::OutMakeEnv ($vmake_path, $output_file, $skel_file, @args);
	return 0;
}

package igme::common;

use SAPDBHosts;
use GME::XMLParser;
use IO::File;
use IO::SMB;

sub ParseXMLFile {
	my ($fh, $rc);

	$igme::common::location = $ENV{'SAPDB_MAKE_LOCATION'};
	if (!defined $igme::common::location ||
	     $igme::common::location eq ''  ||
	     $igme::common::location =~ /^kln$/i) {
		$igme::common::location = 'bea';
	}

	$igme::common::location = lc ($igme::common::location);

	my $XMLFile =
		"\\\\".$SAPDB_PROFILE_HOST."\\toolsrc\\profiles\\releases.xml";
	my $User = "port";
	my $Password = "port_win";

	my $HostOs = ($^O =~ /^win|^mswin/i) ? "WIN32" : "UNIX";

	if ($HostOs eq "UNIX") {
		IO::SMB::logon ($User, $Password);
		$fh = IO::SMB::new ();
		$rc = $fh->sysopen ($XMLFile);
	} else {
		$fh = IO::File::new();
		$rc = open ($fh, "< ".$XMLFile);
	}

	unless ($rc) {
		print ("cannot open ".$XMLFile."\n");
		return 0;
	}

	my $Parser = XMLParser::new ();
	my $Tree = $Parser->ParseStream ($fh);
	$fh->close ();
	return ($Tree);
}

package igme::util;

sub RemoveHeadingZeros {
	$_ = shift;

	# check parameters
	return undef unless (defined $_);

	# is it zero
	if (/^0+$/) {
		return "0";
	}

	# more than one digit
	if (/([1-9])(\d+)/) {
		return ($1.$2)
	}

	# only one digit
	if (/([1-9])/) {
		return $1;
	}

	return undef;
}

sub SplitTarget {
	my $Release;
	my $State;
	my $Correction;

	$_ = shift;
	if ($_ =~ /^([\d*]+)\.([\d*]+)\.([\d*]+).([\w*]+)$/) {
		$Release = $1.$2;
		$Correction = $3;
		$State = $4;
	} elsif ($_ =~ /^([\w*]+)$/) {
		# could be TOOL
		$Release = $1;
	}
	return ($Release, $Correction, $State);
}

sub IsMatching {
	my ($Val0, $Val1) = @_;

	return 1 if ($Val1 eq "*");
	return 1 if ($Val0 eq $Val1);
	return 0;
}

sub IsMatchingRelease {
	my ($Val0, $Val1) = @_;

	return 1 if ($Val1 eq "**");
	return 1 if ($Val0 eq $Val1);
	if ($Val1 =~ /^(\d+)(\*)/) {
		return 1 if ($Val0 =~ /^$1/);
	}
	if ($Val1 =~ /^(\*)(\d+)/) {
		return 1 if ($Val0 =~ /$2$/);
	}
	return 0;
}

sub PrintListEntry {
	my $Current = shift;
	my $Out = "";

	my $Release = $Current->GetAttribute ("release");
	my $Correction = $Current->GetAttribute ("correction");
	my $Build = $Current->GetAttribute ("build");
	my $Alias = $Current->GetAttribute ("alias");
	my $State = $Current->GetAttribute ("state");

	if ($Release =~ /^(\d)(\d)$/) {
		$Out = $1.".".$2;
	} else {
		$Out = $Release;
	}

	$Out .= ".".$Correction if (defined $Correction);
	$Out .= ".".$State if (defined $State);

	$Out .= " (build ".$Build.")";
	$Out .= " (alias ".$Alias.".".$State.")"
		if (defined $Alias);

	return $Out;
}

sub FindTarget {
	my ($Current, $Release, $Correction, $State) = @_;

	return 0 if (! defined $Release);
	return 0 if ($Current == 0);
	return 0 if ($Current->GetType() ne "releases");

	if (defined $Correction && defined $State) {
		$Current = $Current->VisitChild ();
		do {
			next if ($Current->GetType () ne "vmake_path");

			if ($Release eq $Current->GetAttribute ("release") &&
			$Correction eq $Current->GetAttribute ("correction") &&
			$State eq $Current->GetAttribute ("state")) {
				return ($Current);
			}
		} while (($Current = $Current->VisitNeighbor ()) != 0);
	} elsif (! defined $Correction && !defined $State) {
		$Current = $Current->VisitChild();
		do {
			next if ($Current->GetType () ne "vmake_path");

			if ($Release eq $Current->GetAttribute ("release")) {
				return ($Current);
			}
		} while (($Current = $Current->VisitNeighbor ()) != 0);
	}
	return 0;
}

sub FindTargetList {
	my ($Current, $Release, $Correction, $State) = @_;
	my @Matched = ();

	return () if (! defined $Release);
	return () if ($Current == 0);
	return () if ($Current->GetType() ne "releases");

	if (defined $Correction && defined $State) {
		$Current = $Current->VisitChild ();
		do {
			next if ($Current->GetType () ne "vmake_path");
			if (IsMatchingRelease (
			$Current->GetAttribute ("release"), $Release)
			&& IsMatching (
			$Current->GetAttribute ("correction"), $Correction)
			&& IsMatching (
			$Current->GetAttribute ("state"), $State)) {
				push @Matched, $Current;
			}
		} while (($Current = $Current->VisitNeighbor ()) != 0);
	} elsif (!defined $Correction && !defined $State) {
		$Current = $Current->VisitChild();
		do {
			next if ($Current->GetType () ne "vmake_path");
			if (IsMatching (
			$Current->GetAttribute ("release"), $Release)) {
				push @Matched, $Current;
			}
		} while (($Current = $Current->VisitNeighbor ()) != 0);
	}
	return @Matched;
}

sub FindAnyTarget {
	my $Current = shift;
	my $Matched = ();

	return () if ($Current->GetType()  ne "releases");
	$Current = $Current->VisitChild ();
	do {
		if ($Current->GetType() eq "vmake_path") {
			push @Matched, $Current;
		}
	} while (($Current = $Current->VisitNeighbor ()) != 0);
	return @Matched;
}

package igme::igme;

use Getopt::Long;

sub igme {
	my ($Outfile, $Skeleton, @Args) = GetArguments (@_);

	my $Tree = igme::common::ParseXMLFile ();
	return 0 if ($Tree == 0);

	return (GetVmakePath ($Tree, @Args)) if (wantarray ());

	my $VmakePath = GetVmakePath ($Tree, @Args);

	return 0 if ($VmakePath eq "");

	OutMakeEnv ($VmakePath, $Outfile, $Skeleton, @Args);
	return 1;
}

sub GetArguments {
	local @ARGV = @_;

	my ($Skeleton, $OutFile, $Private, $Release, $Correction, $State);
	my $Os = "";
	my $ObjCopy = 1;
	my $IgnoreServer = 0;

	GetOptions ('x!', \$IgnoreServer,
		'o:s', \$Os, 'p:s', \$Private, 'f:s', \$OutFile,
		's:s', \$Skeleton, 'objcopy!', \$ObjCopy);

	$Os = "WIN32" if (($Os =~ /^Win/i) || ($Os =~ /^MsWin/i));

	$_ = shift @ARGV;
	unless (defined $_) {
		usage ();
		exit;
	}

	return ($Skeleton, $OutFile, $Os, !$ObjCopy,
	        igme::util::SplitTarget ($_), $Private, $IgnoreServer);
}

sub usage {
	print "igme::igme - get make environment\n";
	print "usage:\n";
	print "igme -f output <-s skeleton> <-p private_path> <-x> <-o os> ".
	       "release_name\n";
	print "\n";
}

sub OutMakeEnv {
	my ($VmakePath, $Skeleton, $OutFile, $Os) = @_;

	my $HAS_SAPDB_INTERNAL_YES = 0;
	my $fh_out = IO::File::new();
	unless (open ($fh_out, "> ".$OutFile)) {
		print "cannot open ".$OutFile."\n";
		return;
	}

	if ($Os ne "WIN32") {
		print $fh_out "#!/bin/sh\n";
		print $fh_out "#\n";
		print $fh_out "# ".$OutFile."\n";
		print $fh_out "# this file was generated by igme.pl\n";
		print $fh_out "# please do not change this file\n";
		print $fh_out "#\n";
	}
	print $fh_out $VmakePath;

	# append skeleton file if there is any
	if (defined $Skeleton) {
		my $fh_in = IO::File::new();
		unless (open ($fh_in, "< ".$Skeleton)) {
			$fh_out->close();
			print "cannot open ".$Skeleton."\n";
			return;
		}
		print $fh_out "\n";
		while ($_ = $fh_in->getline()) {
			chomp;
			if ($_ =~ /SAPDB_INTERNAL/ && $_ =~ /YES/) {
				$HAS_SAPDB_INTERNAL_YES = 1;
			}
			print $fh_out $_."\n";
		}
		$fh_in->close();
	}

	$fh_out->close();

	if ($HAS_SAPDB_INTERNAL_YES == 0) {
		print "WARNING: SAPDB_INTERNAL not set to YES in .iprofile\n";
	}
}

sub TraverseReleases {
	my $Current = shift;
	return () if ($Current->GetType()  ne "releases");

	$Current = $Current->VisitChild();
	do {
		if ($Current->GetType() eq "vmake_path") {
			my $VmakePath = TraverseVmakePath($Current);
			print $VmakePath."\n";
		}
	} while (($Current = $Current->VisitNeighbor ()) != 0);
}

sub GetVmakePath {
	my ($Current, $Os, $NoObjCopy, $Release,
	    $Correction, $State, $Private, $IgnoreServer) = @_;

	$Current =
	igme::util::FindTarget ($Current,  $Release, $Correction, $State);
	if ($Current == 0) {
		return undef if (wantarray ());
		return "";
	}

	my %GME = TraverseVmakePath
		($Os, $NoObjCopy, $Current, $Private, $IgnoreServer);

	return (%GME) if (wantarray ());

	my ($key, $value);
	my $Out = "";
	while (($key, $value) = each %GME) {
		if ($Os eq "WIN32") {
			if ($key eq "VMAKE_PATH") {
				$value =~ s/\$OWN/%OWN%/;
			}
			if (defined $value) {
				$Out .= "set ".$key."=".$value."\n";
			} else {
				$Out .= "set ".$key."=\n";
			}
		} else {
			if (defined $value) {
				$Out .= $key."=".$value;
				$Out .= " export ".$key."\n";
			} else {
				$Out .= "unset ".$key."\n";
			}
		}
	}
	return $Out;
}

sub TraverseVmakePath {
	my ($Os, $NoObjCopy, $Current, $Private, $IgnoreServer) = @_;
	my %GME = ();

	return undef unless ($Current->GetType() eq "vmake_path");

	my $Release = $Current->GetAttribute ("release");
	my $Correction = $Current->GetAttribute ("correction");
	my $Build = $Current->GetAttribute ("build");
	my $Depot = undef;

	$Correction = igme::util::RemoveHeadingZeros ($Correction);
	$Build = igme::util::RemoveHeadingZeros ($Build);

	$GME{"RELVER"} = "R".$Release;
	$GME{"CORRECTION_LEVEL"} = $Correction;
	$GME{"BUILDPRAEFIX"} = $Build;

	my $VmakePath = "";
	my $GRP = undef;
	$Current = $Current->VisitChild();
	do {
		if ($Current->GetType() eq "layer") {
			my $Layer = TraverseLayer ($Os, $NoObjCopy, $Current);
			if (defined $Layer && $Layer ne '') {
				if ($IgnoreServer == 0) {
					$VmakePath .= ','
					unless ($VmakePath eq '');
					$VmakePath .= $Layer;
				}

				if (($Layer =~ /^\$OWN$/) ||
				($Layer =~ /^%OWN%$/i)) {
					if ($IgnoreServer != 0) {
						$VmakePath .= ','
						unless ($VmakePath eq '');
						$VmakePath .= $Layer;
					}
					#set private dir after '$OWN'
					if (defined $Private &&
					$Private ne '') {
						$VmakePath .= ','.$Private;
					}
				} elsif (!defined $GRP) {
					$GRP = $Layer;
				}
			}
		} elsif ($Current->GetType() eq "depot") {
			$Depot = TraverseDepot ($Current);
		}
	} while (($Current = $Current->VisitNeighbor ()) != 0);

	if ($Os eq "WIN32") {
		$GME{"GRP"} = $GRP;
	}
	$GME{"VMAKE_PATH"} = $VmakePath;
	$GME{"DEPOT"} = (defined $Depot && $Depot ne '') ? $Depot : undef;
	return %GME;
}

sub TraverseDepot {
	my $Current = shift;
	my $Content = $Current->GetContent ();
	return ($Content);
}

sub TraverseLayer {
	my ($Os, $NoObjCopy, $Current) = @_;
	my $Layer = '';

	return '' if ($Current->GetType()  ne "layer");

	my $Location = $Current->GetAttribute ('loc');
	return '' if (
		defined $Location && $Location ne '' &&
		$Location ne $igme::common::location);

	$Current = $Current->VisitChild();

	do {
		my $Type = $Current->GetType();
		if ($Type eq "host") {
			$Layer .= TraverseHost ($Os, $Current);
		} elsif ($Type eq "share") {
			$Layer .= TraverseShare ($Os, $Current);
		} elsif ($Type eq "dir") {
			$Layer .= TraverseDir ($Os, $NoObjCopy, $Current);
		}
	} while (($Current = $Current->VisitNeighbor()) != 0);

	return $Layer;
}

sub TraverseHost {
	my ($Os, $Current) = @_;
	my $Content = $Current->GetContent ();
	if ($Os eq "WIN32") {
		return ("\\\\".$Content."\\");
	} else {
		return ($Content.":");
	}
}

sub TraverseShare {
	my ($Os, $Current) = @_;
	my $Content = $Current->GetContent ();
	if ($Os eq "WIN32") {
		return ("");
	} else {
		return ($Content."/");
	}
}

sub TraverseDir {
	my ($Os, $NoObjCopy, $Current) = @_;
	my $Content = $Current->GetContent ();
	if ($Os eq "WIN32") {
		unless ($Content =~ /^\$(\w+)/) {
			$Content =~ s/\//\\/;
			$Content .= "//" if ($NoObjCopy);
		}
		return ($Content);
	} else {
		$Content .= "//" unless ($Content =~ /^\$/);
		return ($Content);
	}
}

package igme::list;

sub GetMaxLength {
	my @Lines = @_;
	my $MaxLen = 0;

	foreach $_ (@Lines) {
		@_ = split (' ', $_);
		my $Len = length ($_[0]);
		$MaxLen = $Len if ($Len > $MaxLen);
	}

	return $MaxLen;
}

sub FormatOutput {
	my @In = @_;
	my $MaxLen = GetMaxLength (@In);
	my @Out = ();

	foreach $_ (@In) {
		@_ = split (' ', $_);
		$_ = shift (@_);

		my $HowMany = $MaxLen - length ($_) + 1;
		push @Out, $_.(" " x $HowMany).join (" ", @_);
	}

	return @Out;
}

sub GetReleaseNames {
	my $Current = shift;
	my $Target = shift;
	my @Releases = ();
	my @Matched = ();

	if (defined $Target) {
		@Matched = igme::util::FindTargetList
		($Current, igme::util::SplitTarget($Target));
	} else {
		@Matched =
		igme::util::FindAnyTarget ($Current);
	}
	foreach $Current (@Matched) {
		push @Releases, igme::util::PrintListEntry ($Current);
	}

	@Releases = FormatOutput (@Releases);
	return (@Releases);
}

sub list {
	my $Target = shift;

	my $Tree = igme::common::ParseXMLFile ();
	return 0 if ($Tree == 0);

	my @ReleaseNames = GetReleaseNames ($Tree, $Target);

	return 0 if ($#ReleaseNames == -1);

	foreach $_ (@ReleaseNames) {
		print $_."\n";
	}

	return 1;
}

package igme::cat;

sub TraverseLayer {
	my $Current = shift;
	my $Layer = '';

	return '' if ($Current->GetType()  ne "layer");
	my $Location = $Current->GetAttribute ('loc');

	return '' if (
		defined $Location && $Location ne '' &&
		$Location ne $igme::common::location);

	$Current = $Current->VisitChild();

	do {
		my $Type = $Current->GetType();
		if ($Type eq "host") {
			$Layer .= TraverseHost ($Current);
		} elsif ($Type eq "share") {
			$Layer .= TraverseShare ($Current);
		} elsif ($Type eq "dir") {
			$Layer .= TraverseDir ($Current);
		}
	} while (($Current = $Current->VisitNeighbor()) != 0);

	return $Layer;
}

sub TraverseHost {
	my $Current = shift;
	my $Content = $Current->GetContent ();
	return ($Content.":");
}

sub TraverseShare {
	my $Current = shift;
	my $Content = $Current->GetContent ();
	return ($Content."/");
}

sub TraverseDir {
	my $Current = shift;
	my $Content = $Current->GetContent ();
	return ($Content);
}

sub TraverseDepot {
	my $Current = shift;
	my $Content = $Current->GetContent ();
	return ($Content);
}

sub TraverseVmakePath {
	my $Current = shift;
	my $Out = "";

	return () unless ($Current->GetType() eq "vmake_path");

	my $Release = $Current->GetAttribute ("release");
	my $Correction = $Current->GetAttribute ("correction");
	my $Build = $Current->GetAttribute ("build");

	$Correction = igme::util::RemoveHeadingZeros ($Correction);
	$Build = igme::util::RemoveHeadingZeros ($Build);

	my $VmakePath = "";
	my $Depot = undef;
	$Current = $Current->VisitChild();
	do {
		if ($Current->GetType() eq "layer") {
			my $Layer = TraverseLayer ($Current);
			if (defined $Layer && $Layer ne '') {
				$VmakePath .= "\n" unless ($VmakePath eq '');
				$VmakePath .= '  '.$Layer;
			}
		} elsif ($Current->GetType() eq "depot") {
			$Depot = TraverseDepot ($Current);
		}
	} while (($Current = $Current->VisitNeighbor ()) != 0);

	if (defined $Depot) {
		$Out .= "depot:\n";
		$Out .= "  ".$Depot."\n";
	}

	$Out .= "path:\n";
	$Out .= $VmakePath."\n";
	return $Out;
}

sub GetVmakePath {
	my $Out = "";
	my @Matched = igme::util::FindTargetList (@_);

	foreach $Current (@Matched) {
		$Out .= igme::util::PrintListEntry ($Current)."\n";
		$Out .= TraverseVmakePath ($Current)."\n";
	}
	return $Out;
}

sub cat {

	my $Target = shift;
	return 0 unless (defined $Target);
	return 0 if ($Target eq "");

	my $Tree = igme::common::ParseXMLFile ();
	return 0 if ($Tree == 0);

	my $VmakePath = GetVmakePath ($Tree, igme::util::SplitTarget ($Target));

	return 0 if ($VmakePath eq "");

	print $VmakePath;

	return 1;
}

