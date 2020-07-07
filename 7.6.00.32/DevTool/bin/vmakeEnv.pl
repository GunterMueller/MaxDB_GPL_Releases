#!/usr/bin/perl5

use File::Path;
use File::Basename;
use Cwd;
package vmakeEnv;

$| = 1;
init();

# constants
my $BAS_TREE                = "/bas";
my $SRC_TREE                = "/bas/LCAPPS/";
my $LC_BUILD_SELECT_TAG     = "LCSelect";
my $LCVERSION_TAG           = "LCVersion";

if($^O=~/.*win32.*/i)
{ 
	$OWN_ROOT="C:\\APOMAKE\\"; 	
	$LOCAL_OWN_ROOT="C:\\APOMAKE\\"; 	
	$PROFILE_NAME="iprofile.bat";
	$bit="32";
	my $platformtag_on_lcpool = getPlatform();
	if ($platformtag_on_lcpool =~ /^NTx86_64$/i)
	{ $platformtag_on_lcpool = "ntamd64"; }
	$LC_POOL = ($ENV{'LC_POOL_ROOT'} =~ /\S/) ? $ENV{'LC_POOL_ROOT'} : "\\\\production\\SCM\\MaxDB\\LC_POOL\\$platformtag_on_lcpool" ;
	
}
else
{ 
	$OWN_ROOT="/bas/LCAPPS"; 
	$LOCAL_OWN_ROOT="/LCAPPS"; 
	$bit="64";	
	$PROFILE_NAME=".iprofile" ;
	$LC_POOL="/bas/SAP_DB";
}

@ALL_VARS = ("OWN","VMAKE_DEFAULT_VARIANT","BRANCH_INFO","LC_OWN","APO_SRC","LC_STATE","RELVER","CORRECTION_LEVEL","BUILDPRAEFIX",
			 "VMAKE_PATH","LCVERSION","LCROOT","APO_COM_RELEASE","APO_COM_BUILD","APO_COM_SHORT","APO_PATCH_LEVEL","LCMAKEID","LC_POOL_ROOT");

vmakeEnvOptions(@ARGV);


1;


sub init
{
	undef $release;
	undef $type;
	undef $platform;
	undef $lc_version;
	undef $lc_buildpref;
	undef $formated_buildpref;
	undef $lc_relver;
	undef $lc_major;
	undef $lc_minor;
	undef $lc_corlevel;
	undef $lc_formated_corlevel;
	undef $lc_own;
	undef $own;
	undef $apo_src;
	undef $branch_info;
	undef $vmake_default_variant;
	undef $apo_com_release;
	undef $apo_com_build;
	undef $apo_com_short;
	undef $apo_patch_level;
	undef $lcversion;
	undef $profile_name;
	undef $makedbv;
	undef $lcpool_count;
	undef $remotecall;
	undef $config_xml_tree;
	undef $lc_state;
	undef $local_own;
	undef $debug;
	undef $ext_debug;
	undef $formated_lcversion;
	undef $src_dir;
	undef $lcmakeid;
	undef $lc_pool_root;
	undef $no_src_dir;
	undef $vmake_path;
	undef $forced_lc_state;
	$write_profile=0;	
	vmakeEnvOptions(@_);
}

sub vmakeEnvOptions
{
	local @args=@_;
	while (@args) 
	{
	  my $arg = shift @args;
  	  
	  if ($arg eq "-apo_com_short") 
	  {
	    $apo_com_short = shift @args; 
	    next;
	  }
	  if ($arg eq "-apo_patch_level") 
	  {
	    $apo_patch_level = shift @args; 
	    next;
	  }	  
	  if ($arg eq "-apo_src") 
	  {
 	    $apo_src = shift @args; 
 	    # workaround for iniAPO_COM.cmd
 	    if ( $apo_src =~ /\.\./ )
 	    { 
 	    	local $curr_dir = Cwd::cwd();
 	    	chdir ($apo_src);
 	    	$apo_src = Cwd::cwd();
 	    	chdir ($curr_dir);
 	    }
	    next;
	  }
	  if ($arg eq "-bit") 
	  {
	    $bit = shift @args; 
	    next;
	  }
	  if ($arg eq "-makedbv")
	  {
	      $makedbv = shift @args;
	      next;
	  }
	  if ($arg eq "-lcpool_count")
	  {
	      $lcpool_count = shift @args;
	      next;
	  }
   	if ($arg eq "-lc_state")
	  {
	      $lc_state = shift @args;
	      $lc_state =~ tr/a-z/A-Z/;
	      next;
	  }
	  if ($arg eq "-lcversion") 
	  {
	    $lcversion = shift @args; 
	    next;
	  }
	  if ($arg eq "-lc_own") 
	  {
	    $lc_own = shift @args; 
	    next;
	  }
	  if ($arg eq "-local_own")
	  {
	      $local_own=1;
	      next;
	  }
	  if ($arg eq "-own")
	  {
	      $own = shift @args;
	      next;
	  }
	  if ($arg eq "-platform")
	  {
	      $platform = shift @args;
	      next;
	  }
  	  if ($arg eq "-profile_name")
	  {
	      $profile_name = shift @args;
	      next;
	  }
	  if ($arg eq "-release") 
	  {
	    $release = shift @args; 
	    next;
	  }
	  if ($arg eq "-remcall")
	  {
	      $remcall=1;
	      next;
	  }
	  
	  if ($arg eq "-no_src_dir") 
	  {
 	    $no_src_dir = 1;
 	    next;
 	  }
 	  	  
	  if ($arg eq "-src_dir") 
	  {
 	    $src_dir = shift @args; 
 	    # workaround for iniAPO_COM.cmd
 	    if ( $src_dir =~ /\.\./ )
 	    { 
 	    	local $curr_dir = Cwd::cwd();
 	    	chdir ($src_dir);
 	    	$src_dir = Cwd::cwd();
 	    	chdir ($curr_dir);
 	    }
	    next;
	  }
	  if ($arg eq "-lc_pool_root")
	  {
	      $lc_pool_root = shift @args;
	      next;
	  }
	  if ($arg eq "-type")
	  {
	      $type = shift @args;
	      next;
	  }
	  if ($arg =~ /^-debug(.*)$/ )
	  {
	  	  $debug = 1;
	  	  if ($1 =~ /^X$/ )
	  	  {
	  	  	$ext_debug=1;
	  	  }
	  	  next;
	  }
	  
	  die "Error: $arg unknown option\n";
	}	
}

