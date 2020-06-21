#!/devtool/local/bin/perl
#
# LCAMakePreCheck.pm
#
#
# return values:
#
# -1 -> decline start / not allowed
#  0 -> not neccessary but allowed
#  1 -> start neccessary

package LCAMakePreCheck;
use qadb ;
use qadblca;
use strict;

my $lcmakeid = $ENV{LCMAKEID};
my $lcamakeid = undef;
my $force_make = 0;
my $type = "opt";
my $branch_info = $ENV{BRANCH_INFO};
my $lca_state = "RAMP";
my @LCAPPSDIRS=();
my $last_lcappsdir = undef;
my $qah = undef;
my $qahlca = undef;
my $changelist = undef;
my $PreCheckMessage="";


#######################################################
# print info line of check name                       #
#######################################################
sub _print_precheck_header
{
	my $line = shift;
	print "\n---------------------------------------------------\n";
	print ("\n$line\n");
}


######################################################
# open handle to qadb - reopen if different lcmakeid #
######################################################
sub _open_qa_handle
{
	my $lcmakeid = shift;
	unless ( (defined $qah) && ( $lcmakeid == $qah->{'ID'} ))
	{	 
		# check for LC_OK of liveCache
		if ( $lcmakeid == 1 )
		{	return _force_make ("No lcmakeid found.", 1 );	}
		else
		{
			$qah=qadb->new({'ID'=>$lcmakeid});
			if ( $qah->{error_code} != 0 )
			{	return _force_make ( "ERROR: can't get info for given lcmakeid (lcmakeid=$lcmakeid):\n"."       ".$qah->{error_text}, 1) ; }
		}
	}
	return (0);
}


######################################################
# open handle to qadb - reopen if different lcmakeid #
######################################################
sub _open_qalca_handle
{
	my $lcamakeid = shift;
	unless ( (defined $qahlca) &&  ( $lcamakeid == $qahlca->{'ID'} ) )
	{ 
		$qahlca = qadblca -> new ( {'ID' =>"$lcamakeid"}); 
		if ( $qahlca->{error_code} != 0 )
		{	return _force_make ( "ERROR: can't get info for given makeid (makeid=$lcamakeid):\n"."       ".$qahlca->{error_text}, 1) ; }
	}
	return (0);	
}


###########################################
# read changelist from $OWN/changelist.log
###########################################
sub _get_current_changelist
{
	my $current_cl = undef;
	if ( open ( CHANGELIST_LOG , "$ENV{OWN}/changelist.log" ))
	{
		if ( $current_cl = <CHANGELIST_LOG> )
		{      chomp ( $current_cl ) ;   }
	}
	close ( CHANGELIST_LOG ) ;
	return $current_cl;
}


###########################################
# read changelist from $OWN/changelist.log
###########################################
sub _get_makeid
{
	my $dir = shift;
	my $makeid = undef;
	$dir = $ENV{'OWN'} unless (defined $dir);
	if ( open ( MAKE_ID , "$dir/make.id" ))
	{
		if ( $makeid = <MAKE_ID> )
		{      chomp ( $makeid ) ;   }  
	}
	close ( MAKE_ID ) ;
	return $makeid;
}


###########################################
# force or reject make with output
###########################################
sub _force_make
{
	$PreCheckMessage = shift;
	my $ret = shift;
	print "\n$PreCheckMessage\n\n";
	print "\n===================================================\n";
	return ($ret,$PreCheckMessage);
}


###########################################
# check for purpose of livecache
###########################################
sub _check_lc_purpose
{
	(_open_qa_handle (@_))[0] == 0 || return (1, $PreCheckMessage );
	# only run 
	unless ($qah->{'BUILD_PURPOSE'} =~/A|L/)
	{	return _force_make ("SAPDB/liveCache Build (lcmakeid=$qah->{'ID'}) is not released for livecache", -1);	}
	return ( _force_make ("SAPDB/liveCache Build (lcmakeid=$qah->{'ID'}) is released for livecache", 0 ));
}


