/*!
  \file    DBMSrvCmd_DbSpeed.hpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   declaration of class handling DBMServer command db_speed
           
           

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

#include "DBM/Srv/DBMSrv_Command.hpp"

#if !defined(DBMSrvCmd_DbSpeed_hpp)
#define DBMSrvCmd_DbSpeed_hpp


//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

/*! \brief A class for handling the DBMServer command db_speed
    
    The run delivers information about the database state
 */
class DBMSrvCmd_DbSpeed : public DBMSrv_Command
{
  public:
    DBMSrvCmd_DbSpeed();

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

  private:
}; // end class DBMSrvCmd_DbSpeed

#endif // DBMSrvCmd_DbSpeed_hpp
