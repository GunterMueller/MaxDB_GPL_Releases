use File::Copy;

$TempFile="JdbcVersionHistory.html.tmp";
$OutFile="\\\\P26354\\inetpub\\wwwroot\\Interfaces\\JDBC\\JdbcVersionHistory.html";
$lastchangeNumber = 0;

sub date;


sub getLastChangeListfromHistory{
  open(FILE_IN, "<$OutFile")  || die "Can't open $OutFile (input): $!";
  open(FILE_OUT, ">$TempFile") || die "Can't open $TempFile (output): $!";
  while(<FILE_IN>) {
    if (/<\/table>/i){
#      print "last changelist: ".$lastchangeNumber."\n";
      last;
    }
    if(/<td.*>(\d+)<\/TD><!-- changenr -->/i){
      $lastchangeNumber=$1;
    }
    print FILE_OUT $_;

  }
  close(FILE_OUT); close(FILE_IN);
}

sub getHistory{
  $cmd="p4 changes -s submitted ".$ENV{SRC}."/jv/sapdbc.jar";
  $allchanges= qx{$cmd};
  @changelist=split /\n/, $allchanges;

  # open OUTPUT, "versions.csv";
  open(FILE_OUT, ">>$TempFile") || die "Can't open $TempFile (output): $!";
  for ($i=$#changelist; $i>=0; --$i) {
    $current=$changelist[$i];
    $current=~/Change (.+) on (.+) by/;
    $change=$1;
    $date=$2;
    if ($change > $lastchangeNumber){
      #get PTS-link
      $cmd =  "p4 describe -s ".$change;
      $output = qx{$cmd} . "\n";
      #        JDBC 3.0 74.04 PTS: http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1118478
      $pts= "unknown";
      $ptsnr = "unknown";
      if ($output=~/(http.+pts.+?)(\d+)\s+/) {
        $pts=$1.$2;
        $ptsnr=$2;
      }
      #retrieve jdbc version
      $cmd =  "p4 sync ".$ENV{SRC}."/jv/sapdbc.jar@" . $change;
      qx{$cmd} . "\n";
      $cmdjava = "java -jar ".$ENV{SRC}."/jv/sapdbc.jar -V";
      $output = qx{$cmdjava};
      chomp $output;
      $output =~/^package(.+),(.+),(.+),\s*\"*(.+)\"*/;
      $vers =$4;

      print FILE_OUT "<TR>\n  <td class=\"solid\">". (1 + $#changelist - $i) . "</TD>\n";
      print FILE_OUT "  <td class=\"solid\">" . $change . "</TD><!-- changenr -->\n";
      print FILE_OUT "  <td class=\"solid\">" . $date   . "</TD>\n";
      print FILE_OUT "  <td class=\"solid\">" . $vers   . "</TD>\n";
      if ($pts eq "unknown"){
        print FILE_OUT "  <td class=\"solid\">unknown</TD></TR>\n";
      }
      else {
        print FILE_OUT "  <td class=\"solid\"><a href=\"".$pts."\" target=\"_blank\" >PTS " . $ptsnr    . "</A></TD></TR>\n";
      }
    #  package com.sap.dbtech.jdbc, SAP DB JDBC Driver, SAP AG, 7.3.0    Build 025-000-085-762
      print 1 + $#changelist - $i . ";" . $change . ";".  $date . ";" .$vers . ";" .$ptsnr . "\n";
    }
  }
  close(FILE_OUT);

}

sub printHeader {
  open(FILE_OUT, ">$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "<html>\n";
  print FILE_OUT "<head>\n";
  print FILE_OUT "<title>JDBC Driver Version History</title>\n";
  print FILE_OUT "<link rel=stylesheet type=\"text/css\" href=\"http://pwww:1111/sapdb_bis.css\">\n";
  print FILE_OUT "</head>\n";
  print FILE_OUT "<body>\n";
  print FILE_OUT "<h3>Version history of the JDBC-driver</h3>\n";
  print FILE_OUT "<table><tr class=\"header\"><td>Nr.</td><td>Changelist</td><td>Date</td><td>JDBC Version</td><td>Web-PTS</td></tr>\n";
  close(FILE_OUT);
}
sub printFooter {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "</table>\n";
  print FILE_OUT "<p><small>Last Update: ".date()." by <a href=\"mailto:Marco.Paskamp\@SAP.COM\">MarcoP</a></small>\n";
  print FILE_OUT "</body>\n";
  print FILE_OUT "</html>\n";
  close(FILE_OUT);
}

sub date
{
    my $Year_index = \5;	#constant
    my $Month_index = \4;	#constant
    my $Day_index = \3;	#constant
    my $Hour_index = \2;	#constant
    my $Min_index = \1;	#constant
    my $Sec_index = \0;	#constant
    my @localtime = localtime();
    $localtime[$$Month_index]++;
    $localtime[$$Year_index]+=1900;
    return sprintf("%02d.%02d.%04d %02d:%02d:%02d", $localtime[$$Day_index], $localtime[$$Month_index], $localtime[$$Year_index], $localtime[$$Hour_index], $localtime[$$Min_index], $localtime[$$Sec_index]);
}

#main
print "Generate Version History File of the JDBC-Driver\n";
getLastChangeListfromHistory;
if ($lastchangeNumber==0){
  printHeader;
}
getHistory;
printFooter;
# copy into intranet
copy($TempFile, $OutFile);
unlink $TempFile;
exit;
