/*!
  @file           IFR_Connection.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Connection
  @brief          Implements class for database connection
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
// Prevents a Warning 5004 Uninitialized variable "static_i"
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif

#ifndef externCpp
#define externCpp
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/IFR_Statement.h"
#include "Interfaces/Runtime/IFR_PreparedStmt.h"
#include "Interfaces/Runtime/IFR_SQLWarning.h"
#include "Interfaces/Runtime/IFR_ParseInfoCache.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Lock.h"
#include "Interfaces/Runtime/Util/IFRUtil_StackAllocator.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Factory.h"

IFR_BEGIN_NAMESPACE

/* need for strcasecmp */
#if defined(AIX) || defined(SVR4) || defined(SOLARIS)
#  include <strings.h>
#endif

#define SELECT_FETCH_FETCH_IMMEDIATE 0

#ifdef WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
#endif

// password encryption
externPascal void s02applencrypt (tsp00_Name VAR_ARRAY_REF pw_clear,
                                  tsp00_CryptPw VAR_ARRAY_REF pw_crypt);

/// The threshold for accumulated garbage parse ids being dropped.
#define IFR_CONNECTION_PARSEID_THRESHOLD   20
// The threshold for accumulated long descriptors.
// This is 1, as the locking of the LONG column should be removed asap.
#define IFR_CONNECTION_LONGDESC_THRESHOLD  0

// INCREMENT PROFILE DATA COUNTER
#define IFR_CONNECTION_PROFILE_COUNT_INC(x) (this->m_counter_4[IFR_EnvironmentProfile:: x##_ENV]++)
#define IFR_CONNECTION_PROFILE_COUNT_ADD(x, y) (this->m_counter_4[IFR_EnvironmentProfile:: x##_ENV]+=y)
#define IFR_CONNECTION_PROFILE_COUNT_8_ADD(x, y) (this->m_counter_8[IFR_EnvironmentProfile:: x##_8_ENV]+=y)

//----------------------------------------------------------------------
struct IFR_Connection_CachedPacket {
    IFR_Connection_CachedPacket *nextPacket;
    IFR_Int4                     size;

    static void releaseAll(void *packetPtr, SAPDBMem_IRawAllocator& allocator)
    {
        IFR_Connection_CachedPacket *p = (IFR_Connection_CachedPacket *) packetPtr;
        while(p) {
            IFR_Connection_CachedPacket *nextDelete = p->nextPacket;
            IFR_ALLOCATOR(allocator).Deallocate(p);
            p = nextDelete;
        }
    }
};

