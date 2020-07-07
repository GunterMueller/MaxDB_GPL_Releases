#
# @(#)ICopy		2000-03-27
#
# G. Arnold, SAP AG
#
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

package P4Lib;
require 5.003;

use File::Copy;
use File::Basename;
use Cwd;
use Env;

@EXPORT_OK = qw(p4_sync p4_edit p4_lock p4_revert p4_print p4_fstat p4_history p4_diff p4_add p4hiddenCommand p4sync p4fstat p4_opened p4_edit_cl p4_submit);

# if error return 1 else 0;
sub p4_sync
{

    my($file, $version, $verbose) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "sync $file";
	if (defined $version)
	{
		$p4command .= "#$version" if ($version > 0);
		$p4command .= "#none" if ( $version == 0);
	}

	# reset message list
	@p4messages=();

	$rc = p4_command ($p4command,\@p4messages);

	if ( ($rc != 0) || ( $verbose == 2 ))
	{
		foreach $p4message (@p4messages)
		{
			print ("$p4message");
		}
	}
	return $rc;
}

#
# return result of sync operation on a single file.
#
sub p4sync
{
    local ( $file, $force, $verbose ) = @_;

    local $p4syncResult;

    if ( $force > 0 )
    {
        $p4syncResult = p4hiddenCommand( "p4 sync -f $_[0]" );
    } else {
        $p4syncResult = p4hiddenCommand( "p4 sync $_[0]" );
    }

    print "$p4syncResult\n" if ( $verbose > 0 );

    if ( $p4syncResult =~ " - no such file\\\(s\\\)" ) {
        return -1;
    }

    if ( $p4syncResult =~ " - file\\\(s\\\) up-to-date" ) {
        return 0;
    }

    if ( $p4syncResult =~ " - updating" ) {
        return 1;
    }

    if ( $p4syncResult =~ " - refreshing" ) {
        return 2;
    }

    return -2;
}

sub p4_submit
{
	local ( $filelist, $text, $verbose ) = @_;
	my @depot_files = ();
	my $rc = 0;

	foreach (@$filelist)
	{
		my $output = `p4 fstat $_`;
		if (`p4 fstat $_`=~ m|\.\.\. depotFile\s(\S+)|)
		{
			push @depot_files, $1;
		}
		else
		{
			print "ERROR: Can't determine depot file name for '$_'\n";
			$rc ++;
		}
	}
	if ( $rc==0 && scalar (@depot_files))
	{
		open( SUBMIT_IN, '| p4 submit -i') or die "open p4 submit -i failed! \n$!\n";
		print SUBMIT_IN "Change: new\n";
	    print SUBMIT_IN "Description: $text\n";
	    print SUBMIT_IN "Files:\n";
	    foreach (@depot_files)
	    {
	    	print SUBMIT_IN "\t$_\n";
	    }
	    close (SUBMIT_IN) or $rc = -2;
    }
    if ($rc)
    {  	print "Files not submitted!\n";   }
    return $rc;
}

# if error return 1 else 0;
sub p4_edit
{

    local($file, $verbose) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "edit $file";

	# reset message list
	@p4messages=();

	$rc = p4_command ($p4command, \@p4messages);

	if ( ($rc != 0) || ( $verbose == 2 ))
	{
		foreach $p4message (@p4messages)
		{
			print ("$p4message");
		}
	}
	return $rc;
}


# if error return 1 else 0;
sub p4_edit_cl
{

    local($file, $changelist, $verbose) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "edit -c $changelist $file";

	# reset message list
	@p4messages=();

	$rc = p4_command ($p4command, \@p4messages);

	if ( ($rc != 0) || ( $verbose == 2 ))
	{
		foreach $p4message (@p4messages)
		{
			print ("$p4message");
		}
	}
	return $rc;
}



# if error return 1 else 0;
sub p4_lock
{

    local($file, $verbose) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "lock $file";

	# reset message list
	@p4messages=();

	$rc = p4_command ($p4command, \@p4messages);

	if ( ($rc != 0) || ( $verbose == 2 ))
	{
		foreach $p4message (@p4messages)
		{
			print ("$p4message");
		}
	}
	return $rc;
}



# if error return 1 else 0;
sub p4_revert
{
    local($file, $verbose) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "revert $file";

	# reset message list
	@p4messages=();

	$rc = p4_command ($p4command, \@p4messages);

	if ( ($rc != 0) || ( $verbose == 2 ))
	{
		foreach $p4message (@p4messages)
		{
			print ("$p4message");
		}
	}
	return $rc;
}


