/*



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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





*/
#if !defined(SAPCSCONHDL_H)
#define SAPCSCONHDL_H

#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#ifdef WIN32
#include <process.h>
#include <winbase.h>
#include <mapidefs.h>
#include <limits.h>
#endif

#include "ContentStorage/ContentStorageDefines.h"
#include "ContentStorage/CSMutex.h"

#include "hpa111ODCompr2.h" // ODCompr, ODDecompr

#ifdef WIN32
#define CS_CAST(c) (c)
#else
#define CS_CAST(c)
#endif

#define CHECK_SUM 4711

class DbHdlItem; /* defined in SAPCSConHdl.cpp */

// ******************************************************************************
class SAPCSConHdl  
{
protected:

    DbHdlItem*  FreeHdlList;    // list of waiting connections (not in use). FreeHdlList will be used with 
                                // last in, first out'. DB kernel close connections, that not used for long time
                                // (db parameter session timeout). But on client side these connections have still
                                // all connect and handle informations. If these connections will be used later, 
                                // ODBC reconnects automatically and prepares statements again. 
    DbHdlItem*  DbHdlList;      // list of connections with active transactions
    DbHdlItem*  LatestDbHdl;
    CSMutex     m_Lock;
    Options*    m_Options;  
    ZString     m_SectionName;
    ZString     m_contRepNr;
    long        m_sessionTimeout;
    long        m_maxOpenConn; // maximum number of open connections (max. value of m_numOpenConn). When this limit is
                               // reached and all connections are in use the request for a new connection returns an
                               // error.
    long        m_numOpenConn; // The actual number of open connections (== num. elems from FreeHdlList & DbHdlist).
    bool        m_firstConn;   // only true until first call of GetDbHdl

//  bool    InitStmt(DbHdlItem* dbHdl);
    bool    PrepareStmt(DbHdlItem* dbHdl);
//  bool    PrepareStmt(DbConnection connection, StmtType stmtNo);
    bool    DeleteFreeDbHdl(DbHdlItem* dbHdl, ZString& err);
    void    FatalError( char* methodName, long exception );

public:
            SAPCSConHdl(Options* options);
            ~SAPCSConHdl();
    bool    SetMaxOpenConn( long maxOpenConn );
    bool    SetSectionName( const ZString& sectionName );
    bool    SetSessionTimeout(long sessionTimeout);
    bool    DbDisconnect();
    bool    GetDbHdl(DbConnection& connection, bool& prepared, ZString& err);
    bool    GetOdbcHdl(DbConnection connection, StmtType stmtType, SQLHENV& henv, SQLHDBC& hdbc, SQLHSTMT& hstmt, 
                       ZString& err);
    bool    GetOdbcHdl(DbConnection connection, SQLHENV& henv, SQLHDBC& hdbc, ZString& err);
    bool    PutDbHdl(DbConnection connection, ZString& err);
    void    IsPrepared(DbConnection connection);
    void    SetDocIdOnly(DbConnection connection, bool docIDOnly);
    bool    GetDocIdOnly(DbConnection connection);
    long    SQLErr(ZString& err, DbConnection connection, StmtType stmtNo);

    tpa111_ODCompr2&    GetCompr( DbConnection connection );
    tpa111_ODDecompr2&  GetDecompr( DbConnection connection );
    tpa111_ODCompr2&    GetCompr2( DbConnection connection );
    tpa111_ODDecompr2&  GetDecompr2( DbConnection connection );
};
// ******************************************************************************

#endif // !defined(CONTENT_STORAGE_CONHDL_H)
