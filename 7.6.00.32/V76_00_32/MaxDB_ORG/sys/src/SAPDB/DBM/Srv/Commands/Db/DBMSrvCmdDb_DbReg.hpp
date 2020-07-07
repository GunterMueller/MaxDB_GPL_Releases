/*!
  \file    DBMSrvCmdDb_DbReg.hpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   declaration of class handling database registration

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2005 SAP AG

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

#if !defined(DBMSrvCmdDb_DbReg_hpp)
#define DBMSrvCmdDb_DbReg_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

class DBMSrvUsr_Password;

/*! \brief A class for handling the DBMServer command db_unregister
    
    This command object handles the database registration
 */
class DBMSrvCmdDb_DbReg : public DBMSrv_Command
{
  public:
    DBMSrvCmdDb_DbReg( bool obsolete = false );

    //! the function implementing run call
    static tcn00_Error runCommand(VControlDataT * vcontrol,
                                  CommandT      * command,
                                  char          * replyData,
                                  int           * replyLen,
                                  int             replyLenMax);

    //! the function implementing the command
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & theReply);

    static const char* const m_LongHelp;

    static const char * getCommandName ( );
private:
    bool CheckVersionAndUser
        ( tsp00_DbNamec              szDbName,
          const DBMSrvUsr_Password & oDBMUserAndPassword,
          DBMSrvMsg_Error          & oError );

    //! register the database
    bool ReRegisterDatabase
        ( const tsp00_Pathc        & szDBRoot,
          const tsp00_DbNamec      & szDbName,
          const DBMSrvUsr_Password & oDBMUserAndPassword,
          DBMSrvMsg_Error          & oError );

    bool SetOsUser
        ( const tsp00_DbNamec      & dbname,
          bool                       bDeleteUser,
          const DBMSrvUsr_Password & oUser,
          DBMSrvMsg_Error          & oError );

    bool SetAutostart
        ( const tsp00_DbNamec      & dbname,
          bool                       bValue,
          DBMSrvMsg_Error          & oError );

    bool SetGroup
        ( const tsp00_DbNamec      & dbname,
          const tsp00_Namec        & szGroup,
          DBMSrvMsg_Error          & oError );

}; // end class DBMSrvCmdDb_DbReg

#endif // DBMSrvCmdDb_DbReg_hpp
