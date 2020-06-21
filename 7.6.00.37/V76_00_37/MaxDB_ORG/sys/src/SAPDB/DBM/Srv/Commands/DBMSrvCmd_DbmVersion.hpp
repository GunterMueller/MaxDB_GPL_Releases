/*!
  \file    DBMSrvCmd_DbmVersion.hpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   declaration of class handling DBMServer command dbm_version
           
           

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

#if !defined(DBMSrvCmd_DbmVersion_hpp)
#define DBMSrvCmd_DbmVersion_hpp


//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

/*! \brief A class for handling the DBMServer command dbm_version
    
    The run delivers information about the DBMServer version
 */
class DBMSrvCmd_DbmVersion : public DBMSrv_Command
{
  public:
    DBMSrvCmd_DbmVersion();

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
    /*! boolean member to show the license of the software ("Free" or "Commercial")
      This member has two definition in DBMSrvCmd_DbmVersionFree.cpp with
      values false and in DBMSrvCmd_DbmVersionCommercial.cpp with value true.
      Its a linker feature that symbols in direct linked object files will overwrite
      symbols in libraries. The module DBMSrvCmd_DbmVersionFree.cpp is
      part of the cservlib.lib which will belinked to all dbmsrv executables. Only
      for the commercial dbmsrv variant the All DBMServer variants are linked via a library with
      DBMSrvCmd_DbmVersionFree.cpp. Only the commercial dbmsrv variant
      the file DBMSrvCmd_DbmVersionCommercial.cpp will be linked directly
      because of an entry in dbmsrv.lnk. (Don't hesitate to ask Bernd V. for more
      Information or in case of problems)
    */
    static const bool        m_bCommercial;
}; // end class DBMSrvCmd_DbmVersion

#endif // DBMSrvCmd_DbmVersion_hpp
