#!/devtool/local/bin/perl
#
# NACHT.ALL.pl Script
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
################################################################################

use imake ;
use File::Path ;
use Getopt::Long;
use POSIX;
use File::Copy;
use qadb ;
use CCVers;


################################################################################
################## main ########################################################
################################################################################

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

my $qah ;
my $qadb_id=0 ;
my $qah_open_ok=1 ;
my $CHANGELIST_NO=0 ;
my $TOOL_CHANGELIST_NO=0;
my $LC_POOL_ID=0 ;
my $idobjstat=0 ;
my $prot_text='' ;

my $SCRATCH ;
my $CLI_BIT_32 ;
my $PCR_BIT_32 ;
my $MAKE_TYPE='LC_OLTP' ;
my $CHECK ;
my $HELP ;


my $RV=-1 ;
my $CORR_LEVEL='-1' ;
my $BP='-1' ;
my $ONLY_64_BIT=0 ;
my $ONLY_32_BIT=0 ;
my $X_BIT='64' ;
my $OLTP_ONLY=0 ;
my $LC_ONLY=0 ;

my @argv = @ARGV ;
my @args_rmtree = () ;
my @MAKE_TARGETS_WITH_PATH = () ;
my @MAKE_TARGETS_32=() ;
my @MAKE_TARGETS_FAST=() ;
my @MAKE_TARGETS_QUICK=() ;
my @MAKE_TARGETS_SLOW=() ;

my $MAKE_EXIT=0 ;
my $PCR_REL_EXIT=0 ;
my $TO_CD_IMAGES_EXIT=0 ;
my $MAIN_EXIT=0 ;

my $sysname ;
my $nodename ;
my $release;
my $version ;
my $machine ;
my $SYSTEM_NAME ;
my $WDF_SYSTEM_NAME ;
my $SO_SL_DLL ;

my $ProtDir="$ENV{OWN}/tmp" ;
my $MakeRelease='NACHT_ALL' ;
my $MakeProt="$ProtDir/$MakeRelease.prot";

my @localtime = localtime (time);

my $start_32bit_cli_make = 101 ;
my $end_32bit_cli_make   = 102 ;
my $NOK_32bit_cli_make   = 199 ;
my $ok_32bit_cli_make    = 200 ;

my $start_32bit_pcr_make = 151 ;
my $end_32bit_pcr_make   = 152 ;
my $NOK_32bit_pcr_make   = 159 ;
my $ok_32bit_pcr_make    = 160 ;

my $start_server_make    = 201 ;
my $end_server_make      = 202 ;
my $NOK_server_make      = 299 ;
my $ok_server_make       = 300 ;

my $start_archiving_pcr  = 301 ;
my $end_archiving_pcr    = 302 ;
my $NOK_archiving_pcr    = 399 ;
my $ok_archiving_pcr     = 400 ;

my $start_buildpackage   = 401 ;
my $end_buildpackage     = 402 ;
my $NOK_buildpackage     = 499 ;
my $ok_buildpackage      = 500 ;

my $MAKE_NOK             = 999 ;
my $MAKE_OK              =1000 ;

#------------------------------------------------------------------------------#
#   read options
#------------------------------------------------------------------------------#

Getopt::Long::Configure ("no_auto_abbrev");

prot_parts ( "read and check options" ) ;

if ( GetOptions ( "force_rm"    => \$SCRATCH,
                  "scratch"     => \$SCRATCH,
                  "cli_bit_32"  => \$CLI_BIT_32,
                  "pcr_bit_32"  => \$PCR_BIT_32,
                  "make_type=s" => \$MAKE_TYPE,
                  "check"       => \$CHECK,
                  "help"        => \$HELP,
                  "h"           => \$HELP )  and not  $HELP )
{
   if ( defined $SCRATCH) { $SCRATCH = 1 } ;

   if ( defined $CLI_BIT_32 )
   {
      $CLI_BIT_32  = 1 ;
      $ONLY_32_BIT=1 ;
      $X_BIT='32' ;
   }
   else
   {
      $CLI_BIT_32  = 0 ;
   }

   if ( defined $PCR_BIT_32 )
   {
      $PCR_BIT_32  = 1 ;
      $ONLY_32_BIT=1 ;
      $X_BIT='32' ;
   }
   else
   {
      $PCR_BIT_32  = 0 ;
   }

   if ( defined $CHECK ) { $CHECK = 1 };

   if  ( defined $MAKE_TYPE )
   {
      if (( $MAKE_TYPE ne "OLTP"    ) and
          ( $MAKE_TYPE ne "LC"      ) and
          ( $MAKE_TYPE ne "LC_OLTP" )     )
      {
         print_usage () ;
         caller () ? return ( 1 ) : exit ( 1 ) ;
      }
   }

}
else
{
   print_usage () ;
   caller () ? return ( 1 ) : exit ( 1 ) ;
}


if ( $CHECK and $CLI_BIT_32 )
{
   print "\$CLI_BIT_32 = $CLI_BIT_32\n" ;
}

if ( $CHECK and $PCR_BIT_32 )
{
   print "\$PCR_BIT_32 = $PCR_BIT_32\n" ;
}

if ( $CHECK and $SCRATCH )
{
   print "\$SCRATCH    = $SCRATCH\n" ;
}

if ( $CHECK )
{
   print "\$MAKE_TYPE = $MAKE_TYPE\n" ;
}


#------------------------------------------------------------------------------#
#   get some machine informations
#------------------------------------------------------------------------------#

prot_parts ( "uname" ) ;

( $sysname, $nodename, $release, $version, $machine ) = POSIX::uname();


if ( $CHECK )
{
   print "\$sysname  = $sysname\n" ;
   print "\$nodename = $nodename\n" ;
   print "\$release  = $release\n" ;
   print "\$version  = $version\n" ;
   print "\$machine  = $machine\n" ;
}


#------------------------------------------------------------------------------#
#   only 32-bit make
#------------------------------------------------------------------------------#

prot_parts ( "only 32-bit make ?" ) ;

if ((( $sysname eq "Windows NT" ) and ( $machine eq "x86" ))  or
    (( $sysname eq "Linux" )      and ( $machine eq "i686" ))    )
{
   $ONLY_32_BIT=1 ;
   $X_BIT='32' ;
}

if ( $CHECK )
{
   print "\$ONLY_32_BIT = $ONLY_32_BIT\n" ;
   print "\$X_BIT       = $X_BIT\n" ;
}


#------------------------------------------------------------------------------#
#   only 64-bit make
#------------------------------------------------------------------------------#

prot_parts ( "only 64-bit make ?" ) ;

