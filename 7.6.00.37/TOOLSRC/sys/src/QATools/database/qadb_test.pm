#! /usr/bin/perl
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

package qadb;

@ISA = ('Exporter');
@EXPORT = ('new', 'new_test', 'lock', 'unlock');


use SafeDBI;
use Net::SMTP;
use Net::HTTP;
use Sys::Hostname;
use QAConnect;

sub new_test {
    my $name = shift;
    my $args = shift;
    
    #
    # SESSION
    #
    my $sess_args = shift;
    
    my $self = {};
    my $hostname = hostname();
    my %fields;

    $hostname =~ tr/A-Z/a-z/;

    my @required_fields = ('IDPLATFORM', 'VERSION', 'IDQASTATUS');
    #
    # Initaialize Error-Handlers
    $self->{error_code} = 0;
    $self->{error_text} = "";
    
    
    ###############################################################################
    #########  ID
    ###############################################################################
    
    my $x; # Loop-Helpers
    my $y; #
    my $match_count = 0; # Counts the matches of given arguments with required db-fields
    @required_fields = sort(@required_fields);
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
    
    if ($match_count != @required_fields) {
	    if (!scalar(grep /^IDQASTAUS$/i, keys (%fields)) and (scalar(grep/^QASTATUS$/, keys(%$args)) == 1)) {
		$$args{QASTATUS} =~ tr/[a-z]/[A-Z]/;
		
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id FROM qastatus WHERE DESCTEXT = '" . $$args{'QASTATUS'} . "'");
		
			if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
				$self->{error_code} = 1;
				$self->{error_text} = &throw_err("Could not find out which QASTATUS could be ment with $$args{'QASTATUS'}\n");
				
				return bless $self;
			}
			$fields{'IDQASTATUS'} = $href->{'Rows'}->{'Row'}[0]->{'ID'};
			$match_count++;
	    }
	    
	    unless ($fields{'IDPLATFORM'}) {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id, idplatform FROM servers WHERE HOST = '" . $hostname . "'");
		
		if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
			$self->{error_code} = 1;
			$self->{error_text} = &throw_err("Counld not find host $hostname in the serverlist\n");
			
			return bless $self;
		}
		
		$fields{'IDSERVER'}   = $href->{'Rows'}->{'Row'}[0]->{'ID'};
		$fields{'IDPLATFORM'} = $href->{'Rows'}->{'Row'}[0]->{'IDPLATFORM'};

		
		if ($fields{'VERSION'} && $fields{'IDQASTATUS'}) {
			($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
				"SELECT COUNT(*) AS CRES FROM makes WHERE " .
				" IDPLATFORM  =  $fields{'IDPLATFORM'} AND " .
				" VERSION     = '$fields{'VERSION'}' AND " .
				" IDQASTATUS  =  $fields{'IDQASTATUS'} AND " .
				" IDOBJSTATUS > 999");
			
			if ($href->{'Rows'}->{'Row'}[0]->{'CRES'} == 0 ) {
				($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
					"SELECT idplatform FROM pfcompat WHERE isrunningon = $fields{'IDPLATFORM'}");
				
				my $href_comp;
				my $rc_comp;
				my $i;
				foreach $idh (@{$href->{'Rows'}->{'Row'}}) {
					($rc_comp, $href_comp) = QAConnect::httpsql_request("QADB_HTTPSQL",
						"SELECT COUNT(*) AS CRES FROM MAKES WHERE " .
						" IDPLATFORM  =  " . $idh->{'IDPLATFORM'} . " AND " .
						" VERSION     = '$fields{'VERSION'}' AND " .
						" IDQASTATUS  =  $fields{'IDQASTATUS'} AND " .
						" IDOBJSTATUS > 999");
					if ($href_comp->{'Rows'}->{'Row'}[0]->{'CRES'} > 0) {
						$fields{'IDPLATFORM'} = $idh->{'IDPLATFORM'};
						last;
					}
				}
			}
		}
		delete ($$args{'PLATFORM'});
		$match_count ++;
		
	    }
	
	    
        if ($match_count != @required_fields) {
                $self->{error_code} = 1;
                $self->{error_text} = &throw_err("The arguments given do not match the needed arguments\n");
		
		return bless $self;
        }
    }
    
   ($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
   	"SELECT * FROM makes WHERE idobjstatus > 999 ".
	"AND idplatform = $fields{'IDPLATFORM'} " .
	"AND idqastatus = $fields{'IDQASTATUS'} ".
	"AND version = '$fields{'VERSION'}' ".
	"AND upper(LCPOOLID) <> 'DLTD' ".
	"ORDER BY id DESC ");
	
	
	
	while (($key, $value) = each (%{$href->{'Rows'}->{'Row'}[0]})) {
		print "\tDBG: $key = $value\n";
		$self->{$key} = $value;
	}
	
	bless $self;
	$self->get_testlist();
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
		"SELECT id FROM servers WHERE HOST = '" . $hostname . "'");
		
	if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
		$self->{error_code} = 1;
		$self->{error_text} = &throw_err("Could not find host $hostname in the serverlist\n");
		
		return bless $self;
	}
	$self->{'IDSERVER'} = $href->{'Rows'}->{'Row'}[0]->{'ID'};
	
	
	$self->{'platformname'} = $self->get_platformname();
	
	if (defined ($sess_args)) {
		$self->create_session($sess_args);
	}
	
	return bless $self;
}


