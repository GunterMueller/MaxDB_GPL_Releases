#!/usr/bin/perl
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

$filename=shift;
$filename = "what.cpp" unless ($filename);

########################################
# collect data from ENV

if ( exists $ENV{'USER'} )
{
  $user = $ENV{'USER'};
}
else
{
  $user = "unkown user";
}


if ( exists $ENV{'OWN'} )
{
  $gen_path = $ENV{'OWN'};
}
else
{
  $gen_path = "GEN_DIR was not set";
}

if ( exists $ENV{'VMAKE_PATH'} )
{
  $source_path = $ENV{'VMAKE_PATH'};
}
else
{
  $source_path = "SOURCE_DIR was not set";
}

if ( exists $ENV{'LCVERSION'} )
{
  if ( $ENV{'LCVERSION'} =~ /SAPDB_(\d)(\d)(\d*)_b(.*)$/ )
  {
  	local $lc_major     = $1;
	local $lc_minor    = $2;
	local $lc_corlevel  = $3;
	local $lc_formated_buildpref = $4;
	local $lc_formated_corlevel  = $lc_corlevel  =~ /^\d\d$/ ? $lc_corlevel  : "0$lc_corlevel";
	$db_version = "${lc_major}.${lc_minor}.${lc_formated_corlevel}.$lc_formated_buildpref";
  }
  else
  { $db_version = $ENV{'LCVERSION'}; }
  $db_version.=" (DEV)"  if ( $ENV{'LC_STATE'} =~ /^DEV$/i);
}
else
{
  $db_version = "LCVERSION was not set";
}