if ((( $sysname eq "OSF1" )       and ( $machine eq "alpha" ))  or
    (( $sysname eq "HP-UX" )      and ( $machine eq "ia64"  ))  or
    (( $sysname eq "Linux" )      and ( $machine eq "ia64"  ))  or
    (( $sysname eq "Windows NT" ) and ( defined $ENV{BIT64} ))     )
{
   $ONLY_64_BIT=1 ;
   $X_BIT='64' ;

   if ( $CLI_BIT_32 or $PCR_BIT_32 )
   {
      $CLI_BIT_32  = 0 ;
      $PCR_BIT_32  = 0 ;
      print "WARNING: no 32 bit client makes allowed\n" ;
   }
}

if ( $CHECK )
{
   print "\$ONLY_64_BIT = $ONLY_64_BIT\n" ;
   print "\$X_BIT       = $X_BIT\n" ;
   print "\$CLI_BIT_32  = $CLI_BIT_32 \n" ;
   print "\$PCR_BIT_32  = $PCR_BIT_32 \n" ;
}


#------------------------------------------------------------------------------#
#   only OLTP make
#------------------------------------------------------------------------------#

prot_parts ( "only OLTP make ?" ) ;

if (( $sysname eq "Linux" ) and ( $machine eq "i686" ))
{
   $OLTP_ONLY=1 ;
}

if ( $CHECK )
{
   print "\$OLTP_ONLY = $OLTP_ONLY\n" ;
}


#------------------------------------------------------------------------------#
#   check $RELVER, $CORRECTION_LEVEL and $BUILDPRAEFIX
#------------------------------------------------------------------------------#

prot_parts ( "check release version string" ) ;

if ( exists $ENV{RELVER} )
{
   if    ( $ENV{RELVER} eq "R72" ) { $RV = 72 ; }
   elsif ( $ENV{RELVER} eq "R73" ) { $RV = 73 ; }
   elsif ( $ENV{RELVER} eq "R74" ) { $RV = 74 ; }
   elsif ( $ENV{RELVER} eq "R75" ) { $RV = 75 ; }
   if ( $CHECK )
   {
      print "\$RELVER = $ENV{RELVER}\n" ;
      print "\$RV     = $RV\n" ;
   }
}
else
{
   print STDERR "ERROR: wrong Release >>>${RELVER}<<<\n" ;
   caller () ? return ( 1 ) : exit ( 1 ) ;
}

if ( exists $ENV{CORRECTION_LEVEL} )
{
   $CORR_LEVEL = $ENV{CORRECTION_LEVEL} ;
   if ($CORR_LEVEL =~ /^\d$/)
   {
      $CORR_LEVEL = "0$CORR_LEVEL" ;
   }
   else
   {
      $CORR_LEVEL = "$CORR_LEVEL" ;
   }

   if ( $CHECK )
   {
      print "\$CORRECTION_LEVEL = $ENV{CORRECTION_LEVEL}\n" ;
      print "\$CORR_LEVEL       = $CORR_LEVEL\n" ;
   }
}
else
{
   print STDERR "ERROR: wrong CORRECTION_LEVEL >>>${CORR_LEVEL}<<<\n" ;
   caller () ? return ( 1 ) : exit ( 1 ) ;
}

if ( exists $ENV{BUILDPRAEFIX} )
{
   $BP = $ENV{BUILDPRAEFIX} ;
   if   ($BP =~ /^\d$/)
   {
      $BP = "0$BP" ;
   }
   else
   {
      $BP = "$BP" ;
   }

   if ( $CHECK )
   {
      print "\$BUILDPRAEFIX = $ENV{BUILDPRAEFIX}\n" ;
      print "\$BP           = $BP\n" ;
   }
}
else
{
   print STDERR "ERROR: wrong CORRECTION_LEVEL >>>${CORR_LEVEL}<<<\n" ;
   caller () ? return ( 1 ) : exit ( 1 ) ;
}


#------------------------------------------------------------------------------#
#   system name for precompiler runtime archive
#------------------------------------------------------------------------------#

prot_parts ( "system names" ) ;

if ( $sysname eq "Windows NT" )
{
   $SO_SL_DLL = "dll" ;

   if ( defined $ENV{BIT64} )
   {
      $SYSTEM_NAME = "NT\\IA64" ;
      $WDF_SYSTEM_NAME = "NTia64" ;
   }
   else
   {
      $SYSTEM_NAME = "NT\\I386" ;
      $WDF_SYSTEM_NAME = "NTintel" ;
   }
}

if ( $sysname eq "AIX" )
{
   $SO_SL_DLL = "so" ;

   if ( $version eq "4" )
   {
      if ( $ONLY_32_BIT )
      {
         $SYSTEM_NAME = "UNIX/AIX_32" ;
         $WDF_SYSTEM_NAME = "rs6000" ;
      }
      else
      {
         $SYSTEM_NAME = "UNIX/AIX_64" ;
         $WDF_SYSTEM_NAME = "rs6000_64" ;
      }
   }
   else
   {
      if ( $ONLY_32_BIT )
      {
         $SYSTEM_NAME = "UNIX/AIX_32" ;
         $WDF_SYSTEM_NAME = "rs6000_51" ;
      }
      else
      {
         $SYSTEM_NAME = "UNIX/AIX5_64" ;
         $WDF_SYSTEM_NAME = "rs6000_51_64" ;
      }
   }
}

if ( $sysname eq "OSF1" )
{
   $SO_SL_DLL = "so" ;
   $SYSTEM_NAME = "UNIX/DEC_64" ;
   $WDF_SYSTEM_NAME = "alphaosf" ;
}

if ( $sysname eq "HP-UX" )
{
   if ( $machine eq "ia64" )
   {
      $SO_SL_DLL = "so" ;

      $SYSTEM_NAME = "UNIX/HP11_IA64" ;
      $WDF_SYSTEM_NAME = "hpia64" ;
   }
   else
   {
      $SO_SL_DLL = "sl" ;

      if ( $ONLY_32_BIT )
      {
         $SYSTEM_NAME = "UNIX/HP11_32" ;
         $WDF_SYSTEM_NAME = "hp" ;
      }
      else
      {
         $SYSTEM_NAME = "UNIX/HP11_64" ;
         $WDF_SYSTEM_NAME = "hp_64" ;
      }
   }
}

if ( $sysname eq "Linux" )
{
   $SO_SL_DLL = "so" ;

   if ( $machine eq "ia64" )
   {
      $SYSTEM_NAME = "UNIX/LINUX_64" ;
      $WDF_SYSTEM_NAME = "linuxia64" ;
   }
   else
   {
      $SYSTEM_NAME = "UNIX/LINUX_32" ;
      $WDF_SYSTEM_NAME = "linuxintel" ;
   }
}

