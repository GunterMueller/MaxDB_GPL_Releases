#!/usr/bin/perl
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

################################################################################
################## main ########################################################
################################################################################

select(STDERR); $| = 1;
select(STDOUT); $| = 1;

my $SCRATCH ;
my $CLI_BIT_32 ;
my $IA_64 ;
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
my $SO_SL_DLL ;

my $ProtDir="$ENV{OWN}/tmp" ;
my $MakeRelease='NACHT_ALL' ;
my $MakeProt="$ProtDir/$MakeRelease.prot";

my @localtime = localtime (time);


#--- read options--------------------------------------------------------------#

Getopt::Long::Configure ("no_auto_abbrev");

if ( GetOptions ( "force_rm"   => \$SCRATCH,
                  "cli_bit_32" => \$CLI_BIT_32,
                  "ia64"       => \$IA_64,
                  "check"      => \$CHECK,
                  "help"       => \$HELP,
                  "h"          => \$HELP )  and not  $HELP )
{
   if ( defined $SCRATCH) { $SCRATCH = 1 } ;
   
   if ( defined $CLI_BIT_32 )
   {
      $CLI_BIT_32  = 1 ;
      $ONLY_32_BIT=1 ;
      $X_BIT='32' ;
   }

   if ( defined $IA_64 ) { $IA_64 = 1 };

   if ( defined $CHECK ) { $CHECK = 1 };
}
else
{
   print "usage: $0 [-force_rm] [-cli_bit_32] [-ia64] [-check] [-help | -h]\n" ;
   caller () ? return ( 1 ) : exit ( 1 ) ;
}


if ( $CHECK and $CLI_BIT_32 )
{
   print "\$CLI_BIT_32 = $CLI_BIT_32\n" ;
}

if ( $CHECK and $SCRATCH )
{
   print "\$SCRATCH    = $SCRATCH\n" ;
}


#--- get some machine informations --------------------------------------------#

( $sysname, $nodename, $release, $version, $machine ) = POSIX::uname();


if ( $CHECK )
{
   print "\$sysname  = $sysname\n" ;
   print "\$nodename = $nodename\n" ;
   print "\$release  = $release\n" ;
   print "\$version  = $version\n" ;
   print "\$machine  = $machine\n" ;
}


#--- only 64-bit make ---------------------------------------------------------#

if ((( $sysname eq "OSF1" )  and ( $machine eq "alpha" ))  or
    (( $sysname eq "HP-UX" ) and ( $machine eq "ia64" ))      )
{
   $ONLY_64_BIT=1 ;
   $X_BIT='64' ;

   if ( $CLI_BIT_32 )
   {
      $CLI_BIT_32  = 0 ;
      print "WARNING: no 32 bit client makes allowed\n" ;
   }
}

if ( $CHECK )
{
   print "\$ONLY_64_BIT = $ONLY_64_BIT\n" ;
   print "\$X_BIT       = $X_BIT\n" ;
   print "\$$CLI_BIT_32 = $CLI_BIT_32 \n" ;
}


#--- only 32-bit make ---------------------------------------------------------#

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


#--- only OLTP make -----------------------------------------------------------#

if (( $sysname eq "Linux" ) and ( $machine eq "i686" ))
{
   $OLTP_ONLY=1 ;
}

if ( $CHECK )
{
   print "\$OLTP_ONLY = $OLTP_ONLY\n" ;
}


#--- system name for precompiler runtime archive ------------------------------#

if ( $sysname eq "Windows NT" )
{
   $SO_SL_DLL = "dll" ;

   if ( $IA_64 )
   {
      $SYSTEM_NAME = "NT/IA64" ;
   }
   else
   {
      $SYSTEM_NAME = "NT/I386" ;
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
      }
      else
      {
         $SYSTEM_NAME = "UNIX/AIX_64" ;
      }
   }
   else
   {
      if ( $ONLY_32_BIT )
      {
         $SYSTEM_NAME = "UNIX/AIX_32" ;
      }
      else
      {
         $SYSTEM_NAME = "UNIX/AIX5_64" ;
      }
   }
}

