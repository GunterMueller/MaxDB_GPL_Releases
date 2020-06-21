/*!
  @file           DBMCliEvtDisp_MsgDumper.hpp
  @author         MarcW
  @brief          dump a Msg_List to a FILE*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

*/

#ifndef DBMCliEvtDisp_MsgDumper_HPP
#define DBMCliEvtDisp_MsgDumper_HPP

#include <stdio.h>
#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"

/*!
    @brief dumping a Msg_List
 */
class DBMCliEvtDisp_MsgDumper {
public:
    /*
        @brief write all message texts of aDumpee into aOutFile
        
        @param aDumpee [IN] list to dump
        @param aOutFile [OUT] FILE* to write aDumpee to
    */
    static void dump(const Msg_List& aDumpee, FILE* aOutFile = stdout);

private:
    /*!
        @brief to avoid instatiation
    */
    DBMCliEvtDisp_MsgDumper() {}

    enum DBMEd_MsgID {
        SDBMSG_DBMED_ENUMERATION
    };
};

#endif //DBMCliEvtDisp_MsgDumper_HPP
