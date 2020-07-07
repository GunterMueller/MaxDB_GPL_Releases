#!/usr/bin/perl
#
#----------------------------------------------------------------
# Stripped version of newgen.pl. Not producing Table Information
#----------------------------------------------------------------
#
# Input message files lines have following format
#
# A Message specification line (Each message argument specified by %s)
#	MessageIdentifier,MessageString
#	or if message numbers are not generated
#	MessageIdentifier,MessageNumber,MessageString
#
# A Typecode specification line (Default Error) Valid up to next Typecode specification
#	TYPE=Typecode (Info,Warning,Error)
#
# Set Output Header file list
#	>header{,header}
#
# Add Output Header file to list
#   >+header{,header}
#
# Remove Output Header file from list
#   >-header{,header}
#
# header is the header file basename. The header file name is build by adding a suffix.
#
# Version 2.2 changes
#        Added support for C Headerfiles if target file with .h extension given.
#        Fixed bug in file list generation (suffixes were not parsed correctly)
#
# Version 2.1 changes
#        difftarget replaced by nodifftarget
#
# Version 2.0 changes
#        Perforce option allows to use p4 commands to get readable/editable version of header file for update
#        and to add the file if it did not exist.
#
#        Check option allows to check single file for update without need to create temporary files
#          The check is done by comparing temporary file with the existing one.
#
#        List option creates a list of file to be created
#          This list is simply the output of all possible generated header files
#
# Version 1.3 changes
#        NEW suffixes!!!! .cid replace by .gencid   .msg replaced by .genmsg
#
# Version 1.2 changes
#        Additional option -rawmode recognized. Allows to parse '.cid' files. These '.cid' file
#        have the same syntax as '.msg' file, but TYPE is ignored and no message identifier are
#        generated. Only the first form of message line
#           MessageIdentifier,MessageString
#        is accepted and transformed in a simple line
#           #define MessageIdentifier MessageString
#        This option is used i.e. to create header files for configuration parameter identifier.
#
#
#        ========== licence begin  GPL
#        Copyright (C) 2001 SAP AG
#
#        This program is free software; you can redistribute it and/or
#        modify it under the terms of the GNU General Public License
#        as published by the Free Software Foundation; either version 2
#        of the License, or (at your option) any later version.
#
#        This program is distributed in the hope that it will be useful,
#        but WITHOUT ANY WARRANTY; without even the implied warranty of
#        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#        GNU General Public License for more details.
#
#        You should have received a copy of the GNU General Public License
#        along with this program; if not, write to the Free Software
#        Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#        ========== licence end
#

use Env;
use File::Compare;
use File::Copy;
use Getopt::Long;
use ICopy;

$IdString="genmsg V2.2";

