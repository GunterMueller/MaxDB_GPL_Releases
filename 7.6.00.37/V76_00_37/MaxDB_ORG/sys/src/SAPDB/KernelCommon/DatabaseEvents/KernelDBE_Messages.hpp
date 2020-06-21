/*
        ========== licence begin  GPL
        Copyright (C) 2005 SAP AG

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
#ifndef KERNELDBE_MESSAGES_HPP
#define KERNELDBE_MESSAGES_HPP

// Generated Header file KernelDBE_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define KERNELDBE_DB_FULL_WARNING   SAPDBErr_MessageList::Warning,20000," %s percent of data area occupied, %s pages free",2
#define KERNELDBE_DB_FULL_WARNING_ID 20000
#define KERNELDBE_LOG_FULL_WARNING  SAPDBErr_MessageList::Warning,20001," %s percent of log area occupied, %s pages used",2
#define KERNELDBE_LOG_FULL_WARNING_ID 20001
#define KERNELDBE_DB_UNDERFLOW_INFO SAPDBErr_MessageList::Info,20002,"data area usage fell below %s percent, %s pages free",2
#define KERNELDBE_DB_UNDERFLOW_INFO_ID 20002
#define KERNELDBE_DB_FULL_INFO      SAPDBErr_MessageList::Info,20003," %s percent of data area occupied, %s pages free",2
#define KERNELDBE_DB_FULL_INFO_ID 20003
#define KERNELDBE_LOG_FULL_INFO     SAPDBErr_MessageList::Info,20004," %s percent of log area occupied, %s pages used",2
#define KERNELDBE_LOG_FULL_INFO_ID 20004

#endif /* KERNELDBE_MESSAGES_HPP */
