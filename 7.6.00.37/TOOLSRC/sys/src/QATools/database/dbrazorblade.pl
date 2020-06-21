#!/usr/bin/perl
######################################################
# dbRAZORBLADE 
# clean up the QA-Part of the PTS-Database
# HISTORY:
# 2003-04-22 Version 0.1 by Flessner, Falko (d038447)
#


use DBI;
use Net::HTTP;
use Sys::Hostname;
use HTML::LinkExtor;
use Net::SMTP;
use URI;
use strict;

package d;


open (OB, ">/tmp/outbuf");

my %dbi_attr = ( PrintError => 1, RaiseError => 1);

my $dbh = DBI->connect('DBI:ODBC:10.21.81.184:QADB', 'TESTER', 'TESTER', \%dbi_attr) or die $DBI::errstr;
my $parser = HTML::LinkExtor->new(\&link_callback);
my $monthdelta = 2600000;
my $currtime   = time();
my @to_erase;
my $outtext = "";

&log_start();

&log_entry("main: Turning AutoCommit off");
$dbh->{AutoCommit} = 0;

&do_stmt("CREATE TABLE \"TEMP\".\"MAKEIDS\" (IDMAKE int primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"LCAMAKEIDS\" (IDLCAMAKE int primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"SESSIONIDS\" (IDSESSION int primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"JAVATESTIDS\" (DRIVERSTARTED timestamp primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"LCATESTS_FUNC_IDS\" (IDLCATESTS_FUNC int primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"LCATESTS_PERF_IDS\" (IDLCATESTS_PERF int primary key)");
&do_stmt("CREATE TABLE \"TEMP\".\"LCATESTS_COMMENTS\" (IDLCATESTS_COMMENTS int primary key)");
&do_stmt("INSERT INTO \"TEMP\".\"MAKEIDS\" SELECT idmake FROM makeweg");
&do_stmt("INSERT INTO  \"TEMP\".\"LCAMAKEIDS\" SELECT idlcamake FROM lcamakeweg ");

&do_stmt("INSERT INTO \"TEMP\".\"MAKEIDS\" SELECT DISTINCT idmake FROM makehist WHERE ts < " . &getisotime($currtime - $monthdelta) . " AND idqastatus = 1 AND idmake NOT IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\") ");
&do_stmt("INSERT INTO \"TEMP\".\"MAKEIDS\" SELECT id AS idmake FROM makes WHERE ts < " . &getisotime($currtime - $monthdelta) . " AND id NOT IN (SELECT idmake AS id FROM \"TEMP\".\"MAKEIDS\") AND idqastatus = 1");
&do_stmt("INSERT INTO \"TEMP\".\"JAVATESTIDS\" SELECT driverstarted FROM javatestinfo WHERE makekey IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\")");
&do_stmt("INSERT INTO \"TEMP\".\"JAVATESTIDS\" SELECT driverstarted FROM javatestweg");
#&do_stmt("INSERT INTO \"TEMP\".\"JAVATESTIDS\" SELECT driverstarted FROM javatestinfo WHERE ( driverfqhn LIKE 'p%' or driverfqhn LIKE 'P%' ) and ( driverfqhn not in (SELECT host FROM servers) ) AND timestamp( makedate( year(subdate (timestamp, 7 )), dayofyear(subdate (timestamp, 7)) ), '00:00:00' ) > driverstarted AND driverstarted NOT IN (SELECT driverstarted FROM  \"TEMP\".\"JAVATESTIDS\")");
&do_stmt("DELETE FROM javatestweg");
&do_stmt("DELETE FROM \"TEMP\".\"LCAMAKEIDS\" WHERE idlcamake IN (SELECT idlcamake FROM lca_deliver_main)");
&do_stmt("INSERT INTO \"TEMP\".\"LCATESTS_FUNC_IDS\" SELECT id AS IDLCATESTS_FUNC FROM lcatests_func WHERE idlcamake IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("INSERT INTO \"TEMP\".\"LCATESTS_PERF_IDS\" SELECT id AS IDLCATESTS_PERF FROM lcatests_perf WHERE idlcamake IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("INSERT INTO \"TEMP\".\"LCATESTS_COMMENTS\" SELECT DISTINCT idlcatest_comment FROM lcatests_comments_func WHERE idlcatest_func IN (SELECT idlcatests_func AS idlcatest_func FROM \"TEMP\".\"LCATESTS_FUNC_IDS\")");
&do_stmt("INSERT INTO \"TEMP\".\"LCATESTS_COMMENTS\" SELECT DISTINCT idlcatest_comment FROM lcatests_comments_perf WHERE idlcatest_perf IN (SELECT idlcatests_perf AS idlcatest_perf FROM \"TEMP\".\"LCATESTS_PERF_IDS\") AND idlcatest_comment NOT IN (SELECT IDLCATESTS_COMMENTS AS idlcatest_comment FROM \"TEMP\".\"LCATESTS_COMMENTS\")");