######################################################
#
# CheckMessage ( $Message, $MessageId, $MsgArgs, $ParseFailed )
#
# Count %s and detect bad formatted message string
#
sub CheckMessage {
        local $myMsg=$_[0];
        local $myPos=0;
        local $myOffset=0;
        while ( $myMsg =~ /%[^s]/g ) {
           $myPos = pos $myMsg;
           if ( substr($myMsg, $myPos - 1, 1) eq "%" )
           {
                   $myMsg = substr($myMsg, $myPos);
                   $myOffset = $myOffset + $myPos;
           }
           else
           {
	                 print "Bad Format specifier found:\n";
	                 $i=1;
	                 print "$_[1],";
                 	 while ( $i++ < ($myPos + $myOffset) ) {
                           print " ";
	                 }
	                 print "v\n$_[1],$_[0]\n";
	                 $_[3]++;
                   $myMsg = "";
           }
        }
  	$_[2] = s/%s/%s/g + 1 - 1;
}
######################################################
#
# CheckAndVerifyMessageNo($MessageNo, $MessageId, $RangeMin, $RangeMax, $ParseFailed)
#
# Uses global %UsedIds
#
sub CheckAndVerifyMessageNo {
	if ( $_[2] > 0 ) {
		if ( $_[0] < $_[2] ) {
			print "Bad MessageNumber $_[1] (too small): $_[0] < [ $_[2],$_[3] [\n";
			$_[4]++;
		}
	}
	if ( $_[3] > 0 ) {
		if ( $_[0] >= $_[3] ) {
			print "Bad MessageNumber $_[1] (too big): $_[0] > [ $_[2],$_[3] [\n";
			$_[4]++;
		}
	}
	if ( defined $UsedIds{$_[0]} ) {
		print "Bad MessageNumber $_[1] (doubled): $_[0] ( previous $UsedIds{$_[0]} )\n";
		$_[4]++;
	}
	else
	{
		$UsedIds{$_[0]} = $_[1];
	}
}
######################################################
#
# NextMessageNo ( $MsgNo, $MessageNo, $MessageId, $RangeMax, $ParseFailed )
#
# Uses Global: %UsedIds
#
sub NextMessageNo {
	while ( defined $UsedIds{$_[0]} ) {
		$_[0]++;
	}
	$_[1] = $_[0]++;
	if ( $_[3] > 0 ) {
		if ( $_[1] >= $_[3] ) {
			print "Out of Message numbers $_[2] : $_[1] >= $_[3]\n";
			$_[4]++;
		}
	}
}
######################################################
#
# CheckMsgType ( $MsgType, $MsgTypeIdentifier, $ParseFailed )
#
# Check Message Type
#
sub CheckMsgType {
	$_[0] =~ tr/[a-z]/[A-Z]/;
	if ( $_[0] eq ERROR ) {
		$_[1] = "ERROR";
	} elsif ( $_[0] eq WARNING ) {
		$_[1] = "WARNING";
	} elsif ( $_[0] eq INFO ) {
		$_[1] = "INFO";
	} else {
		print "Bad Message Type Specifier: $_[0]\n";
		$_[2]++;
	}
}
######################################################
#
# Modify the output files
#
# $OutputList is used remove existing files from list of opened files
# $NewOutputList is used to open additional files and refill list of
# opened files
#
sub ModifyOutputList {
	@OldList = split(/,/,$OutputList);
	@NewList = split(/,/,$NewOutputList);

	%OutputFileName = ();
    %COutputWanted = ();

	foreach $File (@NewList) {
		if ( $File =~ /\.h$/ )
		{
			$COutputWanted{$File} = "YES";
			$HeaderFile = $File;
		} elsif ( $File =~ /\.hpp$/ )
		{
			$HeaderFile = $File;
		}
		else
		{
			if ( defined $RawMode )
			{
				$HeaderFile = $File."_ConfigParamId.hpp";
			} else {
				$HeaderFile = $File."_Messages.hpp";
			}
		}
		if ( ! defined $AllOutputFileName{$File} ) {
			$AllOutputFileName{$File} = $HeaderFile;
            if ( defined $ListMode ) {
                print "$HeaderFile\n";
            } else {
                if ( ! defined $COutputWanted{$File} ) {
					$UppercaseHeader = $HeaderFile;
					$UppercaseHeader =~ tr/[a-z]/[A-Z]/;
					$UppercaseHeader =~ tr/./_/;
					open( HEADER, ">$TempDir/$HeaderFile" )
							|| die ( "Can´t create header file $HeaderFile\n" );
					print HEADER GetLicenceText ();
					print HEADER "#ifndef $UppercaseHeader\n";
					print HEADER "#define $UppercaseHeader\n\n";
					print HEADER "// Generated Header file $HeaderFile.\n";
					print HEADER "// Generated by ${IdString} DO NOT EDIT\n\n";
					if ( ! defined $RawMode ) {
						print HEADER "#include \"SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp\"\n\n";
					}
					if ( defined $AddTable ) {
						print HEADER "#ifndef SAPDB_COMPILE_MESSAGES_IN\n\n";
						print HEADER "extern SAPDB_MessageMap * ${TableName}_MessageMap;\n\n";
						print HEADER "extern void ${TableName}_MessageMapInit();\n\n";
					}
					close ( HEADER );
				} else {
					$UppercaseHeader = $HeaderFile;
					$UppercaseHeader =~ tr/[a-z]/[A-Z]/;
					$UppercaseHeader =~ tr/./_/;
					open( HEADER, ">$TempDir/$HeaderFile" )
							|| die ( "Can´t create header file $HeaderFile\n" );
					print HEADER GetLicenceText ();
					print HEADER "#ifndef $UppercaseHeader\n";
					print HEADER "#define $UppercaseHeader\n\n";
					print HEADER "/* Generated Header file $HeaderFile. */\n";
					print HEADER "/* Generated by ${IdString} DO NOT EDIT */\n";
					if ( ! defined $RawMode ) {
						print HEADER "#include \"SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h\"\n\n";
					}
					close ( HEADER );
				}
            }
		}
		$OutputFileName{$File} = $HeaderFile;
	}
	$OutputList = $NewOutputList;
}
######################################################
#
# Remove from list of output files
#
# $OutputList is find opened files. First argument is used to find list
# of files to remove from output list.
#
sub RemoveFromOutputList {
	@OldList = split(/,/,$OutputList);
	@RemoveList = split(/,/,$_[0]);
        %TagList = ();

	foreach $File (@RemoveList) { $TagList{$File} = "1"; undef $OutputFileName{$File}; undef $COutputWanted{$File} }

	undef $NewOutputList;

	foreach $File (@OldList) {
		if ( ! defined $TagList{$File} ) {
			if ( ! defined $NewOutputList ) {
				$NewOutputList = $File;
			} else {
				$NewOutputList = "$NewOutputList,$File";
			}
		}
	}

	$OutputList = $NewOutputList;
}
######################################################
#
# MapSAPDBPath
#
# Map a file using SAPDB Mapping
#
sub MapSAPDBPath {
    local $File = @_[0];
    local $PathToFile;

    $PathToFile = $File;
    ICopy::_MapModuleName( $PathToFile );
    while ( substr($PathToFile,0,1) eq ":" )
    {
        $PathToFile = substr($PathToFile, 1, length($PathToFile)-1);
    }
    $PathToFile = $ENV{OWN}."/sys/src/".$PathToFile;
}

