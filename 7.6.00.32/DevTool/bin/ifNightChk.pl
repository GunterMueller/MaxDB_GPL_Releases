use Net::FTP;
use File::Copy;
use Getopt::Long;

sub printWarn;
sub printError;
sub printDiff;
sub printInfo;

$TempFile="AllInterfaceTestProt.tmp.html";
$TempBuffer="AllIntTestProtTempBuf.out";
$IntraOutFile="\\\\P26326\\inetpub\\wwwroot\\Interfaces\\InterfaceNighttestSummary.html";
$OutFile="InterfaceNighttestSummary.html";
$ProtPath="/test/pc";
$ProtFile="pctest.prot";
$currentDiffPath;
$currIndent = 0;
$verbose = 0;

sub getCurrentIndent() {
  return ("  " x $currIndent);
}

sub getPrefixTestName {
  @_[0]=~/^(.*)\/(\w+)/i ;
  return $1;
}

sub stripTestName (){
    if (@_[0]=~/^(odbc)\/(\w+)/i) {
      return "Lo.$2";
    }
    elsif (@_[0]=~/^(cpc)\/(\w+)/i){
      return "c.$2";
    }
    return (@_[0]);
}

sub getDiffDir() {
  my $sep = chop ($currentDiffPath);
  $currentDiffPath .= $sep;
  return ($currentDiffPath."dif".$sep.&stripTestName(@_[0]).".".@_[1]);
}

sub getOldDir() {
  my $sep = chop ($currentDiffPath);
  $currentDiffPath .= $sep;
  return ($currentDiffPath."old".$sep.&stripTestName(@_[0]).".".@_[1]);
}

sub getWrkDir() {
  my $sep = chop ($currentDiffPath);
  $currentDiffPath .= $sep;
  return ($currentDiffPath."wrk".$sep.&stripTestName(@_[0]).".".@_[1]);
}

sub getProtocol (){
  my ($host,$release) = @_;
  if ($host=~/^(p|itanium)/i){
      # platform is windows
      $currentDiffPath = "//$host/$release$ProtPath/";
      my $protocol = $currentDiffPath."wrk/$ProtFile";
      $currentDiffPath =~ s/\//\\/g;
      print "$protocol\n" if (defined $opt_v);
      my $erg = mycpy($protocol,$TempBuffer);
      if ($erg != 0){
        return $!;
      }
      else {
        return "okay";
      }
  }
  else {
    my $tempcurrentDiffPath = "$release$ProtPath/";
    my $path = $tempcurrentDiffPath."wrk";
    $currentDiffPath="ftp://remuser:Remuser@".$host.$tempcurrentDiffPath;
    return getProtocolfromFTP ($host, $path, $ProtFile);
  }
}

sub getProtocolfromFTP ()
{
    my ($host,$path,$protocol) = @_;
    print "$host: $path/$protocol\n" if (defined $opt_v);
    my $ftpuser = "remuser";
    my $ftppwd  = "Remuser";


    my $ftp = Net::FTP->new($host, Debug =>0);
    if (!$ftp)
    {
      return ("FTP connection to $host failed");
    }

    if (!$ftp->login($ftpuser,$ftppwd))
    {
      $ftp->quit;
      return ("FTP login with $ftpuser failed");
    }

    if (!$ftp->cwd($path)){
      $ftp->quit;
      return ("Cannot access directory $path");
    }

    if (!$ftp->ascii())
    {
      $ftp->quit;
      return ("FTP ascii setup failed.");
    }

    if (!$ftp->get($protocol, $TempBuffer)){
        $ftp->quit;
        return ("Cannot get $protocol");
    }
    $ftp->quit;
    return ("okay");
}

