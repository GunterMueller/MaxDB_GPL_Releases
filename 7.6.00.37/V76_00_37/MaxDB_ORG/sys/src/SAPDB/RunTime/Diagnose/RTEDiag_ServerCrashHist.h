/*!***************************************************************************

  module      : RTEDiag_ServerCrashHist.h

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Diagnostic files
  description : Saving of diagnostic files at restart if the database
                was shutting down abnormally the last time.
                Functions for the server site.

  last changed: 2001-02-08  10:56
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



#ifndef RTEDIAG_SERVERCRASHHIST_H
#define RTEDIAG_SERVERCRASHHIST_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define   CRASH_SEM_RESET   0   /* RESET _DIAG_SEM (XParam) */
#define   CRASH_SEM_POST    1   /* SET _DIAG_SEM (XParam)   */

          
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
  Chapter: RTE Diagnostic (Server)
 */
/*!
  Function:     RTEDiag_SetCrashSem
  description:  The function sets the state of the database Crash Flag. for this it 
                writes directly to the XParam file (_DIAG_SEM).
                At the start of the database the flag has to be initialized with 
                CRASH_SEM_POST.
                In case of an normal shutdown of the database the flag has to be
                reset by CRASH_SEM_RESET.
                
  arguments:
    state       [in] CRASH_SEM_RESET or CRASH_SEM_POST
    dbname      [in] Name of the database

  return value: none
 */
externC void	RTEDiag_SetCrashSem         (SAPDB_UInt                    state, 
                                             RTE_DBName                  dbname);

/*!
  Function:     RTEDiag_MakeCrashHist
  description:  If the Crash Flag (_DIAG_SEM) is set (CRASH_SEM_POST) the diagnostic
                files are saved under the specified path (DIAG_HISTORY_PATH).
                (see also http://pwww/Doku/rel-7.4/SAPDB/RunTime/Diagnose/CrashHistory.doc)
                
  arguments:
    p_xparam    [in] Pointer to XPARAM record 
    dbname      [in] Name of the database

  return value: none
 */
externC void	RTEDiag_MakeCrashHist       (RTE_XPARAM_REC               *p_xparam, 
                                             RTE_DBName                  dbname);

/*!
  EndChapter: RTE Diagnostic (Server)
 */
#endif  /* RTEDIAG_SERVERCRASHHIST_H */
