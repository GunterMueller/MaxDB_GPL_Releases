#! /usr/bin/perl
#
# $Header: //sapdb/TOOLSRC/develop/sys/src/BuildPackage/WinLink.pm#2 $
# $DateTime: 2002/06/10 14:15:26 $
# $Change: 22128 $
#
# Desc:
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

package qadbtool;

@ISA = ('Exporter');
@EXPORT = ('new');


use SafeDBI;
use Net::SMTP;
use Net::HTTP;
use Sys::Hostname;


sub new {
    #
    # Lets get initialized
    #
    my $name = shift;
    my $args = shift;
    my $self = {};
    my $hostname = hostname();
    my %fields;

    $hostname =~ tr/A-Z/a-z/;   my $sth;
    
    my %dbi_attr = ( PrintError => 0, Raise_error => 0);
    
    $self->{'connectstring'} = &getConnectString();
    
    #
    # Initaialize Error-Handlers
    $self->{error_code} = 0;
    $self->{error_text} = "";
    $self->{dbh} = SafeDBI->connect($self->{'connectstring'}, 'TESTER', 'TESTER', \%dbi_attr) or
        $self->{error_code} = 1;

    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("Could not connect to Database:\n $DBI::errstr");
        return bless $self;
    }
    if (keys (%$args) == 1) {
        my ($id_name, $id_value) = each (%$args);
        if ($id_name =~ /ID/i) {
            $fields{'ID'} = $id_value;
        }
        else {
            $self->{error_code} = 1;
            $self->{error_text} = &throw_err("The given parameters was not correct - I need an ID.s\n");
            return bless $self;
        }
    }
    else {
        my @required_fields;
        $sth = $self->{dbh}->prepare("SELECT columnname FROM columns WHERE mode = 'MAN' AND tablename = 'TOOLMAKES'");
        $sth->execute();
        while (my $row = $sth->fetchrow_arrayref) {
            push(@required_fields, @$row[0]);
        }
        @required_filds = sort (@required_fiedls);

        my $x; # Loop-Helpers
        my $y; #
        my $match_count = 0; # Counts the matches of given arguments with required db-fields
        foreach $x (@required_fields) {
            foreach $y (keys(%$args)) {
                if ($x  =~ /^$y$/i) {
                    $match_count++;
                    $fields{$x} = $$args{$y};
                    delete($$args{$y});
                    last;
                }
            }
        }
	
	# Added by d038447 @ 2004-10-06
	# If a idplatform was already given to us, proceed with finding out the idserver only
	# Otherwise, find out both and ignore a possibly given platform name
	# That's not nice, but a first aid for our toolmakes.
	#
	
	if ($fields{'IDPLATFORM'}) {
		$sth = $self->{dbh}->prepare("SELECT id FROM servers WHERE HOST = '" . $hostname . "'");
		$sth->execute();
		($fields{'IDSERVER'}) = ($sth->fetchrow_array);
		$sth->finish();
	} else {
		$sth = $self->{dbh}->prepare("SELECT id, idplatform FROM servers WHERE HOST = '" . $hostname . "'");
		$sth->execute();
		($fields{'IDSERVER'}, $fields{'IDPLATFORM'}) = ($sth->fetchrow_array);
		$sth->finish();
		$match_count++;
	}
	
	# End of addtion
	
	
        if ($match_count != @required_fields) {
            if (!scalar(grep /^IDPLATFORM$/i, keys(%fields))) {
                if (scalar(grep /^PLATFORM$/i, keys(%$args)) == 1) {
                    undef $sth;
                    $sth = $self->{dbh}->prepare("SELECT COUNT(id) FROM platforms WHERE DESCTEXT = '" . $$args{'PLATFORM'} . "'");
                    $sth->execute();
                    if (($sth->fetchrow_array)[0] != 1) {
                        $self->{error_code} = 1;
                        $self->{error_text} = &throw_err("Counld not find you which platform you wanted: $$args{'PLATFORM'} \n");

                        $sth->finish();
                        $self->{'dbh'}->disconnect();
                        delete $self->{'dbh'};
                        return bless $self;
                    }

                    undef $sth;
                    $sth = $self->{dbh}->prepare("SELECT id FROM platforms WHERE DESCTEXT = '" . $$args{'PLATFORM'} . "'");
                    $sth->execute();
                    $fields{'IDPLATFORM'} = (($sth->fetchrow_array)[0]);
                    $sth->finish();
                }
                else {
                    undef $sth;
                    $sth = $self->{dbh}->prepare("SELECT COUNT(id) FROM platforms WHERE perltext = '" . $^O . "'");
                    $sth->execute();
                    my $platform_count =  ($sth->fetchrow_array)[0];
                    if ($platform_count != 1) {
                        $self->{error_code} = 1;
                        $self->{error_text} = &throw_err("Error while trying to figure out the platform we were running on:\n $^O occurs $platform_count in the Database.\n");

                        $sth->finish();
                        $self->{'dbi'}->disconnect();
                        delete $self->{'dbh'};

                        return bless $self;
                    }
                    undef $sth;
                    $sth = $self->{dbh}->prepare("SELECT id FROM platforms WHERE perltext = '" . $^O . "'");
                    $sth->execute();
                    $d = ($sth->fetchrow_array)[0];
                    $sth->finish();
                    $fields{'IDPLATFORM'} = $d;
                }
                $match_count++;
            }

            if (!scalar(grep /^IDQASTAUS$/i, keys (%fields)) and (scalar(grep/^QASTATUS$/, keys(%$args)) == 1)) {
                $$args{QASTATUS} =~ tr/[a-z]/[A-Z]/;

                undef $sth;
                $sth = $self->{dbh}->prepare("SELECT COUNT(id) FROM qastatus WHERE desctext = '" . $$args{'QASTATUS'} . "'");
                $sth->execute();

                if (($sth->fetchrow_array)[0] != 1) {
                    $self->{error_code} = 1;
                    $self->{error_text} = &throw_err("Could not find out whiche QASTATUS could me ment with $$args{'QASTATUS'}\n");
                    return bless $self;
                }

                undef $sth;
                $sth = $self->{dbh}->prepare("SELECT id FROM qastatus WHERE desctext = '" . $$args{'QASTATUS'} . "'");
                $sth->execute();

                $fields{'IDQASTATUS'} = ($sth->fetchrow_array)[0];
                $match_count++;
            }

            if ($match_count != @required_fields) {
                    $self->{error_code} = 1;
                    $self->{error_text} = &throw_err("The arguments given do not match the needed arguments\n");
                    return bless $self;
            }
        }

        $self->{dbh}->{AutoCommit} = 0;
        $self->{dbh}->do("INSERT INTO toolmakes (ID, " . join(", ", keys(%fields)) . ") VALUES ( toolmakes_id.NEXTVAL, '" . join("', '", values(%fields)) . "')") or
                &throw_err("IMPORTANT:\nINSERT INTO toolmakes (" . join(", ", keys(%fields)) . ") VALUES ('" . join("', '", values(%fields)) . "')" . "\n" .
                    $self->{'dbh'}->errstr);

        undef $sth;
        $sth = $self->{dbh}->prepare("SELECT toolmakes_id.CURRVAL FROM DUAL");
        $sth->execute();
        $fields{'ID'} = ($sth->fetchrow_array)[0];
        $self->{dbh}->commit();
        $self->{dbh}->{AutoCommit} = 1;

    } # End of else

    undef $sth;
    $sth = $self->{dbh}->prepare("SELECT * FROM toolmakes WHERE id = $fields{'ID'}");
    $sth->execute();
    foreach $x (keys(%fields)) {
        print "$x\t = $fields{$x}\n";
    }

    %field = %{$sth->fetchrow_hashref};
    while (($key, $value) = each (%field)) {
        print "$key \t = $value\n";
        $self->{$key} = $value;
    }

    $sth->finish();
    $self->{dbh}->disconnect();
    delete $self->{'dbh'};

    return bless $self;
}


