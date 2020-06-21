/*!
  @file           veo11.c
  @author         JoergM
  @special area   paramfile access
  @brief          general xparam access routines ...
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



\endif
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "hsp100.h"
#include "heo11.h"
#include "heo11_1.h"
#include "heo46.h"
#include "heo47.h"
#include "geo60.h"

#include "RunTime/Configuration/RTEConf_BuildParamFileName.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "RunTime/System/RTESys_IO.h"

#if defined (_WIN32)
# include <windows.h>
# include "gos44.h"                     /* nocheck but_imkdep */
#else                                   /*&else*/
# include "RunTime/RTE_saveUNIXcalls.h" /* nocheck but_imkdep */
#endif                                  /*&endif*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define MOD__ "veo11.c :"


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static tsp00_Bool eo11CheckFileExists ( tsp00_Pathc FileToCheck );

static tsp00_Bool eo11DeleteFile ( tsp00_Pathc FileToDelete,
                                   tsp01_RteError  *RteError) ;

static tsp00_Bool eo11SaveParamFileVersion ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                             tsp00_Int4                    VersionToSave ,
                                             tsp01_RteError               *RteError)  ;

static void eo11CreateTimeStamp ( tsp00_C64  VAR_ARRAY_REF  TimeStamp ) ;

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC tsp11_XpReturnCode 
        sqlXparamOpen   ( tsp00_DbNamec       DBNameC,
    	 		          tsp00_Int4         *pXparamId,
		  	 		      tsp00_Int4          OpenMode,
                          tsp01_RteError     *RteError )
{
  return(sqlXparamOpenVersion(DBNameC, XP_ACT_VERSION_SP11, pXparamId, OpenMode, RteError )) ;
}

/*==========================================================================*/

externC tsp11_XpReturnCode 
        sqlXparamOpenVersion   ( tsp00_DbNamec       DBNameC,
                                 tsp00_Int4          VersionToOpen,
		  	 		             tsp00_Int4         *pXparamId,
		  	 		             tsp00_Int4          OpenMode,
                                 tsp01_RteError     *RteError )
{
  tsp00_Bool         bOk;
  tsp11_XpReturnCode XpRet ;
  tsp00_Int4         DefaultMode = 0 ;
  tsp00_ErrText      acErrorText ; 
  tsp00_Pathc        ParamFile ;

  eo46_rte_error_init ( RteError ) ;

  if ( !(OpenMode & XP_OPEN_BIN_SP11) )
    DefaultMode |= XP_OPEN_BIN_SP11 ;

  if ( !(OpenMode & XP_OPEN_BUFFERED_SP11) &&
       !(OpenMode & XP_OPEN_DIRECT_SP11) )
    if (OpenMode & XP_OPEN_READONLY_SP11)
      DefaultMode |= XP_OPEN_DIRECT_SP11 ;
    else
      DefaultMode |= XP_OPEN_BUFFERED_SP11 ;

  bOk = RTEConf_BuildParamFileNameWithPath (DBNameC, VersionToOpen, ParamFile, RteError ) ;

  if ( bOk )
  {
    DBG3 (( MF__, "ParamFile = %s", ParamFile ));
    sql11c_OpenXParam ( ParamFile, pXparamId, acErrorText, (DefaultMode | OpenMode), &bOk );
	if ( !bOk )
	{
      RteError->RteErrCode = RTE_ERROR_SP01 ;
      eo46PtoC ( RteError->RteErrText, acErrorText , sizeof (tsp00_ErrText) );
	}
  }

  XpRet = bOk ? xp_ok_sp11 : xp_not_ok_sp11 ;

  return ( XpRet ) ;
}


/*==========================================================================*/

externC tsp11_XpReturnCode 
        sqlXparamClose  ( tsp00_Int4          XparamId,
		  			      tsp00_Bool          fFlush,
                          tsp01_RteError     *RteError )
{
  tsp00_Bool         bOk;
  tsp00_ErrText      acErrorText ; 
  tsp11_XpReturnCode XpRet ;

  eo46_rte_error_init ( RteError ) ;

  sql11c_CloseXParam ( XparamId, fFlush, acErrorText, &bOk );

  if ( bOk )
  {
    XpRet = xp_ok_sp11 ;
  }
  else
  {
    XpRet = xp_not_ok_sp11 ;
    RteError->RteErrCode = RTE_ERROR_SP01 ;
    eo46PtoC ( RteError->RteErrText, acErrorText , sizeof (tsp00_ErrText) );
  }

  return ( XpRet ) ;
}

/*==========================================================================*/

externC tsp00_Bool sqlXParamCheckExists ( tsp00_DbNamec    DBNameC) 
{
  tsp00_Pathc    Paramfile ;
  tsp01_RteError RteError ;
  tsp00_Bool     Ok ;

  Ok = sqlGetIndependentConfigPath ( Paramfile,
                                  TERM_WITH_DELIMITER_EO01, &RteError ) ;
  if ( Ok )
  {
    strcat (Paramfile, DBNameC ) ;
    Ok =  eo11CheckFileExists ( Paramfile ) ;
  }
  return ( Ok );
}

