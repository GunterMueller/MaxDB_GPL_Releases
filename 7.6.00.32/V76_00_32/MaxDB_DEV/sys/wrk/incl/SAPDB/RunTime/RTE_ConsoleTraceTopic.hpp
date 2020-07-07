/*!
  @file           RTE_ConsoleTraceTopic.hpp
  @author         StefanP
  @special area   Tracing
  @brief          Tracing Support for XCons and Console Thread
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTE_CONSOLETRACETOPIC_HPP
#define RTE_CONSOLETRACETOPIC_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "heo07.h"

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
  @class          ConsoleTracePrinter

 */

class ConsoleTracePrinter : public SAPDBTrace_LinePrinter
{
public:

    bool    Initialize		(SAPDB_Char const * const           nameFile,
                             SAPDBErr_MessageList &				messageList);

    /* function: PrintLn
       description: writes a line to the output medium
     */
    virtual void PrintLn	(const SAPDB_Char*					buf, 
							 int								buflen);

private:
//    HANDLE      m_hMutex;
    teo07_Mutex     m_ConsoleTraceMutex;
    RTE_Path        m_TraceFilePath;
};




/*!
   @description    Initialize tracing
   @param          messageList [in/out] - Message list
   @return value   true/false

 */


SAPDB_Bool      InitTrace (SAPDB_Char const * const             nameFile,
                           SAPDBErr_MessageList &				messageList);


#endif  /* RTE_CONSOLETRACETOPIC_HPP */