######################################################
#
# AddSeparatorToFiles
#
# Add separator into all header files
#
sub AddSeparatorToFiles {
    if ( !defined $ListMode ) {
        foreach $HeaderFile (values(%AllOutputFileName)) {
	        open( HEADER, ">>$TempDir/$HeaderFile" ) || die ( "Can't reopen header file $HeaderFile" );
		    print HEADER "\n# else\n\n";
        	close( HEADER );
        }
    }
}
######################################################
#
# CloseOutputFiles($ParseFailed)
#
# Close all output header files
# Writes final words into header files and copies them if no parsing error occured.
#
# Uses Global %AllOutputFileName, $TempDir, HEADER
#
sub CloseOutputFiles {

    if ( !defined $ListMode ) {

	    foreach $HeaderFile (values(%AllOutputFileName)) {
		    open( HEADER, ">>$TempDir/$HeaderFile" ) || die ( "Can't reopen header file $HeaderFile" );
		    $UppercaseHeader = $HeaderFile;
		    $UppercaseHeader =~ tr/[a-z]/[A-Z]/;
		    $UppercaseHeader =~ tr/./_/;
		    if ( defined $AddTable ) {
			    print HEADER "\n# endif\n";
		    }
		    print HEADER "\n#endif /* $UppercaseHeader */\n";
		    close( HEADER );
	    }

    }

    if ( $_[0] > 0 ) {
	    print "$_[0] parsing errors!!! Correct Messagefile or Range specification\n";
	    exit;
    }

    if ( !defined $ListMode ) {

#
# List of files
#
        local @FilesWithNewVersions = ();
        local @FilesCreated = ();
        local @FilesToGetFromP4Depot = ();
        local @FilesToAddToP4Depot = ();
        local @FilesToSubmitToP4Depot = ();
#
# First loop over all files and make sure they do exist as readable file at least
#
        foreach $HeaderFile (values(%AllOutputFileName)) {
#
# map the header file path using SAPDB 'directory mapping'
#
            local $PathToHeaderFile;
            local $NeededInP4Depot;
            local $LockedInP4Depot;

            if ( length($Target) != 0 ) {
                $PathToHeaderFile = "$Target$HeaderFile";
            } else {
                $PathToHeaderFile = $HeaderFile;
                if ( defined $UseP4 ) {
                    $PathToHeaderFile = MapSAPDBPath( $PathToHeaderFile );
                }
            }

            if ( defined $UseP4 ) {
#---------------------------
# Search file in P4 depot
#
            	local ( $notInDepot, $revDiff, $overWritable, $isAdded ) = P4Lib::p4fstat($PathToHeaderFile);

            	if ( "$notInDepot" eq "1" ) {

		            $NeededInP4Depot = 1;
		            $LockedInP4Depot = 0;
                    unlink( $PathToHeaderFile );
		            push @FilesToAddToP4Depot, $PathToHeaderFile;

            	} else {

                    undef $NeededInP4Depot;

                    if ( $revDiff > 0 ) {
#
# Another revision exists. We must sync to head revision now
#
                        if ( -w $PathToHeaderFile )
                        {
                            if ( P4Lib::p4sync( "p4 sync $PathToHeaderFile", 1 ) < 0 )
                            {
    	            	        print "$PathToHeaderFile not forced syncable!\n";
                                P4Lib::p4sync( "p4 sync $PathToHeaderFile", 1, 1 );
                                exit;
                            }
                        }
                        else
                        {
                            if ( P4Lib::p4sync( "p4 sync $PathToHeaderFile" ) < 0 )
                            {
    	            	        print "$PathToHeaderFile not syncable!\n";
                                P4Lib::p4sync( "p4 sync $PathToHeaderFile", 0, 1 );
                                exit;
                            }
                        }
                    }

                    if ( $overWritable > 0 ) {
			            $LockedInP4Depot = 1;
                        push @FilesToSubmitToP4Depot, $PathToHeaderFile;
        		    } else {
                        undef $LockedInP4Depot;
                    }
            	}
	        }

#------------------------------
# If file is writable check if it is needed to add it to P4 depot
#
    	    if ( ! -w $PathToHeaderFile ) {

    	        if ( -e $PathToHeaderFile && ! -r $PathToHeaderFile )
                {
            	    print "$PathToHeaderFile not readable!!!\n";
                    exit;
                }

            }

            if ( ! defined $DiffTarget
                || compare("$PathToHeaderFile","$TempDir/$HeaderFile") != 0 ) {

                  if ( ! defined $UseP4
                    && -e $PathToHeaderFile
                    && ! -w $PathToHeaderFile ) {
  	                print "$PathToHeaderFile not writable!!!\n";
                    exit;
                  }

            	if ( -e $PathToHeaderFile ) {
	                if ( defined $UseP4 && !defined $LockedInP4Depot ) {
                        push @FilesToGetFromP4Depot, $PathToHeaderFile;
                    }
                    push @FilesWithNewVersions, $HeaderFile;
                } else {
                    push @FilesCreated, $HeaderFile;
                }
        	}
        }

#------------------------------------------------------------
# (P4 only) get all files that need to be locked in depot and if ok, add them to the list to submit
#
        if ( defined $UseP4 && @FilesToGetFromP4Depot >= 0 ) {
            foreach ( @FilesToGetFromP4Depot ) {
                if ( P4Lib::p4_edit($_) != 0 )
                {
                    print "Failed to get ".$_." from P4 depot\n";
                    exit;
                }
                push @FilesToSubmitToP4Depot, $_;
            }
        }

#---------------------
# Copy all new files
#
        if ( @FilesCreated > 0 )
        {
            foreach $HeaderFile ( @FilesCreated ) {

                local $PathToHeaderFile;

                if ( length($Target) != 0 ) {
                    $PathToHeaderFile = "$Target$HeaderFile";
                } else {
                    $PathToHeaderFile = $HeaderFile;
                    if ( defined $UseP4 ) {
                        $PathToHeaderFile = MapSAPDBPath( $PathToHeaderFile );
                    }
                }

                copy("$TempDir/$HeaderFile", "$PathToHeaderFile");
                if ( defined $DiffTarget ) {
                    print "Update $PathToHeaderFile\n";
                } else {
                    print "Overwriting $PathToHeaderFile\n";
	            }
            }
        }

#---------------------------
# Update all modified files
#
        if ( @FilesWithNewVersions > 0 )
        {
            foreach $HeaderFile ( @FilesWithNewVersions ) {

                local $PathToHeaderFile;

                if ( length($Target) != 0 ) {
                        $PathToHeaderFile = "$Target$HeaderFile";
                } else {
                        $PathToHeaderFile = $HeaderFile;
                        if ( defined $UseP4 ) {
                            $PathToHeaderFile = MapSAPDBPath( $PathToHeaderFile );
                        }
                }

                copy("$TempDir/$HeaderFile", "$PathToHeaderFile");
                if ( defined $DiffTarget ) {
                    print "Update $PathToHeaderFile\n";
                } else {
                    print "Overwriting $PathToHeaderFile\n";
                }
            }
        }

#------------------------------------------------------------
# (P4 only) add all files that need to be added
#
        if ( defined $UseP4 && @FilesToAddToP4Depot > 0 ) {
            foreach ( @FilesToAddToP4Depot ) {
                if ( P4Lib::p4_add($_) != 0 )
                {
                    print "Failed to add ".$_." from P4 depot\n";
                    exit;
                }
                push @FilesToSubmitToP4Depot, $_;
            }
        }

#------------------------------------------------------------
# (P4 only) submit all changes in a single changelist
#
        if ( defined $UseP4 && @FilesToSubmitToP4Depot > 0 ) {
            local $submitCmd = "$TOOL/bin/p4put$TOOLEXT -d Generated_By_GENMSG_From__$Messages @FilesToSubmitToP4Depot";
            system( $submitCmd );
        }
    }
}

