/*!
  -----------------------------------------------------------------------------
  module: arguments.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  handle the commandline arguments for dbanalyzer
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 2002-2005 SAP AG


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




  -----------------------------------------------------------------------------
*/

#ifndef _ARGUMENTS_HPP_
#define _ARGUMENTS_HPP_

#include "SAPDB/DBanalyzer/types.hpp"

#define DBA_ARGTXT_SRV "-n"
#define DBA_ARGTXT_DB  "-d"
#define DBA_ARGTXT_USR "-u"
#define DBA_ARGTXT_INT "-t"
#define DBA_ARGTXT_CON "-c"
#define DBA_ARGTXT_CFG "-f"
#define DBA_ARGTXT_OUT "-o"
#define DBA_ARGTXT_USG "-?"
#define DBA_ARGTXT_NOL "-nologo"
#define DBA_ARGTXT_IGN "-i"
#define DBA_ARGTXT_NOE "-noenv"
#define DBA_ARGTXT_STO "-stop"
#define DBA_ARGTXT_REC "-reconnect"
#define DBA_ARGTXT_STA "-state"
#define DBA_ARGTXT_VER "-V"
#define DBA_ARGTXT_DEL "-delete"

class DBA_arguments {
  public:
  DBA_arguments (DBA_int nCount, DBA_char *sArgs[]);
  ~DBA_arguments ();

  DBA_string GetApplication() { return m_sApplication; };
  DBA_string GetServer()      { return m_sServer; };
  DBA_string GetDatabase()    { return m_sDatabase; };
  DBA_string GetUser()        { return m_sUser; };
  DBA_string GetPassword()    { return m_sPassword; };
  DBA_string GetInterval()    { return m_sInterval; };
  DBA_string GetNumber()      { return m_sNumber; };
  DBA_string GetConsole()     { return m_sConsole; };
  DBA_string GetConfigFile()  { return m_sConfigFile; };
  DBA_string GetOutputDir()   { return m_sOutputDir; };
  DBA_bool   GetUsage()       { return m_bUsage; };
  DBA_bool   GetNoLogo()      { return m_bNoLogo; };
  DBA_bool   GetIgnore()      { return m_bIgnore; };
  DBA_bool   GetNoEnv()       { return m_bNoEnv; };
  DBA_bool   GetStop()        { return m_bStop; };
  DBA_bool   GetReconnect()   { return m_bReconnect; };
  DBA_bool   GetState()       { return m_bState; };
  DBA_bool   GetDeleteFlag()  { return m_bDelete; };
  DBA_string GetDeleteDate()  { return m_sDelete; };
  DBA_bool   GetVersion()     { return m_bVersion; };

  DBA_bool IsOK() { return m_bOK; };
  DBA_void Usage();
  DBA_void Version();
  DBA_void Logo();

  private:
  DBA_void Clear();
  DBA_bool Parse (DBA_int nCount, DBA_char *sArgs[]);
  DBA_bool ReadValue(DBA_string & sValue, DBA_string sPrompt, DBA_bool bHidden);

  private:
  DBA_bool   m_bOK;

  DBA_string m_sApplication;
  DBA_string m_sServer;
  DBA_string m_sDatabase;
  DBA_string m_sUser;
  DBA_string m_sPassword;
  DBA_string m_sInterval;
  DBA_string m_sNumber;
  DBA_string m_sConsole;
  DBA_string m_sConfigFile;
  DBA_string m_sOutputDir;
  DBA_bool   m_bUsage;
  DBA_bool   m_bNoLogo;
  DBA_bool   m_bIgnore;
  DBA_bool   m_bNoEnv;
  DBA_bool   m_bStop;
  DBA_bool   m_bReconnect;
  DBA_bool   m_bState;
  DBA_bool   m_bDelete;
  DBA_string m_sDelete;
  DBA_bool   m_bVersion;
};

#endif