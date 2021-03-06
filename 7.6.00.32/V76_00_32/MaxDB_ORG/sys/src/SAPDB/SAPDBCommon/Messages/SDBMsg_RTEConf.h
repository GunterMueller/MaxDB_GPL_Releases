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

#ifndef SDBMSG_RTECONF_H
#define SDBMSG_RTECONF_H
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

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2005-05-03 08:47:07 */
#error SDBMsg_RTEConf.h was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_RTECONF_COMPONENT "RTEConf"
/* Begin of all message defines for component RTEConf -> */
/* Begin of defines for message DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION: ID = 1, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION 1, "RTEConf", __FILE__, __LINE__, "Device/File $DEVICE_OR_FILE$ is already used in definition for $VOLUME_TYPE$ $VOLUME_ID$.", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__VOLUME_ID "VOLUME_ID"
#define SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__VOLUME_TYPE "VOLUME_TYPE"
#define SDBMSGTAG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION__DEVICE_OR_FILE "DEVICE_OR_FILE"
/* <- End of defines for message DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION */
/* Begin of defines for message ERROR_IN_VOLUME_DEFINTION: ID = 2, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_RTECONF_ERROR_IN_VOLUME_DEFINTION 2, "RTEConf", __FILE__, __LINE__, "Error in volume definition for $VOLUME_TYPE$ $VOLUME_ID$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_RTECONF_ERROR_IN_VOLUME_DEFINTION__VOLUME_ID "VOLUME_ID"
#define SDBMSGTAG_RTECONF_ERROR_IN_VOLUME_DEFINTION__VOLUME_TYPE "VOLUME_TYPE"
/* <- End of defines for message ERROR_IN_VOLUME_DEFINTION */
/* <- End of message defines for component RTEConf */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_RTECONF_ENUMERATION \
                    RTEConf_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION=1, \
                    RTEConf_ERROR_IN_VOLUME_DEFINTION=2
#define SDBMSG_RTECONF_INITIALIZATIONLIST(x) \
                    x(SDBMSG_RTECONF_DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION, "DUPLICATE_DEVICE_OR_FILE_IN_VOLUME_DEFINITION"), \
                    x(SDBMSG_RTECONF_ERROR_IN_VOLUME_DEFINTION, "ERROR_IN_VOLUME_DEFINTION")
/* <- some helpful enumerations */

#endif /* SDBMSG_RTECONF_H */
