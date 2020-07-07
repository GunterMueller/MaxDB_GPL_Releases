/*!---------------------------------------------------------------------
  @file           RTESys_CommandPipe.h
  @author         JoergM
  @brief          System Calls for piping a command output, input or both
  @see            


  Currently only command output is needed and implemented...

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/


#ifndef RTESYS_COMMANDPIPE_H
#define RTESYS_COMMANDPIPE_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

/*!
  @brief structure controlling command output
 */
typedef struct RTESys_CommandOutputPipeContext_
{
    RTE_FileHandle cmdOutput; /*!< command output file handle */
#if defined(_WIN32)
    HANDLE         childProcess; ///< child process handle */
#else
    SAPDB_Int4     childPid; /*!< child process id */
    void          *cmdArea; /*!< needed for command separation */
#endif
} RTESys_CommandOutputPipeContext;

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

/*!

  @brief Create a process using given command line and pipe its output through pipe accessable for caller

  After this call returns successfully the member pContext->cmdOutput is a file handle of the command output
  pipe. This file handle can be used by the caller to read command output until EOF is detected. After that,
  the function RTESys_CloseCommandOutputPipe() is used to clean up resources and retrieve command exit status.
   @param   pContext [out] command pipe context to fill (see above)
   @param   command [in] command line with white space separated arguments
   @return System error code ( RTE_SYSTEMRC_NO_ERROR if no error occured )

 --------------------------------------------------------------------*/

externC RTE_SystemRc RTESys_OpenCommandOutputPipe( RTESys_CommandOutputPipeContext VAR_VALUE_REF pContext, 
                                                   SAPDB_Char const *command);

/*!
  @brief  Create a process using given command line and pipe its output through pipe accessable for caller

  After this call returns successfully the member pContext->cmdOutput is a file handle of the command output
  pipe. This file handle can be used by the caller to read command output until EOF is detected. After that,
  the function RTESys_CloseCommandOutputPipe() is used to clean up resources and retrieve command exit status.
   @param   pContext [out] command pipe context to fill (see above)
   @param   commandVector [in] command vector terminated by 0 string
   @return System error code ( RTE_SYSTEMRC_NO_ERROR if no error occured )

 --------------------------------------------------------------------*/

externC RTE_SystemRc RTESys_OpenCommandVectorOutputPipe( RTESys_CommandOutputPipeContext VAR_VALUE_REF pContext, 
                                                   SAPDB_Char **commandVector);
/*------------------------------------------------------*/

/*!--------------------------------------------------------------------
   @brief    Close a previous open command pipe and return exit status of programm
   @param    pContext [inout] command pipe context to close (see above)
   @param    pExitStatus [out] place to store exit status of command
   @return System error code ( RTE_SYSTEMRC_NO_ERROR if no error occured )

 --------------------------------------------------------------------*/

externC RTE_SystemRc RTESys_CloseCommandOutputPipe( RTESys_CommandOutputPipeContext VAR_VALUE_REF pContext, 
                                                    SAPDB_Int4                VAR_VALUE_REF pExitStatus);

#endif /* RTESYS_COMMANDPIPE_H */
