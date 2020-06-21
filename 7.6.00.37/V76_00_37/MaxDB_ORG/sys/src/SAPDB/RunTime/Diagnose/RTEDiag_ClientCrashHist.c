/*!***************************************************************************

  module      : RTEDiag_ClientCrashHist.c

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Diagnostic files
  description : Saving of diagnostic files at restart if the database
                was shutting down abnormally the last time.
                Functions for the client site.


  last changed: 2001-02-07  14:20
  see also    : 

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include  "gsp00.h"
#include  "geo007_1.h"
#include  "RunTime/RTE_Types.h"
#include  "RunTime/Diagnose/RTEDiag_ClientCrashHist.h"
#include  "RunTime/Diagnose/RTEDiag_CommonCrashHist.h"




/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERN FUNCTIONS                                                         *
 *===========================================================================*/
/* For the documentation see header file */
extern	SAPDB_Bool	RTEDiag_DropAllCrashHist       (RTE_DBName    dbname, 
                                                    RTE_Path      crashHistoryPath)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char          routine[] = "RTEDiag_DropAllCrashHist";

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    RTEDiag_DeleteOldHistories (0, dbname, crashHistoryPath);

#if defined(_WIN32) /*--- NT ---*/
    if(!RemoveDirectory (crashHistoryPath))
    {
        RTEDiag_OsError (routine, "RemoveDirectory", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        return (false);
    }
#else               /*--- UNIX ---*/
    if (rmdir(crashHistoryPath) != 0)
    {
        RTEDiag_OsError (routine, "rmdir", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        return (false);
    }
#endif

    return (true);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
