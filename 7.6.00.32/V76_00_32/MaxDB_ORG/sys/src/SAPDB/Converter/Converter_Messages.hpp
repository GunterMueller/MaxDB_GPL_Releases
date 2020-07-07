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
#ifndef CONVERTER_MESSAGES_HPP
#define CONVERTER_MESSAGES_HPP

// Generated Header file Converter_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT1      SAPDBErr_MessageList::Error,20000,"Converter page entry %s marked as used; invalid i/o blockaddress %s found",2
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT1_ID 20000
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT2      SAPDBErr_MessageList::Error,20001,"Converter page entry %s marked as unused; invalid entry refs (backward: %s, forward: %s) found",3
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT2_ID 20001
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT3      SAPDBErr_MessageList::Error,20002,"Converter page entry %s marked as unused, but also marked with 'savepages requested",1
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT3_ID 20002
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT4      SAPDBErr_MessageList::Error,20003,"Converter page entry %s marked as unused, but also marked with 'savepages done'",1
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT4_ID 20003
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT5      SAPDBErr_MessageList::Error,20004,"Converter page entry %s reservd, but not used",1
#define CONVERTER_IPAGE_ENTRY_IN_USE_CONFLICT5_ID 20004
#define CONVERTER_IPAGE_WRONG_COUNTER1              SAPDBErr_MessageList::Error,20005,"Sum of free (= %s) and used entries (= %s) is not equal to the number of all entries (=%s)",3
#define CONVERTER_IPAGE_WRONG_COUNTER1_ID 20005
#define CONVERTER_IPAGE_WRONG_COUNTER2              SAPDBErr_MessageList::Error,20006,"Number of reserved entries (= %s) as stored in the page header is different from real number (=%s)",2
#define CONVERTER_IPAGE_WRONG_COUNTER2_ID 20006
#define CONVERTER_DBFULL                            SAPDBErr_MessageList::Error,20007,"Database full. No more space on data volumes.",0
#define CONVERTER_DBFULL_ID 20007
#define CONVERTER_BAD_ENTRY_STATE                   SAPDBErr_MessageList::Error,20008,"Error occured by processing pageno %s (%s)",2
#define CONVERTER_BAD_ENTRY_STATE_ID 20008
#define CONVERTER_INVALIDVERSION                    SAPDBErr_MessageList::Error,20009,"Invalid converter version '%s' encountered. Actual version is '%s'",2
#define CONVERTER_INVALIDVERSION_ID 20009
#define CONVERTER_MAP_ERROR                         SAPDBErr_MessageList::Error,20010,"No valid converter entry for pageno %s (%s). Error code is '%s'",3
#define CONVERTER_MAP_ERROR_ID 20010
#define CONVERTER_PAGE_IN_FREE_LIST                 SAPDBErr_MessageList::Error,20011,"Data page %s already in free list",1
#define CONVERTER_PAGE_IN_FREE_LIST_ID 20011
#define CONVERTER_PAGE_IN_USE                       SAPDBErr_MessageList::Error,20012,"Requested specified pageno %s (static) is already in use",1
#define CONVERTER_PAGE_IN_USE_ID 20012
#define CONVERTER_EXPAND_FAILED                     SAPDBErr_MessageList::Error,20013,"Expanding the Converter from %s to %s pages failed",2
#define CONVERTER_EXPAND_FAILED_ID 20013
#define CONVERTER_NO_MORE_MEMORY                    SAPDBErr_MessageList::Error,20014,"No more memory %s",1
#define CONVERTER_NO_MORE_MEMORY_ID 20014
#define CONVERTER_UNEXPECTED_ERROR                  SAPDBErr_MessageList::Error,20015,"Unexpected Error state %s",1
#define CONVERTER_UNEXPECTED_ERROR_ID 20015
#define CONVERTER_NO_MEMORY_TO_BUILD_CONVERTER      SAPDBErr_MessageList::Error,20016,"No more memory to create converter",0
#define CONVERTER_NO_MEMORY_TO_BUILD_CONVERTER_ID 20016
#define CONVERTER_MAP_INIT_FAILED1                  SAPDBErr_MessageList::Error,20017,"Initilization of converter maps failed #all p.= %s max dyn p.=%s  max stat p.=%s",3
#define CONVERTER_MAP_INIT_FAILED1_ID 20017
#define CONVERTER_MAP_INIT_FAILED2                  SAPDBErr_MessageList::Error,20018,"Initilization of converter maps failed #all p.= %s",1
#define CONVERTER_MAP_INIT_FAILED2_ID 20018
#define CONVERTER_PNO_NOT_IN_BACKUP_PNO_LIST        SAPDBErr_MessageList::Error,20019,"Given pageno %s (%s) not in backup pageno list",2
#define CONVERTER_PNO_NOT_IN_BACKUP_PNO_LIST_ID 20019
#define CONVERTER_PAGE_IS_FULL                      SAPDBErr_MessageList::Error,20020,"No free entries available on page",0
#define CONVERTER_PAGE_IS_FULL_ID 20020
#define CONVERTER_MAXIMUN_SIZE_REACHED              SAPDBErr_MessageList::Error,20021,"Maxmimum data base size reached",0
#define CONVERTER_MAXIMUN_SIZE_REACHED_ID 20021
#define CONVERTER_FREE_CNT_MISMATCH                 SAPDBErr_MessageList::Error,20022,"Free cnt mismatch for ParentIndexEntry: Entry cnt %s vs. IndexPage cnt %s",2
#define CONVERTER_FREE_CNT_MISMATCH_ID 20022
#define CONVERTER_FULL_PAGE_IN_WRONG_CHAIN          SAPDBErr_MessageList::Error,20023,"Full IndexPage is member of partially used chain. Free count %s",1
#define CONVERTER_FULL_PAGE_IN_WRONG_CHAIN_ID 20023
#define CONVERTER_NOT_FULL_PAGE_IN_WRONG_CHAIN      SAPDBErr_MessageList::Error,20024,"Half filled IndexPage is member of full filled chain. Free count %s",1
#define CONVERTER_NOT_FULL_PAGE_IN_WRONG_CHAIN_ID 20024
#define CONVERTER_EMPTY_PAGE_IN_WRONG_CHAIN         SAPDBErr_MessageList::Error,20025,"Empty IndexPage is member of partially used chain. Free count %s",1
#define CONVERTER_EMPTY_PAGE_IN_WRONG_CHAIN_ID 20025
#define CONVERTER_UNUSED_CHAIN_CORRUPTED            SAPDBErr_MessageList::Error,20026,"Chain of not used converter map entries of section %s is corrupted concerning entry no %s",2
#define CONVERTER_UNUSED_CHAIN_CORRUPTED_ID 20026
#define CONVERTER_PARTIALLY_CHAIN_CORRUPTED         SAPDBErr_MessageList::Error,20027,"Chain of partially used converter map entries of section %s is corrupted concerning entry no %s",2
#define CONVERTER_PARTIALLY_CHAIN_CORRUPTED_ID 20027
#define CONVERTER_FULL_CHAIN_CORRUPTED              SAPDBErr_MessageList::Error,20028,"Chain of full used converter map entries of section %s is corrupted concerning entry no %s",2
#define CONVERTER_FULL_CHAIN_CORRUPTED_ID 20028
#define CONVERTER_UNUSED_CHAIN_LOOP                 SAPDBErr_MessageList::Error,20029,"Chain of unused converter map entries of section %s contains more than %s entries",2
#define CONVERTER_UNUSED_CHAIN_LOOP_ID 20029
#define CONVERTER_PARTIALLY_CHAIN_LOOP              SAPDBErr_MessageList::Error,20030,"Chain of partially used converter map entries of section %s contains more than %s entries",2
#define CONVERTER_PARTIALLY_CHAIN_LOOP_ID 20030
#define CONVERTER_FULL_CHAIN_LOOP                   SAPDBErr_MessageList::Error,20031,"Chain of full used converter map entries of section %s contains more than %s entries",2
#define CONVERTER_FULL_CHAIN_LOOP_ID 20031
#define CONVERTER_ERR_CHECKSUM_MISMATCH             SAPDBErr_MessageList::Error,20032,"Checksum mismatch; calculated: %s found: %s",2
#define CONVERTER_ERR_CHECKSUM_MISMATCH_ID 20032
#define CONVERTER_ERR_BAD_PAGE_TYPE                 SAPDBErr_MessageList::Error,20033,"Bad converter page type %s and type2 %s",2
#define CONVERTER_ERR_BAD_PAGE_TYPE_ID 20033
#define CONVERTER_ERR_WRITE_COUNT_MISMATCH          SAPDBErr_MessageList::Error,20034,"Write count mismatch; header: %s, trailer: %s",2
#define CONVERTER_ERR_WRITE_COUNT_MISMATCH_ID 20034
#define CONVERTER_ERR_BAD_PAGE_TYPE_2               SAPDBErr_MessageList::Error,20035,"Bad converter page - read page is no converter page",0
#define CONVERTER_ERR_BAD_PAGE_TYPE_2_ID 20035
#define CONVERTER_ERR_BAD_LEAF_PAGE_TYPE            SAPDBErr_MessageList::Error,20036,"Bad converter page - read page is no leaf page",0
#define CONVERTER_ERR_BAD_LEAF_PAGE_TYPE_ID 20036
#define CONVERTER_ERR_BAD_INDEX_PAGE_TYPE           SAPDBErr_MessageList::Error,20037,"Bad converter page - read page is no index page",0
#define CONVERTER_ERR_BAD_INDEX_PAGE_TYPE_ID 20037
#define CONVERTER_ERR_BIT_COUNT_MISSMATCH           SAPDBErr_MessageList::Error,20038,"Unauthorized modification of configuration parameter VOLUMENO_BIT_COUNT from %s to %s",2
#define CONVERTER_ERR_BIT_COUNT_MISSMATCH_ID 20038
#define CONVERTER_ERR_MIGRATION_CHECK               SAPDBErr_MessageList::Error,20039,"Could not execute migration, because migration check failed",0
#define CONVERTER_ERR_MIGRATION_CHECK_ID 20039
#define CONVERTER_ERR_SET_TO_OCCUPIED               SAPDBErr_MessageList::Error,20040,"Could not mark converter %s page as occupied in FBM. Given page is dumped into rundirectory",1
#define CONVERTER_ERR_SET_TO_OCCUPIED_ID 20040
#define CONVERTER_WRN_SUSPEND_TASK_DUE_TO_DB_FULL   SAPDBErr_MessageList::Warning,20041,"DB FULL: Task %s suspended (used pages: %s requested: %s data area size: %s)",4
#define CONVERTER_WRN_SUSPEND_TASK_DUE_TO_DB_FULL_ID 20041
#define CONVERTER_WRN_FORCE_ROLLBACK_DUE_TO_DB_FULL SAPDBErr_MessageList::Warning,20042,"Force Rollback due to DB-Full. Tasks %s holds too many temp pages #all p.= %s #all temp. p.= %s #task temp p.= %s",4
#define CONVERTER_WRN_FORCE_ROLLBACK_DUE_TO_DB_FULL_ID 20042
#define CONVERTER_WRN_NO_ADD_DATA_DEVICE            SAPDBErr_MessageList::Warning,20043,"Force Rollback due to DB-Full for Task %s. No data volume can be added",1
#define CONVERTER_WRN_NO_ADD_DATA_DEVICE_ID 20043
#define CONVERTER_WRN_NO_WAIT                       SAPDBErr_MessageList::Warning,20044,"Force Rollback due to DB-Full for Task %s. Task will not wait",1
#define CONVERTER_WRN_NO_WAIT_ID 20044
#define CONVERTER_WRN_NOT_RESTARTABLE               SAPDBErr_MessageList::Warning,20045,"Converter is in state 'maintenance'. Another restart is not possible",0
#define CONVERTER_WRN_NOT_RESTARTABLE_ID 20045
#define CONVERTER_WRN_NOT_RESTARTABLE2              SAPDBErr_MessageList::Warning,20046,"Converter is still active and could not be restarted in state 'maintenance'",0
#define CONVERTER_WRN_NOT_RESTARTABLE2_ID 20046
#define CONVERTER_WRN_EXPAND_UNSUCCESSFUL           SAPDBErr_MessageList::Warning,20047,"Expansion of converter failed, because of insufficient memory",0
#define CONVERTER_WRN_EXPAND_UNSUCCESSFUL_ID 20047
#define CONVERTER_WRN_UNHANDLED_RET_CODE            SAPDBErr_MessageList::Warning,20048,"Return code '%s' is not handled by the caller",1
#define CONVERTER_WRN_UNHANDLED_RET_CODE_ID 20048
#define CONVERTER_PAGENO_NOT_REFERENCED             SAPDBErr_MessageList::Info,20049,"The pageno %s (%s) on block %s/%s is deleted",4
#define CONVERTER_PAGENO_NOT_REFERENCED_ID 20049
#define CONVERTER_SAVEPOINT_NEEDED                  SAPDBErr_MessageList::Info,20050,"Savepoint needed (%s pages changed %s blocks occupied %s blocks available)",3
#define CONVERTER_SAVEPOINT_NEEDED_ID 20050
#define CONVERTER_FLUSHED_SECTION                   SAPDBErr_MessageList::Info,20051,"Flushed section #%s, IOPages: %s",2
#define CONVERTER_FLUSHED_SECTION_ID 20051
#define CONVERTER_INFO_SHUTDOWN                     SAPDBErr_MessageList::Info,20052,"Converter is in state shutdown",0
#define CONVERTER_INFO_SHUTDOWN_ID 20052
#define CONVERTER_INFO_CANCELLED_DURING_DB_FULL     SAPDBErr_MessageList::Info,20053,"Task %s was cancelled during database full situation",1
#define CONVERTER_INFO_CANCELLED_DURING_DB_FULL_ID 20053
#define CONVERTER_INFO_EXPAND_SUCCESSFUL            SAPDBErr_MessageList::Info,20054,"Expansion of converter successfully finished",0
#define CONVERTER_INFO_EXPAND_SUCCESSFUL_ID 20054

#endif /* CONVERTER_MESSAGES_HPP */