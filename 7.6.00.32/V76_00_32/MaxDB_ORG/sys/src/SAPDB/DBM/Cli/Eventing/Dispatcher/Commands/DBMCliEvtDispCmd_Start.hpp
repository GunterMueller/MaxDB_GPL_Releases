/*!
  @file           DBMCliEvtDispCmd_Start.hpp
  @author         MarcW
  @brief          

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

#ifndef DBMCliEvtDispCmd_Start_HPP
#define DBMCliEvtDispCmd_Start_HPP

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Dispatcher.hpp"
#include "Messages/Msg_List.hpp"

class DBMCliEvtDispCmd_Start: public DBMCliEvtDispCmd_Base {
public:
    DBMCliEvtDispCmd_Start(
            bool internal,
            bool remoteAccess,
            const char* nodeName,
            const char* dbName,
            const char* userName,
            const char* cfgFile,
            const char* logFile,
            Msg_List& outList);

    ~DBMCliEvtDispCmd_Start() {}

    bool readyToRun() const {return m_ReadyToRun;}

    bool run();

private:
    bool m_ReadyToRun;
    Msg_List& m_OutList;
    DBMCliEvtDisp_Dispatcher m_Dispatcher;
};
#endif //DBMCliEvtDispCmd_Start_HPP