if ( $sysname eq "SunOS" )
{
   $SO_SL_DLL = "so" ;

   if ( $ONLY_32_BIT )
   {
      $SYSTEM_NAME = "UNIX/SUNOS_32" ;
      $WDF_SYSTEM_NAME = "sun" ;
   }
   else
   {
      $SYSTEM_NAME = "UNIX/SUNOS_64" ;
      $WDF_SYSTEM_NAME = "sun_64" ;
   }
}

if ( $CHECK )
{
   print "\$SO_SL_DLL       = $SO_SL_DLL\n" ;
   print "\$SYSTEM_NAME     = $SYSTEM_NAME\n" ;
   print "\$WDF_SYSTEM_NAME = $WDF_SYSTEM_NAME\n" ;
}


#------------------------------------------------------------------------------#
#   preparation of scratch make
#------------------------------------------------------------------------------#

prot_parts ( "prepare scratch make" ) ;

if ( $sysname eq "Windows NT" )
{
   @args_rmtree = ("del" , "/S" , "/Q") ;
}
else
{
   @args_rmtree = ( "rm" , "-rf" ) ;
}


if ( !( $ONLY_64_BIT) and !( $ONLY_32_BIT ) )
{
   if ( $sysname eq "Windows NT" )
   {
      push @args_rmtree , "sys\\wrk32" ;
   }
   else
   {
      push @args_rmtree , "sys/wrk32" ;
   }
}

if ( $sysname eq "Windows NT" )
{
   push @args_rmtree , ( "sys\\wrk" , "test" ) ;
}
else
{
   push @args_rmtree , ( "sys/wrk" , "usr" , "test" ) ;
}




if ( $CHECK )
{
   print "chdir \"$ENV{OWN}\" ;\n" ;
   print "system ( @args_rmtree ) ;\n" ;
}
else
{
   if ( $SCRATCH )
   {
      chdir "$ENV{OWN}" ;
      my $return = system ( @args_rmtree ) ;
   }
}


#------------------------------------------------------------------------------#
#   create protocol name of the make
#------------------------------------------------------------------------------#

prot_parts ( "build prot name" ) ;

$ProtDir="$ENV{OWN}/prot" ;
$MakeRelease=sprintf ("NACHT_%d%s%s_%s_%s_%02d%s%04d",
              $RV, $CORR_LEVEL, $BP, $X_BIT, $nodename,
              $localtime[3],
              (Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Oct,Nov,Dec)[$localtime[4]],
              $localtime[5] + 1900);
$MakeProt="$ProtDir/$MakeRelease.prot";

mkpath ( [ "$ProtDir" ], 0, 0775 ) ;

if ( $CHECK )
{
   print "\$MakeProt = $MakeProt\n" ;
}


#------------------------------------------------------------------------------#
#   redirect STDOUT and STDERR
#------------------------------------------------------------------------------#

prot_parts ( "redirect STDOUT and STDERR" ) ;

if ( $CHECK )
{
   print "redirection of STDOUT and STDERR\n" ;
}
else
{

   print "OUTPUT_FILTER: OPEN FILE $MakeProt\n";
#   open(SAVEOUT, ">&STDOUT") ;
#   open(SAVEERR, ">&STDERR") ;

#   close(STDOUT) ;
#   close(STDERR) ;

#   open(STDOUT, ">$MakeProt") ;
#   open(STDERR, ">&STDOUT") ;

#   select(STDERR); $| = 1;
#   select(STDOUT); $| = 1;
}


#------------------------------------------------------------------------------#
#   read changelist number
#------------------------------------------------------------------------------#

prot_parts ( "read changelist number" ) ;

if ( open ( CHANGELIST_LOG , "$ENV{OWN}/changelist.log" ))
{
   if ( $CHANGELIST_NO = <CHANGELIST_LOG> )
   {
      chomp ( $CHANGELIST_NO ) ;
   }
   else
   {
      print "WARNING: can\'t read changelist number!\n" ;
      $CHANGELIST_NO = 0;
   }
}
else
{
   print "WARNING: can\'t open changelist.log!\n" ;
   $CHANGELIST_NO = 0;
}

close ( CHANGELIST_LOG ) ;

if ( $CHECK )
{
   print "\$CHANGELIST_NO = $CHANGELIST_NO\n" ;
}


#------------------------------------------------------------------------------#
#   read tool changelist number
#------------------------------------------------------------------------------#

prot_parts ( "read tool changelist number" ) ;

if ( open ( TOOL_CHANGELIST_LOG , "$ENV{TOOL}/data/changelist.log" ))
{
   if ( $TOOL_CHANGELIST_NO = <TOOL_CHANGELIST_LOG> )
   {
      chomp ( $TOOL_CHANGELIST_NO ) ;
   }
   else
   {
      print "WARNING: can\'t read tool changelist number!\n" ;
      $TOOL_CHANGELIST_NO = 0;
   }
}
else
{
   print "WARNING: can\'t open $ENV{TOOL}/data/changelist.log!\n" ;
   $TOOL_CHANGELIST_NO = 0;
}

close ( TOOL_CHANGELIST_LOG ) ;

if ( $CHECK )
{
   print "\$TOOL_CHANGELIST_NO = $TOOL_CHANGELIST_NO\n" ;
}


#------------------------------------------------------------------------------#
#   create a new qadb entry
#------------------------------------------------------------------------------#

prot_parts ( "create new qadb entry" ) ;

if ( $CHECK )
{
   print "\$qah = qadb -> new ( {\'VERSION\'    => \"${RV}${CORR_LEVEL}\" ,\n" ;
   print "                      \'BUILDPFX\'   => \"$BP\" ,\n" ;
   print "                      \'QASTATUS\'   => \"$ENV{LC_STATE}\" ,\n" ;
   print "                      \'CHANGELIST\' => \"$CHANGELIST_NO\" ,\n" ;
   print "                      \'TOOLCL\'     => \"$TOOL_CHANGELIST_NO\" ,\n";
   print "                      \'PLATFORM\'   => \"$WDF_SYSTEM_NAME\" }   ) ;\n" ;
}
else
{
   $qah = qadb -> new ( {'VERSION'    => "${RV}${CORR_LEVEL}" ,
                         'BUILDPFX'   => "$BP" ,
                         'QASTATUS'   => "$ENV{LC_STATE}" ,
                         'CHANGELIST' => "$CHANGELIST_NO" ,
                         'TOOLCL'     => "$TOOL_CHANGELIST_NO" ,
                         'PLATFORM'   => "$WDF_SYSTEM_NAME" }   ) ;

   if ( $qah -> { error_code } != 0 )
   {
      $qah_open_ok = 0 ;
      print STDERR "ERROR: $qah -> { error_text }\n" ;
   }
}

