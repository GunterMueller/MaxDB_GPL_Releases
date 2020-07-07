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
#ifndef RTE_SYSTEMPAGECACHE_MESSAGES_HPP
#define RTE_SYSTEMPAGECACHE_MESSAGES_HPP

// Generated Header file RTE_SystemPageCache_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RTEINFO_SYSTEMPAGECACHE_FREE_BLOCKS            SAPDBErr_MessageList::Info,13800,"Free system page cache blocks (%s different sizes, %s blocks)",2
#define RTEINFO_SYSTEMPAGECACHE_FREE_BLOCKS_ID 13800
#define RTEINFO_SYSTEMPAGECACHE_USED_BLOCKS            SAPDBErr_MessageList::Info,13801,"Used system page cache blocks (%s different sizes, %s blocks)",2
#define RTEINFO_SYSTEMPAGECACHE_USED_BLOCKS_ID 13801
#define RTEINFO_SYSTEMPAGECACHE_EMPTY_CHAIN            SAPDBErr_MessageList::Info,13802,"Size %s pages unused",1
#define RTEINFO_SYSTEMPAGECACHE_EMPTY_CHAIN_ID 13802
#define RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK            SAPDBErr_MessageList::Info,13803,"Size %s pages at %s unsplitted",2
#define RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK_ID 13803
#define RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK_SPLIT      SAPDBErr_MessageList::Info,13804,"Size %s pages at %s first splinter %s next splinter %s",4
#define RTEINFO_SYSTEMPAGECACHE_FIRST_BLOCK_SPLIT_ID 13804
#define RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK             SAPDBErr_MessageList::Info,13805,"                  at %s unsplitted",1
#define RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK_ID 13805
#define RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK_SPLIT       SAPDBErr_MessageList::Info,13806,"                  at %s first splinter %s next splinter %s",3
#define RTEINFO_SYSTEMPAGECACHE_NEXT_BLOCK_SPLIT_ID 13806
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_NO_BLOCKS      SAPDBErr_MessageList::Info,13808,"System page cache released no free blocks",0
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_NO_BLOCKS_ID 13808
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_BLOCKS         SAPDBErr_MessageList::Info,13809,"System page cache released %s pages in %s blocks",2
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_BLOCKS_ID 13809
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_HOLDING        SAPDBErr_MessageList::Info,13810,"System page cache still holding %s pages in %s splitted blocks",2
#define RTEINFO_SYSTEMPAGECACHE_RELEASE_HOLDING_ID 13810
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_BYTES        SAPDBErr_MessageList::Info,13811,"System page cache [bytes] used %s (max %s) controlled %s",3
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_BYTES_ID 13811
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_CALLS        SAPDBErr_MessageList::Info,13812,"System page cache [calls] alloc %s (system alloc %s) dealloc %s (system dealloc %s) errors %s",5
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_CALLS_ID 13812
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_BLOCKS       SAPDBErr_MessageList::Info,13813,"System page cache [blocks] used %s free %s (splinter %s) splitted %s splinter %s",5
#define RTEINFO_SYSTEMPAGECACHE_STATISTIC_BLOCKS_ID 13813

#endif /* RTE_SYSTEMPAGECACHE_MESSAGES_HPP */