sub setOWN
{
	$ENV{OWN} = getOWN ( @_ );
}

sub getOWN
{
	unless (defined $own)
	{
		vmakeEnvOptions(@_);
		check_vmakeVariables("release","type");
		get_lc_build();
		if (defined $local_own)
		{
			$own = "$LOCAL_OWN_ROOT/";
			$own.= ($release =~ /^(DEV|CONS)$/i)?lc($1):$release;
			$own.= "/${lc_relver}$lc_formated_corlevel/".getPlatform()."/gen$type";
		}
		else
		{	
			$own = "$OWN_ROOT/$release/${lc_relver}$lc_formated_corlevel/gen/$type/".getPlatform();
		}
	}
	$own = convertFileString($own);
	print "Info: getOWN returned with $own\n" if (defined $debug);
    return ($own);
}

sub setBRANCH_INFO
{
	$ENV{BRANCH_INFO} = getBRANCH_INFO ( @_ );
}

sub getBRANCH_INFO
{
	unless (defined $branch_info)
	{
		vmakeEnvOptions(@_);
		check_vmakeVariables("release");
		$branch_info=$release;
		$branch_info=~s/^(\d)([^.].*)$/$1.$2/;
	}
	print "Info: getBRANCH_INFO returned with $branch_info\n" if (defined $debug);
    return ($branch_info);
}

sub setVMAKE_DEFAULT_VARIANT
{
	$ENV{VMAKE_DEFAULT_VARIANT} = getVMAKE_DEFAULT_VARIANT ( @_ );
}

sub getVMAKE_DEFAULT_VARIANT
{
	unless (defined $vmake_default_variant)
	{
		vmakeEnvOptions(@_);
		$vmake_default_variant="Release";
		$vmake_default_variant="Debug" if ($type =~ /^dbg$/i);
	}
	print "Info: getVMAKE_DEFAULT_VARIANT returned with $vmake_default_variant\n" if (defined $debug);
    return ($vmake_default_variant);
}

sub setLC_OWN
{
	$ENV{LC_OWN} = getLC_OWN ( @_ );
}

