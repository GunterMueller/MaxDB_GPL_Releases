#!/usr/bin/perl
#    cgicall.pl - cgi script for remote makes
#
#    @(#)cgicall.pl     2003-12-09
#
#    GAR, SAP AG
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

use CGI;
use File::Copy;
use File::Path;
use FileHandle;
use strict;

$| = 1;

# defaults
my $pid_file_name = "build.pid";
my @desc_extensions = split " ","mac shm lnk dld shr rel lib jpr";


$ENV{'TOOL'}="D:\\SAPDevelop\\DevTool";
$ENV{'DOCUMENT_ROOT'}="D:\\SAPDevelop\\remuser\\wwwroot\\make";

my @localfiles = (".ipreprof",".iuser");
my $iprofile = ".iprofile";
if($^O=~/win32/i)
{
	@localfiles = ("ipreprof.bat","iuser.bat");
	$iprofile = "iprofile.bat";
}

my $query = CGI->new();
print $query->header(-type=>'text/html',-expires=>'now');

my $user       = $query->param('user');
my $release    = $query->param('release');
my $lc_state   = $query->param('lc_state');
my $src_dir    = $query->param('src_dir');
my $cmd        = $query->param('cmd');
my @cmdparams  = $query->param('cmdparam');
my $bit64      = $query->param('bit64');
my $ignoreown  = $query->param('ignoreown');
my $debug      = $query->param('debug');
my $action     = $query->param('action');
my $mode       = $query->param('mode');          # telnet
my $type       = $query->param('type');          # lc or lcapps
my $platform   = $query->param('platform');      # platformtag
my $oldprofile = $query->param('oldprofile');    # old -> use old profile | new -> generate profile
my $timeout    = defined $query->param('timeout') ?  $query->param('timeout') : 1000;          
my $version    = $query->param('version'); # fast|quick|slow
my $cmdtargets = $query->param('cmdtargets');
my $cmdoptions = $query->param('cmdoptions');

# lcapps variables
my $branch          = $query->param('branch');
my $apo_com_short   = $query->param('apo_com_short');
my $lcversion       = $query->param('lcversion');
my $lcpool_count    = $query->param('lcpool_count');
my $apo_patch_level = $query->param('apo_patch_level');
my $changelist      = $query->param('changelist');
my $relstat         = $query->param('relstat');
my $p4client        = $query->param('p4client');
my $variant         = $query->param('variant');

my $text_opened = 0;
my $rc = undef;
my $protfile = undef;
my $script_name     = $ENV{'SCRIPT_NAME'};
my $remote_hostname = $query->server_name();
my @dirs_to_cleanup = $query->param('deldirs');
my $http_address ="http://$remote_hostname:$ENV{'SERVER_PORT'}";

chomp $remote_hostname;


unless (defined $type)
{
	$type = "lc";
}

#$lc_state = "DEV" unless (defined $lc_state);

my $userdir = "$type/";

if ( ! @cmdparams && ($cmdtargets || $cmdoptions))
{
	@cmdparams = ();
	push @cmdparams, split " ", $cmdoptions if ($cmdoptions);
	push @cmdparams, split " ", $cmdtargets if ($cmdtargets);
}

if ($type =~ /^lc$/)
{
	if (($release =~ /^(\d)\.(\d).(\d\d)\.(.*)$/) || ($release =~ /^(\d)\.(\d).([^.]*)$/))
	{
		$userdir .= "$1$2";
		my $corlevel = $3;
		$lc_state = $4;
		if (defined $lc_state)
		{	$userdir .= "$corlevel";	}
		else  # make version without corlevel -> 3rd "parameter" = lc_state
		{	$lc_state = $corlevel;		}
		unless ($lc_state =~ /DEV/i)
		{	$userdir .= "$lc_state"; }
		$userdir .= "_64" if (defined $bit64);
		$lc_state = "RAMP" if ($lc_state =~ /\d\d/);		
		$lc_state  =~ tr/a-z/A-Z/;
		$userdir .= "_$user" unless (defined $ignoreown);
	}
	if (($action =~ /start/) && ($version =~ /(.).*/))
	{
		$cmd = "im$1.pl";
	}		
}
elsif ($type =~ /^lcapps$/)
{
	$userdir .= "${branch}";
	$userdir .= "_64" if (defined $bit64);
	$userdir .= "_$user";
	$userdir .= "_$p4client" if (defined $p4client);
	$release = $branch;
	$variant = "Release" unless (defined $variant);
	#if ($variant =~ /Debug/)
	#{ unshift @cmdparams, "--config=Debug"; }
}
else
{
	_print_startline ("cgicall ($user)- Error : wrong type $type");	
	_error_exit ("Unknown type \"$type\" used");
}

	
		
