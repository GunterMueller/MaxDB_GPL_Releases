/*!
  @file           DBMCliEvtDispCmd_List.hpp
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

#ifndef DBMCliEvtDispCmd_List_HPP
#define DBMCliEvtDispCmd_List_HPP

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp"
#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_Properties.hpp"

class DBMCliEvtDispCmd_List: public DBMCliEvtDispCmd_Base {
public:
    DBMCliEvtDispCmd_List(
            Msg_List& outList,
            const char* cfgFile);

    virtual ~DBMCliEvtDispCmd_List();

    bool run();

protected:
    void clearProperties();

    Msg_List& m_OutList;
    Tools_DynamicUTF8String m_PropertiesFile;
    Tools_List<Tools_Property*>* m_AllProps;
};
#endif //DBMCliEvtDispCmd_List_HPP