sub getLC_OWN
{
	my $found_lcown = 0;
	my @lc_states_to_check = ();
	unless (defined $lc_own)
	{
		vmakeEnvOptions(@_);
		get_lc_build();
		local $lcpoolroot=getLC_POOL_ROOT();
		print "InfoX: lock for lc_own of ${lc_major}.${lc_minor}.${lc_formated_corlevel}.${lc_formated_buildpref}\n" if (defined $ext_debug);
		print "Info: LC_POOL_ROOT=$lcpoolroot\n" if (defined $debug);
		if (defined $lc_state)
		{
			$lc_own = $lcpoolroot."/LC_${lc_relver}${lc_formated_corlevel}${lc_formated_buildpref}_${bit}_$lc_state/";
			$lc_own.=  substitute_BuildSelect("${lc_own}LastBuild");
		}
		else
		{
			# x.xx.xx.(ramp|hot)
			if ( $lc_formated_buildpref =~ /^(ramp|hot)$/i )
			{
			
				# get version of last ramp
				local $default_lcstate = $1;
				$default_lcstate =~ tr/a-z/A-Z/; 
				$lc_state = $default_lcstate;
				opendir(DIR, "$lcpoolroot") || die "can't opendir $lc_own: $!\n";
				local @ramp_builds = sort {$b cmp $a} grep { /^LC_${lc_relver}${lc_formated_corlevel}\d\d_${bit}_$default_lcstate/ } readdir(DIR);
				closedir DIR;
				# the first entry should be the latest ramp
				if ( $ramp_builds[0] =~ /^LC_${lc_relver}${lc_formated_corlevel}(\d\d)_${bit}_$default_lcstate/ )
				{
					$lc_own = $lcpoolroot."/$ramp_builds[0]";
					$lc_formated_buildpref = $1;
					$lc_buildpref = $lc_formated_buildpref =~ /^0(\d)$/ ? $1 : $lc_formated_buildpref;				
					$lcversion="SAPDB_${lc_relver}${lc_corlevel}_b${lc_formated_buildpref}";
					$lc_state=$default_lcstate;
					$lc_own.="/".substitute_BuildSelect("${lc_own}/LastBuild");
					$found_lcown = 1;
				}
				else
				{
					die "can't find livecache $default_lcstate builds in $lc_own\n";
				}
			}
			else
			{
				# x.x.xx.xx
				if ( $lc_formated_buildpref =~ /^\d\d$/ ) 
				{
					# check for RAMP
					$lc_own = $lcpoolroot."/LC_${lc_relver}${lc_formated_corlevel}${lc_formated_buildpref}_${bit}_RAMP";
					if ( ( -f "$lc_own/LastBuild")  || ( -l "$lc_own/LastBuild" ))
					{
						$lc_own.="/".substitute_BuildSelect("${lc_own}/LastBuild");
						$lc_state = "RAMP";
						$found_lcown = 1;
					}
					else
					{	
						# check for COR (notz supportet
						# $lc_own = $lcpoolroot."/LC_${lc_relver}${lc_formated_corlevel}${lc_formated_buildpref}_${bit}_COR";
						# if ( ( -f "$lc_own/LastBuild")  || ( -l "$lc_own/LastBuild" ))
						#{
						#	$lc_own.="/".substitute_BuildSelect("${lc_own}/LastBuild");
						#	$lc_state = "COR";
						#}
						#else
						#{
						#	print "InfoX: can't find $lc_formated_buildpref as RAMP\n" if (defined $ext_debug);
						#	@lc_states_to_check = ("cor","dev");
						#}
						print "InfoX: can't find $lc_formated_buildpref as RAMP\n" if (defined $ext_debug);
						# look for version in cor and dev
						@lc_states_to_check = ("cor","dev");
					}
				}
				else
				{
					die "Error: wrong buildnumber ($lc_formated_buildpref) for lcversion\n"  
						unless ($lc_formated_buildpref =~ /^cor|dev$/i);
					@lc_states_to_check = ($lc_formated_buildpref);
				}
				
				foreach $lc_state_to_check (@lc_states_to_check)
				{
					$lc_state_to_check =~ tr/a-z/A-Z/; 
					if (defined $ext_debug)
					{
						if ($lc_formated_buildpref =~ /^\d\d$/)
						{ print "InfoX: lock for build $lc_formated_buildpref_to_check in $lc_state_to_check\n" ; }
						else
						{ print "InfoX: lock for version of latest $lc_state_to_check build\n" ; }
					}
					$lc_own = $lcpoolroot."/LC_${lc_relver}${lc_formated_corlevel}_${bit}_$lc_state_to_check";		
					# ignored if no release area in lcpool found
					next unless ( -d $lc_own );
					# only in DEV use LastOK else LastBuild
					$lc_own.="/".(($lc_state_to_check =~ /DEV/) ? substitute_LastOK($lc_own) : substitute_BuildSelect("${lc_own}/LastBuild"));
					$versionfile = "$lc_own/VERSIONS";
					my $found_lc_formated_buildpref = "";
					print "InfoX: read $versionfile \n" if (defined $ext_debug);
					open(VERSFILE, "< $versionfile") || die "Couldn't open $versionfile for reading";
					while (<VERSFILE>)
					{
						if ( /\[kernel\]/ )
						{
							$_=<VERSFILE>;
							if ( /version=\d\.\d\.\d\d\.(\d\d)/ )
							{
								$found_lc_formated_buildpref = $1;
	
								last;
							}
							else
							{
								die "Error: found wrong format in $versionfile for kernel ($_)\n";
							}
						}
					}
					close (VERSFILE);
					die "Error: no entry for kernel in $versionfile\n" if ($found_lc_formated_buildpref eq ""); 
						
					if (( $lc_formated_buildpref !~ /^\d\d$/) || ($lc_formated_buildpref eq $found_lc_formated_buildpref ))
					{
						$lc_formated_buildpref = $found_lc_formated_buildpref;
						$lc_buildpref = $lc_formated_buildpref =~ /^0(\d)$/ ? $1 : $lc_formated_buildpref;												
						$lcversion="SAPDB_${lc_relver}${lc_corlevel}_b${lc_formated_buildpref}";
						$found_lcown = 1;
						$lc_state = $lc_state_to_check;
						last;
					}
				}	
				die "Error: can't find liveCache version with build $lc_formated_buildpref\n" unless ($found_lcown);
			}
		}
	}
	$lc_own = convertFileString($lc_own);
	print "Info: getLC_OWN returned with $lc_own\n" if (defined $debug);
    return ($lc_own);
}

sub getAPO_SRC
{
	vmakeEnvOptions(@_);
	unless (defined $apo_src)
	{
		if (defined $no_src_dir)
		{
			die "parameter -no_src_dir request option -own\n" unless (defined $own);
			$apo_src = "$own";
		}	
		else
		{
			$apo_src="";
			check_vmakeVariables("release");
			$apo_src="$SRC_TREE$release";
		}
	}
	print "Info: getAPO_SRC returned with $apo_src\n" if (defined $debug);
	return 	($apo_src);
}

sub setAPO_SRC
{
	$ENV{APO_SRC}= getAPO_SRC ( @_ );
}

sub getRELVER
{
	vmakeEnvOptions(@_);
	get_lc_build();
	print "Info: getRELVER returned with R$lc_relver\n" if (defined $debug);
	return 	("R$lc_relver");
}

sub setRELVER
{
	$ENV{RELVER}= getRELVER ( @_ );
}

sub getCORRECTION_LEVEL
{
	vmakeEnvOptions(@_);
	get_lc_build();
	print "Info: getCORRECTION_LEVEL returned with $lc_corlevel\n" if (defined $debug);
	return 	("$lc_corlevel");
}

sub setCORRECTION_LEVEL
{
	$ENV{CORRECTION_LEVEL}= getCORRECTION_LEVEL ( @_ );
}

sub getBUILDPRAEFIX
{
	vmakeEnvOptions(@_);
	get_lc_build();
	print "Info: getBUILDPRAEFIX returned with $lc_buildpref\n" if (defined $debug);
	return 	("$lc_buildpref");
}

sub setBUILDPRAEFIX
{
	$ENV{BUILDPRAEFIX}= getBUILDPRAEFIX ( @_ );
}

sub getLC_STATE
{
	vmakeEnvOptions(@_);
	unless (defined $lc_state)
	{
		getLC_OWN();
	}
	check_vmakeVariables("lc_state");
	print "Info: getLC_STATE returned with $lc_state\n" if (defined $debug);
	return 	($lc_state);
}

sub setLC_STATE
{
	$ENV{LC_STATE}= getLC_STATE ( @_ );
}

