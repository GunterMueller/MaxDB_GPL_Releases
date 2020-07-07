/*!
  -----------------------------------------------------------------------------
  module: database.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  odbc database connection 
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

#ifndef _DATABASE_HPP_
#define _DATABASE_HPP_

#include <time.h>

#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
#include "SAPDB/DBanalyzer/types.hpp"
#include "SAPDB/DBanalyzer/snapshot.hpp"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp"

class StudioOAL_WError;
class DBA_handles;

class DBA_database {
  public:
  DBA_database (const DBA_string & sServer,
                const DBA_string & sDatabase,
                const DBA_string & sUser,
                const DBA_string & sPassword);
  ~DBA_database ();

  DBA_bool Connect ();

  DBA_bool Execute              (const DBA_string & sSQL, DBA_snapshot * pSnapshot);
  DBA_bool Execute              (const DBA_string & sSQL, DBA_snapshot * pSnapshot, StudioOAL_WResult * & pPreparedQuery);
  DBA_bool Disconnect ();

  DBA_int GetInstanceType     ( ) { return m_nInstanceType;  }
  DBA_string GetKernelVersion ( ) { return m_sKernelVersion; }
  DBA_string GetRundirectory  ( ) { return m_sRundirectory;  }
  DBA_string GetDriver        ( ) { return m_sDriver;        }
  DBA_int  GetMajor           ( ) { return m_nMajor;}
  DBA_int  GetMinor           ( ) { return m_nMinor;}
  DBA_int  GetLevel           ( ) { return m_nLevel;}
  DBA_int  GetBuild           ( ) { return m_nBuild;}

  DBA_bool CheckAnalyzer(DBA_uint nSid, DBA_uint nPid, const DBA_string & sDir);
  DBA_bool CheckSession ( );
  DBA_uint GetSid() {return m_nSid;}
  DBA_uint GetPid() {return m_nPid;}

  DBA_bool GetInfos         ( );
  DBA_bool LockSystemTables ( );
  DBA_bool Commit           ( );

  DBA_bool GetTimestamp(time_t & tTime);

  void     SetIgnoreErrors(DBA_string sIgnoreErrors);

  private:
  DBA_bool PrepareQuery         (const DBA_string & sSQL, StudioOAL_WResult * & pPreparedQuery);
  DBA_bool CheckReconnectError (DBA_long nError) const;
  DBA_string GetErrorMsg();
  DBA_bool CheckIgnoreError(DBA_long nError);

  private:
  DBA_string m_sServer;
  DBA_string m_sDatabase;
  DBA_string m_sUser;
  DBA_string m_sPassword;

  DBA_handles * m_pHandles;

  DBA_int    m_nInstanceType;
  DBA_string m_sKernelVersion;
  DBA_string m_sRundirectory;
  DBA_uint   m_nSid;
  DBA_uint   m_nPid;
  DBA_int    m_nMajor;
  DBA_int    m_nMinor;
  DBA_int    m_nLevel;
  DBA_int    m_nBuild;

  DBA_bool   m_bUseOldSysTables;

  typedef Tools_Vector<DBA_long>  ErrorList;
  ErrorList m_oIgnoreErrors;

  StudioOAL_WError * m_pError;

  DBA_string m_sDriver;

};

#endif