sub analyzeTestcase() {
  my %myTest =%{@_[0]};

  #testcasefilter
  if (defined $opt_only
      && $opt_only ne getPrefixTestName($myTest{"testname"})) {
     return;
  }

  #check sequence
  if (! $myTest{"compile"}){
    printWarn ($myTest{"testname"}." testcase wasn't compiled");
  }
  elsif (! $myTest{"build"}){
    printWarn ($myTest{"testname"}." testcase wasn't maked");
  }
  elsif (! $myTest{"execute"}){
    printWarn ($myTest{"testname"}." testcase wasn't executed");
  }
  elsif (! $myTest{"end"}){
    printWarn ($myTest{"testname"}." testcase wasn't finished - maybe canceled");
  }

  #check errors
  unless (defined $opt_nodiff){
    if ( $myTest{"diff_list"}){
      printDiff (" differences in listing ",$myTest{"testname"},"l");
    }
    if ( $myTest{"diff_out"}){
      printDiff (" differences in output ",$myTest{"testname"},"o");
    }
    if ( $myTest{"diff_trc"}){
      printDiff (" differences in trace file ",$myTest{"testname"},"t");
    }
  }
  if ( $myTest{"error_run"}){
    printError ($myTest{"testname"}." test unsuccessfully");
  }
  if ( $myTest{"eror_core"}){
    printError ($myTest{"testname"}." core file");
  }
  if ( $myTest{"error_env"}){
    $myTest{"error_env"}=~/^ERROR: Environment variable\s+(\w+)\s+.*/i;
    printError ($myTest{"testname"}." environment variable $1 not set");
  }
  if ( $myTest{"error_precom"}){
    printError ($myTest{"testname"}." error at precompilation");
  }
  if ( $myTest{"error_link"}){
    printError ($myTest{"testname"}." error at linking");
  }
  if ( $myTest{"error_timeout"}){
    printError ($myTest{"testname"}." timeout occured");
  }
  if ( $myTest{"error_semareset"}){
    printError ($myTest{"testname"}." semaphore can't be removed");
  }
  if ( $myTest{"error_semaset"}){
    printError ($myTest{"testname"}." semaphore can't be set");
  }
}

sub analyzeProtocol() {
  my $t_start_cycle = "";
  my $t_start;
  my $t_end;
  my %testcase=();
  open(FILE_IN, "<$TempBuffer")  || die "Can't open $TempBuffer (input): $!";
  while(<FILE_IN>) {
    $_ =~/(\d\d:\d\d:\d\d \d{4}.\d\d.\d\d)\s+(.*)/i;
    $t_end = $1;
    my $t_CurLin = $2;
#    print "t_end   : $t_end\n";
#    print "t_CurLin: $t_CurLin\n";
    if (! defined $t_start){
      $t_start = $t_end;
    }
    if ($t_CurLin=~/^Start pctest.*cpc\/initcpc/){
      printInfo ("Start Time $t_end\n");
    }
    if ($t_CurLin=~/.*cleanup.*/i){
      #ignore
      next;
    }
    if ($t_CurLin=~/^End.*(cpc|odbc)\/(\w+).*/i){
      $testcase{"testname"}="$1/$2";
      if ($testcase{"start"}){
        $testcase{"end"}=$_;
        &analyzeTestcase(\%testcase);
      }
      else {
        my $t_nam = $_;
        $t_nam =~/(cpc|odbc)\/(\w*).*/i;
        printError ("End without start $2");
      }
      %testcase=();
      next;
    }
    elsif ($t_CurLin=~/^Start pctest.*(cpc|odbc)\/(\w+).*/i){
      if ($testcase{"start"}){
         &analyzeTestcase(\%testcase);
      }
      %testcase=();
      $testcase{"testname"}="$1/$2";
      $testcase{"start"}=$_;
      next;
    }
    elsif ($t_CurLin=~/^Compile.*/i){
      $testcase{"compile"}=$_;
    }
    elsif ($t_CurLin=~/^Build.*/i){
      $testcase{"build"}=$_;
    }
    elsif ($t_CurLin=~/^Execute.*/i){
      $testcase{"execute"}=$_;
    }
    elsif ($t_CurLin=~/^=> DIFF <=.*\.l$/i){
      $testcase{"diff_list"}=$_;
    }
    elsif ($t_CurLin=~/^=> DIFF <=.*\.o$/i){
      $testcase{"diff_out"}=$_;
    }
    elsif ($t_CurLin=~/^=> DIFF <=.*\.t$/i){
      $testcase{"diff_trc"}=$_;
    }
    elsif ($t_CurLin=~/^===> ERROR DURING RUN OF.*/i){
      $testcase{"error_run"}=$_;
    }
    elsif ($t_CurLin=~/^===> ERROR WITH A CORE-FILE FROM.*/i){
      $testcase{"eror_core"}=$_;
    }
    #===> ERROR: Environment variable
    elsif ($t_CurLin=~/^===> ERROR: Environment variable.*/i){
      $testcase{"error_env"}=$_;
    }
    #===> ERROR AT (PRE)/COMPILATION
    elsif ($t_CurLin=~/^===> ERROR AT (PRE)\/COMPILATION.*/i){
      $testcase{"error_precom"}=$_;
    }
    #===> ERROR AT LINKING THE FILE(S) WITH
    elsif ($t_CurLin=~/^===> ERROR AT LINKING THE FILE(S) WITH.*/i){
      $testcase{"error_link"}=$_;
    }
    #===> STOPPING EXECUTION
    elsif ($t_CurLin=~/^===> STOPPING EXECUTION.*/i){
      $testcase{"error_timeout"}=$_;
    }
    #===> CAN'T RESET SEMAFORE FILE
    elsif ($t_CurLin=~/^===> CAN\'T RESET SEMAFORE FILE.*/i){
      $testcase{"error_semareset"}=$_;
    }
    #===> CAN'T SET SEMAFORE FILE
    elsif ($t_CurLin=~/^===> CAN\'T SET SEMAFORE FILE.*/i){
      $testcase{"error_semaset"}=$_;
    }
  }
  close(FILE_IN);
}