sub getLCROOT
{
	vmakeEnvOptions(@_);
	getLC_OWN();
	print "Info: getLC_ROOT returned with ".(convertFileString("$lc_own/sys/src/lcsrc"))."\n" if (defined $debug);
	return 	(convertFileString("$lc_own/sys/src/lcsrc"));
}

sub setLCROOT
{
	$ENV{LCROOT}= getLCROOT ( @_ );
}

sub getLCMAKEID
{
	vmakeEnvOptions(@_);
	getLC_OWN();
	print "Info: getLC_ROOT returned with ".(convertFileString("$lc_own/sys/src/lcsrc"))."\n" if (defined $debug);
	if (-f "$lc_own/make.id")
	{
		open(LCMAKEID, "<$lc_own/make.id");
		$lcmakeid=<LCMAKEID>;
		chomp $lcmakeid;
		close LCMAKEID;
	}
	else
	{
		print ("Waring: no makeid of livecache found \n     set LCMAKEID to 1\n");
		$lcmakeid="1";
	}
	return($lcmakeid);
}

sub setLCMAKEID
{
	$ENV{LCMAKEID}= getLCMAKEID ( @_ );
}

sub getLCVERSION
{
	vmakeEnvOptions(@_);
	get_lc_build();
	# lock for correct patchlevel in lc_own
	getLC_OWN() unless ($lc_formated_buildpref =~ /^\d\d$/);
	print "Info: getLCVERSION returned with $lcversion\n" if (defined $debug);
	return 	($lcversion);
}

sub setLCVERSION
{
	$ENV{LCVERSION}= getLCVERSION ( @_ );
}

sub getformatedLCVERSION
{	
	vmakeEnvOptions(@_);
	get_lc_build();
	$formated_lcversion ="${lc_relver}";
	$formated_lcversion =~ s/(\d)(\d)/$1.$2/;
	$formated_lcversion.= ".${lc_formated_corlevel}.${lc_formated_buildpref}";
	print "Info: getformatedLCVERSION returned with $lcversion\n" if (defined $debug);
	return 	($formated_lcversion);
}

sub getAPO_COM_RELEASE
{
	vmakeEnvOptions(@_);
	init_APO_COM_Vars();
	print "Info: getAPO_COM_RELEASE returned with $apo_com_release\n" if (defined $debug);
	return 	($apo_com_release);
}

sub setAPO_COM_RELEASE
{
	$ENV{APO_COM_RELEASE}= getAPO_COM_RELEASE ( @_ );
}

sub getAPO_COM_BUILD
{
	vmakeEnvOptions(@_);
	init_APO_COM_Vars();
	print "Info: getAPO_COM_BUILD returned with $apo_com_build\n" if (defined $debug);
	return 	($apo_com_build);
}

sub setAPO_COM_BUILD
{
	$ENV{APO_COM_BUILD}= getAPO_COM_BUILD ( @_ );
}


sub getAPO_COM_SHORT
{
	unless ( defined $apo_com_short )
	{
		vmakeEnvOptions(@_);
		init_APO_COM_Vars();
	}
	print "Info: getAPO_COM_SHORT returned with $apo_com_short\n" if (defined $debug);
	return 	($apo_com_short);
}

sub setAPO_COM_SHORT
{
	$ENV{APO_COM_SHORT}= getAPO_COM_SHORT ( @_ );
}

sub getAPO_PATCH_LEVEL
{
	unless ( defined $apo_patch_level )
	{
		vmakeEnvOptions(@_);
		init_APO_COM_Vars();
		print "Info: getAPO_PATCH_LEVEL returned with $apo_patch_level\n" if (defined $debug);
	}
	return 	($apo_patch_level);
}

sub setAPO_PATCH_LEVEL
{
	$ENV{APO_PATCH_LEVEL}= getAPO_PATCH_LEVEL ( @_ );
}


sub setLC_POOL_ROOT
{
	$ENV{'LC_POOL_ROOT'} = getLC_POOL_ROOT( @_ );
}	

sub getLC_POOL_ROOT
{
	vmakeEnvOptions(@_);	
	
	unless (defined $lc_pool_root)
	{ 
		$lc_pool_root="";
		get_lc_build();
		
		if($^O=~/.*win32.*/i)
		{ 
			if ($ENV{'LC_POOL_ROOT'} =~ /\S/) 
			{
				$lc_pool_root = $ENV{'LC_POOL_ROOT'};	
			}
			else
			{
				my $platformtag_on_lcpool = getPlatform();
				if ($platformtag_on_lcpool =~ /^NTx86_64$/i)
				{ $platformtag_on_lcpool = "ntamd64"; }
				$lc_pool_root = "\\\\production\\SCM\\MaxDB\\LC_POOL\\$platformtag_on_lcpool" ;
			}
		}
		else
		{
			$lc_pool_root = ($ENV{'LC_POOL_ROOT'} =~ /\S/) ? 
			                 $ENV{'LC_POOL_ROOT'} : 
			                "$LC_POOL/${lc_relver}${lc_formated_corlevel}/pkg/".getPlatform();
		}	
	}
	return $lc_pool_root;
}

sub getVMAKE_PATH
{
	unless ( defined $vmake_path )
	{
		vmakeEnvOptions(@_);
		getOWN();
		getAPO_SRC();
		getLC_OWN();
		$vmake_path = "$own";
		unless ($own eq $apo_src )
		{
			$vmake_path.= ",$apo_src";
			$vmake_path.= "//" unless ($apo_src =~ /\/\/$/);
		}
		$vmake_path.= ",$lc_own//";
	}
	print "Info: getVMAKE_PATH returned with $vmake_path\n" if (defined $debug);
	return 	($vmake_path);
}

sub setVMAKE_PATH
{
	$ENV{VMAKE_PATH}= getVMAKE_PATH ( @_ );
}

