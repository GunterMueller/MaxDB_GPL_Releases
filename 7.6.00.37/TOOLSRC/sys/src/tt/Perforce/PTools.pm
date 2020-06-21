##################################
#
# @(#)PTools		
#
# Gerald Arnold, SAP AG
#
##################################
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

package PTools;
require 5.003;
use Env;
use File::Basename;
use ICopy;
use P4Lib;

@EXPORT_OK = qw(pview pget pdel phist pstatus pnew);

if ( $ENV{DEPOT} )
{	$Dep_Map = $ENV{DEPOT}; }
else
{	die "Error-PTOOLS:\$DEPOT not set\n"; }

$default_editor=$ENV{EDITOR};


########################################################################
# Funktion: pview ( $r_targets, $editor, $r_failed, $verbose )
#
# $r_targets   : Referenz auf Targetliste
# $editor      : Editorname
# $verbose     : genauere Ausgabe von Ablaufinfos (1)
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pview
{
	local ($r_targets, $editor, $r_failed, $verbose)=@_;
	local ($rc, @tmpfiles, @localfiles);
	local $revision=0;

	$SCRIPTNAME="PVIEW";

	$rc = 0;
	foreach $source ( @$r_targets ) 
	{
		($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);

		# not found
		if ( !defined($rel_path) ) 
		{
			print "Error-$SCRIPTNAME: $source not found !";
			$rc ++;
			push @$r_failed, $source if (defined ($r_failed));
			next;
		}
		local( $src_name, $src_path ) = fileparse($rel_path);
		# set default local path
		$localPath = "$OWN/tmp/$src_name";
		
		if ( -f "$OWN/$rel_path")
		{
			print "$OWN/$rel_path: local source !\n" if($ENV{REMOVE_LOCAL_SOURCES});

			($ourlock,$otherlock,$revision)=P4Lib::p4_fstat("$Dep_Map/$rel_path",$verbose);

			print "Warning-$SCRIPTNAME: local revision is not actual\n" if ( $revision > 0 );

			if ($ourlock == -1)
			{ 
				print "Error-$SCRIPTNAME: Cannot read status info of $Dep_Map/$rel_path\n";
				push @$r_failed, $source if (defined ($r_failed));
				$rc ++;
				next;
			}
			if ($ourlock == 0)  # no lock -> local copy open read only
			{ 	push  @tmpfiles, "$OWN/$rel_path";	}
			else
			{ 	push  @localfiles, "$OWN/$rel_path";	}
			
		}
		elsif ( P4Lib::p4_print($localPath,"$Dep_Map/$rel_path",$verbose) !=0 )
		{
			print "Error-$SCRIPTNAME: Copying from $Dep_Map/$rel_path to $localPath failed\n";
			$rc ++;
			push @$r_failed, $source if (defined ($r_failed));
			next;
		}
		else
		{
			push  @tmpfiles, $localPath;
			print "$SCRIPTNAME: $Dep_Map/$rel_path copied to $localPath\n" if ($verbose == 1);
		}
	}

	if ( ! defined ($editor))
	{
		$editor = $default_editor;
	}

	if ( $editor && ( $#tmpfiles >= 0 ) ) 
	{
		local @ARGV = ("-e", $editor, "-r", @tmpfiles );
		do "$TOOL/bin/opendoc$TOOLEXT";
		if ( $@ ) { print "$@" if $verbose; return 0; }
	}
	if ( $editor && ( $#localfiles >= 0 ) ) {
		local @ARGV = ("-e", $editor, @localfiles );
		do "$TOOL/bin/opendoc$TOOLEXT";
		if ( $@ ) { print "$@" if $verbose; return 0; }
	}
	return $rc;
}

########################################################################
# Funktion: pget ( $r_targets, $exclusive, $editor, $r_failed, $verbose )
#
# $r_targets   : Referenz auf Targetliste
# $exclusive   : 1 exclusive
# $editor      : Editorname
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# $verbose     : genauere Ausgabe von Ablaufinfos (1)
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pget
{
	local ($r_targets, $exclusive, $editor, $r_failed, $verbose) = @_;
	local ($rc, @localfiles, $exlcusive_string);
	local $revision=0;
	
	$SCRIPTNAME="PGET";
	$exclusive_string= $exclusive ? " exclusive" :"";
	
	$rc = 0;
	foreach $source ( @$r_targets ) 
	{
	
		($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);
		# not found
		if ( !defined($rel_path) ) 
		{
			print "Error-$SCRIPTNAME: $source not found !";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
		($ourlock,$otherlock,$revision)=P4Lib::p4_fstat("$Dep_Map/$rel_path",$verbose);

		if ($ourlock == -1)
		{ 
			print "Error-$SCRIPTNAME: Cannot read status info of $Dep_Map/$rel_path\n";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}

		# already locked exclusive by other user
		if ($otherlock == 2)
		{
			if ( $exclusive == 1)
			{
				print "Error-$SCRIPTNAME: $Dep_Map/$rel_path is already exclusive locked by an other user\n";
				push @$r_failed, $source if (defined ($r_failed));
				$rc ++;
				next;
			}
			else
			{
				print "Warning-$SCRIPTNAME: $Dep_Map/$rel_path is already exclusive locked by an other user\n";
			}
		}
		if ($ourlock < 2)
		{
			# no lock before
			if ($ourlock == 0)
			{
				# test for local copy
				if ( ( -f "$OWN/$rel_path") && ($revision==0) )
				{
					print "Warning-$SCRIPTNAME: $OWN/$rel_path already exists\n" if ($ENV{REMOVE_LOCAL_SOURCES});
					# try without error exception
					#print "Error-$SCRIPTNAME: $OWN/$rel_path already exists";
					#push @$r_failed, $source if (defined ($r_failed));
					#$rc ++;
					#next;
				}
				else
				{
					# sync to last version
					if (P4Lib::p4_sync("$Dep_Map/$rel_path",-1,$verbose) != 0)
					{
						# delete from client
						P4Lib::p4_sync("$Dep_Map/$rel_path",0,$verbose);
						push @$r_failed, $source if (defined ($r_failed));
						$rc ++;
						next;
					}
				}
				# edit synced version
				if (P4Lib::p4_edit("$Dep_Map/$rel_path",$verbose) != 0)
				{
					P4Lib::p4_sync("$Dep_Map/$rel_path",0,$verbose);
					push @$r_failed, $source if (defined ($r_failed));
					$rc ++;
					next;
				}
			}
			else
			{
				if ( ! $exclusive )
				{
					print "$Dep_Map/$rel_path already edited\n";
					push @$r_failed, $source if (defined ($r_failed));
					$rc ++;
					next;
				}
			}

			if ($exclusive)
			{
				if (P4Lib::p4_lock("$Dep_Map/$rel_path",$verbose) != 0)
				{
					# want exlusive lock to edited file failed (pget -x ... )
					# -> don't delete local file
					if ( $ourlock != 1 )
					{
						P4Lib::revert ("$Dep_Map/$rel_path",$verbose);
						P4Lib::p4_sync("$Dep_Map/$rel_path",0,$verbose);
					}
					push @$r_failed, $source if (defined ($r_failed));
					$rc ++;
					next;
				}
			}
		}
		else
		{
			print "$Dep_Map/$rel_path already locked\n";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
		push  @localfiles, "$OWN/$rel_path";
		print ("$SCRIPTNAME: $source checked out$exclusive_string to $OWN/$rel_path\n") if ($verbose == 1);
	}

	
	if ( ! defined ($editor))
	{
		$editor = $default_editor;
	}

	if ( $editor && ( $#localfiles >= 0 ) ) 
	{
		local @ARGV = ("-e", $editor, @localfiles );
		do "$TOOL/bin/opendoc$TOOLEXT";
		if ( $@ ) { print "$@" if $verbose; return 0; }
	}
	return $rc;
}


########################################################################
# Funktion: pdel ( $r_targets, $editor, $r_failed, $verbose )
#
# $r_targets   : Referenz auf Targetliste
# $editor      : Editorname
# $verbose     : genauere Ausgabe von Ablaufinfos (1)
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pdel
{
	local ($r_targets, $editor, $r_failed, $verbose)=@_;
	local ($rc, @localfiles);

	$SCRIPTNAME="PDEL";

	$rc = 0;
	foreach $source ( @$r_targets ) 
	{
	
		($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);
		# not found
		if ( !defined($rel_path) ) 
		{
			print "Error-$SCRIPTNAME: $source not found !";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
		
		if ( ! -f "$OWN/$rel_path") # locally not exists
		{
			print "Error-$SCRIPTNAME: $source locally not exists !\n";
			push @$r_failed, $source if (defined ($r_failed));
			$rc++;
			next;
		}

		($ourlock,$otherlock)=P4Lib::p4_fstat("$Dep_Map/$rel_path",$verbose);

		if ($ourlock == -1)
		{ 
			print "Error-$SCRIPTNAME: Cannot read status info of $Dep_Map/$rel_path\n";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}

		# is locked by the user => revert
		if ($ourlock > 0)
		{
			if (P4Lib::p4_revert("$Dep_Map/$rel_path",$verbose) != 0)
			{
				push @$r_failed, $source if (defined ($r_failed));
				$rc ++;
				next;
			}
		
		}

		if ( $ENV{REMOVE_LOCAL_SOURCES} )
		{
			# delete from client
			if (P4Lib::p4_sync("$Dep_Map/$rel_path",0,$verbose) !=0 )
			{
				print "Error-$SCRIPTNAME: Cannot remove $OWN/$rel_path\n";
				push @$r_failed, $source if (defined ($r_failed));
				$rc ++;
				next;
			}
			else
			{
				print "$SCRIPTNAME: $OWN/$rel_path deleted\n" if ($verbose == 1);
			}
		}
		
		push  @localfiles, "$OWN/$rel_path";

	}

	if ( ! defined ($editor))
	{
		$editor = $default_editor;
	}
	
	if ( $editor && ( $#localfiles >= 0 ) ) 
	{
		local @ARGV = ("-e", $editor, @localfiles );
		do "$TOOL/bin/closedoc$TOOLEXT";
		if ( $@ ) { print "$@" if $verbose; return 0; }
	}
	return $rc;
}

########################################################################
# Funktion: pstatus ( $r_targets, $r_failed )
#
# $r_targets   : Referenz auf Targetliste
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pstatus
{
	local ($r_targets, $r_failed)=@_;
	
	$SCRIPTNAME="PSTATUS";

	$rc = 0;
	foreach $source ( @$r_targets ) 
	{
	
		($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);
		# not found
		if ( !defined($rel_path) ) 
		{
			print "Error-$SCRIPTNAME: $source not found !";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
		
		if ( -f "$OWN/$rel_path") # locally not exists
		{
			print "$SCRIPTNAME: $source locally exists !\n";
		}

		($ourlock,$otherlock)=P4Lib::p4_fstat("$Dep_Map/$rel_path",2);

		if ($ourlock == -1)
		{ 
			print "Error-$SCRIPTNAME: Cannot read status info of $Dep_Map/$rel_path\n";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
	}
	return $rc;
}


########################################################################
# Funktion: phist ( $r_targets, $r_failed )
#
# $r_targets   : Referenz auf Targetliste
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub phist
{
	local ($r_targets, $r_failed)=@_;
	
	$SCRIPTNAME="PHIST";

	$rc = 0;
	foreach $source ( @$r_targets ) 
	{
	
		($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);
		# not found
		if ( !defined($rel_path) ) 
		{
			print "Error-$SCRIPTNAME: $source not found !";
			push @$r_failed, $source if (defined ($r_failed));
			$rc ++;
			next;
		}
		$rc++ if (P4Lib::p4_history("$Dep_Map/$rel_path",2) != 0);
	}
	return $rc;
}

########################################################################
# Funktion: pdiff ( $target )
#
# $target      : Target
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pdiff
{
	local ($target)=@_;
	
	$SCRIPTNAME="PDIFF";

	$rc = 0;
	($vmake_path, $rel_path) = ICopy::GetFilePath($target, 0, 0, 0);
	# not found
	if ( !defined($rel_path) ) 
	{
		print "Error-$SCRIPTNAME: $source not found !";
		push @$r_failed, $source if (defined ($r_failed));
		$rc ++;
	}
	elsif ( ! -f "$OWN/$rel_path") # locally not exists
	{
		print "Error-$SCRIPTNAME: $source locally not exists !\n";
		push @$r_failed, $source if (defined ($r_failed));
		$rc++;
	}

	else
	{
		$rc++ if (P4Lib::p4_diff ("$Dep_Map/$rel_path") != 0);
	}
	return $rc;
}

########################################################################
# Funktion: pnew ( $r_targets, $editor, $r_failed, $verbose )
#
# $r_targets   : Referenz auf Targetliste
# $editor      : Editorname
# $verbose     : genauere Ausgabe von Ablaufinfos (1)
# $r_failed    : Referenz auf Liste der Targets, bei denen ein Fehler auftrat
# Rückgabe     : 0 bei Erfolg sonst Anzahl der Fehler
#
sub pnew
{
    local ($r_targets, $editor, $r_failed, $verbose) = @_;
    local ($rc, @localfiles);
    local $revision=0;
    
    $SCRIPTNAME="PNEW";
    
    $rc = 0;
    foreach $source ( @$r_targets ) {
        ($vmake_path, $rel_path) = ICopy::GetFilePath($source, 0, 0, 0);
        if (!defined ($rel_path)) {
            $addpar = $source;
        } else {
            $addpar = "$OWN/$rel_path";
        }
        if (P4Lib::p4_add ($addpar, $verbose) != 0) {
            push @$r_failed, $source if (defined ($r_failed));
            $rc ++;
            next;
        }
        push  @localfiles, "$OWN/$rel_path";
        print ("$SCRIPTNAME: $source checked out$exclusive_string to $OWN/$rel_path\n") 
          if ($verbose == 1);
    }
    
    if ( ! defined ($editor)) {
        $editor = $default_editor;
    }
    
    if ( $editor && ( $#localfiles >= 0 ) ) {
        local @ARGV = ("-e", $editor, @localfiles );
        do "$TOOL/bin/opendoc$TOOLEXT";
        if ( $@ ) { print "$@" if $verbose; return 0; }
    }
    return $rc;
}