sub update_columns{
    my $self = shift;
    my $args = shift;

    my $key;
    my $value;

    my $cmdstring = "UPDATE toolmakes SET ";

    while (($key, $value) = each(%$args)) {
        $cmdstring .= " $key = '$value',";
    }
    chop $cmdstring; # Das , wegschneiden

    $cmdstring .= " WHERE id = " . $self->{'ID'};
    $self->execDML($cmdstring);

    return $self->{error_code};
}

sub write_log {
    my $self = shift;
    my $log_text = shift;

    my $quoted_text = "";
    if (!defined $log_text) {
        $self->{error_code} = 1;
        $self->{error_text} = &throw_err("write_log: Got wrong parameters");
        return 1;
    }

    if (length($log_text) > 1000) { # Take care. The value of 1000 is hard-coded, but may need to be changed according to the DB-Dimension.
        $self->{error_code} = 1;
        $self->{error_text} = &throw_err("write_log: I got a too log log-text:\n$log_text");
        return 1;
    }

    $quoted_text = &quote_sql2($log_text);
    $self->execDML("INSERT INTO toolmakelog (idtoolmake, info) VALUES ($self->{ID}, $quoted_text)");

        if ($self->{error_code} == 1) {
                $self->{error_text} = &throw_err("write_log: Could not execute Database-SQL-Statement $cmdstring:\n $DBI::errstr");
    }

    return $self->{error_code};
}