######################################################
#
# GetLicenceText
#
# Close all output header files
# Writes final words into header files and copies them if no parsing error occured.
#
# Uses Global %AllOutputFileName, $TempDir, HEADER
#
sub GetLicenceText {
	my ($sec, $min, $hour, $day, $mon, $year, $wday, $yday, $isdst)
		= localtime (time);
	$year += 1900;
	my ($result, $relver);
	$relver = $RELVER;
	if ((substr ($relver, 0, 1) eq 'R') && ($relver ge 'R75')) {
		$result = <<EOF;
/*
        ========== licence begin  GPL
        Copyright (C) $year SAP AG

        This program is free software; you can redistribute it and/or
        modify it under the terms of the GNU General Public License
        as published by the Free Software Foundation; either version 2
        of the License, or (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
        ========== licence end
 */
EOF
	}
	else {
		$result = <<EOF;
/*
        ========== licence begin LGPL
        Copyright (C) %s SAP AG

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
        ========== licence end
 */
EOF
	}
	return $result;
}

######################################################
######################################################
# Program starts here....
######################################################
######################################################

#
# Option parsing
#
$Messages = undef;
$RangeMin = undef;
$RangeMax = undef;
$AddTable = undef;
$DiffTarget = undef;
$Destination = undef;
$AddLocation = undef;
$RawMode = undef;
$ListMode = undef;
$CheckFile = undef;
$UseP4 = undef;
GetOptions ( 'rangemin:s', 'rangemax:s', 'target:s', 'addtable!', 'difftarget!', 'nodifftarget!', 'addlocation!', 'rawmode!', 'listmode!', 'checkfile:s', 'usep4!' );
if ( @ARGV > 1 ) {
    print "Only single message file allowed\n";
    exit;
}
$Messages = @ARGV[0];
$RangeMin = $opt_rangemin  if ( defined $opt_rangemin );
$RangeMax = $opt_rangemax  if ( defined $opt_rangemax );
$Target   = $opt_target        if ( defined $opt_target );
$AddLocation = $opt_addlocation if ( defined $opt_addlocation );
$AddTable = $opt_addtable  if ( defined $opt_addtable );
$DiffTarget = "Default" if ( !defined $opt_nodifftarget );
$RawMode = $opt_rawmode if ( defined $opt_rawmode );
$ListMode = $opt_listmode if ( defined $opt_listmode );
$CheckFile = $opt_checkfile if ( defined $opt_checkfile );
$UseP4 = $opt_usep4 if ( defined $opt_usep4 );