sub printHeader {
  open(FILE_OUT, ">$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "<html>\n";
  print FILE_OUT "<head>\n";
  print FILE_OUT "<title>Interface Nighttest Summary</title>\n";
  print FILE_OUT "<link rel=stylesheet type=\"text/css\" href=\"http://pwww:1111/sapdb_bis.css\">\n";
  print FILE_OUT "</head>\n";
  print FILE_OUT "<body>\n";
  print FILE_OUT "<P class=\"head\">Interface Nighttest Summary from ".date()."</p>\n";
  close(FILE_OUT);
}
sub printFooter {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "<p><small>Last Update: ".timestamp()." by <a href=\"mailto:Marco.Paskamp\@SAP.COM\">MarcoP</a></small>\n";
  print FILE_OUT "</body>\n";
  print FILE_OUT "</html>\n";
  close(FILE_OUT);
}

sub printTestName {
  my ($host, $rel) = @_;
  my $sep = chop ($currentDiffPath);
  $currentDiffPath .= $sep;
  my $pctest = $currentDiffPath."wrk".$sep."pctest.prot.html";
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "<P class=\"subhead\">Release $rel"."DEV Platform $host ";
  print FILE_OUT "<A href=\"".$pctest."\">pctest.prot</A></p>\n";
  close(FILE_OUT);
  $currIndent = 2;
}

sub printTabHead {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "<table>\n";
  close(FILE_OUT);
}

sub printTabFoot {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT "</table><p></p>\n";
  close(FILE_OUT);
}

sub printDiff {
  my ($text,$tname,$suffix)=@_;
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT getCurrentIndent()."<tr><td><font class=\"bg-green\">DIFFERENCE: $tname $text</font></td>";
  print FILE_OUT "<td><A href=\"".&getDiffDir($tname,$suffix)."\">diff</A></td>";
  print FILE_OUT "<td><A href=\"".&getWrkDir($tname,$suffix)."\">new</A></td>";
  print FILE_OUT "<td><A href=\"".&getOldDir($tname,$suffix)."\">old</A></td>";
  print FILE_OUT "</tr>\n";
  close(FILE_OUT);
}

sub printInfo {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT getCurrentIndent()."<tr collspann=4><td>INFO:       ".@_[0]."</td></tr>\n";
  close(FILE_OUT);
}

sub printWarn {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT getCurrentIndent()."<tr collspann=4><td><font class=\"green\">WARNING:    ".@_[0]."</font></td></tr>\n";
  close(FILE_OUT);
}

sub printError {
  open(FILE_OUT, ">>$TempFile") || die "Can't open TempFile (output): $!";
  print FILE_OUT getCurrentIndent()."<tr collspann=4><td><font class=\"bg-red\">ERROR:      ".@_[0]."</font></td></tr>\n";
  close(FILE_OUT);
}

sub timestamp
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
    return sprintf("%02d.%02d.%04d", $localtime[$$Day_index], $localtime[$$Month_index], $localtime[$$Year_index]);
}

