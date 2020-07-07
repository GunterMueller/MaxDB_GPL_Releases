#!/usr/bin/perl
# -*-Fundamental-*-

# $Id: //depot/user/p4/bin/p4pr#15 $

# Interpolate change information into a source listing of a p4 file.
# Takes a file name or depot filename, with #<ref> or @<change>.
# Contributed by Bob Sidebotham.
#
# Mods for Netapp "b4p4" wrapper use and support for filelog -i added
# by Richard Geiger.
#

# Netapp-isms:
#
#  Set up "$P4", the "p4" path we'll use
#
if (! -d "/u/p4/VERS")
  {
    $P4 = "p4";

    #  If your site uses a standard "P4CONFIG" name, and your users
    #  don't all define it in their own environments, you might want
    #  to set it here
    #
    #$ENV{"P4CONFIG"} = ""; 
  }
else
  {
    # Looks like we're at NetApp...
    #
    sub nobin
    {
      print STDERR "$Myname: I don't know how to run on this \"$Osname/$Osvers\" host.\n";
      exit 1;
    }

    ($Osname, $Hostname, $Osvers) = split(/\s+/, `/bin/uname -a`);
    $Hostname =~ s/\..*//;

    if ($Osname eq "SunOS")
      {
        if ($Osvers =~ /^5\.5\b/)
          { $bin = "solaris25"; }
        elsif ($Osvers =~ /^5\.[67]\b/)
          { $bin = "solaris26"; }
        elsif ($Osvers =~ /^4\.1\./)
          { $bin = "sunos"; }
        else
          { &nobin; }
      }
    elsif ($Osname eq "OSF1" && $Osvers =~ /^V4\./)
      { $bin = "osf"; }
    elsif ($Osname eq "Linux")
      { $bin = "linuxx86"; }
    elsif ($Osname eq "HP-UX")
      { $bin = "hpux"; }
    else
      { &nobin; }

    $P4 = "/u/p4/VERS/bin.$bin/p4";

    $ENV{"P4CONFIG"} = "P4ENV";

    $pwd = `/bin/pwd`; chomp $pwd;
    $ENV{"PWD"} = $pwd;
  }

# Simplify program name, if it is a path.
$0 =~ s#.*/##;

# Execute a command, keeping the output of the command in an array.
# Returns the array, unless an error occured, in which case the an
# exception is thrown (via die) with an appropriate message.
sub command {
    my($command) = @_;
    my(@results) = `$command`;
    if ($?) {
	my($err) = ($? >> 8);
	print STDERR @results;
	die qq($0: "$command" exited with status $err.\n);
    }
    @results
}

# Fatal usage error
sub usage {
    my($err) = @_;
    die
      "$0: $err\n" .
      "usage: $0 [-i|-b] [-n <nrevs> [-d]] [-g <grevs>] [-m <pattern>] <file> | <file>#<rev> | <file>\@<change>\n" .
      " -i [default]   follow history back across branches.\n" .
      " -c             follow history back across \"copy from\" integrations.\n" .
      " -b             use history from this branch only (faster, but less informative).\n" .
      " -g <grevs>     perform diffs at a granularity of every <grevs> revisions.\n" .
      " -n <nrevs>     only gather history <nrevs> revisions back from the requested revision.\n" .
      " -d (with -n)   show only lines changed in the requested revision range.\n" .
      " -m <pattern>   only show lines matching <pattern> (Perl regular expressions).\n" .
      " <file> may be a client file name or depot file name.\n";
}

$dashi = '-i';