if ( $sysname eq "OSF1" )
{
   $SO_SL_DLL = "so" ;
   $SYSTEM_NAME = "UNIX/DEC_64" ;
}

if ( $sysname eq "HP-UX" )
{
   if ( $machine eq "ia64" )
   {
      $SO_SL_DLL = "so" ;

      $SYSTEM_NAME = "UNIX/HP11_IA64" ;
   }
   else
   {
      $SO_SL_DLL = "sl" ;

      if ( $ONLY_32_BIT )
      {
         $SYSTEM_NAME = "UNIX/HP11_32" ;
      }
      else
      {
         $SYSTEM_NAME = "UNIX/HP11_64" ;
      }
   }
}

if ( $sysname eq "Linux" )
{
   $SO_SL_DLL = "so" ;

   if ( $machine eq "ia64" )
   {
      $SYSTEM_NAME = "UNIX/LINUX_64" ;
   }
   else
   {
      $SYSTEM_NAME = "UNIX/LINUX_32" ;
   }
}

if ( $sysname eq "SunOS" )
{
   $SO_SL_DLL = "so" ;

   if ( $ONLY_32_BIT )
   {
      $SYSTEM_NAME = "UNIX/SUNOS_32" ;
   }
   else
   {
      $SYSTEM_NAME = "UNIX/SUNOS_64" ;
   }
}

if ( $CHECK )
{
   print "\$SO_SL_DLL   = $SO_SL_DLL\n" ;
   print "\$SYSTEM_NAME = $SYSTEM_NAME\n" ;
}


#--- check $RELVER, $CORRECTION_LEVEL and $BUILDPRAEFIX -----------------------#