sub new {
    #
    # Lets get initialized
    #
    my $name = shift;
    my $args = shift;
    my $sess_args = shift;

    my $self = {};
    my $hostname = hostname();
    my %fields;

    $hostname =~ tr/A-Z/a-z/;
    push @{$self->{'connectstrings'}}, QAConnect::get_strings("QADB_DBI");
    
    #
    # Initaialize Error-Handlers
    if ($^O =~ /MSWin32/i) {
        $self->{'delimit'}       = "\\"; # As we know, Windows uses backslashes
        $self->{'pathsep'}       = ";";
    }
    else {
        $self->{'delimit'}       = "/";
        $self->{'pathsep'}       = ":";
    }

    $self->{error_code} = 0;
    $self->{error_text} = "";

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
        ($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT columnname FROM columns WHERE mode = 'MAN' AND tablename = 'MAKES'");
        
	unless (defined $href->{'Rows'}->{'Row'}[0]->{'COLUMNNAME'}) {
		$self->{'error_code'} = 1;
		$self->{'error_text'} = &throw_err("Could not find out which make-columns to fill.\n");
		
		return bless $self;
	}
	
	foreach my $rowhdl (@{$href->{'Rows'}->{'Row'}}) {
		push(@required_fields, $rowhdl->{'COLUMNNAME'});
        }
        @required_fields = sort (@required_fields);
        
        my $x; # Loop-Helpers
        my $y; #
        my $match_count = 0; # Counts the matches of given arguments with required db-fields
        foreach $x (@required_fields) {
            foreach $y (keys(%$args)) {
                if ($x  =~ /^$y$/i) {
                    $fields{$x} = $$args{$y};
                    delete($$args{$y});
                    last;
                }
            }
        }
	
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id, idplatform FROM servers WHERE HOST = '" . $hostname . "'");
	
	if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
		$self->{error_code} = 1;
		$self->{error_text} = &throw_err("Counld not find host $hostname in the serverlist\n");
		
		return bless $self;
	}
	
	$fields{'IDSERVER'}   = $href->{'Rows'}->{'Row'}[0]->{'ID'};
	$fields{'IDPLATFORM'} = $href->{'Rows'}->{'Row'}[0]->{'IDPLATFORM'};
	
	
	
	if (scalar(grep /^PLATFORM$/i, keys(%$args)) == 1) {
                ($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id FROM platforms WHERE DESCTEXT = '" . $$args{'PLATFORM'} . "'");
                if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
                    $self->{error_code} = 1;
                    $self->{error_text} = &throw_err("Counld not find you which platform you wanted: $$args{'PLATFORM'} \n");
		    
                    return bless $self;
                }
		
                $fields{'IDPLATFORM'} = $href->{'Rows'}->{'Row'}[0]->{'ID'};
        }
	
	if (!scalar(grep /^IDQASTAUS$/i, keys (%fields)) and (scalar(grep/^QASTATUS$/, keys(%$args)) == 1)) {
		$$args{QASTATUS} =~ tr/[a-z]/[A-Z]/;
		
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id FROM qastatus WHERE desctext = '" . $$args{'QASTATUS'} . "'");
		
		if (scalar (@{$href->{'Rows'}->{'Row'}}) != 1) {
		    $self->{error_code} = 1;
		    $self->{error_text} = &throw_err("Could not find out whiche QASTATUS could me ment with $$args{'QASTATUS'}\n");
		    
		    return bless $self;
		}
		
		$fields{'IDQASTATUS'} = $href->{'Rows'}->{'Row'}[0]->{'ID'};;
		delete ($$args{'QASTATUS'});
	}
	
	
        ######
        ## Now, start to check if the there are already entries in BUILD_PURPOSE and RELEVANT_TESTS
	
	
        #
        # 1. Dateien lesen
        #

        my $hits = 0;

        my %profiles;
        foreach $vmake_entry (split(",", $ENV{'VMAKE_PATH'})) {
            $pathname = "${vmake_entry}$self->{'delimit'}sys"
                . "$self->{'delimit'}src$self->{'delimit'}jtest"
                . "$self->{'delimit'}tests$self->{'delimit'}";

            foreach $pflname ('all', 'lc', 'oltp') {
                if (-e ($pathname . $pflname . ".pfl")) {

                    open (IFH, ($pathname . $pflname . ".pfl"));
                    while (<IFH>) {
                        if ($_ =~ /^[^\#].*@/) {
                            chomp $_;
                            $profiles{$pflname} = &quote_sql2($_);
                            $hits++;
                            last;
                        }
                    }
                }
            }
        }
	
	bless $self;
        if ($hits != 3) {
            print &throw_err("I could not open one or more profile-files");
        } else {
            ($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
	    	"SELECT COUNT(version) AS CRES FROM relevant_tests WHERE version = '$fields{'VERSION'}' AND buildpfx = '$fields{'BUILDPFX'}'");
            
	    if (($href->{'Rows'}->{'Row'}[0]->{'CRES'}) != 1) {
                $self->execDML("INSERT INTO relevant_tests (version, buildpfx, all_ptl, lc_ptl, oltp_ptl) VALUES ('$fields{'VERSION'}', '$fields{'BUILDPFX'}', " .
                "$profiles{'all'}, $profiles{'lc'}, $profiles{'oltp'})");
            } else {
                $self->execDML("UPDATE relevant_tests set all_ptl = $profiles{'all'}, lc_ptl = $profiles{'lc'}, oltp_ptl = $profiles{'oltp'}" .
                    " WHERE version = '$fields{'VERSION'}' AND buildpfx = '$fields{'BUILDPFX'}'");
            }
        }

       ($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
	    	"SELECT COUNT(version) AS CRES FROM build_purpose WHERE version = '$fields{'VERSION'}' AND buildpfx = '$fields{'BUILDPFX'}' AND idplatform = $fields{'IDPLATFORM'}");
        
        if (($href->{'Rows'}->{'Row'}[0]->{'CRES'}) != 1) {
            $self->execDML("INSERT INTO build_purpose (version, buildpfx, idplatform, relevance) VALUES ('$fields{'VERSION'}', '$fields{'BUILDPFX'}', $fields{'IDPLATFORM'}, 'ALL_PTL')");
        }
	
	foreach $key (keys(%$args)) {
		$fields{$key} = $$args{$key};
	}
	
	delete $fields{'PLATFORM'};
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT makes_id.NEXTVAL AS CRES FROM DBA.DUAL");
		
	$self->{'ID'} =  $href->{'Rows'}->{'Row'}[0]->{'CRES'};
	$fields{'ID'} = $self->{'ID'};
	
	unless  ($self->{'ID'} > 1) {
		$self->{error_code} = 1;
		$self->{error_text} = &throw_err("Could not get a Make-ID\n");
		
		return bless $self;
	}
	
	unless ((defined $fields{'VERSION'})    and
		(defined $fields{'BUILDPFX'})   and
		(defined $fields{'IDPLATFORM'}) and
		(defined $fields{'IDQASTATUS'}) and
		(defined $fields{'CHANGELIST'}) and
		(defined $fields{'IDSERVER'})) {
		
		$self->{'error_code'} = 1;
		$self->{'error_text'} = &throw_err("Not all information could be found out\n");
		return bless $self;
	}
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
	"INSERT INTO makes (ID, VERSION, BUILDPFX, IDPLATFORM, IDQASTATUS, CHANGELIST, IDSERVER) VALUES (" .
			"$fields{'ID'}, " .
			"'$fields{'VERSION'}', " .
			"'$fields{'BUILDPFX'}', " .
			"$fields{'IDPLATFORM'}, " .
			"$fields{'IDQASTATUS'}, " .
			"'$fields{'CHANGELIST'}', " .
			"$fields{'IDSERVER'})");
	}
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT * FROM makes WHERE id = $fields{'ID'}");
	
	%field = %{$href->{'Rows'}->{'Row'}[0]};
	while (($key, $value) = each (%field)) {
		print "\tDBG: $key = $value\n";
		$self->{$key} = $value;
	}
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
			"SELECT id, idplatform FROM servers WHERE HOST = '" . $hostname . "'");
	
	$self->{'IDSERVER'}   = $href->{'Rows'}->{'Row'}[0]->{'ID'};
	$self->{'IDPLATFORM'} = $href->{'Rows'}->{'Row'}[0]->{'IDPLATFORM'};
	
	bless $self;
	$self->{'platformname'} = $self->get_platformname();

	if (defined ($sess_args)) {
		$self->create_session($sess_args);
	}
	
	return bless $self;
}