if ( defined $UseP4 ) {
        use P4Lib;
        $DiffTarget = "P4";
}

#
# Need at least an input file...
#
if ( !defined $Messages )
{
	print "Usage: genmsg message_file [-rangemin min] [-rangemax max] [-target dir] [-difftarget] [-addlocation]\n";
        print "                                                   [-addtable] [-rawmode] [-listmode] [-checkfile file] [-usep4]\n\n";
	print "${IdString}: Generate header files from message file\n";
	print "Options: rangemin    Check and use minimum range for Ids (use 0 if not set)\n";
	print "         rangemax    Check and use maximum range for Ids (don't check if not set)\n";
	print "         target      Target directory for output files\n";
	print "         nodifftarget  Switching off check generated header files for differences with target\n";
	print "                     If not defined only those header files are overwritten that need\n";
	print "                     to be modified.\n";
	print "         addlocation Add __FILE__,__LINE__ to definition output (Default: not defined).\n";
	print "         addtable    Create additional table information (Default: not defined)\n";
	print "                     If defined use flag -DSAPDB_COMPILE_MESSAGES_IN to compile.\n";
	print "                     Generated table are used for multilanguage support.\n\n";
	print "         rawmode     Create raw define lines (Default: not defined)\n";
	print "                     In RawMode '.gencid' files are expected instead of '.genmsg' files.\n";
	print "                     If defined all other options are ignored except target and difftarget!\n";
	print "         listmode    List all output files to create.\n";
    print "                     If this option was found, all other options are ignored!\n";
	print "         checkfile   Check the given file against the output to be created. Do not create any output.\n";
    print "                     If file to compare with did not exist and useperforce was given, the file is\n";
    print "                     synced from repository (readonly).\n";
	print "         usep4       Use perforce commands to get target header files for comparison. Check them out\n";
    print "                     if needed and check them in again after update is done. usep4 implies difftarget!\n";
	print "Files are generated into \$ENV{TMP} first and moved only into directory\n";
	print "where message file was found if differences exist.\n\n";
	print "A bad formatted message file will be reported without changes in existing\n";
	print "header files.\n";
	exit;
}

$TempDir = $ENV{TMP};
#
# Use different suffix in RawMode and never create Table
#
if ( defined $RawMode )
{
	$MessageFileSuffix = ".gencid";
	undef $AddTable;
} else {
	$MessageFileSuffix = ".genmsg";
}

#
# In listmode
#
if ( defined $ListMode )
{
	undef $AddTable;
}


#
# If no suffix was given, add MessageFileSuffix
#
if ( substr($Messages, -length($MessageFileSuffix), length($MessageFileSuffix)) ne $MessageFileSuffix )
{
	$Messages = $Messages . $MessageFileSuffix;
}

if ( ! -r $Messages ) {
#
# Map pathname using SAPDB directory mapping
#
    $Messages = MapSAPDBPath( $Messages );
    if ( ! -r $Messages ) {
        print "Cannot find message file\n";
        exit;
    }
}

#
# Build tablefile path
#
$TableFile  = substr($Messages, 0, length($Messages)-length($MessageFileSuffix)) . "_MessageMap.cpp";

#
# Add file separator to Target if given
#
if ( defined $Target )
{
	$Position = rindex($Target, "/");
	#
	# For Windows file names...
	#
	if ( $Position <= 0 ) { $Position = rindex($Target, "\\"); }
	if ( $Position != length( $Target ) )
	{
		$Target = $Target . "/";
	}
}

#
# Create TableName and Target from Messages file name
#
$Position = rindex($Messages, "/");
#
# For Windows file names...
#
if ( $Position <= 0 ) { $Position = rindex($Messages, "\\"); }
if ( $Position > 0 ) {
	$TableName  = substr($TableFile,  $Position + 1, length($TableFile) - ($Position+1) - length("_MessageMap.cpp"));
        $Target = substr($TableFile, 0, $Position + 1) if ( ! defined( $Target ) ) ;
} else {
	$TableName  = substr($TableFile,  0, length($TableFile) - length("_MessageMap.cpp"));
        $Target = "" if ( ! defined $Target ) ;
}

