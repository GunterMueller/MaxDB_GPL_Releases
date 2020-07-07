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
#ifndef RTE_TEST_MESSAGES_HPP
#define RTE_TEST_MESSAGES_HPP

// Generated Header file RTE_Test_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RTEERR_TEST_OUTSIDE_TASKING                    SAPDBErr_MessageList::Error,18051,"Must be called by a task",0
#define RTEERR_TEST_OUTSIDE_TASKING_ID 18051
#define RTEERR_TEST_OUTSIDE_SESSION                    SAPDBErr_MessageList::Error,18052,"Must be called inside SQL Session",0
#define RTEERR_TEST_OUTSIDE_SESSION_ID 18052
#define RTEERR_TEST_UNKNOWN_SUBCOMMAND                 SAPDBErr_MessageList::Error,18053,"Unknown subcommand %s",1
#define RTEERR_TEST_UNKNOWN_SUBCOMMAND_ID 18053
#define RTEERR_TEST_NON_UNIQUE_COMMAND                 SAPDBErr_MessageList::Error,18054,"Must have unique subcommand. Got %s, but found %s",2
#define RTEERR_TEST_NON_UNIQUE_COMMAND_ID 18054
#define RTEERR_TEST_SLEEP_WAS_ABORTED                  SAPDBErr_MessageList::Error,18055,"SLEEP(3+2) T%s was aborted without cancel",1
#define RTEERR_TEST_SLEEP_WAS_ABORTED_ID 18055
#define RTEERR_TEST_TIMEOUT                            SAPDBErr_MessageList::Error,18056,"%s T%s timeout",2
#define RTEERR_TEST_TIMEOUT_ID 18056
#define RTEERR_TEST_INIT_PARAMETER                     SAPDBErr_MessageList::Error,18057,"%s T%s bad parameter %s %s %s",5
#define RTEERR_TEST_INIT_PARAMETER_ID 18057
#define RTEERR_TEST_SECOND_INITIALIZATION              SAPDBErr_MessageList::Error,18058,"%s T%s already initialized",2
#define RTEERR_TEST_SECOND_INITIALIZATION_ID 18058
#define RTEERR_TEST_PARAMETER                          SAPDBErr_MessageList::Error,18059,"%s T%s bad parameter %s",3
#define RTEERR_TEST_PARAMETER_ID 18059
#define RTEERR_TEST_NOT_INITIALIZED                    SAPDBErr_MessageList::Error,18060,"%s T%s not initialized",2
#define RTEERR_TEST_NOT_INITIALIZED_ID 18060
#define RTEERR_TEST_ABORTING                           SAPDBErr_MessageList::Error,18061,"%s T%s aborting",2
#define RTEERR_TEST_ABORTING_ID 18061
#define RTEERR_TEST_INDEX_OUT_OF_RANGE                 SAPDBErr_MessageList::Error,18062,"%s T%s region %s or task %s out of range",4
#define RTEERR_TEST_INDEX_OUT_OF_RANGE_ID 18062
#define RTEERR_TEST_GO_PENDING                         SAPDBErr_MessageList::Error,18063,"%s T%s go already pending",2
#define RTEERR_TEST_GO_PENDING_ID 18063
#define RTEERR_TEST_WAITER_FOUND                       SAPDBErr_MessageList::Error,18064,"%s T%s found waiter in semaphore",2
#define RTEERR_TEST_WAITER_FOUND_ID 18064
#define RTEWARN_TESTS_DISABLED                         SAPDBErr_MessageList::Warning,18090,"All RTE tests disabled",0
#define RTEWARN_TESTS_DISABLED_ID 18090
#define RTEINFO_TEST_WAS_CANCELLED                     SAPDBErr_MessageList::Info,18100,"%s T%s was cancelled",2
#define RTEINFO_TEST_WAS_CANCELLED_ID 18100
#define RTEINFO_TEST                                   SAPDBErr_MessageList::Info,18101,"%s T%s",2
#define RTEINFO_TEST_ID 18101
#define RTEINFO_TEST_WOKE_UP                           SAPDBErr_MessageList::Info,18102,"T%s woke up",1
#define RTEINFO_TEST_WOKE_UP_ID 18102

#endif /* RTE_TEST_MESSAGES_HPP */