#
# Make qadb(lca) fork save
#
# Release the current DB-connection
sub unlock {
    print "Not used anymore\n";
    return undef;
}


# And re-create it
sub lock {
	print "Not used anymore\n";
	return undef;
}



sub create_session {
	my $self = shift;
	my $sess_args = shift;
	my $rc;
	my $href;
	
	unless (defined $sess_args) {
		print "Wrong arguments in create_session\n";
		return ;
	}
	
	# Install-Session handling
	if ((defined ($sess_args)) && (!defined $$sess_args{'ID'})) {
		my %sess_fields;

	 	foreach $x (keys(%$sess_args)) {
			if ($x  =~ /^IDSESSION_TYPE$/i) {
				($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
				"SELECT count (*) AS CRES FROM session_types WHERE id = " . $x);
				
				if ($href->{'Rows'}->{'Row'}[0]->{'CRES'} == 1) {
					$sess_fields{'idsession_type'} = $$sess_args{$x};
				} else {
					$self->{error_code} = 1;
					$self->{error_text} = &throw_err("The IDSESSION_TYPE $$sess_args{$x} could not be found in SESSION_TYPES\n");
					
					return bless $self;
				}
			}
			elsif ($x =~ /^SESSION_TYPE$/i) {
				($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
				"SELECT id FROM session_types WHERE stext = '" . $$sess_args{$x} . "'");
				if (scalar(@{$href->{'Rows'}->{'Row'}}) == 1) {
					$sess_fields{'idsession_type'} = $href->{'Rows'}->{'Row'}[0]->{'ID'};
				} else {
					$self->{error_code} = 1;
					$self->{error_text} = &throw_err("The SESSION_TYPE(stext) $$sess_args{$x} could not be found in SESSION_TYPES\n");

					return bless $self;
				}
			} elsif ($x =~ /^COMMENT$/i) {
				$sess_fields{'session_comment'} = "'" . $$sess_args{$x} . "'";
			} elsif ($x =~ /^IDOBJSTATUS$/) {
				($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
				"SELECT count(*) AS CRES FROM objstatus WHERE id = " .$$sess_args{$x});
				
				if ($href->{'Rows'}->{'Row'}[0]->{'CRES'} == 1) {
					$sess_fields{'idobjstatus'} = $$sess_args{$x};
				} else {
					$self->{error_code} = 1;
					$self->{error_text} = &throw_err("The IDOBJSTATUS $$sess_args{$x} could not be found in OBJSTATUS\n");
					
					return bless $self;
				}
			} elsif ($x =~/^CHRCNT$i/) {
				if ($$sess_args{$x} =~ /\d\d/) {
					$sess_fields{'chrcnt'} = "'" . $$sess_args{$x} . "'";
				} else {
					$self->{error_code} = 1;
					$self->{error_text} = &throw_err("The CHRCNT $$sess_args{$x} has no valid format\n");

					return bless $self;
				}
			} elsif ($x =~ /^IDMAKE_CPC$/i) {
				($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
				"SELECT count (id) AS CRES FROM makes WHERE id = " . $$sess_args{$x});
				if ($href->{'Rows'}->{'Row'}[0]->{'CRES'} == 1) {
					$sess_fields{'idmake_cpc'} = $$sess_args{$x};
				} else {
					$self->{error_code} = 1;
					$self->{error_text} = &throw_err("The make # $$sess_args{$x} could not be found in MAKES\n");
					
					return bless $self;
				}
			}
		}
		
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
				"SELECT session_seq.NEXTVAL AS CRES FROM DBA.DUAL");
		
		$self->{'SESSION_ID'} =  $href->{'Rows'}->{'Row'}[0]->{'CRES'};
		unless  ($self->{'SESSION_ID'} > 1) {
			$self->{error_code} = 1;
			$self->{error_text} = &throw_err("Could not get a Session-ID\n");
			
			return bless $self;
		}
		$sess_fields{'idmake'} = $self->{ID};
		$sess_fields{'idserver'} = $self->{'IDSERVER'};
		my $sqlcmd = "INSERT INTO  install_sessions SET ";
		
		foreach $x (keys(%sess_fields)) {
			$sqlcmd .= " $x = $sess_fields{$x},";
		}
		
		$sqlcmd .= " id = $self->{'SESSION_ID'}";
		print "\n$sqlcmd\n";
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", $sqlcmd);
        }
	
	if (defined ($sess_args)) {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT * FROM install_sessions WHERE id = " . (
		defined($$sess_args{'ID'}) ? $$sess_args{'ID'} :
			$SQL_SESS_SELECT .= $self->{'SESSION_ID'}));
			
		while (($key, $value) = each (%{$href->{'Rows'}->{'Row'}[0]})) {
			print "\tDBG: SESSION_$key = $value\n";
			$self->{"SESSION_" . $key} = $value;
		}
	}
	
}