###############################################
# check the changelist from the last make
###############################################
sub _check_changelist
{
	my $changelist = _get_current_changelist();
	$changelist || return _force_make (  "WARNING: can\'t read changelist number!", 1 );
	(_open_qalca_handle (@_))[0] == 0 || return (1, $PreCheckMessage );
	if ( $changelist == $qahlca->{'CHANGELIST'} )
	{	return _force_make (  "This LCAPPS build was already started with changelist $changelist (makeid=$qahlca->{'ID'})", 0 );	}
	else
	{	return _force_make (  "Found only build with older changelist $qahlca->{'CHANGELIST'} - Makeid makeid=$qahlca->{'ID'}", 1 ); }
}


###############################################
# check the changelist from the last make
###############################################
sub _get_IDOBJSTATUS
{
	(_open_qalca_handle (@_))[0] == 0 || return ( -1 );
	return ($qahlca->{'IDOBJSTATUS'});
}

###############################################
# precheck  for all -> start make if
#   1. new changelist
#   2. new livecache build 
#  => for extended cor-cor make
###############################################
#sub precheck_all
#{
#	_print_precheck_header ("Precheck: all (changelist,livecache_build)\n");
	
#	my $lcamakeid = _get_makeid ();
#	unless ( $lcamakeid )
#	{ return _force_make ( "Can't find old makeid infomation in $ENV{'OWN'}/make.id", 1 ); }
#	return ( _check_changelist( $lcamakeid ) );
#}

###############################################
# check the changelist from the last make
###############################################
sub precheck_changelist
{
	_print_precheck_header ("Precheck: changelist\n");
	
	my $lcamakeid = _get_makeid ();
	unless ( $lcamakeid )
	{ return _force_make ( "Can't find old makeid infomation in $ENV{'OWN'}/make.id", 1 ); }
	return ( _check_changelist( $lcamakeid ) );
}


######################################################################
# force run if changelis is modified or the other make was canceled
######################################################################
sub precheck_changes
{
	_print_precheck_header ("Precheck: changes\n");
	
	my $lcamakeid = _get_makeid ();
	unless ( $lcamakeid )
	{ return _force_make ( "Can't find old makeid infomation in $ENV{'OWN'}/make.id", 1 ); }
	(_check_changelist( $lcamakeid ))[0] == 0 || return (1, $PreCheckMessage );
	my $objstatus = _get_IDOBJSTATUS ( $lcamakeid );
	if ( $objstatus == 990 )
	{	
		return _force_make (  "A LCAPPS build (makeid=$lcamakeid) with the same changelist was cancled", 1 );
	}
	elsif ( $objstatus == 999 )
	{
		return _force_make (  "A LCAPPS build (makeid=$lcamakeid) with the same changelist was already made - with errors", 0 );
	}
	elsif ( $objstatus > 999 )
	{
		return _force_make (  "A LCAPPS build (makeid=$lcamakeid) with the same changelist was already made", 0 );	
	}
	else
	{
		return _force_make ( "Make (makeid=$lcamakeid) already running", -1); 
	}
}

###########################################
# profile livecache_build                
# build only , if to build against a new    
# liveCache build                        
###########################################
sub precheck_livecache_build
{
	_print_precheck_header ("Precheck: livecache_build\n");

	$lcamakeid = _get_makeid ();
	unless ( $lcamakeid )
	{ return _force_make ( "Can't find old makeid infomation in $ENV{'OWN'}/make.id", 1 ); }
	
	(_open_qalca_handle ($lcamakeid))[0] == 0 || return (1, $PreCheckMessage );
	
	if ( $ENV{'LCMAKEID'} == $qahlca->{'IDMAKE'} )
	{	return _force_make (  "This LCAPPS build was already made with the livCache build (lcmakeid=$ENV{'LCMAKEID'})", 0 );	}
	else
	{	return _force_make (  "Found only build with older liveCache build (lcmakeid=$ENV{'LCMAKEID'})", 1 ); }
}


