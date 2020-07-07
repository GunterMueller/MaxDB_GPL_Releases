/*!
  \file    DBMSrvCmdTrc_TraceOn.hpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   declaration of class handling DBMServer command trace_on
           
           

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

#if !defined(DBMSrvCmdTrc_TraceOn_hpp)
#define DBMSrvCmdTrc_TraceOn_hpp


//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

/*! \brief A class for handling the DBMServer command trace_on
    
    This comand object switches on a trace area.
 */
class DBMSrvCmdTrc_TraceOn : public DBMSrv_Command
{
  public:
    DBMSrvCmdTrc_TraceOn();

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
  static tcn00_Error SwitchAll
      ( VControlDataT * vcontrol,
        int             nLevel,
        bool            bOn,
        DBMSrv_Reply  & Reply );

  static tcn00_Error Switch
      ( VControlDataT * vcontrol,
        const char    * szTokens,
        bool            bOn,
        DBMSrv_Reply  & oReply );

  static const char * const OPTION_ALL;

  friend class DBMSrvCmdTrc_TraceOff;

}; // end class DBMSrvCmdTrc_TraceOn

#endif // DBMSrvCmdTrc_TraceOn_hpp