sub mark_teststart {
	my $self = shift;
	
	$self->execDML("UPDATE makes SET lcok = FALSE, idobjstatus = 2000 WHERE id = $self->{'id'}'");
}


sub mark_testend {
	my $self = shift;
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT count (*) AS CRES FROM makehist WHERE lcok IS TRUE AND idmake = " . $self->{'ID'});
		
	if ($href->{'Rows'}->{'Row'}[0]->{'CRES'} >= 1) {
		$self->execDML("UPDATE makes SET lcok = TRUE, idobjstatus = 3000 WHERE id = $self->{'id'}'");
	} else {
		$self->check_lcok();
		$self->update_idobjstatus(3000);  ## END OF TESTING
	}
}


sub get_platformname {
	my $self = shift;
	my $rc;
	my $href;
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT PLATFORMNAME FROM pfnames WHERE " .
		" idplatform = $self->{'IDPLATFORM'} AND " . 
		" version    = '$self->{'VERSION'}' AND " . 
		" buildpfx   = '$self->{'BUILDPFX'}'");

	if (scalar(@{$href->{'Rows'}->{'Row'}}) != 1) {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT platformname FROM pfnames WHERE " . 
		" idplatform = $self->{'IDPLATFORM'} AND " .
		" version    = '$self->{'VERSION'}' " .
		" ORDER BY buildpfx DESC ");
		
		if ($href->{'Rows'}->{'Row'}[0]->{'PLATFORMNAME'} =~ /../) {
			$self->{'platformname'} = $href->{'Rows'}->{'Row'}[0]->{'PLATFORMNAME'};
		} else {
			($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
			"SELECT desctext FROM platforms WHERE " .
			" id = $self->{'IDPLATFORM'}");
			$self->{'platformname'} = $href->{'Rows'}->{'Row'}[0]->{'DESCTEXT'};
			print "DBG: Warning - could not find any previous platform name. Took hosts default '$self->{'platformname'}'!\n";
		}
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"INSERT INTO pfnames (VERSION, BUILDPFX, IDPLATFORM, PLATFORMNAME) VALUES ( " .
			"'" . $self->{'VERSION'}      . "', " .
			"'" . $self->{'BUILDPFX'}     . "', " .
			      $self->{'IDPLATFORM'}   . ", "  .
			"'" . $self->{'platformname'} . "')");
		
	} else {
		$self->{'platformname'} = $href->{'Rows'}->{'Row'}[0]->{'PLATFORMNAME'};
	}
	return $self->{'platformname'};
}