#------------------------------------------------------------------------------#
#   update TOOL changelist number
#------------------------------------------------------------------------------#

update_qadb_cols ( "TOOLCL" , $TOOL_CHANGELIST_NO ) ;

#------------------------------------------------------------------------------#
#   storing MAKE ID into $OWN/qadb.id
#------------------------------------------------------------------------------#

prot_parts ( "store make id" ) ;
if ( $CHECK )
{
   print "make id into $ENV{OWN}/make.id\n" ;
}
else
{
   $qadb_id = $qah -> {'ID'} ;
   if ( $qah -> { error_code } != 0 )
   {
      $qah_open_ok = 0 ;
      print STDERR "ERROR: $qah -> { error_text }\n" ;
   }
   else
   {
      if ( open ( MAKEID_FILE , ">$ENV{OWN}/make.id" ))
      {
         print MAKEID_FILE $qadb_id ;
      }
      else
      {
         print "WARNING: can\'t open $ENV{OWN}/make.id!\n" ;
      }

      close ( MAKEID_FILE ) ;
   }
}


#------------------------------------------------------------------------------#
#   initial ID ( IDOBJSTATUS = 0 )
#------------------------------------------------------------------------------#

update_qadb_cols ( "IDOBJSTATUS" , 0 ) ;


#------------------------------------------------------------------------------#
#   make
#------------------------------------------------------------------------------#

prot_parts ( "start make" ) ;

@MAKE_TARGETS_32=() ;
@MAKE_TARGETS_FAST=() ;
@MAKE_TARGETS_QUICK=() ;
@MAKE_TARGETS_SLOW=() ;

if ( $CLI_BIT_32 )
{
   #---------------------------------------------------------------------------#
   #   storing client make start into qadb, start make
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $start_32bit_cli_make ) ;

   make_cli_32 ( 0 ) ;
   $MAKE_EXIT = make_cli_32 ( 1 ) ;

   #---------------------------------------------------------------------------#
   #   storing client make end into qadb
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $end_32bit_cli_make ) ;

   if ( $MAKE_EXIT )
   {
      #------------------------------------------------------------------------#
      #   storing client make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $ok_32bit_cli_make ) ;

      print "make 32 bit Client is ok\n" ;
   }
   else
   {
      #------------------------------------------------------------------------#
      #   storing client make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $NOK_32bit_cli_make ) ;

      print STDERR "ERROR: can\'t build 32 bit Client\n" ;
   }
}
elsif ( $PCR_BIT_32 )
{
   #---------------------------------------------------------------------------#
   #   storing precompiler runtime library make start into qadb, start make
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $start_32bit_pcr_make ) ;

   make_pcr_32 ( 0 ) ;
   $MAKE_EXIT = make_pcr_32 ( 1 ) ;

   #---------------------------------------------------------------------------#
   #   storing precompiler runtime library make end into
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $end_32bit_pcr_make ) ;

   if ( $MAKE_EXIT )
   {
      #------------------------------------------------------------------------#
      #   storing precompiler runtime library make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $ok_32bit_pcr_make ) ;

      print "make 32 bit precompiler runtime library is ok\n" ;
   }
   else
   {
      #------------------------------------------------------------------------#
      #   storing precompiler runtime library make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $NOK_32bit_pcr_make ) ;

      print STDERR "ERROR: can\'t build 32 precompiler runtime library\n" ;
   }
}
else
{
   #---------------------------------------------------------------------------#
   #   storing server make start into qadb, start make
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $start_server_make ) ;

   make_lc_oltp ( 0 , $ONLY_64_BIT ) ;
   $MAKE_EXIT = make_lc_oltp ( 1 , $ONLY_64_BIT ) ;

   #---------------------------------------------------------------------------#
   #   storing server make end into qadb
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $end_server_make ) ;

   if ( $MAKE_EXIT )
   {
      #------------------------------------------------------------------------#
      #   storing server make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $ok_server_make ) ;

      print "lC/SAP DB make is ok\n" ;
   }
   else
   {
      #------------------------------------------------------------------------#
      #   storing server make result into qadb
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $NOK_server_make ) ;

      print STDERR "ERROR: can\'t build lC/SAP DB\n" ;
   }
}

prot_parts ( "end make" ) ;


#------------------------------------------------------------------------------#
#   archive precompiler runtimes
#------------------------------------------------------------------------------#

if ( $MAKE_EXIT )
{
   prot_parts ( "precompiler runtimes" ) ;

   #---------------------------------------------------------------------------#
   #   maybe you are running a precompiler runtime library make only
   #   (see "only protocol informations")
   #---------------------------------------------------------------------------#

   $TO_CD_IMAGES_EXIT=1 ;

   #---------------------------------------------------------------------------#
   #   start of archiving
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $start_archiving_pcr ) ;

   $PCR_REL_EXIT = nacht_pcr_rel () ;

   #---------------------------------------------------------------------------#
   #   end of archiving
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $end_archiving_pcr ) ;


   if ( $PCR_REL_EXIT )
   {
      #------------------------------------------------------------------------#
      #   alright
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $ok_archiving_pcr ) ;

      print "copying of precompiler runtime is ok\n" ;
   }
   else
   {
      #------------------------------------------------------------------------#
      #   ERROR
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $NOK_archiving_pcr ) ;


      print STDERR "ERROR: can\'t copy precompiler runtime\n" ;
   }
}


#------------------------------------------------------------------------------#
#   build packages
#------------------------------------------------------------------------------#

if ( $MAKE_EXIT and ! $PCR_BIT_32 and $PCR_REL_EXIT )
{
   prot_parts ( "buildpackage" ) ;

   #---------------------------------------------------------------------------#
   #   start of buildpackage
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $start_buildpackage ) ;

   $TO_CD_IMAGES_EXIT=0 ;

   $TO_CD_IMAGES_EXIT = nacht_to_cd_images ( $CLI_BIT_32 ) ;

   #---------------------------------------------------------------------------#
   #   end of buildpackage
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $end_buildpackage ) ;


   if ( $TO_CD_IMAGES_EXIT )
   {
      #------------------------------------------------------------------------#
      #   alright
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $ok_buildpackage ) ;

      $LC_POOL_ID = get_lc_pool_id () ;

      print "build packages is ok\n" ;
   }
   else
   {
      #------------------------------------------------------------------------#
      #   ERROR
      #------------------------------------------------------------------------#

      update_qadb_cols ( "IDOBJSTATUS" , $NOK_buildpackage ) ;

      $LC_POOL_ID=0 ;

      print STDERR "ERROR: can\'t build packages\n" ;
   }
}


#------------------------------------------------------------------------------#
#   update LCPOOLID
#------------------------------------------------------------------------------#

