#!/usr/bin/perl
# @(#) buildmsg.pl (&gar)
#
# Tool for generation of Mesagefiles
#
#    ========== licence begin  GPL
#    Copyright (C) 2004 SAP AG
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

package BuildMsg;

use DBI;
use strict;
use File::Path;
use Getopt::Long;
use Cwd;
use P4Lib;



#############################
# GLOBALS
#############################
my $verbose = 0;
my $dbh = undef;
my $progname = $0;
$progname=~ s,(.*/)|(.*\\),,;
my %Languages = (); 
my $error_count = 0;

my $all_actions = undef;
my $relative_msgfilepath = "sys/src/SAPDB/SAPDBCommon/Messages";
my $file_prefix = "SDBMsg_";
my $msgdefine_prefix = "#define SDBMSG_";
my $tagdefine_prefix = "#define SDBMSGTAG_";
my $release_define = undef;
my $msgfilepath = undef;
my $user = undef;
my $forced_changelist = undef; 
my $xml_checkout = undef;
my $schema_version = undef;
my %supported_schema_versions = (
			                          #'1.0' => 1,
			                          '2.0' => 1
			                       );

my %Opts;

my %xml_substitition = (
                          '&' => '&amp;',
                          '<' => '&lt;'
                       );
my $xml_search_string = join "|", (keys %xml_substitition);

#my %c_substitition = (
#                          '%' => '%%',
#                          '\\' => '\\\\'
#                       );
#my $c_search_string = join "|", (keys %c_substitition);                       

my $mac_filename = "XMLMessages.mac";
my @mac_filelist = ();
my %current_date = ();
my $msg_version_number = undef;
my $releaseid          = undef;
my %action_hash ;
my $force_header_creation = undef;
my $force_xml_creation = undef;
my @files_to_submit = ();
my $submit_text = undef; 


my $begin_of__all_def_line = "/* Begin of all message defines for Component '<component>' -> */";
my $begin_of_msg_def_line = "/* Begin of defines for message <definename>: ID = <msgid>, TAGS = <tagcount> -> */";
my $end_of_msg_def_line = "/* <- End of message defines for component '<definename>' */";
my $end_of_all_def_line = "/* <- End of message defines for component '<component>' */";
#
#	/* Begin of defines for message <DEFINENAME>: ID = <id>,  TAGS = <count> -> */ 
#	....
#   /* <- End of defines for message <DEFINENAME> */
#   /* <- End of message defines for component <Component> */


#############################
# FUNCTIONS
#############################


######################################################
#
# GetLicenceText
#
# Writes final words into header files and copies them if no parsing error occured.
#
#
sub GetLicenceText {
	my $type = shift;
	dbgdbg("Generate licence text\n", 2);
	my ($year, $mon, $day) = get_db_date('date');
	my ($result, $relver);
	$relver = $ENV{'RELVER'};
	if ((substr ($relver, 0, 1) eq 'R') && ($relver ge 'R75')) {
		$result = <<EOF;
        ========== licence begin  GPL
        Copyright (c) $year SAP AG

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
EOF
	}
	else {
		$result = <<EOF;
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
EOF
	}
	if ($type =~ /^c$/)
	{
		$result = "/*\n$result\n*/\n";
	}
	elsif ($type =~ /^desc$/)
	{
		$result =~ s/\n(.*)/\n#$1/g;
		$result = "#$result\n";
	}
	return wantarray ? split /\n/, $result : $result;
}

sub get_db_date
{
	my $type = shift;
	# default is all
	(defined $type) or $type = 'all';
	my $datestring = "";
	my @datelist = ();
	unless (defined $current_date{'year'})
	{	
		dbgdbg ("Start request for date of database \n", 1);
		connect_qadb();
		my $cmd = "select TIMESTAMP INTO ? FROM DUAL";
		dbgdbg ("Execute: $cmd\n", 3);
		my $dbs = $dbh->prepare("$cmd") or die "prepare '$cmd' failed $DBI::err $DBI::errstr\n";
		$dbs->bind_param_inout(1, \$datestring, 10) or MaxDBTest::logerror(qq{bind_param_inout failed (undef for column 1, INTEGER) $DBI::err $DBI::errstr});
		$dbs->execute() or die "execute failed $DBI::err $DBI::errstr\n";
		dbgdbg ("Current date at the database is $datestring\n", 2);		
		# create a list and cut milli seconds
		if ($datestring =~ /(\d{4})-(\d{2})-(\d{2})\s*(\d{2}):(\d{2}):(\d{2})/)
		{
			$current_date{'year'}  = $1;
			$current_date{'month'} = $2;
			$current_date{'day'}   = $3;
			$current_date{'hour'}  = $4;
			$current_date{'min'}   = $5;
			$current_date{'sec'}   = $6;          
		}		
	}
	if ($type =~ /(all|date)/)
	{
		$datestring = "$current_date{'year'}-$current_date{'month'}-$current_date{'day'}";
		@datelist   = ($current_date{'year'}, $current_date{'month'}, $current_date{'day'});
	}
	($type =~ /(all)/) and $datestring .= " ";
	if ($type =~ /(all|time)/)
	{
		push @datelist, ($current_date{'hour'}, $current_date{'min'}, $current_date{'sec'});
		$datestring .= "$current_date{'hour'}:$current_date{'min'}:$current_date{'sec'}";
	}
	return wantarray ? @datelist : $datestring;
}

sub convert_to_c
{
	my $text  = shift;
	$text =~ s/\\/\\\\/g;
	$text =~ s/\"/\\\"/g;
	return $text;
}

sub my_error
{
	my $text = shift;#
	my $rc = shift;
	print "---------------------------------------------\n";
	print "ERROR: $text\n";
	print " => Creation aborted\n";
	exit (defined $rc) ? $rc : 1;
}

sub dbgdbg
{
	my $text = shift;
	my $level = shift;
	my $DBG_PREFIX = $level == 0 ? "" : ( $level == 1 ? "  INFO:" : "  DBG$level:" );
	my $linelength = 79 - length ($DBG_PREFIX);
	$level = 0 unless (defined $level);	
	if ($level <= $verbose)
	{	
		my $crlf = ($text =~ /\n$/) ? 1 : 0; 
		my @lines = split /\n/, $text;
		my $line = undef;
		my $output = "$DBG_PREFIX";
		foreach $line (@lines)
		{
			if (length ($line) > 0)
			{
				while (length ($line) > $linelength)
				{
					$output .= substr $line, 0, $linelength-1;
					$line =  substr $line, $linelength-1; 
					print "$output\\\n";
					$output  = " " x length ($DBG_PREFIX);
				}	
				print "$output$line";
			}		
		}
		if ($crlf || length ($DBG_PREFIX) > 0)
		{ print "\n";}
	}
}

sub connect_qadb
{
	unless ($dbh)
	{
		dbgdbg("Connecting to the database ... ");		
		$dbh = DBI->connect("DBI:MaxDB:PTS/QADB","MSG_USER","MSG_USER") or die "Can't connect $DBI::err $DBI::errstr\n";
		#$dbh = DBI->connect("DBI:MaxDB:BERD80054596A/T_QADB","MSG_USER","MSG_USER") or die "Can't connect $DBI::err $DBI::errstr\n";
		dbgdbg("successfully\n");
	}
}

# 
# get_component_filename (component)
# 
# 
sub get_component_data 
{
	my $component = shift;
	my %component_data = ();
	
	connect_qadb();
	
	dbgdbg ("look for component info for '$component' in MSG_COMPONENTS\n", 2 );
	my $cmd = "SELECT COMPONENT,DESCRIPTION FROM MSG_COMPONENTS WHERE COMPONENT = '$component'";
	dbgdbg ("Execute: $cmd\n", 3);
	($component_data{'COMPONENT'},$component_data{'DESCRIPTION'}) = $dbh->selectrow_array($cmd);
	if ( ! $component_data{'COMPONENT'} )
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{  
			dbgdbg ("Warning: no component '$component' found\n", 1);
			return undef;
		}
	}
	else
	{
		$component_data{'DEFINENAME'} = $component_data{'COMPONENT'};
		$component_data{'DEFINENAME'} =~ tr/[a-z]/[A-Z]/;		
		get_header_file_name (\%component_data);
		get_xml_file_name (\%component_data);
		return \%component_data;
	}	
}

sub get_taghash_for_component
{
	my $component = shift;
	my $releaseid = get_releaseid();
	
	my $cmd = "SELECT MESSAGEID, TAG, MANDATORY FROM V_MSG_REL_TAGS_MO WHERE COMPONENT='$component' and RELEASE = $releaseid";
	my %tag_hash;
	
	connect_qadb();
	dbgdbg ("Look for existing tags for $component (Releaseid:$releaseid) in tag table\n", 1);
	dbgdbg ("Execute: $cmd\n", 3);
	my $ref = $dbh->selectall_arrayref($cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "get_taghash_for_component: selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{  	dbgdbg ("Info: no tags for component '$component' (Releaseid:$releaseid) found\n", 1); }
	}
	else
	{
		foreach my $row (@$ref) 
		{	
			# mandatory
			if (@$row[2])
			{	push @{$tag_hash{@$row[0]}->{'mandatory'}}, @$row[1];	}
			else # optional
			{	push @{$tag_hash{@$row[0]}->{'optional'}}, @$row[1];	}
		}
			
	}	
			
	return %tag_hash;
}