sub get_lc_build
{
	return if ((defined $lc_buildpref) && (defined $lc_relver) && (defined $lc_corlevel) );
	
	if (defined $lcversion)
	{
		if (($lcversion =~ /SAPDB_(\d)(\d)(\d*)_b(.+)$/) || ($lcversion =~ /^(\d)\.?(\d)\.?(\d\d)[\._]?(.+)$/))
		{	
			$lc_major     = $1;
			$lc_minor    = $2;	
			$lc_relver    = $lc_major.$lc_minor;
			$lc_corlevel  = $3;
			$lc_formated_buildpref = $4;
			$lc_formated_corlevel  = $lc_corlevel  =~ /^\d\d$/ ? $lc_corlevel  : "0$lc_corlevel";
			$lc_buildpref = $lc_formated_buildpref =~ /^0(\d)$/ ? $1 : $lc_formated_buildpref;
			$lcversion="SAPDB_${lc_relver}${lc_corlevel}_b${lc_formated_buildpref}";
		}
		else
		{
			$lcversion_string = get_lcversion_from_configxml();
			unless ($lcversion_string)
			{
				 die "Error in config.xml\n";
			}
		}
		return;
	}

	unless ( defined $makedbv )
	{	
		if (defined $src_dir)
		{ 
			$makedbv = "$src_dir/config/.makedbv";
		}
		else
		{
			getAPO_SRC();
			$makedbv = "$apo_src/sys/src/config/.makedbv";
		}
	}
	if ( -f $makedbv )
	{	
		print "InfoX: read $makedbv \n" if (defined $ext_debug);
		open(MAKEDBV, "< $makedbv") || 
	 	  		die "Couldn't open $makedbv for reading";
	 	  		
		getPlatform();
		while (<MAKEDBV>)
		{
			if ( /${platform}\/lc\s*(\S*)\s*$/ || /${platform}\/lc\s*(\S*\s*\(.*\))\s*$/ )
			{
				local $lcversion_string=$1;
				if (($lcversion_string =~ /^SAPDB_(\d)(\d)(\d*)_b(.*)$/) || ($lcversion_string =~ /^(\d)\.?(\d)\.?(\d\d)\.?(.*)$/))
				{		
					$lc_major     = $1;
					$lc_minor    = $2;	
					$lc_relver    = $lc_major.$lc_minor;
					$lc_corlevel  = $3;
					$lc_formated_buildpref = $4;
					if ($lc_formated_buildpref =~ /(\d*)\s*\((.*)\)/)
					{
						$lc_formated_buildpref = $1;
						$lc_state = $2;
						$lc_state =~ tr/a-z/A-Z/; 
					}
					$lc_formated_corlevel  = $lc_corlevel  =~ /^\d\d$/ ? $lc_corlevel  : "0$lc_corlevel";
					$lc_buildpref = $lc_formated_buildpref =~ /^0(\d)$/ ? $1 : $lc_formated_buildpref;
					$lcversion="SAPDB_${lc_relver}${lc_corlevel}_b${lc_formated_buildpref}";
					if  ( ( defined $lcversion ) && ( $lcversion !~ /^SAPDB_$lc_relver$lc_corlevel/) )
					{
						die "Error: inconsistent version information of call parameter and parameter in makedbv\n $lcversion <-> SAPDB_$lc_relver$lc_corlevel\n";
					}
				}
				else
				{
					die "Error: wrong lcversion declaration in .makedbv for $platform ($lcversion_string)\n" ;
				}
				last;
			}
		}
		close ( MAKEDBV);
		die "Error: can't find lcversion for $platform in .makedbv\n" unless (defined $lc_buildpref);
	}
	else
	{
		$lcversion_string = get_lcversion_from_configxml();
		unless ($lcversion_string)
		{
			 die "Error in config.xml\n";
		}
		
	}
}

