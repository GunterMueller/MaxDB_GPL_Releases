#!/devtool/local/bin/perl
#
#    CheckMake.pm - Module for checking make protocols.
#
#    @(#)Make::CheckMake
#
#    U. Jansen, SAP AG
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

##############################################################################
# Static includes:

package Make::CheckMake;

use strict;
use QAConnect;
use File::Spec;
use Time::Local;
use Mail::Automail;
use Time::Local;
use ICopy;

my %constants = (   'OLD_FILE_TIMESPAN'         => 10,
                    'LAST_SUBMITTER_TIMESPAN'   => 2,
                    'EMAIL_COMMENTLENGTH'       => 60);

my %reason    = (   'NONE'                      => 0,
                    'LAST_SUBMITTER'            => 1,
                    'PLATFORM_RESPONSIBLE'      => 2,
                    'LAYER_RESPONSIBLE'         => 4,
                    'RESPONSIBLE_OLD_FILE'      => 8,
                    'REGEX_MATCHED'             => 16,
                    'DIRECTLY_RESPONSIBLE'      => 32);

my %reason_text = ( 'NONE'                      => '*** This mail has been sent for NO reason! Sorry for that ;-) ***',
                    'LAST_SUBMITTER'            => 'CheckMake was not able to determine a responsible for at least one file. You are among the latest submitters within this release which might be responsible for at least one error.',
                    'PLATFORM_RESPONSIBLE'      => 'you are generally responsible for this platform.',
                    'LAYER_RESPONSIBLE'         => 'you are responsible for one or more layer(s)/file masks having errors.',
                    'RESPONSIBLE_OLD_FILE'      => "of one or more error(s) in file(s) that you have submitted $constants{'OLD_FILE_TIMESPAN'} or more days ago.",
                    'REGEX_MATCHED'             => 'you are responsible for one or more regular expressions matched in error protocols.',
                    'DIRECTLY_RESPONSIBLE'      => 'you may be directly responsible for error(s) in one or more files.');

sub new {
    my $proto = shift;
    my $href   = shift;
    my $class = ref($proto) || $proto;
    my $data  = {};

    bless($data, $class);

    $data->setdata($href) if ($href);
    $data->setfiles(@_) if (@_);

	$data->{'p4call'} = ($ENV{'P4CALL'} ? $ENV{'P4CALL'} : "p4 -u remuser");
	$data->{'p4call'} .= " -p " . ($ENV{'P4PORT'} ? $ENV{'P4PORT'} : ($data->{'release'} =~ /TOOL/ ? 'perforce5001.wdf.sap.corp:5001' : 'perforce3006.wdf.sap.corp:3006'));
	
	$data->{'HOST_NAME'} = `hostname`;
	$data->{'HOST_NAME'} =~ s/\n$//;

	
    print "Reading file/layer responsibles..";
    $data->dbtable2hash('FILE_RESPONSIBLES', 'RESPONSIBLES', 'MASK', 'NAME', "TYPE like 'FILE\%'");
    print "..OK\nReading regex responsibles..";
    $data->dbtable2hash('REGEX_RESPONSIBLES', 'RESPONSIBLES', 'MASK', 'NAME', "TYPE like 'REGEX\%'");
    print ".......OK\nReading platform responsibles..";
    $data->dbtable2hash('PLATFORM_RESPONSIBLES', 'RESPONSIBLES', 'MASK', 'NAME', "TYPE like 'PLATFORM\%'");

    print "....OK\nReading ID Server..";
    $data->{'ID_SERVER'} = $data->getIdServer();

    print "................OK\nReading user/group alias table..";
    $data->dbtable2hash('ALIASES', 'ALIASES', 'ALIAS', 'NAME');
    print "...OK\nReading user information table..";    
    $data->getteldata();
    print "...OK\nGetting P4 release root..........";
	$data->{'p4root'} = ($data->{'release'} =~ /TOOL/ ? $data->getP4ToolRoot() : $data->getP4ReleaseRoot());
	print ($data->{'p4root'} ? "..OK ($data->{'p4root'})\n" : "..FAILED!\n");
	print "Getting local file root..........";
	$data->{'fileroot'} = ($data->{'release'} =~ /TOOL/i ? realname(":MakeTools/Perlscripts/checkmake.pl", 2) : realname(":sp/gsp100.h", 2));
	print ($data->{'fileroot'} ? "..OK ($data->{'fileroot'})\n\n" : "..FAILED!\n\n");



    $data->{'OS_NAME'} = "SUN"     if ( $data->{'HOST_NAME'} =~ /^u.*/i );
    $data->{'OS_NAME'} = "DEC"     if ( $data->{'HOST_NAME'} =~ /^d.*/i );
    $data->{'OS_NAME'} = "AIX"     if ( $data->{'HOST_NAME'} =~ /^i.*/i );
    $data->{'OS_NAME'} = "LINUX"   if ( $data->{'HOST_NAME'} =~ /^l.*/i );
    $data->{'OS_NAME'} = "SNI"     if ( $data->{'HOST_NAME'} =~ /^s.*/i );
    $data->{'OS_NAME'} = "HP"      if ( $data->{'HOST_NAME'} =~ /^h.*/i );
    $data->{'OS_NAME'} = "WINDOWS" if ( $data->{'HOST_NAME'} =~ /^p.*/i );
    $data->{'START_TIME'} = time;
    
    $data->check_qadb();
    
    $data->{'RESPONSIBLES'} = {};
	$data->{'SEND_MASK'} = 0;
    
    return $data;
}

