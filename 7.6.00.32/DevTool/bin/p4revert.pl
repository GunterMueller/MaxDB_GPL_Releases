#!/usr/bin/perl
# p4revert
#
#
# Copyright (c) 2000, Sandy Currier (sandy@releng.com)
# Distributed under the GNU GENERAL PUBLIC LICENSE:
#
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation; either version 1, or (at your option)
#      any later version.
#
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#
#      You should have received a copy of the GNU General Public License
#      along with this program; if not, write to the Free Software Foundation,
#      Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
#

#
# Unbuffer STDERR and STDOUT
select(STDERR);
$| = 1;                 # Make STDERR be unbuffered.
select(STDOUT);
$| = 1;                 # STDOUT too so, they can mix.

#
# set up some globals
# Note: assume that the PATH EV is going to be used to find p4
$ThisCmd = "p4revert.pl";       # this command name
$debug = 0;
$P4 = "p4";                     # the p4 command to execute (can either be
                                # absolute or relative)
$vb = ">>>";
$err = "***";
$printonly = 0;
$verbose = 1;

#
# local variables
@info = ();                     # the output of p4 info
$clientname = "";               # the "Client name: "
$clientroot = "";               # the "Client root: "
$cwd = "";                      # the current working directory
@Changes = ();                  # the list of changes
@RealFiles = ();                # the list of real files
$all_p = 0;                     # weird switch
$regexp = "";                   # a regexp to match files against...

#
# now parse any args
# the usage message (for -h or on error)
$help = "$ThisCmd changelist
Function:
    $ThisCmd will revert a changelist.  It will back out the changelist
    with the following caveats: added files will be deleted unless
    the file has been edited since it was added (in which case it is
    skipped); deleted files will be re-constituted from the deleted
    version; and edited files will be by auto-merged (if possible)
    to the head of the codeline.  If there is a conflict, it will be
    left unresolved.

    By default, will iterate over each file specified in the
    changelists (see the -all option below).

    All changes are left checked out and must be submitted by hand.

Args:
    \@changelist ...  the single change to backout
                      
Switches/Options:
    -h               Prints this help message
    -all             If specified, will not iterate the various
                     p4 commands over the files contained in
                     the specified change number, but instead will
                     run it over the entire client spec.  This is
                     basically a manual performance tradeoff switch.
    -regexp          A perl regular expression to match files from
                     the supplied changelists.  If supplied, only
                     those files that match will be reverted.
";

#
# parse command line
{
    my($i, $param);
    while($i <= $#ARGV) {
        # scan for a help switch
        if ($ARGV[$i] =~ /^-h/i) {
            &DieHelp("", $help);
        }
        # scan for switches
        elsif ($ARGV[$i] =~ /^-debug/i) {
            $debug = 1;
            $i++;
        }
        elsif ($ARGV[$i] =~ /^-all/i) {
            $all_p = 1;
            $i++;
        }
        elsif ($ARGV[$i] =~ /^-n/i) {
            $printonly = 1;
            $i++;
        }
        # scan for variable definitions (-variable value)
        elsif ($ARGV[$i] =~ /^-\w+/ and defined($ARGV[$i+1]) and $ARGV[$i+1] !~ /^-[^-]/) {
            # NOTE: nt has a difficult time with '=' on a command line...
            # process any variable value switches
            my($var) = $ARGV[$i];
            $var =~ s/^-//;
            my($value) = $ARGV[$i+1];
            if (defined $$var) {
                $$var = $value;
            }
            else {
                &DieHelp("Unknown parameter '$var'\n", $help);
            }
            $i=$i+2;
        }
        # catch unsupported switches
        elsif ($ARGV[$i] =~ /^-/) {
            &DieHelp("Unsupported switch \"$ARGV[$i]\"\n", $help);
        }
        else {
            # swallow files or a changeset
            if ($ARGV[$i] =~ /^\@\S+/) {
                my($tmp) = $ARGV[$i];
                $tmp =~ s|^\@||; # remove the @
                push(@Changes, $tmp);
            }
            else {
                # a change without a @
                push(@Changes, $ARGV[$i]);
            }
            $i++; $param++;
        }
    }
}


#
# if debugging, re-arrange variables
if ($debug) {
    $verbose = 2;
}


#
# Note: if there is no args, prompt for an input (explorer launch support)
if ($#Changes < 0) {
    &TheEnd(0);
}