/*------------------------------*/

externC tsp00_Bool sqlXparamFsize ( tsp00_DbNamec    DBNameC,
                                    tsp00_Int4      *Fsize,
                                    tsp01_RteError  *RteError )
{
  #undef  MF__
  #define MF__ MOD__"sqlXparamFsize"

  tsp00_Pathc    Paramfile ;
  tsp00_Bool     Ok ;

  DBGPAS;

  Ok = sqlGetIndependentConfigPath ( Paramfile,
                                  TERM_WITH_DELIMITER_EO01, RteError ) ;
  if ( Ok )
    strcat (Paramfile, DBNameC ) ;

  if ( Ok )
  {
#if defined (_WIN32)
    tsp00_Int      Attl ;
    tsp00_Uint     FSizeLow, FSizeHigh ;
    tsp00_Int4 rc;
    rc = sql44c_get_file_info ( Paramfile, &Attl, &FSizeLow, &FSizeHigh ) ;
    Ok = rc == NO_ERROR ;
    if ( Ok )
      *Fsize = FSizeLow ;
    else
      eo46_set_rte_error( RteError, rc, "", Paramfile ) ;
#else
     struct stat buf;
     Ok = stat ( Paramfile, &buf )  == 0 ;
     if ( Ok ) 
       *Fsize = buf.st_size ;
     else
       eo46_set_rte_error( RteError, undef_ceo00, "", Paramfile ) ;
#endif
  }

  DBG3 (( MF__, "ParamFile:%s. Size = %d", Paramfile, *Fsize ));

  return ( Ok );
}

/*------------------------------*/

externC tsp00_Bool sqlXParamRemoveAll ( tsp00_DbNamec    DBNameC,
                                        tsp01_RteError  *RteError) 
{
  tsp00_Bool     Ok ;
   
  Ok = sqlXParamRemoveOneParamfile ( DBNameC, XP_ACT_VERSION_SP11, RteError ) &&
       sqlXParamRemoveAllVersions  ( DBNameC, RteError );

  return ( Ok );
}

/*------------------------------*/
externC tsp00_Bool 
        sqlXParamRemoveAllVersions ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                     tsp01_RteError               *RteError) 
{
  tsp00_Bool     Ok = true ;
  tsp00_Int4 VersionToDelete ;

  for ( VersionToDelete  = XP_FIRST_VERSION_SP11; 
        VersionToDelete <= XP_MAX_VERSION_SP11 && Ok ; VersionToDelete++ )
    Ok = sqlXParamRemoveOneParamfile ( DBNameC, VersionToDelete, RteError ) ;
  
  return ( Ok );
}

/*------------------------------*/

externC tsp00_Bool sqlXParamRemoveOneParamfile ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                                 tsp00_Int4                    VersionToRemove,
                                                 tsp01_RteError               *RteError) 
{
  tsp00_Bool     Ok ;
  tsp00_Pathc    Paramfile ;

  Ok = RTEConf_BuildParamFileNameWithPath (DBNameC, VersionToRemove, Paramfile, RteError );

  if ( Ok )
    if ( eo11CheckFileExists (Paramfile) )
      Ok = eo11DeleteFile ( Paramfile, RteError ) ;

  return ( Ok );
}

/*------------------------------*/

externC tsp00_Bool  eo11SaveActParamFileAsVersion ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                                    tsp01_RteError               *RteError) 
{
  #undef  MF__
  #define MF__ MOD__"eo11SaveActParamFileAsVersion"

  tsp00_Int4  VersionToHandle = XP_FIRST_VERSION_SP11;
  tsp00_Pathc NewParamFileVersion ;
  tsp00_Bool  VersionExists ;
  DBGIN;

  /* search for the first free version number to use */
  do 
  {
    DBG3 ((MF__, "checking for paramfile version'%s'", NewParamFileVersion));
    if ( RTEConf_BuildParamFileNameWithPath (DBNameC, VersionToHandle, NewParamFileVersion, RteError ) )
      VersionExists = eo11CheckFileExists( NewParamFileVersion ) ;
  }
  while ( RteError->RteErrCode == RTE_NO_ERROR_SP01  && VersionExists &&
          ++VersionToHandle <= XP_MAX_VERSION_SP11 ) ;

  /* if XP_MAX_VERSION_SP11 reached, remove latest version and rename all other */
  if ( VersionToHandle > XP_MAX_VERSION_SP11 )
  {
    VersionToHandle = XP_MAX_VERSION_SP11 ;
    eo11DeleteFile ( NewParamFileVersion, RteError ) ;
  }

  /* save previous versions */
  for ( --VersionToHandle; RteError->RteErrCode == RTE_NO_ERROR_SP01 && 
                           VersionToHandle      >= XP_FIRST_VERSION_SP11;
        --VersionToHandle )
  {
    eo11SaveParamFileVersion ( DBNameC, VersionToHandle, RteError ) ;
    DBG3 ((MF__, "saving paramfile version %d", VersionToHandle));
  }

  /* now we are ready to save the actual version */
  if ( RteError->RteErrCode == RTE_NO_ERROR_SP01 )
  {
    eo11SaveParamFileVersion ( DBNameC, XP_ACT_VERSION_SP11, RteError ) ;
    DBG3 ((MF__, "saving actual paramfile "));
  }

  return( RteError->RteErrCode == RTE_NO_ERROR_SP01 );

}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static tsp00_Bool eo11CheckFileExists ( tsp00_Pathc FileToCheck )
{
  tsp00_Bool     Ok ;
  struct stat buf;
  Ok = stat ( FileToCheck, &buf )  == 0 ;
  return(Ok);
}

