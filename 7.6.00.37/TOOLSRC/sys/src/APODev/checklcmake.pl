#!/usr/bin/perl
#
# checklcmake.pl Script
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#
use qadblca;
use strict;


my $VMAKE_VARIANT=$ENV{VMAKE_DEFAULT_VARIANT};

$VMAKE_VARIANT="Release" unless ($VMAKE_VARIANT);
my $MAKEID=undef;
my $qalca_ok=1;




#define vars
my ($Target, $Prot, $ErrFound, @Lines, @Msg);

my @Targets = ();

while ($_ = $ARGV[0], /^-/) 
{
	shift @ARGV;
	last if /^--$/;
      
	if(/^-config=(.*)$/) 
	{
		$VMAKE_VARIANT = $1 ; 
		next; 
	}
	if(/^-makeid=(.*)$/) 
	{
		$MAKEID = $1; 
		next; 
	}
	  
	if(/^-noconfig$/) 
	{
		$VMAKE_VARIANT = "" ; 
		next; 
	}

	if (/^-[h\?]$/)
	{
		#$need_help=1;
		last;
	}

	next;
}


my $variant_string = ($VMAKE_VARIANT eq "") ? "" : "+${VMAKE_VARIANT}";


foreach my $Target ( @ARGV )
{
	# read prot
    $Prot= "$ENV{WRK}/fast/prot/";
	if ($Target =~ /^(.*)\.([^.]*)$/)
	{
		$Prot.= "$1${variant_string}.${2}.e0";
	}
	else
	{
		die "Please call checklcmake with qualified target names !\n";
	}
    
    if ( !open (IN, "<$Prot") )  {
        print "Make protocol $Prot not found (ignored)\n";
        next;
    }
    @Lines = <IN>;
    close IN;

    print "Checking make protocol $Prot for errors\n";
    $ErrFound = grep { /END:\s*MAKE\s*TARGET:\s*\S*\s*RESULT:\s*ERROR/i } @Lines;
    push @Msg, @Lines if ( $ErrFound );
}

if ( @Msg ) {

    print "Errors found in make protocols\n";

    # send mail
    &SendErrorMail(@Msg) if ( @Msg ); 
    # exit with error code
    exit -1;
}

print "No errors found in make protocols\n";
exit 0;

#########################################################################

