#!/usr/bin/perl
#
# ixterm.pm
# cba 99/01/13
#     split ixterm.pl into this package and a caller ixterm.pl
#     ixterm is now availbale as a function from perl
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


package ixterm;
use Env;
use Getopt::Long;

sub ixterm {
    @ARGV = @_;
    if (-d "/usr/openwin/bin/") {
        # we are on a sun
        $xterm_dir = "/usr/openwin/bin/";
    } else {
        $xterm_dir = "/usr/bin/X11/";
    }

    if (-x $xterm_dir."ansi_xterm") {
        $xterm = $xterm_dir."ansi_xterm"
    } elsif (-x $xterm_dir."xterm") {
        $xterm = $xterm_dir."xterm";
    } else {
        die "executable ".$xterm_dir."xterm not found\n";
    }

    $sb = $cr = $fn = $fg = $bg = $display = $bash = undef;

    @tmp=@ARGV;
    @ARGV=();
    @cmd_opts=();
    foreach (@tmp) {
        if ($#cmd_opts == -1 && $_ ne "-e") {
                push @ARGV, $_;
        } else {
                push @cmd_opts, $_;
        }
    }

    GetOptions ('cr:s', 'fn:s', 'fg:s', 'bg:s', 'display:s', 'sb!', 'bash!');

    # get default from environment
    $display = $ENV{'DISPLAY'} if (defined $ENV{'DISPLAY'});
    $cr = $ENV{'CR'} if (defined $ENV{'CR'});
    $fn = $ENV{'FN'} if (defined $ENV{'FN'});
    $fg = $ENV{'FG'} if (defined $ENV{'FG'});
    $bg = $ENV{'BG'} if (defined $ENV{'BG'});
    $sb = true if (defined $ENV{'SB'});

    # and then ixterm options
    $display = $opt_display if (defined $opt_display);
    $cr = $opt_cr if (defined $opt_cr);
    $fn = $opt_fn if (defined $opt_fn);
    $fg = $opt_fg if (defined $opt_fg);
    $bg = $opt_bg if (defined $opt_bg);
    $sb = true if (defined $opt_sb);
    $bash = true if (defined $opt_bash);

    @ARGV = $xterm;
    if (defined $display) {
        push @ARGV, "-display", $display;
    }
    if (defined $cr) {
        $ENV{'CR'}=$cr;
        push @ARGV, "-cr", $cr;
    }
    if (defined $fn) {
        $ENV{'FN'}=$fn;
        push @ARGV, "-fn", $fn;
    }
    if (defined $fg) {
        $ENV{'FG'}=$fg;
        push @ARGV, "-fg", $fg;
    }
    if (defined $bg) {
        $ENV{'BG'}=$bg;
        push @ARGV, "-bg", $bg;
    }
    if (defined $sb) {
        $ENV{'SB'}=$sb;
        push @ARGV, "-sb";
    }

    # are we running 'bash' ?
    if (get_ppid_cmd () =~ /bash$/ ) {
        $bash = true;
    }

    if (defined $bash) {
        $_ = shift @cmd_opts;
        push @ARGV, "-e", "bash";
        push @ARGV, "-c", @cmd_opts if ($#cmd_opts != -1);
    } else {
        push @ARGV, @cmd_opts;
    }
    return start_job (@ARGV);
}

#
# create a child process from argument list
#
sub start_job {
        $pid = fork;
        die "fork failed\n" if (! defined $pid);
        exec @_ if ($pid == 0);
	return $pid;
}

#
# get the command name of the parent process
# we use the output of ps
# this is a bit tricky, but it works under all :-) UNIXen
#
sub get_ppid_cmd {
        $ppid = getppid ();
		$PSCMD = "ps -p $ppid |";
        open (PIPE_IN, $PSCMD);
        $first_line = true;
        while (<PIPE_IN>) {
                chomp $_;
                @ps = split ' ', $_;
                if ($first_line eq true) {
                        $first_line = false;
			# find position the command field
                        $col = 0;
                        foreach (@ps) {
				# under HP-UX and LINUX they call it COMMAND ?! ?! ?!
                                unless (($_ =~ /^CMD$/) or ($_ =~ /^COMMAND$/))
                                {
                                        $col++;
                                }

                        }
                } else {
                        #return command name from command field
			close (PIPE_IN);
                        return ($ps[$col]);
                }
        }
        close (PIPE_IN);
        return undef;
}

# the only thing to do is to return a true value to make perl happy
1;
