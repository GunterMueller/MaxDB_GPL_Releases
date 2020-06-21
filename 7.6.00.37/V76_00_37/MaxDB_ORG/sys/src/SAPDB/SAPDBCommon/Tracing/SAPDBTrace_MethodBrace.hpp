/*!
  @file           SAPDBTrace_MethodBrace.hpp
  @author         DanielD
  @brief          classes to trace method entry and exit

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


#ifndef SAPDBTRACE_METHODBRACE_HPP
#define SAPDBTRACE_METHODBRACE_HPP

#include "SAPDBCommon/SAPDB_Types.h"

class SAPDBTrace_Topic;

/*!
  @class          SAPDBTrace_MethodBrace
   @brief          traces method entry and exit


         Objects of this class shouldn't be instantiated directly.
         Instead, the macros SAPDBTRACE_ROUTINE_DEBUG and
         SAPDBTRACE_METHOD_DEBUG should be used.
         Method enty and exit will always appear in pairs, changing
         the trace level after method entry will have no efect
         on the exit trace.

 */

class SAPDBTrace_MethodBrace
{
public:
    /*!
       @brief          traces method entry
       @param          nameP [in] the name to be written
       @param          traceTopic [in] the &lt;Class>SAPDBTrace_Topic&lt;/> to test against
       @param          traceLevel [in] the lvel to test against

     */

    SAPDBTrace_MethodBrace (
        const char * nameP,
        SAPDBTrace_Topic & traceTopic,
        int traceLevel);

    /*!
       @brief          traces method entry
       @param          nameP [in] the name to be written
       @param          thisPtr [in] the value of the this pointer in object methods
       @param          traceTopic [in] the &lt;Class>SAPDBTrace_Topic&lt;/> to test against
       @param          traceLevel [in] the lvel to test against

     */
    SAPDBTrace_MethodBrace (
        const char * nameP,
        const void * thisPtr,
        SAPDBTrace_Topic & traceTopic,
        int traceLevel);

    /*!
       @brief          traces method exit

     */

    ~SAPDBTrace_MethodBrace ();

protected:
    const SAPDB_Char * methodName;
    SAPDBTrace_Topic * traceTopic;
};



#endif