sub get_lcversion_from_configxml
{
	my $default_lcversion = undef;
	my $searched_lcrel = $lcversion;
	my $found_version = undef;
	my $lcrel = undef;
	my $plaform = getPlatform();
	my %lcrels;
	my $lcversion_string = undef;
	
	if ($searched_lcrel =~ /^(\d)(\d)(\d\d)$/)
	{	$searched_lcrel = "$1.$2.$3";	}
	elsif ($searched_lcrel =~ /^(\d)(\d)$/)
	{	$searched_lcrel = "$1.$2";	}
	
	unless (defined $src_dir)
	{
		getAPO_SRC();
		$src_dir = "$apo_src/sys/src";
	}
	if ( -f "$src_dir/config/config.xml" )
	{
		unless (defined $config_xml_tree)
		{
			local @INC = ("$ENV{TOOL}/bin",@INC);
			
			if($^O=~/.*win32.*/i)
			{  unshift @INC, "$ENV{TOOL}/lib/perl"  }
			else
			{  unshift @INC, "$ENV{TOOL}/lib/perl5" }
			
			require XML::Simple;
			import XML::Simple;		
			print "InfoX: read $src_dir/config/config.xml\n" if (defined $ext_debug);
			$config_xml_tree = eval { XMLin("$src_dir/config/config.xml"); };
			if ($@) 
			{ 
				print "Error while parsing $src_dir/config/config.xml: $res\n";
				return undef;
			}		
		}
		foreach $lcrel (@{$config_xml_tree->{Setting}->{LCRelease}})
		{
			if (defined ($lcrel->{Default}))
			{
				# the default version
				if (defined $default_lcrel)
				{
					print "Error: Found second default definition for LCRelease in config.xml\n";
					return undef;
				}	
				$default_lcrel = $lcrel->{Default};
			}
			else
			{
				$lcrels{$lcrel->{Release}}=$lcrel;
			}
		}
		
		unless (defined $searched_lcrel)
		{
			if (defined $default_lcrel)
			{
				$searched_lcrel = $default_lcrel;
			}
			else
			{
				print "Error: Default definition for LCRelease in config.xml is missing\n";
				return undef;
			}
		}
		# look for the right LCRelease
		foreach (keys %lcrels)
		{
			if (/^$searched_lcrel/)
			{
				if (defined $found_version)
				{ 
					print "Error: Given LCRelease definition ($searched_lcrel) matched with more than one entries in config.xml \n";
					return undef;
				}	
				else
				{ 
					$found_version = $lcrels{$_};
				}
			}
		}
		
		unless (defined $found_version)
		{
			print "Error: Can't find LCRelease definition ($searched_lcrel) in config.xml \n";
			return undef;
		}
		
		# ok I found a LCRelease
		# and now I try to get the right buildnumber
		print "InfoX: Found release $found_version->{Release}\n" if (defined $ext_debug);
		
		# use default build
		$lcversion_string = $found_version->{DefaultBuild};
		my @special_platforms = (ref($found_version->{Build}) eq "ARRAY") ? @{$found_version->{Build}} : ($found_version->{Build});
		foreach (@special_platforms)
		{
			if ($_->{Platform} =~ /^$platform$/i)
			{
				$lcversion_string = $_->{Number};
			}
		}	
		
		if ($lcversion_string)
		{	
			print "InfoX: Found mapped build '$lcversion_string'\n" if (defined $ext_debug);
			$lcversion_string="$found_version->{Release}.$lcversion_string"; 	
		}
		else
		{
			print "ERROR: Can't find build infomation for platform $platform\n";
			print "       (A default defintion for build number doesn't exist)\n";
			return undef;
		}
		
		if ($lcversion_string =~ /^(\d)\.(\d)\.(\d\d)\.(.*)$/)
		{
			$lc_major     = $1;
			$lc_minor    = $2;	
			$lc_relver    = $lc_major.$lc_minor;
			$lc_formated_corlevel  = $3;
			$lc_formated_buildpref = $4;
			$lc_corlevel  = $lc_formated_corlevel =~ /^0(\d)$/ ? $1 : $lc_formated_corlevel;
			$lc_buildpref = $lc_formated_buildpref =~ /^0(\d)$/ ? $1 : $lc_formated_buildpref;
			$lcversion="SAPDB_${lc_relver}${lc_corlevel}_b${lc_formated_buildpref}";
		}
	}
	else
	{	"Error: can't find config.xml\n";		}
	
	return ($lcversion_string);
	
}


sub init_APO_COM_Vars
{
	return if ( ( defined $apo_com_release ) && ( defined $apo_com_build ) && ( defined $apo_com_short ) );
	vmakeEnvOptions(@_);
	# if apo_com_short defined generate apo_com_build/release from it (for remuser)
	if ( defined $apo_com_short )
	{
		if ($apo_com_short =~ /^(\d)(\d)(\d\d)$/)
		{
			$apo_com_release = "$1.${2}0";
			$apo_com_build   = "$3";
		}
		elsif ($apo_com_short =~ /^(\d)(\d\d)(\d\d)$/)
		{
			$apo_com_release = "$1.$2";
			$apo_com_build   = "$3";
		}
		else
		{
			die "wrong format for APO_COM_SHORT ($apo_com_short)\n" if ($apo_com_short !~ /(\d)(\d)(\d\d)$/);
		}
	}
	else
	{
		unless (defined $src_dir)
		{
			getAPO_SRC();
			$src_dir = "$apo_src/sys/src";
		}

		print "InfoX: read $src_dir/maketools/version.bat \n" if (defined $ext_debug);
		open(VERSION_BAT, "< $src_dir/maketools/version.bat") || 
	    	die "Couldn't open $src_dir/maketools/version.bat for reading";
		while ($line=<VERSION_BAT>)
		{
			if ($line =~ /set APO_COM_RELEASE=(.*)$/)
			{	$apo_com_release=$1;	}
			elsif ($line =~ /set APO_COM_BUILD=(.*)$/)
			{	$apo_com_build=$1;		}
			elsif ($line =~ /set APO_COM_SHORT=(.*)$/)
			{	$apo_com_short=$1;		}
			elsif ($line =~ /set APO_PATCH_LEVEL=(.*)$/)
			{	$apo_patch_level="$1";		}
		}
		close (VERSION_BAT);
	}
}

sub check_vmakeVariables
{
	local @args = @_;
	while (@args) 
	{
		my $arg = shift @args;		
		unless (eval "defined \$$arg")
		{
			die ("Error: -$arg must be given\n");
			
		}
	}
}