//----------------------------------------------------------------------
static inline IFR_Bool
IFR_Connection_checkBooleanOption(const char *checkstr)
{
    if(STRICMP(checkstr, "1") == 0) {
        return true;
    }
    if(STRICMP(checkstr, "YES") == 0) {
        return true;
    }
    if(STRICMP(checkstr, "TRUE") == 0) {
        return true;
    }
    return false;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::connect(const char *connectURL,
                        IFR_Length connectURLLength,
                        const char *connectCommand,
                        IFR_Length connectCommandLength,
                        const IFR_StringEncoding connectCommandEncoding,
                        IFR_ConnectProperties& properties)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_Connection, connect);
    DBUG_PRINT_BUFFER(connectURL, connectURLLength, connectURLLength, IFR_StringEncodingAscii);
    DBUG_PRINT_BUFFER(connectCommand, connectCommandLength, connectCommandLength, connectCommandEncoding);

    //>>> SQL TRACE
    IFR_SQL_TRACE << endl << "::CONNECT " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE

    IFR_Bool memory_ok=true;
    IFR_ConnectProperties usedProperties(properties, memory_ok);
    if(IFR_MEM_NOT_OK) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    clearError();

    if(!checkProperties(usedProperties)) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    const char *packetcount_str=properties.getProperty(IFR_CONNECTPROPERTY_PACKETCOUNT, "UNLIMITED");
    int packetcount=1;
    if(STRICMP(packetcount_str, "UNLIMITED")) {
        packetcount=atoi(packetcount_str);
        if(packetcount <= 0) {
            error().setRuntimeError(IFR_ERR_INVALID_PACKETCOUNT_S, packetcount_str);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }

    IFR_String url(IFR_StringEncodingAscii, allocator);
    IFR_String connectCommandStr(connectCommandEncoding, allocator);

    if(connectURL) {
        url.setBuffer(connectURL, connectURLLength, IFR_StringEncodingAscii, memory_ok);
    }
    if(connectCommand) {
        connectCommandStr.setBuffer(connectCommand, connectCommandLength, connectCommandEncoding, memory_ok);
    }
    char       *username         = 0;
    IFR_Length  usernamelength   = 0;
    IFR_StringEncoding userpwdencoding = IFR_StringEncodingAscii;
    
    // If URL AND command are 0, we try to look for a KEY option
    if(connectCommand == 0 && connectURL == 0) {
        if(usedProperties.containsProperty(IFR_CONNECTPROPERTY_KEY)) {
            char       *servernode       = 0;
            IFR_Length  servernodelength = 0;
            char       *serverdb         = 0;
            IFR_Length  serverdblength   = 0;
            // --- XUSER ---
            char x_servernode[256];
            IFR_Int4 x_servernode_length = 256;

            char x_serverdb[256];
            IFR_Int4 x_serverdb_length = 256;

            char x_username[256];
            IFR_Int4 x_username_length = 256;
            IFR_StringEncoding x_username_encoding;
            const char *tmp_unicode_str=properties.getProperty(IFR_CONNECTPROPERTY_UNICODE, "0");
            if(IFR_Connection_checkBooleanOption(tmp_unicode_str)) {
                x_username_encoding = IFR_StringEncodingUCS2Native;
            } else {
                x_username_encoding = IFR_StringEncodingAscii;
            }

            IFR_Int4 x_isolationlevel = -1;
            IFR_Int4 x_cachelimit = -1;
            IFR_Int4 x_sqlmode = -1;
            IFR_Int4 x_timeout = -1;
            //>>> SQL TRACE
            IFR_SQL_TRACE << "XUSERKEY  : '" << usedProperties.getProperty(IFR_CONNECTPROPERTY_KEY) << "'" << endl;
            //<<< SQL TRACE

            SQLDBC_IRuntime::Error runtimeerror;
            if(!runtime.checkKeyOption(usedProperties.getProperty(IFR_CONNECTPROPERTY_KEY),
                                       x_servernode,
                                       x_servernode_length,
                                       x_serverdb,
                                       x_serverdb_length,
                                       x_isolationlevel,
                                       x_cachelimit,
                                       x_sqlmode,
                                       x_timeout,
                                       x_username,
                                       x_username_length,
                                       x_username_encoding,
                                       runtimeerror)) {
                if(runtimeerror) {
                    error().setFromRuntimeError(runtimeerror);
                    DBUG_RETURN(IFR_NOT_OK);
                }
            } else {
                if(x_servernode_length) {
                    servernode = (char *)x_servernode;
                    servernodelength = x_servernode_length;
                }
                if(x_serverdb_length) {
                    serverdb = (char *)x_serverdb;
                    serverdblength = x_serverdb_length;
                }
                if(x_username_length) {
                    username = (char *)x_username;
                    usernamelength = x_username_length;
                    userpwdencoding = x_username_encoding;
                }
                if(x_isolationlevel != -1) {
                    char s[32];
                    sp77sprintf(s, 32, "%d", x_isolationlevel);
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_ISOLATIONLEVEL, s, memory_ok);
                }
                if(x_cachelimit != -1) {
                    char s[32];
                    sp77sprintf(s, 32, "%d", x_cachelimit);
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_CACHELIMIT, s, memory_ok);
                }
                if(x_timeout != -1) {
                    char s[32];
                    sp77sprintf(s, 32, "%d", x_timeout);
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_TIMEOUT, s, memory_ok);
                }
                if(x_sqlmode != -1) {
                    switch(x_sqlmode) {
                    case IFR_INTERNAL:
                        usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL", memory_ok);
                        break;
                    case IFR_ANSI:
                        usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "ANSI", memory_ok);
                        break;
                    case IFR_DB2:
                        usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "DB2", memory_ok);
                        break;
                    case IFR_ORACLE:
                        usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "ORACLE", memory_ok);
                        break;
                    case IFR_SAPR3:
                        usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "SAPR3", memory_ok);
                        break;
                    }
                }
                if(serverdb) {
                    if(!createConnectURL(url,
                                         (!servernode)?"":servernode,
                                         servernodelength,
                                         serverdb,
                                         serverdblength,
                                         usedProperties,
                                         memory_ok)) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
                if(username) {
                    if(!createConnectCommand(connectCommandStr,
                                             usedProperties,
                                             memory_ok)) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
            }
        }
    }

    if(IFR_MEM_NOT_OK) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
    }

    //<<< SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE  << "CONNECT URL    : '" << url << "'" << endl
                       << "CONNECT COMMAND: '" << connectCommandStr << "'" << endl;
        usedProperties.sqlTrace(IFR_TRACE_STREAM);
    }
    //>>> SQL TRACE

    SAPDB_Int4   packetListSize=2;
    void *packetlist[2];
    void *sessionInfoReply;
    SQLDBC_IRuntime::Error runtimeerror;

    SAPDB_Int8 tmpSessionId;
    SAPDB_Bool connectSuccess = runtime.getSession(url.getBuffer(),
                                                   connectCommandStr.getBuffer(),
                                                   username,
                                                   usernamelength,
                                                   0,
                                                   0,
                                                   connectCommandEncoding,
                                                   tmpSessionId,
                                                   (SAPDB_UInt4&)m_packetsize,
                                                   packetListSize,
                                                   packetlist,
                                                   &sessionInfoReply,
                                                   runtimeerror,
                                                   &allocator);


    if(!connectSuccess) {
        m_connectionid=-1;
        error().setRuntimeError(IFR_ERR_CONNECTFAILED_INTERNAL_SS, "RTE", runtimeerror.errortext);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    m_connectionid = tmpSessionId;
    //>>> SQL TRACE
    IFR_SQL_TRACE << "SESSION ID: " << m_connectionid << endl;
    //<<< SQL TRACE

    IFRPacket_ReplyPacket replypacket;

    replypacket.setPacket((tsp1_packet *)sessionInfoReply, 0);

    IFR_PACKET_TRACE << replypacket << endl;

    IFRPacket_ReplySegment connectreply(replypacket);
    if(connectreply.getSQLError(error(), allocator)) {
        release();
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_SessionInfoPart sessioninfopart;

    IFR_Retcode rc=connectreply.getPart(IFRPacket_PartKind::SessionInfoReturned_C, sessioninfopart);

    if(rc==IFR_NO_DATA_FOUND) {
        error().setRuntimeError(IFR_ERR_CONNECTFAILED_INTERNAL_SS, "INT", "missing session info part");
        release();
        DBUG_RETURN(IFR_NOT_OK);
    }

    setSessionInfos(sessioninfopart);

#if IFR_HAS_FEATUREPART
    IFRPacket_FeaturePart featurepart;
    rc = connectreply.getPart(IFRPacket_PartKind::Feature_C, featurepart);
    if(rc == IFR_OK) {
        evalFeaturePart(featurepart);
    }
#endif
#if SELECT_FETCH_FETCH_IMMEDIATE
    if(m_connectionflags.selectfetchstrategy == SelectFetchScrollable_C &&
       m_connectionflags.scrollable == 0) {
        m_connectionflags.selectfetchstrategy = SelectFetchFetchImmediate_C;
    }
#endif
    m_requestpacket=new IFR_ALLOCATOR(allocator)  IFRPacket_RequestPacket((tsp1_packet*)packetlist[0],
                                                                          m_packetsize,
                                                                          m_connectionflags.clientunicode?true:false,
                                                                          getDatabaseSQLMode(m_client_sqlmode),
                                                                          m_client_application,
                                                                          m_client_version,
                                                                          *this,
                                                                          memory_ok);
    if(m_requestpacket == 0 || IFR_MEM_NOT_OK) {
        release();
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    /* store connection data */
    runtime.lockMutex(m_status_lock);

    m_databasename.setBuffer("", IFR_StringEncodingAscii, memory_ok);
    m_hostname.setBuffer("", IFR_StringEncodingAscii, memory_ok);
    m_username.setBuffer("", IFR_StringEncodingAscii, memory_ok);
    m_connecturl.assign(url, memory_ok);


    setupConnectionData(connectCommandStr, memory_ok);
    if(IFR_MEM_NOT_OK) {
        runtime.releaseMutex(m_status_lock);
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    runtime.releaseMutex(m_status_lock);

    //>>> SQL TRACE
    IFR_SQL_TRACE << "OK" << endl;
    //<<< SQL TRACE

    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::connect(const char *servernode,
                        IFR_Length servernodeLength,
                        const char *serverdb,
                        IFR_Length serverdbLength,
                        const char *username,
                        IFR_Length usernameLength,
                        const char *password,
                        IFR_Length passwordLength,
                        const IFR_StringEncoding userpwdEncoding)
{
    // No lock, will be set in subsequent connect.
    IFR_Bool memory_ok = true;
    IFR_ConnectProperties emptyProperties(allocator, memory_ok);
    if(IFR_MEM_NOT_OK) {
        error().setMemoryAllocationFailed();
        return IFR_NOT_OK;
    }
    return this->connect(servernode, servernodeLength,
                         serverdb, serverdbLength,
                         username, usernameLength,
                         password, passwordLength,
                         userpwdEncoding, emptyProperties);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::connect(const char *p_servernode,
                        IFR_Length p_servernodeLength,
                        const char *p_serverdb,
                        IFR_Length p_serverdbLength,
                        const char *p_username,
                        IFR_Length p_usernameLength,
                        const char *password,
                        IFR_Length passwordLength,
                        const IFR_StringEncoding p_userpwdEncoding,
                        const IFR_ConnectProperties& properties)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_Connection, connect);
    DBUG_PRINT_BUFFER(p_servernode, p_servernodeLength, p_servernodeLength, IFR_StringEncodingAscii);
    DBUG_PRINT_BUFFER(p_serverdb, p_serverdbLength, p_serverdbLength, IFR_StringEncodingAscii);
    DBUG_PRINT_BUFFER(p_username, p_usernameLength, p_usernameLength, p_userpwdEncoding);

    IFR_Bool memory_ok=true;
    IFR_ConnectProperties usedProperties(properties, memory_ok);
    if(IFR_MEM_NOT_OK) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    clearError();

    // used variables for servernode, serverdb, username
    // These are different from the parameters because they can be overridden with XUSER
    char *servernode = (char *)p_servernode;
    IFR_Length servernodeLength = p_servernodeLength;
    char *serverdb = (char *)p_serverdb;
    IFR_Length serverdbLength = p_serverdbLength;
    char *username = (char *)p_username;
    IFR_Length usernameLength = p_usernameLength;
    IFR_StringEncoding userpwdEncoding = p_userpwdEncoding;

    //>>> SQL TRACE
    IFR_SQL_TRACE << endl << "::CONNECT " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE

    // --- XUSER ---
    char x_servernode[256];
    IFR_Int4 x_servernode_length = 256;

    char x_serverdb[256];
    IFR_Int4 x_serverdb_length = 256;

    char x_username[256];
    IFR_Int4 x_username_length = 256;
    IFR_StringEncoding x_username_encoding;
    const char *tmp_unicode_str=properties.getProperty(IFR_CONNECTPROPERTY_UNICODE, "0");
    if(IFR_Connection_checkBooleanOption(tmp_unicode_str)) {
        x_username_encoding = IFR_StringEncodingUCS2Native;
    } else {
        x_username_encoding = IFR_StringEncodingAscii;
    }

    IFR_Int4 x_isolationlevel = -1;
    IFR_Int4 x_cachelimit = -1;
    IFR_Int4 x_sqlmode = -1;
    IFR_Int4 x_timeout = -1;

    if(usedProperties.containsProperty(IFR_CONNECTPROPERTY_KEY)) {
        //>>> SQL TRACE
        IFR_SQL_TRACE << "XUSERKEY  : '" << usedProperties.getProperty(IFR_CONNECTPROPERTY_KEY) << "'" << endl;
        //<<< SQL TRACE

        SQLDBC_IRuntime::Error runtimeerror;
        if(!runtime.checkKeyOption(usedProperties.getProperty(IFR_CONNECTPROPERTY_KEY),
                                   x_servernode,
                                   x_servernode_length,
                                   x_serverdb,
                                   x_serverdb_length,
                                   x_isolationlevel,
                                   x_cachelimit,
                                   x_sqlmode,
                                   x_timeout,
                                   x_username,
                                   x_username_length,
                                   x_username_encoding,
                                   runtimeerror)) {
            if(runtimeerror) {
                error().setFromRuntimeError(runtimeerror);
                DBUG_RETURN(IFR_NOT_OK);
            }
        } else {
            if(x_servernode_length) {
                servernode = (char *)x_servernode;
                servernodeLength = x_servernode_length;
            }
            if(x_serverdb_length) {
                serverdb = (char *)x_serverdb;
                serverdbLength = x_serverdb_length;
            }
            if(x_username_length) {
                username = (char *)x_username;
                usernameLength = x_username_length;
                userpwdEncoding = x_username_encoding;
            }
            if(x_isolationlevel != -1) {
                char s[32];
                sp77sprintf(s, 32, "%d", x_isolationlevel);
                usedProperties.setProperty(IFR_CONNECTPROPERTY_ISOLATIONLEVEL, s, memory_ok);
            }
            if(x_cachelimit != -1) {
                char s[32];
                sp77sprintf(s, 32, "%d", x_cachelimit);
                usedProperties.setProperty(IFR_CONNECTPROPERTY_CACHELIMIT, s, memory_ok);
            }
            if(x_timeout != -1) {
                char s[32];
                sp77sprintf(s, 32, "%d", x_timeout);
                usedProperties.setProperty(IFR_CONNECTPROPERTY_TIMEOUT, s, memory_ok);
            }
            if(x_sqlmode != -1) {
                switch(x_sqlmode) {
                case IFR_INTERNAL:
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL", memory_ok);
                    break;
                case IFR_ANSI:
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "ANSI", memory_ok);
                    break;
                case IFR_DB2:
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "DB2", memory_ok);
                    break;
                case IFR_ORACLE:
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "ORACLE", memory_ok);
                    break;
                case IFR_SAPR3:
                    usedProperties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "SAPR3", memory_ok);
                    break;
                }
            }
        }
    }
    if(IFR_MEM_NOT_OK) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    // -- XUSER
    //>>> SQL TRACE
    IFR_SQL_TRACE << "SERVERNODE: '" << inputlength(servernodeLength) << servernode << "'" << endl
                  << "SERVERDB  : '" << inputlength(serverdbLength) << serverdb << "'" << endl
                  << "USER      : '" << inputencoding(userpwdEncoding)
                  << inputlength(usernameLength) << username << "'" << endl;
    if(IFR_SQL_TRACE_ENABLED) {
        usedProperties.sqlTrace(IFR_TRACE_STREAM);
    }
    //<<< SQL TRACE


    if(!checkProperties(usedProperties)) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(password == 0) {
        error().setRuntimeError(IFR_ERR_PASSWORD_MISSING);
        DBUG_RETURN(IFR_NOT_OK);
    }

    const char *packetcount_str=properties.getProperty(IFR_CONNECTPROPERTY_PACKETCOUNT, "UNLIMITED");
    int packetcount=1;
    if(STRICMP(packetcount_str, "UNLIMITED")) {
        packetcount=atoi(packetcount_str);
        if(packetcount <= 0) {
            error().setRuntimeError(IFR_ERR_INVALID_PACKETCOUNT_S, packetcount_str);
            DBUG_RETURN(IFR_NOT_OK);
        }
    }


    IFR_String url(IFR_StringEncodingAscii, allocator);
    if(!createConnectURL(url,
                         servernode,
                         servernodeLength,
                         serverdb,
                         serverdbLength,
                         usedProperties,
                         memory_ok)) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    // we now have a connect URL with all changes.
    m_databasename.setBuffer(serverdb, serverdbLength, IFR_StringEncodingAscii, memory_ok);
    if(servernode) {
        m_hostname.setBuffer(servernode, servernodeLength, IFR_StringEncodingAscii, memory_ok);
    } else {
        m_hostname.setBuffer("", 0, IFR_StringEncodingAscii, memory_ok);
    }
    if(username) {
        m_username.setBuffer(username, usernameLength, userpwdEncoding, memory_ok);
    } else {
        m_username.setBuffer("", 0, userpwdEncoding, memory_ok);
    }
    if(IFR_MEM_NOT_OK) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_String connectCommand(userpwdEncoding, allocator);
    if(!createConnectCommand(connectCommand,
                             usedProperties,
                             memory_ok)) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    //>>> SQL TRACE
    IFR_SQL_TRACE << "CONNECT COMMAND: " << connectCommand << endl;
    //<<< SQL TRACE


    SAPDB_Int4   packetListSize=2;
    void *packetlist[2];
    void *sessionInfoReply;
    SQLDBC_IRuntime::Error runtimeerror;
    SAPDB_Int8 tmpSessionId;
    SAPDB_Bool connectSuccess = runtime.getSession(url.getBuffer(),
                                                   connectCommand.getBuffer(),
                                                   username, 
                                                   usernameLength,
                                                   password,
                                                   passwordLength,
                                                   userpwdEncoding,
                                                   tmpSessionId,
                                                   (SAPDB_UInt4&)m_packetsize,
                                                   packetListSize,
                                                   packetlist,
                                                   &sessionInfoReply,
                                                   runtimeerror,
                                                   &allocator);
    m_connectionid=tmpSessionId;

    //>>> SQL TRACE
    IFR_SQL_TRACE << "SESSION ID: " << m_connectionid << endl;
    //<<< SQL TRACE

    if(!connectSuccess) {
        m_connectionid=-1;
        error().setRuntimeError(IFR_ERR_CONNECTFAILED_INTERNAL_SS, "RTE", runtimeerror.errortext);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_ReplyPacket replypacket;

    replypacket.setPacket((tsp1_packet *)sessionInfoReply, 0);

    IFR_PACKET_TRACE << replypacket << endl;
    
    IFRPacket_ReplySegment connectreply(replypacket);
    if(connectreply.getSQLError(error(), allocator)) {
        release();
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_SessionInfoPart sessioninfopart;

    IFR_Retcode rc=connectreply.getPart(IFRPacket_PartKind::SessionInfoReturned_C, sessioninfopart);

    if(rc==IFR_NO_DATA_FOUND) {
        error().setRuntimeError(IFR_ERR_CONNECTFAILED_INTERNAL_SS, "INT", "missing session info part");
        release();
        DBUG_RETURN(IFR_NOT_OK);
    }

    setSessionInfos(sessioninfopart);
#if IFR_HAS_FEATUREPART
    IFRPacket_FeaturePart featurepart;
    rc = connectreply.getPart(IFRPacket_PartKind::Feature_C, featurepart);
    if(rc == IFR_OK) {
        evalFeaturePart(featurepart);
    }
#endif
#if SELECT_FETCH_FETCH_IMMEDIATE
    if(m_connectionflags.selectfetchstrategy == SelectFetchScrollable_C &&
       m_connectionflags.scrollable == 0) {
        m_connectionflags.selectfetchstrategy = SelectFetchFetchImmediate_C;
    }
#endif
    m_requestpacket=new IFR_ALLOCATOR(allocator)  IFRPacket_RequestPacket((tsp1_packet*)packetlist[0],
                                                                          m_packetsize,
                                                                          m_connectionflags.clientunicode?true:false,
                                                                          getDatabaseSQLMode(m_client_sqlmode),
                                                                          m_client_application,
                                                                          m_client_version,
                                                                          *this,
                                                                          memory_ok);
    if(IFR_MEM_NOT_OK || m_requestpacket==0) {
        release();
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    /* store connection data */
    runtime.lockMutex(m_status_lock);

    m_connecturl.assign(url, memory_ok);
    if(IFR_MEM_NOT_OK) {
        runtime.releaseMutex(m_status_lock);
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    setupConnectionData(connectCommand, memory_ok);

    if(!memory_ok) {
        runtime.releaseMutex(m_status_lock);
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    runtime.releaseMutex(m_status_lock);

    //>>> SQL TRACE
    IFR_SQL_TRACE << "OK" << endl;
    //<<< SQL TRACE

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::checkProperties(IFR_ConnectProperties& properties)
{
    DBUG_METHOD_ENTER(IFR_ConnectProperties, checkProperties);
    IFR_Bool memory_ok=true;
    IFR_Bool isSAPR3 = false;
    
    const char *sqlmode = properties.getProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL");
    if(strcmp(sqlmode, "SAPR3") == 0) {
        isSAPR3 = true;
    }
    
    // look for application and version, and check them for correctness.
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_APPLICATION)) {
        properties.setProperty(IFR_CONNECTPROPERTY_APPLICATION, "ODB", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_APPVERSION)) {
        properties.setProperty(IFR_CONNECTPROPERTY_APPVERSION, "70700", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_SQLMODE)) {
        properties.setProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_PACKETCOUNT)) {
        properties.setProperty(IFR_CONNECTPROPERTY_PACKETCOUNT, "UNLIMITED", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_UNICODE)) {
        properties.setProperty(IFR_CONNECTPROPERTY_UNICODE, "0", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_STATEMENTCACHESIZE)) {
        properties.setProperty(IFR_CONNECTPROPERTY_STATEMENTCACHESIZE, "1000", memory_ok);
    }
    if(!properties.containsProperty(IFR_CONNECTPROPERTY_COMPNAME)) {
        properties.setProperty(IFR_CONNECTPROPERTY_COMPNAME, "R ifr80", memory_ok);
    }
   
    if(!properties.containsProperty("VARIABLEINPUT")) {
        //-- request new short field infos if possible
        properties.setProperty(IFR_CONNECTPROPERTY_VARIABLEINPUT, "1", memory_ok);
    }

    if(isSAPR3) {
        // properties.setProperty("TRANSLATE_BINARY_AS_ENCODED", "1", memory_ok); // obsolete
        properties.setProperty("IGNORE_COLUMNNAMES", "1", memory_ok);
        properties.setProperty("CACHE_ALL_STATEMENTS", "1", memory_ok);
        properties.setProperty("SELECTFETCHOPTIMIZE", "1", memory_ok);
        properties.setProperty("NUMBERS_WITHOUT_EXPONENT", "1", memory_ok);
        // set to large value to fetch always complete long value
        properties.setProperty("MINGETVALSIZE", "1048576", memory_ok); 
        if(!properties.containsProperty("OPTIMIZEDSTREAMS")) {
            properties.setProperty("OPTIMIZEDSTREAMS", "1", memory_ok);
        }

    }   
    
    if(!properties.containsProperty("SELECTFETCHOPTIMIZE")) {
        properties.setProperty("SELECTFETCHOPTIMIZE", "1", memory_ok);
    }
    if(!properties.containsProperty("REPARSE_ON_CONVERSIONERROR")) {
        properties.setProperty("REPARSE_ON_CONVERSIONERROR", "1", memory_ok);
    }
    
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }

    const char *client_application_property=properties.getProperty(IFR_CONNECTPROPERTY_APPLICATION);
    strncpy(m_client_application, client_application_property, 3);
    m_client_application[3]='\0';
    if(strlen(client_application_property)!=3) {
        error().setRuntimeError(IFR_ERR_INVALID_CONNECT_PROPERTY_S, "APPLICATION");
        DBUG_RETURN(false);
    }

    const char *client_version_property=properties.getProperty(IFR_CONNECTPROPERTY_APPVERSION);
    strncpy(m_client_version, client_version_property, 5);
    m_client_version[5]='\0';
    if(strlen(client_version_property)!=5 ||
       !isdigit(client_version_property[0]) ||
       !isdigit(client_version_property[1]) ||
       !isdigit(client_version_property[2]) ||
       !isdigit(client_version_property[3]) ||
       !isdigit(client_version_property[4])) {
        error().setRuntimeError(IFR_ERR_INVALID_CONNECT_PROPERTY_S, "APPVERSION");
        DBUG_RETURN(false);
    }

    // check and extract SQL mode
    const char *sqlmode_str=properties.getProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL");
    if((STRICMP(sqlmode_str, "ADABAS")==0)
       || (STRICMP(sqlmode_str, "SAPDB")==0)
       || (STRICMP(sqlmode_str, "INTERNAL")==0)) {
        m_client_sqlmode=IFR_INTERNAL;
    } else if(STRICMP(sqlmode_str, "ORACLE")==0) {
        m_client_sqlmode=IFR_ORACLE;
    } else if(STRICMP(sqlmode_str, "ANSI")==0) {
        m_client_sqlmode=IFR_ANSI;
    } else if(STRICMP(sqlmode_str, "DB2")==0) {
        m_client_sqlmode=IFR_DB2;
    } else if(STRICMP(sqlmode_str, "SAPR3")==0) {
        m_client_sqlmode=IFR_SAPR3;
    } else {
        error().setRuntimeError(IFR_ERR_INVALID_CONNECT_PROPERTY_S, "SQLMODE");
        DBUG_RETURN(false);
    }

    // check and extract Unicode
    const char *unicode_str=properties.getProperty(IFR_CONNECTPROPERTY_UNICODE, "0");
    if(STRICMP(unicode_str, "TRUE")==0 || atoi(unicode_str)) {
        m_connectionflags.clientunicode = 1;
    } else {
        m_connectionflags.clientunicode = 0;
    }
    
    //----------------------------------------------------------------------
    const char *producer_str = properties.getProperty("PRODUCER", "0");
    if (strcmp(producer_str, "0") != 0) {    // producer set at all?
      if (strcmp(producer_str, "NIL") == 0) {
        m_producer = sp1pr_nil;
      } else if (strcmp(producer_str, "USER") == 0) {
        m_producer = sp1pr_user_cmd;
      } else if (strcmp(producer_str, "INTERNAL") == 0) {
        m_producer = sp1pr_internal_cmd;
      } else if (strcmp(producer_str, "KERNEL") == 0) {
        m_producer = sp1pr_kernel;
      } else if (strcmp(producer_str, "INSTALLATION") == 0) {
        m_producer = sp1pr_installation;
      } else if (strcmp(producer_str, "VIEW_OPTIMIZER") == 0) {
        m_producer = sp1pr_view_optimizer;
      } else if (strcmp(producer_str, "COMPLEX_VIEW_HANDLING") == 0) {
        m_producer = sp1pr_complex_view_handling;
      } else if (strcmp(producer_str, "QUERYREWRITE") == 0) {
        m_producer = sp1pr_queryrewrite;
      } else {
        error().setRuntimeError(IFR_ERR_INVALID_CONNECT_PROPERTY_S, "PRODUCER");
        DBUG_RETURN(false);
      }
    }

    const char *minlongfetchsize_str = properties.getProperty("MINGETVALSIZE", "1");
    m_mingetvalsize = atoi(minlongfetchsize_str);
    // must be an EVEN number.
    if(m_mingetvalsize % 2) { 
        --m_mingetvalsize;
    }

    const char *chop_blanks_str = properties.getProperty("CHOPBLANKS", "0");
    if(atoi(chop_blanks_str)) {
        m_connectionflags.chop_blanks = 1;
    } else {
        m_connectionflags.chop_blanks = 0;
    }

    const char *numbers_wo_exp_str = properties.getProperty("NUMBERS_WITHOUT_EXPONENT", "0");
    if(atoi(numbers_wo_exp_str)) {
        m_connectionflags.numbers_without_exponent = 1;
    } else {
        m_connectionflags.numbers_without_exponent = 0;
    }

    const char *ascii_7bit_str = properties.getProperty("ASCII_IS_7BIT", "0");
    if(atoi(ascii_7bit_str)) {
        m_connectionflags.ascii_7bit = 1;
    } else {
        m_connectionflags.ascii_7bit = 0;
    }
    
    const char *odbc_bin2hex_str = properties.getProperty("ODBC_BIN2HEX", "0");
    if(atoi(odbc_bin2hex_str)) {
        m_connectionflags.odbc_bin2hex = 1;
    } else {
        m_connectionflags.odbc_bin2hex = 0;
    }

    const char *number_trunc_str = properties.getProperty("ODBC_NUMBER_TRUNCATION", "0");
    if(atoi(number_trunc_str)) {
        m_connectionflags.number_trunc = 1;
    } else {
        m_connectionflags.number_trunc = 0;
    }

    
    const char *cache_all_statements = properties.getProperty("CACHE_ALL_STATEMENTS", "0");
    if(atoi(cache_all_statements)) {
        m_connectionflags.cache_all_statements = 1;
    } else {
        m_connectionflags.cache_all_statements = 0;
    }

    const char *reparse_on_conversionerror = properties.getProperty("REPARSE_ON_CONVERSIONERROR", "0");
    if(atoi(reparse_on_conversionerror)) {
        m_connectionflags.reparse_conversionerror = 1;
    } else {
        m_connectionflags.reparse_conversionerror = 0;
    }

    const char *select_fetch_strategy = properties.getProperty("SELECTFETCHOPTIMIZE", "0");
    if(atoi(select_fetch_strategy)==1) {
        m_connectionflags.selectfetchstrategy = SelectFetchScrollable_C;
    } 
#if SELECT_FETCH_FETCH_IMMEDIATE
    else if(atoi(select_fetch_strategy)==2) {
        m_connectionflags.selectfetchstrategy = SelectFetchFetchImmediate_C;
    } 
#endif
    else {
        m_connectionflags.selectfetchstrategy = SelectFetchNone_C;
    }
    
    if(m_connectionflags.selectfetchstrategy == SelectFetchScrollable_C) {
        properties.setProperty(IFR_CONNECTPROPERTY_CHECKSCROLLABLE, "1", memory_ok);
    } else {
        properties.setProperty(IFR_CONNECTPROPERTY_CHECKSCROLLABLE, "0", memory_ok);
    }
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }
    
    //----------------------------------------------------------------------
    

    const char *packetcount_str=properties.getProperty(IFR_CONNECTPROPERTY_PACKETCOUNT, "UNLIMITED");
    int packetcount=1;
    if(STRICMP(packetcount_str, "UNLIMITED") == 0) {
        m_packetmode=IFRPacket_RequestPacket::Dynamic_C;
    } else {
        if(packetcount > 1) {
            m_packetmode=IFRPacket_RequestPacket::Dynamic_C;
        } else {
            m_packetmode=IFRPacket_RequestPacket::Root_C;
        }
    }

    const char *statementcachesize_str=properties.getProperty(IFR_CONNECTPROPERTY_STATEMENTCACHESIZE, "1000");
    IFRUtil_Delete(m_parseinfocache, allocator);
    if(statementcachesize_str) {
        if(STRICMP(statementcachesize_str, "UNLIMITED")==0) {
            m_parseinfocache=new IFR_ALLOCATOR(allocator)
                IFR_ParseInfoCache(-1, *this, memory_ok);
            if(m_parseinfocache == 0) {
                error().setMemoryAllocationFailed();
                DBUG_RETURN(false);
            } else if(!memory_ok) {
                error().setMemoryAllocationFailed();
                IFRUtil_Delete(m_parseinfocache, allocator);
                m_parseinfocache=0;
                DBUG_RETURN(false);
            }
        } else {
            char *endptr;
            long l=strtol(statementcachesize_str, &endptr, 0);
            if(*endptr!=0 || l>(long)MAX_IFR_INT4 || l<0) {
                error().setRuntimeError(IFR_ERR_INVALID_STATEMENTCACHESIZE_S,
                                        statementcachesize_str);
                DBUG_RETURN(false);
            }
            m_parseinfocache=new IFR_ALLOCATOR(allocator)
                IFR_ParseInfoCache((IFR_Int4)l, *this, memory_ok);
            if(m_parseinfocache == 0) {
                error().setMemoryAllocationFailed();
                DBUG_RETURN(false);
            } else if(!memory_ok) {
                error().setMemoryAllocationFailed();
                IFRUtil_Delete(m_parseinfocache, allocator);
                m_parseinfocache=0;
                DBUG_RETURN(false);
            }
        }
    } else {
        m_parseinfocache=new IFR_ALLOCATOR(allocator)
            IFR_ParseInfoCache(0, *this, memory_ok);
        if(m_parseinfocache == 0) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(false);
        } else if(!memory_ok) {
            error().setMemoryAllocationFailed();
            IFRUtil_Delete(m_parseinfocache, allocator);
            m_parseinfocache=0;
            DBUG_RETURN(false);
        }
    }

    const char *prefix = properties.getProperty("CURSORPREFIX", 0);
    if(prefix) {
        m_uniqueid.setPrefix(prefix, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(false);
        }
    }

    const char *drop_parseids_direct_str = properties.getProperty("DROPPARSEIDDIRECT", "0");
    if(atoi(drop_parseids_direct_str)==1) {
        m_accumulate_parseids = false;
    } else {
        m_accumulate_parseids = true;
    }


    DBUG_RETURN(true);
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::createConnectURL(IFR_String& url,
                                 const char *servernode,
                                 IFR_Length servernodeLength,
                                 const char *serverdb,
                                 IFR_Length serverdbLength,
                                 const IFR_ConnectProperties& properties,
                                 IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_ConnectProperties,createConnectURL);

    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }

    if(servernode==0) {
        servernode="";
    }
    if(serverdb==0) {
        error().setRuntimeError(IFR_ERR_SERVERDB_MISSING);
        DBUG_RETURN(false);
    }
    IFR_Bool servernode_is_url=false;
    IFR_Bool servernode_url_hasparameters=false;
    char maxdbbuf[7];
    char lvcbuf[11];
    memset(maxdbbuf, 0, sizeof(maxdbbuf));
    memset(lvcbuf, 0, sizeof(lvcbuf));
    strncpy(maxdbbuf, servernode, 6);
    strncpy(lvcbuf, servernode, 10);
    
    if(STRICMP(maxdbbuf, "maxdb:")==0 || STRICMP(lvcbuf, "livecache:")==0) {
        servernode_is_url=true;
        for(IFR_Int4 i=servernodeLength-1; i>=0; --i) {
            if(servernode[i] == '?') {
                servernode_url_hasparameters = true;
                break;
            }
        }
    }
    if(servernode_is_url) {
        url.append(servernode, IFR_StringEncodingAscii, servernodeLength, memory_ok);
    } else {
        if(strlen(servernode) == 0) {
            url.append("maxdb://./database/", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            url.append(serverdb, IFR_StringEncodingAscii, serverdbLength, memory_ok);            
        } else {
            // detect a SAP router string, and construct the URL accordingly
            if(servernode[0]=='/' &&
               (servernode[1]=='H' || servernode[1]=='h')) {
                url.append("maxdb:sapni:", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                url.append(servernode, IFR_StringEncodingAscii, servernodeLength, memory_ok);
                url.append(":inpas/database/", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                url.append(serverdb, IFR_StringEncodingAscii, serverdbLength, memory_ok);            
            } else {
                url.append("maxdb:remote://", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                url.append(servernode, IFR_StringEncodingAscii, servernodeLength, memory_ok);
                url.append("/database/", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                url.append(serverdb, IFR_StringEncodingAscii, serverdbLength, memory_ok);            
            }
        }
    }
        
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }
    
    if(strlen(properties.getParameterString())!=0) {
        if(servernode_url_hasparameters) {
            url.append("&", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        } else {
            url.append("?", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        }
        url.append(properties.getParameterString(), IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(false);
        }
    }
    DBUG_PRINT(url);
    DBUG_RETURN(true);
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::createConnectCommand(IFR_String& connectCommand,
                                     const IFR_ConnectProperties& properties,
                                     IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, createConnectCommand);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }
    
    connectCommand.append("CONNECT ? IDENTIFIED BY ? SQLMODE ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    connectCommand.append(properties.getProperty(IFR_CONNECTPROPERTY_SQLMODE),
        IFR_StringEncodingAscii, IFR_NTS, memory_ok);

    const char *timeout_str=properties.getProperty(IFR_CONNECTPROPERTY_TIMEOUT);
    if(timeout_str) {
        connectCommand.append(" TIMEOUT ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        connectCommand.append(timeout_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    }

    const char *isolevel_str=properties.getProperty(IFR_CONNECTPROPERTY_ISOLATIONLEVEL);
    if(isolevel_str) {
        char *errorptr;
        long tmp=strtol(isolevel_str, &errorptr, 0);
        // the isolation level must be in one of the predefined
        // values. and a number anyway
        if( (*errorptr!=0) ||
            (tmp!=0 && tmp!=1 && tmp!=10 && tmp!=15 && tmp!=2 && tmp!=20 && tmp!=3 && tmp!=30)) {
            error().setRuntimeError(IFR_ERR_INVALID_ISOLATIONLEVEL_S, isolevel_str);
            DBUG_RETURN(false);
        }
        connectCommand.append(" ISOLATION LEVEL ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        connectCommand.append(isolevel_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        m_isolationlevel=(IFR_Int4)tmp;
    } else {
        m_isolationlevel=1; // Default isolation level is 1 (read committed).
    }

    const char *cachelimit_str=properties.getProperty(IFR_CONNECTPROPERTY_CACHELIMIT);
    if(cachelimit_str) {
        connectCommand.append(" CACHELIMIT ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        connectCommand.append(cachelimit_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    }
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(false);
    }

    DBUG_PRINT(connectCommand);
    DBUG_RETURN(true);
}

//----------------------------------------------------------------------
IFR_Connection::IFR_Connection(IFR_Environment& environment,
                               SAPDBMem_IRawAllocator& iallocator,
                               IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(environment, iallocator),
 IFR_ConnectionItem(iallocator),
 m_environment(environment),
 m_uniqueid("SQLCURS_",
    IFR_StringEncodingAscii,
    environment.runtime,
    allocator,
    memory_ok),
 m_connecturl(IFR_StringEncodingAscii, iallocator),
 m_connectcommand(IFR_StringEncodingAscii, iallocator),
 m_databasename(IFR_StringEncodingAscii, iallocator),
 m_hostname(IFR_StringEncodingAscii, iallocator),
 m_username(IFR_StringEncodingAscii, iallocator),
 m_garbageparseids(iallocator),
 m_garbagecursors(iallocator, 0, m_username, memory_ok), // m_username is a dummy as AIX wants to use
                                                        // the copy constructor here.
 m_garbagelongs(iallocator),
 m_connectcount(0),
 m_sqlaexecute_lock(0),
 m_sqlaexecute_executing(0),
 m_status_lock(0),
 m_packetcache_lock(0),
 m_mingetvalsize(1),
 m_packetcache(0),
 m_producer(sp1pr_user_cmd),
 IFR_ConnectionProfile(0)
  {
    if(IFR_MEM_OK) {
        initializeMembers(memory_ok);
        SQLDBC_IRuntime::Error ignored;
        runtime.createMutex(m_sqlaexecute_lock, allocator, ignored);
        if(m_sqlaexecute_lock == 0) {
            error().setMemoryAllocationFailed();
            IFR_MEM_FAIL;
        } else {
            runtime.createMutex(m_status_lock, allocator, ignored);
            if(m_status_lock == 0) {
                IFR_MEM_FAIL;
                error().setMemoryAllocationFailed();
            } else {
                runtime.createMutex(m_packetcache_lock, allocator, ignored);
                if(m_packetcache_lock == 0) {
                    IFR_MEM_FAIL;
                    error().setMemoryAllocationFailed();
                  } else{
                    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_CONNECTIONCOUNT );
                  }
            }
        }
    }
}

//----------------------------------------------------------------------
IFR_Connection::~IFR_Connection()
{
    DBUG_METHOD_ENTER(IFR_Connection, ~IFR_Connection);
    if(m_connectionid >= 0) {
        close();
        // close does not throw way the statement, as close is called
        // in situations where the statement is still needed.
        if (m_internalstatement){
            releaseStatement(m_internalstatement);
            m_internalstatement=0;
        }
    } else {
        // the statement may be existing anyway ...
        if (m_internalstatement){
            releaseStatement(m_internalstatement);
            m_internalstatement=0;
        }
        clearError();
        IFR_SQL_TRACE << endl << "::RELEASE CONNECTION " << currenttime
                      << " [0x" << (void*)this << "]" << endl;
        if(m_packetcache) {
            IFR_Connection_CachedPacket::releaseAll(m_packetcache, allocator);
            m_packetcache = 0;
        }
    }

    SQLDBC_IRuntime::Error ignored;
    // The mutexes may be 0
    if(m_sqlaexecute_lock) {
        runtime.destroyMutex(m_sqlaexecute_lock, allocator, ignored);
    }
    if(m_status_lock) {
        runtime.destroyMutex(m_status_lock, allocator, ignored);
    }
    if(m_packetcache_lock) {
        runtime.destroyMutex(m_packetcache_lock, allocator, ignored);
    }
}

IFR_Retcode IFR_Connection::close(IFR_Bool commit, IFR_Bool sessionclosed)
{
    DBUG_METHOD_ENTER(IFR_Connection, close);

    clearError();

    IFR_SQL_TRACE << endl << "::CLOSE CONNECTION " << currenttime
                  << " [0x" << (void*)this << "]" << endl;

    IFR_Retcode rc = IFR_OK;
    IFR_Retcode rc_internal= IFR_OK;
    if (m_connectionid >= 0) {
        if(m_status_lock == 0 || m_sqlaexecute_lock == 0) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        runtime.lockMutex(m_status_lock);
        IFR_Bool memory_ok=true;
        clearError();
        if(!sessionclosed) {
            // ROLLBACK WORK RELEASE must not send any appendix data, as
            // the command already closes the session
            if(commit) {
                IFR_String close_statement("COMMIT WORK RELEASE",
                                           IFR_StringEncodingAscii,
                                           allocator,
                                           memory_ok);
                if(!memory_ok) {
                    runtime.releaseMutex(m_status_lock);
                    error().setMemoryAllocationFailed();
                    DBUG_RETURN(IFR_NOT_OK);
                }
                m_connection_closing = true;
                 rc_internal = m_internalstatement->execute(close_statement, false);
                if(m_internalstatement->error().isMemoryAllocationFailed()) {
                  runtime.releaseMutex(m_status_lock);
                  error().setMemoryAllocationFailed();
                  DBUG_RETURN(IFR_NOT_OK);
                }
            } else {
              if (isConnected()) {
                IFR_String close_statement("ROLLBACK WORK RELEASE",
                                           IFR_StringEncodingAscii,
                                           allocator,
                                           memory_ok);
                if(!memory_ok) {
                    runtime.releaseMutex(m_status_lock);
                    error().setMemoryAllocationFailed();
                    DBUG_RETURN(IFR_NOT_OK);
                }
                m_connection_closing = true;
                rc_internal = m_internalstatement->execute(close_statement, false);
                if (m_internalstatement->error().getErrorCode() == -10807) {
                  // ignore error for rollback work release
                  m_internalstatement->error().clear();
                  rc_internal = IFR_OK;
                }

                if(m_internalstatement->error().isMemoryAllocationFailed()) {
                  runtime.releaseMutex(m_status_lock);
                  error().setMemoryAllocationFailed();
                  DBUG_RETURN(IFR_NOT_OK);
                }
              }
            }
        }
        IFR_ErrorHndl error_internal;
        if (rc_internal != IFR_OK) {
            error_internal.assign(m_internalstatement->error(), false);
        }
        // releaseStatement(m_internalstatement);
        // m_internalstatement=0;
        IFRUtil_Delete(m_requestpacket, allocator);
        IFRUtil_Delete(m_parseinfocache, allocator);
        
        IFRUtil_Delete(m_specialparameters[0], allocator);
        IFRUtil_Delete(m_specialparameters[1], allocator);
        
        runtime.releaseMutex(m_status_lock);
        runtime.lockMutex(m_sqlaexecute_lock);
        rc = release();
        if (rc == IFR_OK && rc_internal != IFR_OK) {
            error().assign(error_internal, true); // submit error to connection
            rc = rc_internal;
        }
        initializeMembers(memory_ok, false);
        runtime.releaseMutex(m_sqlaexecute_lock);
    } else {
      error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
      DBUG_RETURN(IFR_NOT_OK);
    }

    if(m_packetcache) {
        IFR_Connection_CachedPacket::releaseAll(m_packetcache, allocator);
        m_packetcache = 0;
    }

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
void
IFR_Connection::initializeMembers(IFR_Bool& memory_ok, IFR_Bool clearStatement)
{
    m_connectionid     = -1;
    m_datetimeformat   = IFR_DateTimeFormat::None_C;
    m_requestpacket    = 0;
    m_packetsize       = 0;
    m_packetmode       = IFRPacket_RequestPacket::Dynamic_C;
    m_connecturl.clear();
    m_connectcommand.clear();
    m_databasename.clear();
    m_hostname.clear();
    m_username.clear();
    m_sql_connectionid = -1;
    m_client_sqlmode   = IFR_INTERNAL;
    m_isolationlevel   = 1;
    if(clearStatement) {
        m_internalstatement = 0;
    }
    m_parseinfocache      = 0;
    m_accumulate_parseids = true;
    m_withInfo          = true;
    m_garbageparseids.Clear();
    m_garbagecursors.Clear();
    m_garbagelongs.Clear();
    m_cancellationpoint = false;
    m_connection_closing = false;
    memset(m_kernelversion, 0, sizeof(m_kernelversion));
    memset(m_client_application, 0, sizeof(m_client_application));
    memset(m_client_version, 0,sizeof(m_client_version));
    m_error.setConnection(this);
    m_uniqueid.setPrefix("SQLCURS_", memory_ok);
    memset(&m_connectionflags, 0, sizeof(m_connectionflags));
    memset(m_specialparameters, 0, sizeof(m_specialparameters));
    ++m_connectcount;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::getAutoCommit() const
{
    DBUG_METHOD_ENTER(IFR_Connection, getAutoCommit);
    DBUG_RETURN(m_connectionflags.autocommit==1?true:false);
}

//----------------------------------------------------------------------
void
IFR_Connection::setAutoCommit(IFR_Bool autocommit)
{
    DBUG_METHOD_ENTER(IFR_Connection, setAutoCommit);
    DBUG_PRINT(autocommit);
    clearError();
    if(autocommit) {
        IFR_SQL_TRACE << "::SET AUTOCOMMIT ON " << currenttime << " [0x" << (void*)this << "]" << endl; 
        m_connectionflags.autocommit = 1;
    } else {
        IFR_SQL_TRACE << "::SET AUTOCOMMIT OFF " << currenttime << " [0x" << (void*)this << "]" << endl; 
        m_connectionflags.autocommit = 0;
    }
}
//----------------------------------------------------------------------
void
IFR_Connection::setSQLMode(IFR_SQLMode sqlmode)
{
    DBUG_METHOD_ENTER(IFR_Connection, setSQLMode);
    DBUG_PRINT(sqlmode);
    clearError();
    m_client_sqlmode=sqlmode;
}
//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::isClosed() const
{
    return ((m_connectionid == -1) ? true : false);
}

//----------------------------------------------------------------------
IFR_Statement *
IFR_Connection::createStatement()
{
    DBUG_METHOD_ENTER(IFR_Connection, createStatement);

    clearError();

    IFR_Bool memory_ok=true;
    IFR_Statement *statement=new IFR_ALLOCATOR(allocator) IFR_Statement(*this, memory_ok);
    if(statement == 0) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN((IFR_Statement*)0);
    }
    if(!memory_ok) {
        IFRUtil_Delete(statement, allocator);
        error().setMemoryAllocationFailed();
        DBUG_RETURN((IFR_Statement*)0);
    }
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_STATEMENTCOUNT );
    DBUG_RETURN(statement);
}

//----------------------------------------------------------------------
IFR_PreparedStmt *
IFR_Connection::createPreparedStatement()
{
    DBUG_METHOD_ENTER(IFR_Connection, createPreparedStatement);

    clearError();

    IFR_Bool memory_ok=true;
    IFR_PreparedStmt *result=new IFR_ALLOCATOR(allocator)  IFR_PreparedStmt (*this, memory_ok);
    if(result == 0) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN((IFR_PreparedStmt*)0);
    }
    if(!memory_ok) {
      IFRUtil_Delete(result, allocator);
      error().setMemoryAllocationFailed();
      DBUG_RETURN((IFR_PreparedStmt*)0);
    }
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_PREPAREDSTATEMENTCOUNT );
    DBUG_RETURN(result);
}

void
IFR_Connection::releaseStatement(IFR_Statement *stmt)
{
    DBUG_METHOD_ENTER(IFR_Connection, releaseStatement);

    stmt->submitCounters(this->m_counter_8, this->m_counter_4);

    clearError();

    IFRUtil_Delete(stmt, allocator);
}

void
IFR_Connection::releaseStatement(IFR_PreparedStmt *stmt)
{
    DBUG_METHOD_ENTER(IFR_Connection, releaseStatement);
    
    if (!stmt) return;
    stmt->submitCounters(this->m_counter_8, this->m_counter_4);
    clearError();
    IFRUtil_Delete(stmt, allocator);
}


//----------------------------------------------------------------------
IFR_Retcode IFR_Connection::commit()
{
    DBUG_METHOD_ENTER(IFR_Connection, commit);
    
    clearError();
    
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_COMMITCOUNT );

    IFR_SQL_TRACE << endl << "::COMMIT " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    runtime.lockMutex(m_status_lock);
    IFR_Bool memory_ok=true;
    if(m_internalstatement) {
        m_internalstatement->clearError();
    } else {
        memory_ok = false;
    }
    IFR_String commit_statement("COMMIT",
                                IFR_StringEncodingAscii,
                                allocator,
                                memory_ok);
    if(!memory_ok) {
        runtime.releaseMutex(m_status_lock);
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=m_internalstatement->execute(commit_statement);
    if(rc != IFR_OK) {
        error().assign(m_internalstatement->error(), true); // submit error to connection
    }
    runtime.releaseMutex(m_status_lock);
    DBUG_RETURN(rc);
}
//----------------------------------------------------------------------
IFR_Retcode IFR_Connection::stopKerneltrace()
{
    DBUG_METHOD_ENTER(IFR_Connection, stopKerneltrace);

    clearError();

    IFR_SQL_TRACE << endl << "::STOP KERNEL TRACE " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    runtime.lockMutex(m_status_lock);
    IFR_Bool memory_ok=true;
    if(m_internalstatement) {
        m_internalstatement->clearError();
    } else {
        memory_ok = false;
    }
    IFR_String commit_statement("DIAGNOSE VTRACE DEFAULT OFF",
                                IFR_StringEncodingAscii,
                                allocator,
                                memory_ok);
    if(!memory_ok) {
        runtime.releaseMutex(m_status_lock);
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=m_internalstatement->execute(commit_statement);
    if(rc != IFR_OK) {
        error().assign(m_internalstatement->error(), true); // submit error to connection
    }
    runtime.releaseMutex(m_status_lock);
    DBUG_RETURN(rc);
}
//----------------------------------------------------------------------
IFR_Retcode IFR_Connection::rollback()
{
    DBUG_METHOD_ENTER(IFR_Connection, rollback);

    clearError();
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_ROLLBACKCOUNT );
    IFR_SQL_TRACE << endl << "::ROLLBACK " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    runtime.lockMutex(m_status_lock);
    IFR_Bool memory_ok=true;
    if(m_internalstatement) {
        m_internalstatement->clearError();
    } else {
        memory_ok = false;
    }
    IFR_String rollback_statement("ROLLBACK", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        runtime.releaseMutex(m_status_lock);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=m_internalstatement->execute(rollback_statement);
    if(rc != IFR_OK) {
        error().assign(m_internalstatement->error(), true); // submit to connection
    }
    runtime.releaseMutex(m_status_lock);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::executeInternalCommand(const char *command)
{
    DBUG_METHOD_ENTER(IFR_Connection, executeInternalCommand);

    clearError();

    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    runtime.lockMutex(m_status_lock);
    IFR_Bool memory_ok=true;
    if(m_internalstatement) {
        m_internalstatement->clearError();
    } else {
        memory_ok = false;
    }
    IFR_String statement(command, IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        runtime.releaseMutex(m_status_lock);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=m_internalstatement->execute(statement);
    if(rc != IFR_OK) {
        error().assign(m_internalstatement->error(), true); // submit to connection
    }
    runtime.releaseMutex(m_status_lock);
    DBUG_RETURN(rc);

}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::getRequestPacket(IFRPacket_RequestPacket& result,
                                 IFR_ErrorHndl& err,
                                 const IFRPacket_RequestPacket::PacketType packetType)
{
    DBUG_METHOD_ENTER(IFR_Connection, getRequestPacket);
    // SQLDBC_IRuntime::TaskID currentid = runtime.getCurrentTaskID();
    switch(packetType) {
    case IFRPacket_RequestPacket::Dynamic_C: {

        tsp1_packet *packetdata = 0;

        if(m_packetcache) {
            runtime.lockMutex(m_packetcache_lock);
            if(m_packetcache) {
                IFR_Connection_CachedPacket *p = (IFR_Connection_CachedPacket*)m_packetcache;
                m_packetcache = p->nextPacket;
                packetdata = (tsp1_packet *) ((void *) p);
            }
            runtime.releaseMutex(m_packetcache_lock);
        }

        if(packetdata == 0) {
            packetdata=(tsp1_packet *) IFR_ALLOCATOR(allocator).Allocate(m_packetsize);
        }
        if(packetdata == 0) {
            err.setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFRPacket_DynamicLock *plock = new IFR_ALLOCATOR(allocator) IFRPacket_DynamicLock(runtime, allocator, this);
        if(plock == 0) {
            err.setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFRPacket_RequestPacket rp(packetdata,
                                   m_packetsize,
                                   m_connectionflags.clientunicode?true:false,
                                   getDatabaseSQLMode(m_client_sqlmode),
                                   m_client_application,
                                   m_client_version,
                                   IFRPacket_RequestPacket::Dynamic_C,
                                   plock,
                                   *this);
        DBUG_RETURN(result.assign(rp, err));
    }
    case IFRPacket_RequestPacket::Root_C: {
        IFR_Retcode rc =result.assign(*m_requestpacket, err, &m_sqlaexecute_executing);
        if(rc == IFR_NO_DATA_FOUND) {
            rc = IFR_NOT_OK;
        }
        DBUG_RETURN(rc);
    }
    default:
        switch(m_packetmode) {
        case IFRPacket_RequestPacket::Root_C:
            DBUG_RETURN(result.assign(*m_requestpacket, err));
        default: {
            tsp1_packet *packetdata = 0;

            if(m_packetcache) {
                runtime.lockMutex(m_packetcache_lock);
                if(m_packetcache) {
                    IFR_Connection_CachedPacket *p = (IFR_Connection_CachedPacket*)m_packetcache;
                    m_packetcache = p->nextPacket;
                    packetdata = (tsp1_packet *) ((void *) p);
                }
                runtime.releaseMutex(m_packetcache_lock);
            }

            if(packetdata == 0) {
                packetdata=(tsp1_packet *) IFR_ALLOCATOR(allocator).Allocate(m_packetsize);
            }
            IFRPacket_DynamicLock *plock = new IFR_ALLOCATOR(allocator) IFRPacket_DynamicLock(runtime, allocator, this);
            if(plock == 0) {
                err.setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
            IFRPacket_RequestPacket rp(packetdata,
                                       m_packetsize,
                                       m_connectionflags.clientunicode?true:false,
                                       getDatabaseSQLMode(m_client_sqlmode),
                                       m_client_application,
                                       m_client_version,
                                       IFRPacket_RequestPacket::Dynamic_C,
                                       plock,
                                       *this);
            DBUG_RETURN(result.assign(rp, err));
        }
        }
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::sqlaexecute(IFRPacket_RequestPacket &requestPacket,
                            IFRPacket_ReplyPacket& replyPacket,
                            AppendMode appendFlag,
                            IFR_ErrorHndl& execError,
                            IFRUtil_AsyncOperation *operation)
{
    DBUG_METHOD_ENTER(IFR_Connection, sqlaexecute);
    execError.clear();

    IFR_Bool memory_ok = true;
    IFR_Bool requestpacket_isroot = requestPacket.isRoot();
    
    if(m_garbageparseids.GetSize() > IFR_CONNECTION_PARSEID_THRESHOLD) {
        if(appendFlag == AppendAllowed_C) {
            appendGarbageParseIDs(requestPacket, memory_ok);
        } else if(!requestpacket_isroot && appendFlag != StreamMode_C) {
            dropGarbageParseIDs(memory_ok);
        }
    }
    
    if( m_garbagelongs.GetSize() > IFR_CONNECTION_LONGDESC_THRESHOLD) {
        if(requestpacket_isroot && appendFlag == AppendAllowed_C) {
            appendGarbageLongDescriptors(requestPacket, memory_ok);
        } else if(appendFlag != StreamMode_C) {
            dropGarbageLongDescriptors(memory_ok);
        }
    } 

    if(!memory_ok) {
        execError.setMemoryAllocationFailed();
        // error() = execError;
        DBUG_RETURN(IFR_NOT_OK);
    }

    // we must look whether the packet is synthetic. Currently, we cannot use a synthetic packet
    // for doing an sqlaexecute, in future this will work
    if(!requestpacket_isroot) {
        IFRPacket_RequestPacket rootPacket(*this);
        IFR_Retcode rc = getRequestPacket(rootPacket, execError, IFRPacket_RequestPacket::Root_C);
        if(rc != IFR_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        rootPacket.copyInto(requestPacket);
        DBUG_RETURN(sqlaexecute(rootPacket, replyPacket, appendFlag, execError, operation));
    }

    tsp1_packet * rawRequest = requestPacket.GetRawPacket();

    SAPDB_Bool     success;
    tsp1_packet * rawReply = 0;
    tsp00_Int4    replyLen;
    SQLDBC_IRuntime::Error runtimeerror;

    // communicate synchronized on the connection3
    {
        runtime.lockMutex(m_sqlaexecute_lock);
        // For multi-threading, this is enough, anyone who wants the root packet 
        // will lock here. Single-Threaded stuff will jump nicely through this stuff
        m_sqlaexecute_executing = 1;
        
        // we put the trace into the synchronized range so that it is not shredded

        IFR_PACKET_TRACE << requestPacket << endl;

        IFR_CONNECTION_PROFILE_COUNT_8_ADD(IFR_SENDSIZE, requestPacket.Length ());    
        if(success = runtime.request(m_connectionid, rawRequest, requestPacket.Length (), runtimeerror)) {
            IFR_CONNECTION_PROFILE_COUNT_INC(IFR_SENDCOUNT);            
            m_cancellationpoint = true;
            if(operation && !operation->isExecuted()) {
                operation->execute();
            }
            success = runtime.receive(m_connectionid, (void**)&rawReply, replyLen, runtimeerror);
            m_cancellationpoint = false;
            IFR_CONNECTION_PROFILE_COUNT_8_ADD(IFR_RECEIVESIZE, replyLen+sizeof (tsp1_packet_header));            
        }

        m_sqlaexecute_executing = 0;
        runtime.releaseMutex(m_sqlaexecute_lock);
    }
    // if a communication error happened, we invalidate the reply packet, and
    // return
    if (!success) {
        IFR_Retcode rc = IFR_OK;
        requestPacket.invalidate();
        replyPacket.setPacket(0,0);

        // A communication error also means that the connection is broken.
        if(!m_connection_closing) {
            rc = close(false, true);
        }

        if(rc == IFR_OK) {
            execError.setFromRuntimeError(runtimeerror);
        }
        error().assign(execError, false); // already set to execError
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        replyPacket.setPacket(rawReply, requestPacket.getLock());
        IFR_PACKET_TRACE << replyPacket << endl;

        // spy into the packet and close the connection, if necessary
        {
            IFRPacket_ReplySegment error_segment(replyPacket);
            error_segment.getSQLError(execError, allocator, true);
            // If the error is one of the connection-releasing errors,
            // we have grabbed the error, and
            if(execError.isConnectionReleasing()) {
                if(!m_connection_closing) {
                    requestPacket.invalidate();
                    replyPacket.setPacket(0,0);
                    close(false, true);
                    error().assign(execError, false); // was cleared possibly by 'close'
                    DBUG_RETURN(IFR_NOT_OK);
                }
            } else if(execError.getErrorCode() == 100) {
                execError.clear();
            }

        }
        // error() = execError;
        DBUG_RETURN(IFR_OK);
    }
}

//----------------------------------------------------------------------
void IFR_Connection::nextCursorName (IFR_String& cursorname,
                                     IFR_Bool& memory_ok)
{
    m_uniqueid.getNextID(cursorname, memory_ok);
}

//----------------------------------------------------------------------
IFR_Connection*
IFR_Connection::getConnection() const
{
  return (IFR_Connection*) this;
}


//----------------------------------------------------------------------
IFR_DateTimeFormat::Format
IFR_Connection::getDateTimeFormat() const
{
    return m_datetimeformat;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Connection::isUnicodeDatabase() const
{
  DBUG_METHOD_ENTER(IFR_Connection, isUnicodeDatabase);
  DBUG_RETURN((m_connectionflags.unicode == 1)?true:false);
}

//----------------------------------------------------------------------
IFR_Int4
IFR_Connection::getKernelVersion() const
{
  DBUG_METHOD_ENTER(IFR_Connection, getKernelVersion);
  IFR_Int4 version;
  version = (m_kernelversion[4]-'0')
    + (m_kernelversion[3]-'0') * 10
    + (m_kernelversion[2]-'0') * 100
    + (m_kernelversion[1]-'0') * 1000
    + (m_kernelversion[0]-'0') * 10000;
    DBUG_RETURN(version);
}

//----------------------------------------------------------------------
void
IFR_Connection::setSessionInfos(IFRPacket_SessionInfoPart& sessioninfo)
{
    char *sessioninfo_data=sessioninfo.getData();
    m_datetimeformat=(IFR_DateTimeFormat::Format)sessioninfo_data[169];
    m_connectionflags.unicode = sessioninfo_data[0] ? 1 : 0;
    memcpy(&m_sql_connectionid, sessioninfo_data+1, sizeof(m_sql_connectionid));
    memcpy(m_kernelversion, sessioninfo_data+2200, 5);
    m_kernelversion[5]='\0';
}

//----------------------------------------------------------------------
void
IFR_Connection::evalFeaturePart(IFRPacket_FeaturePart& featurepart)
{
    DBUG_METHOD_ENTER(IFR_Connection, evalFeaturePart);
#if IFR_HAS_FEATUREPART
    IFR_Int2 featurecount = featurepart.getPartArguments();
    char    *data         = featurepart.getData();
    for(IFR_Int2 i=0; i<featurecount; ++i, data+=2) {
        switch(data[0]) {
        case IFRPacket_Feature::MultipleDropParseID_C:
            m_connectionflags.multipleparseiddrop = data[1]; 
            if(m_connectionflags.multipleparseiddrop) {
                IFR_SQL_TRACE << "MULTIPLE PARSE ID DROP ENABLED" << endl;
            } else {
                IFR_SQL_TRACE << "MULTIPLE PARSE ID DROP DISABLED" << endl;
            }
            break;
        case IFRPacket_Feature::SpaceOption_C:
            m_connectionflags.spaceoption = data[1];
            if(m_connectionflags.spaceoption) {
                IFR_SQL_TRACE << "SPACE OPTION ENABLED" << endl;
            } else {
                IFR_SQL_TRACE << "SPACE OPTION DISABLED" << endl;
            }
            break;
        case IFRPacket_Feature::VariableInput_C:
            m_connectionflags.variableinput = data[1];
            if(m_connectionflags.variableinput) {
                IFR_SQL_TRACE << "VARIABLE INPUT ENABLED" << endl;
            } else {
                IFR_SQL_TRACE << "VARIABLE INPUT DISABLED" << endl;
            }
            break;
        case IFRPacket_Feature::OptimizedStreams_C:
            m_connectionflags.optimizedstreams = data[1];
            if(m_connectionflags.optimizedstreams) {
                IFR_SQL_TRACE << "OPTIMIZED/COMPRESSED STREAMS ENABLED" << endl;
            } else {
                IFR_SQL_TRACE << "OPTIMIZED/COMPRESSED STREAMS DISABLED" << endl;
            }
            break;
        case IFRPacket_Feature::CheckScrollableOption_C: {
            m_connectionflags.scrollable = data[1];
            if(m_connectionflags.scrollable) {
                IFR_SQL_TRACE << "SCROLLABLE CURSORS ARE DISABLED" << endl;
            } else {
                IFR_SQL_TRACE << "SCROLLABLE CURSORS ARE ENABLE" << endl;
            }
        }
        default:
            // oops. what idea did the kernel had? anyway - we ignore what we
            // don't know ...
            ;
        }
    }
#endif
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::setTransactionIsolation(IFR_Int4 isolationlevel)
{
    DBUG_METHOD_ENTER(IFR_Connection, setIsolationLevel);
    DBUG_PRINT(isolationlevel);

    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(isolationlevel!=0 && isolationlevel!=1 && isolationlevel!=10
       && isolationlevel!=2 && isolationlevel!=20 && isolationlevel!=15
       && isolationlevel!=3 && isolationlevel!=30) {
        error().setRuntimeError(IFR_ERR_INVALID_ISOLATIONLEVEL_I, isolationlevel);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_SETISOLATIONCOUNT );
    IFR_Bool memory_ok = true;
    runtime.lockMutex(m_status_lock);
    if(m_isolationlevel==isolationlevel) {
        runtime.releaseMutex(m_status_lock);
        DBUG_RETURN(IFR_OK);
    } else {
        char statement_buf[23];
        sp77sprintf(statement_buf, 22, "SET ISOLATION LEVEL %d", isolationlevel);
        IFR_String statementstr(statement_buf,
                                IFR_StringEncodingAscii,
                                allocator,
                                memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
	    runtime.releaseMutex(m_status_lock);
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Retcode sqlrc=m_internalstatement->execute(statementstr);
        if(sqlrc!=IFR_OK) {
            error().assign(m_internalstatement->error(), true); // submit to connection
            runtime.releaseMutex(m_status_lock);
            DBUG_RETURN(sqlrc);
        }  else {
            m_isolationlevel=isolationlevel;
            runtime.releaseMutex(m_status_lock);
            DBUG_RETURN(IFR_OK);
        }
    }
}

//----------------------------------------------------------------------
IFR_Int4
IFR_Connection::getTransactionIsolation() const
{
    DBUG_METHOD_ENTER(IFR_Connection, getTransactionIsolation);
    DBUG_RETURN(m_isolationlevel);
}

//----------------------------------------------------------------------
static inline int clientapplication_valid(const char *application)
{
    const char * client_applications[] = {
        "CPC", "CPP", "DBM", "DBG", "GPC", "JDB", "KNL", "LOA", "ODB", "UTI", "XCI"
    };
    for(IFR_size_t i=0; i<sizeof(client_applications) / sizeof(char *); ++i) {
        if(strcmp(client_applications[i], application)==0) {
            return 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::setClientApplication(const char *clientapplication)
{
    DBUG_METHOD_ENTER(IFR_Connection, setClientApplication);
    DBUG_PRINT(clientapplication);
    clearError();
    IFR_SQL_TRACE << "::SET CLIENT APPLICATION TO " << (clientapplication?clientapplication:(const char *)"(null)") << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    if(clientapplication == 0) {
        error().setRuntimeError(IFR_ERR_INVALID_CLIENTAPPLICATION_S, "(null)");
        DBUG_RETURN(IFR_NOT_OK);
    } else if(!clientapplication_valid(clientapplication)) {
        error().setRuntimeError(IFR_ERR_INVALID_CLIENTAPPLICATION_S, clientapplication);
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        strcpy(m_client_application, clientapplication);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
const char *
IFR_Connection::getClientApplication() const
{
    return m_client_application;
}

//----------------------------------------------------------------------
void
IFR_Connection::dropParseID(IFR_ParseID& parseid, IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, dropParseID);
    runtime.lockMutex(m_status_lock);
    if(m_accumulate_parseids) {
        m_garbageparseids.InsertEnd(parseid, memory_ok);
    } else {
        // old implementation, which drops directly
        IFRPacket_RequestPacket requestpacket(*this);
        IFR_Retcode commandrc;
        commandrc=getRequestPacket(requestpacket, error());
        while(commandrc==IFR_OK && requestpacket.isValid()) {
            IFRPacket_RequestSegment segment(requestpacket, IFRPacket_CommandMessageType::Dbs_C);
            IFRPacket_CommandPart command;
            commandrc = segment.addPart(command);
            if(commandrc != IFR_OK) {
                break;
            }

            IFR_String dropparseid_stm("DROP PARSEID", IFR_StringEncodingAscii, allocator, memory_ok);
            commandrc=command.setText(dropparseid_stm, error());
            if(commandrc != IFR_OK) {
                break;
            }
            segment.closePart();
            IFRPacket_ParseIDPart parseidpart;
            commandrc = segment.addPart(parseidpart);
            if(commandrc != IFR_OK) {
                break;
            }
            commandrc = parseidpart.addParseID(parseid);
            if(commandrc != IFR_OK) {
                break;
            }
            segment.closePart();
            segment.close();
            IFRPacket_ReplyPacket replypacket;
            IFR_Retcode rc = sqlaexecute(requestpacket, replypacket, AppendNotAllowed_C, error());
            break;
        }
        clearError();
    }
    runtime.releaseMutex(m_status_lock);
}

//----------------------------------------------------------------------
void
IFR_Connection::dropCursor(IFR_String& cursorname, IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, dropCursor);
    DBUG_PRINT(cursorname);

    // If the session is not connected, bail out immediately.
    if(!isConnected()) {
        return;
    }

    IFRPacket_RequestPacket requestpacket(*this);
    IFR_Retcode commandrc;
    IFR_String close_prefix("CLOSE \"", IFR_StringEncodingAscii, allocator, memory_ok);
    IFR_String close_suffix("\"", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        return;
    }
    commandrc=getRequestPacket(requestpacket, error());
    while(commandrc==IFR_OK && requestpacket.isValid()) {
        IFRPacket_RequestSegment segment(requestpacket, IFRPacket_CommandMessageType::Dbs_C);
        IFRPacket_CommandPart command;
        commandrc = segment.addPart(command);
        if(commandrc != IFR_OK) {
            break;
        }
        commandrc=command.setText(close_prefix, error());
        if(commandrc != IFR_OK) {
            break;
        }
        commandrc=command.setText(cursorname, error());
        if(commandrc != IFR_OK) {
            break;
        }
        commandrc=command.setText(close_suffix, error());
        if(commandrc != IFR_OK) {
            break;
        }
        IFR_CONNECTION_PROFILE_COUNT_INC( IFR_DROPCURSORCOUNT );
        segment.closePart();
        segment.close();
        IFRPacket_ReplyPacket replypacket;
        sqlaexecute(requestpacket, replypacket, AppendNotAllowed_C, error());
        break;
    }
    clearError();
    return;

}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::release()
{
    DBUG_METHOD_ENTER(IFR_Connection, release);

    SQLDBC_IRuntime::Error ignored;
    IFR_Int8               connectionid=m_connectionid;

    m_connectionid = -1;
    DBUG_RETURN( runtime.releaseSession(connectionid, ignored)
                 ? IFR_OK
                 : IFR_NOT_OK );
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Connection::cancel()
{
    DBUG_METHOD_ENTER(IFR_Connection, cancel);
    DBUG_PRINT(m_connectionid);
    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::CANCEL " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    IFR_SQL_TRACE <<     "SESSION ID : " << m_connectionid << endl;
    //<<< SQL TRACE ENTRY
    if(!isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_CONNECTION_PROFILE_COUNT_INC( IFR_CANCELCOUNT );
    runtime.lockMutex(m_status_lock);
    if(!m_cancellationpoint) {
        runtime.releaseMutex(m_status_lock);
        IFR_SQL_TRACE << "RETURN     : " << (int)IFR_NO_DATA_FOUND << endl
                      << "MESSAGE    : No active command found." << endl;
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    } else {
        SQLDBC_IRuntime::Error cancelerror;
        IFR_Bool cancelled=runtime
            .cancelCurrentCommand(m_connectionid,
                                  cancelerror);
        runtime.releaseMutex(m_status_lock);
        if(cancelled) {
            DBUG_RETURN(IFR_OK);
        } else {
            if(cancelerror) {
                error().setFromRuntimeError(cancelerror);
                DBUG_RETURN(IFR_NOT_OK);
            } else {
              IFR_SQL_TRACE << "RETURN     : " << (int)IFR_NO_DATA_FOUND << endl
                            << "MESSAGE    : No active command found." << endl;
              DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
        }
    }
}

//----------------------------------------------------------------------
void
IFR_Connection::appendGarbageParseIDs(IFRPacket_RequestPacket& requestPacket,
                                      IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, appendGarbageParseIDs);
    if(!memory_ok) {
        return;
    }
    runtime.lockMutex(m_status_lock);
    IFR_Int2 numberOfSegments=requestPacket.getNumberOfSegments();
    IFR_StringEncoding packetencoding = requestPacket.getEncoding();
    IFR_Int4 single_reserve = packetencoding == IFR_StringEncodingAscii ? 256 : 512;
    IFR_Int4 replyReserve = (numberOfSegments+1) * single_reserve + ( (numberOfSegments >= 1) ? 8192 - single_reserve : 0);

    IFR_String dropparseid_stm("DROP PARSEID", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        return;
    }

    IFR_Int4 oneRequestSize = sizeof(tsp1_segment_header)
         + sizeof(tsp1_part_header)
         + packetencoding == IFR_StringEncodingAscii ? 12 : 24 // 'DROP PARSEID'
         + sizeof(tsp1_part_header)
         + IFR_ParseID_Size;

    if(m_connectionflags.multipleparseiddrop) {
        if(requestPacket.remainingBytes() > replyReserve + oneRequestSize &&
            m_garbageparseids.GetSize()) {
            IFRPacket_RequestSegment segment(requestPacket, IFRPacket_CommandMessageType::Dbs_C, false);
            IFRPacket_CommandPart command;
            segment.addPart(command);
            command.setText(dropparseid_stm, error());
            segment.closePart();
            IFRPacket_ParseIDPart parseid;
            segment.addPart(parseid);
            IFR_Int4 maxparseidfill = (parseid.getRemainingBytes())/ IFR_ParseID_Size;
            while(m_garbageparseids.GetSize() > 0 && maxparseidfill > 0) {
                parseid.addParseID(*(m_garbageparseids.Last()));
                IFR_CONNECTION_PROFILE_COUNT_INC( IFR_DROPPARSEIDCOUNT );
                m_garbageparseids.DeleteEnd();
                --maxparseidfill;
            }
            segment.closePart();
            segment.close();
        }
    } else {
        while(requestPacket.remainingBytes() >= replyReserve + oneRequestSize
            && m_garbageparseids.GetSize() > 0) {
            ++numberOfSegments;
            replyReserve += single_reserve;

            IFRPacket_RequestSegment segment(requestPacket, IFRPacket_CommandMessageType::Dbs_C, false);

            IFRPacket_CommandPart command;
            segment.addPart(command);
            command.setText(dropparseid_stm, error());
            segment.closePart();
            IFRPacket_ParseIDPart parseid;
            segment.addPart(parseid);
            IFR_ParseID *pid = &( *(m_garbageparseids.Last()));
            parseid.addParseID(*pid);
            IFR_CONNECTION_PROFILE_COUNT_INC( IFR_DROPPARSEIDCOUNT );
            m_garbageparseids.DeleteEnd();
            segment.closePart();
            segment.close();
        }
    }
    runtime.releaseMutex(m_status_lock);
}



//----------------------------------------------------------------------
void
IFR_Connection::dropLongDescriptor(IFRPacket_LongDescriptor& longdesc, 
                                   IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, dropLongDescriptor);
    if(memory_ok) {
        runtime.lockMutex(m_status_lock);
        m_garbagelongs.InsertEnd(longdesc, memory_ok);
        runtime.releaseMutex(m_status_lock);
    }
    return;
}

//----------------------------------------------------------------------
void
IFR_Connection::freeRequestPacket(void *packetPointer)
{
    runtime.lockMutex(m_packetcache_lock);
    IFR_Connection_CachedPacket *p = (IFR_Connection_CachedPacket*) packetPointer;
    if(m_packetcache == 0) {
        p->nextPacket = 0;
        p->size       = 1;
    } else {
        p->nextPacket = (IFR_Connection_CachedPacket*)m_packetcache;
        p->size = ((IFR_Connection_CachedPacket*)m_packetcache)->size + 1;
    }
    m_packetcache = p;
    runtime.releaseMutex(m_packetcache_lock);
}

//----------------------------------------------------------------------
void
IFR_Connection::appendGarbageLongDescriptors(IFRPacket_RequestPacket& requestPacket, IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, appendGarbageLongDescriptors);
    if(!memory_ok) {
        return;
    }

    runtime.lockMutex(m_status_lock);
    IFR_Int4 garbagecount = m_garbagelongs.GetSize();
    if(garbagecount == 0) {
        runtime.releaseMutex(m_status_lock);
        return;
    }

    IFR_Int2 numberOfSegments=requestPacket.getNumberOfSegments();
    IFR_StringEncoding packetencoding = requestPacket.getEncoding();
    IFR_Int4 single_reserve = packetencoding == IFR_StringEncodingAscii ? 256 : 512;
    IFR_Int4 replyReserve = (numberOfSegments+1) * single_reserve + ( (numberOfSegments >= 1) ? 8192 - single_reserve : 0);
    
    IFR_Int4 oneRequestSize = sizeof(tsp1_segment_header)
        + sizeof(tsp1_part_header);
    if(requestPacket.remainingBytes() > replyReserve + oneRequestSize
       && m_garbagelongs.GetSize()) {
        IFRPacket_RequestSegment close_segment(requestPacket, IFRPacket_CommandMessageType::Getval_C, false);
        IFRPacket_LongDataPart longdatapart;
        close_segment.addPart(longdatapart);
        IFR_Int4 maxlongdescfill = longdatapart.getRemainingBytes() / sizeof(IFRPacket_LongDescriptor);
        IFR_Int4 argcount=0;
        while(m_garbagelongs.GetSize() > 1 && maxlongdescfill > 0) {
            longdatapart.addDescriptor(*(m_garbagelongs.Last()));
            ++argcount;
            --maxlongdescfill;
            m_garbagelongs.DeleteEnd();
        }
        longdatapart.setArgCount(argcount);
        close_segment.closePart();
        close_segment.close();
    }
    return;
}

//----------------------------------------------------------------------
void
IFR_Connection::dropGarbageLongDescriptors(IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, dropGarbageLongDescriptors);
    
    if(!memory_ok) {
        return;
    }

    // if we are not connected, we can simply forget remaining LONG
    // descriptors that haven't been closed yet.
    if(m_connectionid < 0) {
        m_garbagelongs.Clear();
        return;
    }

    runtime.lockMutex(m_status_lock);
    IFR_Int4 garbagecount = m_garbagelongs.GetSize();
    if(garbagecount == 0) {
        runtime.releaseMutex(m_status_lock);
        return;
    }
    IFRPacket_RequestPacket requestpacket(*this);
    IFR_Retcode rc = getRequestPacket(requestpacket, error());
    if(rc != IFR_OK) {
        runtime.releaseMutex(m_status_lock);
        return;
    }

    IFR_Int1 last_sqlmode = requestpacket.switchSqlMode(IFR_INTERNAL);
    IFRPacket_RequestSegment close_segment(requestpacket, IFRPacket_CommandMessageType::Getval_C);
    IFRPacket_LongDataPart longdatapart;
    if(close_segment.isValid()) {
        if(close_segment.addPart(longdatapart) != IFR_OK) {
            runtime.releaseMutex(m_status_lock);
            requestpacket.switchSqlMode(last_sqlmode);
            return;
        }
    } else {
        runtime.releaseMutex(m_status_lock);
        requestpacket.switchSqlMode(last_sqlmode);
        return;
    }
    for(IFR_Int4 i=0; i<garbagecount; ++i) {
        longdatapart.addDescriptor(m_garbagelongs[i]);
    }
    // Argument count is what we have done - the number of items in the vector.
    IFR_CONNECTION_PROFILE_COUNT_ADD(IFR_DROPLONGDESCCOUNT, garbagecount);
    longdatapart.setArgCount(garbagecount);
    m_garbagelongs.Clear();
    runtime.releaseMutex(m_status_lock);
    close_segment.closePart();
    close_segment.close();
    requestpacket.switchSqlMode(last_sqlmode);
    IFRPacket_ReplyPacket replypacket;
    rc=sqlaexecute(requestpacket, replypacket, AppendAllowed_C, error());
    clearError();
    return;
}

//----------------------------------------------------------------------
void
IFR_Connection::dropGarbageParseIDs(IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_Connection, dropGarbageParseIDs);
    if(!memory_ok) {
        return;
    }
    IFRPacket_RequestPacket requestPacket(*this);
    IFR_Retcode rc=IFR_OK;
    do {
        runtime.lockMutex(m_status_lock);
        IFR_Int4 garbagecount=m_garbageparseids.GetSize();
        if(garbagecount == 0) {
            runtime.releaseMutex(m_status_lock);
            break;
        } else {
            runtime.releaseMutex(m_status_lock);
        }
        {
            rc = getRequestPacket(requestPacket, error());
            if(rc != IFR_OK) {
                return;
            }
            IFRPacket_RequestSegment segment(requestPacket, IFRPacket_CommandMessageType::Dbs_C, false);
            if(!segment.isValid()) {
                rc=IFR_NOT_OK;
                break;
            }
            IFRPacket_CommandPart command;
            rc=segment.addPart(command);
            if(rc!=IFR_OK) {
                break;
            }
            IFR_String dropparseid_stm("DROP PARSEID",
                        IFR_StringEncodingAscii,
                        allocator,
                        memory_ok);
            //<<<
            if(!memory_ok) {
                return;
            }
            rc = command.setText(dropparseid_stm, error());
            if(rc!=IFR_OK) {
                break;
            }

            segment.closePart();
            IFRPacket_ParseIDPart parseid;
            rc = segment.addPart(parseid);
            if(rc!=IFR_OK) {
                break;
            }
            rc = parseid.addParseID(*(m_garbageparseids.Last()));
            if(rc!=IFR_OK) {
                break;
            }
            m_garbageparseids.DeleteEnd();
            if(m_connectionflags.multipleparseiddrop) {
                IFR_Int4 numparseids = parseid.getRemainingBytes() / IFR_ParseID_Size;
                while(numparseids >0 && m_garbageparseids.GetSize()>0) {
                    parseid.addParseID(*(m_garbageparseids.Last()));
                    m_garbageparseids.DeleteEnd();
                }
            }

            segment.closePart();
            segment.close();
            IFRPacket_ReplyPacket replypacket;
            // the appendGarbageParseIDs method will add the other parse ids.
            rc=sqlaexecute(requestPacket, replypacket, AppendAllowed_C, error());
        } // The inner block is necessary for correct initialisation of
          // the request packet
    } while(rc==IFR_OK);
    clearError();
    return;
}

//----------------------------------------------------------------------
IFR_Int4
IFR_Connection::getMinimumLongFetchSize() const
{
    DBUG_METHOD_ENTER(IFR_Connection, getMinimumLongFetchSize);
    DBUG_RETURN(m_mingetvalsize);
}

//----------------------------------------------------------------------
IFRConversion_Converter *
IFR_Connection::findSpecialParameter(IFR_Int4 tag)
{
    switch(tag) {
    case SQLDBC_FIRST_INSERTED_SERIAL:
        return m_specialparameters[0];
    case SQLDBC_LAST_INSERTED_SERIAL:
        return m_specialparameters[1];
    }
    return 0;
}

//----------------------------------------------------------------------
void
IFR_Connection::setupConnectionData(IFR_String& connectCommand, IFR_Bool& memory_ok)
{
    if(memory_ok) {
        m_connectcommand.assign(connectCommand, memory_ok);
        if(!memory_ok) {
            return;
        }
        m_internalstatement = createStatement();
        if(m_internalstatement == 0) {
            memory_ok = false;
            return;
        }
        IFR_ShortInfo si;
        si.mode = 0;
        si.iotype = 0;
        si.datatype = IFR_SQLTYPE_FLOAT;
        si.frac   = 0;
        si.length = 38;
        si.iolength = 21;
        si.pos.bufpos = 1;
        m_specialparameters[0] = IFRConversion_Factory::createInstance(&si, 0, *this, false, error());
        si.pos.bufpos = 22;
        m_specialparameters[1] = IFRConversion_Factory::createInstance(&si, 0, *this, false, error());
        if( (m_specialparameters[0] == 0) || (m_specialparameters[1] == 0) ) {
            memory_ok = false;
            return;
        }
    }
}


//======================================================================
// IFR_ConnectionProfile
//======================================================================
/*
const IFR_ProfileCountValues IFR_ConnectionProfile::m_counter_8_map[] =  {

IFR_SENDSIZE               ,
IFR_RECEIVESIZE            ,
IFR_DATAREADSIZE           ,
IFR_DATAWRITESIZE          ,
IFR_LONGDATAREADSIZE       ,
IFR_LONGDATAWRITESIZE      ,
};

const IFR_ProfileCountValues IFR_ConnectionProfile::m_counter_4_map[] =  {
IFR_CONNECTIONCOUNT        ,
IFR_COMMITCOUNT            ,
IFR_ROLLBACKCOUNT          ,
IFR_SETISOLATIONCOUNT      ,
IFR_SENDCOUNT              ,
IFR_CANCELCOUNT            ,
IFR_STATEMENTCOUNT         ,
IFR_PREPAREDSTATEMENTCOUNT ,
IFR_NOTSELECTFETCHOPTIMIZED,
IFR_DROPPARSEIDCOUNT       ,
IFR_DROPCURSORCOUNT        ,
IFR_DROPLONGDESCCOUNT      ,
IFR_PREPARECOUNT           ,
IFR_PARSEINFOHITCOUNT      ,
IFR_PARSINFOMISSCOUNT      ,
IFR_PARSEAGAINCOUNT        ,
IFR_PARSEINFOSQUEEZECOUNT  ,
IFR_EXECUTECOUNT           ,
IFR_INSERTCOUNT            ,
IFR_UPDATECOUNT            ,
IFR_SELECTCOUNT            ,
IFR_DELETECOUNT            ,
IFR_FETCHCOUNT             ,
IFR_SQLERRORCOUNT          ,
IFR_ROWNOTFOUNDCOUNT       ,
IFR_FETCHROWCOUNT          ,
};
*/
//----------------------------------------------------------------------
void IFR_ConnectionProfile::submitCounters(IFR_UInt8 *counter_8, IFR_UInt4 *counter_4){
    IFR_Connection* conn = static_cast <IFR_Connection*> (this);
    m_counter_4[IFR_Environment::IFR_SQLERRORCOUNT_ENV] += conn->error().getErrorCount();
    m_counter_4[IFR_Environment::IFR_ROWNOTFOUNDCOUNT_ENV] += conn->error().getRowNotFoundCount();
    for(IFR_Length i = 0; i<sizeof(m_counter_8)/sizeof(IFR_UInt8); ++i) {
        counter_8[i] += m_counter_8[i];
    }
    for(IFR_Length j = 0; j<sizeof(m_counter_4)/sizeof(IFR_UInt4); ++j) {
        counter_4[j] += m_counter_4[j];
    }
//     memcpy(counter_8, m_counter_8, sizeof(m_counter_8));
//     memcpy(counter_4, m_counter_4, sizeof(m_counter_4));
    this->resetCounters();
}

//----------------------------------------------------------------------
IFR_ConnectionProfile::IFR_ConnectionProfile(IFRUtil_LinkedList<SQLDBC::SQLDBC_StatementStorage>* list)
:m_StmtList(list)
{
  this->resetCounters();
}
//----------------------------------------------------------------------
void IFR_ConnectionProfile::collectCounters() {
  SQLDBC::SQLDBC_StatementStorage * profile = static_cast<SQLDBC::SQLDBC_StatementStorage*>  (this->m_StmtList->First());
   if (!profile) return;
   do {
//     profile->collectCounters();
//  
     profile->getStatement()->submitCounters(this->m_counter_8, this->m_counter_4);
   } while (0 != (profile = static_cast<SQLDBC::SQLDBC_StatementStorage*>  (this->m_StmtList->Next(*profile))));
}
//----------------------------------------------------------------------

void IFR_ConnectionProfile::resetCounters(){
      memset(&m_counter_8[0], 0, sizeof(m_counter_8));
      memset(&m_counter_4[0], 0, sizeof(m_counter_4));
}


//----------------------------------------------------------------------
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_Connection& connection )
{
  s << "conn " << connection.m_connectionid;
  {
    char tmp[sizeof(connection.m_sql_connectionid)];
    memcpy(tmp, &connection.m_sql_connectionid, sizeof(connection.m_sql_connectionid));
    s << " sess " << hex << inputlength(sizeof(connection.m_sql_connectionid))
      << tmp;
  }
  switch (connection.m_client_sqlmode) {
  case (IFR_INTERNAL): {
    break;
  }
  case (IFR_ORACLE): {
    s << " oracle"; break;
  }
  case (IFR_ANSI): {
    s << " ansi"; break;
  }
  case (IFR_DB2): {
    s << " db2"; break;
  }
  case (IFR_SAPR3): {
      s << "sapr3"; break;
  }
  default: {
    s << " unknown(" << connection.m_client_sqlmode << ")"; break;
  }
  }
  if (connection.m_connectionflags.clientunicode)
    s << " unicode";
  if (connection.m_connectionflags.autocommit == 1)
    s << " autocommit";
  return s;
}

IFR_END_NAMESPACE