sub p4_print
{
	local ($target, $source, $verbose) = @_;
	my $p4command = "p4 -s print $source ";
	$rc = -1;
	if ( -f $target)
	{
		unlink ("$target");
	}
	unless (open(FILE_OUT, ">$target"))
	{
		print STDERR "Can't open $target (output): $!";
		return -1;
	}

	open(P4_COMMAND,"$p4command |");
	while( (defined($line=<P4_COMMAND>)) && ($rc ==-1) )
	{

		if ($line=~/^Error/i)
		{
			print ("$line\n");
			close (P4_COMMAND);
			close (FILE_OUT);
			unlink "$target";
			return 1;
		}

		if ( ( $line =~ /^info: (.*)$/i ) && $verbose )
		{
			print "$1\n";
		}
		else
		{
			if  ( $line =~ /^exit: (\d)$/i)
			{
				$rc = 0;
			}
			else
			{
				# cut "text: "-token
				$line =~ s/^text: //i;
				print FILE_OUT $line;
			}
		}
	}
	close (P4_COMMAND);
	close (FILE_OUT);
	print "$source copied to $target\n" if ($verbose);
	return $rc;
}


# return 0 or 1
sub p4_command
{
	local($p4_command, $r_p4messages) = @_;
	$rc=0;

	# reset message list
	@$r_p4messages=();

	open(P4_COMMAND,"p4 -s $p4_command |");
	while(defined($line=<P4_COMMAND>))
	{

		# fill message list
		push @$r_p4messages, $line if ($line!~/^exit/i);
		$rc=1 if ($line=~/^Error/i);
	}
	close(P4_COMMAND);
	return $rc;
}

#
# p4hiddenCommand(cmdString)
#
# Used for commands producing "small" output. STDOUT and STDERR are both
# redirected into a single result string. Usefull since p4 puts diagnostic
# messages to STDERR while output is found in STDOUT. Nice addon is that
# all output of p4 command is effectivly suppressed this way...
#
# return complete result string
#
sub p4hiddenCommand {
    local $cmdResult = "$ENV{TMP}/p4cmd$$";
    local $result = "";

    open(OLDOUT, ">&STDOUT");
    open(OLDERR, ">&STDERR");

    open(STDOUT, ">$cmdResult") || die "Can't redirect stdout";
    open(STDERR, ">&STDOUT") || die "Can't dup stdout";

    system( $_[0] );

    close(STDOUT);
    close(STDERR);

    open(STDOUT, ">&OLDOUT");
    open(STDERR, ">&OLDERR");

    open(CMDOUT, "<$cmdResult") || die "Can't open result file";
    while ( <CMDOUT> ) {
	$result .= $_;
    }
    close(CMDOUT);
    unlink(CMDOUT);

    return $result;
}

# return -1 = Error
# returnparam 1
# return 0 not locked
# return 1 self mlocked
# return 2 self locked
# --------------------------
# returnparam 2
# return 0 other not locked
# return 2 other mlocked
# return 3 other locked
# --------------------------
# return param 3
# return 0 : local revision = actual head revision
# else > 0 (difference between head revision and local revision

# return ... other mlocked
sub p4_fstat
{
	local ($file, $verbose) = @_;
	my @mlocked =();
	my $locked = "";
	my $p4command = "p4 -s fstat $file ";
	$rc = -1;
	$open_number = 0;
	$last_user = ();
	$ourlock=0;
	$otherlock=0;
	$headRev=0;
	$haveRev=0;

	open(P4_COMMAND,"$p4command |");
	while( (defined($line=<P4_COMMAND>)) && ($rc ==-1) )
	{

		if ($line=~/^Error/i)
		{
			print ("$line\n");
			close (P4_COMMAND);
			unlink "$target";
			return (-1,0,-1);
		}

		if ( ( $line =~ /^info(\d+): (.*)$/i ))
		{
			# info1 all and self client information
			$info_number=$1;
			$info_text=$2;


			if  ( $info_text =~ /^otherOpen(\d+) *(\S*)$/i)
			{
				$last_user{number} = $1;
				$last_user{info_number} = $info_number;
				$last_user{name} = $2;
				$last_user{status} = 0; # nothing
			}

			if ( $info_text =~ /^otherAction(\d+) *(\S*)$/i)
			{
				if ($1 eq $last_user{number})
				{
					if ($2 eq "edit")
					{
						push @mlocked, $last_user{name};
						$otherlock=1 if ($otherlock < 1);
					}
				}
			}
			if ( $info_text =~ /^otherLock/i)
			{
				if ($info_number eq $last_user{info_number})
				{
					pop @mlocked;
					$locked= $last_user{name};
					$otherlock=2;
				}
			}

			if ( $info_text =~ /^action edit.*/i )
			{
				$ourlock=1 if ($ourlock < 1);
				print "$file edited by yourself\n" if( $verbose == 2 );
			}


			if ( $info_text =~ /^ourLock/i )
			{
				$ourlock = 2;
				print "$file locked by yourself\n" if( $verbose == 2 );
			}

			if ( $info_text =~ /^headRev (\d*)$/i )
			{
				$headRev = $1;
				print "head revision of \"$file\" is $headRev\n" if( $verbose == 2 );
			}

			if ( $info_text =~ /^haveRev (\d*)$/i )
			{
				$haveRev = $1;
				print "local revision of \"$file\" is $headRev\n" if( $verbose == 2 );
			}
		}
	}
	close (P4_COMMAND);
	if ($locked ne "")
	{
		print ("$file is locked by $locked\n") if( $verbose == 2 );
	}
	if ($verbose)
	{
		foreach $mlock (@mlocked)
		{
			print ("$file is edited by $mlock\n") if( $verbose == 2 );
		}
	}

	return ($ourlock,$otherlock,$headRev-$haveRev);
}