sub substitute_LastOK
{
	local $lb_path = shift ;
	local $sub_LCBuild = "";
	if ( defined $lcpool_count )
	{
		$sub_LCBuild = $lcpool_count;
		$sub_LCBuild = substitute_BuildSelect ( "$lb_path/$sub_LCBuild")
		   if ((defined $lb_path) && ( $sub_LCBuild !~ /^\d+$/));
	}
	else
	{	
		if (defined $remcall)
		{
			if (defined $lb_path)
			{
				$sub_LCBuild = substitute_BuildSelect ( "$lb_path/LastBuild" ) ;
			}
			else
			{
				$sub_LCBuild = "LastBuild";
			}
		}
		else
		{
			unless (defined $src_dir)
			{
				getAPO_SRC();
				$src_dir = "$apo_src/sys/src";
			}
			if ( -f "$src_dir/config/config.xml" )
			{
				unless (defined $config_xml_tree)
				{
					local @INC = ("$ENV{TOOL}/bin",@INC);
					
					if($^O=~/.*win32.*/i)
					{  unshift @INC, "$ENV{TOOL}/lib/perl"  }
					else
					{  unshift @INC, "$ENV{TOOL}/lib/perl5" }
					
					require	Util::XMLUtil;
					import Util::XMLUtil;
					
					$config_xml_tree = ParseXMLFile ("$src_dir/config/config.xml");
				}

				local %attrib=();
				$attrib{Name} = $LC_BUILD_SELECT_TAG;
				local $LC_OK_subtree = getSubTree ($config_xml_tree, "Setting", 0,\%attrib);
				%attrib=();
				$attrib{Platform}=getPlatform();
				local $platform_subtree = eval 'getSubTree ($LC_OK_subtree, "os", 0 ,\%attrib)';
				if ($@)
				{
					print ("Warning: Can't determine LC_POOL ID for '$attrib{Platform}' in config.xml:\n$@\n");
					print ("Now I use the LastBuild\n"); 
					if (defined $lb_path)
					{
						$sub_LCBuild = substitute_BuildSelect ( "$lb_path/LastBuild" ) ;
					}
					else
					{
						$sub_LCBuild = "LastBuild";
					}
				}
				else
				{		
					foreach $value (getChildren($platform_subtree))
					{
						if ( $value->GetType() eq "Value" )
						{
							$sub_LCBuild = $value->GetContent();
							$sub_LCBuild = substitute_BuildSelect ( "$lb_path/$sub_LCBuild")
						         if ((defined $lb_path) && ( $sub_LCBuild !~ /^\d+$/));
							last;
						}
					}
					die "Error: can't find $LC_BUILD_SELECT_TAG Value for platform:".(getPlatform())."\n" if ($sub_LCBuild eq "");
				}
			}
			else 
			{
				if (defined $lb_path)
				{
					$sub_LCBuild = substitute_BuildSelect ( "$lb_path/LastBuild" ) ;
				}
				else
				{
					$sub_LCBuild = "LastBuild";
				}
			}
		}
	}		
	if (defined $lb_path)
	{ 
		die "Error: LC_OWN $lb_path/$sub_LCBuild not existent\n" unless (-d "$lb_path/$sub_LCBuild");
	}	
	return $sub_LCBuild;
}


sub substitute_BuildSelect 
{
	local $lb_string= shift;
	local $sub_LastBuild="";
	if ( -f "$lb_string" )
	{
		print "InfoX: read $lb_string \n" if (defined $ext_debug);
		open(LB, "< $lb_string") || die "Couldn't open $lb_string for reading";
		$sub_LastBuild=<LB>;
		chomp $sub_LastBuild;
		close(LB);
	}
	elsif ( -l "$lb_string" )
	{
		$sub_LastBuild = readlink $lb_string;
		if ( $sub_LastBuild =~ /(\d\d\d\d?)$/ )
		{	$sub_LastBuild=$1;	} 
		else
		{	die "Error: found wrong link ($lb_string -> $sub_LastBuild)\n";	}
	}
	else
	{
		die "Error: Can't find regular BuildSelect ($lb_string) !\n";
	}
	return ($sub_LastBuild);
}

###
# TEST
###
sub printInfo 
{
	vmakeEnvOptions(@_);
	print ("OWN                   = ".getOWN ()."\n");
	print ("VMAKE_DEFAULT_VARIANT = ".getVMAKE_DEFAULT_VARIANT()."\n");
	print ("VMAKE_PATH            = ".getVMAKE_PATH()."\n");
	print ("APO_SRC               = ".getAPO_SRC()."\n");
	print ("BRANCH_INFO           = ".getBRANCH_INFO()."\n");
	print ("APO_COM_RELEASE       = ".getAPO_COM_RELEASE()."\n");
	print ("APO_COM_BUILD         = ".getAPO_COM_BUILD()."\n");
	print ("APO_COM_SHORT         = ".getAPO_COM_SHORT()."\n");
	print ("APO_PATCH_LEVEL       = ".getAPO_PATCH_LEVEL()."\n");
	print ("LC_OWN                = ".getLC_OWN()."\n");
	print ("LCROOT                = ".getLCROOT()."\n");
	print ("LC_STATE              = ".getLC_STATE()."\n");
	print ("LCVERSION             = ".getLCVERSION()."\n");
	print ("RELVER                = ".getRELVER()."\n");
	print ("CORRECTION_LEVEL      = ".getCORRECTION_LEVEL()."\n");
	print ("BUILDPRAEFIX          = ".getBUILDPRAEFIX()."\n");
	print ("LCMAKEID              = ".getLCMAKEID()."\n");
}

sub createProfile
{
	local @args=@_;
	local @profile_vars;
	local @append_files;
	while (@args) 
	{
		my $arg = shift @args;
		if ($arg =~ /^-all$/i)
		{
			@profile_vars=@ALL_VARS;
			next;
		}
		if ($arg =~ /^-name$/i)
		{
			$profile_name=shift @args;
			next;
		}

		if ($arg =~ /^-append$/i)
		{
			push @append_files , shift @args; 
			next;
		}
		push @profile_vars, $arg;
	}
	# default profile in $OWN
	unless ( defined $profile_name )
	{
		getOWN();
		$profile_name = "$own/$PROFILE_NAME";
	}
	
	checkdir($profile_name);
	unlink "$profile_name" if ( -f "$profile_name" );
	print "InfoX: write $profile_name \n" if (defined $ext_debug);
	open(PROFILE, ">$profile_name") || die "Couldn't open $lb_string for writing";
	if($^O=~/.*win32.*/i)
	{
		print PROFILE "rem --- profile generated by vmakeEnv.pl ---\n";
		print PROFILE "\@echo off\n\n";
	}
	else
	{
		print PROFILE "#!/bin/sh\n";
		print PROFILE "# --- profile was generated by vmakeEnv.pl ---\n\n";
	}
	foreach $var (@profile_vars)
	{
		writeVarToProfile ($var);
	}
	foreach $file (@append_files)
	{
		print "InfoX: read $file for appending \n" if (defined $ext_debug);
		open(APPEND_FILE, "< $file") || 
    	die "Couldn't open $file for reading";
    	if($^O=~/.*win32.*/i)
		{
			print PROFILE "\nrem -------------------------\n";
			print PROFILE "rem --- import from $file ---\n";
			print PROFILE "rem -------------------------\n";
		}
		else
		{
			print PROFILE "\n# -------------------------\n";
			print PROFILE "# --- import from $file ---\n";
			print PROFILE "# -------------------------\n";
		}
		while (<APPEND_FILE>)
		{
			print PROFILE $_;
		}
		close APPEND_FILE;
	}
	close PROFILE;	
}

