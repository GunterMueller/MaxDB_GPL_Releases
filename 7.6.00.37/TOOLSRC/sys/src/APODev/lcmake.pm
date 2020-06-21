#!/usr/bin/perl
# lcmake           [vmake_options] targets
#
# @(#)lcmake 2002-02-28
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

package lcmake;

use imake;
use Env;

@ISA = ('Exporter');
@EXPORT = ('lcmake');


my $VMAKE_VARIANT;
# default is not sync
my $sync = -1;
my @NEWARGV=();
my $makecall ="imf";
my $found_config="";	
my $found_number=0;
my $DSP2VMAKE=0;
my $snapshot=0;
my $progname="";
my @org_ARGV=();
my $receiver="";
my $rc;
my $old_supress_vmake_pidfile = undef;
my @desc_extensions = split " ","mac shm lnk dld shr rel lib jpr";


sub my_die
{
	my $text=shift;
	if (defined $old_supress_vmake_pidfile)
	{
		$ENV{SUPPRESS_VMAKE_PIDFILE} = $old_supress_vmake_pidfile;
	}
	else
	{	
		delete $ENV{SUPPRESS_VMAKE_PIDFILE};
	}
	die ("$text");
}

sub lcmake
{
	$VMAKE_VARIANT=$ENV{VMAKE_DEFAULT_VARIANT};
	$VMAKE_VARIANT="Release" unless ($VMAKE_VARIANT);
	
	$sync = -1;
	@NEWARGV=();
	$makecall ="imf";
	$found_config="";	
	$found_number=0;
	$DSP2VMAKE=0;
	$snapshot=0;
	$progname="";
	@org_ARGV=();
	$receiver="";
	$rc = 0;
	$old_supress_vmake_pidfile = undef;
	$old_supress_vmake_pidfile = $ENV{SUPPRESS_VMAKE_PIDFILE} if (defined $ENV{SUPPRESS_VMAKE_PIDFILE});
	
	my @args = @_;
	
	$progname = shift @args;
	@org_ARGV = @args;

	while ($_ = $args[0], /^-/)
	{
		shift @args;
		
		if(/^--config=(.*)$/)
		{
			$found_config = $1 ;
			next;
		}

		if(/^--dsp2vmake$/)
		{
			$DSP2VMAKE=1;
			next;
		}
		
		if (/^--env=([^=]+)(=.*)?$/) 
		{
			if ($2) 
			{ $ENV{$1} = substr ($2, 1); }
			else 
			{	$ENV{$1} = 'YES'}
			next;
		}
		
		if(/^--inc_check(=.+)?$/)
		{
			$ENV{'INCLUDE_COMPILE_CHECK'} = ($1) ? substr ($1, 1) : "YES"; 
			next;
		}
		
		if(/^--inc_check_type=(.+)$/)
		{
			$ENV{'INCLUDE_COMPILE_TYPE'} = $1;		
			next;
		}
								
		if(/^--noconfig$/)
		{
			$found_config = "none" ;
			next;
		}
		
		if (/^--no_dircache$/) 
		{
			$ENV{'VMAKE_NO_DIRCACHE'} = "1";
			next;
		}
		
		if(/^--number=(.*)$/)
		{
			my $number = $1;
			$ENV{LATEST_CHANGE_LIST}=$1;
			$ENV{LATEST_CHANGE}=$ENV{LATEST_CHANGE_LIST};	
			$found_number = 1;
			next;
		}
		
		if(/^--mail=(.*)$/)
		{
			$receiver = $1;
			unless ($receiver =~ /\@$/)
			{
				if ($receiver =~ /^d\d\d/i)
				{	$receiver.="\@exchange.sap.corp";	}
				else
				{   $receiver.="\@sap.com"; }
			}
			next;
		}
				
		if(/^--mail$/)
		{
			$receiver = $ENV{USER};
			if ((! defined $receiver) && ($^O =~ /win/i))
			{
				$receiver = $ENV{USERNAME};
			}
			if ($receiver =~ /^(.*)\(remuser\)/)
			{
				$receiver = $1;
			}
			$receiver.="\@exchange.sap.corp";
			next;
		}

		if(/^--(force_run|fr)$/)
		{
			$ENV{SUPPRESS_VMAKE_PIDFILE}="yes";
			next;
		}


		if(/^--(error_for_warning_number|efwn)=(.*)$/)
		{
			$ENV{ERROR_FOR_WARNING_NUMBER}="$2";
			next;
		}
		
		if(/^--(force_warning|fw)=(.*)$/)
		{
			$ENV{FORCED_WARNING_NUMBER}="$2";
			next;
		}
		
		if(/^--(warning_protocol|wp)$/)
		{
			$ENV{VMAKE_WARNING_PROTOCOL}="1";
			next;
		}
		
		if(/^--snapshot$/)
		{
			$snapshot=1;
			next;
		}
		
		if(/^--noquiet$/)
		{
			$ENV{NOQUIET}="yes";
			next;
		}
		
		if (/^--report_percent(=\d{1,2})?$/)
		{
			$ENV{'VMAKE_REPORT_PERCENT'} = $1 ? substr ($1, 1) : "10";
			next;
		}
		
		if (/^--silent(=.*)?$/) 
		{
			$ENV{'VMAKE_SILENT_MODE'} = "1";
			if ($1) 
			{ $ENV{'VMAKE_SILENT_STRING'} = substr ($1, 1); }
			next;
		}
		
		if (/^--statistics$/) 
		{
			$ENV{'VMAKE_PRINT_STATISTICS'} = "1";
			if ($2) 
			{ $ENV{'VMAKE_SILENT_STRING'} = substr ($2, 1); }
			next;
		}
		
		if (/^--?[h\?]$/)
		{
			usage();
		}
		if (/^--/)
		{
			my_die "Error: unknown option $_\n";
		}

		
		# add all not matched options
		push @NEWARGV, $_;
		next;
	}
	if ($ENV{APOMAKE})
	{
		# ckeck for default or modified Configuration and changelists
		if ( $found_config ne "" )
		{
			$VMAKE_VARIANT=$found_config;
		}
		
		if ($DSP2VMAKE)
		{
			my_die "Error: Option -dsp2vmake on remote machines not allowed !\n" if ($ENV{USER} =~ /\(remuser\)/);
			print ("Creating vmake descriptions ...\n");
			if ( $^O =~ /mswin/i)
			{
				system ("python $ENV{TOOL}\\bin\\dsp2vmake.py")== 0
				or my_die "Error while executing dsp2vmake.py\n" ;
			}
			else
			{
				system ("python $ENV{TOOL}/bin/dsp2vmake.py")== 0
				or my_die "Error while executing dsp2vmake.py\n" ;
			}
		}
		if ( $found_config eq "none" )
		{
			push @NEWARGV, @args;
		}
		else
		{
			while ($_ = $args[0])
			{
				shift @args;
				my $target;
				if (/^(.*)(\.[^.]*)$/)
				{
					$target=$1."+$VMAKE_VARIANT".$2;
				}
				else
				{
					$target=$_."+$VMAKE_VARIANT";
				}
				push @NEWARGV, $target;
			}			
		}
		$ENV{VMAKE_CURRENT_VARIANT}=$VMAKE_VARIANT;
	}
	else
	{
		if ($ENV{'RELSTAT'} eq "PROD")
		{	push @NEWARGV, @args;	}
		else
		{	push @NEWARGV, CreateQualifiedTargetList(@args);	}
			
		if ( $found_config )
		{
			if ( $found_config =~ /^fast$/i)
			{	$makecall="imf";}
			elsif ( $found_config =~ /^quick$/i)
			{	$makecall="imq";}
			elsif ( $found_config =~ /^slow$/i)
			{	$makecall="ims";}					
			else
			{
				my_die "wrong syntax for -config=fast|quick|slow\n";
			}
		}
		else
		{
			$makecall=$progname;
			if ($makecall =~ /lcmake/)
			{
				$makecall = "imf";	
			}
		}
	}
		
	unless ($found_number)
	{
		if ($ENV{RELSTAT} =~ /^PROD$/i)
		{
			if ( -f "$ENV{OWN}/syncinfo.dat")
			{
				open(SYNCINFO, "< $ENV{OWN}/syncinfo.dat") || my_die "Couldn't open $ENV{OWN}/syncinfo.dat for reading";
				my $syncnumber = <SYNCINFO>;
				if ( $syncnumber =~ /<maxchangenr>(.*)<\/maxchangenr>/)
					{
						$ENV{LATEST_CHANGE_LIST}= $1;
					}
					close (SYNCINFO);
					print ("Current changelist is $ENV{LATEST_CHANGE_LIST}\n");
			}
			elsif (-f "$ENV{OWN}/changelist.log")
			{
				open(CHLST, "<$OWN/changelist.log");
				my $changelist=<CHLST>;
				chomp $changelist;
				$ENV{LATEST_CHANGE_LIST} = $changelist;
				close CHLST;
				print ("Current changelist is $ENV{LATEST_CHANGE_LIST}\n");
			}
			elsif ( $ENV{LATEST_CHANGE} )
			{
				$ENV{LATEST_CHANGE_LIST}=$ENV{LATEST_CHANGE};
			}
			else
			{
				print ("Warning: no changlist found \n     set LATEST_CHANGE_LIST to 111\n");
				$ENV{LATEST_CHANGE_LIST}="111";
			}
			if (-f "$ENV{OWN}/make.id")
			{
				open(MAKEID, "<$OWN/make.id");
				my $makeid=<MAKEID>;
				chomp $makeid;
				$ENV{MAKEID} = $makeid;
				close MAKEID;
			}
			else
			{
				print ("Warning: no makeid found \n     set MAKEID to 000\n");
				$ENV{MAKEID}="000";
			}
		}
		else
		{
			$ENV{LATEST_CHANGE_LIST}="000000";
			$ENV{MAKEID}="000000";
		}
		$ENV{LATEST_CHANGE} = $ENV{LATEST_CHANGE_LIST};
	}
		
	if ($snapshot)
	{
		my_die ("snapshort option is not yet supported for windows\n")  if ($^O=~/.*win32.*/i);
		my $org_VMAKE_PATH=$ENV{VMAKE_PATH};
		my $org_VMAKE_OPTION=$ENV{VMAKE_OPTION};
		my $remPATH;
		mkdir ("$ENV{OWN}/snapshot", 0755);
		my $pos = (index $ENV{VMAKE_PATH},",");
		$remPATH = substr $ENV{VMAKE_PATH}, $pos;
		$ENV{VMAKE_PATH} = "$ENV{OWN}/snapshot$remPATH";
		$ENV{OWN}= "$ENV{OWN}/snapshot";
		$ENV{INSTROOT}= "$ENV{OWN}/usr";
		$ENV{VMAKE_OPTION}="SVI";
		$ENV{VMAKE_OPTION}.="n" if ($org_VMAKE_OPTION=~ /n/);
		
		$rc = &{$makecall}(@NEWARGV);
		my_die $@ if $@;
	
		unless ($receiver eq "")
		{
			sendMail("Snapshot");
		}
		my_die "Snapshot failed\n" unless ( $rc == 0 );
		$ENV{OWN} =~ s/\/snapshot$//;
		$ENV{INSTROOT}= "$ENV{OWN}/usr";
		$ENV{VMAKE_OPTION} = $org_VMAKE_OPTION;
		$ENV{VMAKE_PATH} = "$ENV{OWN},$ENV{OWN}/snapshot$remPATH";
	}

	$rc = &{$makecall}(@NEWARGV);
	my_die $@ if $@;
	
	if ($receiver ne "")
	{
	    sendMail("Make",@NEWARGV);
	}
	if (defined $old_supress_vmake_pidfile)
	{
		$ENV{SUPPRESS_VMAKE_PIDFILE} = $old_supress_vmake_pidfile;
	}
	else
	{	
		delete $ENV{SUPPRESS_VMAKE_PIDFILE};
	}

	return $rc;
}