sub get_release_define
{
	unless (defined $release_define)
	{
		if (($ENV{'LC_VER_STR'} =~ /^(\d)(\d)\.(\d+)$/) || ("$ENV{'RELVER'}.$ENV{'CORRECTION_LEVEL'}" =~ /^R(\d)(\d)\.(\d+)$/))
		{
			$release_define = "SDB_RELEASE_0${1}0${2}" . ("0" x (2 - length ($3))). "$3"; 		
		}
		else
		{
			die "Error: Wrong release infomation (LC_VER_STR = $ENV{LC_VER_STR}, RELVER=$ENV{RELVER},CORRECTION_LEVEL= $ENV{CORRECTION_LEVEL} )\n";
		}
	}
	return ($release_define);
}

sub get_releaseid
{
	unless (defined $releaseid)
	{
		
		my $cmd = "select ID INTO ? FROM MSG_RELEASES WHERE ";
		if (($ENV{'LC_VER_STR'} =~ /^(\d)(\d)\.(\d+)$/) || ("$ENV{'RELVER'}.$ENV{'CORRECTION_LEVEL'}" =~ /^R(\d)(\d)\.(\d+)$/))
		{
			$cmd .= "MAJOR_RELEASE = '$1' and MINOR_RELEASE = '$2' and CORRECTION_LEVEL='" . ("0" x (2 - length ($3))). "$3'";
			dbgdbg ("Start request for releaseid for release\n", 1);	
			connect_qadb();
			dbgdbg ("Execute: $cmd\n", 3);		
		}
		else
		{
			die "Error: Wrong release infomation (LC_VER_STR = $ENV{LC_VER_STR}, RELVER=$ENV{RELVER},CORRECTION_LEVEL= $ENV{CORRECTION_LEVEL} )\n";
		}
			
		my $dbs = $dbh->prepare($cmd) or die "prepare '$cmd' failed $DBI::err $DBI::errstr\n";
		$dbs->bind_param_inout(1, \$releaseid, 10) or MaxDBTest::logerror(qq{bind_param_inout failed (undef for column 1, INTEGER) $DBI::err $DBI::errstr});
		$dbs->execute() or die "execute failed $DBI::err $DBI::errstr\n";
		dbgdbg ("ReleaseID is $releaseid\n", 2);
	}
	return ($releaseid);
}

sub get_schema_version
{
	unless (defined $schema_version)
	{
		connect_qadb();
		my $cmd = "select SCHEMAVERSION INTO ? FROM MSG_VERSION";
		dbgdbg ("Execute: $cmd\n", 3);
		my $dbs = $dbh->prepare($cmd) or die "prepare '$cmd' failed $DBI::err $DBI::errstr\n";
		$dbs->bind_param_inout(1, \$schema_version, 10) or MaxDBTest::logerror(qq{bind_param_inout failed (undef for column 1, INTEGER) $DBI::err $DBI::errstr});
		$dbs->execute() or die "execute failed $DBI::0err $DBI::errstr\n";
		chomp $schema_version;
		$schema_version =~ s/ //g;
		dbgdbg ("Schema version is '$schema_version'\n", 2);
	}
	return ($schema_version);
}


sub get_message_list_for_header
{
	my $refto_component_data = shift;
	my @msgids = @_;
	my @msg_list = ();
	my $msg_select = (scalar (@msgids) == 0) ? "" : ("AND MESSAGEID in ( " . (join ", ", @msgids) . ") ");
	my $releaseid = get_releaseid();
	connect_qadb();
	my $cmd = "SELECT MESSAGEID, MESSAGEVERSION, DEFINENAME, SHORTTEXT, DESCRIPTION ".
	          "FROM V_MSG_REL_MESSAGES WHERE COMPONENT = '$refto_component_data->{'COMPONENT'}' and LANGUAGE = 'EN' and RELEASE = $releaseid ${msg_select}ORDER BY MESSAGEID";
	dbgdbg ("collect all header infos for '$refto_component_data->{'COMPONENT'}'\n", 1);
	dbgdbg ("Execute: $cmd\n", 3);
	my $ref = $dbh->selectall_arrayref($cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{  	dbgdbg ("Warning: no messages for component '$refto_component_data->{'COMPONENT'}' found\n"); }
	}
	else
	{
		dbgdbg ("Found  ". scalar(@$ref) . " messages \n", 2);
		foreach my $row (@$ref) {
			my %message = ();
			($message{'ID'},$message{'VERSION'},$message{'DEFINENAME'},$message{'SHORTTEXT'},$message{'DESCRIPTION'}) = @$row;
			$message{'COMPONENT_NAME'} = $refto_component_data->{'COMPONENT'};
			$message{'COMPONENT_DEFINE'} = $refto_component_data->{'DEFINENAME'};
			$message{'DESCRIPTION'} =~ s/\015\012/\n/;
	    	dbgdbg ("ADD $message{'DEFINENAME'}($message{'ID'}-$message{'VERSION'}) to msglist\n",2);
	    	dbgdbg ("SHORTTEXT:$message{'SHORTTEXT'}\n", 3);
	    	dbgdbg ("DESCRIPTION:\n$message{'DESCRIPTION'}\n", 3);
	    	push @msg_list, \%message;
		}
	}	
	$refto_component_data->{'MESSAGES'} = \@msg_list;	
	return (@msg_list);
}

sub get_xml_messagelist 
{
	my $refto_component_data = shift;
	my $language = shift;
	$language = 'EN' unless (defined $language);
	my @xml_msg_list = ();
	connect_qadb();
	my $cmd = "SELECT MESSAGEID, MESSAGEVERSION, DEFINENAME, SHORTTEXT, DESCRIPTION, ACTIONNAME ".
	          "FROM V_MSG_XML_MESSAGES WHERE COMPONENT = '$refto_component_data->{'COMPONENT'}' and LANGUAGE = '$language' ORDER BY MESSAGEID, MESSAGEVERSION";
	dbgdbg ("collect all message infos for $refto_component_data->{'COMPONENT'} for language '$language'\n", 1);
	dbgdbg ("Execute: $cmd\n", 3);
	my $ref = $dbh->selectall_arrayref($cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{  	dbgdbg ("Warning: no messages for component $refto_component_data->{'COMPONENT'} found\n"); }
	}
	else
	{
		foreach my $row (@$ref) {
			my %xml_message = ();
			($xml_message{'ID'}, $xml_message{'VERSION'},
			 $xml_message{'DEFINENAME'}, $xml_message{'SHORTTEXT'},
			 $xml_message{'DESCRIPTION'}, $xml_message{'ACTIONNAME'}) = @$row;
			$xml_message{'COMPONENT'} = $refto_component_data->{'COMPONENT'};
	    	dbgdbg ("ADD $xml_message{'DEFINENAME'}($xml_message{'ID'}) to msglist\n",2);
	    	dbgdbg ("SHORTTEXT:$xml_message{'SHORTTEXT'}\n", 3);
	    	dbgdbg ("DESCRIPTION:\n$xml_message{'DESCRIPTION'}\n", 3);
	    	dbgdbg ("ACTIONNAME:\n$xml_message{'ACTIONNAME'}\n", 3);
	    	push @xml_msg_list, \%xml_message;
		}
	}	
	return \@xml_msg_list;
}


sub get_action_text
{
	my $action_name = shift;
	my $action_hash = get_action_hash();
	return $action_hash->{$action_name};
}

