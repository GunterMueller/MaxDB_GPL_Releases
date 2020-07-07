/*!
  @file           ProcRTE_ServerControl.hpp
  @author         DanielD
  @brief          class ServerControl controls the ProcServer process.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



\endif
*/


#ifndef PROCRTE_SERVERCONTROL_HPP
#define PROCRTE_SERVERCONTROL_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "gsp00.h"

class RTETask_ITaskClientConnection;
class ProcRTE_ServerControlPrivates;
class SAPDBErr_MessageList;

/*!
  @class          ProcRTE_ServerControl

 */

class ProcRTE_ServerControl
{
public:
    /*!
       @brief          creares a new instance
       @param          commandTemplate [in] contains the absolute path
        to the proc server executable. Any additional
        required parameters like the path
        to a named pipe are encoded as ${varname}.

     */

    static ProcRTE_ServerControl * createServerControl (
            bool         connectToRunning,
            const char * commandTemplate,
            SAPDB_Int4   templateLen,
            SAPDBErr_MessageList * messageList);

    ~ProcRTE_ServerControl ();

    /*!
       @brief          stops the proc server

     */
    void stop ();

    /*!
       @brief          creates a new session object


           When this call finishes, the session waits for the first
           procedure call.

     */
    RTETask_ITaskClientConnection * createSession (SAPDBErr_MessageList * messageList);

    /*!
       @brief          deletes session


           The client session in the proc server is expected
           to have returned from the last sqlareceive
           (possibly sqlarelease?)
           This method is called by ProcRTE_ServerSession::close (),
           the kernel session is not required to keep the reference
           to the ServerControl.

     */
    void releaseSession (RTETask_ITaskClientConnection * session);

    /*!
       @brief          checks if the proc server is still alive.

     */
    bool isRunning () const;

    /*!
       @brief          try a reconnect if connection is down.

     */
    bool tryReconnect (SAPDBErr_MessageList * messageList);

    /*!
       @brief          executes a custom command
       category       Goody
     */

    bool cmd (const SAPDB_Char     * cmdString,
              SAPDB_Char           * outbuf,
              SAPDB_Int4             outbufSize,
              SAPDB_Int4           & bytesRead);

    /*!
        @brief          executes a command, resulting in true or false
     */
    bool simpleCmd (const SAPDB_Char * cmdString);

    /*!
        @brief          executes a command, resulting in true or false
     */
    bool simpleCmd (const SAPDB_Char * cmdString, int cmdlen);

protected:
    ProcRTE_ServerControl ();
    ProcRTE_ServerControlPrivates * privates;
    /*!
       @brief pure virutal connect call
       Must be overloaded with actual connect operation
       @param messageList [out] error message
       @return true if conencted, false if connect failed (messageList filled)
     */
    virtual bool connect (SAPDBErr_MessageList * messageList) = 0;

private:
    ProcRTE_ServerControl *  createServerControl (const char * commandTemplate, SAPDBErr_MessageList * messageList);

private:
    bool running;
};



/*!
   @brief          the following communication kinds are possible


   - named pipe: this requires a special library on Windows
        to make the pipe available to certain programming languages
   - anonymous pipe with arbitrary handles: this requires a special
    library on windows to make the pipe available to certain
    programming languages
   - anonymous pipe on stdin, stdout: dangerous because printf etc.         will disturb the protocol
   - sockets (created by kernel):
   - sockets (created by ProcServer):

 */


#endif