if ( exists $ENV{'LCROOT'} )
{
  $dbroot = $ENV{'LCROOT'};
  $version_in_incl = "Version not found";
  $incl_file_name = "$dbroot/incl/lcbasetypes.h";
  if ( open LC_INCL_FILE, $incl_file_name )
  {
    while ( <LC_INCL_FILE> )
    {
      if( /LIVECACHEOMS_VERSION .*\"(.*)\"/ )
      {
        $version_in_incl = $1;
        last;
      }
    }
    close LC_INCL_FILE or die "Can't close  $incl_file_name :$!";
   }
   else
   {
     $version_in_incl = "Can't open $incl_file_name";
   }
}
else
{
  $dbroot = "LCROOT was not set";
  $version_in_incl ="LCROOT was not set";
}

#if ( exists $ENV{'COMPILE_CPLUS'} )
#{
#  $compile_cmd = $ENV{'COMPILE_CPLUS'};
#  $compile_cmd = &expand_env ( $compile_cmd );
#}
#else
#{
#  $compile_cmd = "COMPILE_CPLUS was not set";
#}

if ( exists $ENV{'APO_COM_RELEASE'} )
{
  $release_info = $ENV{'APO_COM_RELEASE'};
}
else
{
  $release_info = "APO_COM_RELEASE not set";
}

if ( exists $ENV{'APO_COM_BUILD'} )
{
  $build_info = $ENV{'APO_COM_BUILD'};
}
else
{
  $build_info = "APO_COM_BUILD not set";
}

if ( exists $ENV{'APO_COM_SHORT'} )
{
  $short_info = $ENV{'APO_COM_SHORT'};
}
else
{
  $short_info = "APO_COM_SHORT not set";
}

if ( exists $ENV{'APO_COM_OS'} )
{
  $os_info = $ENV{'APO_COM_OS'};
}
else
{
  $os_info = "APO_COM_OS not set";
}

########################################
# Compiler
$cpp_version = "Unknown version";
$cpp_version = &get_compiler_version ( );

$c_compiler = $ENV{'CC_CPLAIN'};
$c_version = "Unknown version";
if ( $c_compiler and open ( C_V, "$c_compiler -V 2>&1|" ) )
{
  $c_version = <C_V>;
  chop ( $c_version );
  close C_V;
}

########################################
# some other data
$uname = `uname -a`;
if ( $uname )
{
  chop ( $uname );
}
$date = `date`;
chop ($date);

if ( exists $ENV{'TYPE'} )
{
  $make_type = $ENV{'TYPE'};
}
else
{
  $make_type = "TYPE was not set";
}

if ( exists $ENV{'LATEST_CHANGE'} )
{
  $change_list = $ENV{'LATEST_CHANGE'};
}
else
{
  $change_list = "LATEST_CHANGE was not set";
}

########################################
# write File
if ( not open ( OUT, ">$filename" ) )
{
  die "Can't open $filename for write";
}


if ( exists $ENV{'BUILDSTRING'} )
{ $full_build_info = $ENV{'BUILDSTRING'}; }
else
{ $full_build_info = $build_info; }

write_what_string ("char __what00__[] ", "\@(#)APO $release_info Build: $full_build_info");
write_what_string ("char __what01__[] ", "\@(#)make machine: $uname");
write_what_string ("char __what02__[] ", "\@(#)date: $date");
write_what_string ("char __what03__[] ", "\@(#)user: $user");
write_what_string ("char __what04__[] ", "\@(#)release_info: $release_info");
write_what_string ("char __what04a__[] ", "\@(#)changelist: $change_list");
write_what_string ("char __what04b__[] ", "\@(#)build: $build_info");
write_what_string ("char __what04c__[] ", "\@(#)release info short: $short_info");
write_what_string ("char __what04d__[] ", "\@(#)os info for file: $os_info");
if ( exists $ENV{'APO_COM_ADD'} )
{
  $add_info = $ENV{'APO_COM_ADD'};
  write_what_string ("char __what04e__[] ", "\@(#)add_info: $add_info");
}
if ( exists $ENV{'RELEASE'} )
{
  $rel = $ENV{'RELEASE'};
  write_what_string ("char __what04f__[] ", "\@(#)release: $rel");
}
write_what_string ("char __what05__[] ", "\@(#)sources from: $source_path");
write_what_string ("char __what06__[] ", "\@(#)build dir: $gen_path");
write_what_string ("char __what07__[] ", "\@(#)livecache version .makedbv: $db_version");
write_what_string ("char __what08__[] ", "\@(#)livecache dir: $dbroot");
write_what_string ("char __what09__[] ", "\@(#)version out of include file: $version_in_incl");
#write_what_string ("char __what10__[] ", "\@(#)compiler C++: $cpp_version");
#write_what_string ("char __what11__[] ", "\@(#)compiler C: $c_version");
write_what_string ("char __what12__[] ", "\@(#)make type: $ENV{VMAKE_DEFAULT_VARIANT}");
#write_what_string ("char __what13__[] ", "\@(#)compiler cmd: $compile_cmd");
if ( exists $ENV{'SKIP_CLEAN_STEP'} )
{
  write_what_string ("char __what14__[] ", "\@(#)SKIP_CLEAN_STEP was set");
}
write_what_string ("char __what15__[] ", "\@(#)");

close OUT or die "Can't write to file $filename :$!";

0;


sub write_what_string
{
	local ($ident, $identstring)=@_;
	print OUT "$ident = \"$identstring\\n\";\n";
}

sub expand_env
{
  local $work = $_[0] ;
  local $sub_str;
  local $env_str;
  local $befor;
  local $after;
  $_ = $work;
  while ( /\$\([^\).]*\)/ )
  {
    $befor = $`;
    $after = $';
    $sub_str = $&;
    $_ = $sub_str;
    /\$\((.*)\)/;
    $env_str = $ENV{ $1 };
    $work = "$befor$env_str$after";
    $_ = $work;
  }
  return $work;
}

sub get_compiler_version
{
  local $return_val = "Version not found";
  local $bs_name = `uname -s`;
  chomp ( $bs_name );
  if ( not $bs_name )
  {
    print "Can't get name from operating system (using \"uname -s\")!\n";
    return "Version not found";
  }
  if ( $bs_name eq "AIX")
  {
    if ( open( LS_LPP_OUT, 'lslpp -hc  vacpp.cmp.core |') )
    {
      while ( <LS_LPP_OUT> )
      {
        if ( /vacpp/i and /batch/i )
        {
          /(.*)/ ;
          $return_val = $1;
        }
      }
      close ( LS_LPP_OUT ) or print "ERROR Can't close lslpp -hc  vacpp.cmp.core ! \n$!\n";
    }
    else
    {
      print "ERROR Can't open lslpp -hc vacpp.cmp.batch ! \n$!\n";
    }
  }
  else
  {
    local $cpp_compiler = $ENV{'CC_CPLUS'};
    if ( $cpp_compiler and open ( CPP_V, "$cpp_compiler -V 2>&1|" ) )
    {
      $return_val = <CPP_V>;
      chop ( $return_val );
      close CPP_V;
    }
  }
  return $return_val;
}