###############################################
# check the changelist from the last make
###############################################
sub precheck_lc_purpose
{
	_print_precheck_header ("Precheck:  lc_purpose\n");
	
	# check for LC_OK of liveCache
	if ( $lcmakeid == 1 )
	{	return _force_make ("No lcmakeid found", 1 );	}
	return ( _check_lc_purpose( $lcmakeid ) );
}


###########################################
# profile livecache_cor
# build only if:
# - new liveCache COR with LC_OK
# or 
# - not yet build an other LCAPPS build 
#   with this lC COR successfully
###########################################
sub precheck_livecache_cor
{
	my $major_number = "";
	_print_precheck_header ("Check profile: livecache_cor\n");
	
	# check for LC_OK of liveCache
	if ( $lcmakeid == 1 )
	{	return _force_make ("No lcmakeid found", 1 );	}
	else
	{
		my $check_purpose = (_check_lc_purpose( $lcmakeid ))[0];
		$check_purpose == 0 || return ($check_purpose, $PreCheckMessage );
	}
	
	# look for older lcabuilds 
	if ($ENV{BRANCH_INFO} =~ /.*(DEV|COR|RAMP)$/)
	{ $lca_state = $1; }
	
	if ($ENV{BRANCH_INFO} =~ /^(\d)\.(\d)/)
	{ $major_number = "$1$2"; }
	
	opendir(LCOWN, "$ENV{'LC_OWN'}") || die "can't opendir $ENV{LC_OWN}: $!\n";
	@LCAPPSDIRS = sort grep /^LCAPPS_$major_number.*_${type}_$lca_state$/i, readdir(LCOWN);
	closedir(LCOWN);
	$last_lcappsdir = "$ENV{'LC_OWN'}/$LCAPPSDIRS[0]";
	
	# check the last build
	if (@LCAPPSDIRS)
	{
		if($^O=~/.*win32.*/i)
		{
			require WinLink;
			import WinLink;
			$last_lcappsdir .= "/". readLink ("$last_lcappsdir/LastBuild");
		}
		else
		{	$last_lcappsdir .= "/". readlink "$last_lcappsdir/LastBuild"; }
	}
	else
	{
		$last_lcappsdir = $ENV{'OWN'};
	}
	
	$lcamakeid = _get_makeid ("$last_lcappsdir");
	unless ( defined $lcamakeid )
	{ return  (1, "No makeid in $last_lcappsdir found\n"); }
	
	(_open_qalca_handle ($lcamakeid))[0] == 0 || return (1, $PreCheckMessage );
	if ( $qahlca->{'IDMAKE'} != $lcmakeid )
	{	return _force_make (  "Last COR-COR make ran with an other liveCache build", 1 );	}

	if ( $qahlca->{'IDOBJSTATUS'} == 999 )
	{
		$changelist = _get_current_changelist();
		$changelist || return _force_make (  "WARNING: can\'t read changelist number!", 1 );
		if ( $changelist == $qahlca->{'CHANGELIST'} )
		{
			return _force_make (  "A LCAPPS build (makeid=$lcamakeid) with the same changelist was already made with this livecache (with errors)", 0 );
		}
		else
		{	return _force_make (  "Found only old LCAPPS build (makeid=$lcamakeid) with errors", 1 );	}
	}
	elsif ( $qahlca->{'IDOBJSTATUS'} == 990 )
	{
		return _force_make ( "Found only canceled LCAPPS build (makeid=$lcamakeid)", 1 );
	}
	elsif ( $qahlca->{'IDOBJSTATUS'} > 999 )
	{	return _force_make ( "An other LCAPPS build (makeid=$lcamakeid) was already made successfully with this liveCache", 0); }
	else
	{	return _force_make ( "Make (makeid=$lcamakeid) already running", 0); }
}


#######################################################
sub Usage
{
	print "Following precheck profiles are possible:\n";
	print "       - livecache_cor    : check COR<->COR\n";
	print "       - changelist       : check changelist\n" ;
	print "       - changes          : check necessity of make\n";
	print "       - lc_purpose       : check purpose for livecache\n";
	print "       - livecache_build  : check liveCache-ID\n";
}

1;