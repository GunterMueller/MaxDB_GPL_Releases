/*
        ========== licence begin  GPL
        Copyright (c) 2005 SAP AG

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

#ifndef SDBMSG_DIAGNOSE_H
#define SDBMSG_DIAGNOSE_H
/*
This header file was generated by buildmsg.pl
--- Do not edit....
*/
#ifdef __cplusplus
#ifndef MSG_LIST_HPP
#include "SAPDB/Messages/Msg_List.hpp"
#endif
#ifndef MSG_ARG_HPP
#include "SAPDB/Messages/Msg_Arg.hpp"
#endif
#ifndef MSG_SIMPLEARG_HPP
#include "SAPDB/Messages/Msg_SimpleArg.hpp"
#endif
#endif

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2005-04-13 10:39:01 */
#error SDBMsg_DIAGNOSE.h was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_DIAGNOSE_COMPONENT "DIAGNOSE"
/* Begin of all message defines for component DIAGNOSE -> */
/* Begin of defines for message CHECK_INSTANCE_LOG_QUEUE_TITLE: ID = 1, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE 1, "DIAGNOSE", __FILE__, __LINE__, "MaxLoops: $MAX_LOOPS$, MinReserveSize: $MIN_RESERVE_SIZE$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE__MAX_LOOPS "MAX_LOOPS"
#define SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE__MIN_RESERVE_SIZE "MIN_RESERVE_SIZE"
/* <- End of defines for message CHECK_INSTANCE_LOG_QUEUE_TITLE */
/* Begin of defines for message CHECK_INSTANCE_LOG_QUEUE_RESULT: ID = 2, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT 2, "DIAGNOSE", __FILE__, __LINE__, "Reserve: $RESERVE_SIZE$, Time: $TIME_VALUE$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT__RESERVE_SIZE "RESERVE_SIZE"
#define SDBMSGTAG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT__TIME_VALUE "TIME_VALUE"
/* <- End of defines for message CHECK_INSTANCE_LOG_QUEUE_RESULT */
/* <- End of message defines for component DIAGNOSE */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_DIAGNOSE_ENUMERATION \
                    DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE=1, \
                    DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT=2
#define SDBMSG_DIAGNOSE_INITIALIZATIONLIST(x) \
                    x(SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_TITLE, "CHECK_INSTANCE_LOG_QUEUE_TITLE"), \
                    x(SDBMSG_DIAGNOSE_CHECK_INSTANCE_LOG_QUEUE_RESULT, "CHECK_INSTANCE_LOG_QUEUE_RESULT")
/* <- some helpful enumerations */

#endif /* SDBMSG_DIAGNOSE_H */
