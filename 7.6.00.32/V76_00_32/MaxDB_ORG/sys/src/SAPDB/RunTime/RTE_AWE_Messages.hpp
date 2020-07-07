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
#ifndef RTE_AWE_MESSAGES_HPP
#define RTE_AWE_MESSAGES_HPP

// Generated Header file RTE_AWE_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RTEERR_SYS_AWE_OPT                             SAPDBErr_MessageList::Error,13000,"AWE: Could not open process token, rc = %s",1
#define RTEERR_SYS_AWE_OPT_ID 13000
#define RTEERR_SYS_AWE_LPV                             SAPDBErr_MessageList::Error,13001,"AWE: Could not lookup for privilage value '%s', rc = %s",2
#define RTEERR_SYS_AWE_LPV_ID 13001
#define RTEERR_SYS_AWE_ATP                             SAPDBErr_MessageList::Error,13002,"AWE: Could not adjust token privilage for '%s', rc = %s",2
#define RTEERR_SYS_AWE_ATP_ID 13002
#define RTEERR_SYS_AWE_NOT_INIT                        SAPDBErr_MessageList::Error,13003,"AWE: Invalid function call (not initialized)",0
#define RTEERR_SYS_AWE_NOT_INIT_ID 13003
#define RTEERR_SYS_AWE_ALREADY_INIT                    SAPDBErr_MessageList::Error,13004,"AWE: Object already initialized",0
#define RTEERR_SYS_AWE_ALREADY_INIT_ID 13004
#define RTEERR_SYS_AWE_OUT_OF_HEAP_MEM                 SAPDBErr_MessageList::Error,13005,"AWE: Out of heap memory, rc = %s",1
#define RTEERR_SYS_AWE_OUT_OF_HEAP_MEM_ID 13005
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA                  SAPDBErr_MessageList::Error,13006,"AWE: Could not allocate mapping area, rc = %s",1
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA_ID 13006
#define RTEERR_SYS_AWE_FREE_MAP_AREA                   SAPDBErr_MessageList::Error,13007,"AWE: Could not free mapping area, rc = %s",1
#define RTEERR_SYS_AWE_FREE_MAP_AREA_ID 13007
#define RTEERR_SYS_AWE_ALLOC_FRAME_INFO                SAPDBErr_MessageList::Error,13008,"AWE: Could not allocate frame or info area, rc = %s",1
#define RTEERR_SYS_AWE_ALLOC_FRAME_INFO_ID 13008
#define RTEERR_SYS_AWE_ALLOC_PHYS_PAGES                SAPDBErr_MessageList::Error,13009,"AWE: Could not allocate %s physical pages, rc = %s",2
#define RTEERR_SYS_AWE_ALLOC_PHYS_PAGES_ID 13009
#define RTEERR_SYS_AWE_ALL_PHYS_RESERVED               SAPDBErr_MessageList::Error,13010,"AWE: All physical pages reserved (Num: %s)",1
#define RTEERR_SYS_AWE_ALL_PHYS_RESERVED_ID 13010
#define RTEERR_SYS_AWE_MAP_PHYS_PAGE                   SAPDBErr_MessageList::Error,13011,"AWE: Could not map physical page %s, rc = %s",2
#define RTEERR_SYS_AWE_MAP_PHYS_PAGE_ID 13011
#define RTEERR_SYS_AWE_UNMAP_PHYS_PAGE                 SAPDBErr_MessageList::Error,13012,"AWE: Could not unmap physical page, rc = %s",1
#define RTEERR_SYS_AWE_UNMAP_PHYS_PAGE_ID 13012
#define RTEERR_SYS_AWE_OUT_OF_RES_MAP_AREA             SAPDBErr_MessageList::Error,13013,"AWE: Out of reserved map area (cache) pages (max: %s)",1
#define RTEERR_SYS_AWE_OUT_OF_RES_MAP_AREA_ID 13013
#define RTEERR_SYS_AWE_MISS_USER_RIGHT                 SAPDBErr_MessageList::Error,13014,"AWE: Missing user right '%s'",1
#define RTEERR_SYS_AWE_MISS_USER_RIGHT_ID 13014
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA_INFO             SAPDBErr_MessageList::Error,13015,"AWE: Could not allocate map area (cache) page infos, rc = %s",1
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA_INFO_ID 13015
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA_HASH             SAPDBErr_MessageList::Error,13016,"AWE: Could not allocate hash slots for map area (cache) page infos, rc = %s",1
#define RTEERR_SYS_AWE_ALLOC_MAP_AREA_HASH_ID 13016
#define RTEERR_SYS_AWE_NO_MORE_HL_ITEMS                SAPDBErr_MessageList::Error,13017,"AWE: No more hash list items",0
#define RTEERR_SYS_AWE_NO_MORE_HL_ITEMS_ID 13017
#define RTEERR_SYS_AWE_MAP_AREA_OUT_OF_MEM             SAPDBErr_MessageList::Error,13018,"AWE: Could not allocate mapping area (out of memory)",0
#define RTEERR_SYS_AWE_MAP_AREA_OUT_OF_MEM_ID 13018
#define RTEERR_SYS_AWE_PHYS_PAGE_ALR_MAPPED            SAPDBErr_MessageList::Error,13019,"AWE: Physical page %s already mapped on address: %s ( map address: %s )",3
#define RTEERR_SYS_AWE_PHYS_PAGE_ALR_MAPPED_ID 13019
#define RTEINFO_SYS_AWE_WRONG_MAP_AREA_SIZE            SAPDBErr_MessageList::Info,13030,"No AWE used - Wrong map area (cache) size, available: %s MB, requested: %s MB)",2
#define RTEINFO_SYS_AWE_WRONG_MAP_AREA_SIZE_ID 13030
#define RTEINFO_SYS_AWE_OUT_OF_PHYS_MEM                SAPDBErr_MessageList::Info,13031,"No AWE used - Physical memory, Total: %s MB, Used (other Apps. and Sys.): %s MB, Reserved: %s MB",3
#define RTEINFO_SYS_AWE_OUT_OF_PHYS_MEM_ID 13031
#define RTEINFO_SYS_AWE_MAP_AREA_TO_BIG                SAPDBErr_MessageList::Info,13032,"No AWE used - Number of phyical pages smaller than number of map area (cache) pages",0
#define RTEINFO_SYS_AWE_MAP_AREA_TO_BIG_ID 13032
#define RTEINFO_SYS_AWE_AVAIL_INFO                     SAPDBErr_MessageList::Info,13033,"AWE available - Used memory: %s MB (Map area (cache): %s MB included)",2
#define RTEINFO_SYS_AWE_AVAIL_INFO_ID 13033

#endif /* RTE_AWE_MESSAGES_HPP */