sub write_prot {
    my $self = shift;
    my $protname = shift;
    my $prot = shift;
    my $info_text = shift;
    my %head_content;
    $self->{error_code} = 0;

    # Config-Parameters
    # Should mabye be store elsewhere.
    my $httpmachine = "pts.wdf.sap.corp";
    my $httpport    = 1080;
    my $webroot     = "/webdav/toolmakes/";

    if (!defined($prot)) {
        $self->{error_code} = 1;
        $self->{error_text} = &throw_err("write_prot: Got wrong parameters");
        return 1;
    }

    if (!defined($info_text)) {
        $info_text = $protname;
    }

    my $hostname    = hostname();
    my $day   = ((localtime())[3]);
    my $month = ((localtime())[4] + 1);
    my $year  = ((localtime())[5] + 1900);


    $head_content{'User-Agent'} = "LCqaCLI/1.0";

    ##### CREATE THE SUBDIRECTORY FOR THE HOST ON THE WEBDAV-SERVER ###################################################################
    my $current_path = $webroot . $hostname;
    my $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the host-directory $current_path :\n$@");
        return 1;
    }
    $dav_req->write_request('MKCOL', $current_path, %head_content);
    ($code, $mess, %headers) = $dav_req->read_response_headers();


    ##### CREATE THE SUBDIRECTORY FOR THE YEAR ON THE WEBDAV-SERVER ###################################################################
    $current_path .= "/$year";
    $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the year-directory $current_path :\n$@");
        return 1;
    }
    $dav_req->write_request('MKCOL', $current_path, %head_content);
    ($code, $mess, %headers) = $dav_req->read_response_headers();


    ##### CREATE THE SUBDIRECTORY FOR THE MONTH ON THE WEBDAV-SERVER ###########################################################
    $current_path .= "/$month";
    $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the month-directory $current_path :\n$@");
        return 1;
    }
    $dav_req->write_request('MKCOL', $current_path, %head_content);
    ($code, $mess, %headers) = $dav_req->read_response_headers();


    ##### CREATE THE SUBDIRECTORY FOR THE DAY OF THE MONTH ON THE WEBDAV-SERVER #########################################################
    $current_path .= "/$day";
    $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the day-directory $current_path :\n$@");
        return 1;
    }
    $dav_req->write_request('MKCOL', $current_path, %head_content);
    ($code, $mess, %headers) = $dav_req->read_response_headers();


    ##### CREATE THE SUBDIRECTORY FOR THE MAKE-ID ON THE WEBDAV-SERVER ###################################################################
    $current_path .= "/" . $self->{ID};
    $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the day-directory $current_path :\n$@");
        return 1;
    }

    $dav_req->write_request('MKCOL', $current_path, %head_content);
    ($code, $mess, %headers) = $dav_req->read_response_headers();
    ##### ENOUGH DIRECTORIES FOR TODAY ;-)) ###############################################################################################

    ##### WRITEING THE FILE ITSELF ########################################################################################################

    $current_path .= "/$protname";
    $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("write_prot: Could not open Request for creating the the file itself $current_path :\n$@");
        return 1;
    }

    $head_content{'Content-type'} = 'text/plain';
    $dav_req->write_request('PUT', $current_path, %head_content, $prot);

    ($code, $mess, %headers) = $dav_req->read_response_headers();

    if ($code ne "201") {
        $self->{error_code} = 1;
        $self->{error_text} = &throw_err("write_prot: Could not create $current_path on the server:\nHTTP-Statuscode: $code \nMessage: $mess \n");
        return 1;
    }

    #### Okay, it seems that we have succefully created a file on the WebDAV-Server. Let's go on and put a new entry into the Table:

    my $url         = &quote_sql2("http://$httpmachine:$httpport" . "$current_path");
    my $url_plain   = "http://$httpmachine:$httpport" . "$current_path";

    my $quoted_text = &quote_sql2($info_text);
    my $cmdstring = "INSERT INTO toolmakeprot (idtoolmake, info, url) VALUES ($self->{ID}, $quoted_text, $url)";
    $self->execDML($cmdstring);

        if ($self->{error_code} == 1) {
                $self->{error_text} = &throw_err("write_prot: Could not execute Database-SQL-Statement $cmdstring:\n $DBI::errstr");
    }

    return $url_plain;
}