#######################################################################################
#
# Prepare TABLE output file (Message header files are prepared when first time opened
#
if ( defined $AddTable ) {
  open(TABLE, ">$TableFile") || die ( "Can´t create table file\n");
  print TABLE  "// Generated Table Definition file.\n";
  print TABLE  "// Generated by ${IdString} DO NOT EDIT\n";
  print TABLE  "#include \"SAPDBCommon/SAPDB_Message.hpp\"\n\n";
  print TABLE  "SAPDB_MessageMap * ${TableName}_MessageMap = NULL;\n\n";
  print TABLE  "void ${TableName}_MessageMapInit()\n{\n";
  print TABLE  "        ${TableName}_MessageMap = new SAPDB_MessageMap(\"$TableName\");\n\n";
}

#
# Initialize arrays for output files
#
%OutputFileName=();
%AllOutputFileName=();
%COutputWanted=();
%AllChecksums=();
%CPlusPlusMessageType=();
$CPlusPlusMessageType{"ERROR"} = "SAPDBErr_MessageList::Error";
$CPlusPlusMessageType{"WARNING"} = "SAPDBErr_MessageList::Warning";
$CPlusPlusMessageType{"INFO"} = "SAPDBErr_MessageList::Info";
%CMessageType=();
$CMessageType{"ERROR"}   = "SAPDB_MESSAGE_TYPE_ERROR";
$CMessageType{"WARNING"} = "SAPDB_MESSAGE_TYPE_WARNING";
$CMessageType{"INFO"}    = "SAPDB_MESSAGE_TYPE_INFO";
#
# Initialize global flags
#
$ParseFailed = 0;
$MaxMessageIdLength = 0;
#######################################################################################
# Padding space   12345678901234567890123456789012
$PaddingSpaces = "                                                                ";
$MaxPaddingSpaces = length($PaddingSpaces);
#######################################################################################
#
# First Scan through the Message File
#
# Build the 'UsedIds' array to allow automatic generation of unused MessageNo's
#
%UsedIds = ();

open(MESSAGES, "<$Messages") || die ( "Can´t open message file '$Messages'\n");
LINE: while (<MESSAGES>) {
	next LINE if /^\s*#/;	# discard comments
	next LINE if /^\s*\n$/;	# discard empty lines

	chomp;

	SWITCH: {
		# Add to list of current output header files
		if ( /^\s*>\+([\w,]*)/ )
			{ last SWITCH; }
		# Remove from list of current output header files
		if ( /^\s*>\-([\w,]*)/ )
			{ last SWITCH; }
		# Set list of current output header files
		if ( m/^\s*>([\w,]*)/ )
			{ last SWITCH; }
		if ( m/^\s*TYPE\s*=\s*\w+.*$/ ) {
			if ( defined $RawMode )
			{
				print "TYPE not allowed in raw mode: $_\n";
				$ParseFailed++;
			        last SWITCH;
			}
			else
			{
				last SWITCH;
			}
		}

		undef $Message;
		if ( m/^\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
			$MessageId = $_;
			$MessageId =~ s/^\s*(\w+).*$/$1/;
			$MessageId =~ s/^_(\w+)/${TableName}_$1/;
			if ( length($MessageId) > $MaxMessageIdLength ) {
				$MaxMessageIdLength = length($MessageId);
			}
		}
		else
		{
			if ( m/^\s*\w+\s*,\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
				if ( defined $RawMode )
				{
					print "Messagenumber not allowed in raw mode: $_\n";
					$ParseFailed++;
				}
				$MessageId = $_;
				$MessageId =~ s/^\s*(\w+).*$/$1/;
				$MessageId =~ s/^_(\w+)/${TableName}_$1/;
				if ( length($MessageId) > $MaxMessageIdLength ) {
					$MaxMessageIdLength = length($MessageId);
				}
				$MessageNo = $_;
				$MessageNo =~ s/^\s*\w+\s*,\s*(\w+).*$/$1/;
				CheckAndVerifyMessageNo($MessageNo, $MessageId, $RangeMin, $RangeMax, $ParseFailed);
			}
			else
			{
				print "Syntax error in line $_\n";
				$ParseFailed++;
			}
		}
	}
}
close(MESSAGES);

######################################################################
# Limit the number of padding spaces up to maximum size of message id
if ( (1 + $MaxMessageIdLength) < $MaxPaddingSpaces ) {
	$MaxPaddingSpaces = 1 + $MaxMessageIdLength;
}

$OutputList = undef;

