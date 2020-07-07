/*!
  \file    DBMSrvShMAdm_Manager.hpp
  \author  MarcW
  \brief   shared memory administration

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

*/

#if !defined(DBMSrvShMAdm_Manager_hpp)
#define DBMSrvShMAdm_Manager_hpp

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

class DBMSrvShMAdm_Manager {
public:
    DBMSrvShMAdm_Manager(int argc, char* argv[]);
    ~DBMSrvShMAdm_Manager();

    int run();

private:

    enum Action {
        ActionUnlock,
        ActionDelete,
        ActionCheck
    };

    bool checkArguments();
    bool unlockSharedMemory(DBMSrvMsg_Error& errOut);
    bool deleteSharedMemory(DBMSrvMsg_Error& errOut);
    bool checkSharedMemory(DBMSrvMsg_Error& errOut);

    int m_ArgCount;
    char** m_ArgVector;
    Action m_Action;
    Tools_DynamicUTF8String m_ShMFolder;
    Tools_DynamicUTF8String m_DBName;

    Tools_DynamicUTF8String m_ExecutableName;

    static const int returnValueOkay;
    static const int returnValueParameters;
    static const int returnValueNoSuccess;
};

#endif //DBMSrvShMAdm_Manager_hpp