update_qadb_cols ( "LCPOOLID" , $LC_POOL_ID ) ;


#------------------------------------------------------------------------------#
#   look for e0 files
#------------------------------------------------------------------------------#

prot_parts ( "prepare e0 files" ) ;

if ( $CLI_BIT_32 )
{
   if ( find_e0_files ( "cli_bit_32" ) )
   {
      print "fast only: found e0 files ok\n" ;
   }
   else
   {
      print STDERR "ERROR: (fast only) can\'t find e0 files\n" ;
   }
}
elsif ( $PCR_BIT_32 )
{
   if ( find_e0_files ( "pcr_bit_32" ) )
   {
      print "fast only: found e0 files ok\n" ;
   }
   else
   {
      print STDERR "ERROR: (fast only) can\'t find e0 files\n" ;
   }
}
else
{
   if ( find_e0_files ( "all" ) )
   {
      print "found e0 files ok\n" ;
   }
   else
   {
      print STDERR "ERROR: can\'t find e0 files\n" ;
   }
}


#------------------------------------------------------------------------------#
#   send mail
#------------------------------------------------------------------------------#

prot_parts ( "send mail" ) ;

if ( $CHECK )
{
   print "system ( checkmake ... )\n" ;
}
else
{
   my $return ;
   my @arg_checkmake = ( "$ENV{TOOLSHELL}", "$ENV{'TOOL'}/bin/checkmake.pl" ) ;

   push @arg_checkmake , @MAKE_TARGETS_WITH_PATH ;

   $return = system ( @arg_checkmake ) ;

   if (( !defined $return ) and ( $return != 0 ))
   {
      print STDERR "ERROR: file \"checkmake.pl\" failed\n";
   }
}


#------------------------------------------------------------------------------#
#   only protocol informations
#------------------------------------------------------------------------------#

print "\n\nwrite protocoll file \"$MakeRelease.prot\" into qadb\n" ;

if ( $MAKE_EXIT and $PCR_REL_EXIT and $TO_CD_IMAGES_EXIT )
{
   print "\nMAIN EXIT: Congratulations!!! No make errors occured!\n" ;

   #---------------------------------------------------------------------------#
   #   storing results into qadb ( IDOBJSTATUS = OK )
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $MAKE_OK ) ;

}
else
{
   #---------------------------------------------------------------------------#
   #   storing results into qadb ( IDOBJSTATUS = NOK )
   #---------------------------------------------------------------------------#

   update_qadb_cols ( "IDOBJSTATUS" , $MAKE_NOK ) ;

   print "\nMAIN EXIT: ATTENTION---ERRORS---ATTENTION---ERRORS---ATTENTION\n" ;
   print "MAIN EXIT: see above\n" ;
}


#------------------------------------------------------------------------------#
#   reset STDOUT and STDERR
#------------------------------------------------------------------------------#

prot_parts ( "reset STDOUT and STDERR" ) ;

if ( $CHECK )
{
   print "reset STDOUT and STDERR\n" ;
}
else
{
    print "OUTPUT_FILTER: CLOSE FILE\n";
#   close ( STDOUT );
#   close ( STDERR );

#   open ( STDOUT , ">&SAVEOUT" ) ;
#   open ( STDERR , ">&SAVEERR" ) ;
}


#------------------------------------------------------------------------------#
#   write out make protocol to STDOUT and into qadb
#------------------------------------------------------------------------------#

if ( $CHECK )
{
   print "write out make protocol to STDOUT and into qadb\n" ;
   print "protocol file \"$MakeRelease.prot\"\n" ;
}
else
{
   open( IN , "<$MakeProt" ) ;

   while ( <IN> )
   {
#      print $_ ;
      $prot_text .= $_ ;
   }

   close ( IN ) ;

   if ( $qah_open_ok )
   {
      if ( $qah -> write_prot ( "$MakeRelease.prot" , $prot_text ) != 0 )
      {
         my $errtxt = $qah -> { error_text } ;
         print STDERR "ERROR: can\'t write prot: $errtxt\n" ;
      }
   }
   else
   {
      print "WARNING: don't write prot because of an open error\n" ;
   }
}


#------------------------------------------------------------------------------#
#   END, return MAIN exit code
#------------------------------------------------------------------------------#

if ( $MAKE_EXIT and $PCR_REL_EXIT and $TO_CD_IMAGES_EXIT )
{
   $MAIN_EXIT = 0 ;

}
else
{
   $MAIN_EXIT = 1 ;
}

if ( $CHECK )
{
   print "main: \$MAKE_EXIT         = $MAKE_EXIT\n" ;
   print "main: \$PCR_REL_EXIT      = $PCR_REL_EXIT\n" ;
   print "main: \$TO_CD_IMAGES_EXIT = $TO_CD_IMAGES_EXIT\n" ;
   print "main: \$MAIN_EXIT         = $MAIN_EXIT\n" ;
}

caller () ? return ( $MAIN_EXIT ) : exit ( $MAIN_EXIT ) ;

################################################################################
################## subroutines #################################################
################################################################################

sub print_usage
{
   print "usage: $0\n\n";
   print "       [-force_rm]\n" ;
   print "          initialise a scratch make\n\n" ;
   print "       [-cli_bit_32]\n" ;
   print "          make the 32 bit client package\n\n" ;
   print "       [-pcr_bit_32]\n" ;
   print "          make the 32 bit precompiler runtime library\n\n" ;
   print "       [-make_type=<OLTP | LC | LC_OLTP>]\n" ;
   print "          specifies the type of the make, default is LC_OLTP\n\n" ;
   print "       [-check]\n" ;
   print "          checks and shows only the workflow, don't execute anything\n\n" ;
   print "       [-help | -h]\n" ;
   print "          show this help message\n" ;
}


################################################################################

sub prot_parts
{
   my $prot_part = shift ;

   print "\n\n===========================================================\n" ;
   print "=   $prot_part\n" ;
   print "===========================================================\n\n" ;
}


################################################################################