&do_stmt("INSERT INTO \"TEMP\".\"SESSIONIDS\" SELECT id AS idsession FROM install_sessions WHERE idmake in (SELECT idmake FROM  \"TEMP\".\"MAKEIDS\")");
&do_stmt("INSERT INTO webdav_delete (url) SELECT url FROM makeprot WHERE idmake IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\")");
&do_stmt("INSERT INTO webdav_delete (url) SELECT url FROM lcamakeprot WHERE idlcamake IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("INSERT INTO webdav_delete (url) SELECT url FROM sessionprot WHERE idsession IN (SELECT idsession FROM \"TEMP\".\"SESSIONIDS\")");

&log_entry("<B>COMMIT!</B>");
$dbh->commit();

my $stmt = "SELECT drivermailreport FROM javatestreport WHERE driverstarted IN (SELECT driverstarted FROM \"TEMP\".\"JAVATESTIDS\")";

my $sth = $dbh->prepare($stmt);
$sth->execute();

$outtext = "<TT>" . $stmt . "</TT><BR>After parsing, this returns the following URLs to DELETE<BR>\n";

#&log_entry($outtext);

while ( my @urlrow = $sth->fetchrow_array ) {
        $parser->parse($urlrow[0]);
}


&log_entry("<B>COMMIT!</B>");
$dbh->commit();


$sth = $dbh->prepare("INSERT INTO webdav_delete (url) VALUES (?)");

my $i = 0;
foreach my $url (@to_erase) {
        $i++;
        #&log_entry ("Adding URL Nr. $i :\n<A HREF=\"$url\"><TT>$url</TT></A><BR>");
        $sth->execute($url);
}
&do_stmt("update INSTALL_SESSIONS set idmake_cpc = NULL where idmake_cpc in (SELECT idmake FROM  \"TEMP\".\"MAKEIDS\")");
&do_stmt("update SESSIONHIST      set idmake_cpc = NULL where idmake_cpc in (SELECT idmake FROM  \"TEMP\".\"MAKEIDS\")");

&do_stmt("DELETE FROM lcamakelog         WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcamakeprot        WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcatests           WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcamakehist        WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcamakes_comment   WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcatests_comments_func WHERE idlcatest_comment IN (SELECT idlcatests_comments AS idlcatest_comment FROM \"TEMP\".\"LCATESTS_COMMENTS\")");
&do_stmt("DELETE FROM lcatests_comments_perf WHERE idlcatest_comment IN (SELECT idlcatests_comments AS idlcatest_comment FROM \"TEMP\".\"LCATESTS_COMMENTS\")");
&do_stmt("DELETE FROM lcatests_comments  WHERE id            IN (SELECT idlcatests_comments AS id FROM \"TEMP\".\"LCATESTS_COMMENTS\")");
&do_stmt("DELETE FROM lcatests_perf_comment WHERE idlcatest_perf IN (SELECT idlcatests_perf AS idlcatest_perf FROM\"TEMP\".\"LCATESTS_PERF_IDS\")");
&do_stmt("DELETE FROM lcatests_func_comment WHERE idlcatest_func IN (SELECT idlcatests_func AS idlcatest_func FROM\"TEMP\".\"LCATESTS_FUNC_IDS\")");
&do_stmt("DELETE FROM lcatests_perf    WHERE id            IN (SELECT idlcatests_perf AS id FROM\"TEMP\".\"LCATESTS_PERF_IDS\")");
&do_stmt("DELETE FROM lcatests_func    WHERE id            IN (SELECT idlcatests_func AS id FROM\"TEMP\".\"LCATESTS_FUNC_IDS\")");
&do_stmt("DELETE FROM lcatests_func    WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcatests_perf    WHERE idlcamake     IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");
&do_stmt("DELETE FROM lcamakes         WHERE id            IN (SELECT idlcamake FROM \"TEMP\".\"LCAMAKEIDS\")");

&do_stmt("DELETE FROM makelog          WHERE idmake        IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\")");
&do_stmt("DELETE FROM makeprot         WHERE idmake        IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\")");
&do_stmt("DELETE FROM makehist         WHERE idmake        IN (SELECT idmake FROM \"TEMP\".\"MAKEIDS\")");

&do_stmt("DELETE FROM sessionlog       WHERE idsession     IN (SELECT idsession  FROM \"TEMP\".\"SESSIONIDS\")");
&do_stmt("DELETE FROM sessionprot      WHERE idsession     IN (SELECT idsession  FROM \"TEMP\".\"SESSIONIDS\")");
&do_stmt("DELETE FROM sessionhist      WHERE idsession     IN (SELECT idsession  FROM \"TEMP\".\"SESSIONIDS\")");
&do_stmt("DELETE FROM install_sessions WHERE id            IN (SELECT idsession  FROM \"TEMP\".\"SESSIONIDS\")");

&do_stmt("DELETE FROM javatestreport   WHERE driverstarted IN (SELECT driverstarted FROM \"TEMP\".\"JAVATESTIDS\")");
&do_stmt("DELETE FROM javatestinfo     WHERE driverstarted IN (SELECT driverstarted FROM \"TEMP\".\"JAVATESTIDS\")");
&do_stmt("DELETE FROM \"TEMP\".\"MAKEIDS\" WHERE idmake  IN (SELECT DISTINCT idmake FROM lcamakes)");
&do_stmt("DELETE FROM \"TEMP\".\"MAKEIDS\" WHERE idmake  IN (SELECT DISTINCT idmake FROM lcatests_func)");
&do_stmt("DELETE FROM makes WHERE id IN (SELECT idmake AS id FROM \"TEMP\".\"MAKEIDS\")");

&log_entry("<B>COMMIT!</B>");
$dbh->commit();
$dbh->{AutoCommit} = 1;

$sth = $dbh->prepare("SELECT id, url FROM webdav_delete");
$sth->execute();

my $del_ref = $sth->fetchall_arrayref();

my $del_sth = $dbh->prepare("DELETE FROM webdav_delete WHERE id = ?");
foreach my $row (@$del_ref) {
        my ($id, $url) = @$row;
        &webdav_delete($url);
        $del_sth->execute($id);
}

&log_end();

close (OB);

open (IB, "/tmp/outbuf");

my $smtp = Net::SMTP->new("mail.sap.corp");
$smtp->mail("falko.flessner\@sap.com");
$smtp->to("falko.flessner\@sap.com", "dennis.adler\@sap.com", "mechthild.bore-wuesthof\@sap.com");
$smtp->data();
$smtp->datasend("To: falko.flessner\@sap.com; dennis.adler\@sap.com; mechthild.bore-wuesthof\@sap.com\n");
$smtp->datasend("Subject: dbRazorblade\n");
$smtp->datasend("Content-type: text/html\n");
$smtp->datasend("\n");
while (<IB>) {
        $smtp->datasend($_);
}

$smtp->dataend();
$smtp->quit();

close IB;

sub link_callback {
        my ($tag, %attr) = @_;
        if ($tag ne "a") {
                &log_entry("I have found a unwanted tag: $tag");
                return;
        }

        foreach my $val (values(%attr)) {
                push(@to_erase, $val);
        }
}


sub do_stmt {
        my $statement = shift;
        print $statement . "\n";
        my $rv = $dbh->do($statement);

        if ($rv) {
                &log_entry("<TT>$statement </TT><BR>$rv rows affected");
        } else {
                &log_entry("<TT>$statement </TT><BR>ERROR: $dbh->errstr");
                &log_entry("<H1>Rolling back and exiting!!</H1>");
                $dbh->rollback();
                &log_end();
                exit(1);
        }
        print $statement . "\n";
}

sub webdav_delete {
        my $url_string = shift;
        my %head_content;
        my @tlist = localtime(time());
        $head_content{'User-Agent'} = "LCqaCLI/1.0";

        my $url = URI->new($url_string);

        my $dav_req = Net::HTTP->new('Host' => $url->host(), 'PeerPort' => $url->port());
        $dav_req->write_request('DELETE', $url->path());
        my ($code, $mess, %headers) = $dav_req->read_response_headers();

        &log_entry(" webdav_delete $url_string : $code - $mess\n");
}

sub log_start {
        my @tlist = localtime(time());
        print OB "<HTML><HEAD><TITLE>dbrazorblade - log</TITLE></HEAD><BODY BGCOLOR=#FFFFFF><H1><FONT FACE=\"Arial, Helvetica\">dbRAZORBLADE</H1><H3><FONT FACE=\"Arial, Helvetica\">Log vom ";
                printf OB ("%04d-%02d-%02d %02d:%02d:%02d",
                $tlist[5] + 1900,
                $tlist[4] + 1,
                $tlist[3],
                $tlist[2],
                $tlist[1],
                $tlist[0]);

        print OB "</H3><TABLE BORDER=1>\n";

}

sub log_end {
        print OB "\n</TABLE><HR></BODY></HTML>\n";
}

sub log_entry {
        my $entry = shift;
        my @tlist = localtime(time());

        print OB "<TR><TD><FONT FACE=\"Arial, Helvetica\">";
        printf OB ("%04d-%02d-%02d %02d:%02d:%02d",
                $tlist[5] + 1900,
                $tlist[4] + 1,
                $tlist[3],
                $tlist[2],
                $tlist[1],
                $tlist[0]);
        print OB "</TD><TD><FONT FACE=\"Arial, Helvetica\">$entry </TD></TR>\n";
}

sub getisotime {
        my $ts = shift;
        my @tlist = localtime($ts);

        return sprintf("'%04d-%02d-%02d %02d:%02d:%02d.0000'",
                $tlist[5] + 1900,
                $tlist[4] + 1,
                $tlist[3],
                $tlist[2],
                $tlist[1],
                $tlist[0]);
}

