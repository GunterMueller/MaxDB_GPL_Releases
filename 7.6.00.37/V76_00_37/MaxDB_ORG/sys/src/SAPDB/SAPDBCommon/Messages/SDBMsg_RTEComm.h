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

#ifndef SDBMSG_RTECOMM_H
#define SDBMSG_RTECOMM_H
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

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2005-02-21 15:48:22 */
#error __FILE__ was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_RTECOMM_COMPONENT "RTEComm"
/* Begin of all message defines for component RTEComm -> */
/* Begin of defines for message URI_UNKNOWN_PROTOCOL_FROM_HOST: ID = 1, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST 1, "RTEComm", __FILE__, __LINE__, "No communication protocol could be determined from host=$HOSTNAME$ and encryption=$BOOL$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST__BOOL "BOOL"
#define SDBMSGTAG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST__HOSTNAME "HOSTNAME"
/* <- End of defines for message URI_UNKNOWN_PROTOCOL_FROM_HOST */
/* Begin of defines for message URI_MISSING_SERVERDB: ID = 2, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_RTECOMM_URI_MISSING_SERVERDB 2, "RTEComm", __FILE__, __LINE__, "No database name was provided", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message URI_MISSING_SERVERDB */
/* Begin of defines for message URI_ESCAPE_DATABASE: ID = 3, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_RTECOMM_URI_ESCAPE_DATABASE 3, "RTEComm", __FILE__, __LINE__, "The given database name '$DATABASE_NAME$' could not be escaped", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_ESCAPE_DATABASE__DATABASE_NAME "DATABASE_NAME"
/* <- End of defines for message URI_ESCAPE_DATABASE */
/* Begin of defines for message URI_PORT_NOT_SUPPORTED: ID = 4, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_RTECOMM_URI_PORT_NOT_SUPPORTED 4, "RTEComm", __FILE__, __LINE__, "A port is not needed for the local communication protocol", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message URI_PORT_NOT_SUPPORTED */
/* Begin of defines for message URI_ENCRYPTION_NOT_SUPPORTED: ID = 5, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_RTECOMM_URI_ENCRYPTION_NOT_SUPPORTED 5, "RTEComm", __FILE__, __LINE__, "Encryption is not supported for the local communication protocol", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message URI_ENCRYPTION_NOT_SUPPORTED */
/* Begin of defines for message URI_MEMORY_ALLOCATION: ID = 6, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_RTECOMM_URI_MEMORY_ALLOCATION 6, "RTEComm", __FILE__, __LINE__, "Memory allocation of $ALLOC_SIZE$ bytes failed", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_MEMORY_ALLOCATION__ALLOC_SIZE "ALLOC_SIZE"
/* <- End of defines for message URI_MEMORY_ALLOCATION */
/* Begin of defines for message URI_BUFFER_TO_SMALL: ID = 7, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_RTECOMM_URI_BUFFER_TO_SMALL 7, "RTEComm", __FILE__, __LINE__, "Given URI buffer is to small: wanted=$BUFFERSIZE_NEEDED$ bytes, is=$BUFFERSIZE$ bytes", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_BUFFER_TO_SMALL__BUFFERSIZE "BUFFERSIZE"
#define SDBMSGTAG_RTECOMM_URI_BUFFER_TO_SMALL__BUFFERSIZE_NEEDED "BUFFERSIZE_NEEDED"
/* <- End of defines for message URI_BUFFER_TO_SMALL */
/* Begin of defines for message URI_HOSTNAME_LENGTH: ID = 8, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_RTECOMM_URI_HOSTNAME_LENGTH 8, "RTEComm", __FILE__, __LINE__, "The hostname length is to long: allowed=$STRING_LENGTH_MAX$, given=$STRING_LENGTH$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_HOSTNAME_LENGTH__STRING_LENGTH "STRING_LENGTH"
#define SDBMSGTAG_RTECOMM_URI_HOSTNAME_LENGTH__STRING_LENGTH_MAX "STRING_LENGTH_MAX"
/* <- End of defines for message URI_HOSTNAME_LENGTH */
/* Begin of defines for message URI_ESCAPE_OPTION: ID = 9, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_RTECOMM_URI_ESCAPE_OPTION 9, "RTEComm", __FILE__, __LINE__, "The given URI option name '$URI_OPTION_NAME$' could not be escaped", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_ESCAPE_OPTION__URI_OPTION_NAME "URI_OPTION_NAME"
/* <- End of defines for message URI_ESCAPE_OPTION */
/* Begin of defines for message URI_ESCAPE_VALUE: ID = 10, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_RTECOMM_URI_ESCAPE_VALUE 10, "RTEComm", __FILE__, __LINE__, "The given URI option value '$URI_OPTION_VALUE$' could not be escaped", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_ESCAPE_VALUE__URI_OPTION_VALUE "URI_OPTION_VALUE"
/* <- End of defines for message URI_ESCAPE_VALUE */
/* Begin of defines for message URI_MISSING_OPTION: ID = 11, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_RTECOMM_URI_MISSING_OPTION 11, "RTEComm", __FILE__, __LINE__, "No option was provided", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message URI_MISSING_OPTION */
/* Begin of defines for message URI_ILLEGAL_PROTOCOL: ID = 12, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_RTECOMM_URI_ILLEGAL_PROTOCOL 12, "RTEComm", __FILE__, __LINE__, "The protocol $URI_PROTOCOL$ is not allowed for the $URI_PATH$ URI", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECOMM_URI_ILLEGAL_PROTOCOL__URI_PATH "URI_PATH"
#define SDBMSGTAG_RTECOMM_URI_ILLEGAL_PROTOCOL__URI_PROTOCOL "URI_PROTOCOL"
/* <- End of defines for message URI_ILLEGAL_PROTOCOL */
/* <- End of message defines for component RTEComm */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_RTECOMM_ENUMERATION \
                    RTEComm_URI_UNKNOWN_PROTOCOL_FROM_HOST=1, \
                    RTEComm_URI_MISSING_SERVERDB=2, \
                    RTEComm_URI_ESCAPE_DATABASE=3, \
                    RTEComm_URI_PORT_NOT_SUPPORTED=4, \
                    RTEComm_URI_ENCRYPTION_NOT_SUPPORTED=5, \
                    RTEComm_URI_MEMORY_ALLOCATION=6, \
                    RTEComm_URI_BUFFER_TO_SMALL=7, \
                    RTEComm_URI_HOSTNAME_LENGTH=8, \
                    RTEComm_URI_ESCAPE_OPTION=9, \
                    RTEComm_URI_ESCAPE_VALUE=10, \
                    RTEComm_URI_MISSING_OPTION=11, \
                    RTEComm_URI_ILLEGAL_PROTOCOL=12
