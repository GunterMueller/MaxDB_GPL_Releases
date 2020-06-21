/*!
  \file    DBMSrvCmd_InstInfo.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command inst_info

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "cn/buildinfo/buildinfo.h"

#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_InstInfo.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_InstInfo
//-----------------------------------------------------------------------------

#define INFO_FULL_S "f"
#define INFO_FULL_F "full"

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmd_InstInfo::DBMSrvCmd_InstInfo
//-----------------------------------------------------------------------------
DBMSrvCmd_InstInfo::DBMSrvCmd_InstInfo()
    : DBMSrv_Command( DBMSrv_Command::KeyInstInfo, false, DBMSrvCmd_InstInfo::m_LongHelp ) 
{
} // end DBMSrvCmd_InstInfo::DBMSrvCmd_InstInfo

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmd_InstInfo::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmd_InstInfo::m_LongHelp=
"@command inst_info You display the version information about the binary components (executables) of an "
                    "installation. These information contains internal development data to "
                    "identify an installation during a support call. "
    "@preconditions You do not need to log on to the Database Manager to execute this DBM command. "
                   "Nor do you require any server authorization."
    "@syntax inst_info [[-f[ull]] <component>]"
    "@param  -full       You display the full information about the specified component. "
    "@param  <component> You display only informations about the specified component. Without "
                         "the <component> argument you display a list of available components with "
                         "their buildnumbers."
    "@reply OK<NL>"
           "<component>         = <buildnumber><NL>"
           "<component>         = <buildnumber><NL>"
           "...|<NL>"
           "OK<NL>"
           "BUILDNUMBER     = <buildnumber>|<NL>"
           "OK<NL>"
           "FILENAME        = <filename><NL>"
           "BUILDNUMBER     = <buildnumber><NL>"
           "MAKEPATH        = <makepath><NL>"
           "DATE            = <date><NL>"
           "MAKEUSER        = <makeuser><NL>"
           "MAKEID          = <makeid><NL>"
           "MAKEDATE        = <makedate><NL>"
           "MAKESTATE       = <makestate><NL>"
           "PURPOSE         = <purpose><NL>"
           "CHANGELIST      = <changelist><NL>"

           "@replyvalue <component>     Binary component of the installation."
           "@replyvalue <buildnumber>   Buildnumber of the component"
           "@replyvalue <filename>      Filename of the specified component"
           "@replyvalue <buildnumber>   Buildnumber of the specified component"
           "@replyvalue <makepath>      Makepath of the specified component"
           "@replyvalue <date>          Date of the specified component"
           "@replyvalue <makeuser>      User during make"
           "@replyvalue <makeid>        Identifier of make"
           "@replyvalue <makestate>     State of make"
           "@replyvalue <purpose>       Pupose of make"
           "@replyvalue <changelist>    Changelist of the version management tool";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmd_InstInfo::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_InstInfo::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmd_InstInfo oVersion;

  return oVersion.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmd_InstInfo::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmd_InstInfo::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  Tools_DynamicUTF8String sComponent = command->oArguments.getValue(1);
  Tools_DynamicUTF8String sOption    = command->oArguments.getName(1);

  Reply.startWithOK();

  if (sOption.Length() > 0) {
    sOption.ToLower();
    if ((sOption == INFO_FULL_S) || (sOption == INFO_FULL_F)) {
      if (sComponent.Length() == 0) {
        sComponent = command->oArguments.getValue(2);
      } // end if

      if (sComponent.Length() == 0) {
        nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
        Reply.appendLine("-1,missing component after option '"INFO_FULL_S"'");
      } else {
        if (!cn46BuildInfoFull(sComponent.CharPtr(),Reply.giveCurrentEnd(), Reply.giveAvailableSpace())) {
          nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
          Reply.appendString("-1,cannot open '");
          Reply.appendString(sComponent.CharPtr());
          Reply.appendLine("'");
        } // end if
      } // end if
    } else {
      nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
      Reply.appendString("-1,unknown option '");
      Reply.appendString(command->oArguments.getName(1).CharPtr());
      Reply.appendLine("'");
    } // end if
  } else if (sComponent.Length() > 0) {
    if (!cn46BuildInfo(sComponent.CharPtr(), Reply.giveCurrentEnd(), Reply.giveAvailableSpace())) {
      nFuncReturn = Reply.startWithError(ERR_PARAM_CN00);
      Reply.appendString("-1,cannot open '");
      Reply.appendString(sComponent.CharPtr());
      Reply.appendLine("'");
    } // end if
  } else {
    if (!cn46BuildInfoList(Reply.giveCurrentEnd(), Reply.giveAvailableSpace())) {
      nFuncReturn = Reply.startWithError(ERR_CN00);
    } // end if
  } // end if

  Reply.adaptLength();

  return nFuncReturn;
} // end DBMSrvCmd_InstInfo::run
