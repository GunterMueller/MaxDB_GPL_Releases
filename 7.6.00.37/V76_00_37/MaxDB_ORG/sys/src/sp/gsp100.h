/*!
  @file           hsp100.h
  @author         RaymondR
  @special area   Version
  @brief          Version information type definitions


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG
sys/src/sp/gsp100.h
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/


#ifndef GSP100_H
#define GSP100_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*
 // --- Component Name  (!!!!!! max. 9 characters !!!!!!!!)
 */
#define COMP_NAME_KERNEL_SP100        "Kernel   "
#define COMP_NAME_QUICKKNL_SP100      "QuickKnl "
#define COMP_NAME_SLOWKNL_SP100       "SlowKnl  "
#define COMP_NAME_OMSTSTKNL_SP100     "OmsTstKnl" // PTS 1112358

#define COMP_NAME_QUERY_SP100         "Query    "
#define COMP_NAME_LOAD_SP100          "Load     "
#define COMP_NAME_DIAGNOSE_SP100      "Diagnose "

#define COMP_NAME_BACKUP_SP100        "Backup   "
#define COMP_NAME_RESTORE_SP100       "Restore  "
#define COMP_NAME_REPL_MAN_SP100      "Loader   "
#define COMP_NAME_DBMSERVER_SP100     "DBMServer"
#define COMP_NAME_C_PREC_SP100        "C-PreComp"
#define COMP_NAME_CPC_DRV_SP100       "CPC Drv  "
#define COMP_NAME_ODBC_DRV_SP100      "ODBC Drv "
#define COMP_NAME_OCI_DRV_SP100       "OCI Drv  "
#define COMP_NAME_SQLDBC_DRV_SP100    "libSQLDBC"
#define COMP_NAME_SQLDBC_HDR_SP100    "SQLDBC.H "

/*
  Definition of OS Specific 'Component_Name' of RunTime
 */
#if defined(_WIN32)

#  ifdef BIT64
#    define COMP_NAME_RTE_INTRO_SP100   "W64/"
#  else
#    define COMP_NAME_RTE_INTRO_SP100   "W32/"
#  endif

# define COMP_NAME_RTE_SP100           "RTE      "

/* Windows part                                        only 5 characters !!!! */

# define COMP_NAME_RTE_WIN9X_SP100  COMP_NAME_RTE_INTRO_SP100 "Win9x"

# if  defined(_X86_) || defined(_IA64_)
#   define COMP_NAME_RTE_NT_SP100   COMP_NAME_RTE_INTRO_SP100 "INTEL"
# endif

# if  defined(_AMD64_)
#   define COMP_NAME_RTE_NT_SP100   COMP_NAME_RTE_INTRO_SP100 "AMD64"
# endif

#else

#  ifdef BIT64
#    define COMP_NAME_RTE_INTRO_SP100   "X64/"
#  else
#    define COMP_NAME_RTE_INTRO_SP100   "X32/"
#  endif

/* UNIX part                                          only 5 characters !!!! */

#  if defined(FREEBSD)
#    define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "BSD44"
#  endif

#  if defined(LINUX)
#    if defined(IA64)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "LIA64"
#    elif defined(S390X)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "LS390"
#    elif defined(SDBonPPC64)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "LIPPC"
#    elif defined(X86_64)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "LIX86"
#    else
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "LINUX"
#    endif
#  endif

#  if defined(SVR4)  && defined(NMP)
#    define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "FSC  "
#  endif

#  if defined(AIX)   && defined(_IBMR2)
#    define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "AIX  "
#  endif

#  if defined(HPUX)  && (defined(HP9)||defined(PA20W)||defined(PA11)||defined(HP_IA64))
#    define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "HPUX "
#  endif

#  if (defined(SVR4)  && defined(SUN)) || defined(SOLARIS)
#    if defined(SUN) || defined(SPARC)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "SUN  "
#    elif defined(X86_64)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "SUX64"
#    elif defined(I386)
#      define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "SUX86"
#    endif
#  endif

# if defined(OSF1)   && defined(ALPHA)
#    define COMP_NAME_RTE_SP100   COMP_NAME_RTE_INTRO_SP100 "TRU64"
# endif

#endif
/*
  End of Definition of OS Specific 'Component_Name' of RunTime
 */


 /*
 *    'KERNEL    M.m.c    Build 000-000-000-001'
 */
#define BEG_OF_1ST_VERS_SUBSTR_SP100   10
#define END_OF_1ST_VERS_SUBSTR_SP100   17
#define BEG_OF_2CND_VERS_SUBSTR_SP100  25
#define END_OF_2CND_VERS_SUBSTR_SP100  27

