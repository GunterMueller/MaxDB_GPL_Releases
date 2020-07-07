/*!**************************************************************************

    module      : Kernel_Test.cpp
    responsible : UweH
    special area: KernelCommon
    description : entrypoint for KernelCommon tests
    last changed: 2001-01-17
    copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_Test.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Messages/Msg_List.hpp"

/* --------------------------------------------------------------------------- */
externCpp bool Kernel_Test (RTETask_ITask &task,
                            Msg_List      &errlist)
{
    Msg_List localErrList;
    bool     result = true;
    
    if ( ! Kernel_Version::Test(localErrList) )
    {
        errlist.AppendNewMessage(localErrList);
        localErrList.ClearMessageList();
        result = false;
    }

    if ( ! Kernel_Migration::Test(localErrList) )
    {
        errlist.AppendNewMessage(localErrList);
        localErrList.ClearMessageList();
        result = false;
    }

    return result;
}