use Net::FTP;
use File::Copy;

my @hosts = grep { /^\s*[^#]\S+.*$/ } <DATA>; 
    
foreach my $host (@hosts) {
    
    chomp $host;
    
    if ( $host =~ /^p.*/i ) {
    } else {
        getProtocolWithFTP($host);
    }
}

system ("pparse.exe");


#######################################################################


sub getProtocolWithFTP ()
{
    my ($host) = @_;
    
    $hostkind = 'SUN'   if ( $host =~ /^u.*/i ); 
    $hostkind = 'SNI'   if ( $host =~ /^s.*/i ); 
    $hostkind = 'AIX'   if ( $host =~ /^i.*/i ); 
    $hostkind = 'DEC'   if ( $host =~ /^d.*/i ); 
    $hostkind = 'HP'    if ( $host =~ /^h.*/i ); 
    $hostkind = 'LINUX' if ( $host =~ /^l.*/i ); 
    
    my $path    = "/rel/nachtlauf";
    my $ftpuser = "remuser"; 
    my $ftppwd  = "Remuser"; 
    
    
    my $ftp = Net::FTP->new($host, Debug =>0);
    if (!$ftp)
    {
      print"FTP connection to $host failed";
      exit (-1);
    }
    
    if (!$ftp->login($ftpuser,$ftppwd))
    {
      print "FTP login with $ftpuser failed";
      $ftp->quit;                       
      exit (-1);
    }
    
    if (!$ftp->cwd($path)){
      print "Cannot access directory $path";
      $ftp->quit;                      
      exit (-1);
    } 
   
    if (!$ftp->ascii())  
    {
      print"FTP ascii setup failed.";
      $ftp->quit;                     
      exit (-1);
    }
    
    my @remotedir = $ftp->ls();
    
    my @searchdate = localtime (time - 1 * 24 * 3600);
    my $prefix = sprintf ("%02d%s%04d", $searchdate[3], (Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Oct,Nov,Dec)[$searchdate[4]],  $searchdate[5] + 1900); 
    
    if ($searchdate[6] == 0) { # es ist Montag
        $prefix .= '|';
        @searchdate = localtime (time - 2 * 24 * 3600);
        $prefix .= sprintf ("%02d%s%04d", $searchdate[3], (Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Oct,Nov,Dec)[$searchdate[4]],  $searchdate[5] + 1900); 
        $prefix .= '|';
        @searchdate = localtime (time - 3 * 24 * 3600);
        $prefix .= sprintf ("%02d%s%04d", $searchdate[3], (Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Oct,Nov,Dec)[$searchdate[4]],  $searchdate[5] + 1900); 
    } 

    my @files = grep { /^$prefix\S*$/ } @remotedir;
   
    foreach my $file (@files) {

        my ($release, $bit, $mode, $target);
        
        if ( $file =~ /^[^_]+_([^_]+)_(\d+)Bit_([^_]+)$/ ) {
            $release = $1;
            $bit = $2;
            $mode = $3;
            
            if ( $bit == 3264 ) {
                $target = join "_" , ($hostkind,$release,$mode);
            } else {
                $target = join "_" , ($hostkind,$release,$bit,$mode);
            }

            
            print "copy $file -> $target\n";
            if (!$ftp->get($file, $target)){
                print "Cannot copy $file to $target";
                $ftp->quit;                    
                exit (-1);
                }

            open (IN, "<$target") or die "can't open $target\n";
            my @target = <IN>;
            close (IN);
            
            my (@targetold);
            if ( -f "$target.old" ) {
                open (IN, "<$target.old") or die "can't open $target.old\n";
                @targetold = <IN>;
                close (IN);
            }
            
            if ( @target == @targetold ) {
                print "$target already checked\n";
                unlink ($target);
                last;
            }
            
            unlink ("$target.old");
            copy ($target, "$target.old");

        } 

    }
   
        
    
    $ftp->quit;
}

__DATA__
#host,release
us8803
us8802
hs8802
ss8802
ds0124
is8801
is8802
ls8800
ls8801