#######################################################################################
#
# Second Scan through the Message File (skipped of AddTable not defined or RawMode)
#
# For each Message Output File add the '#ifndef SAPDB_COMPILE_MESSAGES_IN' entries.
# For each Message add a 'AddEntry'-line to the TABLE file.
#
if ( defined $AddTable ) {
	$MsgType = "Error";
	CheckMsgType($MsgType,$MsgTypeIdentifier, $ParseFailed);
	$MsgNo = $RangeMin;

	open(MESSAGES, "<$Messages") || die ( "Can´t open message file '$Messages'\n");
	LINE: while (<MESSAGES>) {
		next LINE if /^\s*#/;	# discard comments

		chomp;

		SWITCH: {
			# Add to list of current output header files
			if ( m/^\s*>\+([\w.,]*)/ )
				{ $NewOutputList = $OutputList.",".$1; ModifyOutputList(); last SWITCH; }
			# Remove from list of current output header files
			if ( m/^\s*>\-([\w.,]*)/ )
				{ RemoveFromOutputList($1); last SWITCH; }
			# Set list of current output header files
			if ( m/^\s*>([\w.,]*)/ )
				{ $NewOutputList = $1;                 ModifyOutputList(); last SWITCH; }
			if ( m/^\s*TYPE\s*=\s*\w+.*$/ )
				{ $MsgType = $_; $MsgType =~ s/^\s*TYPE\s*=\s*(\w+).*$/$1/;
								 CheckMsgType($MsgType, $MsgTypeIdentifier, $ParseFailed);         last SWITCH; }

			undef $Message;
			if ( m/^\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
				$MessageId = $_;
				$MessageId =~ s/^\s*(\w+).*$/$1/;
				$MessageId =~ s/^_(\w+)/${TableName}_$1/;
				NextMessageNo($MsgNo, $MessageNo, $MessageId, $RangeMax, $ParseFailed);
				$Message = $_;
				$Message =~ s/^\s*\w+\s*,\s*("[^"]*")\s*$/$1/;
			} else {
				if ( m/^\s*\w+\s*,\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
					$MessageId = $_;
					$MessageId =~ s/^\s*(\w+).*$/$1/;
					$MessageId =~ s/^_(\w+)/${TableName}_$1/;
					$MessageNo = $_;
					$MessageNo =~ s/^\s*\w+\s*,\s*(\w+).*$/$1/;
					$Message = $_;
					$Message =~ s/^\s*\w+\s*,\s*\w+\s*,\s*("[^"]*")\s*$/$1/;
				}
			}

			if ( defined $Message ) {

				undef $WantAtLeastOneCPlusPlusOutputFile;
        		foreach $Output (keys(%OutputFileName)) {
					if ( defined $OutputFileName{$Output}
					 && !defined $COutputWanted{$Output} ) {
						$WantAtLeastOneCPlusPlusOutputFile = "YES";
					}
                }

				if ( !defined $OutputList ) {
					$NewOutputList = $TableName;
					ModifyOutputList();
				}

				if ( !defined $RawMode && defined $WantAtLeastOneCPlusPlusOutputFile ) {
					CheckMessage($Message, $MessageId, $MsgArgs, $ParseFailed);
				}
				#
				# Message Number not specified in Message File
				if ( length($MessageId) < $MaxPaddingSpaces ) {
					$Padding = substr($PaddingSpaces, 0, $MaxPaddingSpaces - length($MessageId) );
				} else {
					$Padding = " ";
				}
				if ( defined $AddLocation ) {
					$hppDefine = "#define $MessageId$Padding","__FILE__,__LINE__,$MessageNo";
				} else {
					$hppDefine = "#define $MessageId$Padding$MessageNo";
				}
				$hppDefine2 = "#define $MessageId"."_ID$Padding$MessageNo";
                if ( !defined $ListMode )
                {
        			foreach $Output (keys(%OutputFileName)) {
        				open ( HEADER, ">>$TempDir/$Output");
							if ( defined $COutputWanted{$Output} ) {
		        				print HEADER "$hDefine\n";
							} else {
		        				print HEADER "$hppDefine\n";
							}
		                	print HEADER "$hppDefine2\n";
				        close ( HEADER );
                    }
				}
				if ( defined $AddTable && defined $WantAtLeastOneCPlusPlusOutputFile ) {
					#
					# Table Entry generated
					#
					$tableEntry = " $MessageNo,$Message,$CPlusPlusMessageType{$MsgTypeIdentifier},$MsgArgs ";
					print TABLE  "        ${TableName}_MessageMap->AddEntry($tableEntry);\n";
				}
			}
		}
	}
	close(MESSAGES);

	AddSeparatorToFiles();
}

#######################################################################################
#
# Third and last Scan through the Message File
#
# For each Message Output File add the entries containing the messages.
#
$MsgType = "Error";
CheckMsgType($MsgType, $MsgTypeIdentifier, $ParseFailed);
$MsgNo = (defined $RangeMin ? $RangeMin : 20000);

$OutputList = undef;