sub mycpy{

	my $src = $_[0];
	my $dest = $_[1];
	-f $src or return ("cp: file \"$src\" not found\n");
	$dest =~ /^[\ ]*$/ and return("cp: there are no destination\n");
	if($^O =~ /.*win32.*/i){
		$src=~s/\//\\/g;
		$dest=~s/\//\\/g;
		#print"copy $src $dest\n";
		$rc=`copy $src $dest`;
	}
	else{
		$rc=`cp -p $src $dest`;
	}
	if ($rc != 0){
	  return "Error copying file";
	}
	else {
	  return "okay";
        }
}
sub USAGE {
	print STDOUT ("usage: ifNightChk: [-h] [other options]\n\n");
	print STDOUT ("   -h               Print this help message.\n");
	print STDOUT ("   -v               Verbose\n");
	print STDOUT ("   -o <filename>    path and filename for HTML output.\n");
	print STDOUT ("   -web             update intranet page with HTML output.\n");
	print STDOUT ("   -nodiff          Suppress check for differences.\n");
	print STDOUT ("   -only <cpc|odbc> Limits checks for cpc or odbc tests only.\n");
	print STDOUT ("   -rel  <release>  Limits checks to the given release.\n");
	#print possible releases;
	print STDOUT ("   these releases are present:\n");
	my @tests = grep { /^\s*[^#]\S+.*$/ } <DATA>;
        my %release;
        foreach my $test (@tests) {
          chomp $test;
          my ($host,$path,$rel) = split(/\,/,$test,3);
          $release{$rel}=$rel;
        }
        @outval = sort keys %release;
        foreach my $vers (@outval)
        {
          print STDOUT ("         $vers\n");
        }
	exit 1;
}
#main

GetOptions ( 'nodiff', 'h', 'v', 'only:s', 'rel:s' ,'o:s', 'web') or USAGE();
USAGE if (defined $opt_h);
if (defined $opt_o){
  $OutFile=$opt_o;
  print "Output: $OutFile\n" if ((defined $opt_v));
}
elsif (defined $opt_web){
  $OutFile=$IntraOutFile;
  print "Output: $OutFile\n" if ((defined $opt_v));
}
else{
  print "WARNING: no output file given using default ./$OutFile\n" if ((defined $opt_v));
}

printHeader();
my @tests = grep { /^\s*[^#]\S+.*$/ } <DATA>;
if (defined $opt_rel){
  $relfilter = qr/^$opt_rel$/i;
}
foreach my $test (@tests) {
    chomp $test;
    my ($host,$path,$rel) = split(/\,/,$test,3);
#   print ">$relfilter<\n";
   if ( (defined $relfilter
	&& $rel=~/$relfilter/)
	|| ! defined $relfilter){
    }
    else {
#      print "next\n";
      next;
    }

    $erg = &getProtocol($host,$path);
    printTestName($host,$rel);
    if($erg ne "okay"){
      printError ($erg);
      next;
    }
    printTabHead();
    analyzeProtocol();
    printTabFoot();
}
printFooter();
unlink($TempBuffer);
mycpy ($TempFile,$OutFile);
unlink($TempFile);

__DATA__
#host,path,release
#sun
us8802,/rel/72_32,72
us8802,/rel/72_64,72
us8802,/rel/73_32,73
us8802,/rel/73_64,73
us8802,/rel/731_32,731
us8802,/rel/731_64,731
us0062,/SAP_DB/7403DEV,743
us0062,/SAP_DB/7404DEV,74
#dec
ds0124,/rel/72,72
ds0124,/rel/73,73
ds0124,/rel/731,731
ds0116,/SAP_DB/7403DEV,743
ds0116,/SAP_DB/7404DEV,74
#aix
is8802,/rel/72_32,72
is8802,/rel/72_64,72
is8802,/rel/73_32,73
is8802,/rel/73_64,73
is8802,/rel/731_32,731
is8802,/rel/731_64,731
is0025,/SAP_DB/7403DEV,743
is0025,/SAP_DB/7404DEV,74
#hp
hs8802,/rel/72_32,72
hs8802,/rel/72_64,72
hs8802,/rel/73_32,73
hs8802,/rel/73_64,73
hs8802,/rel/731_32,731
hs8802,/rel/731_64,731
hs0102,/SAP_DB/7403DEV,743
hs0102,/SAP_DB/7404DEV,74
#linux
ls8800,/rel/72,72
ls8800,/rel/73,73
ls8800,/rel/731,731
ls8800,/rel/743,743
ls8800,/rel/74,74
#sni
ss8802,/rel/72_32,72
ss8802,/rel/72_64,72
ss8802,/rel/73_32,73
ss8802,/rel/73_64,73
ss8802,/rel/731_32,731
ss8802,/rel/731_64,731
#windows
p30883,v72/develop,72
p64529,v73/c_00/develop,73
p26327,v73/develop,731
#p64531,v74/c_03/develop,743
#p64532,v74/develop,74
pwdf0238,D/SAP_DB/7403DEV,743
pwdf0238,D/SAP_DB/7404DEV,74
#ia64
#Pintel64_04,7.4.2,742
#Pintel64_03,SAPDevelop\V7403,743
#Pintel64_03,SAPDevelop\V74,74
itanium21,D/SAP_DB/7403DEV,743
itanium21,D/SAP_DB/7404DEV,74
