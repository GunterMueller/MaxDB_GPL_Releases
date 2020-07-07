
/*!**************************************************************************

    module      : Kernel_Test.hpp

    -------------------------------------------------------------------------

    responsible : N.N.

    special area: KernelCommon
    description : entrypoint for KernelCommon tests

    last changed: 2001-01-19

    -------------------------------------------------------------------------

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

#ifndef Kernel_Test_HPP
#define Kernel_Test_HPP

class RTETask_ITask;
class Msg_List;

externCpp bool Kernel_Test (RTETask_ITask &task,
                            Msg_List      &errlist);

#endif // Kernel_Test_HPP
