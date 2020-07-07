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

#ifndef SDBMSG_ADMIN_H
#define SDBMSG_ADMIN_H
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

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2005-06-22 10:59:07 */
#error SDBMsg_Admin.h was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_ADMIN_COMPONENT "Admin"
/* Begin of all message defines for component Admin -> */
/* Begin of defines for message NUMLOGQUEUESTOLARGE: ID = 1, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_ADMIN_NUMLOGQUEUESTOLARGE 1, "Admin", __FILE__, __LINE__, "The number of log queues is too big.", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message NUMLOGQUEUESTOLARGE */
/* Begin of defines for message ABORT: ID = 2, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_ADMIN_ABORT 2, "Admin", __FILE__, __LINE__, "A fatal error caused EMERGENCY SHUTDOWN. $ERRORTEXT$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_ADMIN_ABORT__ERRORTEXT "ERRORTEXT"
/* <- End of defines for message ABORT */
/* Begin of defines for message STATE_OFFLINE_REACHED: ID = 3, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_ADMIN_STATE_OFFLINE_REACHED 3, "Admin", __FILE__, __LINE__, "Database state: OFFLINE", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message STATE_OFFLINE_REACHED */
/* <- End of message defines for component Admin */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_ADMIN_ENUMERATION \
                    Admin_NUMLOGQUEUESTOLARGE=1, \
                    Admin_ABORT=2, \
                    Admin_STATE_OFFLINE_REACHED=3
#define SDBMSG_ADMIN_INITIALIZATIONLIST(x) \
                    x(SDBMSG_ADMIN_NUMLOGQUEUESTOLARGE, "NUMLOGQUEUESTOLARGE"), \
                    x(SDBMSG_ADMIN_ABORT, "ABORT"), \
                    x(SDBMSG_ADMIN_STATE_OFFLINE_REACHED, "STATE_OFFLINE_REACHED")
/* <- some helpful enumerations */

#endif /* SDBMSG_ADMIN_H */