/*------------------------------*/

static tsp00_Bool eo11DeleteFile ( tsp00_Pathc FileToDelete,
                                   tsp01_RteError  *RteError) 
{
  tsp00_Bool     Ok ;
#if defined (_WIN32)
    /* the backup files are read-only now, so DeleteFile() would fail */
       SetFileAttributes( FileToDelete,GetFileAttributes(FileToDelete) & ~FILE_ATTRIBUTE_READONLY);
       Ok = DeleteFile ( FileToDelete ) != 0 ; 
#else
       Ok = RTE_save_unlink ( FileToDelete )  == 0 ;
#endif

  if ( !Ok ) 
    eo46_set_rte_error( RteError, undef_ceo00, "", FileToDelete ) ;

  return(Ok);
}

/*------------------------------*/

static tsp00_Bool  eo11SaveParamFileVersion ( tsp00_DbNamec  VAR_ARRAY_REF  DBNameC,
                                              tsp00_Int4                    VersionToSave ,
                                              tsp01_RteError               *RteError) 
{
  #undef  MF__
  #define MF__ MOD__"eo11SaveParamFileVersion"

  tsp00_Pathc    ParamFileToSave, NewParamFileVersion;
  tsp00_Int4     NewVersion = VersionToSave + 1 ;
  tsp00_Bool     Ok ;

  DBGIN;

  Ok = RTEConf_BuildParamFileNameWithPath (DBNameC, VersionToSave, ParamFileToSave    , RteError ) &&
       RTEConf_BuildParamFileNameWithPath (DBNameC, NewVersion   , NewParamFileVersion, RteError ) ;

  if ( Ok )
  {
    if ( VersionToSave == XP_ACT_VERSION_SP11 )
    {
      tsp00_VfReturn_Param ReturnStatus;
      RTESys_IOCopyFileWithDestinationAtribute( ParamFileToSave,NewParamFileVersion,READONLY_ATTRIBUTE_RTESYS_IO,&ReturnStatus,NULL );
      Ok = (vf_ok == ReturnStatus);
      DBG3 ((MF__, "copy '%s' --> '%s', rte_rc = %d, os_rc = %d" ,
             ParamFileToSave, NewParamFileVersion, RteError->RteErrCode, RteError->OsErrCode));
    }
    else
    {
      DBG3 ((MF__, "rename '%s' --> '%s'" ,ParamFileToSave, NewParamFileVersion));
      Ok = rename ( ParamFileToSave, NewParamFileVersion ) == 0;
    }
  }

  DBGOUT;
  return(Ok) ;
}

/*------------------------------*/

/* PTS 1104471 */

static void eo11CreateTimeStamp ( tsp00_C64  VAR_ARRAY_REF  TimeStamp )
{
  #undef  MF__
  #define MF__ MOD__"eo11GetTimeStamp"

#ifdef WIN32
  {
    SYSTEMTIME   DateTime;
    GetLocalTime(&DateTime);
    wsprintf( TimeStamp,
              "%d-%02d-%02d %02d:%02d:%02d",
              DateTime.wYear,
              DateTime.wMonth,
              DateTime.wDay,
              DateTime.wHour,
              DateTime.wMinute,
              DateTime.wSecond ) ;
  }
#else
  {
#include <time.h>
#ifdef _REENTRANT
    struct tm                         tm_buffer ;
#endif
    struct tm                       * tmbuf ;
    time_t                            clk = time ( (time_t *) 0 );

#ifdef _REENTRANT
    tmbuf = localtime_r ( & clk, &tm_buffer );
#else
    tmbuf = localtime ( & clk );
#endif
    (void) sprintf ( (char *)TimeStamp , "%d-%02d-%02d %02d:%02d:%02d",
                     1900 + tmbuf->tm_year, tmbuf->tm_mon+1, tmbuf->tm_mday,
                     tmbuf->tm_hour, tmbuf->tm_min  , tmbuf->tm_sec );
  }
#endif
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
