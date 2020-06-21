/*!
  @file           SAPDBDiag_CheckTopic.hpp
  @author         DanielD
  @brief          allows to switch groups of conditional statements

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
*/


#ifndef SAPDBDIAG_CHECKTOPIC_HPP
#define SAPDBDIAG_CHECKTOPIC_HPP

#include "SAPDBCommon/Diagnostics/SAPDBDiag_Topic.hpp"

/*!
  @class          SAPDBDiag_CheckTopic

 */

class SAPDBDiag_CheckTopic : public SAPDBDiag_Topic
{
public:
    /*!
       @brief          creates a new SAPDBDiag_CheckTopic.
       @param          topicKey [in] the string to be used by
                &lt;Method>Matches&lt;/>

     */

    SAPDBDiag_CheckTopic (const SAPDB_Char * topicKey,
                          const SAPDB_Char * topicDescription);

    /*!
       @returns        true if the local check level is smaller or equal
    &lt;Param>requestedLevel&lt;/>

     */

    bool ChecksLevel (int requestedLevel) const;

    /*!
       @brief          parses a line with switch instructions


           The line has the format "topic level [topic level ...]",
           a topic key followed by a numeric trace level.

     */

    static void MultiSwitch (
        SAPDBDiag_CheckTopic * const topics [],
        const SAPDB_Char * line,
        int                lineLen = -1);

protected:
    /* Function: GetKind

        returns: the 'class' of the topic
     */
    virtual const SAPDB_Char * GetTopicKind () const;

};



#endif

