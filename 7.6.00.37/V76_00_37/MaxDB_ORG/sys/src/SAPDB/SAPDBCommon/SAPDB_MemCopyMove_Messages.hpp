/*
        ========== licence begin  GPL
        Copyright (C) 2006 SAP AG

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
#ifndef SAPDB_MEMCOPYMOVE_MESSAGES_HPP
#define SAPDB_MEMCOPYMOVE_MESSAGES_HPP

// Generated Header file SAPDB_MemCopyMove_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define SAPDB_MEMFILL_PARAMETER_INVALID       SAPDBErr_MessageList::Error,20005,"Bad parameter: Fill in [%s] %s bytes",2
#define SAPDB_MEMFILL_PARAMETER_INVALID_ID 20005
#define SAPDB_MEMCPY_PARAMETER_INVALID        SAPDBErr_MessageList::Error,20006,"Bad parameter: Copy from [%s] -> [%s] %s bytes",3
#define SAPDB_MEMCPY_PARAMETER_INVALID_ID 20006
#define SAPDB_MEMMOVE_PARAMETER_INVALID       SAPDBErr_MessageList::Error,20007,"Bad parameter: Move from [%s] -> [%s] %s bytes",3
#define SAPDB_MEMMOVE_PARAMETER_INVALID_ID 20007
#define SAPDB_MEMCPY_OVERLAPPING              SAPDBErr_MessageList::Error,20008,"Memory region [%s,%s] and [%s,%s] overlap",4
#define SAPDB_MEMCPY_OVERLAPPING_ID 20008
#define SAPDB_MEMCPY_PASCAL_FILL              SAPDBErr_MessageList::Error,20009,"Bad parameter: limit %s, [%s]+%s, %s bytes %s",5
#define SAPDB_MEMCPY_PASCAL_FILL_ID 20009
#define SAPDB_MEMCPY_PASCAL_UNICODE_FILL      SAPDBErr_MessageList::Error,20010,"Bad parameter: limit %s, [%s]+%s, %s bytes %s|%s",6
#define SAPDB_MEMCPY_PASCAL_UNICODE_FILL_ID 20010
#define SAPDB_MEMCPY_PASCAL_MOVE              SAPDBErr_MessageList::Error,20011,"Bad parameter: source size [%s] dest size [%s], source addr [%s]+%s, dest addr [%s]+%s, %s bytes to copy",7
#define SAPDB_MEMCPY_PASCAL_MOVE_ID 20011
#define SAPDB_MEMCPY_PASCAL_ERROR_ALREADY_SET SAPDBErr_MessageList::Error,20012,"Internal error: Error code already set to [%s]",1
#define SAPDB_MEMCPY_PASCAL_ERROR_ALREADY_SET_ID 20012
#define SAPDB_MEMCPY_EXTERNAL_MOVE_ERROR      SAPDBErr_MessageList::Error,20013,"Module %s call index %s",2
#define SAPDB_MEMCPY_EXTERNAL_MOVE_ERROR_ID 20013

#endif /* SAPDB_MEMCOPYMOVE_MESSAGES_HPP */