#
# Release the current DB-connection
sub unlock {
    $self = shift;

    $rc = $self->{'dbh'}->disconnect;
    delete $self->{'dbh'};

    return $rc;
}

# And re-create it
sub lock {
    $self = shift;

    $self->{dbh} = SafeDBI->connect($self->{'connectstring'}, 'TESTER', 'TESTER', \%dbi_attr) or
        $self->{error_code} = 1;

    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("Could not connect to Database:\n $DBI::errstr");
    }

    my $sth = $self->{dbh}->prepare("SELECT * FROM makes WHERE id = $self->{'ID'}");
    $sth->execute();

    %field = %{$sth->fetchrow_hashref};
    while (($key, $value) = each (%field)) {
        $self->{$key} = $value;
    }
    $sth->finish();
    return $self->{'error_code'};
}
####

sub execDML {
    $self = shift;
    my $sqlstmt = shift;

    my $httpmachine = "pts.wdf.sap.corp";
    my $httpport    = 1080;
    my $webroot     = "SQLService?action=DirectExecute";
    my %head_content;

    $head_content{'User-Agent'} = "LCqaCLI/1.0";

    my $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
    if ($self->{error_code} == 1) {
        $self->{error_text} = &throw_err("execDML: could not open request for executing SQL: $sqlstmt :\n$@");
        return 1;
    }
    print $sqlstmt . "\n";
    $dav_req->write_request('PUT', $webroot, %head_content, $sqlstmt);
    ($code, $mess, %headers) = $dav_req->read_response_headers();
}


sub quote_sql2 {
    $qstr = shift;
    return "NULL" unless defined $qstr;
        $qstr =~ s/'/''/g;           # ISO SQL2 # '
        return "'$qstr'";
}


sub throw_err {
    my $errortext = shift;

    $errortext .= "\n########################################################\nENVIRONMENT:\n";
    for $x (keys(%ENV)) {
        $errortext .= "\n${x} = $ENV{$x}";
    }

    $errortext .= "\n";

    my $smtp = Net::SMTP->new("mail.sap.corp");
    if (!(defined $smtp)) {
        print "#############################################################################\n";
        print "################################### ERROR ###################################\n";
        print "######## COULD NOT CONNECT TO mail.sap.corp                         ########\n";
        print "######## Wanted to send the following by mail:                       ########\n";
        print $errortext . "\n";
        print "################################ END OF ERROR ###############################\n";
        print "#############################################################################\n";
    }
    else {
        $smtp->mail("remuser\@is0025.wdf.sap.corp");
        $smtp->to("falko.flessner\@sap.com");
        $smtp->data();
        $smtp->datasend("To: falko.flessner\@sap.com\n");
        $smtp->datasend("Subject: Error during qadb-run \n");
        $smtp->datasend("Priority: Urgent\nX-Priority: 1 (Highest)\n");
        $smtp->datasend("\n");
        $smtp->datasend($errortext);
        $smtp->dataend();
        $smtp->quit;
    }
    return $errortext;
}