sub get_action_hash
{	
	my $language = shift;
	$language = 'EN' unless (defined $language);
	if (scalar(keys %action_hash) == 0)
	{
		connect_qadb();
		my $cmd = "SELECT ACTIONNAME, DESCRIPTION FROM MSG_ACTIONS_TEXT WHERE LANGUAGE = '$language'";
		dbgdbg ("collect all actions for language '$language'\n", 1);
		dbgdbg ("Execute: $cmd\n", 3);
		
		my $ref = $dbh->selectall_arrayref($cmd);
		unless ($ref)
		{
			if ( $DBI::err )
			{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		}
		else
		{
			foreach my $row (@$ref) {
				my ($aname, $atext) = @$row ;
		    	$action_hash{$aname} = $atext;
		    	dbgdbg ("ADD ACTION $aname to action hash\n",2);
		    	dbgdbg ("DESCRIPTION:\n$atext\n", 3);
		    }
		}
	}
	return \%action_hash;
}

sub get_version_number
{	
	unless (defined $msg_version_number)
	{
		$msg_version_number = '12345123';
		dbgdbg ("Start request for new message version number \n", 1);
		connect_qadb();
		dbgdbg ("Execute: select S_MSG_VERSION_NR.NEXTVAL INTO ? FROM DUAL\n", 3);
		my $dbs = $dbh->prepare("select S_MSG_VERSION_NR.NEXTVAL INTO ? FROM DUAL") or die "prepare select NEXTVAL failed $DBI::err $DBI::errstr\n";
		$dbs->bind_param_inout(1, \$msg_version_number, 10) or MaxDBTest::logerror(qq{bind_param_inout failed (undef for column 1, INTEGER) $DBI::err $DBI::errstr});
		$dbs->execute() or die "execute failed $DBI::err $DBI::errstr\n";
		dbgdbg ("Message version number is $msg_version_number\n", 2);
	}
	return $msg_version_number;	
}

########################
# HEADER FILE AS LINES #
########################

#
# open file and return file handle
# create list of lines of the complete file
# 
sub get_header_file_name
{
	my $refto_component_data = shift;	
	defined ($refto_component_data->{'HEADER_FILE_NAME'}) or 
		$refto_component_data->{'HEADER_FILE_NAME'} = "$msgfilepath/$file_prefix$refto_component_data->{'COMPONENT'}".".h";
	return ($refto_component_data->{'HEADER_FILE_NAME'});
}

sub read_headerfile
{
	my $refto_component_data = shift;
	my @header_lines = ();
	my $file = get_header_file_name ($refto_component_data);
	( -f $file ) or 
		die "Header file '$file' does not exist \n"; 
	
	@header_lines = read_file ($file);
	$refto_component_data->{'OLD_HEADER_LINES'} = \@header_lines;
	return (@header_lines);
}

sub read_xml_file
{
	my $refto_component_data = shift;
	my @xml_lines = ();
	my $xml_file_name = get_xml_file_name ($refto_component_data);
	if ( -f $xml_file_name ) 
	{
		@xml_lines = read_file ($xml_file_name);
	}
	else
	{
		dbgdbg ("XML file '$xml_file_name' does not exist \n", 1); 
	}
	return (@xml_lines);
}

sub read_file
{
	my $filename = shift;
	my @lines = ();
	open(IN_FILE , "<$filename" ) || die ( "Can't read file $filename\n" );
	while (<IN_FILE>) 
	{
		chomp;
		push @lines, $_;
	}
	close (IN_FILE);
	return (@lines);
}

 

sub write_header_file
{
	 my $refto_component_data = shift;
	 my $filename = get_header_file_name($refto_component_data);
	 dbgdbg ("Write header file for component $refto_component_data->{'COMPONENT'}\n", 1);
	 checkout($filename, 1);
	 dbgdbg ("Write header text in $filename\n", 1);
	 open(NEW_HEADER_FILE , ">$filename" ) || 
	 		die ( "Can't write header file $filename\n" );
	 foreach  (@{$refto_component_data->{'HEADER_LINES'}})
	 { 	print NEW_HEADER_FILE "$_\n"; }
	 close (NEW_HEADER_FILE);
} 

#	/* Begin of all message defines for Component '<Component>' -> */
#
#	/* Begin of defines for message <DEFINENAME>: ID = <id>, VERSION = <vid>, TAGS = <count> -> */ 
#	....
#   /* <- End of defines for message <DEFINENAME> */
#	/* Begin of defines for message <DEFINENAME>: ID = <id>, VERSION = <vid>, TAGS = <count> -> */ 
#	....
#   /* <- End of defines for message <DEFINENAME> */

#   /* <- End of message defines for component <Component> */
#
# return (%hash);
# if ID undef no
sub new_headerfile
{	
	my $refto_component_data = shift;
	# the begin will be always regenerated 
	my $file = get_header_file_name ($refto_component_data);
	( -f $file ) or sync ($file);
	dbgdbg ("Create header file for component $refto_component_data->{'COMPONENT'}\n");
	my @new_headerlines = get_header_intro($refto_component_data);
	push @new_headerlines, "/* Begin of all message defines for component $refto_component_data->{'COMPONENT'} -> */";
	my @new_msg_list = @{$refto_component_data->{'MESSAGES'}};
	my $TilosEnumerationString = "";
	my $TilosInitListString = "";
	foreach my $message (@new_msg_list)
	{	
		push @new_headerlines, create_msgid_area ($message);	
		add_enum_defines (\$TilosEnumerationString, \$TilosInitListString, $message);
	}	
	push @new_headerlines, "/* <- End of message defines for component $refto_component_data->{'COMPONENT'} */";
	# and now grandpa's enumerations
	push @new_headerlines, create_enums_defines(\$TilosEnumerationString, \$TilosInitListString);
	push @new_headerlines, create_header_end($refto_component_data);
	$refto_component_data->{"HEADER_LINES"} = \@new_headerlines;
	write_header_file($refto_component_data);	
	dbgdbg ("=> Finished successfully\n"); 
}

sub modify_headerfile
{	
	my $refto_component_data = shift;
	my $file = get_header_file_name ($refto_component_data);
	dbgdbg ("Modify header file for component $refto_component_data->{'COMPONENT'}\n");
	# sync if not exists 
	unless (( -f $file ) or sync ($file)) 
	{
		dbgdbg ("Header file does not exists - force regeneration\n");
		new_headerfile($refto_component_data);
	}
	else
	{
		my @old_headerlines = read_headerfile ($refto_component_data);
		# the begin will be always regenerated 
		my @new_headerlines = get_header_intro($refto_component_data);
		my $line;
		my $next_msg_define;
		# look for the begin of message defines
		while (defined ($line = shift @old_headerlines))
		{
			chomp;
			if ($line =~ /\/\*\s*Begin of all message defines for component.*->\s*\*\//)
			{	
				# found
				push @new_headerlines, $line;	
				last;
			}
		}
		unless (defined $line) 
		{
			my_error  ("Missing begin tag for message definitions\n".
			           "       Existing header file is inconsistent and must be regenerated completely!");	
		}
		
		my @new_msg_list = @{$refto_component_data->{'MESSAGES'}};
		my $TilosEnumerationString = "";
		my $TilosInitListString = "";
	
		while ($next_msg_define = read_next_msgid_area(\@old_headerlines))
		{
			while ((scalar (@new_msg_list) > 0) and  ($next_msg_define->{'ID'} > $new_msg_list[0]->{'ID'} )) 
			{
				push @new_headerlines, create_msgid_area ($new_msg_list[0]);
				add_enum_defines (\$TilosEnumerationString, \$TilosInitListString, $new_msg_list[0]);
				shift @new_msg_list;
			}
			
			if ((scalar (@new_msg_list) > 0) and  ($next_msg_define->{'ID'} == $new_msg_list[0]->{'ID'} )) 
			{
				push @new_headerlines, create_msgid_area ($new_msg_list[0]);
				add_enum_defines (\$TilosEnumerationString, \$TilosInitListString, $new_msg_list[0]);
				shift @new_msg_list;
			}
			else
			{
				push @new_headerlines, @{$next_msg_define->{'LINES'}};
				$next_msg_define->{'COMPONENT_NAME'} = $refto_component_data->{'COMPONENT'};
				$next_msg_define->{'COMPONENT_DEFINE'} = $refto_component_data->{'DEFINENAME'};
				add_enum_defines (\$TilosEnumerationString, \$TilosInitListString, $next_msg_define);
			}
		}
		# the rest at the end
		foreach my $message (@new_msg_list)
		{	
			push @new_headerlines, create_msgid_area ($message);
			add_enum_defines (\$TilosEnumerationString, \$TilosInitListString, $message);	
		}
		push @new_headerlines, "/* <- End of message defines for component $refto_component_data->{'COMPONENT'} */";
		push @new_headerlines, create_enums_defines(\$TilosEnumerationString, \$TilosInitListString);
		push @new_headerlines, create_header_end($refto_component_data);
		$refto_component_data->{'HEADER_LINES'} = \@new_headerlines;
		my $rc = count_differences (\@new_headerlines, $refto_component_data->{"OLD_HEADER_LINES"});
		if ($rc > 0)
		{
			dbgdbg ("Files are different equal\n", 2);
			write_header_file($refto_component_data);
			dbgdbg ("=> Finished successfully\n"); 
		}
		else
		{
			dbgdbg ("=> No changes in header file found. Generation of header file skipped\n"); 
		}
	}
}

sub get_last_changedate
{
	my $refto_component_data = shift;
	my $component = $refto_component_data->{'COMPONENT'};
	my $changedate = 0;
	my $cmd = "SELECT MAX (CHANGEDATE) INTO ? FROM (SELECT CHANGEDATE from MSG_MESSAGE_VERSIONS where COMPONENT = '$component' ".
	          "UNION ALL SELECT A.CHANGEDATE FROM MSG_ASSIGN_MSG_ACTION M, MSG_ACTIONS_TEXT A WHERE M.COMPONENT = '$component' ".
			  "and A.ACTIONNAME = M.ACTIONNAME) t1";
	connect_qadb();
	dbgdbg ("Execute: $cmd\n", 3);		
	my $dbs = $dbh->prepare($cmd) or die "prepare '$cmd' failed $DBI::err $DBI::errstr\n";
	$dbs->bind_param_inout(1, \$changedate, 10) or MaxDBTest::logerror(qq{bind_param_inout failed (undef for column 1, INTEGER) $DBI::err $DBI::errstr});
	$dbs->execute() or die "execute failed $DBI::err $DBI::errstr\n";
	$refto_component_data->{'CHANGEDATE'} = $changedate;
	($changedate =~ /^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\.\d*/) and $changedate = $1;
	dbgdbg ("Changedate is $changedate\n", 2);
	return ($changedate);
}

sub count_differences
{
	my $ref_list1 = shift;
	my $ref_list2 = shift;
	my $diffcount = 0;
	my $linecount = scalar(@$ref_list1);
	dbgdbg ("Check differences beetween the original and the generated file\n", 1);
	# first check the same count of lines
	if ($linecount != scalar(@$ref_list2))
	{			
		dbgdbg ("Different count of lines ($linecount:".int(scalar(@$ref_list2)).")\n", 2);	
		$diffcount = abs($linecount-int(scalar(@$ref_list2))) ;
	}
	else
	{
		my $currentline;
		for($currentline = 0; $currentline < $linecount; $currentline++) 
		{ 
			unless ("$ref_list1->[$currentline]" eq "$ref_list2->[$currentline]")
			{
				# check for date
				my $string1 = $ref_list1->[$currentline];
				my $string2 = $ref_list2->[$currentline];
				# header file timestamp differences
				$string1 =~ s/date([:\s]+)\d{4}-\d{2}-\d{2}(\s+)\d\d:\d\d:\d\d/date${1}YYYY-MM-DD${2}HH:MM:SS/g;
				$string2 =~ s/date([:\s]+)\d{4}-\d{2}-\d{2}(\s+)\d\d:\d\d:\d\d/date${1}YYYY-MM-DD${2}HH:MM:SS/g;
				# xml file time and version differnces 
				$string1 =~ s/(Metadata generated=\")\d{4}-\d{2}-\d{2}(\s+)\d\d:\d\d:\d\d(\".*versionnr=\")\d+(\")/${1}YYYY-MM-DD${2}HH:MM:SS${3}XXX${4}/g;
				$string2 =~ s/(Metadata generated=\")\d{4}-\d{2}-\d{2}(\s+)\d\d:\d\d:\d\d(\".*versionnr=\")\d+(\")/${1}YYYY-MM-DD${2}HH:MM:SS${3}XXX${4}/g;
				if ( $string1 eq $string2 )
				{	dbgdbg ("Found lines different only by date:\n$ref_list1->[$currentline]\n$ref_list2->[$currentline]\n", 2); }
				else
				{	
					$diffcount++;
					dbgdbg ("Found Differences:\n $ref_list1->[$currentline]\n $ref_list2->[$currentline]\n", 2);	
				}
			}
		}
		#for 
	}
	dbgdbg (($diffcount ? "Found $diffcount differences\n" : "No differences found\n"), 1);
	return $diffcount;
}

####################
# HEADER START/END #
####################
sub get_header_intro
{
	my $refto_component_data = shift;
	my @new_intro_lines = ();
	my $UppercaseHeader = "$file_prefix$refto_component_data->{'COMPONENT'}_H";
	$UppercaseHeader =~ tr/[a-z]/[A-Z]/;
	dbgdbg("Generating header intro for $refto_component_data->{'COMPONENT'}...\n", 2);			
	@new_intro_lines = GetLicenceText ("c");
	push @new_intro_lines, ("","#ifndef $UppercaseHeader");
	push @new_intro_lines, "#define $UppercaseHeader";
	push @new_intro_lines, ("/*","This header file was generated by $progname");
	push @new_intro_lines, "--- Do not edit....","*/";
	dbgdbg("Insert message defines in header ...\n", 2);
	push @new_intro_lines, "#ifdef __cplusplus";
	push @new_intro_lines, "#ifndef MSG_LIST_HPP";
	push @new_intro_lines, "#include \"SAPDB/Messages/Msg_List.hpp\"";
	push @new_intro_lines, "#endif";
	push @new_intro_lines, "#ifndef MSG_ARG_HPP";
	push @new_intro_lines, "#include \"SAPDB/Messages/Msg_Arg.hpp\"";
	push @new_intro_lines, "#endif";
	push @new_intro_lines, "#ifndef MSG_SIMPLEARG_HPP";
	push @new_intro_lines, "#include \"SAPDB/Messages/Msg_SimpleArg.hpp\"";
	push @new_intro_lines, "#endif";
	push @new_intro_lines, ("#endif","");
	# check of right release
	push @new_intro_lines, "#ifndef ".get_release_define(). "  /* don't merge this define - only this date: " .get_db_date('all'). " */";
	push @new_intro_lines, split /\n/, "#error $file_prefix$refto_component_data->{'COMPONENT'}.h was probably integrated between releases, do not integrate this header file, \\\nuse buildmsg";
	push @new_intro_lines, ("#endif","","");		
	push @new_intro_lines, "#define SDBMSG_$refto_component_data->{'DEFINENAME'}_COMPONENT \"$refto_component_data->{'COMPONENT'}\"";
	return 	(@new_intro_lines);
}

sub create_header_end
{
	my $refto_component_data = shift;
	my @new_end_lines = ();
	my $UppercaseHeader = "$file_prefix$refto_component_data->{'COMPONENT'}_H";
	$UppercaseHeader =~ tr/[a-z]/[A-Z]/;
	push @new_end_lines, split /\n/, "\n#endif /* $UppercaseHeader */\n";
	return (@new_end_lines);
}

######################
# MESAGE/TAG DEFINES #
######################
sub read_next_msgid_area
{
	my $ref_to_lines = shift;
	my $line;
	my %message_hash;
	my $found_begin_tag = 0;
	
	while ( $line = shift (@$ref_to_lines) )
	{
		chomp $line;
		if ($found_begin_tag)
		{
			push @{$message_hash{'LINES'}}, $line;
			if ($line =~ /\/* <- End of defines for message $message_hash{'DEFINENAME'}\s*\*\//)
			{	
				return (\%message_hash);	
			}
		}
		else
		{
			if ($line =~ /\/* End of message defines for component/)
			{
				dbgdbg ("Found end of message defines\n", 2);
				return (undef);
			}
			if ($line =~ /\/\*\s*Begin of defines for message (\S*)\s*: ID\s*=\s*(\d*)\s*,\s*VERSION\s*=\s*(\d*)\s*,\s*TAGS\s*=\s*(\d*)\s*->\s*\*\//) 
			{
				$message_hash{'DEFINENAME'}=$1;
				$message_hash{'ID'}=$2;
				$message_hash{'VERSION'}=$3;
				$message_hash{'TAGS'}=$4;
				$message_hash{'DATE'}=$5;
				push @{$message_hash{'LINES'}}, $line;
				$found_begin_tag = 1;
			}
		}
	}
	$found_begin_tag && print ("Missing end tag for message $message_hash{'DEFINENAME'}\n");
	die ("Existing header file is inconsistent and must be regenerated completely !");	
}

#
#	get hash of a message
#	hash {ID}               -> <msgid>
#        {VERSION}          -> <msgversion>
#        {DEFINENAME}       -> <name>
#        {SHORTTEXT}        -> <shorttext>
#        {LONGTEXT}         -> <longtext>
#        {DATE}             -> <changedate>
#        {TAGS}             -> @taglist
#        {ACTION}           -> %action_hash
#        {COMPONENT_NAME}   -> component name
#        {COMPONENT_NAME}   -> component define
sub create_msgid_area
{
	my $single_message = shift;
	my @new_msg_lines = ();
	my %tag_already_defined ;
	my $tagcount = defined $single_message->{'TAGS'}->{'mandatory'} ? scalar(@{$single_message->{'TAGS'}->{'mandatory'}}) + 1 : 1;
	push @new_msg_lines, "/* Begin of defines for message $single_message->{'DEFINENAME'}: ".
	                     "ID = $single_message->{'ID'}, VERSION = $single_message->{'VERSION'}, ".
	                     "TAGS = $tagcount -> */";
	
	# normal define
	push @new_msg_lines, "$msgdefine_prefix$single_message->{'COMPONENT_DEFINE'}_$single_message->{'DEFINENAME'} ".
	                 "$single_message->{'ID'}, \"$single_message->{'COMPONENT_NAME'}\", __FILE__, __LINE__, ".
	                 "\"".convert_to_c($single_message->{'SHORTTEXT'})."\", (Msg_Has${tagcount}Args*)0, ".
	                 "Msg_SimpleArg(\"_MESSAGEVERSION\", reinterpret_cast<const SAPDB_UTF8*>(\"$single_message->{'VERSION'}\"))";	      
	                 #"Msg_Arg(\"_MESSAGEVERSION\", $single_message->{'VERSION'})";	
	
	foreach ( @{$single_message->{'TAGS'}->{'mandatory'}})
	{
		unless (defined $tag_already_defined{$_})
		{
			push @new_msg_lines, "$tagdefine_prefix$single_message->{'COMPONENT_DEFINE'}_$single_message->{'DEFINENAME'}__$_ \"$_\"";
			$tag_already_defined{$_} = 1;
		}	
	}
	foreach ( @{$single_message->{'TAGS'}->{'optional'}} )
	{
		unless (defined $tag_already_defined{$_})
		{
			push @new_msg_lines, "$tagdefine_prefix$single_message->{'COMPONENT_DEFINE'}_$single_message->{'DEFINENAME'}__$_ \"$_\" /* optional */";
			$tag_already_defined{$_} = 1;
		}	
	}
	              
	push @new_msg_lines, "/* <- End of defines for message $single_message->{'DEFINENAME'} */";      
	return (@new_msg_lines);              
}

######################
# TILOS ENUMERATIONS #
######################
sub create_enums_defines
{
	my $ref_to_enum_string = shift;
	my $ref_to_initlist_string = shift;
	my @new_enum_lines = ();
	push @new_enum_lines, "","/* some helpful enumerations (created by $progname) -> */","";
	push @new_enum_lines, split /\n/, "$$ref_to_enum_string\n\n";
	push @new_enum_lines, split /\n/, "$$ref_to_initlist_string\n\n";
	push @new_enum_lines, split /\n/, "/* <- some helpful enumerations */\n";
	return (@new_enum_lines);
} 

sub add_enum_defines
{
	my $ref_to_enum_string = shift;
	my $ref_to_initlist_string = shift;
	my $single_message = shift;
	add_to_enumeration_string ($ref_to_enum_string, $single_message);
	add_to_initlist_string ($ref_to_initlist_string, $single_message);
		
}

sub add_to_enumeration_string
{
	my $ref_to_enum_string = shift;
	my $single_message = shift;
		
	$$ref_to_enum_string .=  ($$ref_to_enum_string eq "") ? 
	                               "#define SDBMSG_$single_message->{'COMPONENT_DEFINE'}_ENUMERATION " :
	                               ", ";
	$$ref_to_enum_string .= "\\\n                    $single_message->{'COMPONENT_NAME'}_$single_message->{'DEFINENAME'}=$single_message->{'ID'}"; 
}

sub add_to_initlist_string
{
	my $ref_to_initlist_string = shift;
	my $single_message = shift;
	$$ref_to_initlist_string .= ($$ref_to_initlist_string eq "") ? 
	                              "#define SDBMSG_$single_message->{'COMPONENT_DEFINE'}_INITIALIZATIONLIST(x) " :
	                              ", ";
	$$ref_to_initlist_string .=   "\\\n                    x(SDBMSG_$single_message->{'COMPONENT_DEFINE'}_$single_message->{'DEFINENAME'}, \"$single_message->{'DEFINENAME'}\")";                  		
}

######################
# XML FILE GENERATION
# ->
# return 
# 1  - created
# 0  - unchanged
# -1 - error
sub new_messagefile
{
	my $refto_component_data = shift;
	my $language = shift;
	$language = 'EN' unless (defined $language);
	my $db_last_date = get_last_changedate($refto_component_data);
	my $file = get_xml_file_name ($refto_component_data);
	sync ($file, 1);
	my $file_last_date = undef;
	my $new_version_number = undef;
	my @new_xml_lines = ();
	my $xml_msg_list = undef;
	my $xml_msg_info = undef;
	my @old_xml_lines = ();
	
	dbgdbg ("Create message file for component $refto_component_data->{'COMPONENT'}\n");
	
	unless (defined $force_xml_creation)
	{
		# sync now and check out/or change permissions - toDO
		
		@old_xml_lines = read_xml_file($refto_component_data);
		# look for change date
		foreach (@old_xml_lines)
		{
			if (/generated=\"(\d\d\d\d-\d\d-\d\d \d\d\:\d\d:\d\d)\"/)
			{
				$file_last_date = $1;
				last;
			}
		}
		if (defined $file_last_date)  	
		{
			if (("$file_last_date" ge "$db_last_date") and not defined $Opts{'rc'})
			{
				dbgdbg ("There are no changes since $file_last_date\n", 1);
				dbgdbg ("=> Skipped - No changes since last generation\n");
				return (0);
			} 
		}
		else
		{
			dbgdbg ("Can't determine change date of existing xml file\n", 1);
			dbgdbg ("Force regeneration of xml file\n", 1);
		}
	}
	@new_xml_lines = get_xml_intro ();
	$xml_msg_list = get_xml_messagelist ($refto_component_data, $language);
	foreach $xml_msg_info (@$xml_msg_list)
	{	push @new_xml_lines, create_xml_area($xml_msg_info);	}
	push @new_xml_lines, "</MessageDefinitions>";
	$refto_component_data->{'XML_LINES'} = \@new_xml_lines;
	unless (defined $force_xml_creation)
	{
		my $rc = count_differences ($refto_component_data->{'XML_LINES'}, \@old_xml_lines);
		if ($rc > 0)
		{	dbgdbg ("Files are different\n", 3); }
		else
		{	
			dbgdbg ("=> No changes found. Generation of xml file skipped\n"); 
			return (0);
		}
	}
	write_xml_file($refto_component_data);
	dbgdbg ("=> Finished successfully\n"); 
	push @mac_filelist, "./$language/$file_prefix$refto_component_data->{'COMPONENT'}".".xml";
	return;
}

sub get_xml_intro
{
	my @new_xmlintro_lines = ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>", "<MessageDefinitions>");
	push @new_xmlintro_lines, "<Metadata generated=\"". get_db_date() . 
	                     "\" releaseGlobal=\"YES\" versionnr=\"" . get_version_number () . "\"/>";
	return 	(@new_xmlintro_lines);
}

#
#	get hash of a message
#	hash {ID}               -> <msgid>
#        {VERSION}          -> <msgversion>
#        {DEFINENAME}       -> <name>
#        {SHORTTEXT}        -> <shorttext>
#        {LONGTEXT}         -> <longtext>
#        {ACTIONNAME}       -> look for it in the %action_hash
#        {COMPONENT_NAME}   -> component name
sub create_xml_area
{
	my $single_message = shift;
	my @new_xml_lines = ();
	push @new_xml_lines, "<MSG COMP=\"$single_message->{'COMPONENT'}\" ".
	                     "ID=\"$single_message->{'ID'}\" VERSION=\"$single_message->{'VERSION'}\">";
	 
	push @new_xml_lines, "<DefineName>", "$single_message->{'DEFINENAME'}", "</DefineName>";
	push @new_xml_lines, "<ShortText>", "$single_message->{'SHORTTEXT'}", "</ShortText>";
	$single_message->{'DESCRIPTION'} and 
	      ( push @new_xml_lines, '<Description><![CDATA[', ( split /\n/, $single_message->{'DESCRIPTION'}) , ']]></Description>' ); 
	$single_message->{'ACTIONNAME'} and
	      push @new_xml_lines, ('<Action><![CDATA[', (split /\n/, get_action_text($single_message->{'ACTIONNAME'})), ']]></Action>');
	push @new_xml_lines, "</MSG>";
	return (@new_xml_lines);              
}

sub get_xml_file_name
{
	my $refto_component_data = shift;	
	my $language = shift;
	$language = 'EN' unless (defined $language);
	defined ($refto_component_data->{'XML_FILE_NAME'}) or 
		$refto_component_data->{'XML_FILE_NAME'} = "$msgfilepath/$language/$file_prefix$refto_component_data->{'COMPONENT'}".".xml";
			
	return ($refto_component_data->{'XML_FILE_NAME'});
}


sub write_xml_file
{
	 my $refto_component_data = shift;
	 my $filename = get_xml_file_name($refto_component_data);
	 dbgdbg ("Write xml file for component $filename\n", 1);
	 if ($xml_checkout)
	 {	checkout($filename, 0);	 }
	 else
	 {	
	 	revert ($filename, 1);
	  	set_write_permissions($filename, 1); 
	 }
	 dbgdbg ("Write header text in $filename\n", 1);
	 open(NEW_XML_FILE , ">$filename" ) || 
	 		die ( "Can't write xml file $filename\n" );
	 foreach  (@{$refto_component_data->{'XML_LINES'}})
	 { 	print NEW_XML_FILE "$_\n"; }
	 close (NEW_XML_FILE);
	 ($xml_checkout) or set_write_permissions($filename, 0);
} 


sub create_mac_file
{
	my $macfile = "$msgfilepath/$mac_filename";
	dbgdbg ("Check (and recreate) mac description for new message files\n");
	dbgdbg ("mac description is '$macfile'\n", 1);
		
	my ($rc, $already_edit_head_rev) = checkout($macfile);
	if ( $rc == 0 )
	{	
		my %existing_msgfiles = ();
		
		# open existing mac file and parse for existing files
		open( MACFILE, "<$macfile" ) || die ( "Can't read mac description $macfile\n" );
		{
			my $line = undef;
			my $newfiles_count = 0;
			while ($line = <MACFILE>) 
			{	last if ($line =~ /was generated by $progname/);	}
			if ($line)
			{
				dbgdbg ("Found following (right) lines in mac description", 3);
				while  ($line = <MACFILE>)
				{
					if ($line =~ /^\.\/(.*)\/(.*\.xml)(.*)$/)
					{
						$existing_msgfiles{"./$1/$2"} = defined $3 ? "$3" : "";
						dbgdbg ("Found - $line", 3);
					}
					else
					{
						# ignored lines
					}
				}
				close MACFILE;
				# check for existence of generated xml files
				dbgdbg ("Look for generated files in mac description", 2);
				foreach (@mac_filelist)
				{	
					unless (defined $existing_msgfiles{$_})
					{
						dbgdbg ("File not found: $_\n", 3);
						$existing_msgfiles{$_} = undef;	
						$newfiles_count++;
					}
				}
			}
			else
			{
				# empty mac description (or manipulated)
				# regeneration necessary
				$newfiles_count = -1;
			}
			if ($newfiles_count == 0)
			{
				dbgdbg ("Found all generated files in mac description - recreate not necessary\n", 1);
				pop @files_to_submit;
				P4Lib::p4_revert($macfile) unless ($already_edit_head_rev);
			}
			else
			{
				dbgdbg ("New files found - mac description must be recreated\n", 1);
				open( MACFILE, ">$macfile" )
						|| die ( "Can't create mac description $macfile\n" );
				dbgdbg("Generate intro for mac description\n", 2);			
				dbgdbg ("write intro fo mac description\n", 2);
				print MACFILE "# @(#) $relative_msgfilepath/$mac_filename   - ".get_db_date()."\n";
				print MACFILE join "\n", GetLicenceText("desc");
				print MACFILE "#\n#\n#     This description was generated by $progname\n".
			                  "#       !!! do not edit !!!\n\n";
				dbgdbg ("write list of message files to mac description\n", 2);			                  
			    foreach (sort keys %existing_msgfiles)
			    {
			    	my $line = "$_";
			    	if (defined $existing_msgfiles{$_})
			    	{
			    		$line .= $existing_msgfiles{$_};
			    		dbgdbg ("OLD:$line\n", 3);
			    	}
			    	else
			    	{
			    	    $line .= " " x ((length($_) > 45 ) ? 1 : ( 45 - length($_)))."distrib     # added by $progname";
			    	    dbgdbg ("NEW:$line\n", 3);
			    	}
			    	print MACFILE "$line\n";
			    }
			    close MACFILE;
			}
		}		
		return 0;
	}
	else
	{
			die "Can't check out mac description $macfile\n";
	}
}

sub set_write_permissions
{
	my $file = shift;
	my $to_write = shift;
	my $chmod_cmd;
		
	if($^O=~/.*win32.*/i)
	{	
		$file =~ tr/\//\\/;
		$chmod_cmd ="attrib " . ($to_write ? "-R" : "+R") . " $file";	
	}
	else
	{	
		$chmod_cmd ="chmod ". ($to_write ? "+w" : "-w") . " $file";	
		
	}
	system ("$chmod_cmd") == 0 
			or die "\nError: Error while executing '$chmod_cmd'\n";
	
}

sub revert
{
	my $file = shift;
	my $sync_to_head = shift;
	my $rc = 0;
	dbgdbg ("Revert $file ". ( $sync_to_head ? "with" : "without" ) . " syncing\n", 2 );
	my ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded ) = P4Lib::p4fstat($file);
	if ( $fileDoesNotExist )
	{	return (0);	}
	if ($fileOverwritable)
	{
		$rc = P4Lib::p4_revert($file);	
	}	
	$sync_to_head and $fileNotSynced and $rc = P4Lib::p4_sync($file);
	return 1;
}


sub sync
{
	my $file = shift;
	my $revert_existing = shift;
	my $current_changelist = 0;
	my $already_edit_head_rev = 0;
	my $rc;
	dbgdbg ("Sync $file ". ( $revert_existing ? "with" : "without" ) . " reverting existing file\n",1 );
	my ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded ) = P4Lib::p4fstat($file);
	if ( $fileDoesNotExist )
	{	return (0);	}
	if ($fileNotSynced)
	{
		if ($fileOverwritable)
		{
			if ($revert_existing)
			{
				$current_changelist = P4Lib::p4_opened($file);	
				$rc = P4Lib::p4_revert($file);	
				$fileOverwritable = 0;
			}
			else
			{	dbgdbg ("Warning: Existing file is writable - sync skipped");}
		}
		$rc = P4Lib::p4_sync($file);
	}
	elsif ($fileOverwritable || $fileAdded)
	{
		$already_edit_head_rev = 1;
	}
	return (1);	
}

sub checkout 
{
	my $file = shift;
	my $no_sync = shift;
	my $rc = 0;
	my $current_changelist = 0;
	my $already_edit_head_rev = 0;
	
	dbgdbg ("Check out of $file ". ( $no_sync ? "without" : "with" ) . "sync before\n",1 );
	
	my ( $fileDoesNotExist, $fileNotSynced, $fileOverwritable, $fileAdded ) = P4Lib::p4fstat($file);
	if ( $fileDoesNotExist )
	{
		unless ( -f $file)
		{
			open( DUMMY, ">$file" )	|| die ( "Can't create file $file\n" );
			print DUMMY "first generation for add";
			close DUMMY;
		}
		$rc = P4Lib::p4_add($file);
	}
	else
	{
		if ($fileNotSynced && ! $no_sync)
		{
			if ($fileOverwritable)	
			{	
				$current_changelist = P4Lib::p4_opened($file);	
				$rc = P4Lib::p4_revert($file);	
				$fileOverwritable = 0;
			}
			$rc = P4Lib::p4_sync($file);
		}
		elsif ($fileOverwritable || $fileAdded)
		{
			$already_edit_head_rev = 1;
		}
 		unless ($fileOverwritable || $fileAdded)
		{
			my $changelist = (defined $forced_changelist) ? $forced_changelist : $current_changelist;
			$rc = ($changelist > 0) ?  P4Lib::p4_edit_cl($file,$changelist) : P4Lib::p4_edit($file);
		}
	}
	push @files_to_submit, $file;
	
	return wantarray () ? ($rc,$already_edit_head_rev) : $rc  ;
}

sub submit_changes
{
	my $rc = 0;
	if (scalar @files_to_submit)
	{
		$rc = P4Lib::p4_submit (\@files_to_submit, $submit_text, 0);
	}
	else
	{
		print "No changes to submit found.\n";
	}
	return $rc ? 1 : 0;
}

sub get_all_components
{
	my @comp_list = ();
	my $cmd = "SELECT COMPONENT FROM MSG_COMPONENTS";
	connect_qadb();
	dbgdbg ("Look for existing components in components table\n");
	dbgdbg ("Execute: $cmd\n", 3);
	my $ref = $dbh->selectcol_arrayref($cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{  	dbgdbg("Warning: no component entries found\n"); }
	}
	else
	{
		@comp_list =  @$ref;
	}
	dbgdbg ("Found:\n  ". (join "\n  ", @comp_list)."\n", 3 );
	return @comp_list;
}


sub get_all_languages
{
	my %lang_hash = ();
	my $cmd = "SELECT LANGUAGE FROM MSG_LANGUAGES";
	connect_qadb();
	dbgdbg ("look existing languages in languages table\n", 1 );
	dbgdbg ("Execute: $cmd\n", 3);
	my $ref = $dbh->selectcol_arrayref($cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
		else
		{   dbgdbg ("Warning: no component entries found\n"); }
	}
	else
	{
		foreach (@$ref)
		{ $lang_hash{$_} = 1; }
	}
	dbgdbg ("Found:\n  ". (join "\n  ", keys %lang_hash)."\n", 3 );
	return \%lang_hash;
}


sub Usage
{
	print <<USAGE_END;
    
Usage: $progname [-c <component>] [-id <msg id>] [-(a|all_components)] 
                   [-(ho|header_only)] [-(mo|msgfile_only)]
                   [-no_p4] [-(cl|changelist) <changelist>]  
                   [-(fs|file_single) <msgfile|headerfile>] 
                   [-(fa|file_all) <msgfile|headerfile>] 
                   [-(lc|list_components)] [-(s|silent)]
                   [-fm|force_msgfile] [-ci <description>]
                   [-rc|recreate]
                   [-(v|verbose)] [-(d|debug) <level>]
                   [-(h|help|?)]
                   
                                       
Where:
     -c <component>
         Component name
     
     -id <message id>
         List of message id's  (separated by comma without spaces)
     
     -a 
         create files for all existing components    
          
     -ho or -header_only  *
         Generate header files only             
            
     -mo or -msgfile_only *
         Generate xmlfiles for messages only             
        
     -fs <file> or -file_single <file>
         Only this file will be created or regenerated 
         (component option not necessary)
     
     -fa <file> or -file_all <file>
         All related files (header,xml) will be created or regenerated 
         (component option not necessary)
        
     -no_p4
         Switch perforce handling off
            
     -cl <changelist> or -changelist <changelist>
         Assign the changes to a specific changelist (open on local client)
     
     -lc or -list_components
         List all existing components and exit
     
     -v or -verbose  
         A little bit more output
         
     -s or silent 
         silent mode: only errors will be print
                
     -h or -help -?
         Show this help          

Note: 
     If neither -mo nor -ho is given buildmsg.pl will generate all files.
        
            
USAGE_END
}

##########################
# MAIN
##########################


my @complist = ();
my @forced_idlist = ();


if (!GetOptions( \%Opts,
                'a|all',
                'c|component=s',
                'id=i',
                'cl|changelist=s',
                'u|user:s',
                'd|debug=i',
                'fa|file_all=s',                
                'fm|force_msgfile',
                'fs|file_single=s',
                'h|help|?',
                'ho|header_only',
                'rc|recreate',
                #'l|language=s',
                'lc|list_comonents',
                'mo|msgfile_only',
                'ci|checkin=s',
                'no_p4',
                's|silent',
                'v|verbose') || $Opts{'h'})
{
    Usage();
    exit(0);
}

##########
########### GSTART
#my %component_data ;

#$component_data{'COMPONENT'} = shift;
#$component_data{'DEFINENAME'} = $component_data{'COMPONENT'};
#$component_data{'DEFINENAME'} =~ tr/[a-z]/[A-Z]/;
#my $testopt= shift;
#$msgfilepath = "D:\\MSGTEST";
#$force_xml_creation = 1;
#my $date = get_last_changedate(\%component_data);
#print ("Changedate: $date\n");
#my $current_date = get_db_date();
#print ("Currentdate: $current_date\n");
#$verbose =1;
#my $rc = new_messagefile(\%component_data);
#exit;
#$verbose = 4;
#my $r_xml_msg_list = get_xml_messagelist ($component_data{'COMPONENT'});
#my $r_action_hash = get_action_hash ();

#exit;

#my %testhash=get_taghash_for_component($component_data{'COMPONENT'});
#my @msg_lists ;
#if (defined $testopt)
#{
#	@msg_lists = get_message_list_for_header (\%component_data, $testopt );
#}
#else
#{
#	@msg_lists = get_message_list_for_header (\%component_data);
#}
#foreach my $msg_data (@msg_lists)
#{
#	if (defined $testhash{$msg_data->{'ID'}})
#	{	
#		$msg_data->{'TAGS'} =  $testhash{$msg_data->{'ID'}};
#	}
#}

#get_header_file_name (\%component_data);
#if (defined $testopt)
#{
#	modify_headerfile(\%component_data);
#}
#else
#{
#	new_headerfile(\%component_data);
#}

#
#$Opts{'l'} = "EN";
#exit;

####### <- END OF TEST

###############################
# check for paranoid options
###############################
my $opt_check = (defined $Opts{'c'} ? 1 : 0)  + 
                (defined $Opts{'a'} ? 1 : 0)  + 
                (defined $Opts{'fs'} ? 1 : 0) + 
                (defined $Opts{'fa'} ? 1 : 0) + 
                (defined $Opts{'lc'} ? 1 : 0);

if ( $opt_check == 0 )
{
	die "Error: option -c, -a, -fs  or -fa is mandatory\n". 
	    "       (For more help call with -h)\n";
}
elsif ( $opt_check > 1 )
{
	die "Error: only -c, -a, -fs or -fa is allowed\n". 
	    "       (For more help call with -h)\n";
}

$verbose = (defined $Opts{'s'} ? -1 : (defined $Opts{'d'} ? $Opts{'d'} : ( defined $Opts{'v'} ? 1 : 0 )));

if (defined $Opts{'c'})
{
	@complist = split ',', $Opts{'c'} ;
}
elsif (defined $Opts{'a'} || defined $Opts{'lc'} )
{
	@complist = get_all_components();
}

if (defined $Opts{'cl'})
{
	$forced_changelist = $Opts{'cl'};
}

if (defined $Opts{'ci'})
{
	$submit_text = $Opts{'ci'};
}

if (defined $Opts{'lc'})
{
	dbgdbg ("Existing components:\n");
	foreach (@complist)
	{ 
		dbgdbg ("  $_\n");
	}
	exit 0;
}

if (defined $Opts{'id'})
{
	@forced_idlist = split ',', $Opts{'id'} ;
}

if (defined $Opts{'fm'})
{	$xml_checkout = 1; }

if (scalar (@complist) > 1 && @forced_idlist)
{
	die "Error: $progname doesn't support multiple components with a given message id\n";
}

# if file name is given 
# -> try to find out component from filename 
if (defined $Opts{'fs'} || defined $Opts{'fa'})
{
	my $filepath = defined $Opts{'fs'} ? $Opts{'fs'} : $Opts{'fa'};
	unless (-f $filepath)
	{
		die "Error: can use option -f". 
		      (defined $Opts{'fs'} ? "s" : "a").
		      " only for existing files\n".
		      "(file $filepath not existis)\n";
	} 
	my $namepos = undef;
	# if not complete path append current path
	unless ($filepath =~ /^(\/|\\|\w:)/)
	{
		my $current_dir = cwd();
		$filepath = "$current_dir/$filepath";
	}
	$filepath =~ s/\\/\//g;
	if ($filepath =~ /^(.*)\/$relative_msgfilepath\/(.*)$/)
	{
		$msgfilepath = "$1/$relative_msgfilepath";
		my $filename = $2;
		my $filetype = undef;
		my $comp_name = undef;
		my $lang_name = undef;
		
		if ($filename =~ /$file_prefix(.*)\.(h|xml)$/)
		{
			# a header file
			$comp_name = $1;
			$filetype  = $2;
			if ($filename =~ /^(.*)\/$file_prefix$comp_name\.xml$/)
			{	
				$lang_name = $1;
				$lang_name =~ tr/[a-z]/[A-Z]/;
			}
		}
		die "Error: unknown file type \n" unless (defined $filetype);
		die "Error: wrong language directory\n" if (($filetype =~ /xml/) && ! defined $lang_name);
		@complist = ($comp_name);
		if (defined $Opts{'fs'})
		{
			if ($filetype =~ /xml/)
			{
				$Opts{'mo'} = 1 ;
				$Languages{$lang_name} = 0 ; 
				delete $Opts{'ho'};
			}
			if ($filetype =~ /h/)
			{
				$Opts{'ho'} = 1 ;
				delete $Opts{'mo'};
				$Languages{'EN'} = 0 ; 
			}			
		}
	}
	else
	{
		die "Error: given file in wrong directory \n". 
		      "(it should be located in $relative_msgfilepath)\n";
	}
}
else
{
	$msgfilepath = "$ENV{'OWN'}/$relative_msgfilepath";
}

get_schema_version();
unless (defined $supported_schema_versions{$schema_version})
{
		die "Error: buildmsg doesn't support schema version defined in the qadb \n". 
		    "        qadb version       : $schema_version\n".
		    "        supported versions : ". (join ", ", keys (%supported_schema_versions)). "\n";
}

unless (defined $Opts{'l'} || scalar (keys %Languages) > 0 )
{
	%Languages = %{get_all_languages()};
}
else
{
	if (defined $Opts{'l'})
	{
		foreach (split ',', $Opts{'l'})
		{
			tr/[a-z]/[A-Z]/;
			$Languages{$_} = 0;
		}
	}
	# check languages
	dbgdbg ("Check given languages (". (join ",", keys %Languages) . ")\n", 2);
	my $check_lang = get_all_languages();
	dbgdbg ("database have following languages:".(join ",", keys %$check_lang) . ")\n", 3);
	foreach (keys %Languages)
	{
		unless (defined $check_lang->{$_})
		{	
			die "Error: unknown language '$_'\n".
			    "       existing languages: ".(join ",", keys %$check_lang)."\n";
		}
		# language is ok 
		$Languages{$_} = 1 ;
	}
}

# and now generate the message files
foreach my $component (@complist)
{
	my $refto_component_data = get_component_data ($component);
	
	if ($refto_component_data)
	{	
		my @idlist = scalar (@forced_idlist) ? @forced_idlist : ();
		if (defined $user)
		{
			push @idlist, get_msgids_for_user ($refto_component_data);
			if (scalar (@idlist) == 0)
			{	my_error ("There aren't any mesage id's for component '$component' (changed by $user)");	}
		}	
		
		# create header file now
		unless ( defined $Opts{'mo'} )
		{
			my @msg_lists = get_message_list_for_header ($refto_component_data, @idlist);
			if (scalar (@msg_lists) > 0 )
			{
				my %tag_hash  = get_taghash_for_component($refto_component_data->{'COMPONENT'});
				# 
				foreach my $msg_data (@msg_lists)
				{
					if (defined $tag_hash{$msg_data->{'ID'}})
					{	
						$msg_data->{'TAGS'} =  $tag_hash{$msg_data->{'ID'}};
					}
				}
				
				# modify header file if single id's are given
				if (scalar (@idlist) > 0)
				{
					modify_headerfile($refto_component_data);
				}	
				# new header file
				else
				{
					new_headerfile($refto_component_data);
				}
			}
			elsif (scalar (@idlist) == 0)
			{
				dbgdbg ("Generation of files of component $refto_component_data->{'COMPONENT'}\n");
				dbgdbg ("=> Skipped - No messages for component found\n");
				next;
			}
			else
			{	my_error ("Can't find message with the given id's");	}
		}
		unless ( defined $Opts{'ho'} )
		{	
			new_messagefile($refto_component_data);			
			
		}
	}
	else
	{  
		dbgdbg ("Error: Unknown component definition '$component'\n");
		dbgdbg ("         (it will be ignored)\n");
		$error_count++;
	}
}

if ($xml_checkout && scalar @mac_filelist > 0)
{
	$error_count += create_mac_file();
}

if ($submit_text)
{
	$error_count += submit_changes();
}

exit $error_count;




#
# TESTS and some helpfull functions
sub test_xml_convert
{
	my $testtext="Das ist ein Test um zum Beispiel < und & zu testten.\n". 
	"Alles andre sollte noch normal weiter funktionieren, ausser natürlich & und \n".
	"<\n";
	
	$testtext = convert_to_xml($testtext);
	print "$testtext";
	exit;
}

sub test_c_convert
{
	my $testtext="Das ist ein Test um zum Beispiel \" und % und \\ zu testten.\n". 
	"Alles andere sollte noch normal weiter funktionieren, ausser natürlich % und \\\n";
	
	$testtext = convert_to_c($testtext);
	print "$testtext";
	exit;
}


sub named_connect
{
	my $db_handle = shift;
	my $database = shift;
	my $server   = shift;
	
	unless ($$db_handle)
	{
		dbgdbg("connecting to the database ... ");
		my $dbh = DBI->connect("DBI:MaxDB:$server/$database","MSG_USER","MSG_USER") or die "Can't connect $DBI::err $DBI::errstr\n";
		$$db_handle = $dbh;
		dbgdbg("successfully\n");
	}
}

sub copy_tables
{
	my $src_dbh = undef;
	my $trg_dbh = undef;
	named_connect(\$trg_dbh, "QADB", "PTS" );
	named_connect(\$src_dbh, "QADB2", "pwdf2027.wdf.sap.corp" );
	my $src_cmd =  "select * from MSG_ACTIONS"; 
	dbgdbg ("Execute: $src_cmd\n", 3);
	
	my $ref = $src_dbh->selectall_arrayref($src_cmd);
	unless ($ref)
	{
		if ( $DBI::err )
		{	die "selectrow_array failed $DBI::err $DBI::errstr"; }
	}
	else
	{		
		foreach my $row (@$ref) {
			my $insert_cmd = "INSERT INTO MSG_ACTIONS (ACTIONNAME, CREATEDBY, CREATEDATE) VALUES ('@$row[0]', 132, '2001-09-16 21:03:00.548000')";
			my $ret = $trg_dbh->do($insert_cmd);	
			$insert_cmd = "INSERT INTO MSG_ACTIONS_TEXT VALUES ('@$row[0]', '@$row[1]', '@$row[2]', @$row[3], @$row[4], '@$row[5]')" ;
			$ret = $trg_dbh->do($insert_cmd);	
	    }
	}
	
}



# VIEW definitions
my $view_msg = 'create view V_MSG_MESSAGES as select  MM.COMPONENT AS COMPONENT, MM.MESSAGEID AS MESSAGEID, MM.DEFINENAME AS DEFINENAME,  MT.LANGUAGE AS LANGUAGE, MT.SHORTTEXT as SHORTTEXT, MT.DESCRIPTION as DESCRIPTION from MSG_MESSAGES MM, MSG_TEXT MT where MM.COMPONENT = MT.COMPONENT and MM.MESSAGEID = MT.MESSAGEID'; 
my $view_actions = 'create view V_MSG_ACTION as select  MAA.COMPONENT AS COMPONENT, MAA.MESSAGEID AS MESSAGEID, MAA.DBRELEASE AS DBRELEASE, MAA.ACTIONNAME AS ACTIONNAME,  MAT.LANGUAGE as LANGUAGE,  MAT.DESCRIPTION AS DESCRIPTION,  MAT.STATUS AS STATUS from MSG_ASSIGN_MSG_ACTION MAA, MSG_ACTIONS_TEXT MAT where MAA.ACTIONNAME = MAT.ACTIONNAME';
my $release_view_tags= "create view V_MSG_REL_TAGS as select  MAMR.COMPONENT AS COMPONENT, MAMR.MESSAGEID AS MESSAGEID, MAMR.RELEASE AS RELEASE, MAMR.MESSAGEVERSION AS MESSAGEVERSION, MT.TAG AS TAG FROM MSG_ASSIGN_MSG_RELEASE MAMR,  MSG_ASSIGN_MSG_TAGS MAMT, MSG_TAGS MT WHERE MAMR.COMPONENT = MAMT.COMPONENT and MAMR.MESSAGEID = MAMT.MESSAGEID and MAMR.MESSAGEVERSION = MAMT.MESSAGEVERSION and MAMT.TAG=MT.ID";
my $release_view_messages = "create view V_MSG_REL_MESSAGES as select MAMR.COMPONENT AS COMPONENT, MAMR.MESSAGEID AS MESSAGEID, MAMR.RELEASE AS RELEASE, MAMR.MESSAGEVERSION AS MESSAGEVERSION, MM.DEFINENAME AS DEFINENAME, MT.SHORTTEXT AS SHORTTEXT, MT.DESCRIPTION AS DESCRIPTION, MT.LANGUAGE AS LANGUAGE FROM MSG_ASSIGN_MSG_RELEASE MAMR,  MSG_MESSAGES MM, MSG_TEXT MT WHERE MAMR.COMPONENT = MT.COMPONENT and MAMR.MESSAGEID = MT.MESSAGEID and MAMR.MESSAGEVERSION = MT.MESSAGEVERSION and MAMR.COMPONENT = MM.COMPONENT and MAMR.MESSAGEID = MM.MESSAGEID";
my $xml_view_messages = "create view V_MSG_XML_MESSAGES as select MT.COMPONENT AS COMPONENT, MT.MESSAGEID AS MESSAGEID, MM.DEFINENAME AS DEFINENAME, MT.MESSAGEVERSION AS MESSAGEVERSION, MT.LANGUAGE AS LANGUAGE, MT.SHORTTEXT as SHORTTEXT, MT.DESCRIPTION as DESCRIPTION, MAMA.ACTIONNAME AS ACTIONNAME FROM MSG_TEXT MT, MSG_MESSAGES MM, MSG_ASSIGN_MSG_ACTION MAMA WHERE MT.COMPONENT = MM.COMPONENT and MT.MESSAGEID = MM.MESSAGEID and  MT.COMPONENT = MAMA.COMPONENT (+) and MT.MESSAGEID = MAMA.MESSAGEID (+) and MT.MESSAGEVERSION = MAMA.MESSAGEVERSION (+)";
my $release_view_tags1= "create view V_MSG_REL_TAGS1 as select  MAMR.COMPONENT AS COMPONENT, MAMR.MESSAGEID AS MESSAGEID, MAMR.RELEASE AS RELEASE, MAMR.MESSAGEVERSION AS MESSAGEVERSION, MT.TAG AS TAG, MAMT.MANDATORY AS MANDATORY FROM MSG_ASSIGN_MSG_RELEASE MAMR,  MSG_ASSIGN_MSG_TAGS MAMT, MSG_TAGS MT WHERE MAMR.COMPONENT = MAMT.COMPONENT and MAMR.MESSAGEID = MAMT.MESSAGEID and MAMR.MESSAGEVERSION = MAMT.MESSAGEVERSION and MAMT.TAG=MT.ID";