sub setdata {
    my ($self, $href) = @_;
    foreach my $key (keys %{$href}) {$self->{$key} = $href->{$key};}
}

sub setfiles {
    my $self = shift;
    foreach (@_) { push @{$self->{'FILES'}}, $_; }
}

sub check_qadb {
    my $self = shift;
    return 0 unless ($self->{'qaid'}); 
    return 1 if ($self->{'QAH'});
    if($self->{'release'} =~ /TOOL/) {
        require qadbtool;
        $self->{'QAH'} = qadbtool->new({'ID' => "$self->{'qaid'}" });
    } else {
        require qadb;
        $self->{'QAH'} = qadb->new({'ID' => "$self->{'qaid'}"}); #, 'NO_DEBUG' => '1' });
    }
    if ($self->{'QAH'}->{'error_code'}) {
        print "ERROR: Can't get QADB connection! --> $self->{'QAH'}->{'error_text'}\n";
        $self->{'QAH'} = undef;
        return 0;
    }
    return 1;
}

sub dbtable2hash {
    my ($self, $hashname, $table, $col1, $col2, $cond) = @_;
    my ($ok, $href);
    if ($cond) {
        ($ok, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", "SELECT * FROM $table where $cond");
    } else {
        ($ok, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", "SELECT * FROM $table");
    }
    if ($ok) {
        my $rows = scalar(@{$href->{'Rows'}->{'Row'}});
        my $row_count = 0;
        while ($row_count < $rows) {
            if (($href->{'Rows'}->{"Row"}[$row_count]->{'RELFILTER'} ne "?") && ($self->{'release'} !~ $href->{'Rows'}->{"Row"}[$row_count]->{'RELFILTER'})) {
                $row_count ++;
                next;
            }
            my $val = $href->{'Rows'}->{"Row"}[$row_count]->{$col1};
            push @{$self->{$hashname}->{$val}}, $href->{'Rows'}->{"Row"}[$row_count]->{$col2};
            $row_count ++;
        }
    }
}

sub getIdServer{
	my $self = shift;
	my ($ok, $href);
	
	($ok, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", "SELECT id FROM servers WHERE host LIKE '" . lc($self->{'HOST_NAME'}) . "\%'");
	if ($ok && (scalar(@{$href->{'Rows'}->{'Row'}}) == 1)) {
		return $href->{'Rows'}->{"Row"}[0]->{'ID'};
	}
	
	return undef;
}

sub getteldata {
    my $self = shift;
    return undef unless (-e "$ENV{'TOOL'}/bin/tel.pl");
    if (open (TEL_IN, "<$ENV{'TOOL'}/bin/tel.pl")){
        my $sharp = 0;
        while (<TEL_IN>) {
            if (/_DATA__/) {
                $sharp = 1;
                next;
            }
            next unless ($sharp);
            @{$self->{'TELDATA'}->{convertUmlauts($1)}} = ($2, lc($3), $4, $5, $6) if (/"([^,]*)"\s*,\s([^,]*),\s([^,\s]*)\s*,\s\\\\([^,\s]*)\s*,\s([^,\s]*)\s*,\s(.*)$/i);
        }
        close (TEL_IN);
    }
}

sub convertUmlauts {
    my $str = shift;
    $str =~ s/Ä/Ae/g;
    $str =~ s/Ö/Oe/g;
    $str =~ s/Ü/Ue/g;
    $str =~ s/ä/ae/g;
    $str =~ s/ö/oe/g;
    $str =~ s/ü/ue/g;
    $str =~ s/ß/ss/g;
    $str =~ s/é/e/g;
    return $str;
}

sub scanProtocols {
    my $self = shift;

    @{$self->{'ERROR_LINES'}} = ();
    $self->{'ERROR_PROTS'} = 0;
    foreach my $protfile (@{$self->{'FILES'}}) {
        print "Checking make protocol '$protfile' for errors..";
        unless (open(PROT_IN, "<$protfile")) {
            print "..not found (ignored)\n";
            next;
        }
        
        my $errors = 0;
        my $collect_phase = 0;
        my @ProtLines;
    
        while (<PROT_IN>) {
            push @ProtLines, $_;
            $collect_phase = 0 if (/^PHASE:\sMAKE/);
            $errors += 1 if (($_ !~ /^[ -]*$/) && $collect_phase);
            if (/^PHASE:\sCOLLECTING\s\((\S+\s\S+)\sstarted/) {
            	$self->setStartTime($1);
	            $collect_phase = 1;
            }
            $errors += 1 if (/STATUS:\sERROR/);
            foreach my $regex (keys %{$self->{'REGEX_RESPONSIBLES'}}) {
				my ($to_match, $outtext, $send_mask) = split('%', $regex);
                if (/$to_match/) {
                    $errors += 1;
					my $resnames;
					unless (defined $self->{'REGEX_MATCHES'}->{$outtext}) {
						foreach my $responsible (@{$self->{'REGEX_RESPONSIBLES'}->{$regex}}) { 
							$self->setResponsibles($responsible, 'REGEX_MATCHED'); 
						}
						$self->{'REGEX_MATCHES'}->{$outtext} = join(", ", @{$self->{'REGEX_RESPONSIBLES'}->{$regex}});
						$self->{'REGEX_MATCH_COUNT'} += 1;
						if ($send_mask) {
							$self->{'SEND_MASK'} |= $send_mask; 
						}
					}
				}
            }
        }
        close (PROT_IN);
        
        if ($errors) {
            push @{$self->{'ERROR_LINES'}}, @ProtLines;
            $self->{'ERROR_PROTS'} ++;
            print "..$errors errors (including dependency errors)!\n";
        } else {
            print "..none.\n";
        }
    }
    return $self->{'ERROR_PROTS'};
}

sub getErrorTargets {
    my $self = shift;
    my $collect_phase = 0;
    my $collect_error = 0;
    my @collect_lines;
    my @errlines = @{$self->{'ERROR_LINES'}};
	my @cmddesc;
	my @cmderr;

    return 0 unless ($self->{'ERROR_PROTS'});
    while (@errlines) {
        my $line = shift @errlines;
        my $trg = "";
        if ($line =~ /^PHASE:\s+COLLECTING/) {
            $collect_phase = 1;
            @collect_lines = ();
            $collect_error = 0;
        } elsif ($line =~ /^PHASE:\s+MAKE\s+TARGET:\s+(\S+)\s+VERSION/) {
            $trg = killVariants($1);
            if ($collect_error && !defined($self->{'ERROR_TARGETS'}->{$trg})) {
                $self->{'ERROR_TARGETS'}->{$trg} = ++$self->{'ERROR_TARGET_COUNT'};
                $self->{'ERROR_LAYERS'}->{$1}    = ++$self->{'ERROR_LAYER_COUNT'} if ($trg =~ /::?(.*)\/[^\/]*$/);
                $self->{'ERROR_DESCS'}->{$trg}    = join(" ", @collect_lines);
                $self->{'ERROR_DESCS'}->{$trg}    = (substr($self->{'ERROR_DESCS'}->{$trg}, 0, $constants{'EMAIL_COMMENTLENGTH'}) . "...") if (length($self->{'ERROR_DESCS'}->{$trg}) > $constants{'EMAIL_COMMENTLENGTH'});
            }
            $collect_phase = 0;
        } elsif ($collect_phase && ($line !~ /^[ -]*$/)) {
            $collect_error = 1;
            chomp $line;
            push @collect_lines, $line;
        } elsif ($line =~ /^TARGET:\s+command\s+'(.*)'\s+in\s(\S+)\s+STATUS:\sERROR/) {
			unshift @cmderr, $1;
			unshift @cmddesc, $2;
        } elsif ($line =~ /TARGET:\s*'(.*)'\s*.*ERROR.*/) {
			my $tmptrg = $1;
            $trg = killVariants($tmptrg);
			if ((scalar(@cmddesc)) && ($cmddesc[0] ne "") && ($tmptrg =~ /.*\Q$cmddesc[0]\E$/)) {
				unless (defined $self->{'ERROR_TARGETS'}->{$trg}) {
					$self->{'ERROR_TARGETS'}->{$trg} = ++$self->{'ERROR_TARGET_COUNT'};
					$self->{'ERROR_LAYERS'}->{$1}    = ++$self->{'ERROR_LAYER_COUNT'} if ($trg =~ /::?(.*)\/[^\/]*$/);
					$self->{'ERROR_DESCS'}->{$trg}    = "Command error in '$cmderr[0]'";
					$self->{'ERROR_DESCS'}->{$trg}    = (substr($self->{'ERROR_DESCS'}->{$trg}, 0, $constants{'EMAIL_COMMENTLENGTH'}) . "...'") if (length($self->{'ERROR_DESCS'}->{$trg}) > $constants{'EMAIL_COMMENTLENGTH'});
				}
				shift @cmddesc;
				shift @cmderr;
			} else {
				unless (defined $self->{'ERROR_TARGETS'}->{$trg}) {
					if ($errlines[1] =~ /not made because of dependency errors of/) {
						$self->{'ERROR_DEPENDS'}->{$trg} = ++$self->{'ERROR_DEPEND_COUNT'};
					} else {
						$self->{'ERROR_TARGETS'}->{$trg} = ++$self->{'ERROR_TARGET_COUNT'};
						$self->{'ERROR_LAYERS'}->{$1}    = ++$self->{'ERROR_LAYER_COUNT'} if ($trg =~ /::?(.*)\/[^\/]*$/);
					}
				}
			}
        }
    }
	
	while (@cmddesc) {
		my $trg = killVariants($cmddesc[0]);
		unless (defined $self->{'ERROR_TARGETS'}->{$trg}) {
			$self->{'ERROR_TARGETS'}->{$trg} = ++$self->{'ERROR_TARGET_COUNT'};
			$self->{'ERROR_LAYERS'}->{$1}    = ++$self->{'ERROR_LAYER_COUNT'} if ($trg =~ /::?(.*)\/[^\/]*$/);
			$self->{'ERROR_DESCS'}->{$trg}    = "Unresolved description / Command error in '$cmderr[0]'";
			$self->{'ERROR_DESCS'}->{$trg}    = (substr($self->{'ERROR_DESCS'}->{$trg}, 0, $constants{'EMAIL_COMMENTLENGTH'}) . "...'") if (length($self->{'ERROR_DESCS'}->{$trg}) > $constants{'EMAIL_COMMENTLENGTH'});
		}
		shift @cmddesc;
		shift @cmderr;
	}
    return 1;
}

sub killVariants {
	my $trg = shift;
	return "$1$2$3" if ($trg =~ /^(.*)(\w+)[-\+]\w+(\.\w+)$/);
	return $trg;
}

sub getResponsibles {
    my $self = shift;
    
	@{$self->{'HEADER_TARGETS'}} = ();
    print "Getting responsibles:\n\n";
    foreach my $target (keys %{$self->{'ERROR_TARGETS'}}) {
        print "Target '$target'..";
        my $releasepart = realname($target, 1);
        if ($releasepart) {
            my $p4rc = $self->getP4Responsible("$self->{'p4root'}/$releasepart", $target);
            if ($p4rc == 2) {
                $self->getLatestSubmitters();
                print "..found old P4 submission.\n";
            } elsif ($p4rc == 0) {
                $self->getFileResponsible("$self->{'fileroot'}/$releasepart", $target);
                print "..took file header info.\n";
            } else {
				push @{$self->{'HEADER_TARGETS'}}, $target;
                print "..took P4 submitter.\n";
            }
            $self->checkLayerResponsibles($releasepart);
        } else {
            $self->checkLayerResponsibles($target);
            $self->getLatestSubmitters();
            print "..file not found.\n";
            $self->{'ERROR_DESCS'}->{$target} = ($self->{'ERROR_DESCS'}->{$target} ? "file not found, $self->{'ERROR_DESCS'}->{$target}" : "file not found");
        }
    }
    unless ($self->{'ERROR_TARGET_COUNT'}) {
    	$self->getLatestSubmitters();
    }
    if ($ENV{'MACHINE_CONFIG'}) {
        foreach my $machtype (keys %{$self->{'PLATFORM_RESPONSIBLES'}}) {
            if ($ENV{'MACHINE_CONFIG'} =~ /$machtype/i) {
                foreach my $responsible (@{$self->{'PLATFORM_RESPONSIBLES'}->{$machtype}}) {
                    $self->setResponsibles($responsible, 'PLATFORM_RESPONSIBLE');
                }
            }
        }
    }
    print "\n";
}

sub realname {
    my ($name, $type) = @_; 
	my $hostpart;
	my $releasepart;
	#type: 0=fullpath, 1=release part, 2=host part 
    my $fullpath = ICopy::GetFilePath($name, 0, 0, 1);
	($hostpart, $releasepart) = ($1, $2) if ($fullpath =~ /^(.*)[\\\/](sys[\\\/].*)$/);
	unless (-e "$fullpath") {
		foreach $hostpart (split(",", $ENV{'VMAKE_PATH'})) {
			$hostpart = $1 if ($hostpart =~ /^(.*)[\\\/][\\\/]$/);
			last if (-e "$hostpart/$releasepart");
		}
	}
	$fullpath = "$hostpart/$releasepart" if (-e "$hostpart/$releasepart");
	if    ($type == 0) { return "$hostpart/$releasepart"; }
	elsif ($type == 1) { return "$releasepart"; }
	else 			   { return "$hostpart"; }
}

sub getLatestSubmitters {
    my $self = shift;
    
    return 1 if ($self->{'LATEST_SUBMITTERS'});
    my $sharp = ($self->{'chlist'} ? 0 : 1);
    my $limit_path = $self->{'p4root'} . "/...";
    
    my $olddate = "";
    my $days = 0;
    open (CMD_OUT, "$self->{'p4call'} changes -s submitted $limit_path |") or return 0;
    while (<CMD_OUT>) {
        my ($chlist, $date, $duser) = ($1, $2, lc($3)) if (/^Change\s+(\d+)\s+on\s+(\d+\/\d+\/\d+)\s+by\s+(d\d+)@.*/i);
		next if ($duser =~ /makefact/i);
        unless ($sharp) {
            if ($chlist <= $self->{'chlist'}) {
                $sharp = 1;
            } else {
                next;
            }
        }
        if ($date ne $olddate) {
            $olddate = $date;
            $days ++;
        }
        last if ($days > $constants{'LAST_SUBMITTER_TIMESPAN'});
        $self->setResponsibles($duser, 'LAST_SUBMITTER');
    }
    close (CMD_OUT);
    $self->{'LATEST_SUBMITTERS'} = 1;
    return 1;
}

sub getP4Responsible {
    my ($self, $file, $target_name) = @_;
    my $sharp = ($self->{'chlist'} ? 0 : 1);
	my $already_fixed = 0;
    my $rc = 0;
    open (CMD_OUT, "$self->{'p4call'} filelog $file |") or return 0;
    while (<CMD_OUT>) {
        if (/change\s+(\d+)\s+.*on\s(\d+)\/(\d+)\/(\d+)\s+by\s+(d\d+)@.*/i) {
            my ($chlist, $year, $mon, $day, $duser) = ($1, $2, $3, $4, lc($5));
            unless ($sharp) {
                if ($chlist <= $self->{'chlist'}) {
                    $sharp = 1;
                } else {
					$already_fixed = 1;
                    next;
                }
            }
            if (getTimeDiffDays($year, $mon, $day) >= $constants{'OLD_FILE_TIMESPAN'}) {
                my $fullname = $self->dUser2FullName($duser);
                my $tmptarget; 
                if ($fullname) {
                    $tmptarget = ($already_fixed ? "P4: $fullname (old file, new version already available)" : "P4: $fullname (old file)");
                } else {
                    $tmptarget = ($already_fixed ? "old file (new version already available)" : "old file");
                }
                $self->{'ERROR_DESCS'}->{$target_name} = ($self->{'ERROR_DESCS'}->{$target_name} ? "$tmptarget, $self->{'ERROR_DESCS'}->{$target_name}" : "$tmptarget");
                $rc = 2;
                $self->setResponsibles($duser, 'RESPONSIBLE_OLD_FILE');
            } else {
                my $fullname = $self->dUser2FullName($duser);
                if ($fullname) {
                    $self->{'ERROR_DESCS'}->{$target_name} = ($self->{'ERROR_DESCS'}->{$target_name} ? "P4: $fullname" . ($already_fixed ? " (new version already available)" : "") . ", $self->{'ERROR_DESCS'}->{$target_name}" : "P4: $fullname" . ($already_fixed ? " (new version already available)" : ""));
                }
                $rc = 1;
                $self->setResponsibles($duser, 'DIRECTLY_RESPONSIBLE');
            }
            last;
        }
    }
    close (CMD_OUT);
    return $rc;
}

sub dUser2FullName {
    my ($self, $duser) = @_;
    
    foreach my $fullname (keys %{$self->{'TELDATA'}}) {
        return $fullname if ($self->{'TELDATA'}->{$fullname}->[1] eq $duser);
    }
    return undef;
}

sub getTimeDiffDays {
    my ($year, $mon, $day) = @_;
    my $now = time();
    my $past = timelocal(0,0,0,$day, $mon - 1, $year);
    return (($now - $past)/86400);
}

sub getFileResponsible {
    my ($self, $file, $target_name) = @_;
    my @authors;
    my $found = 0;
    my $responsibility = (((time() - (stat($file))[9])/86400 > $constants{'OLD_FILE_TIMESPAN'}) ? 'RESPONSIBLE_OLD_FILE' : 'DIRECTLY_RESPONSIBLE');

    open (IN_FILE, "<$file") or return 0;
    while (<IN_FILE>) {
        if (/\.TT\s3\s\$([^\$]+)[\$]+.*\$\d{4}-\d{2}-\d{2}\$$/i) { $authors[0] = $1; last; }
        if (/responsible\s*:\s+(\S.*\S)\s*$/i) { @authors = split(/, /,$1); last; }
        if (/Author\s*:\s+(\S.*\S)\s*$/i) { @authors = split(/, /,$1); last; }
        if (/\@author\s+(\S.*\S)\s*$/i) { @authors = split(/, /,$1); last; }
    }
    close (IN_FILE);
    
    if (@authors) {
        $self->{'ERROR_DESCS'}->{$target_name} = "File header: " . join(", ", @authors) . ($responsibility eq 'RESPONSIBLE_OLD_FILE' ? " (old file)" : "") unless ($self->{'ERROR_DESCS'}->{$file});
        foreach my $author (@authors) { $found = $self->setResponsibles($author, $responsibility); }
    }
    return $found;
}

sub setResponsibles {
    my ($self, $name, $res_str) = @_;
    my $duser = "";
    my $rc = 0;

    if (defined $self->{'ALIASES'}->{$name}) {
        foreach my $alias (@{$self->{'ALIASES'}->{$name}}) {
            $rc += $self->setResponsibles($alias, $res_str);
        }
        return $rc;
    } elsif ($name =~ /^d\d+/i) {
        $duser = lc($name);
        $rc = 1;
    } elsif ($self->{'TELDATA'}->{$name}) {
        $duser = lc($self->{'TELDATA'}->{$name}->[1]);
        $rc = 1;
    } elsif ($name =~ /^\S+$/i) {
        foreach my $vollname (keys %{$self->{'TELDATA'}})
        {
            if ($name eq $self->{'TELDATA'}->{$vollname}->[4]) {
                $duser = lc($self->{'TELDATA'}->{$vollname}->[1]);
                $rc = 1;
                last;
            }
        }
    } else {
        return 0;
    }
    
    if ($self->{'RESPONSIBLES'}->{$duser}) {
        if ($self->{'RESPONSIBLES'}->{$duser} < $reason{$res_str}) {
            $self->{'RESPONSIBLES'}->{$duser} = $reason{$res_str};
        }
    } else {
        $self->{'RESPONSIBLES'}->{$duser} = $reason{$res_str};
    }

    return $rc;
}

sub checkLayerResponsibles {
    my ($self, $file) = @_;
    foreach my $layer_regex (keys %{$self->{'FILE_RESPONSIBLES'}}) {
        if ($file =~ /$layer_regex/) {
            foreach my $responsible (@{$self->{'FILE_RESPONSIBLES'}->{$layer_regex}}) { $self->setResponsibles($responsible, 'LAYER_RESPONSIBLE'); }
        }
    }
}

sub sendMails {
    my $self = shift;
    my $extra_warn = 0;
    
    print "Sending mail(s)..";
    
    foreach my $key (keys %{$self->{'RESPONSIBLES'}}) {
        if ($self->{'RESPONSIBLES'}->{$key} > 1) {
            $self->{'SEND_MASK'} = 62;
            $extra_warn = 1;
        }
    } 

	$self->{'SEND_MASK'} = 63 unless ($self->{'SEND_MASK'});
    $self->{'MAIL_BODY'}  = "Make Start      = " . scalar localtime($self->{'START_TIME'}) . "\n";
    $self->{'MAIL_BODY'} .= "Release         = $self->{'release'}\n";
    $self->{'MAIL_BODY'} .= "Hostname        = $self->{'HOST_NAME'}\n";
    $self->{'MAIL_BODY'} .= "Changelist      = $self->{'chlist'}\n" if ($self->{'chlist'});
    $self->{'MAIL_BODY'} .= "QADB ID         = $self->{'qaid'}\n" if ($self->{'qaid'});
    $self->{'MAIL_BODY'} .= "Platform        = $ENV{'MACHINE_CONFIG'}\n" if ($ENV{'MACHINE_CONFIG'});
    $self->{'MAIL_BODY'} .= "Error protocol  = $self->{'LINK_NAME'}\n" if ($self->{'LINK_NAME'});
    $self->{'MAIL_BODY'} .= "Make details    = http://pts:1081/TestMonitor/" . ($self->{'release'} =~ /TOOL/ ? "Tool" : "") . "Make_Details.jsp?id=$self->{'qaid'}\n" if ($self->{'qaid'});
    $self->{'MAIL_BODY'} .= "Server details  = http://pts:1081/TestMonitor/Server.jsp?id=" . $self->{'ID_SERVER'} . "&cmd=detail\n" if ($self->{'ID_SERVER'});
    $self->{'MAIL_BODY'} .= "\n---------------------------------------------------\n\n";
    
	if ($self->{'REGEX_MATCH_COUNT'}) {
		$self->{'MAIL_BODY'} .= "Regular expressions matched ($self->{'REGEX_MATCH_COUNT'}):\n\n";
		foreach my $desctext (keys %{$self->{'REGEX_MATCHES'}}) {
			$self->{'MAIL_BODY'} .= "$desctext (Responsible: $self->{'REGEX_MATCHES'}->{$desctext})\n";
		}
        $self->{'MAIL_BODY'} .= "\n";
	}
	
    if ($self->{'ERROR_TARGET_COUNT'})
    {
        $self->{'MAIL_BODY'} .= "Affected targets ($self->{'ERROR_TARGET_COUNT'}):\n\n";
        foreach my $target (sort keys %{$self->{'ERROR_TARGETS'}}) {
            $self->{'MAIL_BODY'} .= "$target" . ($self->{'ERROR_DESCS'}->{$target} ? " ($self->{'ERROR_DESCS'}->{$target})\n" : "\n");
        }
        $self->{'MAIL_BODY'} .= "\n";
    }
    
    if ($self->{'ERROR_DEPEND_COUNT'}) {
        $self->{'MAIL_BODY'} .= "Dependency errors ($self->{'ERROR_DEPEND_COUNT'}):\n\n";
        foreach my $dependency (sort keys %{$self->{'ERROR_DEPENDS'}}) {
            $self->{'MAIL_BODY'} .= "$dependency\n";
        }
        $self->{'MAIL_BODY'} .= "\n";
    }
    
    if ($self->{'ERROR_LAYER_COUNT'}) {
        $self->{'MAIL_BODY'} .= "Affected Layers ($self->{'ERROR_LAYER_COUNT'}):\n\n";
        foreach my $layer (sort keys %{$self->{'ERROR_LAYERS'}}) {
            $self->{'MAIL_BODY'} .= "$layer\n";
        }
    }

    my $tinytitle = @{$self->{'HEADER_TARGETS'}} ? join(', ', @{$self->{'HEADER_TARGETS'}}) : join (', ', keys %{$self->{'ERROR_TARGETS'}});
    $tinytitle = substr ($tinytitle, 0, 50) . "..." if (length($tinytitle) > 50);
    
    my %mail = ( 'Category' => 'Make',
                 'Type'     => 'Error');

    $self->{'MAIL_OK'} = 1;

    if ($self->{'email'}) {
		$mail{'Infotext'} = "Manual receiver ($tinytitle)";
        $mail{'To'} = $self->{'email'};
        push @{$self->{'MAIL_RECEIVERS'}}, $mail{'To'};
		$self->{'MAIL_BODY'} .= "\n\n---------------------------------------------------\n\n";
		$self->{'MAIL_BODY'} .= "Original Receivers:\n\n";
        foreach my $type (keys %reason) {
			next unless ($self->{'SEND_MASK'} & $reason{$type});
			my @receivers;
			foreach my $duser (keys %{$self->{'RESPONSIBLES'}}) {
				if ($self->{'RESPONSIBLES'}->{$duser} == $reason{$type}) {
					foreach my $vollname (keys %{$self->{'TELDATA'}}) {
						if ($self->{'TELDATA'}->{$vollname}->[1] eq $duser) {
							push @receivers, $self->{'TELDATA'}->{$vollname}->[3];
							last;
						}
					}
				}
			}
			if (@receivers) {
				$self->{'MAIL_BODY'} .= "$type:\n";
				$self->{'MAIL_BODY'} .= "\t" . join("\n\t", @receivers) . "\n\n";
			}
		}
		
		$self->{'MAIL_BODY'} .= "\nThis mail has been automatically created by CheckMake v2.03\n\n";
        $mail{'Message'} = "\nYou are receiving this mail because you have been manually set as email receiver.\n\n". $self->{'MAIL_BODY'};
		if (!Mail::Automail::Send(%mail)) {
            print "FATAL: sendmail error $Mail::Sendmail::error\n";
            $self->{'MAIL_OK'} = 0;
        }
    } else {
        # Generate the complete receiver list:
		$self->{'MAIL_BODY'} .= "\n\n---------------------------------------------------\n\n";
		$self->{'MAIL_BODY'} .= "Complete receiver list (who else got this mail?):\n\n";
        foreach my $type (keys %reason) {
			next unless ($self->{'SEND_MASK'} & $reason{$type});
			my @receivers;
			foreach my $duser (keys %{$self->{'RESPONSIBLES'}}) {
				if ($self->{'RESPONSIBLES'}->{$duser} == $reason{$type}) {
					foreach my $vollname (keys %{$self->{'TELDATA'}}) {
						if ($self->{'TELDATA'}->{$vollname}->[1] eq $duser) {
							push @receivers, $self->{'TELDATA'}->{$vollname}->[3];
							last;
						}
					}
				}
			}
			if (@receivers) {
				$self->{'MAIL_BODY'} .= "$type:\n";
				$self->{'MAIL_BODY'} .= "\t" . join("\n\t", @receivers) . "\n\n";
			}
		}
		
		$self->{'MAIL_BODY'} .= "\nThis mail has been automatically created by CheckMake v2.03\n\n";
        foreach my $type (keys %reason) {
			next unless ($self->{'SEND_MASK'} & $reason{$type});
            my @receivers;
            $mail{'To'} = "";
            foreach my $duser (keys %{$self->{'RESPONSIBLES'}}) {
                if ($self->{'RESPONSIBLES'}->{$duser} == $reason{$type}) {
                    foreach my $vollname (keys %{$self->{'TELDATA'}}) {
                        if ($self->{'TELDATA'}->{$vollname}->[1] eq $duser) {
                            push @receivers, $self->{'TELDATA'}->{$vollname}->[3];
                            last;
                        }
                    }
                }
            }
            if (@receivers) {
				my $mtype = lc($type);
				$mtype = ucfirst($mtype);
				$mtype =~ s/_/ /g;
				$mail{'Infotext'} = "$mtype ($tinytitle)";
                $mail{'To'} = join(",", @receivers);
                push @{$self->{'MAIL_RECEIVERS'}}, @receivers;
                $mail{'Message'} = "\nYou are receiving this mail because " . $reason_text{$type} . "\n\n" . $self->{'MAIL_BODY'};
                if (!Mail::Automail::Send(%mail)) {
                    print "FATAL: sendmail error $Mail::Sendmail::error\n";
                    $self->{'MAIL_OK'} = 0;
                }
            }
        }
    }
    
    $self->{'SENT_EMAIL'} = "To:\n\n";
    $self->{'SENT_EMAIL'} .= "\t" . join("\n\t", @{$self->{'MAIL_RECEIVERS'}});
    $self->{'SENT_EMAIL'} .= "\n\n";
    $self->{'SENT_EMAIL'} .= $self->{'MAIL_BODY'};
    
    if ($self->check_qadb()) {
        if (!$self->{'MAIL_OK'}) {
            $self->{'QAH'}->write_log("ERROR: Couldn't send error mail!");
        } else {
            my @localtime = localtime(time);
            my $protname = "EMail_" . $self->{'OS_NAME'}. "_" . $self->{'release'} . "_" . sprintf ("%04d%02d%02d%02d%02d", $localtime[5] + 1900, $localtime[4] + 1, $localtime[3], $localtime[2], $localtime[1]);
            $self->{'QAH'}->write_prot("$protname", "$self->{'SENT_EMAIL'}", "Copy of sent automake mail");
            if ($self->{'QAH'}->{'error_code'}) {
                print "ERROR: Can't upload email to QADB!\n$self->{'QAH'}->{'error_text'}\n";
            }
        }
    }

    if ($self->{'outfile'}) {
        unlink ("$self->{'outfile'}") if (-e "$self->{'outfile'}");
        if (open (OUT_FILE, "> $self->{'outfile'}")) {
	    print OUT_FILE "\n\n(----------------------- CHECKMAKE OUTPUT -----------------------)\n\n";
            print OUT_FILE "$self->{'SENT_EMAIL'}\n";
            close (OUT_FILE);
        }
    }
    
    print "..OK\n" if ($self->{'MAIL_OK'});
}

sub uploadErrProt {
    my $self = shift;
    
    print "Uploading Error Protocol..";
    my @localtime = localtime(time);
    my $protname = $self->{'OS_NAME'}. "_" . $self->{'release'} . "_" . sprintf ("%04d%02d%02d%02d%02d", $localtime[5] + 1900, $localtime[4] + 1, $localtime[3], $localtime[2], $localtime[1]);
    my $success = 0;
    my $rc = 0;
    my $prot = join ("", @{$self->{'ERROR_LINES'}});
    if ($self->check_qadb()) {
        $self->{'LINK_NAME'} = $self->{'QAH'}->write_prot("$protname", "$prot", "Error Protocol (.e0 file summary)");
        if ($self->{'QAH'}->{'error_code'}) {
            print "..ERROR!\n$self->{'QAH'}->{'error_text'}\n";
        } else {
            $success = 1;
            print "..OK\n";
        }
    } else {
        print "..NO QA-ID available!\n";
    }
    unless ($success) {
        print "Trying to upload to webdav..";
        open (OUT,">$protname");
        print OUT $prot;
        close OUT;
        if ($^O=~/.*win32.*/i)
        {   $rc = system("python $ENV{TOOL}\\bin\\webdav.py $protname"); }
        else
        {   $rc = system("webdav.py $protname");   }
        unlink ($protname);
        if ($rc) {
            print "..ERROR!\n";
            $self->{'LINK_NAME'} = undef;
        } else {
            print "..OK\n";
            $self->{'LINK_NAME'} = "http://pts.wdf.sap.corp:1080/webdav/daily_makes/$protname";
        }
    }
}

sub setStartTime {
	my ($self, $time) = @_;
	
	if ($time =~ /(\d\d)-(\d\d)-(\d\d)\s(\d\d):(\d\d):(\d\d)/) {
		my $longtime = timelocal($6, $5, $4, $1, ($2 - 1), ($3 + 100));
		$self->{'START_TIME'} = $longtime if ($longtime > 0 && ($longtime < $self->{'START_TIME'}));
	}
}

sub getP4Dir
{
    my ($self, $dir) = @_;

    my $href = {};

    $dir =~ tr/\\/\//;
    $dir = "//" . $dir unless ($dir =~ /^\/\/(.*)$/);
    $dir = $1 if ($dir =~ /^(\/\/.*)\/$/);

    $href->{'DIRS'} = ();

    if (open DIR_OUT, "$self->{'p4call'} dirs $dir/* 2>&1 |") {
		while (<DIR_OUT>) {
			last if (/.*no\ssuch\sfile/);
			push @{$href->{'DIRS'}}, $1 if (/^$dir\/(.*)$/);
		}
		close DIR_OUT;
	}

    $href->{'FILES'} = ();

    if (open FILE_OUT, "$self->{'p4call'} fstat $dir/* 2>&1 |")
	{
		while (<FILE_OUT>) {
			last if (/.*no\ssuch\sfile/);
			push @{$href->{'FILES'}}, $1 if (/^\.\.\.\s+depotFile\s+$dir\/(.*)$/);
			pop @{$href->{'FILES'}} if (/^\.\.\.\s+headAction\s+delete$/);
		}
		close FILE_OUT;
	}
    return $href;
}

sub getP4ToolRoot {
	my $self = shift;
	my $toolrel = ($ENV{'TOOLREL'} ? $ENV{'TOOLREL'} : "dev");
	return "//prodprojects/vmake/$toolrel/src/_tools/maketools";	
}

sub getP4ReleaseRoot {
    my $self = shift;
	return "//sapdb/DEV/develop" if ($self->{'release'} =~ /^DEV$/i);
	my ($lc_ver, $lc_cor, $lc_build) = ($1, $2, $3) if ($self->{'release'} =~ /^(\d\d)(\d\d)(\S+)$/);
	$lc_build = "develop" if ($lc_build =~ /^dev/i);
    my $dirstr = "//sapdb/V$lc_ver/c_$lc_cor/";
    $lc_build = lc ($lc_build);
    if ($lc_build =~ /^\d\d$/) {
        $dirstr .= "b_";
    }
    $dirstr .= "$lc_build";

    my $dir = $self->getP4Dir($dirstr);

    return $dirstr if ($dir->{'DIRS'});
	
	if ($lc_build =~ /^ramp$/) {
        if ($dir->{'FILES'}) {
            if (open(BUILD_NR, "$self->{'p4call'} print -q $dirstr/buildnumber.cfg |")) {
                if (<BUILD_NR> =~ /^build=(b_\d\d)$/) {
                    $lc_build = $1;
                    $dirstr =~ s/ramp/$lc_build/;
                    close (BUILD_NR);
                    return $dirstr;
                }
                close (BUILD_NR);
                return "";
            }
        }
		return "";
    }
	
    return "" if (($lc_build !~ /^dev.*$/) && $lc_build);

    $lc_build = "develop";
    undef $dir;
    $dir = $self->getP4Dir("//sapdb/V$lc_ver");

    if ($dir->{'DIRS'}) {
        my @cors = grep { /^c_\d\d$/ } reverse sort @{$dir->{'DIRS'}};
        if (@cors) {
            my $tmp_cor = ($lc_cor - 1);
            if (($cors[0] =~ /^c_$lc_cor$/) || ($cors[0] =~ /^c_0?$tmp_cor$/)) {
                $dir = $self->getP4Dir("//sapdb/V$lc_ver/develop");
                if ($dir->{'DIRS'}) { return "//sapdb/V$lc_ver/develop"; }
                else { return "//sapdb/DEV/develop"; }
            }
        } elsif ($dir->{'DIRS'}->[0] =~ /^develop$/) {
            return "//sapdb/V$lc_ver/develop";
        } elsif ($lc_ver > 0) {
			return "//sapdb/DEV/develop";
		}
    } elsif ($lc_ver > 0) {
	    return "//sapdb/DEV/develop";
	}
    return "";
}

1;