#
# make sure that a valid client is selected
{
    my($client_string) = "Client name: ";
    my($root_string) = "Client root: ";
    my($cwd_string) = "Current directory: ";
    my(@tmp);
    @info = &ExecuteP4Cmd("$P4 info", $verbose, 1);
    if ($?) {
        die "$ThisCmd - could not execute '$P4 info'\n$info";
    }
    chomp(@info);
    # now get client name
    @tmp = grep(/^$client_string/,@info); # grep out the client name
    $clientname = &other2unix($tmp[0]); # transfer to a scalar
    $clientname =~ s/^$client_string//; # ditch the uninteresting part
    if ($clientname eq "") {    # check things
        die "$ThisCmd - something wrong - no client name found from p4 info output";
    }
    # get the client root
    @tmp = grep(/^$root_string/,@info); # grep out the client name
    $clientroot = &other2unix($tmp[0]); # transfer to a scalar
    $clientroot =~ s/^$root_string//; # ditch the uninteresting part
    if ($clientroot eq "") {    # check things
        die "$ThisCmd - something wrong - no client name found from p4 info output";
    }
    # get the cwd
    @tmp = grep(/^$cwd_string/,@info); # grep out the client name
    $cwd = &other2unix($tmp[0]);        # transfer to a scalar
    $cwd =~ s/^$cwd_string//; # ditch the uninteresting part
    if ($cwd eq "") {   # check things
        die "$ThisCmd - something wrong - no cwd found from p4 info output";
    }
}


#
# before continuing, grab the already opened files for statistics
{
    @oldfiles = &ExecuteP4Cmd("$P4 opened", $verbose);
    chomp(@oldfiles);
    # if no files, will get "File(s) not opened on this client"
    if ($oldfiles[0] =~ /^File\(s\) not opened on this client/) {
        undef @oldfiles;
    }
}


