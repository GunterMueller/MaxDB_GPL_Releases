/*!***************************************************************************

  module      : RTEDiag_CommonCrashHist.h

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Diagnostic files
  description : Saving of diagnostic files at restart if the database
                was shutting down abnormally the last time
                Functions for the client and server site.

  last changed: 2001-02-08  10:58
  see also    : http://pwww/Doku/rel-7.4/SAPDB/RunTime/Diagnose/CrashHistory.doc

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



#ifndef RTEDIAG_COMMONCRASHHIST_H
#define RTEDIAG_COMMONCRASHHIST_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/* Defines for the format of the history name (without leading database name)   */
/* The Format of HIST_NAME_SKEL can be changed arbitrarily. But it must contain */
/* the elements yyyy, mm, dd, HH, MM, SS in exact the given length, e.g         */
/* 'ddmmyyyyHHMMSS' is allowed but '_yymmdd_HH-MM-SS' is not !!                 */
/* This yields because of the given format of date and time in vdattime ()      */
/* (see heo52.h).                                                               */

#define     HIST_NAME_SKEL      "_yyyymmdd_HH-MM-SS" 

/* !!!Do not change: */
#define     HIST_YEAR           "y"
#define     HIST_MONTH          "m"
#define     HIST_DAY            "d"
#define     HIST_HOUR           "H"
#define     HIST_MINUTE         "M"
#define     HIST_SECOND         "S"
#define     HIST_LEN_YEAR       4
#define     HIST_LEN_MONTH      2
#define     HIST_LEN_DAY        2
#define     HIST_LEN_HOUR       2
#define     HIST_LEN_MINUTE     2
#define     HIST_LEN_SECOND     2



#if defined(_WIN32)
/*--- NT    */
#define     SEPERATOR                   '\\'
#else
/*--- UNIX  */
#define     SEPERATOR                   '/'
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*!
  Chapter: RTE Diagnostic (Common (Client + Sever))
 */

/*!
  Function:     RTEDiag_OsError
  description:  This function write messages to KNLDIAG for  
                errors notified by the OS.
                
  arguments:
    routine         [in]    Name of function which called this functions 
    callFunction    [in]    Name of the OS function which notified the error
    zusText         [in]    Additional error text
    rc              [in]    NT:     System return code if already known or UNDEF_ERROR
                            UNIX:   Not evaluated
    type            [in]    Type of error: INFO_TYPE or ERR_TYPE

  return value: none
*/
externC  void                RTEDiag_OsError                (SAPDB_Char        *routine,
                                                             SAPDB_Char        *callFunction,
                                                             SAPDB_Char        *zusText,
                                                             RTE_SystemRc       rc,
                                                             SAPDB_UInt         type);

/*!
  Function:     RTEDiag_DeleteOldHistories 
  description:  This function deletes all histories of the database older than the 
                'numRemainHist' newest ones. 
                .
  arguments:
    numRemainHist       [in]    Number of remaining histories (see description)
    dbname              [in]    Name of the database
    crashHistoryPath    [in]    Path of the crash history (XParam: DIAG_HISTORY_PATH)


  return value: none 
  
*/
externC  void                RTEDiag_DeleteOldHistories     (SAPDB_UInt                     numRemainHist,
                                                             RTE_DBName                     dbname,
                                                             RTE_Path                       crashHistoryPath); 


/*!
  EndChapter: RTE Diagnostic (Common)
 */
#endif  /* RTEDIAG_COMMONCRASHHIST_H */