sub check_lcok {
	my $self = shift;
	
	# Find out if all the tests happend meanwhile were okay
	my @okseq;
	my $rc;
	my $href;
	my $cmdstring = "SELECT DISTINCT testsequence FROM javatestinfo WHERE" .
			" makekey  = " . $self->{'ID'} . " AND " .
			" driverendstatus = 'OK' AND ".
			" drivererrors = 0 ". 
			" ORDER BY testsequence";
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", $cmdstring);
	
	
	foreach my $namerow (@{$href->{'Rows'}->{'Row'}}){
		$namerow->{'TESTSEQUENCE'} =~ s/\.seq$//;
		push(@okseq, $namerow->{'TESTSEQUENCE'});
	}
	
	my $okseq_cnt = scalar(@{$self->{'worktests'}});
	
	foreach my $x (@{$self->{'worktests'}}) {
			if (scalar(grep /$x/, @okseq) >= 1) {
				$okseq_cnt--;
		}
	}
	
	if ($okseq_cnt <= 0) {
		$self->execDML("UPDATE makes SET lcok = TRUE WHERE id = $self->{'ID'}");
	}
	
	return $self->{'error_code'}
}


sub set_sessiontype {
	my $self = shift;
	my $sessiontype = shift;
	
	$self->execDML("UPDATE install_sessions SET idsession_type = (SELECT id FROM session_types WHERE STEXT = '" . $sessiontype . "') WHERE id = $self->{'SESSION_ID'}");
	return 0;
}