sub SendErrorMail 
{
    use Mail::Sendmail;

    my ($msg, $sub, $receiver, $cc, $bcc, %mail, $it, $id, @targets, %t, $changelist );
    my ($hostname, $rel, $os, $tinsub, %duser, @duser,@deperrors);
    my (@localtime, $webdavname, $rc, @Lines, %layers, %d, @other_errors, %receivers, @responsibles);
    my $qahlca;

    $os = getPlatform();
    $hostname =  `hostname`; 
    $hostname =~ s/\n$//;
        
	@Lines = @_;
	$it = 0;
	$id = 0;
	$changelist = "";
	if (-f "$ENV{OWN}/changelist.log")
	{
		open(CHLST, "<$ENV{OWN}/changelist.log");
		$changelist = <CHLST>;
		chomp $changelist;
		close CHLST;
	}
	while ($_ = $Lines[0])
	{
		shift @Lines;
		if (s/^TARGET:\s*'(\S*)'\s*.*/$1/)
		{
			chomp;
			my $target = $_; 
			unless ( defined $t{$target} )
			{
				if ($Lines[1] =~ /not made because of dependency errors of/)
				{
					$d{$target} = ++$id;	
				}
				else
				{
					$t{$target} = ++$it ;
					$layers{$1} = 1 if (/::?(.*)\/[^\/]*$/);
				}
				
			}
		}	
	}
	
	@localtime = localtime(time);
    @targets = sort {$t{$a} <=> $t{$b}} keys %t;
    @deperrors = sort {$d{$a} <=> $d{$b}} keys %d;
	$webdavname = $os . "_LCAPPS_$ENV{BRANCH_INFO}_";
	$webdavname.= "${VMAKE_VARIANT}_" if ($VMAKE_VARIANT ne "");
	$webdavname.= sprintf ("%04d%02d%02d%02d%02d", $localtime[5] + 1900, $localtime[4] + 1, $localtime[3], $localtime[2], $localtime[1]);  
    @other_errors = ();
    
    unless (defined $ENV{APO_SRC})
    { 	push @other_errors, "missing environment (APO_SRC not set)" }
    else
    {
	    foreach (keys %layers)
	    {
	    	@responsibles = getresponsibles ($_); 
	    	if (scalar(@responsibles))
	    	{
	    		foreach (@responsibles)
	    		{
	    			$receivers{$_} = 1;
				}
	    	}
	    	else
	    	{	push @other_errors, "missing responsible.txt for $_"; }
	    }
	}
	
    $msg  = "\nChangelist: $changelist\n\n-------------------------\n";
    
    if ( defined $MAKEID )
    {
    	$qahlca = qadblca -> new ( {'ID' =>"$MAKEID"});
    	
		if ( $qahlca->{error_code} != 0 )
		{
		   $MAKEID = undef;	
		   print "ERROR: ".$qahlca->{error_text}."\n" ;
		   $msg .= "\nWARNING: can't write protocol to QADB (id error -".$qahlca->{error_text}.")\n" ;
		}
		else
		{
			my $prot_text = "";
			foreach (@_) {$prot_text.=$_;}
			$prot_text.="\n-----------------------------------\n\nChangelist: $changelist \n\n" if ($changelist ne ""); 
			my $prot_link = $qahlca->write_prot("$webdavname", $prot_text, "make error protocol");
            if ($qahlca->{'error_code'} != 0)
            {
				print STDERR "ERROR: can\'t write prot to DB: $qahlca->{error_text}\n" ;
				$msg .= "\nWARNING: can't write protocol to QADB (write error - ".$qahlca->{error_text}.")\n" ;
				$MAKEID = undef;
            }
			else
			{
				$msg .= "\nfor more infomation see:\n";
				$msg .= "  makedetails:    http://pts:1081/TestMonitor/LCAMake_Details.jsp?id=$MAKEID\n";
				$msg .= "  error protocol: $prot_link\n";
			}
		}
    }
    if ( ! defined $MAKEID )
    {
 	   	open (OUT,">$webdavname");
    	print OUT @_ ;
    	print OUT "\n-----------------------------------\n\nChangelist: $changelist \n\n" if ($changelist ne ""); 
    	close OUT;
	    print "calling webdav.py $webdavname\n";
	    if ($^O=~/.*win32.*/i)
	    { 	$rc = system("python $ENV{TOOL}\\bin\\webdav.py $webdavname"); }
	    else
	    {	$rc = system("webdav.py $webdavname");   }
	    print "system rc = $rc\n";
    	unlink ($webdavname);
    	$msg .= "\nfor more info see protfile: http://pts:1080/webdav/daily_makes/$webdavname\n";
    }
    
    $msg .= "\n-------------------------\n";
    $bcc = 'gerald.arnold@sap.com';
    $cc = '';
    
    $msg.= "\nAffected targets:\n\n" . join("\n",@targets);
    $msg.= "\n\n\ndependency errors:\n\n" . join("\n",@deperrors);
    $sub = "make problem on $os ($hostname) $ENV{BRANCH_INFO} $VMAKE_VARIANT (CL $changelist)";

    if (scalar(@other_errors))
    { $msg.= "\n\n\nother errors:\n\n" . join("\n",@other_errors); }
    
    if ( -f "$ENV{APO_SRC}/sys/src/responsible.txt") 
    {
		if ( open (IN, "<$ENV{APO_SRC}/sys/src/$_/responsible.txt") )  
	    {
	    	my $user ="";
	        while (<IN>)
	    	{
	    		if (/^\s*(\S*)\s*\((.*)\)/)
	    		{
	    			$user = $1;
	    			my @user_platforms = split(',',$2);	
	    			foreach (@user_platforms)
	    			{
	    				s/ //g;
	    				if ( $_ =~ /$os/i )
	    				{
	    					$cc.= ($cc eq '') ? "$user":",$user" unless ( defined $receivers{$user}); ;
	    					last;
	    				}
	    			}
	    		}
	    		else
	    		{
	    			chomp;
	    			$user = $_;
	    			$cc.= ($cc eq '') ? "$user":",$user" unless ( defined $receivers{$user}); ;
	    		}
	    	}
	    }
	    close IN;    		
    }
    else
    {  	$cc = 'sonja.zscherpel@sap.com,wolfgang.wolesak@sap.com,el.houssine.touh@sap.com'; }
    
        
    $receiver = join (',', keys %receivers);
    unless ($receiver)
    {
    	$receiver = $cc;
    	$cc="";
    }
    
    %mail = ( To      => "$receiver",
              From    => 'autolcmake@sap.corp',
              Message => "$msg",
              Cc      => "$cc",
              Bcc	  => "$bcc",
              Smtp    => 'mail.sap.corp',
              Subject => "$sub" );

	print "Message: \n$msg\n";

    my $email ="";
    my $email_link_name = "Copy of sent autolcmake mail";
     
    if ( ! sendmail(%mail) ) {
       print "FATAL: sendmail error $Mail::Sendmail::error\n";
       $email_link_name = "Error while sending autolcmake email\n";
       $email .= "FATAL: sendmail error $Mail::Sendmail::error for following email\n\n";
	}
	
	if (defined $MAKEID)
    {
        $email .= "To     : $receiver\n";
        $email .= "Cc     : $cc\n";
        $email .= "Subject: $sub\n\n";
        $email .= "Content:\n\n$msg\n";

        my $ename = "EMail_" . $webdavname;

        my $mail_link = $qahlca->write_prot("$ename", $email, $email_link_name);
        if ($qahlca->{'error_code'} != 0)
        {
            print STDERR "ERROR: Can't write protocol to QADB!\n$qahlca->{'error_text'}\n";
            $MAKEID = undef;
        }
    }
}
0;

