#!/usr/bin/perl
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
use Env;
use Getopt::Std;

if ( !getopts('k:') ) {
    print "USAGE: nachtkill.pl [-k <release>]\n";
    print "\n";
    print "       kills running nachtRelease, without parameter shows all running jobs\n";
    print "\n";
    exit 1;
}

my %Runs = GetRuns();

if ( $opt_k ) {

	if ( $Runs{$opt_k} ) {
		print "Killing release $opt_k (Pid:$Runs{$opt_k}[0])\n";
		PauseCron ($Runs{$opt_k}[0]);
		KillCron ($Runs{$opt_k}[0]);

		my @DBNames = GetSutDBNames($opt_k);
		foreach my $DB (@DBNames) {
			print "stop $DB\n";
	        system ("dbmcli -d $DB -u superdba,colduser db_stop");
		}

		my @LogFiles = GetLogFiles($opt_k);
		foreach my $LogFile (@LogFiles) {
			print "rm $LogFile in $ENV{TOOL}/currentLogin\n";
	        unlink ("$ENV{TOOL}/currentLogin/$LogFile");
		}

	} else {
		print "unknown or not running release: $opt_k\n"
	}

} else {

		foreach ( keys %Runs ) {
			print "release $_ started at $Runs{$_}[1] (Pid: $Runs{$_}[0])\n";
		}

}

########################################
sub GetRuns ()
{
    my %RunHash;

	opendir(DIR_IN, "$TOOL/currentLogin") or die "can't readdir $TOOL/currentLogin\n";
	my @Files = grep /.*\.rel/i, readdir(DIR_IN);
	closedir DIR_IN;

	foreach (@Files) {
			open (IN,"<$TOOL/currentLogin/$_");
			my @Lines = <IN>;
			close (IN);
			my ($Release) = $_ =~ /(\d+(_\d\d)?)\.rel/;
			my ($Pid, $Date) = $Lines[0] =~ /.*Pid=(\d+) logged in at (.*)$/;
			my @Parents = GetAllParentPids($Pid);
        	$RunHash{$Release} = [$Parents[length(@Parents)], $Date];
	}


    return %RunHash;
}
########################################
sub PauseCron ()
{
	my (@ProcList) = @_;

    while (@ProcList) {
		my $ActPid = shift @ProcList;
		kill 'STOP', $ActPid;
		push @ProcList , GetChildPids($ActPid);
	}

}

########################################
sub KillCron ()
{
	my (@ProcList) = @_;

    while (@ProcList) {
		my $ActPid = shift @ProcList;
		push @ProcList , GetChildPids($ActPid);
		kill 'KILL', $ActPid;
	}

}

########################################
sub GetAllChildPids ()
{
	my (@ProcList) = @_;
    my (@AllChilds);

    while (@ProcList) {
		my $ActPid = shift @ProcList;
		my @Childs = GetChildPids($ActPid);
		push @ProcList , @Childs;
		push @AllChilds, @Childs;
	}
return @AllChilds;
}

########################################
sub GetChildPids ()
{
	my ($ParentPid) = @_;

	my @ChildPids;
    my @psout = `ps -ef | grep $ENV{USER} | grep $ParentPid`;
    foreach my $ps ( @psout ) {
   		push (@ChildPids, $1) if ( $ps =~ /.*\s+(\d+)\s+$ParentPid\s+.+$/ );
	}

    return @ChildPids;

}

########################################
sub GetAllParentPids ()
{
	my (@ProcList) = @_;
    my (@AllParents);

    while (@ProcList) {
		my $ActPid = shift @ProcList;
		my @Parents = GetParentPids($ActPid);
		push @ProcList , @Parents;
		push @AllParents, @Parents;
	}
return @AllParents;
}

########################################
sub GetParentPids ()
{
	my ($ChildPid) = @_;

    my $psout = `ps -fp  $ChildPid`;
   	if ( $psout =~ /$ENV{USER}\s+$ChildPid\s+(\d+)\s+.+$/ ) {
		 my $PPid = $1;
		 $psout = `ps -fp $PPid`;
		 return ($PPid) if ( $psout =~ /$ENV{USER}\s+$PPid\s+\d+\s+.+$/ );
	}

	return ();

}

########################################
sub GetSutDBNames ()
{
	my ($RelName) = @_;

	my ($Rel,$BitKind) = $RelName =~ /(\d+)_(.*)/;

    my ($RelLength) = length $Rel;
    my @SutDBNames;
  	if (($ENV{BIT32Supported} eq "NO") or ($ENV{BIT64Supported} eq "NO")) {
	    push @SutDBNames, "SUT${Rel}";
    } elsif ( $RelLength <= 2 ) {
        if ( ($BitKind eq "32Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SUT${Rel}_32";
        }
        if ( ($BitKind eq "64Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SUT${Rel}_64";
        }
    } elsif ( $RelLength == 3 ){
        if ( ($BitKind eq "32Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SUT${Rel}32";
        }
        if ( ($BitKind eq "64Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SUT${Rel}64";
        }
	} else {
        if ( ($BitKind eq "32Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SU${Rel}32";
        }
        if ( ($BitKind eq "64Bit") or ($BitKind eq "32__64") ) {
            push @SutDBNames, "SU${Rel}64";
        }
    }

    return @SutDBNames;

}

########################################
sub GetLogFiles ()
{
	my ($RelName) = @_;

	my ($Rel,$BitKind) = $RelName =~ /(\d+)_(.*)/;

    my @LogFiles;
    if  ($BitKind) {
		push @LogFiles, "${Rel}_${BitKind}.rel";
	} else {
		push @LogFiles, "${Rel}.rel";
	}

    return @LogFiles;

}

