  /*!
  @file           Kernel_Exception.hpp
  @author         KernelGroup
  @ingroup        Kernel
  @brief          Exception interface of this component

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

*/
#ifndef KERNEL_EXCEPTION_HPP
#define KERNEL_EXCEPTION_HPP
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
/// declare Kernel_Exception and Admin_Exception and DevTest
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Kernel );
/// declare Admin_Exception
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Admin );
/// declare DevTest_Exception
SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( Test );
#endif // KERNEL_EXCEPTION_HPP