#
# p4fstat
#
# Return status of a depot file as tupel:
#
# ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded )
#
# fileDoesNotExist: 0 file exists in depot (even if only added, but not submitted)
#                   1 file does not exist in depot
#
# fileNotSynced:    0 file is synced (even if only added, but not submitted)
#                   > 0 head revision - local file revision
#
# fileOverwritable: 0 file is not checked out (readonly)
#                   1 file is locally opened for edit (overwritable)
#
# fileAdded:        0 file was already submitted before
#                   1 file is already marked to be added to depot, but not yet submitted
#
# Some combinations:
# (0,0,0,0) file check in and synced to head revision
# (0,1,0,0) file check in and NOT synced to head revision
# (0,0,1,0) file checked out and synced to head revision and editable
# (0,0,1,1) file to be added and editable
# (1,0,0,0) file does not exist in depot
#
sub p4fstat {

    local ( $file , $verbose ) = @_;

    local $p4fstatResult = p4hiddenCommand( "p4 fstat $file" );

    print "Result: $p4fstatResult" if ( $verbose > 0 );

    local $fileDoesNotExist="0";
    local $fileNotSynced="0";
    local $fileOverwritable="0";
    local $fileAdded="0";
    local $headRev;
    local $haveRev;

    if ( $p4fstatResult =~ " - no such file" ) {

        $fileDoesNotExist = "1";

    } else {

        $_ = $p4fstatResult;
        if ( /[^\.]*... headRev (\d*)\D*/m ) {
            $headRev = $1;
        }

        $_ = $p4fstatResult;
        if ( /[^\.]*... haveRev (\d*)\D*/m ) {
            $haveRev = $1;
        }

        if ( "$headRev" ne "$haveRev" ) {
            $fileNotSynced = $headRev - $haveRev;
        }

        if ( $p4fstatResult =~ "... action edit" ) {
            $fileOverwritable = "1";
        }

        if ( $p4fstatResult =~ "... ourLock" ) {
            $fileOverwritable = "2";
        }

        if ( $p4fstatResult =~ "... action add" ) {
            $fileOverwritable = "3";
            $fileAdded = "1";
        }

    }

    print " ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded )\n" if ( $verbose > 0 );

    return ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded );
}

# file
# if error return 1 else 0;
sub p4_history
{
    local($file) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "filelog -l $file";

	open(P4_COMMAND,"p4 -s $p4command |");
	while(defined($line=<P4_COMMAND>))
	{
		# fill message list
		if ($line =~ /^info\d*:(.*)$/i)
		{
			print "$1\n";
		}

		$rc++ if ($line=~/^Error/i);
	}
	close(P4_COMMAND);
	return $rc;
}

# diff between local file and the file on the server
# if error return 1 else 0;
sub p4_diff
{
    local($file) = @_;
	$rc = 0;
	# build p4 command
	my $p4command = "diff $file";

	open(P4_COMMAND,"p4 -s $p4command |");
	while(defined($line=<P4_COMMAND>))
	{
		# fill message list
		if ($line =~ /^info\d*:(.*)$/i)
		{
			print "$1\n";
		}
		elsif ($line=~/^Error/i)
		{
			$rc++
		}
		elsif ($line=~/^exit:/i)
		{
			last;
		}
		else
		{
			print $line;
		}
	}
	close(P4_COMMAND);
	return $rc;
}

# if error return 1 else 0;
sub p4_add
{
    local ($file, $verbose) = @_;
    $rc = 0;
    # build p4 command
    my $p4command = "add -t text $file";

    # reset message list
    @p4messages=();

    $rc = p4_command ($p4command, \@p4messages);

    if ( ($rc != 0) || ( $verbose == 2 )) {
        foreach $p4message (@p4messages) {
            print ("$p4message");
        }
    }
    return $rc;
}

#
# return mapping of opened files to changelists
#
sub p4_opened
{
    local ($file, $verbose) = @_;
    my $rc = 0;

    my $p4syncResult = p4hiddenCommand( "p4 opened $file" );

    print "$p4syncResult\n" if ( $verbose > 0 );

    if ( $p4syncResult =~ / - not opened on this client/ ) {
        return -1;
    }

    if ( $p4syncResult =~ / - (add|edit) default change/ ) {
        return 0;
    }

    if ( $p4syncResult =~ / - (add|edit) change (\d*) / ) {
        return int($2);
    }
    return -2;
}