sub writeVarToProfile 
{
	local $var = shift;
	local $val = eval "get$var()";
	if ($@)
	{
		$errormessage = $@;
		chop $errormessage;
		die "Error: wrong variable ($var) to write in profile\n       ($errormessage)\n";
	}
	if (defined $val)
	{
		if($^O=~/.*win32.*/i)
		{
			print PROFILE "set $var=$val\n";
		}
		else
		{
			print PROFILE "$var=$val\n    export $var\n";
		}
	}
	return $val;
}

sub setEnv
{
	local @args=@_;
	local @env_vars=();
	while (@args) 
	{
	  my $arg = shift @args;
	  if ($arg =~ /^all$/i)
	  {
	  	@env_vars=@ALL_VARS;
	  	last;
	  }
	  push @env_vars, $arg;
	}
	foreach $var (@env_vars)
	{
		$ENV{$var}=	eval "get$var()";
		die "Error: wrong variable ($var) to set in environment($@)\n"  if ($@);
	}
}

# parameter: file with full path or directory with "/" at the end
sub checkdir
{
	local $path = shift;
	local $dir_path = "";
	local $mode = 0775;
	# convert \ to /
	$path =~ s/\\/\//g;
	# search for directories
	while ($path =~ /([^\/]*\/)/g)
    {
		$dir_path.=$1;
	    if ( ! -d $dir_path )
		{
			die "Error: Can't create directory $dir_path\n" if ( mkdir($dir_path, $mode) == 0 );
			unless($^O =~ /mswin/i)
			{
				#
				# workaround for perl bug
				#
				my @statbuf = stat ($dir_path);
				my $filemode = $statbuf[2] & 0777;
				if ($filemode != $mode) 
				{ die "Error: Can't change permissions of directory $dir_path\n" if (chmod ($mode, $dir_path)== 0); }
			}
		}
	}
}

sub getPlatform
{
	unless (defined $platform)
	{
		$uname="";
		%mapping = ();
		$mapping{'HP-UX'} = "hp_64";
		$mapping{'HPIA64'} = "hpia64";
		$mapping{'AIX'} = "rs6000_64";
		$mapping{'AIX5'} = "rs6000_51_64";
		$mapping{'SunOS'} = "sun_64";
		$mapping{'OSF1'} = "alphaosf";  
		$mapping{'Windows_NT'} = "NTintel";
		$mapping{'Windows_IA64'} = "NTia64";
		$mapping{'Windows_AMD64'} = "NTx86_64";
		$mapping{'Linux_ia64'} = "linuxia64";
		$mapping{'Linux_ia64_9'} = "linuxia64sles9";
		$mapping{'Linux_ix86'} = "linuxintel";
		$mapping{'Linux_ppc64'} = "linuxppc64";
		$mapping{'Linux_x86_64'} = "linuxx86_64";
		$mapping{'Sun_x86_64'} = "sunx86_64";
				
		if($ENV{'OS'})
		{
			chomp $ENV{'OS'};
			$uname=$ENV{'OS'};
			if(($^O=~/.*win32.*/i) && ( $bit eq "64" ))
			{
				$uname = ($ENV{PROCESSOR_ARCHITECTURE} =~ /AMD64/) ? "Windows_AMD64" : "Windows_IA64";
			}
		}
		else
		{
			if ( $^O =~ /linux/i)
			{
				open(UNAME, "uname -m |");
				$utmp=<UNAME>;
				close UNAME;
				chomp $utmp;		
				$uname = "Linux_$utmp";
				$uname =~ s/Linux_i\d86/Linux_ix86/;
				if ($utmp =~ /^ia64$/i)
				{
					getAPO_COM_SHORT();
					if ($apo_com_short lt "5000")
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
			}
			elsif ( $^O =~ /solaris/i)
			{
				$uname = "SunOS";
				open(UNAME, "uname -p |");
				$utmp=<UNAME>;
				close UNAME;
				chomp $utmp;
				if ($utmp =~ /i386/i )
				{   $uname = "Sun_x86_64";	}
			}	
			elsif ( $^O =~ /aix/i)
			{
				$uname = "AIX";
				getAPO_COM_SHORT();
				open(UNAME, "uname -v |");
				$utmp=<UNAME>;
				close UNAME;
				chomp $utmp;
				if ($apo_com_short lt "4000" )
				{
				   $uname = "AIX5" if($utmp == 5);
				}
			}
			elsif ( $^O =~ /hpux/i )
			{
				$uname = "HP-UX";
				open(UNAME, "uname -m |");
				$utmp=<UNAME>;
				close UNAME;
				chomp $utmp;
				if ($utmp =~ /ia64/i )
				{   $uname = "HPIA64";	}
			}
			else
			{
				open(UNAME,"uname|");
				$uname=<UNAME>;
				close(UNAME);
				chomp $uname;
			}
		}
		$platform = $mapping{$uname};
	} 
	return $platform;
}

sub convertFileString
{
	local $filestring = shift;
	if($^O=~/.*win32.*/i)
	{
		$filestring =~ tr/\//\\/;
	}
	else
	{
		$filestring =~ tr/\\/\//;
	}
	return $filestring;
}


