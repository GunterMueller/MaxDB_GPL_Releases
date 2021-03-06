#!/usr/bin/perl
# istat         [-d date] [-j days] [-t time] [-u user] [-n node] \
#               [-a action] [-f file]
#
# Generates a report of actions on files stored in the "$GRP" tree. This
# report is based on the log file "$GRP/.statfile.PROT".
# "date" has one of the formats "yyyymmdd", "mmdd" or "dd", denoting year,
# month and day, month and day of the current year or day of the current
# month respectively. All actions between the specified date and today are
# included in the report.
# "days" is an absolute number of days. All actions not older than "days"
# days are included in the report.
# "time" has one of the formats "HH", "HHMM" or "HHMMSS", denoting the
# starting hour (minutes and seconds) of the first day to be included in
# the report. If neither -d nor -j is specified, -j 0, i.e. the current
# day, is assumed.
# "user" and "node" specify the originating user and node (the subject)
# of an action to be included in the report. "users" can be specified with
# their first names, last names or prtions of their names. (e.g. -u axel)
# "action" specifies the type of an action, which may be one of "get",
# "put", "del", "lock" or "unlock" (g,p,d,l,u), denoting the attachement
# of a file version to "user", the generation of a new file version by
# "user", the detachement of a file version from "user", the receipt of
# an exclusive lock for "file" by "user" or the releasement of a lock.
# "file" specifies the object of an action, i.e. a file name in the "$GRP"
# tree.
# For all option values regular expressions (ed) may be specified. The
# conjunction of different options is taken. If istat is invoked without
# any option, all lines of "$GRP/.statfile.PROT" are included in the report. The
# generated report is written to standard output.
# &gar : CTS 1103611
# new option -s => call istatus.exe <filename>
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


package istat;
use Env;
use Getopt::Std;
use DUser;

$USAGE = "Usage: istat [-d date] [-t time] [-j days] [-u user] [-n node] \n".
		 "             [-a action] [[-s] -f file]\n".
		 "actions: [g]et, [p]ut , [d]el, [l]ock, [u]nlock (g==l, p==u)\n";

$statfilename = "statfile64.PROT";
$oldstatfilename = "statfile.PROT";
$CHANGEDATE="1999-12-07";
$CHANGETIME="14:00:00";

#my( $i, $maxi ) = ( 0, $#ARGV );
#while( $_ = $ARGV[ $i ], ( /^-/ && $i<=$maxi ) )
#{
#	if (/^-old/)
#	{
#		$statfilename = "statfile.PROT";
#		splice @ARGV, $i, 1;
#		next;
#	}
#	$i++;
#}

getopts('a:d:j:t:f:n:u:os') || die $USAGE;
die $USAGE if @ARGV != 0;
$ACTION = $opt_a if defined($opt_a);
$DATE 	= $opt_d if defined($opt_d);
$DAYS 	= $opt_j if defined($opt_j);
undef $DATE if defined($opt_j);
$TIME 	= $opt_t if defined($opt_t);
$FILE 	= $opt_f if defined($opt_f);
$NODE 	= $opt_n if defined($opt_n);
$USER 	= $opt_u if defined($opt_u);
$OLD    = $opt_o if defined($opt_o);

