#!/usr/bin/perl -w

package PerfSet;

use Env;

#unshift @INC, "$ENV{TOOL}/bin";
require PerforceEnv;
import PerforceEnv;
require igme;
import igme;

#############################################################

sub SetPerforceUser
{
    my $p4user = $ENV{P4USER};
 
    unless( $p4user )
    {
        if (exists $ENV{'CURRENTUSER'} )
        {
            $p4user= $ENV{'CURRENTUSER'};
        }
        else
        {
            $p4user= getlogin or die "Can't get the user name! \n$!\n";
        }
    
        $p4user =~ tr/A-Z/a-z/;
        print "Setting Perforce user to \"$p4user\"...\n"; 
        my $p4in = "p4 user -i";
        open USER_IN, "| $p4in" or die "Can't open \"p4 user -i\"!\n$!\n";
        print USER_IN "User: $p4user\n";
        print USER_IN "Email: $1\n" if qx($ENV{TOOL}/bin/tel.pl $p4user) =~ /,\s(\S+@\S+),/;
        print USER_IN "FullName: $1\n" if qx($ENV{TOOL}/bin/tel.pl $p4user) =~ /^\"([\w|\s]+)\",/;
        close USER_IN or die "Can't close p4 user -i! \n$!\n";
        
    }
    return $p4user;
}
 
#############################################################

sub SetClientView
{
    my ($p4user, $p4client) = @_;

	@p4clients = qx(p4 clients);	

	$p4client= $ENV{P4CLIENT} unless ($p4client);
	
	unless ($p4client)
	{
		if(exists $ENV{'COMPUTERNAME'})
		{	$hostname= $ENV{'COMPUTERNAME'}; }
		elsif(exists $ENV{'HOSTNAME'})
		{ 	$hostname= $ENV{'HOSTNAME'}; }
		else
		{	$hostname= qx(uname -n) or die "Can't get the host name! \n$!\n";	}	
		
		$p4client = $hostname
	}
	
	foreach ( @p4clients)
	{
		if ($p4client and /^Client\s$p4client\s.*/)
		{
			$serverclient = $p4client;
			last;
		}
	}

    if ($serverclient)
    {
        print "\nThere is already a client [$serverclient] installed on your computer.\nDo you want to use this one (y/n)? ";
        if (<STDIN>=~ /^[1jy]/i)
        {
            $newclient = $serverclient;
        }
    }
    
    unless ($newclient)
    {
        print "\nPlease enter a name for the Perforce client you want to use.\n";
        $newclient = RequestInput("Client name [ENTER = $p4client] : ", $p4client, '^\w+$', "Please use only [a-z,A-Z,0-9,_]!\n");
        
        if($newclient eq $serverclient)
        {
            print "\nDo you really want to overwrite the existing client [$newclient] (y/n)? ";
            if (<STDIN>=~ /^[1jy]/i)
            {   
                PerforceEnv::make_perforce_client($p4user,$newclient);
            }
            else
            {
                print "Client information not changed.\n";
            }
        }
        else
        {
            PerforceEnv::make_perforce_client($p4user,$newclient);
        }
    }
    
    return $newclient;
}

#############################################################

sub SetViewReleaseEntry
{
    my ($p4client,$release,$own) = @_;
    my $client_out = q(p4 client -o ).$p4client;
    my $oldpath;
    my $relpath = GetReleasePath($release).'/...';
    my $rootpath;    
    my $newpath;

    open CLIENT_OUT, "$client_out |" or die "Can't open p4 client -o ! \n$!\n";
    
    while (<CLIENT_OUT>)
    {
        if (/^Root:\s+(\S+)/)
        {
            $rootpath = $1;
        }
        
        last if (/^View:/);
    }

    $rootpath =~ tr/\\/\//;
    $newpath = $own;
    $newpath =~ tr/\\/\//;
    if ($newpath =~ /^($rootpath)/i)
    {
        $newpath =~ s[$1][$p4client];
    }
    $newpath = "//$newpath/...";
    
    while (<CLIENT_OUT>)
    {
        if (/$relpath\s+(\S+)/)
        {
            $oldpath = $1;
        }
    }

    close CLIENT_OUT or die "Can't close p4 client -o ! \n$!\n"; 

    if ($oldpath)
    {
        if ($oldpath eq $newpath)
        {
            return;
        }
        
        print "Release $release is currently set to $oldpath.\nDo you want to change settings to $newpath (y/n)? ";
        return unless (<STDIN>=~ /^[1jy]/i);
    }

    &ReplaceViewPath($p4client, $relpath, $newpath);   
}

#############################################################

sub ReplaceViewPath
{
  my ($p4client, $relpath, $newpath)= @_;
  my $client_out= q(p4 client -o ).$p4client; 
  my $client_in = q(p4 client -i ); 

  print "\n";
  
  # Copy old Branches
  open CLIENT_IN,  "| $client_in"  or die "Can't open p4 client -i ! \n$!\n";
  open CLIENT_OUT, "$client_out |" or die "Can't open p4 client -o ! \n$!\n";
  
  while ( <CLIENT_OUT> )
  {
    last if /^View:.*/;
    print CLIENT_IN;
  }
  print CLIENT_IN "View:\n";

  # drop old branch definition
  while ( <CLIENT_OUT> )
  {
    unless ( /$relpath/)
	{
		print CLIENT_IN if /^\s-*\/\/[\d|\w].*/;
	}
  }
  
  close CLIENT_OUT or die "Can't close p4 client -o ! \n$!\n"; 
  
  # Create the new Branch
  
  print CLIENT_IN qq(\t$relpath $newpath\n);
      
  close CLIENT_IN or die "Can't save p4 client -i ! \n$!\n";
  print "\nThe branch \"$relpath\" was added to \"$p4client\".\n";
  return 1;
}
#############################################################

