/*!
  @file           SAPDBTrace_Topic.hpp
  @author         DanielD
  @brief          allows to switch groups of trace statements

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


#ifndef SAPDBTRACE_TOPIC_HPP
#define SAPDBTRACE_TOPIC_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_Topic.hpp"

class SAPDBTrace_LinePrinter;

/*!
  @class          SAPDBTrace_Topic

 */

class SAPDBTrace_Topic : public SAPDBDiag_Topic
{
public:
    /*!
       @brief          creates a new SAPDBTrace_Topic.
       @param          switchKey [in] the string to be used by
                &lt;Method>SwitchConditional&lt;/>
                       switchDescription [in] the description of the switch

     */

    SAPDBTrace_Topic (const SAPDB_Char * switchKey,
                      const SAPDB_Char * switchDescription);

    /*!
       @returns        true if the local trace level is smaller or equal
    &lt;Param>requestedLevel&lt;/>

     */

    bool TracesLevel (int requestedLevel) const;

    /*!
       @brief          changes the global trace printer for this topic
       @param          printer [in] the new trace printer

     */

    void SetTracePrinter (SAPDBTrace_LinePrinter * printer);

    /*!
       @returns        the shared trace printer

     */

    SAPDBTrace_LinePrinter & GetTracePrinter () const;

    /*!
       @brief          sets the trace printer for a group of topics.

     */

    static void InitGroup (
        SAPDBTrace_Topic * const topics [],
        SAPDBTrace_LinePrinter * printer);

    /*!
       @brief          parses a line with trace instructions


           The line has the format "topic level [topic level ...]",
           a topic key followed by a numeric trace level.

     */

    static void MultiSwitch (
        SAPDBTrace_Topic * const topics [],
        const SAPDB_Char * line,
        int                lineLen = -1);

protected:
    /* Function: GetKind

        returns: the 'class' of the topic
     */
    virtual const SAPDB_Char * GetTopicKind () const;

private:
    SAPDBTrace_LinePrinter * linePrinter;
};



#endif