if ( exists $ENV{RELVER} )
{
   if    ( $ENV{RELVER} eq "R72" ) { $RV = 72 ; }
   elsif ( $ENV{RELVER} eq "R73" ) { $RV = 73 ; }
   elsif ( $ENV{RELVER} eq "R74" ) { $RV = 74 ; }
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
   if   (( $CORR_LEVEL <= 9 ) and ( $CORR_LEVEL >= 0 ))
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
   if   (( $BP <= 9 ) and ( $BP >= 0 ))
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


#--- make ---------------------------------------------------------------------#

@MAKE_TARGETS_32=() ;
@MAKE_TARGETS_FAST=() ;
@MAKE_TARGETS_QUICK=() ;
@MAKE_TARGETS_SLOW=() ;

if ( $CLI_BIT_32 )
{
   make_cli_32 ( 0 ) ;
   $MAKE_EXIT = make_cli_32 ( 1 ) ;
   if ( $MAKE_EXIT )
   {
      print "make 32 bit Client is ok\n" ;
   }
   else
   {
      print STDERR "ERROR: can\'t build 32 bit Client\n" ;
   }
}
else
{
   make_lc_oltp ( 0 , $ONLY_64_BIT ) ;
   $MAKE_EXIT = make_lc_oltp ( 1 , $ONLY_64_BIT ) ;
   if ( $MAKE_EXIT )
   {
      print "lC/SAP DB make is ok\n" ;
   }
   else
   {
      print STDERR "ERROR: can\'t build lC/SAP DB\n" ;
   }
} 


#--- only protocol informations -----------------------------------------------#

if ( $MAKE_EXIT and $PCR_REL_EXIT and $TO_CD_IMAGES_EXIT )
{
   print "\nMAIN EXIT: Congratulations!!! No errors occured!\n" ;
}
else
{
   print "\nMAIN EXIT: ATTENTION---ERRORS---ATTENTION---ERRORS---ATTENTION\n" ;
   print "MAIN EXIT: see above\n" ;
}


#--- END ----------------------------------------------------------------------#

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
   my $LIB64 ;
   if ( defined $ENV{BIT64} and ($sysname ne "OSF1" ))
   {
      $LIB64="lib64" ;
   }
   else
   {
      $LIB64="" ;
   }
   my $SOURCE="." ;
   my $DEST_PCR="d:/CD-Images/PCR_REL" ;
   my $DEST="$DEST_PCR/$RV$CORR_LEVEL$BP/$SYSTEM_NAME/lib/$LIB64" ;
   my $DEST_SERVER="p26325.bea.sap.corp" ;
   my $FILE_LIST ;
   if ( $sysname eq "Windows NT" )
   {
      $FILE_LIST="" ;
   }
   else
   {
      $FILE_LIST="libpcr.$SO_SL_DLL" ;
   }
   my $RUNTIME ;
   if   ( $RV == 72 )
   {
      $RUNTIME="$ENV{OWN}/usr/lib/$LIB64" ;
   }
   else
   {
      $RUNTIME="$ENV{OWN}/usr/runtime/$RV$CORR_LEVEL/lib/$LIB64" ;
   }

   if ( $CHECK )
   {
      print "nacht_prc_rel\n" ;
      print "\$SOURCE      = $SOURCE\n" ;
      print "\$DEST_PCR    = $DEST_PCR\n" ;
      print "\$DEST        = $DEST\n" ;
      print "\$DEST_SERVER = $DEST_SERVER\n" ;
      print "\$FILE_LIST   = $FILE_LIST\n" ;
      print "\$RUNTIME     = $RUNTIME\n" ;
      if ( $ENV{LC_STATE} eq "RAMP" )
      {
         print "rem mput ${DEST_SERVER} ${SOURCE} ${DEST} \"ls ${FILE_LIST}\"\n" ;
      }
      else
      {
         print "rem -u mput ${DEST_SERVER} ${SOURCE} ${DEST} \"ls ${FILE_LIST}\"\n" ;
      }

      $EXIT=1 ;
   }
   else
   {
      if ( $sysname eq "Windows NT" )
      {
         #### Windows not yet implemented ####
         return ( 1 ) ;
      }

      $EXIT=1 ;

      chdir "$RUNTIME" ;

      my @arg = () ;
      if ( $ENV{LC_STATE} eq "RAMP" )
      {
         @arg = ( "rem" , "mput" , "${DEST_SERVER}" , "${SOURCE}" , "${DEST}" ,
                  "ls ${FILE_LIST}" );
      }
      else
      {
         @arg = ( "rem" , "-u" , "mput" , "${DEST_SERVER}" , "${SOURCE}" ,
                  "${DEST}" , "ls ${FILE_LIST}" );
      }

      my $ret1 = 0xffff & system ( @arg ) ;
      if ( $ret1 != 0 )
      {
         print STDERR "ERROR in \"rem mput ...\"!\n" ;
         $EXIT=0 ;
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
                              "VERSIONS.er");

   if ( $SECOND )
   {
      push @MAKE_TARGETS_FAST , @argv_32bit_targets ;
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

      @argv_targets = ("kernel.shm" , "dcom.mac" ) ;
      eval
      {
         $IMF_EXIT = imf ( @argv_targets ) ;
      } ; $IMF_EXIT = 1 if $@ ;

      if (( !defined $IMF_EXIT ) or ( $IMF_EXIT !=0 ))
      {
         $EXIT=0 ;
         print STDERR "ERROR: can't make kernel, dcom\n" ;
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


################################################################################

sub find_e0_files_in_prot_dirs
{
   my $ONLY_FAST= shift ;
   my $EXIT=0 ;
   my @e0_targets= () ;
   my $FAST_32_PROT="$ENV{OWN}/sys/wrk32/fast/prot/" ;
   my $FAST_PROT="$ENV{OWN}/sys/wrk/fast/prot/" ;
   my $QUICK_PROT="$ENV{OWN}/sys/wrk/quick/prot/" ;
   my $SLOW_PROT="$ENV{OWN}/sys/wrk/slow/prot/" ;

   @MAKE_TARGETS_WITH_PATH=() ;

   if ( opendir OPEN_PROT_DIR , $FAST_PROT )
   {
      $EXIT=1 ;
      @e0_targets= () ;

      if ( $CHECK ) { print "\$FAST_PROT            = $FAST_PROT\n" ;}

      if ( @e0_targets = grep /.*\.e0$/ , readdir OPEN_PROT_DIR )
      {
         if ( $CHECK ) { print "\@e0_targets             = @e0_targets\n" ; }

         while ( @e0_targets )
         {
            my $target = shift @e0_targets ;
            push @MAKE_TARGETS_WITH_PATH , "$FAST_PROT"."$target" ;
         }

         if ( $CHECK ) { print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ; }
      }
      else
      {
         print "no error protocols found\n" ;
         @e0_targets =  () ;
      }

      if ( $CHECK )
      {
         print "\$FAST_PROT  = $FAST_PROT\n" ;
         print "\@e0_targets = @e0_targets\n" ;
      }
   }
   else
   {
      $EXIT=0 ;
      print "ERROR: can't open directory $FAST_PROT\n" ;
   }


   if ( $ONLY_FAST eq "all" )
   {
      if (( ! $ONLY_64_BIT ) and ( ! $ONLY_32_BIT ))
      {
         if ( opendir OPEN_PROT_DIR , $FAST_32_PROT )
         {
            $EXIT=1 ;
            @e0_targets= () ;

            if ( $CHECK ) { print "\$FAST_32_PROT         = $FAST_PROT\n" ;}

            if ( @e0_targets = grep /.*\.e0$/ , readdir OPEN_PROT_DIR )
            {
               if ( $CHECK ) { print "\@e0_targets             = @e0_targets\n" ; }

               while ( @e0_targets )
               {
                  my $target = shift @e0_targets ;
                  push @MAKE_TARGETS_WITH_PATH , "$FAST_32_PROT"."$target" ;
               }

               if ( $CHECK ) { print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ; }
            }
            else
            {
               print "no error protocols found\n" ;
               @e0_targets =  () ;
            }

            if ( $CHECK )
            {
               print "\$FAST_PROT  = $FAST_32_PROT\n" ;
               print "\@e0_targets = @e0_targets\n" ;
            }
         }
         else
         {
            $EXIT=0 ;
            print "ERROR: can't open directory $FAST_32_PROT\n" ;
         }
      }


      if ( opendir OPEN_PROT_DIR , $QUICK_PROT )
      {
         @e0_targets= () ;

         if ( $CHECK ) { print "\$QUICK_PROT            = $QUICK_PROT\n" ;}

         if ( @e0_targets = grep /.*\.e0$/ , readdir OPEN_PROT_DIR )
         {
            if ( $CHECK ) { print "\@e0_targets             = @e0_targets\n" ; }

            while ( @e0_targets )
            {
               my $target = shift @e0_targets ;
               push @MAKE_TARGETS_WITH_PATH , "$QUICK_PROT"."$target" ;
            }

            if ( $CHECK ) { print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ; }
         }
         else
         {
            print "quick: no error protocols found\n" ;
            @e0_targets =  () ;
         }

         if ( $CHECK )
         {
            print "\$FAST_PROT  = $FAST_PROT\n" ;
            print "\@e0_targets = @e0_targets\n" ;
         }
      }
      else
      {
         $EXIT=0 ;
         print "ERROR: can't open directory $QUICK_PROT\n" ;
      }


      if ( opendir OPEN_PROT_DIR , $SLOW_PROT )
      {
         @e0_targets= () ;

         if ( $CHECK ) { print "\$SLOW_PROT            = $SLOW_PROT\n" ;}

         if ( @e0_targets = grep /.*\.e0$/ , readdir OPEN_PROT_DIR )
         {
            if ( $CHECK ) { print "\@e0_targets             = @e0_targets\n" ; }

            while ( @e0_targets )
            {
               my $target = shift @e0_targets ;
               push @MAKE_TARGETS_WITH_PATH , "$SLOW_PROT"."$target" ;
            }

            if ( $CHECK ) { print "\@MAKE_TARGETS_WITH_PATH = @MAKE_TARGETS_WITH_PATH\n" ; }
         }
         else
         {
            print "slow: no error protocols found\n" ;
            @e0_targets =  () ;
         }
      }
      else
      {
         $EXIT=0 ;
         print "ERROR: can't open directory $SLOW_PROT\n" ;
      }
   }

   return ( $EXIT ) ;
}

################## subroutines #################################################