sub getresponsibles 
{
	my $layer = shift;
	my @responsibles = ();
	if ( (-f "$ENV{APO_SRC}/sys/src/$layer/responsible.txt") && open (IN, "<$ENV{APO_SRC}/sys/src/$layer/responsible.txt"))
	{
	    while (<IN>)
	    {
	    	chomp;
	    	push @responsibles, $_;
	    }
	    close IN;
	}
	elsif ( $layer =~ /^(.*)\/[^\/]*$/ ) # look in the parent directory
	{
		@responsibles = getresponsibles ($1);
	}
	
	return @responsibles;
}

sub getPlatform
{
	my $uname="";
	my %mapping = ();
	$mapping{'HP-UX'} = "hp_64";
	$mapping{'HPIA64'} = "hpia64";
	$mapping{'AIX'} = "rs6000_64";
	$mapping{'AIX5'} = "rs6000_51_64";
	$mapping{'SunOS'} = "sun_64";
	$mapping{'SunX86_64'} = "sunx86_64";
	$mapping{'OSF1'} = "alphaosf";  
	$mapping{'Windows_NT'} = "NTintel";
	$mapping{'Windows_IA64'} = "NTia64";
	$mapping{'Windows_AMD64'} = "NTx86_64";
	$mapping{'Linux_ia64'} = "linuxia64";
	$mapping{'Linux_ia64_9'} = "linuxia64sles9";
	$mapping{'Linux_ix86'} = "linuxintel";
	$mapping{'Linux_ppc64'} = "linuxppc64";
	$mapping{'Linux_x86_64'} = "linuxx86_64";
			
	if($ENV{'OS'})
	{
		chomp $ENV{'OS'};
		$uname=$ENV{'OS'};
		if(($^O=~/.*win32.*/i) && defined $ENV{BIT64})
		{
			$uname = ($ENV{PROCESSOR_ARCHITECTURE} =~ /AMD64/) ? "Windows_AMD64" : "Windows_IA64";
		}
	}
	elsif ( $^O =~ /linux/i)
	{
		open(UNAME, "uname -m |");
		my $utmp=<UNAME>;
		close UNAME;
		chomp $utmp;		
		$uname = "Linux_$utmp";
		$uname =~ s/Linux_i\d86/Linux_ix86/;
		if (($utmp =~ /^ia64$/i) && ($ENV{BRANCH_INFO} =~ /^[34]/))
		{
			open(UNAME, "uname -r |");
			$utmp=<UNAME>;
			close UNAME;
			chomp $utmp;		
			if ($utmp =~ /^2\.6/)
			{
				$uname .= "_9";
			}
		}
	}
	elsif ( $^O =~ /aix/i)
	{
		$uname = "AIX";
		open(UNAME, "uname -v |");
		my $utmp=<UNAME>;
		close UNAME;
		chomp $utmp;
		if ($ENV{BRANCH_INFO} =~ /^3/)
		{
			$uname=$uname . $utmp if($utmp == 5);
		}
	}
	elsif ( $^O =~ /hpux/i )
	{
		$uname = "HP-UX";
		open(UNAME, "uname -m |");
		my $utmp=<UNAME>;
		close UNAME;
		chomp $utmp;
		if ($utmp =~ /ia64/i )
		{   $uname = "HPIA64";	}
	}
	elsif ( $^O =~ /solaris/i )
	{
		$uname = "SunOS";
		open(UNAME, "uname -m |");
		my $utmp=<UNAME>;
		close UNAME;
		chomp $utmp;
		if ($utmp =~ /i86pc/i )
		{   $uname = "SunX86_64";	}
	}
	else
	{
		open(UNAME,"uname|");
		$uname=<UNAME>;
		close(UNAME);
		chomp $uname;
	}
	
	return $mapping{$uname};
}