sub getConnectString {
	
	my $httpmachine = "pts.wdf.sap.corp";
	my $httpport    = 1080;
	my $webroot     = "webdav/lcmakes/cs.txt";
	my $buf         = "";
	my %head_content;
	
	$head_content{'User-Agent'} = "LCqaCLI/1.0";
	
	my $dav_req = Net::HTTP->new('Host' => $httpmachine, 'PeerPort' => $httpport) || ($self->{error_code} = 1);
	if ($self->{error_code} == 1) {
		$self->{error_text} = &throw_err("getConnectString: could not open request for executing :\n$@\n$!");
		return 1;
	}
	
	$dav_req->write_request('GET', $webroot, %head_content,);
	($code, $mess, %headers) = $dav_req->read_response_headers();
	
	$dav_req->read_entity_body($buf, 1000);
	chomp($buf);
	chomp($buf);
	chomp($buf);
	print "Connect-string >" . $buf . "<\n";
	return $buf;
}



__END__

=head1 NAME

qadb - A perl Module for creating and updateing entries in
the SAP DB/liveCache QA-Database.

=head1 NOTE

This module is intended for internal use only.
Although it is free software, it won't be very usefull for the wide world

=head1 SYNOPSIS OF CREATING A NEW ENTRY

 use qadb;
 $qah =  qadb->new({'VERSION' => '7403',
    'BUILDPFX'   => '07',
    'QASTATUS'   => 'DEV',
    'CHANGELIST' => '12345'}) ;

 if ($qah->{error_code} != 0) {
    print "Error:\n$qah->{error_text}\n";
    return -1;
 }

=head1 SYNOPSIS OF LOADING A OLD ENTRY

 use qadb;
 $qah =  qadb->new({'ID' => 1234}) ;

 if ($qah->{error_code} != 0) {
    print "Error:\n$qah->{error_text}\n";
    return -1;
 }

=head1 DESCRIPTION

The C<qadb> class is a interface to the SAP-internal QA-System for SAP DB
and liveCache.

Each instance of C<qadb> represents a complete make in the sense of making
programs out of sourcecode. Aditionaly, it can carry informations about
the status of the programs made - test results for example.

A new instance of C<qadb> can be created in two ways:

=over 4

=item Creating a new entry

A couple of informations are required to create a new entry. Following
the perl standards, the constructor of the class is named C<new>. It
requires a hash-reference with the following entries:

  Name          Description                   Example value

 VERSION       4-digit Version              '7402'
 BUILDPFX      2-digit Build-prefix         '02'
 QASTATUS      The quality-status           'DEV'
 CHANGELIST    The CL-Number                '32456'

For AIX-Machines, the aditional "PLATFORM"-entry is required. This is
necssary becase the perl-interpreter does not make a difference between
AIX 4.x and AIX 5.x as we do it.

Currently, the followning values are accepted for PLATFORM:

    - sun_64
    - alphaosf
        - rs6000_51_64
    - rs6000_64
    - hp_64

