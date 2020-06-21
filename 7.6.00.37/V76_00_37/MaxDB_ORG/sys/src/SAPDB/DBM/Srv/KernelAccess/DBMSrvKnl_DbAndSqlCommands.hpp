/*!
  @file           DBMSrvKnl_DbAndSqlCommands.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Specification

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef _DBMSrvKnl_DbAndSqlCommands_HPP_
#define _DBMSrvKnl_DbAndSqlCommands_HPP_

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */

/*! @brief Global sessions container
    This object stores all the global DBMServer session to the kernel. */
class DBMSrvKnl_DbAndSqlCommands {
  public:
    /*! @brief  Creates the requested session.
        @param  oType      [IN]  - type of session to use
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error connect
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);

    /*! @brief  Destroys the requested session.
        @param  oType      [IN]  - type of session to use
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error release 
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);
    
    /*! @brief  Execute a command with the requested session.
        @param  oType      [IN]  - type of session to use
        @param  bNice      [IN]  - answer in nice format or not
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error execute
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        bool                                    bNice,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);

    /*! @brief  fetch additional answer data from the database kernel.
        @param  oType      [IN]  - type of session to use
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error fetch
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);

    /*! @brief  fetch additional answer data from the database kernel in the nice format.
        @param  oType      [IN]  - type of session to use
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error fetchnice
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);

    /*! @brief  Execute a command with the requested session.
        @param  oType      [IN]  - type of session to use
        @param  vcontrol   [IN]  - the well known vcontrol structure
        @param  command    [IN]  - the well known command structure
        @param  theReply   [IN]  - the well known reply object
        @return The well known DBMServer return code */
    static tcn00_Error info
      ( DBMSrvKnl_GlobalSessions::SessionType   oType,
        VControlDataT                         * vcontrol,
        CommandT                              * command,
        DBMSrv_Reply                          & theReply);

    /*! @brief  checks statement for DBA history action.
        @param  szStatement [IN]  - statement to check
        @param  pAction     [OUT] - action for DBA history
        @return true it is a DDBA action */
    static bool checkForDBAAction 
      ( const char *   szStatement, 
        const char * & pAction );

    /*! @brief  checks statement for a SYSDBA password change
        @param  szStatement   [IN]  - statement to check
        @param  szUser        [OUT] - affected user or empty if current user affected
        @param  szNewPassword [IN]  - the new password
        @return true if it is a password change */
    static bool checkForPasswordChange 
      ( const char              * szStatement,
        Tools_DynamicUTF8String & szUser,
        Tools_DynamicUTF8String & szNewPassword );

}; // end class DBMSrvKnl_DbAndSqlCommands

#endif /* _DBMSrvKnl_DbAndSqlCommands_HPP_ */