###################

sub get_last_lcapp {
    my $self = shift;
    my $lca_version = shift;
    my $lca_status  = shift;
}

####### 
sub select_count
{
	my $self = shift;
	my $table = shift;
	my $condition = shift;
	
	my $cmdstring = "SELECT count(*) AS CRES FROM $table";
	$cmdstring .= " WHERE $condition" if ($condition);
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", $cmdstring);
	
	return $href->{'Rows'}->{'Row'}[0]->{'CRES'};
}



sub has_been_tested {
	my $self = shift;
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", 
		"SELECT COUNT(idobjstatus) AS CRES " . 
		" FROM makes WHERE " .
		" id = " . $self->{'ID'} .
		" AND idobjstatus > 3000");
		
	return $href->{'Rows'}->{'Row'}[0]->{'CRES'};
}


sub update_columns{
	my $self = shift;
	my $args = shift;
	
	my $key;
	my $value;
	
	my $cmdstring = "UPDATE makes SET ";
	
	while (($key, $value) = each(%$args)) {
		$cmdstring .= " $key = '$value',";
	}
	
	chop $cmdstring; # Das , wegschneiden
	
	$cmdstring .= " WHERE id = " . $self->{'ID'};
	$self->execDML($cmdstring);
	
	if ($self->{error_code} == 1) {
		$self->{error_text} = &throw_err("update_columns: Could not execute Database-SQL-Statement $cmdstring:\n $DBI::errstr");
	}
	
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
	
	###### DEBUG ######
	#if (length($log_text) > 900) {
	#   &throw_err("ExecDML-Aufruf: INSERT INTO makelog (idmake, info) VALUES ($self->{ID}, ${quoted_text})");
	#}
	my $cmdstring = "";
	
	if (defined $self->{'SESSION_ID'}) {
		$cmdstring = "INSERT INTO sessionlog (idsession, info) VALUES ($self->{'SESSION_ID'}, ${quoted_text})";
	} else {
		$cmdstring = "INSERT INTO makelog (idmake, info) VALUES ($self->{ID}, ${quoted_text})";
	}
	$self->execDML($cmdstring);
	
	return $self->{error_code};
}