#define PROD_NAME_SP100               "<PRODUCT_NAME>"
#define COMPANY_NAME_SP100            "<COMPANY_NAME>"
#define COPYRIGHT_RC_SP100            "Copyright \251 <COPYRIGHT_DATE> by <COMPANY_NAME>" /* Resource Copyright Strings */
#define COPYRIGHT_SP100               "<COPYRIGHT_STRING>"
#define DATE_SP100                    "Date <PATCH_DATE>"
#define SAPDB_PURPOSE_SP100           "<SAPDB_PURPOSE>"               /* set by setbuildinfo.pl */
#define SAPDB_BUILD_STATE_SP100       "<LC_STATE>"                    /* set by setbuildinfo.pl */
#define SAPDB_RELSTAT_SP100           "<RELSTAT>"                     /* set by setbuildinfo.pl */

#if defined ( VSP100_CPP ) || defined ( RESOURCE_FILE )

#  define MAJOR_VERSION_NO_SP100         <MAJOR_VERSION>               /* set by setbuildinfo.pl */
#  define MAJOR_VERSION_NO_STR_SP100    "<MAJOR_VERSION>"              /* set by setbuildinfo.pl */

#  define MINOR_VERSION_NO_SP100         <MINOR_VERSION>               /* set by setbuildinfo.pl */
#  define MINOR_VERSION_NO_STR_SP100    "<MINOR_VERSION>"              /* set by setbuildinfo.pl */

#  define CORRECTION_LEVEL_SP100         <CORRECTION_LEVEL>            /* set by setbuildinfo.pl */
#  define CORRECTION_LEVEL_STR_SP100    "<CORRECTION_LEVEL>"           /* set by setbuildinfo.pl */

#  define BUILD_PREFIX_SP100            <BUILD_PREFIX>                 /* set by setbuildinfo.pl */
#  define BUILD_PREFIX_STR_SP100        "<FORMATED3_BUILD_PREFIX>"     /* set by setbuildinfo.pl */

 /*
 // --- Resource Version Strings
 */
# if defined(_WIN32)
#   define VERSION_MM_STR_SP100         MAJOR_VERSION_NO_STR_SP100 "."  \
                                        MINOR_VERSION_NO_STR_SP100

#   define VERSION_MMC_STR_SP100        MAJOR_VERSION_NO_STR_SP100 "."  \
                                        MINOR_VERSION_NO_STR_SP100 "."  \
                                        CORRECTION_LEVEL_STR_SP100

#   define VERSION_MM_SP100             MAJOR_VERSION_NO_SP100,         \
                                        MINOR_VERSION_NO_SP100

#   define VERSION_MMC_SP100            MAJOR_VERSION_NO_SP100,         \
                                        MINOR_VERSION_NO_SP100,         \
                                        CORRECTION_LEVEL_SP100
# endif


 /*
// --- max. 40 characters
 */
#  define VERSION_STR_SP100             "%-9.9s %-8.8s Build %-3.3s-%-11.11s"

#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif
typedef char* ( *tsp100_BuildNumberFunc )   ( void );
typedef char* ( *tsp100_BuildNumberFuncEx ) ( unsigned char  *BuildNumberPrefix,
                                              unsigned long  *BuildNumber );
#if defined(__cplusplus)
}
#endif

typedef char   tsp100_CompName[9 + 1];             /* --- #define COMP_NAME_xxxxx */

typedef enum {
   IsEqual_esp100,
   IsOlder_esp100,
   IsNewer_esp100
} tsp100_VersionResult;


typedef enum {
   VersionIDType0_esp100,
   VersionIDType1_esp100,
   VersionIDType2_esp100
} tsp100_VersionIDType;


typedef struct tsp100_VersionID0 {
  unsigned char                     MajorVersion_sp100;
  unsigned char                     MinorVersion_sp100;
  unsigned char                     CorrLevel_sp100;
} tsp100_VersionID0;

typedef struct tsp100_VersionID1 {
  unsigned char                     MajorVersion_sp100;
  unsigned char                     MinorVersion_sp100;
  unsigned char                     CorrLevel_sp100;
  unsigned char                     BuildNumberPrefix_sp100;
} tsp100_VersionID1;

typedef struct tsp100_VersionID2 {
  unsigned char                     MajorVersion_sp100;
  unsigned char                     MinorVersion_sp100;
  unsigned char                     CorrLevel_sp100;
  unsigned char                     BuildNumberPrefix_sp100;
  unsigned long                     BuildNumber_sp100;
} tsp100_VersionID2;


#if defined ( VSP100_CPP )
 typedef union tsp100_VersionID {
   tsp100_VersionID0 ID0;
   tsp100_VersionID1 ID1;
   tsp100_VersionID2 ID2;
 } tsp100_VersionID;
#else
 typedef void tsp100_VersionID;
#endif

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GSP100_H */