#define SDBMSG_RTECOMM_INITIALIZATIONLIST(x) \
                    x(SDBMSG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST, "URI_UNKNOWN_PROTOCOL_FROM_HOST"), \
                    x(SDBMSG_RTECOMM_URI_MISSING_SERVERDB, "URI_MISSING_SERVERDB"), \
                    x(SDBMSG_RTECOMM_URI_ESCAPE_DATABASE, "URI_ESCAPE_DATABASE"), \
                    x(SDBMSG_RTECOMM_URI_PORT_NOT_SUPPORTED, "URI_PORT_NOT_SUPPORTED"), \
                    x(SDBMSG_RTECOMM_URI_ENCRYPTION_NOT_SUPPORTED, "URI_ENCRYPTION_NOT_SUPPORTED"), \
                    x(SDBMSG_RTECOMM_URI_MEMORY_ALLOCATION, "URI_MEMORY_ALLOCATION"), \
                    x(SDBMSG_RTECOMM_URI_BUFFER_TO_SMALL, "URI_BUFFER_TO_SMALL"), \
                    x(SDBMSG_RTECOMM_URI_HOSTNAME_LENGTH, "URI_HOSTNAME_LENGTH"), \
                    x(SDBMSG_RTECOMM_URI_ESCAPE_OPTION, "URI_ESCAPE_OPTION"), \
                    x(SDBMSG_RTECOMM_URI_ESCAPE_VALUE, "URI_ESCAPE_VALUE"), \
                    x(SDBMSG_RTECOMM_URI_MISSING_OPTION, "URI_MISSING_OPTION"), \
                    x(SDBMSG_RTECOMM_URI_ILLEGAL_PROTOCOL, "URI_ILLEGAL_PROTOCOL")
/* <- some helpful enumerations */

#endif /* SDBMSG_RTECOMM_H */
