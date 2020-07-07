/*!
  @file           geo43.h
  @author         JoergM
  @special area   ceo43_CmdPipe/x_cons show API
  @brief          class interface to access X_CONS show output

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



#ifndef GEO43_H
#define GEO43_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo47.h"
#include <stdio.h>
#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          ceo43_CmdPipe

 */

class ceo43_CmdPipe
{
private:
    FILE     * m_CommandPipe_eo43;
    tsp00_Int4 m_LastExitCode_eo43;
    TCHAR    * m_OpenedCommand_eo43;
public:
    /*!
       @brief          Construct the object, but does NOT create the command pipe
       @return         none

     */

    ceo43_CmdPipe();
    /*!
       @brief          Cleans up command pipe if exists
       @return         none

     */

    ~ceo43_CmdPipe();
    /*!
       @brief          Create the output pipe and start the given command to fill it
       @param          command [in] complete command line to execute
       @param          openOk [out] Flag to indicate starting command succeeded
       @return         none

     */

    void eo43OpenCommand(const TCHAR *command, tsp00_Bool & openOk );
    /*!
       @brief          Close the command output pipe and free internal buffer if any
       @param          ExitCode [out] The exit code of the program executing the command
       @return         none

     */

    void eo43CloseCommand( tsp00_Int4 & ExitCode );
    /*!
       @brief          Get the next line from the output pipe
       @param          Buffer [out] The buffer to be filled
       @param          SizeInChars [in] Buffer size in number of characters
       @param          NoNewline [out] Flag to indicate that no newline at end of returned buffer content
       @param          ExitCode [out] Set to exit code of process, if 0 bytes are returned. Untouched otherwise
       @return         number of characters filled into buffer. 0 if end of pipe (ExitCode valid).


                    This call blocks until data is available in the pipe.
                    The given buffer is filled. If buffer is too small for
                    the next input line, a flag is set.
                    The buffer returned is always zero terminated.

     */

    tsp00_Int4 eo43NextLine( TCHAR *Buffer, 
                             const tsp00_Int4 SizeInChars, 
                             bool & NoNewline, 
                             tsp00_Int4 & ExitCode );
};


/*!
  @class          ceo43_xshow

 */

class ceo43_xshow: public ceo43_CmdPipe
{
private:
    TCHAR    * m_Command_eo43;
public:
    /*!
       @brief          x_cons show command output class derived from ceo43_CmdPipe.
       @param          showCommand [in] The specific show command
       @param          openOk [out] Flag to indicate starting command succeeded
       @return         none


                    The class simplifies the interface for ceo43_CmdPipe. Only the
                    database name and the specific show command are needed.
                    The usage is siplified too. Only the constructor is needed and
                    a loop over eo43NextLine() until 0 is returned:
                      ... sqlxinit(dbname, dbroot); // Mandatory for all sqlx... calls
                      ... ceo43_xshow xshow_cmd("all", openOk);
                      ... if ( !openOk ) command did not start
                      ... while ( xshow_cmd.eo43NextLine(Buffer, BufferSize, NoNewline, ExitCode) != 0 )
                      ........ do something with buffered output of show all....
                      ... if ( ExitCode != 0 ) command reported problem...

     */

    ceo43_xshow( const TCHAR *showCommand, tsp00_DbNamec DBName, tsp00_Bool & openOk );
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO43_H */