sub get_lc_pool_id
{
   my $sym_link ;
   my $sym_link_version ;
   my $LB=-1 ;

   if ( $ENV{LC_STATE} eq "RAMP" )
   {
      $sym_link_version = "LC_${RV}${CORR_LEVEL}${BP}_${X_BIT}_$ENV{LC_STATE}" ;
   }
   else
   {
      $sym_link_version = "LC_${RV}${CORR_LEVEL}_${X_BIT}_$ENV{LC_STATE}" ;
   }

   $sym_link = "/LC_POOL/" . "$sym_link_version" . "/" . "LastBuild" ;


   if ( $sysname eq "Windows NT" )
   {

      if ( $ENV{LC_POOL_ROOT} )
      {
         $sym_link = "$ENV{LC_POOL_ROOT}/" . "$sym_link_version" . "/" . "LastBuild" ;
      }

      if ( open ( WIN_LastBuild_file , "$sym_link" ))
      {
         if ( $LB = <WIN_LastBuild_file> )
         {
            chomp ( $LB ) ;
         }
         else
         {
            print "WARNING: can\'t read LastBuild!\n" ;
            $LB = 0 ;
         }
      }
      else
      {
         print "WARNING: can\'t open $sym_link!\n" ;
         $LB = 0 ;
      }

      close ( WIN_LastBuild_file ) ;

   }
   else
   {
      $LB = readlink ( $sym_link ) ;
   }

   if ( $CHECK )
   {
      print "\$sym_link = $sym_link\n" ;
      print "\$LB       = $LB\n" ;
   }

   $LB ? return ( $LB ) : return ( -1 ) ;
}


################################################################################

sub update_qadb_cols
{
   my $col_name  = shift ;
   my $col_value = shift ;

   if ( $CHECK )
   {
      print "\$qah_open_ok = $qah_open_ok\n" ;
      print "\$qah -> update_columns ( {\"$col_name\" => \"$col_value\"} )\n" ;
   }
   else
   {
      if ( $qah_open_ok )
      {
         if ( $qah -> update_columns ( {"$col_name" => "$col_value"} ) != 0 )
         {
            my $errtxt = $qah -> { error_text } ;
            print STDERR "ERROR: can\'t update $col_name: $errtxt\n" ;
         }
      }
      else
      {
         print "WARNING: don't update $col_name because of an open error\n" ;
      }
   }
}


################################################################################

sub nacht_to_cd_images
{
   my $CLI_32 = shift ;
   my $PKG='srv' ;
   my $PROFILE1 ;
   my $PTYPE='sar' ;
   my $FTP='-ftp' ;
   my $PCR='' ;
   my $EXIT=0 ;
   my $ret1=1 ;
   my $ret2=1 ;

   if ( $CLI_32 )
   {
      $PROFILE1="Base,PCR $RV$CORR_LEVEL,Server Utilities" ;
   }
   else
   {
      $PROFILE1="Server" ;
   }

   if ( $CHECK )
   {
      print "nacht_to_cd_images\n" ;
      print "\$CLI_32 = $CLI_32\n" ;
      print "imf srvinst\n" ;
      print "imf -U ::install/perl/allpackages\n" ;
      print "chdir \"$ENV{OWN}/sys/wrk/fast/install\"\n" ;
      if ( $CLI_32 )
      {
         print "buildpackage.pl -package \"$PROFILE1\" -finalize\n" ;
      }
      else
      {
         if (( $sysname eq "Linux" ) and
             ((( $RV == 74 ) and ($ENV{CORRECTION_LEVEL} == 3 ) and ( $BP >= 10 )) or
              (( $RV == 74 ) and ($ENV{CORRECTION_LEVEL}  > 3 )) or
              (  $RV  > 74 )))
         {
            print "buildpackage.pl -profile \"$PROFILE1\" -format \"$PTYPE\" -finalize -components -rpm\n" ;
         }
         else
         {
            print "buildpackage.pl -profile \"$PROFILE1\" -format \"$PTYPE\" -finalize -components\n" ;
         }
      }
      push @MAKE_TARGETS_FAST , "srvinst.mac" ;
      push @MAKE_TARGETS_FAST , "install/perl/allpackages.mac" ;
      $EXIT=1 ;
   }
   else
   {
      eval
      {
         $ret1 = imf ( "srvinst.mac" ) ;
      }; $ret1 = 1 if $@ ;
      push @MAKE_TARGETS_FAST , "srvinst.mac" ;

      my @arg = ( "-U" , "::install/perl/allpackages.mac" ) ;
      eval
      {
         $ret2 = imf ( @arg ) ;
      }; $ret2 = 1 if $@ ;
      push @MAKE_TARGETS_FAST , "install/perl/allpackages.mac" ;

      if ((( defined $ret1 ) and ( $ret1 == 0 )) and
          (( defined $ret2 ) and ( $ret2 == 0 ))     )
      {
         $EXIT=1 ;
         chdir "$ENV{OWN}/sys/wrk/fast/install" ;

         if (( $RV >= 74 ) and ( $ENV{CORRECTION_LEVEL} >= 1 ))
         {
            if ( $CLI_32 )
            {
               @arg = ("buildpackage.pl", "-package",
                                               "$PROFILE1", "-finalize" ) ;
            }
            else
            {
               if (( $sysname eq "Linux" )
                           and
                   ((( $RV == 74 ) and ($ENV{CORRECTION_LEVEL} == 3 ) and ( $BP >= 10 ))
                                or
                    (( $RV == 74 ) and ($ENV{CORRECTION_LEVEL}  > 3 ))
                                or
                    (  $RV  > 74 )))
               {
                  @arg = ("buildpackage.pl" , "-profile", "$PROFILE1",
                     "-format", "$PTYPE", "-finalize", "-components", "-rpm") ;
               }
               else
               {
                  @arg = ("buildpackage.pl" , "-profile", "$PROFILE1",
                     "-format", "$PTYPE", "-finalize", "-components") ;
               }
            }

            $ret1 = 0xffff & system ( @arg ) ;
            if ( $ret1 != 0 )
            {
               print STDERR "ERROR in buildpackage!\n" ;
               $EXIT=0 ;
            }
         }
         else
         {
            print "currently there are no packages available\n" ;
            $EXIT=0 ;
         }
      }
      else
      {  $EXIT=0 ;
         print STDERR "ERROR: can't make allpackages and srvinst\n";
      }
   }

   return ( $EXIT ) ;
}


################################################################################