sub write_prot {
    my $self = shift;
    my $protname = shift;
    my $prot = shift;
    my $info_text = shift;
    my %head_content;

    # Config-Parameters
    # Should mabye be store elsewhere.
    my $httpmachine = "pts.wdf.sap.corp";
    my $httpport    = 1080;
    my $webroot     = "/webdav/lcmakes/";
    
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

    if ($protname =~ /\.(gz|zip|tgz)$/) 
    { $info_text .= " - compressed"; }
    else
    { $head_content{'Content-type'} = 'text/plain'; }
    $dav_req->write_request('PUT', $current_path, %head_content, $prot);

    ($code, $mess, %headers) = $dav_req->read_response_headers();

    if ($code ne "201") {
        $self->{error_code} = 1;
        $self->{error_text} = &throw_err("write_prot: Could not create file entry on the server:\nHTTP-Statuscode: $code \nMessage: $mess\nFile name: '$current_path'\nContent: '$prot'\n");
        return 1;
    }

    #### Okay, it seems that we have succefully created a file on the WebDAV-Server. Let's go on and put a new entry into the Table:

    my $url         = quote_sql2("http://$httpmachine:$httpport" . "$current_path");
    my $url_plain   = "http://$httpmachine:$httpport" . "$current_path";
    my $quoted_text = quote_sql2($info_text);
    my $cmdstring = "";
    if (defined $self->{'SESSION_ID'}) {
	    $cmdstring = "INSERT INTO sessionprot (idsession, info, url) values ($self->{'SESSION_ID'}, $quoted_text, $url)";
    } else {
	    $cmdstring = "INSERT INTO makeprot (idmake, info, url) VALUES ($self->{ID}, $quoted_text, $url)";
    }
    $self->execDML($cmdstring);
    
        if ($self->{error_code} == 1) {
                $self->{error_text} = &throw_err("write_prot: Could not execute Database-SQL-Statement $cmdstring:\n $DBI::errstr");
        return $self->{'error_code'}
    }

    return $url_plain;
}

sub create_testprofile {
	my $self = shift;
	my $profile = shift;
	my $idtestset;
	
	my $rc;
	my $href;
	
	if (!(defined $profile)) {
		$self->{'error_code'} = 1;
		$self->{'error_text'} = "create_testprofile: No argument given!\n";
	}
	
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT id FROM testsets WHERE testset = " . &quote_sql2($profile)); 
		
	if ($href->{'Rows'}->{'Row'}[0]->{'ID'} > 1) {
		$idtestset = $href->{'Rows'}->{'Row'}[0]->{'ID'};
		
	} else {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
			"SELECT testsets_seq.NEXTVAL AS CRES FROM DBA.DUAL");
		$idtestset = $href->{'Rows'}->{'Row'}[0]->{'CRES'};
		$self->execDML("INSERT INTO testsets (id, testset) VALUES " . 
			"( $idtestset, $profile )");
	}
	
	$self->execDML("UPDATE makes SET idtestset = $idtestset WHERE id = " . $self->{'ID'});
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT * FROM makes WHERE id = $self->{'ID'}");
	
	%field = %{$href->{'Rows'}->{'Row'}[0]};
	while (($key, $value) = each (%field)) {
		$self->{$key} = $value;
	}
	
	return $rc;
}


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
		$self->{error_text} = &throw_err("execDML: could not open request for executing SQL: $sqlstmt :\n$@\n$!");
		return 1;
	}
	
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
    my $hostname = hostname();
	my $i = 0;
	my $oldpkg = "";
	my $oldfile = "";
    my $errstr = "";

    while ((caller($i))[0])
    {
    	if (((caller($i))[0] ne $oldpkg) || ((caller($i))[1] ne $oldfile))
    	{
    		$errstr .= "\n  Package '" . (caller($i))[0] . "' in file '" . (caller($i))[1] . "':\n\n";
    		$oldpkg = (caller($i))[0];
    		$oldfile = (caller($i))[1];
       	}
    	$errstr .= "    Func '" . (caller($i))[3] . "' called in line " . (caller($i))[2] . "\n";
    	$i += 1;
    }
	$errortext .= "\n########################################################\n\nFUNCTION TRACE:\n\n$errstr\n";    
    $errortext .= "\n########################################################\nENVIRONMENT:\n";
    for $x (keys(%ENV)) {
        $errortext .= "\n${x} = $ENV{$x}";
    }

    $errortext .= "\n";

    my $smtp = Net::SMTP->new("mail.sap.corp");
    if (!(defined $smtp)) {
	    print "#############################################################################\n";
	    print "################################### ERROR ###################################\n";
	    print "######## COULD NOT CONNECT TO mail.sap.corp                          ########\n";
	    print "######## Wanted to send the following by mail:                       ########\n";
	    print $errortext . "\n";
	    print "################################ END OF ERROR ###############################\n";
	    print "#############################################################################\n";
    }
    else {
	    $smtp->mail("remuser\@is0025.wdf.sap.corp");
	    $smtp->to("falko.flessner\@sap.com", "gerald.arnold\@sap.com");
	    $smtp->data();
	    $smtp->datasend("To: falko.flessner\@sap.com\n");
	    $smtp->datasend("Subject: Error during qadb-run on $hostname\n");
	    $smtp->datasend("Priority: Urgent\nX-Priority: 1 (Highest)\n");
	    $smtp->datasend("\n");
	    $smtp->datasend($errortext);
	    $smtp->dataend();
	    $smtp->quit;
    }
    return $errortext;
}