sub GetClientRoot
{
    my ($p4client) = @_;
    my $rootpath;
    
  	$p4client= $ENV{P4CLIENT} unless ($p4client);
	
	unless ($p4client)
	{
		if(exists $ENV{'COMPUTERNAME'})
		{	$hostname= $ENV{'COMPUTERNAME'}; }
		elsif(exists $ENV{'HOSTNAME'})
		{ 	$hostname= $ENV{'HOSTNAME'}; }
		else
		{	$hostname= qx(uname -n) or die "Can't get the host name! \n$!\n";	}	
		
		$p4client = $hostname
	}
	
    my $client_out = q(p4 client -o ).$p4client;

    open CLIENT_OUT, "$client_out |" or die "Can't open p4 client -o ! \n$!\n";
    
    while (<CLIENT_OUT>)
    {
        if (/^Root:\s+(\S+)/)
        {
            $rootpath = $1;
        }
    }
        
    close CLIENT_OUT or die "Can't close p4 client -o ! \n$!\n"; 

    return $rootpath;
}

#############################################################

sub GetPort
{
    return 'perforce3006.wdf.sap.corp:3006';
}

#############################################################

sub SetOwn
{
    my ($CalledOWN, $rel) = @_;
    
    my $p4root = &GetClientRoot();
    my $relroot;
    print "\nOWN is not set. Please select one of the following:\n\n";
    print " [c] = use current OWN [$CalledOWN]\n";
    if ($p4root)
    {
        print " [p] = use Perforce client root [$p4root]\n";
        if ($rel && (GetReleasePath($rel) =~ m|^//sapdb/(\S+)|i))
        {
            $relroot = $p4root."\\".$1;
            $relroot =~ tr/\//\\/;
            print " [r] = use release default root [$relroot] (default)\n";
        }
    }
    print " [s] = manually select a different path\n";
    print "\nPlease select: ";
    my $res = <STDIN>;
    if ($res =~ /^c/i)
    {
        return $CalledOWN; 
    }
    elsif ($p4root && ($res =~ /^p/i))
    {
        return $p4root;
    }
    elsif ($relroot && (($res =~ /^r/i) || ($res =~/^$/)))
    {
        return $relroot;
    }
    elsif ($res =~ /^s/i)
    {
        return ($p4root . RequestInput("Path = $p4root", $CalledOWN, '^[\s\S]+$', "The string you entered is incorrect.\n"));    
    }
    else
    {
        die "OWN not set!\n.";
    }
}

#########################################################
sub RequestInput
{
    my ($outstr,$defval,$inpmask,$errstr) = @_;
    my $tempval;
    
    if ($outstr)
    {
        print $outstr;
    }
    
    unless ($defval =~ /$inpmask/)
    {
        print "\nThe input mask doesn't seem to be appropriate! Exiting...".$inpmask;
        exit 0;
    }
    
    $tempval = <STDIN>;
    chomp $tempval;
    
    until (($tempval =~ /$inpmask/) or ($defval and ($tempval =~ /^$/)))
    {
        print $errstr.$outstr;
        $tempval = <STDIN>;
        chomp $tempval;
    }
    
    if ($tempval =~ /^$/)
    {
        $tempval = $defval;
    }
    
    return $tempval;
}

#########################################################

sub GetReleasePath
{
    my ($rel) = @_;
    my $igme = igme::api::new();
    
    $rel = SetFullRelStr($rel);
    $rel = $1 if ($rel =~ /^(\S+)\s+/);

    my %res = $igme->igme($rel);
    $relpath = $res{'DEPOT'};
    return $relpath;
}

#########################################################

sub SetFullRelStr
{
    my ($relstr) = @_;
    my ($rel, $may, $min, $ver) = ("0","0","0","develop");
    my $outstr = "";
    
    $relstr = lc $relstr;
    
    if (($relstr == "8.0") or ($relstr == "research"))
    {
        return "8.0.00.develop";
    }
    elsif ($relstr == "toolsrc")
    {
        return "TOOL";
    }
    elsif ($relstr == "7.1.0.4")
    {
        return "7.1.04.develop";
    }
    elsif ($relstr == "6.2.TO.7.2")
    {
        return "6.2.10.to72";
    }
    elsif ($relstr =~ /^(\d+)\.(\d+)\.(\d+)$/)
    {
        return (sprintf "%1d.%1d.%2d.develop", $1, $2, $3);
    }
    elsif ($relstr =~ /^(\d+)\.(\d+)$/)
    {
        return IgmeGetLatest (sprintf "%1d.%1d.*.develop", $1, $2);
        
    }
    else
    {
        die "Invalid version! Exiting...\n";
    }
}

#########################################################

sub IgmeGetLatest
{
    my ($verstr) = @_;
    my $gtver = "";
    
    my $igme = igme::api::new();
    my @res = $igme->list($verstr);
    
    @res = sort @res;
    
    return $res[$#res];
}

#########################################################


1;