open(MESSAGES, "<$Messages") || die ( "Can´t open message file '$Messages'\n");
LINE: while (<MESSAGES>) {
	next LINE if /^\s*#/;	# discard comments

	chomp;

	SWITCH: {
		# Add to list of current output header files
		if ( m/^\s*>\+([\w.,]*)/ )
			{ $NewOutputList = $OutputList.",".$1; ModifyOutputList(); last SWITCH; }
		# Remove from list of current output header files
		if ( m/^\s*>\-([\w.,]*)/ )
			{ RemoveFromOutputList($1); last SWITCH; }
		# Set list of current output header files
		if ( m/^\s*>([\w.,]*)/ )
			{ $NewOutputList = $1;                 ModifyOutputList(); last SWITCH; }
		if ( m/^\s*TYPE\s*=\s*\w+.*$/ )
			{ $MsgType = $_; $MsgType =~ s/^\s*TYPE\s*=\s*(\w+).*$/$1/;
							 CheckMsgType($MsgType, $MsgTypeIdentifier, $ParseFailed);         last SWITCH; }

		undef $Message;
		if ( m/^\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
			$MessageId = $_;
			$MessageId =~ s/^\s*(\w+).*$/$1/;
			$MessageId =~ s/^_(\w+)/${TableName}_$1/;
			NextMessageNo($MsgNo, $MessageNo, $MessageId, $RangeMax, $ParseFailed);
			$Message = $_;
			$Message =~ s/^\s*\w+\s*,\s*("[^"]*")\s*$/$1/;
		} else {
			if ( m/^\s*\w+\s*,\s*\w+\s*,\s*"[^"]*"\s*$/ ) {
				$MessageId = $_;
				$MessageId =~ s/^\s*(\w+).*$/$1/;
				$MessageId =~ s/^_(\w+)/${TableName}_$1/;
				$MessageNo = $_;
				$MessageNo =~ s/^\s*\w+\s*,\s*(\w+).*$/$1/;
				$Message = $_;
				$Message =~ s/^\s*\w+\s*,\s*\w+\s*,\s*("[^"]*")\s*$/$1/;
			}
		}

		if ( defined $Message ) {
			if ( !defined $OutputList ) {
				$NewOutputList = $TableName;
				ModifyOutputList();
			}

			undef $WantAtLeastOneCPlusPlusOutputFile;
        	foreach $Output (keys(%OutputFileName)) {
				if ( defined $OutputFileName{$Output}
				 && !defined $COutputWanted{$Output} ) {
					$WantAtLeastOneCPlusPlusOutputFile = "YES";
				}
            }

			if ( !defined $RawMode && defined $WantAtLeastOneCPlusPlusOutputFile ) {
				CheckMessage($Message, $MessageId, $MsgArgs, $ParseFailed);
			}

			if ( length($MessageId) < $MaxPaddingSpaces ) {
				$Padding = substr($PaddingSpaces, 0, $MaxPaddingSpaces - length($MessageId) );
			} else {
				$Padding = " ";
			}
			if ( defined $RawMode ) {
				if ( defined $WantAtLeastOneCPlusPlusOutputFile ) {
					$hppDefine = "#define $MessageId$Padding$Message";
				}
				$hDefine = "#define $MessageId$Padding$Message";
			} elsif ( defined $AddLocation ) {
				if ( defined $WantAtLeastOneCPlusPlusOutputFile ) {
					$hppDefine = "#define $MessageId$Padding"."__FILE__,__LINE__,$CPlusPlusMessageType{$MsgTypeIdentifier},,$MessageNo,$Message,$MsgArgs";
				}
				$hDefine = "#define $MessageId$Padding$MessageNo,$CMessageType{$MsgTypeIdentifier},$Message";
			} else {
				if ( defined $WantAtLeastOneCPlusPlusOutputFile ) {
					$hppDefine = "#define $MessageId$Padding$CPlusPlusMessageType{$MsgTypeIdentifier},$MessageNo,$Message,$MsgArgs";
				}
				$hDefine = "#define $MessageId$Padding$MessageNo,$CMessageType{$MsgTypeIdentifier},$Message";
			}
			$hppDefine2 = "#define $MessageId"."_ID $MessageNo";
            if ( !defined $ListMode )
            {
        		foreach $Output (values(%OutputFileName)) {
        			open ( HEADER, ">>$TempDir/$Output");
						if ( defined $COutputWanted{$Output} ) {
		        			print HEADER "$hDefine\n";
						} else {
		        			print HEADER "$hppDefine\n";
						}
		        		if ( !defined $RawMode ) {
				        	print HEADER "$hppDefine2\n";
			        	}
        			close ( HEADER );
        		}
			}
		}
	}
}
close(MESSAGES);

#######################################################################################
#
# Close all output files
#
if ( defined $AddTable ) {
	print TABLE  "}\n";
	close(TABLE);
}

CloseOutputFiles($ParseFailed);
#
#######################################################################################
1;
