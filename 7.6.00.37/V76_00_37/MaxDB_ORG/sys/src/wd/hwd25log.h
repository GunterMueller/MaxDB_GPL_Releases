/*!**********************************************************************

  module: hwd25log.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Log routines

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




************************************************************************/

#ifndef HWD25LOG_H
#define HWD25LOG_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd25LogP

  description: The twd25LogP type is used to log messages to a
               specific file.
  
************************************************************************/

typedef struct st_wa_log *twd25LogP;

/*!**********************************************************************
  Function: wd25GetLogFilename

  description: Determines the log filename

  arguments:
    filename  [in]  Pointer to log-object.
      
  return value: A pointer to the log filename.

************************************************************************/

const char* wd25GetLogFilename( twd25LogP log );

void wd25SetLogInfoFlag( twd25LogP log, sapdbwa_Bool withInfo );

void wd25WriteLogInfoMsg( twd25LogP log, char *msg, sapdbwa_Bool withHeader );


/*!**********************************************************************
  Function: wd25CreateLog

  description: Create a new Log-Type.

  arguments:
    filename  [in]  The name of the log-file.
      
  return value: A pointer to the created log object or NULL if the file
                could not be opened or memory allocation failed. */

twd25LogP wd25CreateLog( char *filename );


/*!**********************************************************************
  Function: wd25DestroyLog

  description: Free memory and file-handles of the log-object.

  arguments:
    log  [in]  Pointer to log-object that should be freed.

  return value: void */

void wd25DestroyLog( twd25LogP log );


/*!**********************************************************************
  Function: wd25EmptyLog

  description: Delete contents of log-file.

  arguments:
    log         [in]  Pointer to log-object.
    makeCopy    [in]  If true log file will be copied before.

  return value: void */

void wd25EmptyLog( twd25LogP log, sapdbwa_Bool makeCopy );

    
/*!**********************************************************************
  Function: wd25GetFileName

  description: Get the log filename

  arguments:
    log  [in]  Pointer to log-object.

  return value: pointer to log filename */

const char *wd25GetFileName( twd25LogP log );

/*!**********************************************************************
  Function: wd25WriteLogMsg

  description: Write log message plus end of line.

  arguments:
    log  [in]  Pointer to log-object that should be freed.
    msg  [in]  The message that should be written.
    
  return value: void */

void wd25WriteLnLogMsg( twd25LogP log, char *msg );

/*!**********************************************************************
  Function: wd25WriteLogMsg

  description: Write log message.

  arguments:
    log  [in]  Pointer to log-object that should be freed.
    msg  [in]  The message that should be written.
    
  return value: void */

void wd25WriteLogMsg( twd25LogP log, char *msg );

/*!**********************************************************************
  Function: wd25IsLogFileEmpty

  description: Checks for log file empty.

  arguments:
    log  [in]  Pointer to log-object that should be freed.
    
  return value: true if log file is empty otherwise false */

sapdbwa_Bool wd25IsLogFileEmpty( twd25LogP log );

/*!**********************************************************************

  EndChapter: twd25LogP
  
************************************************************************/

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