my $own  ="$ENV{'DOCUMENT_ROOT'}/$userdir";
if($^O=~/win32/i)
{ $own  =~ tr/\//\\/; }

$ENV{'USER'} = defined $user ? "$user(remuser)" : "remuser";

unless ($action =~ /^start|show|stop|prot|clean$/ )
{
	_print_startline ("$release($user)- Error : wrong action $action");	
	_error_exit ("Unknown action \"$action\" used");
}

#$SIG{'KILL'}= \&_handle_stop_signal;
_print_dbg ("CMD: $cmd @cmdparams", 3); 

$rc = eval "${action}_make()";
_close_html();


######################

sub start_make
{
	_print_startline ("$release($user): $cmd @cmdparams");	
	_print_headline("Remote make");
	_check_path($own);
	# check for running make 
	if ( -f "$own/prot/build.pid" )
	{
		my @make_infos = _get_make_info("$own/prot/build.pid");
		_print_h3("An other make is running ($make_infos[0])");
		_print_h3("command: $make_infos[1]");
		$action="skip";
		if ($mode =~ /telnet/)
		{		
			print "To show current make use the remshow command !\n";
			print "To cancel current make use the remkill command !\n";
		}
		else
		{
			my $href = "${script_name}"._get_http_options();
			_print_nl ();
			print "<A HREF=$href&action=show>Show current make</A>";
			_print_nl (2);
			print "<A HREF=$href&action=stop>Cancel current make</A>";
			_print_nl (2);
			_print_link_to_own(1);
			_print_nl (1);
		}
		return;	
	}
	_begin_text();
	_write_pid_file ("$cmd @cmdparams");
	_end_text();
	
	if ( ( defined $oldprofile ) && ( ! -e "$own/$iprofile" ))
	{
		_print_h3 ("Can't find profile ($own/$iprofile) - force generation");
		_print_nl();
		$oldprofile = undef;
	}
	unless ( defined $oldprofile )
	{	
		_prepare_make(); 
		_print_hr();
	}
	
	# check for existing targets
	my ($tmp_version,$tmp_targets) = _analyze_cmdparams();
	if (@$tmp_targets)
	{
		# ready for starting make 
		_open_protfile("$own/prot/build.prot");
		_print_h3 ("Start make ($cmd @cmdparams)");
		_print_to_protfile("\nStart make ($cmd @cmdparams)\n");
		_begin_text();
		
		# create make command
		my $make_cmd;
		if ($^O =~/win/i)
		{
			if ( $type =~ /^lc$/)
			{ $make_cmd = "call \"$own\\iprofile.bat\" & cmd /c %TOOLSHELL% -S ";	}
			else
			{ $make_cmd = "call \"$own\\..\\iprofile.bat\" $own & cmd /c %TOOLSHELL% -S";	}
		}		
		else
		{ $make_cmd = ". $own/.iprofile ; \$TOOLSHELL -S "; }	
			
		# set environment variables for mail
		my $http_options = _get_http_options();
		$ENV{'HTTP_PROT_ADDRRESS'}="$http_address$script_name$http_options&cmd=$cmd";
		foreach (@cmdparams)
		{
			$ENV{'HTTP_PROT_ADDRRESS'} .= "&cmdparam=$_";
		}
		$ENV{'HTTP_SHOW_ADDRRESS'} = "$ENV{'HTTP_PROT_ADDRRESS'}&action=show";
		$ENV{'HTTP_PROT_ADDRRESS'} .= "&action=prot";
		$ENV{'HTTP_OWN_ADDRRESS'}="$http_address/$userdir";
		
		_print_dbg ("HTTP_PROT_ADDRRESS: \n $ENV{'HTTP_PROT_ADDRRESS'}", 2 );
		_print_dbg ("HTTP_SHOW_ADDRRESS: \n $ENV{'HTTP_SHOW_ADDRRESS'}", 2 );
		_print_dbg ("HTTP_OWN_ADDRRESS: \n $ENV{'HTTP_OWN_ADDRRESS'}", 2 );
		
		_print_dbg ("call: $make_cmd $cmd @cmdparams",2);
		open(MAKE,"$make_cmd $cmd @cmdparams |");
		while(<MAKE>)
		{
			_print_dual ("$_");
		}
		close MAKE;
		$rc = $?;
		_print_dbg("return value: $rc", 2);
		_print_to_protfile("Remote make finished ".( $rc!=0 ? "with errors" : "successfully"));
		_end_text();
		if ( $rc == 0 )
		{ _print_h3("Remote make finished successfully") ;	}
		else
		{ _print_h3("Remote make finished with errors", "red" );}
		_print_to_protfile("\nRemcall was finished\n");
		_close_protfile();
		_print_link_to_own(1);
		_print_links_to_protocols();
		_print_maketarget(1);
	}
	else
	{
		_print_maketarget();
		_print_link_to_own(1);
	}
	#_remove_pid_file();
	#return 0;
}


######################

sub show_make
{
	my $running_command = (_get_make_info("$own/prot/build.pid"))[1];
	if ( ! defined $running_command)
	{
		$running_command = "$cmd @cmdparams";
	}
	_print_startline ("$release($user): show make - $running_command");	
	_print_headline  ("Showing remote make");
	_check_path($own);
	
	_set_cmdparams($running_command);
		
	my $protfile = new FileHandle "$own/prot/build.prot", "r";
	my $timeout_counter = 0;
	my $line = "";
	if (defined $protfile)
	{
		_begin_text();
		while ( $line !~ /Remcall was finished/)
		{
			$line = <$protfile>;
			last if ($line =~ /Remcall was finished/);
			if (defined $line)
			{	
				$timeout_counter = 0;
				print ($line);
			}
			else
			{
				if ($timeout_counter > $timeout)
				{
					_error_exit("Timeout ($timeout s) for reading reached");
				}
				else
				{
					sleep 5;
					$timeout_counter += 5;
				}
			}
		}
		_end_text();
	}
	else
	{
		_print_h3 ("No make infomation found.");
	}
	_print_link_to_own();
	_print_links_to_protocols(1);
}

######################

sub prot_make
{
	my $running_command = (_get_make_info("$own/prot/build.pid"))[1];
	my @targets;
	my @options;
	_print_startline ("$release($user): protocols of remote make");	
	_print_headline  ("Protocols");
	_print_links_to_protocols();
	_print_link_to_own(1);
}


######################

sub stop_make
{
	my @make_infos = _get_make_info("$own/prot/build.pid");
	if (defined $make_infos[0])
	{
		_print_startline ("$release($user): stop make - $make_infos[1] - PID $make_infos[0]");	
		_print_headline  ("Canceling remote make");
		_stop_make($own);
	}
	else
	{
		_print_startline ("$release($user): stop make - can't find current make information");	
		_print_headline  ("Canceling remote make");
		_print_h3 ("Can't find make infomation. No make is running.");			
	}
	_print_link_to_own();
	_print_links_to_protocols(1);
}

######################

sub clean_make
{
	my ($version, $targets, $options)=_analyze_cmdparams();
	my $check = undef;
	my $all = undef;
	my $user_to_cleanup = $user;
	if ( @dirs_to_cleanup )
	{
		_print_startline ("$release($user): clean selected directories");
		_print_headline ("Removing selected make directories");	
	}		
	else
	{
		foreach (@$options)
		{
			$all = 1 if (/-all/);
			$check = 1 if (/-check/);
			if (/-user=(.*)$/)
			{
				$user_to_cleanup = $1;
			}
		}
		
		if ($all)
		{
			_print_startline ("$release($user): clean all of $user_to_cleanup");	
			_print_headline ("Removing make directories". ($check ? "($check)" : ""));	
			
			opendir MAKEDIR, "$own/..";
	    		my @found_dirs = grep {/_$user_to_cleanup/} readdir MAKEDIR;
			close DEVDIR; 		
			
			if ( ( $mode =~ /telnet/ ) || defined $check )
			{
				@dirs_to_cleanup = 	@found_dirs;
			}
			else
			{	
				print $query->startform (-method=>"get");
				print $query->checkbox_group(-name=>'deldirs',
									-values=>[@found_dirs],
									-defaults=>[@found_dirs],
									-linebreak=>'yes');
				_print_nl();
				print $query->hidden(-name=>'action',-default=>'clean');
				print $query->hidden(-name=>'user',-default=>$user);
				print $query->hidden(-name=>'debug',-default=>$debug) if (defined $debug);
				print $query->hidden(-name=>'type',-default=>$type);
				print $query->hidden(-name=>'release',-default=>$release);
				print $query->submit('Delete','Delete');
				print $query->submit('Cancel','Cancel');
				print $query->endform;
				return;
			}
		}
		elsif ( -d "$ENV{'DOCUMENT_ROOT'}/$userdir" )
		{
			_print_startline ("$release($user): $userdir");	
			_print_headline ("Removing make directory");	
			if ($userdir =~ /$type\/(.*)$/)
			{
				@dirs_to_cleanup = ($1);
			}
		}
		else
		{
			_print_startline ("$release($user): clean $userdir");	
			_print_headline ("Removing make directory");	
		}
	}	
	
	if ( @dirs_to_cleanup  &&  ! $query->param('Cancel') )
	{
		my @errors = ();
		my $count = 0;
		foreach (@dirs_to_cleanup)
		{
			my $own_to_cleanup = "$ENV{'DOCUMENT_ROOT'}/$type/$_";
			if ( $check )
			{
				_print_h3 ("To delete $own_to_cleanup");
			}
			else
			{
				_print_h3 ("Delete $own_to_cleanup ...");
				if (  -d $own_to_cleanup )
				{
					if (defined ((_get_make_info("$own_to_cleanup/prot/build.pid"))[0]))
					{
						_stop_make($own_to_cleanup);	
						sleep (2);	
					}
					_print_nl();
					if ( rmtree($own_to_cleanup, defined $debug ? 1 : 0  ) > 0)
					{	
						_print_h3 ("... $own_to_cleanup removed"); 
						$count++;
					}
					else
					{	
						_print_h3 ("... error while removing $own_to_cleanup");	
						push @errors, $own_to_cleanup;
					}
				}
				else
				{
					_print_h3 ("... $own_to_cleanup not found");		
				}
				_print_hr();
			}
		}
		_print_nl();
		_print_h3 ("$count make ". ( $count>1 ? "directories" : "directory" ). " deleted successfully") if ($count);
		if (@errors)
		{
			_print_h3 ("Deleting of ". (join ",", @errors) . "failed !"); 
		}
	}
	else
	{
		_print_h3 ("Nothing to delete ...");
	}
}


######################

sub _prepare_make
{
	# create a new profile
	
	_print_h3("Create a new profile");
	_begin_text();
	my $tooldir = $ENV{'TOOL'};
	if ($type =~ /^lc$/)
	{
		local @ARGV = ("-own", $own, "-tool", $tooldir, "-lc_state", $lc_state, "-vmake_path", "$own,$src_dir");
		push @ARGV, "-bit64"  if ( defined $bit64 );
		_print_dbg ("call $tooldir/bin/createlcprofile.pl @ARGV");
		do "$tooldir/bin/createlcprofile.pl";
	}
	else
	{
		local @ARGV=("-own", $own, "-lcversion", "$lcversion", "-apo_src", "$src_dir", "-apo_com_short", "$apo_com_short", "-remcall");
		push @ARGV, "-release", "$branch";
		push @ARGV, "-lcpool_count", "$lcpool_count" if ($lcpool_count);
		push @ARGV, "-apo_patch_level", $apo_patch_level if ($apo_patch_level);
		push @ARGV, "-bit","64" if ($bit64);
		push @ARGV, "-debugX" if (defined $debug);
		push @ARGV, "-type", "dbg" if ($variant =~ /Debug/);
		do "$tooldir/bin/vmakeEnv.pl";
		_print_dbg ("call vmakeEnv::createProfile");
		eval { vmakeEnv::createProfile("-all"); };
		if ($@)
		{
		  print ("Error:$@\n");
			die;
		}
	}
	
	my $localfiledir = "$ENV{'DOCUMENT_ROOT'}/$type";
	if (( $type =~ /^lcapps$/ ) && ($^O=~/win32/i) )
	{
		$localfiledir .= ( defined $bit64 ? "/bit64" : "/bit32" );
	}
	# copy iuser and ipreprof
	foreach my $localfile (@localfiles)
	{
		if (-e "$localfiledir/$localfile")
		{
			unlink ("$own/$localfile");
			_print_dbg ("copy $localfiledir/$localfile $own/$localfile", 2);
			copy ( "$localfiledir/$localfile", "$own/$localfile");
		}
	}
	
	if ($debug > 3)
	{
		print ("Current Settings after run iprofile:\n");
		system ("call \"$own\\iprofile.bat\" & set");
	}
	_end_text();
}

###############################
# stop all makes (lc, lcapps)
###############################
sub _stop_make
{
	my $own = shift;
	my @make_infos = _get_make_info("$own/prot/build.pid");
	my $rc = 0;
	_set_cmdparams($make_infos[1]);
	_print_h3 ("Stop current make ($make_infos[1])");
	_begin_text();
	if ( _kill_process($make_infos[0], $make_infos[1]) > 0 )
	{
		$rc ++;
	}
	#else
	#{
		unlink ("$own/prot/build.pid");		
	#}
	$rc += _kill_vmake($own);
	_end_text();
	if ( $rc > 0 )
	{
		_print_h3 ("Error while stopping running make");
	}
	else
	{
		_open_protfile(">$own/prot/build.prot");		
		_print_to_protfile("\n\nRemote make canceled by an other process");
		_print_to_protfile("\nRemcall was finished\n");
		_print_h3 ("Make stopped successfully");
		_close_protfile();
	}
	_print_dbg ("_stop_make returned with $rc", 3);
	return $rc;
}


######################

sub _analyze_cmdparams
{
	my @targets = ();
	my @options = ();
	my $version = "fast";
	my $ignore_default_variants = shift;
	if ($type =~ /^lc$/)
	{
		_print_dbg ("analyze '$cmd @cmdparams'",2); 
		if ($cmd =~ /i[pm]q/)
		{	$version = "quick"; }
		elsif ($cmd =~ /i[pm]s/)
		{	$version = "slow"; }
		_print_dbg ("Version is set to '$version'",2); 
		
		foreach my $param (@cmdparams)
		{
			_print_dbg ("Analyze param '$param'",2); 
			if ( $param =~ /^-/ )
			{ push @options, $param; }
			else
			{
				if ( $param =~ /^(.*)\/[\/-]/ )
				{	push @targets, $1;	}
				else
				{	push @targets, $param;	}
			}
		}
	}
	else
	{
		foreach my $param (@cmdparams)
		{
			_print_dbg ("Analyze param '$param'",2); 
			if ( $param =~ /^-/ )
			{ 
				push @options, $param; 
				if ($param =~ /^--config=(.*)$/)
				{
					$variant = $1;
				}
				if ($param =~ /^--noconfig$/)
				{
					$variant = undef;
				}
			}
			else
			{
				if ( $param =~ /^(.*)\/[\/-]/ )
				{	push @targets, $1;	}
				else
				{	push @targets, $param;	}
			}
		}
		if ((defined $variant) && ! $ignore_default_variants)
		{
			_print_dbg ("Append variant '$variant' to targetnames",2);
			for ( my $count=0; $count <= $#targets; $count++)
			{
				_print_dbg ("Append variant '$variant' to $targets[$count]",3); 
				if ($targets[$count] =~ /^(.*)\.([^\.]*)$/)
				{	$targets[$count] = "$1+${variant}.$2"; }	
				else
				{	$targets[$count] .= "+$variant";	}
				_print_dbg ("Target now $targets[$count]",3); 
			}
			
		}
	}
	return ($version, \@targets, \@options);
}

######################

sub _kill_process
{
	my $pid = shift;
	my $text = shift;
	if ( defined $text)
	{
		print "Stopping \"$text\" (PID: $pid) ... ";
		my $count = kill 9, $pid;
		if ($count >= 1 )
		{
			print "OK\n";
			return 0;
		}
		else
		{
			print "ERROR\n";
			return 1;
		}
	}
}

######################
#
sub _print_link_to_own
{
	my $seperator = shift;
	return if ($mode =~ /telnet/);
	my $old_text_opened = $text_opened;
	_end_text();
	_print_hr() if ($seperator);
	_print_h3("Make directory:   ".$query->a({-href => "/$userdir"},"$userdir"));
	_begin_text() if ($old_text_opened == 1);
}


######################
#
sub _print_links_to_protocols
{
	my $seperator = shift;
	return if ($mode =~ /telnet/);
	my $old_text_opened = $text_opened;
	_end_text();
	if ($seperator)
	{
		_print_nl();
		_print_hr() 
	}
	_print_h3("Protocols:");
	my $targets;
	my $options;
	my $version;
	($version, $targets, $options)=_analyze_cmdparams();
	
	print $query->start_table({-border => '1', -cellpadding=>5} );
		
	#print "<TABLE with=100% cellpadding=10>";
	foreach my $target (@$targets)
	{
		my $name = $target;
		my $prot = "sys/wrk/$version/prot/";
		if ($target =~ /::?(.*)/)
		{	$prot .= "$1"; }
		else
		{	$prot .= "$target" };
		# description without extension
		# -> look for <extension>.e0
		if ( (($name =~ /^::/) || ($name !~ /\//)) && ($name !~ /\./))
		{
			_print_dbg ("found description without extension", 1 );
			foreach (@desc_extensions)
			{
				_print_dbg ("check description for $_", 2 );
				if ( -f "$own/$prot.$_.p0" )
				{
					$target .= ".$_";
					$prot   .= ".$_";
					last;
				}
			}
		}
		print $query->Tr($query->th({-align => 'right'},"$target:"),
			  $query->td( -e "$own/$prot.e0" ? $query->a({-href => "/$userdir/$prot.e0"},'Error protocol') : "Error protocol not found"),
			  $query->td( -e "$own/$prot.p0" ? $query->a({-href => "/$userdir/$prot.p0"},'Normal protocol') : "Normal protocol not found"),
			  $query->td( -e "$own/$prot.x0" ? $query->a({-href => "/$userdir/$prot.x0"},'Extended protocol') : "Extended protocol not found"));			
	}
	print $query->end_table();
	print "</TABLE>";
	_begin_text() if ($old_text_opened == 1);
}

######################
sub _print_maketarget
{
	my $seperator = shift;
	return if ($mode =~ /telnet/);
	my ($version, $targets, $options)=_analyze_cmdparams(1);
	my $old_text_opened = $text_opened;
	_end_text();
	_print_hr() if ($seperator);
	_print_h3("Restart make");
	print $query->startform (-method=>"get");
	print "Config:  ";
	if ($type =~ /lcapps/)
	{
		print $query->radio_group(-name=>'variant',
	                              -values=>['Release','Debug'],
	                              -default=>'Release');
	}
	else
	{
		print $query->radio_group(-name=>'version',
	                              -values=>['fast', 'quick', 'slow'],
	                              -default=>$version);
    }
	_print_nl(2);                   
	print "Options:  ";
	print $query->textfield(-name=>'cmdoptions', 
	                        -override=>1,
	                        -size=>50,
	                        -default=>join " ", @$options);
	_print_nl(2);						
	print "Targets:  ";					
	print $query->textfield(-name=>'cmdtargets', 
	                        -override=>1,
	                        -size=>50,
	                        -default=>join " ", @$targets);
	_print_nl(2);						
	if ($type =~ /^lc$/)
	{
		if (defined $oldprofile)
		{
			$query->param(-name=>'oldprofile',-value=>'ON');
			print $query->checkbox(-name=>'oldprofile',
			                       -checked=>'checked',
			                       -value=>'ON',
			                       -label=>"  Don't generate a new profile");	
		}	
		else
		{
			print $query->checkbox(-name=>'oldprofile',
			                       -label=>"  Don't generate a new profile");	
		}							
		_print_nl(2);
	}
	print $query->hidden(-name=>'action',-default=>'start');
	print $query->hidden(-name=>'user',-default=>$user);
	print $query->hidden(-name=>'debug',-default=>$debug) if (defined $debug);
	print $query->hidden(-name=>'type',-default=>$type);
	print $query->hidden(-name=>'cmd',-default=>"lcmake");
	print $query->hidden(-name=>'src_dir',-default=>$src_dir);
	print $query->hidden(-name=>'bit64',-default=>$bit64) if (defined $bit64);
	print $query->hidden(-name=>'platform',-default=>$platform) if (defined $platform);
	if ($type =~ /^lc$/)
	{
		print $query->hidden(-name=>'release',-default=>$release);
		print $query->hidden(-name=>'lc_state',-default=>$lc_state) if (defined $lc_state);
		print $query->hidden(-name=>'ignoreown',-default=>$ignoreown) if (defined $ignoreown);
	}
	else
	{
		print $query->hidden(-name=>'branch',-default=>$branch);
		print $query->hidden(-name=>'apo_com_short',-default=>$apo_com_short);
		print $query->hidden(-name=>'lcversion',-default=>$lcversion);
		print $query->hidden(-name=>'lcpool_count',-default=>$lcpool_count) if (defined $lcpool_count);
		print $query->hidden(-name=>'apo_patch_level',-default=>$apo_patch_level) if (defined $apo_patch_level);
		print $query->hidden(-name=>'changelist',-default=>$changelist) if (defined $changelist);
		print $query->hidden(-name=>'relstat',-default=>$relstat) if (defined $relstat);
		print $query->hidden(-name=>'p4client',-default=>$p4client) if (defined $p4client);
	}
	
	print $query->submit('Start','Start');
	print $query->endform;
	_begin_text() if ($old_text_opened == 1);
	return;
}


######################
# write output to $protfile and stdout
sub _print_dual
{
	my $text = shift;
	print "$text";
	_print_to_protfile ($text);
}

######################

sub _write_pid_file
{
	my $text     = shift;
	my $filename = "$own/prot/$pid_file_name";
	_print_dbg ("write pidfile $filename", 2);
	my $fh = new FileHandle ">$filename";
	if (defined $fh)
	{
		$fh->print("$$\n");
		$fh->print("$text\n") if (defined $text);
		$fh->close;
	}
	else
	{
		_error_exit("Can't open $filename");
	}
}

######################

sub _open_protfile
{
	my $filename = shift;
	
	$protfile = new FileHandle (">$filename");
	$protfile->autoflush(1);
	unless (defined $protfile)
	{
		_error_exit("can't open $filename for writing");
	};
}

######################

sub _close_protfile
{
	$protfile->close();
}

######################

sub _get_make_info
{
	my $file = shift;
	my ($pid,$command);
	my $fh = new FileHandle "$file", "r";
	if ( defined $fh )
	{
		$pid = <$fh>;
		chomp $pid;
		$command = <$fh>;
		chomp $command;
	}
	return ($pid,$command);
}

######################

sub _kill_vmake
{
	my $own = shift;
	my $pidfile = "$own/sys/wrk/vmake.pid";
	my $pid = undef;
	my $fh = new FileHandle "$pidfile", "r";
	my $rc = 1;
	if ( defined $fh )
	{
		$pid = <$fh>;
		chomp $pid;
	}
	$fh->close;
	if (defined $pid)
	{
		$rc = _kill_process ( $pid, "vmake" );
		if ( $rc == 0 )
		{ 
			_print_dbg ("unlink '$pidfile'", 2);
			sleep 1;
			unlink "$pidfile"; 
		}
	}
	else
	{
		print "Error: can't get process infomation of vmake\n";
	}
	return ($rc);
}

######################
#
sub _set_cmdparams
{
	my $make_infos = shift;
	if (defined $make_infos)
	{
		# init $cmd and @cmdparams with running make
		@cmdparams = split ' ', $make_infos;
		$cmd = shift @cmdparams;	
	}
	return;
}

######################

sub _get_http_options
{
	my $action=shift;
	my $http_options = "?user=$user&type=$type";
	$http_options .= "&bit64=1" if (defined $bit64);
	$http_options .= "&debug=$debug" if (defined $debug);
	$http_options .= "&platform=$platform" if (defined $platform);
	if ($type =~ /^lc$/)
	{	$http_options .= "&release=$release&lc_state=$lc_state"; }
	else
	{
		$http_options .= "&branch=$branch";
		$http_options .= "&p4client=$p4client" if (defined $p4client);
	}
	return ($http_options);
}

######################

sub _close_html
{
	if ($mode =~ /telnet/)
	{
		_print_dual ("Remcall was finished");
	}
	else
	{
		_print_hr();
		my $responsible_text = '<p>Responsible for problems and questions: <a href="mailto:gerald.arnold@sap.com';
		#$responsible_text .= '?cc=ulrich.jansen@sap.com,falko.flessner@sap.com';
		my $body = '?body=%0D%0A%0D%0AImportant infomations for supporting: %0D%0Acurrent action: '."$action";
		if ( $action =~ /start/i)
		{
			$body .= '%0D%0Ashow make: '.$ENV{'HTTP_SHOW_ADDRRESS'}. '&action=show';
			$body .= '%0D%0Aown: '.$ENV{'HTTP_OWN_ADDRRESS'};
		}
		else
		{
			$body .= '%0D%0Aown: '."http://$ENV{'HTTP_HOST'}/$userdir";
		} 
		$body .= '%0D%0Acomplete call: '.$query->url(-query => 1);
		$responsible_text .= _to_mailformat ("$body");
		$responsible_text .= '%0D%0A">Team ESW </a></p>';
		print "$responsible_text\n";
		print $query->end_html;
	}
	if ($action =~ /start/)
	{
		unlink "$own/prot/$pid_file_name";
	}
}

######################

sub _begin_text
{
	if ( $text_opened == 0 )
	{
		$text_opened = 1;
		print "<PRE>\n" unless ($mode =~ /telnet/);
	}
}

######################

sub _end_text
{
	if ( $text_opened != 0 )
	{
		$text_opened = 0;
		print "</PRE>\n" unless ($mode =~ /telnet/);
	}
}


######################

sub _print_h2
{
	my $text = shift;
	my $color = shift;
	if ($mode =~ /telnet/)
	{
		print ("$text\n");
		print ("=" x length ($text) );
		print "\n";
	}
	else
	{
		if (defined $color)
		{
			print $query->h2({-style=>"Color: $color;"},"$text");
		}
		else
		{
			print $query->h2("$text");
		}
	}
}


######################

sub _print_h3
{
	my $text = shift;
	my $color = shift;
	if ($mode =~ /telnet/)
	{
		print ("$text\n");
	}
	else
	{
		if (defined $color)
		{
			print $query->h3({-style=>"Color: $color;"},"$text");
		}
		else
		{
			print $query->h3("$text");
		}
	}
}

######################

sub _print_nl
{
	my $count = shift;
	$count = 1 unless (defined $count);
	while ($count > 0)
	{
		if (($mode =~ /telnet/) || $text_opened )
		{	print "\n";	}
		else
		{	print "<BR>\n";	}
		$count--;
	}
}

#######################

sub _print_hr
{
	_print_nl();
	if (($mode =~ /telnet/) || $text_opened )
	{	print "_____________________________________________________________________\n";	}
	else
	{	print "<HR>\n";	}
	_print_nl();
}


######################

sub _print_startline
{
	my $text = shift;
	if ($mode =~ /telnet/)
	{
		print "Remcall was started\n";
		my $len = length ($text) + 2;
		print ("\n+".("-" x $len)."+" );
		print ("\n| $text |\n");
		print ("+".("-" x $len)."+\n\n" );
	}
	else
	{
		print $query->start_html("$text ($remote_hostname)");
		print "<meta name=\"pragma\" content=\"no-cache\">\n";
	}
}

#######################

sub _print_headline
{
	my $action = shift;
	_print_h2("$action of ". 
	           ($type =~ /^lc$/ ? "$release ($lc_state)" : "$branch" ) . " on $remote_hostname". 
                   ( defined $platform ? "($platform)" : "" ) );
	_print_hr();
}
  

######################

sub _print_to_protfile
{
	my $text = shift;
	if (defined $protfile)
	{
		print $protfile "$text";
	}
	else
	{
	# recursion	_error_exit ("can't write to protocol file ($own/prot/build.prot)");
	}
}

######################

sub _error_exit 
{
	my $text = shift;
	_print_dbg("error_exit: $text", 2 );
	_end_text();
	
	if ($action =~ /start/)
	{
		unlink "$own/prot/$pid_file_name";
	}
	_print_h3("Error: $text", "red");
	_close_html();
	exit 0;
}

######################

sub _check_path
{
	my $own = shift ;
	my $old_text_opened = $text_opened;
	_begin_text();
	mkpath("$own", 0775) unless (-d "$own");
	mkpath("$own/prot", 0775) unless (-d "$own/prot");
	_end_text() if ($old_text_opened == 0);
}

######################

sub _print_dbg
{
	my $text = shift;
	my $dbg_level = shift;
	my $old_text_opened = $text_opened;
	$dbg_level = 1  unless (defined $dbg_level);
	_begin_text();
	if ($debug >= $dbg_level)
	{
		print ("DBG$dbg_level: $text\n");
	}
	_end_text() if ($old_text_opened == 0);
}

#####################

sub _handle_stop_signal
{
	print ("Get kill signal ...\n");
	_end_text();
	_print_nl();
	_print_h3 ("Make canceled");
	_print_nl();
	_close_html();
	exit 1;
}

sub _to_mailformat
{
	my $text = shift;
	$text =~ s/\&/%26/g;
	$text =~ s/ /%20/g;
	return ($text);
}