sub get_testlist {
	my $self = shift;
	
	my $tested = $self->has_been_tested();
	my $parseme;
	
	if ($self->{'IDTESTSET'} != 1) {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
			"SELECT testset FROM testsets WHERE id = " . $self->{'IDTESTSET'});
			$parseme = $href->{'Rows'}->{'Row'}[0]->{'TESTSET'};
	} else {
		($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT relevance, all_ptl, lc_ptl, oltp_ptl FROM to_test " . 
		" WHERE version = '$fields{'VERSION'}'  and " .
		" buildpfx      = '$self->{'BUILDPFX'}' and " .
		" idplatform    = $fields{'IDPLATFORM'}");
		
		$parseme = $href->{'Rows'}->{'Row'}[0]->{$href->{'Rows'}->{'Row'}[0]->{'RELEVANCE'}};
	}
	
	$parseme = (split('@', $parseme))[$tested];
	
	$self->{'worktests'}    = [];
	$self->{'weekendtests'} = [];
	$self->{'lowtests'}     = [];
	$self->{'lowtests_we'}  = [];
	
	foreach $i (split('&', $parseme)) {
		my @variants = split(/\|/, $i);
		my $workday  = ((split(/%/, $variants[0]))[0]);
		my $weekend  = ((split(/%/, $variants[-1]))[0]);
		
		if ($workday =~ /^sut/) {
			push(@{$self->{'lowtests'}}, $workday);
			push(@{$self->{'lowtests_we'}}, $weekend);
		} else {
			push(@{$self->{'worktests'}},    $workday);
			push(@{$self->{'weekendtests'}}, $weekend);
		}
	}
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT seqname, instpfx FROM testnames");
	
	$self->{'testnames'} = {};
	foreach my $namerow (@{$href->{'Rows'}->{'Row'}}) {
		$self->{'testnames'}->{$namerow->{'SEQNAME'}} = $namerow->{'INSTPFX'};
	}
	
	# A LITTLE CANDY: While beeing here, figure out our QASTATUS
	# It will be used later
	
	($rc, $href) = QAConnect::httpsql_request("QADB_HTTPSQL",
		"SELECT desctext FROM qastatus where ID = " . $self->{'IDQASTATUS'});
	
	$self->{'QASTATUS'} = $href->{'Rows'}->{'Row'}[0]->{'DESCTEXT'};
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

=head1 SYNOPSIS OF LOADING AN ENTRY FOR TESTS

 use qadb;
 $qah = qadb->new_test({'PLATFORM' => 'alphaosf', 'VERSION' => 7404, 'QASTATUS' => 'DEV'});

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
for updating IDOBJSTATUS, LCPOOLID, LC_OK and LCOK_TRANS.

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

Please note that B<IDQASTATUS> and B<IDPLATFORM> differ from the
parameters B<QASTATUS> and B<PLATFORM> for the C<new>-constructor. The values stored in the
database are simple numeric representations of their alphanumeric
assignments. These assignments are stored in the tables B<PLATFORMS>
and B<QASTATUS>.

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