sub usage
{
	if ($ENV{APOMAKE})
	{
	    print ("USAGE: $progname [--dsp2vmake] [--config=<configuration>] [-noquiet]\n");
	    print ("                 [--number=<changelist>] [--mail[=<mailaddress>]]\n");
	    print ("                 ");	
	}
	else
	{
	    print ("USAGE: $progname [--mail[=<mailaddress>]] [--noquiet] ");
	}
	
	print ("[--snapshot] ") unless ($^O=~/.*win32.*/i);
	print ("<vmake options> <targetlist>\n\n");

	if ($ENV{APOMAKE})
	{
		print ("    --dsp2vmake\n");
		print ("              call dsp2vmake before make\n");
		print ("              (sources in OWN/sys/src directory necessary)\n\n");
		
		print ("    --config=<configuration>\n");
		print ("              set configuration (Release, Debug)\n");
		print ("              default configuration is set by ".(($^O=~/.*win32.*/i) ? "\%VMAKE_DEFAULT_VARIANT\%":"\$VMAKE_DEFAULT_VARIANT")."\n\n");
		
		print ("    --number=<changelist>\n");
		print ("              set <changlist> for make\n\n");

	}
	print ("    --mail=<email address>\n");
	print ("              inform the receiver about the make result\n\n");
	print ("    --force_run\n");
	print ("              allow parallel vmake run\n\n");
	print ("    --noquiet\n");
	print ("              print compiler and linker commands to stdout\n\n");
	print ("    --env=<varname>\n");
	print ("              set Environment variable <varname> to YES\n\n");
	print ("    --env=<varname>=<value>\n");
	print ("              set Environment variable <varname> to <value>\n\n");
	print ("    --report_percent[=<value>]\n");
	print ("              print vmake report info every <value> percent \n");
	print ("              (default is 10) \n\n");
	print ("    --inc_check[=force]\n");
	print ("              activate compile check for marked include files\n");
	print ("              (use force to check all includes) \n\n");
		
	unless ($^O=~/.*win32.*/i)
	{
		print ("    --snapshot\n");
		print ("              copy files to local directory before make\n\n");
	}
	else
	{
		print ("    --error_for_warning_number=<number>[,<number>...]\n");
		print ("              force errors for warning numbers \n");
		print ("              Short form: --efwn=... \n\n");
		print ("    --forced_warning=<number>[,<number>...]\n");
		print ("              force output for warning numbers \n");
		print ("              Short form: --fw=... \n\n");
	}
	
	imf ("-?");
	exit 1;
}