# Parse options
while (@ARGV && $ARGV[0] =~ /^-/) {
    $opt = shift;
    if ($opt eq '-i') {
	$dashi = '-i'; # Follow history beyond current branch
    } elsif ($opt eq '-help') {
	usage(""); 
    } elsif ($opt eq '-b') {
	$dashi = '';   # Don't follow history beyond current branch
    } elsif ($opt eq '-d') {
	$chonly = 1;   # Show changed lines only
    } elsif ($opt eq '-c') {
	$followcp = 1; # Follow back through the other branch from copies.
    } elsif ($opt eq '-m') {
	if ($#ARGV == -1) {
          usage("-m option needs an argument");
        }
	$pattern = shift;
        "" =~ /$pattern/; # make sure perl likes the regexp!
    } elsif ($opt eq '-n') {
	if ($#ARGV == -1) {
          usage("-n option needs an argument");
        }
	if (($nrevs = shift) !~ /^\d+/) {
          usage("-n option needs a numeric argument");
        }
	if ($nrevs <= 0) {
          usage("-n argument must be > 0");
        }
    } elsif ($opt eq '-g') {
	if ($#ARGV == -1) {
          usage("-g option needs an argument");
        }
	if (($grevs = shift) !~ /^\d+/) {
          usage("-g option needs a numeric argument");
        }
	if ($grevs <= 0) {
          usage("-g argument must be > 0");
        }
    } else {
	usage("invalid option $opt"); 
    }
}

# Get file argument.
usage("file name expected") if !@ARGV;
usage("invalid argument") if @ARGV > 1;

# Filearg is the original user file argument (with rev or change)
$filearg = $file = shift;

if ($filearg eq "help") {
  print "[For help use \"p4p4 -help\"]\n";
}

# Check that the file specification maps to exactly one file.
@list = command qq($P4 files $filearg);
if (@list > 1) {
    die("$0: the specified file pattern maps to more than one file.\n");
}

