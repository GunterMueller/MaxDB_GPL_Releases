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
#ifndef FILEDIR_MESSAGES_HPP
#define FILEDIR_MESSAGES_HPP

// Generated Header file FileDir_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define FILEDIR_RESTART                         SAPDBErr_MessageList::Info,20000,"File directory restarting.",0
#define FILEDIR_RESTART_ID 20000
#define FILEDIR_RESTART_FINISHED                SAPDBErr_MessageList::Info,20001,"File directory restart finished.",0
#define FILEDIR_RESTART_FINISHED_ID 20001
#define FILEDIR_CONVERTER_VERSION_OUT_OF_SYNC   SAPDBErr_MessageList::Warning,20002,"Unexpected converter version encountered. Internal is %s, encountered was %s.",2
#define FILEDIR_CONVERTER_VERSION_OUT_OF_SYNC_ID 20002
#define FILEDIR_CONVERTER_VERSION_TOO_SMALL     SAPDBErr_MessageList::Warning,20003,"Converter version is too small (is %s). This could result in skewed statistics.",1
#define FILEDIR_CONVERTER_VERSION_TOO_SMALL_ID 20003
#define FILEDIR_NOT_ACTIVATED                   SAPDBErr_MessageList::Error,20004,"A method of the new FileDirectory has been called although the new FileDirectory has not been activated during compilation.",0
#define FILEDIR_NOT_ACTIVATED_ID 20004
#define FILEDIR_OUT_OF_MEMORY                   SAPDBErr_MessageList::Error,20005,"Out of memory: failed to allocate %s bytes.",1
#define FILEDIR_OUT_OF_MEMORY_ID 20005
#define FILEDIR_LOAD_PAGE_FAILED                SAPDBErr_MessageList::Error,20006,"Could not read file directory page %s, error code %s.",2
#define FILEDIR_LOAD_PAGE_FAILED_ID 20006
#define FILEDIR_TEST_FAILED                     SAPDBErr_MessageList::Error,20007,"FileDirectory test failed with %s errors.",1
#define FILEDIR_TEST_FAILED_ID 20007
#define FILEDIR_RECREATE_INDEX_FAILED           SAPDBErr_MessageList::Error,20008,"FileDirectory could not find table %s for index %s.",2
#define FILEDIR_RECREATE_INDEX_FAILED_ID 20008
#define FILEDIR_ADD_INDEX_FAILED                SAPDBErr_MessageList::Error,20009,"FileDirectory could not add index %s to table %s.",2
#define FILEDIR_ADD_INDEX_FAILED_ID 20009
#define FILEDIR_RECREATE_SHORTCOLUMN_FAILED     SAPDBErr_MessageList::Error,20010,"FileDirectory could not find table %s for short column file %s.",2
#define FILEDIR_RECREATE_SHORTCOLUMN_FAILED_ID 20010
#define FILEDIR_RECREATE_KEYPARTITION_FAILED    SAPDBErr_MessageList::Error,20011,"FileDirectory could not find oms key %s for omy key partition file %s.",2
#define FILEDIR_RECREATE_KEYPARTITION_FAILED_ID 20011
#define FILEDIR_ADD_KEYPARTITION_FAILED         SAPDBErr_MessageList::Error,20012,"FileDirectory could not add omy key partition %s to oms key %s.",2
#define FILEDIR_ADD_KEYPARTITION_FAILED_ID 20012
#define FILEDIR_REFCOUNT_NOT_NULL               SAPDBErr_MessageList::Error,20013,"Reference counter is %s in destructor for file-id %s.",2
#define FILEDIR_REFCOUNT_NOT_NULL_ID 20013
#define FILEDIR_REFCOUNT_NOT_NULL_REMOVE        SAPDBErr_MessageList::Error,20014,"Reference counter is %s during remove for file-id %s.",2
#define FILEDIR_REFCOUNT_NOT_NULL_REMOVE_ID 20014
#define FILEDIR_FILEID_SPACE_EXHAUSTED          SAPDBErr_MessageList::Error,20015,"FileId space exhausted.",0
#define FILEDIR_FILEID_SPACE_EXHAUSTED_ID 20015
#define FILEDIR_SPINLOCKPOOL_INIT_FAILED        SAPDBErr_MessageList::Error,20016,"Could not initialize spinlock pool.",0
#define FILEDIR_SPINLOCKPOOL_INIT_FAILED_ID 20016
#define FILEDIR_FILEDIR_INIT_FAILED             SAPDBErr_MessageList::Error,20017,"Could not initialize persistent file directory.",0
#define FILEDIR_FILEDIR_INIT_FAILED_ID 20017
#define FILEDIR_RECREATE_ENTRY_FAILED           SAPDBErr_MessageList::Error,20018,"Could not recreate file directory entry with file id %s.",1
#define FILEDIR_RECREATE_ENTRY_FAILED_ID 20018
#define FILEDIR_RECREATE_ADD_ENTRY_FAILED       SAPDBErr_MessageList::Error,20019,"Could not add file directory entry with file id %s during file directory restart. Error code %s.",2
#define FILEDIR_RECREATE_ADD_ENTRY_FAILED_ID 20019
#define FILEDIR_HASH_RESIZE_ADD_ERROR           SAPDBErr_MessageList::Error,20020,"Error %s during file directory hash resize, file id %s.",2
#define FILEDIR_HASH_RESIZE_ADD_ERROR_ID 20020
#define FILEDIR_SET_FILEID_ONLY_DURING_RESTART  SAPDBErr_MessageList::Error,20021,"Setting highest file id only allowed during redo.",0
#define FILEDIR_SET_FILEID_ONLY_DURING_RESTART_ID 20021
#define FILEDIR_FILEID_REQUESTED_DURING_RESTART SAPDBErr_MessageList::Error,20022,"Cannot request new file id during redo.",0
#define FILEDIR_FILEID_REQUESTED_DURING_RESTART_ID 20022
#define FILEDIR_WRONG_PAGE_NUMBER               SAPDBErr_MessageList::Error,20023,"Requested page number %s not in file directory.",1
#define FILEDIR_WRONG_PAGE_NUMBER_ID 20023
#define FILEDIR_INTERNAL_ERROR                  SAPDBErr_MessageList::Error,20024,"An unexpected state was detected within the file directory.",0
#define FILEDIR_INTERNAL_ERROR_ID 20024
#define FILEDIR_INIT_PAGE_FAILED                SAPDBErr_MessageList::Error,20025,"Failed to initialize file directory page #%s.",1
#define FILEDIR_INIT_PAGE_FAILED_ID 20025
#define FILEDIR_ADD_ENTRY_FAILED                SAPDBErr_MessageList::Error,20026,"Internal error when adding file directory entry with id %s.",1
#define FILEDIR_ADD_ENTRY_FAILED_ID 20026

#endif /* FILEDIR_MESSAGES_HPP */