sub sendMail
{
	
	require Mail::Sendmail;
	import Mail::Sendmail;

	my ($sub, $msg, %mail);
	my $action = shift;
	my @args = @_;
	my @targets = ();
	my $OWN_SHARE = undef;
	my $hostname =  `hostname`;
	$hostname =~ s/\n$//;
	
	if (($^O=~/.*win32.*/i) && defined $ENV{'SAPDB_INTERNAL'})
	{
		my $share_name = undef;
		my $share_path = undef;
		
		open (SHARE_CMD, "net share |");
		while (<SHARE_CMD>)
		{
			if (/^(\S+)\s+(\S+Develop\S*)\s*/)
			{
				$share_name = $1;
				$share_path = $2;
				$share_path =~ s/(\/|\\)/\\$1/g;
				if (($share_name !~ /\$$/) && ($ENV{'OWN'} =~ /^$share_path(.*)$/))
				{
					$OWN_SHARE = "\\\\$hostname\\$share_name$1";
					last;
				}
			}
		}
	}
	
	foreach (@args)
	{
		unless (/^-/) 
		{
			
			if (/^(.*)(\/\/|\/-)/)
			{ push @targets, $1; }
			else
			{ push @targets, $_; }
		}
	}
	
	$sub = "$action ";
	defined ($ENV{'LC_VER_STR'}) and
		$sub .= "$ENV{'LC_VER_STR'} ";
	$sub .=  "on $hostname ";
	if ($rc == 0)
	{	$sub.="successfully finished";	}
	else
	{	$sub.="failed";	}
	$sub.=" ($progname ". (join " ", @org_ARGV).")";
	$msg = "Following make finished on $hostname:";
	$msg = "ACTION: $action\n";
	if (defined $ENV{'HTTP_OWN_ADDRRESS'})
	{ $msg.= "OWN: $ENV{'HTTP_OWN_ADDRRESS'}\n"; }
	else
	{ $msg.= "OWN: $ENV{OWN}\n"; }
	$msg.= "VMAKE_PATH: $ENV{VMAKE_PATH}\n";
	$msg.= "COMMAND: $progname @org_ARGV\n";
	$msg.= "STATUS: ";
	if ($rc == 0)
	{	$msg.="OK\n";	}
	else
	{	$msg.="ERROR\n";	}
	unless ($ENV{'HTTP_PROT_ADDRRESS'})
	{
		my $vmake_version = ($progname =~ /imf/) ? "fast" :  (($progname =~ /imq/) ? "quick" : "slow");
		foreach my $target (@targets)
		{
			$msg .= "\n";                                
			# missing extension and missing protfile without extension
			# seems a unqualified description 
			unless ( ($target =~ /\./) || (-f "$ENV{'OWN'}/sys/wrk/$vmake_version/prot/$target.p0"))
			{
				foreach (@desc_extensions)
				{
					if ( -f "$ENV{'OWN'}/sys/wrk/$vmake_version/prot/$target.$_.p0")
					{
						$target .= ".$_";
						last;
					}
				}
			}
			my $prot_path = (defined ($OWN_SHARE) ? "$OWN_SHARE" : "$ENV{'OWN'}").
			                 "/sys/wrk/$vmake_version/prot/";
			if ($^O=~/.*win32.*/i)
			{
				$prot_path =~ tr /\//\\/;
				$prot_path = "$prot_path";
			}
			$msg .= "\nTarget:         $target\n";
			
			my $target_path = $target;
			$target_path =~ tr/://d;
			($^O=~/.*win32.*/i) and
				$target_path =~ tr/\//\\/;
						
			if ($rc != 0 )
			{
				
				$msg .= (-f "$ENV{'OWN'}/sys/wrk/$vmake_version/prot/$target_path.e0") ?
				        ("Error protocol: $prot_path$target_path.e0\n"):
				         "Can't find error protocol\n";
			} 
			$msg .= (-f "$ENV{'OWN'}/sys/wrk/$vmake_version/prot/$target_path.p0") ?
			        "Make protocol:  $prot_path$target_path.p0\n":
			         "Can't find make protocol\n";
		}
	}
	unless ($action =~ /Snapshot/i)
	{
		
		if (defined $ENV{'HTTP_SHOW_ADDRRESS'})
		{
			$msg .= "\n";                                
			$msg .= "Show make:\n  $ENV{'HTTP_SHOW_ADDRRESS'}\n";
		}
		if (defined $ENV{'HTTP_PROT_ADDRRESS'})
		{
			$msg .= "\n";
			$msg .= "For protocols see:\n  $ENV{'HTTP_PROT_ADDRRESS'}\n";
		}
	}
	
	%mail = ( To      => "$receiver",
              From    => "$progname\@sap.corp",
              Message => "$msg",
              Smtp    => 'mail.sap.corp',
              Subject => "$sub" );

	print "Send mail to $receiver\n";
	
    if ( ! sendmail(%mail) ) {
       print "FATAL: sendmail error $Mail::Sendmail::error\n";
	}
}

sub CreateQualifiedTargetList
{
	my @args = @_;
	my @new_list = ();
	require SDB::Make::Path;
	import SDB::Make::Path;
	foreach (@args)
	{	push @new_list, ($_ =~ /^[\+-]/) ? $_ : SDB::Make::Path::GetQualifiedTargetName($_);	}
	return @new_list;
}
1;