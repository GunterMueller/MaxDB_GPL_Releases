#!/usr/bin/perl -w
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


#use warnings;
#use strict;

package PerforceEnv;


######################################################### 

@PerforceEnv::p4_exlude_branch= ();
#  ('cockpit', 'dps', 'extern', 'fcs', 'forecast',
#   'ops', 'optgui', 'snp', 'snpopt');

$branch_key = 'BRANCH_INFO';
$ch_list_key= 'LATEST_CHANGE_LIST';


# ($PerforceEnv::p4_user,   $PerforceEnv::p4_client,
# $PerforceEnv::p4_branch, $PerforceEnv::change_list)=
# &check_perforce_env;

#&check_perforce_branch($PerforceEnv::p4_client,
#                       $PerforceEnv::p4_branch);

 
#print qx(p4 sync $PerforceEnv::sync_flags);
 
#######################################################
sub sync_perforce_branch
{
 my ($Branch, $p4_changelist) = @_;	
 unless ($p4_changelist)
 {
	 $p4_changelist= 
		  $1 if qx(p4 changes -m 1 -s submitted $Branch/...)=~ m|^Change\s*(\d+)\s|; 
 }
 
 $ENV{$ch_list_key}= $p4_changelist;
 my $sync_flags = $Branch.q(/...@).$p4_changelist;
 print "sync files ($sync_flags) ...\n";
	
 my $check_client= q(p4 client -o ).$p4_client; 
 open P4_CLIENT_SPEC, "$check_client |" or
  die "Can't open perforce client: $!";
 
 my ($perf_root, $line)= ();
 while (<P4_CLIENT_SPEC>)
 {
   $perf_root= $1, last if /^Root:\s+(.*)/;
 }


 print qx(p4 sync $sync_flags);
}

########################################################
sub check_perforce_client
{
	my ($p4_user, $p4_client) = @_;

	@p4_clients = qx(p4 clients);	

	$p4_client= $ENV{P4CLIENT} unless ($p4client);
	
	unless ($p4_client)
	{
		if(exists $ENV{'COMPUTERNAME'})
		{	$host_name= $ENV{'COMPUTERNAME'}; }
		elsif(exists $ENV{'HOSTNAME'})
		{ 	$host_name= $ENV{'HOSTNAME'}; }
		else
		{	$host_name= qx(uname -n) or die "Can't get the host name! \n$!\n";	}	
		
		$p4_client = $host_name
	}
	
	foreach ( @p4_clients)
	{
		if ($p4_client and /^Client\s$p4_client\s.*/)
		{
			print $_, "\n"; 
			$server_client = $p4_client;
			last;
		}
	}

	unless ($server_client)
 	{
	    print "\nDo you want to make a new perforce client (y/n)? "; 
	    exit 0 unless <STDIN>=~ /^[1jy]/i;
	
	    $p4_client= &make_perforce_client($p4_user, $p4_client);
	}
 	return $p4_client;
} 

##########################################################
sub check_perforce_user
{
 $p4_user  = $ENV{P4USER};

 unless( $p4_user )
 {
    if (exists $ENV{'CURRENTUSER'} )
    {
    	$p4_user= $ENV{'CURRENTUSER'};
    }
    else
    {
      $p4_user= getlogin or die "Can't get the user name! \n$!\n";
    }
    
    $p4_user =~ tr/A-Z/a-z/;
    print "\nPerforce user is not set!\n";
    print "Do you want to set it to: $p4_user (y/n)? "; 
    exit 0 unless <STDIN>=~ /^[1jy]/i;
    $ENV{'P4USER'}= $p4_user;
 }
 return $p4_user;
}
 