#
# handle multiple changes
foreach my $change (@Changes) {
    my($tmp, $script, @output, $line);
    my($DescribeCmd) = "$P4 describe -s"; # this gets the filelist from the changelist if supplied
    # describe the changeset
    # get the file list with revision number
    @output = &ExecuteP4Cmd("$DescribeCmd $change", $verbose, 1);
    # see if @output contains a files
    chomp(@output);
    foreach $line (@output) {
        # if not a valid file line, punt and go to next one
        next unless ($line =~ /^\.\.\. \/\//);
        # push the complete file spec into the array
        $line =~ s|^\.\.\. ||;  # remove beginning text
        # note: if $regexp, then match
        next if ($regexp and ($line =~ /$regexp/));
        push(@RealFiles, $line);
        my($file, $revision, $action) = &SplitFilename3($line);
        push(@JustFiles, $file);
        # note: the first action is for work done within the codeline
        #       the second action is for merged work of some type...
        if ($action eq "edit" or $action eq "integrate") {
            push(@EditFiles, $file);
        }
        elsif ($action eq "add" or $action eq "branch") {
            my(@tmp);
            # note: if the current head of the file is not revision 1, then
            # it has been edited.  If so, just warn and do not delete...
            @tmp = &ExecuteP4Cmd("$P4 files \"$file\"");
            # ignore errors?
            my($f, $r) = &SplitFilename3($tmp[0]);
            if ($r and $r ne "1") {
                &PrintWarning("Warning: the added file \"$file\",
is no longer at revision 1; it will not be deleted");
                push(@SkippedFiles, $file);
            }
            else {
                push(@AddFiles, $file);
            }
        }
        elsif ($action eq "delete") {
            push(@DeleteFiles, $file);
        }
        else {
            &PrintError("unknown action for line\n$line");
        }
    }
}


#
# Note: the file list is now known.
# perform the sequence of commands to backout this change
{
    &PrintMessage("\nAbout to backout change $Changes[0]:\n");
}


#
# now back out the change
{
    my($tmp, @output, $script, $previous, $file);
    $previous = $Changes[0] - 1;
    # this is executed after all the integrates have been performed
    if ($all_p) {
        @output = &ExecuteP4Cmd("$P4 sync \@$previous", $verbose);
    }
    else {
        foreach my $file (@JustFiles) {
            @output = &ExecuteP4Cmd("$P4 sync \"$file\@$previous\"", $verbose);
        }
    }
    # now loop over all edits
    &PrintMessage("checking out files for edit (or add)...") if ($verbose);
    foreach $file (@EditFiles) {
        @output = &ExecuteP4Cmd("$P4 edit \"$file\"");
    }
    # now loop over all deleted files and add them back in
    foreach $file (@DeleteFiles) {
        @output = &ExecuteP4Cmd("$P4 add \"$file\"");
    }
    # now do second sync
    if ($all_p) {
        @output = &ExecuteP4Cmd("$P4 sync \@$Changes[0]", $verbose);
    }
    else {
        foreach my $file (@JustFiles) {
            @output = &ExecuteP4Cmd("$P4 sync \"$file\@$Changes[0]\"", $verbose);
        }
    }
    # now do resolve -ay (keep the backed out changes)
    @output = &ExecuteP4Cmd("$P4 resolve -ay", $verbose);
    # now sync to head
    if ($all_p) {
        @output = &ExecuteP4Cmd("$P4 sync", $verbose);
    }
    else {
        foreach my $file (@JustFiles) {
            @output = &ExecuteP4Cmd("$P4 sync \"$file\"", $verbose);
        }
    }
    # now do resolve (this could result in conflicts...)
    @output = &ExecuteP4Cmd("$P4 resolve -am", $verbose, 1);
    # delete those files that were added in the changeset
    foreach $file (@AddFiles) {
        @output = &ExecuteP4Cmd("$P4 delete \"$file\"", $verbose);
        # ignore errors?
    }
    # never script a submit
}

#
# now print something
{
    my($tmp, $string);

    # print some statistics?...
    @newfiles = &ExecuteP4Cmd("$P4 opened", $verbose);
    chomp(@newfiles);
    # compare the old with the new, and print something
    # construct a hash
    # if no files, will get "File(s) not opened on this client"
    if ($newfiles[0] =~ /^File\(s\) not opened on this client/) {
        undef @newfiles;
    }
    else {
        foreach (@newfiles) {
            # let the entire string be the key
            if (!defined($new{$_})) {
                $new{$_} = 1;
            }
        }
        # delete from the hash anything that matches from @oldfiles
        foreach (@oldfiles) {
            if (defined($new{$_})) {
                delete $new{$_};
            }
        }
    }
    $string = sprintf "edited  %3d file(s)", $#EditFiles+1;
    &PrintNote($string);
    $string = sprintf "added   %3d file(s)", $#DeleteFiles+1;
    &PrintNote($string);
    $string = sprintf "deleted %3d file(s)", $#AddFiles+1;
    &PrintNote($string);
    $string = sprintf "skipped %3d file(s)", $#SkippedFiles+1;
    &PrintNote($string);
    $string = sprintf "        %3d total files", ($#RealFiles+1); 
    &PrintNote($string);
}

#
# the end
&TheEnd();

#
# subroutines (these should come from an include file, but not
# enough time now to set it up)
#

# will convert a random OS delimited pathname to a perl pathname
sub other2unix {
    my($filename) = @_;
    my($pattern) = $Platform{'pd'};
    $pattern =~ s/(\W)/\\$1/g;  # escape wildchars
    $filename =~ s|$pattern|/|g;
    return("/") if ($filename =~ /^\/+$/); # if just /+, return just /
    if ($filename =~ /^\/\//) {
        # add them back in later
        $filename =~ s|/+|/|g;  # remove doubles
        $filename = "/$filename";
    }
    else {
        $filename =~ s|/+|/|g;  # remove doubles
    }
    # remove trailing
    $filename =~ s|/+$||;
    return($filename);
}

sub DieHelp {
    my($str, $help) = @_;
    print STDERR "$err $str\nUsage: $help";
    exit(2);
}

#
# Note: this will actually execute any command...
# returns the action of the revision of the specified file#revision
sub ExecuteP4Cmd {
    my($script, $verbose, $print_output, $no_error_check, $stream_p) = @_;
    my(@output);
    if ($stream_p) {
        print $stream_p "$vb\n$vb running: $script\n$vb\n" if ($verbose);
    }
    else {
        print STDOUT "$vb\n$vb running: $script\n$vb\n" if ($verbose);
    }
    if (!$Platform{'nt'} and $Platform{'os'} eq "win32") {
        @output = `$script` unless ($printonly);
    }
    else {
        @output = `$script 2>&1` unless ($printonly);
    }
    if ($stream_p) {
        print $stream_p "@output" if ($print_output);
    } else {
        print STDOUT "@output" if ($print_output);
    }
    if (!$no_error_check and $?) {
        # now what - just keep going
        &PrintError("$ThisCmd - something happened with '$script'\n$?", $stream_p);
    }
    return(@output);
}

# can handle, somewhat, either # or @...
# Note: the output of a 'p4 change ...' will not be of the form
# ... //depot/main/scm/tests/bar#4 edit
# ... //depot/main/scm/tests/xxx#1 add
# ... //depot/main/scm/tests/zzz#1 add
#
# the output of s 'p4 files ...' will be something like
# //depot/main/scm/tests/foo#4 - edit change 1833 (text)
# try to handle both here...
sub SplitFilename3 {
    my($thing) = @_;
    my($f, $tmp, $r, $a, $d, $junk);
    if ($thing =~ /\#/){
        ($f, $tmp) = split('#', $thing);
        $d = "\#";
    }
    elsif ($thing =~ /\@/) {
        ($f, $tmp) = split('@', $thing);
        $d = "\@";
    }
    else {
        # hoping that the thing passed in is really a file...
        $f = $thing;
    }
    return($f, $r, $a, $d) unless ($tmp); # if empty $tmp, just return now
    if ($tmp =~ / - /) {
        ($r, $a) = split(/ - /, $tmp); # split on the first ' - ' (here's hoping again)
    }
    else {
        # if no ' - ', split on first space...
        ($r, $a) = split(/ /, $tmp);
    }
    ($a, $junk) = split(' ', $a); # just use first word
    return($f, $r, $a, $d);
}

# should not be called by a server
sub TheEnd {
    my($tmp);
    print STDERR "$err exiting with $Error{'Errors'} Error(s) & $Error{'Warnings'} Warning(s)\n";
    # exit with the number of errors in the bottom 16 bits
    # and the number of warnings in the top
    # Note: make sure that if things shift off, that error is at least still set
    $tmp = $Error{'Warnings'} << 16;
    $tmp |= $Error{'Errors'};
    # explicitly set $! to the explicit value
    # see the documentation on die
    exit($tmp);
}

sub PrintError {
    my($text, $stream_p) = @_;
    my($tmp);
    # first, increment error count
    $Error{'Errors'}++;
    # make sure $? is set
    $? = 1;
    # prepend with the correct prefix
    $text =~ s/^(.*)$/$err $1/gm;
    # store error away
    push(@{$Error{'ErrorSummary'}}, $text);
    # add a \n
    $text = "$text\n";
    # print and log (maybe)
    if ($stream_p) {
        print $stream_p "$text";
    }
    else {
        print STDERR "$text";
    }
    $LogOutput = "$LogOutput$text" if (defined($LogOutput));
    return($tmp);
}

# will increment $Error{'Warnings'} and append $err to every line
sub PrintWarning {
    my($text, $stream_p) = @_;
    my($tmp);
    # first, increment warning count
    $Error{'Warnings'}++;
    # prepend with the correct prefix
    $text =~ s/^(.*)$/$err $1/gm;
    # store error away
    push(@{$Error{'WarningSummary'}}, $text);
    # add a \n
    $text = "$text\n";
    # print and log (maybe)
    if ($stream_p) {
        print $stream_p "$text";
    }
    else {
        print STDERR "$text";
    }
    $LogOutput = "$LogOutput$text" if (defined($LogOutput));
    return($tmp);
}

# will append $vb to every line
sub PrintMessage {
    my($text, $stream_p) = @_;
    my($tmp);
    # prepend with the correct prefix
    $text =~ s/^(.*)$/$vb $1/gm;
    # add a \n
    $text = "$text\n";
    # print and log (maybe)
    if ($verbose) {
        if ($stream_p) {
            print $stream_p "$text";
        }
        else {
            print STDOUT "$text";
        }
    }
    $LogOutput = "$LogOutput$text" if (defined($LogOutput));
    return($tmp);
}

# will append $err to every line (but not set or increment any error variables)
sub PrintNote {
    my($text, $stream_p) = @_;
    my($tmp);
    # prepend with the correct prefix
    $text =~ s/^(.*)$/$err $1/gm;
    # add a \n
    $text = "$text\n";
    # print and log (maybe)
    if ($stream_p) {
        print $stream_p "$text";
    } 
    else {
        print STDERR "$text";
    }
    $LogOutput = "$LogOutput$text" if (defined($LogOutput));
    return($tmp);
}
