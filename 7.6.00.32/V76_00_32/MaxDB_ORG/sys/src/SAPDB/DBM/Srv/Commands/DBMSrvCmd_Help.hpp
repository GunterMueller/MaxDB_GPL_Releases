/*!
  \file    DBMSrvCmd_Help.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a class for the DBMServer command help

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

#if !defined(DBMSrvCmd_Help_hpp)
#define DBMSrvCmd_Help_hpp


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "gcn00.h"

#include "DBM/Srv/DBMSrv_Command.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvCmd_Help
//-----------------------------------------------------------------------------

//! \brief a class handling the DBMServer command help
class DBMSrvCmd_Help : public DBMSrv_Command
{
  public:
    DBMSrvCmd_Help();

    //! a static string containing the explain information for the help command
    static const char * m_LongHelp;

    /*! the function implementing the help command for 7.5.00
        \param vcontrol    [INOUT] DBMServer session data
        \param command     [IN]    DBMServer command
        \param replyData   [OUT]   reply buffer
        \param replyLen    [OUT]   lenght of reply
        \param replyLenMax [IN]    max. length of reply buffer
        \return error code specified in gcn00.h */
    static tcn00_Error runCommand(VControlDataT * vcontrol,
                                  CommandT      * command,
                                  char          * replyData,
                                  int           * replyLen,
                                  int             replyLenMax);

    /*! the function implementing the help command
        \param vcontrol    [INOUT] DBMServer session data
        \param command     [IN]    DBMServer command
        \param theReply    [OUT]   reply buffer
        \return error code as specified in gcn00.h */
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & reply);

    /*! the function implementing  the help command for the DBM Server
        \param showObsolete  [IN]  flag signaling whether obsolete commands should be listed, or not
        \param searchSimilar [IN]  flag signaling whether a fuzzy search for commands should be done, or not (classic style)
        \param command       [IN]  name of the command or start of the name of the commands we are looking for
        \param reply         [OUT] reply buffer
        \return error code as specified in gcn00.h */
    tcn00_Error run(
        bool            showObsolete,
        bool            searchSimilar,
        const char    * command,
        size_t          commandLength,
        DBMSrv_Reply  & reply);

  private:
    tcn00_Error allocateCopyWithoutC(char * & copy, const char * string, size_t length, char c, DBMSrv_Reply  & Reply);
};

#endif