sub nacht_pcr_rel
{
   my $LIB64='' ;

   if ( defined $ENV{BIT64}         and
        ($sysname ne "OSF1" )       and
        ($sysname ne "Linux")       and
        ($sysname ne "Windows NT" )     )
   {
      $LIB64="lib64" ;
   }

   my $SOURCE="." ;
   my $DEST_PCR="d:/CD-Images/PCR_REL" ;
   if ( $sysname eq "Windows NT" ) { $DEST_PCR=~s/\//\\/g ; }

   my $DEST="$DEST_PCR/$RV$CORR_LEVEL$BP/$SYSTEM_NAME/lib/$LIB64" ;
   if ( $sysname eq "Windows NT" )
   {
      $DEST= "$DEST_PCR\\$RV$CORR_LEVEL$BP\\$SYSTEM_NAME" ;
   }

   my $DEST_SERVER="p26325.bea.sap.corp" ;
   my @FILE_LIST ;

   my $N='' ;
   my $DBG_PDB='dbg' ;
   my $PGM32_64='pgm' ;
   if ( $sysname eq "Windows NT" )
   {
      if (( defined $ENV{BIT64} ) or ( $ENV{_MSC_VER} >= 13 ))
      {
         $DBG_PDB='pdb' ;
      }

      if ( defined $ENV{BIT64} )
      {
         $PGM32_64='pgm\\pgm64' ;
      }

      if (( $RV >= 74 ) and ($ENV{CORRECTION_LEVEL} >= 4 ))
      {
         $N='n' ;
      }

      @FILE_LIST =
      (
        [ "runtime\\$RV$CORR_LEVEL\\$PGM32_64\\libpcr.$SO_SL_DLL" , "pgm\\libpcr.$SO_SL_DLL" ] ,
        [ "pgm\\sqluser$RV$N.$SO_SL_DLL" , "pgm\\sqluser$RV$N.$SO_SL_DLL" ] ,
        [ "Support\\Mapfiles\\libpcr.map", "Support\\Mapfiles\\libpcr.map" ] ,
        [ "Support\\Mapfiles\\sqluser$RV$N.map" , "Support\\Mapfiles\\sqluser$RV$N.map" ],
        [ "symbols\\libpcr.$DBG_PDB" , "symbols\\libpcr.$DBG_PDB" ] ,
        [ "symbols\\sqluser$RV$N.$DBG_PDB" , "symbols\\sqluser$RV$N.$DBG_PDB" ]
      ) ;


   }
   else
   {
      @FILE_LIST="libpcr.$SO_SL_DLL" ;
   }
   my $RUNTIME ;
   if ( $sysname eq "Windows NT" )
   {
      $RUNTIME="$ENV{OWN}\\usr" ;
   }
   else
   {
      if   ( $RV == 72 )
      {
         $RUNTIME="$ENV{OWN}/usr/lib/$LIB64" ;
      }
      else
      {
         $RUNTIME="$ENV{OWN}/usr/runtime/$RV$CORR_LEVEL/lib/$LIB64" ;
      }
   }


   if ( $CHECK )
   {
      print "nacht_prc_rel\n" ;
      print "\$SOURCE      = $SOURCE\n" ;
      print "\$DEST_PCR    = $DEST_PCR\n" ;
      print "\$DEST        = $DEST\n" ;
      print "\$DEST_SERVER = $DEST_SERVER\n" ;
      if ( $sysname eq "Windows NT" )
      {
         for $ARRAY_REF ( @FILE_LIST )
         {
            print "\@FILE_LIST   = @$ARRAY_REF\n" ;
         }
      }
      else
      {
         print "\@FILE_LIST   = @FILE_LIST\n" ;
      }
      print "\$RUNTIME     = $RUNTIME\n" ;
      print "rem -u mput ${DEST_SERVER} ${SOURCE} ${DEST} \"ls \@FILE_LIST\"\n" ;

      $EXIT=1 ;
   }
   else
   {
      my $ret1 ;
      my @arg = () ;
      my $i ;

      $EXIT=1 ;
      chdir "$RUNTIME" ;

      if ( $sysname eq "Windows NT" )
      {
         for $i ( 0 .. $#FILE_LIST )
         {
            @arg = ( "_rem" , "-u" , "put" , "${DEST_SERVER}" ,
                     "$FILE_LIST[$i][0]" ,
                     "${DEST}\\$FILE_LIST[$i][1]" );

            $ret1 = 0xffff & system ( @arg ) ;
            if ( $ret1 != 0 )
            {
               print STDERR "ERROR in \"rem mput ...\" (Windows)!\n" ;
               print STDERR "$FILE_LIST[$i][0]\n" ;
               $EXIT=0 ;
            }
         }
      }
      else
      {
         @arg = ( "rem" , "-u" , "mput" , "${DEST_SERVER}" , "${SOURCE}" ,
                  "${DEST}" , "ls @{FILE_LIST}" );

         $ret1 = 0xffff & system ( @arg ) ;
         if ( $ret1 != 0 )
         {
            print STDERR "ERROR in \"rem mput ...\" (Unix/Linux)!\n" ;
            $EXIT=0 ;
         }
      }
   }

   return ( $EXIT ) ;
}

################################################################################

sub make_cli_32
{
   my $SECOND = shift ;
   my $EXIT=0 ;
   my $IMF_EXIT=1 ;
   my @argv_32bit_targets = ( "dbmcli.lnk" ,
                              "dbmgetf.lnk" ,
                              "dbmrfc.lnk" ,
                              "irconf.lnk" ,
                              "irtrace.lnk" ,
                              "libpcr.dld" ,
                              "allrtextr.mac" ,
                              "allrte.mac" ,
                              "cpc.mac" ,
                              "pctools.mac" ,
                              "vttest.lnk" ,
                              "VERSIONS.er" );

   if ( $SECOND )
   {
      push @MAKE_TARGETS_FAST , @argv_32bit_targets ;

      if (( $RV != 73 ) and ($RV > 71 ))
      {
         push @MAKE_TARGETS_FAST , "cn/sdbinfo/sdbinfo.lnk" ;
      }
   }

   if (( $RV != 73 ) and ($RV > 71 ))
   {
      push @argv_32bit_targets , "::cn/sdbinfo/sdbinfo.lnk" ;
   }

   if ( $CHECK )
   {
      print "make_cli_32\n" ;
      print "\$IMF_EXIT          = imf (@argv_32bit_targets) ;\n" ;
      print "\$SECOND            = $SECOND\n" ;
      print "\@MAKE_TARGETS_FAST = @MAKE_TARGETS_FAST\n" ;
      $IMF_EXIT=0 ;
      $EXIT=1 ;
   }
   else
   {
      eval
      {
         {
            $IMF_EXIT = imf (@argv_32bit_targets) ;
         }

         if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
         {
            $EXIT = 0 ;
         }
         else
         {
            $EXIT = 1 ;
         }
      }; $EXIT=1 if $@ ;
   }

   return ( $EXIT ) ;
}


################################################################################

sub make_pcr_32
{
   my $SECOND = shift ;
   my $EXIT=0 ;
   my $IMF_EXIT=1 ;
   my @argv_32bit_targets = ( "libpcr.dld" );

   if ( $SECOND )
   {
      push @MAKE_TARGETS_FAST , @argv_32bit_targets ;
   }


   if ( $CHECK )
   {
      print "make_pcr_32\n" ;
      print "\$IMF_EXIT          = imf (@argv_32bit_targets) ;\n" ;
      print "\$SECOND            = $SECOND\n" ;
      print "\@MAKE_TARGETS_FAST = @MAKE_TARGETS_FAST\n" ;
      $IMF_EXIT=0 ;
      $EXIT=1 ;
   }
   else
   {
      eval
      {
         {
            $IMF_EXIT = imf (@argv_32bit_targets) ;
         }

         if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
         {
            $EXIT = 0 ;
         }
         else
         {
            $EXIT = 1 ;
         }
      }; $EXIT=1 if $@ ;
   }

   return ( $EXIT ) ;
}


################################################################################

sub make_lc_oltp
{
   my $SECOND=shift ;
   my $ONLY_64_BIT=shift ;
   my $EXIT=1 ;
   my $IMF_EXIT=1 ;
   my @argv_targets = ();

   if ( $CHECK )
   {
      print "make_lc_oltp\n" ;
      print "\$ONLY_64_BIT       = $ONLY_64_BIT\n" ;
      print "\$ret               = imf (\@argv_targets) ;\n" ;
      print "\@MAKE_TARGETS_FAST = @MAKE_TARGETS_FAST\n" ;
      #### simulation ####
      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST  , "libsqlptc.rel" ;
         push @MAKE_TARGETS_FAST  , "kernel.shm" ;
         push @MAKE_TARGETS_FAST  , "dcom.mac" ;
         push @MAKE_TARGETS_FAST  , "all.mac" ;
         push @MAKE_TARGETS_32    , "all32.mac" ;
         push @MAKE_TARGETS_QUICK , "quickknl.shm" ;
         push @MAKE_TARGETS_SLOW  , "slowknl.shm" ;
      }
      $EXIT=1 ;
   }
   else
   {
      $EXIT=1 ;

      eval
      {
         $IMF_EXIT = imf ( 'libsqlptc.rel' );
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , "libsqlptc.rel" ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make libsqlptc\n" ;
      }
      else
      {
         if ( $ONLY_64_BIT )
         {
            File::Copy::copy("$ENV{OWN}/sys/wrk/fast/obj/libsqlptc.rel.o" ,
                             "$ENV{TOOL}/lib/lib64" );
         }
         else
         {
            File::Copy::copy("$ENV{OWN}/sys/wrk/fast/obj/libsqlptc.rel.o" ,
                             "$ENV{TOOL}/lib" );
         }
      }



      if (( ! $ONLY_64_BIT ) and ( ! $ONLY_32_BIT ))
      {
         @argv_targets = ("-32" , "all32.mac" ) ;
         eval
         {
            $IMF_EXIT = imf ( @argv_targets ) ;
         } ; $IMF_EXIT = 1 if $@ ;

         if ( $SECOND )
         {
            push @MAKE_TARGETS_32 , "all32.mac" ;
         }

         if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
         {
            $EXIT=0 ;
            print STDERR "ERROR: can't make all32\n" ;
         }
      }



      @argv_targets = ("kernel.shm" , "dcom.mac" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , @argv_targets ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT !=0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make kernel, dcom\n" ;
      }



      @argv_targets = ( "all.mac" , "allbw.mac" , "ci.mac" , "mut.mac" ,
                        "alltest.mac" , "jtestinst.mac" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , @argv_targets ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make all, allbw, ci, mut alltest, jtestinst\n" ;
      }



      @argv_targets = ("alltestpkg.mac" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , @argv_targets ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make alltestpkg\n" ;
      }



      @argv_targets = (":tt/makeCARLIST" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , "tt/makeCARLIST" ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make :tt/makeCARLIST\n" ;
      }



      @argv_targets = ( "sapdbinst.mac" , "srvinst.mac" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , @argv_targets ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make sapdbinst, srvinst\n" ;
      }



      @argv_targets = ("VERSIONS.er" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , @argv_targets ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make VERSIONS.er\n" ;
      }



      eval
      {
         $IMF_EXIT = imq ( "quickknl.shm" ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_QUICK , "quickknl.shm" ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make quickknl\n" ;
      }



      eval
      {
         $IMF_EXIT = ims ( "slowknl.shm" ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_SLOW , "slowknl.shm" ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make slowknl\n" ;
      }



      eval
      {
         $IMF_EXIT = imf ( "sut.mac" ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if ( $SECOND )
      {
         push @MAKE_TARGETS_FAST , "sut.mac" ;
      }

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT != 0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make sut\n" ;
      }
   }

   return ( $EXIT ) ;
}


################################################################################

sub find_e0_files
{
   my $ONLY_FAST= shift ;
   my $EXIT=1 ;
   my $FAST_32_PROT="$ENV{OWN}/sys/wrk32/fast/prot/" ;
   my $FAST_PROT="$ENV{OWN}/sys/wrk/fast/prot/" ;
   my $QUICK_PROT="$ENV{OWN}/sys/wrk/quick/prot/" ;
   my $SLOW_PROT="$ENV{OWN}/sys/wrk/slow/prot/" ;

   @MAKE_TARGETS_WITH_PATH=() ;

   if ( $CHECK )
   {
      print "find_e0_files\n" ;
      print "\@MAKE_TARGETS_32        = @MAKE_TARGETS_32\n" ;
      print "\@MAKE_TARGETS_FAST      = @MAKE_TARGETS_FAST\n" ;
      print "\@MAKE_TARGETS_QUICK     = @MAKE_TARGETS_QUICK\n" ;
      print "\@MAKE_TARGETS_SLOW      = @MAKE_TARGETS_SLOW\n" ;
      print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ;
   }

   if ( @MAKE_TARGETS_FAST )
   {
      foreach my $target ( @MAKE_TARGETS_FAST )
      {
         push @MAKE_TARGETS_WITH_PATH ,"$FAST_PROT"."$target".".e0" ;
      }
   }


   if ( $ONLY_FAST eq "all" )
   {
      if (( ! $ONLY_64_BIT ) and ( ! $ONLY_32_BIT ))
      {
         if ( @MAKE_TARGETS_32 )
         {
            foreach my $target ( @MAKE_TARGETS_32 )
            {
               push @MAKE_TARGETS_WITH_PATH ,"$FAST_32_PROT"."$target".".e0" ;
            }
         }
      }


      if ( @MAKE_TARGETS_QUICK )
      {
         foreach my $target ( @MAKE_TARGETS_QUICK )
         {
            push @MAKE_TARGETS_WITH_PATH ,"$QUICK_PROT"."$target".".e0" ;
         }
      }


      if ( @MAKE_TARGETS_SLOW )
      {
         foreach my $target ( @MAKE_TARGETS_SLOW )
         {
            push @MAKE_TARGETS_WITH_PATH ,"$SLOW_PROT"."$target".".e0" ;
         }
      }
   }


   if ( $CHECK )
   {
      print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ;
   }

   return ( $EXIT ) ;
}


################## subroutines #################################################