# &gar : CTS 1103611
if ($opt_s)
{
	die "istat: filename necessary for option -s\n$USAGE" if (! $FILE );
	system ("$TOOL/bin/istatus.exe $FILE");
}
else
{
	die "istat: illegal argument for option -d\n$USAGE" if $DATE !~ /^\d*$/;
	die "istat: illegal argument for option -j\n$USAGE" if $DAYS !~ /^\d*$/;
	die "istat: illegal argument for option -t\n$USAGE" if $TIME !~ /^\d*$/;
	$LD = length($DATE);
	$LJ = length($DAYS);
	$LT = length($TIME);
	die "time formats are: HH or HHMM or HHMMSS\n" if $LT % 2;
	die "date formats are: DD or MMDD or YYYYMMDD\n" if $LD % 2;
	$Year_index = \5;	#constant
	$Month_index = \4;	#constant
	$Day_index = \3;	#constant
	@localtime = localtime(); $localtime[$$Month_index]++; $localtime[$$Year_index] += 1900;
	$localtime[$$Day_index] = "0$localtime[$$Day_index]" if $localtime[$$Day_index] < 10;
	$localtime[$$Month_index] = "0$localtime[$$Month_index]" if $localtime[$$Month_index] < 10;

	# compute the start value of the date field from the -d option:

	if ( $LD > 0 ) {
		if ( $LD == 2 ) {
			$DATE = $localtime[$$Year_index].$localtime[$$Month_index].$DATE;
		} elsif ( $LD == 4 ) {
			$DATE = $localtime[$$Year_index].$DATE;
		}
		$DATE =~ /(\d\d\d\d)(\d\d)(\d\d)/;
		die "Hey, how many month has a year?\n" if $2 > 12;
		die "Hey, how many days has a month?\n" if $3 > 31;
		$DATE = "$1-$2-$3";
	}

	# compute the start value of the date field from the -j option:

	$TODAY = "$localtime[$$Year_index]-$localtime[$$Month_index]-$localtime[$$Day_index]";
	if ( $LJ > 0 ) {
		$Y = $localtime[$$Year_index];
		$M = $localtime[$$Month_index];
		$D = $localtime[$$Day_index];
		while ( $DAYS > 0 ) {
			if ( $D > $DAYS ) {
				$D -= $DAYS;
				$DAYS = 0;
			} else {
				$DAYS -= $D;
				if ( $M > 1 ) { $M-- }
				else { $Y--; $M = 12 }

				CASE: {
					if ( $M =~ /^(1|3|5|7|8|10|12)$/ ) { $D = 31; last CASE }
					if ( $M =~ /^(4|6|9|11)$/ ) { $D = 30; last CASE }
					if ( $M =~ /^2$/ ) {
						if ( $Y % 4 == 0 ) { $D = 29 } else { $D = 28 }
						last CASE;
					}
				} #CASE
			}
		} #while
		$D = "0$D" if length($D) < 2;
		$M = "0$M" if length($M) < 2;
		$DATE = "$Y-$M-$D";
	}

	# include the value of the -t option into the date field

	if ( $LT > 0 ) {
		if ( $LT == 2 ) {
			$TIME = "$TIME:00:00";
		} elsif ( $LT == 4 ) {
			$TIME =~ /(\d\d)(\d\d)/;
			$TIME = "$1:$2:00"
		} else {
			$TIME =~ /(\d\d)(\d\d)(\d\d)/;
			$TIME = "$1:$2:$3";
		}

	}

	$DATE = "1997-10-01" if ( ! defined $DATE && defined $OLD );
	$DATE = $TODAY unless defined $DATE;

	CASE: {
		if ( $ACTION =~ /^(l|g)/i ) { $ACTION = "LOCKED";   last CASE; }
		if ( $ACTION =~ /^(u|p)/i ) { $ACTION = "UNLOCKED"; last CASE; }
		if ( $ACTION =~ /^d/i )     { $ACTION = "DEL_LOCK"; last CASE; }
	}

	#($grp) = $GRP =~ m!^(\\\\\w+\\\w+[^\\]*)!;
	($grp) = $GRP =~ m!(.*)\\[a-z/]+$!i;
	unless (defined $grp) {
		# $GRP liegt nicht in UNC vor
		#($grp) = $GRP =~ m!^(\w+:[^\\]*)! ;
		($grp) = $GRP =~ m!^(\w+:.*)\\! ;
	}
	$statfile = "$grp/$statfilename";
	$oldstatfile = "$grp/$oldstatfilename";

    if ( -f $oldstatfile ) {

	if ( ($DATE lt $CHANGEDATE) ||
         (($DATE eq $CHANGEDATE) && ( $TIME < $CHANGETIME )) )
	{
		open(FILE_IN, "$oldstatfile" ) || die "Can't open $oldstatfile: $!\n";
		while(<FILE_IN>) {
			$printit = 0;
			/(.*?) (.*?)  (.*?) (.*)\((.*):(.*)\)/;
			# $1 timestamp, $1 date, $2 time, $3 file, $4 status $5 Rechner, $6 duser
			local($date, $time, $file, $status, $Rechner, $duser);
			$date= $1; $time = $2; $file = $3; $status = $4; $Rechner = $5, $duser = $6;
			if ( $DATE le $date ) {
				$printit = 1;
				if ( $TIME ) {
					$printit = 0 unless $TIME le $time;
				}
				if ( $NODE ) {
					$printit = 0 unless $Rechner =~ /$NODE/i;
				}
				if ( $USER ) {
					local(@duser);
					if ( $USER =~ /^d\d{6}/i ) {
						@duser = ( $USER );
					} else {
						@duser = DUser::userlookup($USER);
					}
					local($users) = join "|", @duser;
					$printit = 0 unless $duser =~ /$users/i;
				}
				if ( $ACTION ) {
					$printit = 0 unless $status =~ /^$ACTION/;
				}
				if ( $FILE ) {
					$printit = 0 unless $file =~ /$FILE/i;
				}
			}
			if ( $printit ) {
				if (length($file) < 12 ) { $file = "$file\t"; }
				local($user) = DUser::userlookup($duser);
				print ("$date $time $file \t$status($Rechner:$user)\n");
			}
		}
		close(FILE_IN);
	}
    }

	open(FILE_IN, "$statfile" ) || die "Can't open $statfile: $!\n";
	while(<FILE_IN>) {
		$printit = 0;
		/(.*?) (.*?)  (.*?) (.*)\((.*):(.*)\)/;
		# $1 timestamp, $1 date, $2 time, $3 file, $4 status $5 Rechner, $6 duser
		local($date, $time, $file, $status, $Rechner, $duser);
		$date= $1; $time = $2; $file = $3; $status = $4; $Rechner = $5, $duser = $6;
		if ( $DATE le $date ) {
			$printit = 1;
			if ( $TIME ) {
				$printit = 0 unless $TIME le $time;
			}
			if ( $NODE ) {
				$printit = 0 unless $Rechner =~ /$NODE/i;
			}
			if ( $USER ) {
				local(@duser);
				if ( $USER =~ /^d\d{6}/i ) {
					@duser = ( $USER );
				} else {
					@duser = DUser::userlookup($USER);
				}
				local($users) = join "|", @duser;
				$printit = 0 unless $duser =~ /$users/i;
			}
			if ( $ACTION ) {
				$printit = 0 unless $status =~ /^$ACTION/;
			}
			if ( $FILE ) {
				$printit = 0 unless $file =~ /$FILE/i;
			}
		}
		if ( $printit ) {
			if (length($file) < 12 ) { $file = "$file\t"; }
			local($user) = DUser::userlookup($duser);
			print ("$date $time $file \t$status($Rechner:$user)\n");
		}
	}
	close(FILE_IN);
}

__END__
