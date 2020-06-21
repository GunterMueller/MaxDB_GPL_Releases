#! /usr/bin/perl
#

package SysCheck;

use strict;
use QAConnect;
use Mail::Sendmail;

$| = 1;

my $sdbinfo_exe = ($^O =~ /win32/i ? "\\sapdb\\programs\\bin\\sdbinfo.exe" : "/SAP_DB/programs/bin/sdbinfo");
my $separator   = ($^O =~ /win32/i ? "\\" : "/");

sub run_check {
    my ($version, %email) = @_;
    my ($changed_parts, $old_config, $new_config) = ('','','');
    my %sections;
    my $ts = QAConnect::getisotime(time);
    
    chomp (my $hostname = `hostname`);
    
    # DBG:
    $hostname = 'hs0030';

    # Get Host & Sections information:
    my ($ok, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", "select A.ID as IDSERVERCONFIG, A.VARNAME as SECTION, A.VARVALUE as TITLE, B.ID AS IDSERVER from SERVERCONFIG A, SERVERS B where B.HOST = '$hostname' and B.ID = A.IDSERVER and A.TARGET = 'INFO'", 1);
    QAConnect::throw_error("CONFIG ERROR", "This host ($hostname) has either no entry in the SERVERS table\nor there are no configuration sections defined in the SERVERCONFIG table!", 1) unless (ref($href) && $href->{'Rows'}->{'Row'}[0]->{'IDSERVERCONFIG'});
    for (my $i; $i < scalar(@{$href->{'Rows'}->{'Row'}}); $i++) { $sections{$href->{'Rows'}->{'Row'}[$i]->{'SECTION'}} = [$href->{'Rows'}->{'Row'}[$i]->{'IDSERVERCONFIG'}, "$href->{'Rows'}->{'Row'}[$i]->{'TITLE'}", ""]; }
    
    # Find the sdbinfo executable:
    unless (-e $sdbinfo_exe) {
        my ($loc_ok, $loc_href) = QAConnect::httpsql_request("QADB_HTTPSQL", "select VARVALUE FROM SERVERCONFIG WHERE IDSERVER=$href->{'Rows'}->{'Row'}[0]->{'IDSERVER'} and VARNAME='indeppath'", 1);
        QAConnect::throw_error("CONFIG/INSTALL ERROR", "The needed executable 'sdbinfo' is not in default location and the\nSERVERCONFIG has no entry for a different indep programs path!", 1) unless ($loc_href->{'Rows'}->{'Row'}[0]->{'VARVALUE'});
        $sdbinfo_exe = $loc_href->{'Rows'}->{'Row'}[0]->{'VARVALUE'} . $separator . "bin" . $separator . ($^O =~ /win32/i ? "sdbinfo.exe" : "sdbinfo");
        QAConnect::throw_error("CONFIG/INSTALL ERROR", "The needed executable '$sdbinfo_exe' could not be found!", 1) unless (-e $sdbinfo_exe);
    }
    
    # Execute sdbinfo and parse the selected sections:
    my $act_section = undef;
    open (CMD_OUT, "$sdbinfo_exe |") or QAConnect::throw_error("COMMAND ERROR", "Failed to execute command '$sdbinfo_exe'!\n$!", 1);
    while (<CMD_OUT>) {
        next if (/^#/ || $_ eq "");
        chomp($_);
        if (/^\[(.*)\]/) { $act_section = ($sections{$1} ? $1 : undef); }
        elsif (/SDBINFO\.PRT/) { $act_section = undef; }
        else { if ($act_section) { $sections{$act_section}->[2] .= "$_\n"; } }
    }
    close (CMD_OUT);
    
    # Compare the selected sections against latest QADB entries:
    foreach my $key (keys %sections) {
        $sections{$key}->[2] =~ s/'/''/g;
        $sections{$key}->[2] =~ s/\x00/ /g;
        ($ok, $href) = QAConnect::httpsql_request("QADB_HTTPSQL", "select OUTPUT from SERVER_INFOS where IDSERVERCONFIG=$sections{$key}->[0] " . ($version ? "AND VERSIONINFO='$version' " : "") . "ORDER BY ID DESC", 1);
        if  (!ref($href) || !$href->{'Rows'}->{'Row'}[0]->{'OUTPUT'}) {
            $changed_parts .= "$key (new)\n";
            $old_config .= "$sections{$key}->[1]:\n\n<not set>\n\n";
            $new_config .= "$sections{$key}->[1]:\n\n$sections{$key}->[2]\n";
            print "Section '$key' is new!\n";
        } elsif ($sections{$key}->[2] ne $href->{'Rows'}->{'Row'}[0]->{'OUTPUT'}) {
            $changed_parts .= "$key\n";
            $old_config .= "$sections{$key}->[1]:\n\n$href->{'Rows'}->{'Row'}[0]->{'OUTPUT'}\n";
            $new_config .= "$sections{$key}->[1]:\n\n$sections{$key}->[2]\n";
            print "Section '$key' has changed!\n";
        } else { next; }
        QAConnect::httpsql_request("QADB_HTTPSQL", "insert into SERVER_INFOS (IDSERVERCONFIG, TS, OUTPUT, VERSIONINFO) values ($sections{$key}->[0], $ts, '$sections{$key}->[2]', " . ($version ? "'$version'" : "' '") . ")", 1);
    }
    
    # Exit function unless something has changed:
    if ($changed_parts) { print "Some information has changed! Sending email...\n"; } 
    else { print "No changed parts. Everything is fine...\n"; return 1; }
    
    # Something has changed: Write Mail
    $old_config = "<none set>\n" unless ($old_config);
    my %mail = (
        'Subject' => ($version ? "$hostname ($version) - Server configuration has changed since last version" : "$hostname - Server configuration has changed"),
        'Message' => ($version ? "The following configuration parts have been changed since the last make of version $version:\n\n" : "The following configuration parts have been changed:\n\n") . "$changed_parts\n\n---- Complete configuration output of all changed parts ----\n\nOld config:\n\n$old_config\n\nNew config:\n\n$new_config\n\n",
        'To'      => 'falko.flessner@sap.com',
        #'Cc'      => 'frank.strassenburg@sap.com,robert.voelkel@sap.com',
        'Bcc'     => '',
        'From'    => 'automail@maxdb.sap.corp',
        'Smtp'    => 'mail.sap.corp'
    );
    
    if (sendmail(%mail)) { return 0; }
    else { print "ERROR sending mail: $Mail::Sendmail::error \n"; return -1; }
}