Please keep in mind that a C<qadb>-instance normaly contains a variable
called C<ID> (you can access it with B<$qah-E<gt>{'ID'}>. This C<ID> identifies
a make-entry and will be needed later. So, I suggest to write this C<ID>
to the harddisk.

=item Loading a old entry

For loading a previously created entry, you need to call the contructor with
a hash-refernece, containing the ID generated by the inital creation of
the entry.

  Name          Description                   Example value

 ID            The ID taken from inital      5739
               creation

=back

=head1 METHODS

C<qadb> provides the following methods:

=over 4

=item $rv = update_columns({name1 => value1, ... , nameN => valueN});

Performs a update-statement on the main table. This should only be used
for updating IDQASTATUS, LCPOOLID, LC_OK and LCOK_TRANS.

It takes a hash-reference as arguement, filled with columnnames and the
corresponing values.

The "VARIABLES"-Section of this manual contains a complete description of all
fields.

Returns 0 on success.

=item $rv = write_log($log_text);

This adds a comment to the entry. The log-Text must not contain more than
1000 characters.

Returns 0 on success.

=item $rv = write_prot($prot_name, $prot  [, $info_text]);

Writes a protocoll to the WebDAV-server and creates a entry in the
appropriate table in the database.

It takes a protocolname, the protocol itself and a optional info text as
arguments.

If the info text is not provieded, the protocolname will be used for it.

Returns 0 on success.


=item $rv = unlock();

Releases the current DB-Connection, but don't forget about the Values.

This becomes necessary when the program forkes. See B<lock> for
further informations

Returns 0 on success.

=item $qah = lock();

Re-Creates the DB-Connection. This becomes necessary after performing
an B<unlock> in forking situations.

B<TAKE CARE:> this method will return a new instance. Overwrite the current one
with it. The following example will give you an idea how to do this:

   $qah->unlock();
   $pid = fork();
   $qah = $qah->lock();

   if ($pid) {
       #
       # go on here

=back

=head1 VARIABLES

C<qadb> contains the following variables. Variables corresponding with
fields in the database are marked with a X.

  Name         DB-Variable       Description

 ID                X            Identifies the complete build-process
 LCPOOLID          X            The number in the LC_POOL-directory
 VERSION           X            A four-digit version, eg. "7402"
 BUILDPFX          X            A two-digit buildprefix, eg. "05"
 IDPLATFORM        X            The numeric id of the platform
 IDQASTATUS        X            The numeric id if the QA-status
 IDOBJSTATUS       X            The numeric id of the make-status
 CHANGELIST        X            The Changelist-number
 TS                X            The timestamp of the last modification
 LCOK              X            Will be set when the tests are finished
                                successfully.
 LCOK_TRANS        X            Will be set after the LCOK-bit is
                                transfered into the appropriate structures
                in the filesystem.
 HISTCOUNT         X            Counts the number of changes in on these
                                informations. Will be updated automaticaly.
 error_code                     Conains the last error code set. After
                                successfull opterations it will be set to
                0.
 error_text                     Contains a human-readable description of
                                the last error.

=head1 ERROR HANDLING

Beneath the already introduced variables B<error_code> and B<error_text>
for error handling, a email will be sent in each case of a detected error.

The recipients of these Mails are currently hard-coded.

=head1 DBI INSTANCE

C<qadb> contains a ready-to-use DBI instance. It can be accessed by
B<$qah-E<gt>{dbh}>. Please use this with extreme care and use it
only if you can not avoid it.

The DBI documentation describes it in depth.

=head1 EXAMPLE

 use qadb;
 my $qah =  qadb->new({'VERSION' => '7403',
    'BUILDPFX'   => '07',
    'QASTATUS'   => 'DEV',
    'CHANGELIST' => '12345'}) ;

 if ($qah->{error_code} != 0) {
    print "Fehler:\n$qah->{error_text}\n";
    return -1;
 }

 if ($qah->update_columns({'LCPOOLID' => '012'}) != 0 ) {
     print "Error while update:\n$qah->{error_text}\n";
     return -1;
 }

 if ($qah->write_log("Hallo Welt, dies ist ein Test")) {
     print "Error while writing a log:\n$qah->{error_text}\n";
     return -1;
 }

 my $protocol = "";
 open (PROTOFILE, "/path/to/protocol") or die "Error reading protocol\n";

 while (<PROTOFILE>) {
     $protocol .= $_;
 }

 if ($qah->write_prot("make.log", $protocol, "This protocol contains the make-output.\n")) {
     print "Error while writing protocol make.log:\n$qah->{error_text}\n";
 }

=head1 COPYRIGHT

Copyright 2002 SAP AG

=cut