# Check that the revision is not deleted.
if ($list[0] =~ /(.*#\d+) - delete change/) {
    die("$0: revision $1 is deleted.\n")
}

@have = `$P4 have $filearg 2>&1`;
if ($have[0] =~ /^(\/\/.*) - /) {
  $file = $1;
}  

# Handle # and @ notation (only for numeric changes and revisions).
$change = $1 if $file =~ s/@(\d+)//;
$head = $1 if $file =~ s/#(\d+)//;


# Get the fullname of the file and the history, all from
# the filelog for the file.
(@history) = command qq($P4 filelog $dashi $file);

my %files;
my @files;

# Extract the revision to change number mapping. Also
# get the author of each revision, and for merged
# or copied revisions, the "branch name", which we
# use instead of an author.

hist:
while ($#history >= 0)
  {
    while ($_ = shift(@history)) {
      if (/^\/\//) {
          $thisfile = $_;
          chop $thisfile;
          if (! defined($files{$thisfile})) {
            push(@files, $thisfile);
            $files{$thisfile} = $#files;
          }
      } elsif (/^\.\.\. #(\d+) change (\d+) (.*?) on .*? by (.*?)@/) {
  	# If a change number or revision is specified, then ignore
  	# later revisions.
  	next if $change && $change < $2;
  	next if ((! $headseen) && $head && ($head < $1));
        if ($dashi && $3 eq 'branch') {
	  $head = ""; # In case $head = #1, where #1 was a branch
	  next;
        }
  	# If we see a change that deleted the file, then
          # we know that previous revisions did not contribute.
          last if $3 eq "delete";

  	$change{"$thisfile#$1"} = $2;
        $author{"$thisfile#$1"} = $4;

        if ($nrevs && $#revs == ($nrevs-1)) {
           $author{"$thisfile#$1"} = "(base)";
        }
        $head = $1 if !$head;
        unshift(@revs, "$thisfile#$1");
        if ($nrevs && $#revs == $nrevs) {
           last hist;
        }
  	$thisrev = $1;
  	$headseen = 1;
      } else {
  	# If we see a branch from, then we know that
  	# previous revisions did not contribute to the current
  	# revision. Don't do this, however, if we haven't seen
  	# the revision we've been requested to print, yet.
  	# We used to do this for copy from, but I think
  	# it's better not to.
  	next unless $headseen;
  	if (/^\.\.\. \.\.\. (copy|branch|merge) from (\/\/[^#]*).*#([0-9]+)/) {
          my $fromfile = "$2#$3";
          # If merged or copied from another part of the
          # tree, then we use the first component of the
          # name that is different, and call that the "branch"
          # Further, we make the "author" be the name of the
          # branch.
          my($type) = $1;
          # If branched, we don't bother getting any more
          # history. We treat this as starting with the branch.
          if (! $dashi) {
	      @history = ();
              $author{"$thisfile#$thisrev"} = "($type)";
              last;
          }
          if ($type eq "copy" && $followcp) {
            (@history) = command qq($P4 filelog $dashi $fromfile);
            last;
          }
  	}
      }
    }
  }

foreach $r (@allrevs)
{ print $r."\n"; }

if ($grevs) {
    my @grevs;
    for (my $i = $#revs; $i >= 0; $i -= $grevs) { unshift(@grevs, $revs[$i]); }
    @revs = @grevs;
    }

# Get first revision, and list of remaining revisions
($base, @revs) = @revs;

# Get the contents of the base revision of the file,
# purely for the purposes of counting the lines.
@text = command qq($P4 print -q $base);

# For each line in the file, set the change revision
# to be the base revision.
@lines = ($base) x @text;

# For each revision from the base to the selected revision
# "apply" the diffs by manipulating the array of revision
# numbers. If lines are added, we add a corresponding 
# set of entries with the revision number that added it.
# We ignore the actual revision text--that will be merged
# with the change information later.
$prevrev = $base;

for $rev (@revs) {
    # Apply the diffs in reverse order to maintain correctness
    # of line numbers for each range as we apply it.
    for (reverse command qq($P4 diff2 $prevrev $rev)) {
	my( $la, $lb, $op, $ra, $rb ) = /^(\d+),?(\d*)([acd])(\d+),?(\d*)/;
	next unless defined($ra);
	$lb = $la if ! $lb;
	++$la if $op eq 'a';
	$rb = $ra if ! $rb;
	++$ra if $op eq 'd';
	splice @lines, $la - 1, $lb - $la + 1, ($rev) x ($rb - $ra + 1);
    }
  $prevrev = $rev
}
			
# Get the text of the selected revision. The number of lines
# resulting from applying the diffs should equal the number of
# of lines in this revision.
($header, @text) = command qq($P4 print $prevrev);
if (@text != @lines) {
    die("$0: internal error applying diffs - please contact the author\n")
}

my $dokeys = ($#files > 0);

if ($dokeys) {
    # Print a legend showing the correspondence between "file keys" and
    # file pathnames in the depot.
    my($fmt) = "%5s %s";
    @fields = ("  key", "file". ' ' x 66);
    printf("$fmt\n", @fields);
    printf("$fmt\n", map('-' x length($_), @fields));
    for (my $i = 0; $i <= $#files; $i++) {
      printf("$fmt\n", $i, @files[$i]);
    }
    print "\n";
}

# Print a pretty header. Note that the interpolated information
# at the beginning of the line is a multiple of 8 bytes (currently 24)
# so that the default tabbing of 8 characters works correctly.
my($fmt) = "%5s %10s %6s %6s %s";
@fields = (" line", "    author", "change", "   rev", $header);
printf($fmt, @fields);
printf("$fmt\n", map('-' x length($_), @fields));

# Interpolate the change author and number into the text.
my($line) = 1;
my($inbase) = 0;
while (@text) {
    my($revkey) = shift(@lines);
    my($path, $rev) = split(/#/, $revkey);    
    $rev = ($dokeys ? "$files{$path}" : ""). "#" . $rev;
    my $text = shift @text;
    if (($chonly && $author{$revkey} eq "(base)") || ($pattern && $text !~ /$pattern/)) {
      if (! $inbase) {
	  print ".....\n";
          $inbase = 1;
      }
    } else {
        if ($grevs) { $author{$revkey} = "*"; }
        printf($fmt, $line, $author{$revkey}, $change{$revkey}, $rev, $text);
        $inbase = 0;
    }
    $line++;
}