######################################################### 
sub check_perforce_branch
{
 my ($p_client, $Depot, $Branch,  $subdir)= @_;
 my $check_client= q(p4 client -o ).$p_client; 
 open P4_CLIENT_SPEC, "$check_client |" or
  die "Can't open perforce client: $!";
 
 my ($perf_root, $line)= ();
 while (<P4_CLIENT_SPEC>)
 {
   $perf_root= $1, last if /^Root:\s+(.*)/;
 }
 print "Perforce root directory: ", $perf_root, "\n";

 while (<P4_CLIENT_SPEC>)
 {
   $line= $_, last if m(//);
 }
 
 my $match_str= $Depot.'/'.$Branch.'/';
 while ($line and $line!~ m(^\s+$match_str) )
 {
   $line= <P4_CLIENT_SPEC>;
 }
 
 close P4_CLIENT_SPEC or die "Can't close p4 client -o ! \n$!\n"; ;

 $ENV{$branch_key} = $Branch;
 
 if(!$line)
 {
   print "The branch has not been found. Updating the client...\n";
   return &add_perforce_branch($p_client, $Depot, $Branch, $subdir);
 }
 elsif ( $line!~ m($Branch/$subdir))
 {
   print "The branch \"$match_str\" is mapped to an other directory. \n Please remove this branch definition from client ...\n";
   <STDIN>;
   exit;
   # return &change_perforce_branch($p_client, $p_branch, $subdir);
 }
 else
 {
   return 0;
 }
}

######################################################### 
sub add_perforce_branch
{
  my ($client_name, $depot_name, $branch_name, $subdir )= @_;
  my $client_out= q(p4 client -o ).$client_name; 
  my $client_in = q(p4 client -i ); 

  # Copy old Branches
  open CLIENT_IN,  "| $client_in"  or die "Can't open p4 client -i ! \n$!\n";
  open CLIENT_OUT, "$client_out |" or die "Can't open p4 client -o ! \n$!\n";
  
  while ( <CLIENT_OUT> )
  {
    last if /^View:.*/;
    print CLIENT_IN;
  }
  print CLIENT_IN "View:\n";
  while ( <CLIENT_OUT> )
  {
    print CLIENT_IN if /^\s-*\/\/[\d|\w].*/;
  }
  
  close CLIENT_OUT or die "Can't close p4 client -o ! \n$!\n"; 
  
  # Create the new Branch
  
  my $p4_prefix = $depot_name.'/'.$branch_name;  
  my $p4_postfix= q(/... //).$client_name.'/'.$branch_name.'/';
  $p4_postfix.=$subdir if ($subdir);
  
  print CLIENT_IN qq(\t$p4_prefix$p4_postfix...\n);
      
  foreach (@PerforceEnv::p4_exlude_branch)
  {
    print CLIENT_IN qq(\t-$p4_prefix/$_$p4_postfix$_/...\n);
  }   
  close CLIENT_IN or die "Can't save p4 client -i ! \n$!\n";
  print "\nThe branch \"$branch_name\" was added to \"$client_name\".\n";
  return 1;
}


######################################################### 
sub make_perforce_client
{
  my ($p_user, $p_client)= @_;
  my $build_path;  
  if(exists $ENV{'BUILD_PATH'})
  {
    $build_path = $ENV{'BUILD_PATH'};
  }
  else
  {
    print "Please enter the build path: ";
    chop ( $build_path= <STDIN> );
  }

  my $gethostname = q(hostname); 
  

  # Copy old Branches
  open HOSTNAME, "$gethostname |" or die "Can't get hostname ! \n$!\n";
  my $p_hostname= <HOSTNAME>;
  close HOSTNAME;
  
   
  $ENV{'P4CLIENT'}= $p_client;  
  my $perforce_dir = "$build_path"; 
  print "\nYour perforce root directory is: \"$perforce_dir\"";

  print "\nCreating client \"$p_client\".\n";
  open( CLIENT_IN, '| p4 client -i') or die "open p4 client -i failed! \n$!\n";
  print CLIENT_IN "Client: $p_client\n";
  print CLIENT_IN "Owner: $p_user\n";
  print CLIENT_IN "Host: $p_hostname\n";
  print CLIENT_IN "Description:\n\tCreated by VMFD-Tool.\n";
  print CLIENT_IN "Root:\t$perforce_dir\n";
  print CLIENT_IN "Options:\tnoallwrite noclobber nocompress crlf unlocked nomodtime normdir\n"; 
  print CLIENT_IN "View:\n";
  close CLIENT_IN or die "Can't save the p4 client! \n$!\n";

  print "Perforce client \"$p_client\" was created.\n";
  return $p_client;
}

############################
# OBSOLETE
############################
######################################################### 
sub check_perforce_env
{
 my $change_list= $ENV{$ch_list_key};
 my ($p4_user, $p4_client, $server_client, $host_name)= ();
 
 $p4_user= &check_perforce_user;
 $p4_client = &check_perforce_client($p4user);
  
 unless ( exists $ENV{$ch_list_key} )
 {
    $change_list= 
      $1 if qx(p4 changes -m 1 -s submitted)=~ m|^.+\s(\d+)\s|; 
    $ENV{$ch_list_key}= $change_list;
 }
  
 print "Perforce User:   ", $p4_user, "\n";
 print "Perforce Client: ", $p4_client, "\n";
 print "Change List:     ", $change_list, "\n";
 
 my $p4_branch;
 if( exists $ENV{$branch_key} )
 {
   $p4_branch= $ENV{$branch_key};
   print "Perforce Branch: $p4_branch\n";
 }
 else
 {
   print "\nPerforce Branch not defined !\n";
   while(1)
   {
  	  print "\nPlease enter the branch name: ";
      $p4_branch= <STDIN>;
      print "\nBranch name is: $p4_branch Is it O.K.? ";
      last if <STDIN>=~ /^[1jy]/i;
   }     
   chop($p4_branch);

   $ENV{$branch_key}= $p4_branch;
 }
   
 return ($p4_user, $p4_client, $p4_branch, $change_list);
}

######################################################### 
sub change_perforce_branch
{
  my ($client_name, $branch_name, $subdir )= @_;
  my $client_out= q(p4 client -o ).$client_name; 
  my $client_in = q(p4 client -i ); 

  # Copy old Branches
  open CLIENT_IN,  "| $client_in"  or die "Can't open p4 client -i ! \n$!\n";
  open CLIENT_OUT, "$client_out |" or die "Can't open p4 client -o ! \n$!\n";
  
  while ( <CLIENT_OUT> )
  {
    last if /^View:.*/;
    print CLIENT_IN;
  }
  print CLIENT_IN "View:\n";

  my $p4_prefix = q(//apo/).$branch_name;  
  my $p4_postfix= q(/... //).$client_name.'/'.$branch_name.'/';
  $p4_postfix.=$subdir if ($subdir);

  # drop old branch definition
  while ( <CLIENT_OUT> )
  {
    unless ( /$p4_prefix\//)
	{
		print CLIENT_IN if /^\s-*\/\/[\d|\w].*/;
	}
  }
  
  close CLIENT_OUT or die "Can't close p4 client -o ! \n$!\n"; 
  
  # Create the new Branch
  
  print CLIENT_IN qq(\t$p4_prefix$p4_postfix...\n);
      
  foreach (@PerforceEnv::p4_exlude_branch)
  {
    print CLIENT_IN qq(\t-$p4_prefix/$_$p4_postfix$_/...\n);
  }   
  close CLIENT_IN or die "Can't save p4 client -i ! \n$!\n";
  print "\nThe branch \"$branch_name\" was added to \"$client_name\".\n";
  return 1;
}


