/*!========================================================================

  @file         RTEComm_URIBuilder.cpp
  @ingroup      Runtime
  @author       StefanP

  @brief        Build a liveCache Uniform Resource Identifier (URI)

                The liveCache Uniform Resource Identifier (URI) is a compact
                string of characters used to identify liveCache specific 
                resources.
                The following class is used to build up such an URI from given
                URI parts.

  @since        2005-01-10  12:24
  @sa           

  ==========================================================================

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "geo401.h"
#include    "geo43_0.h"
#include    "RunTime/Communication/RTEComm_URIBuilder.hpp"
#include    "SAPDBCommon/SAPDB_string.h"
#include    "SAPDBCommon/Messages/SDBMsg_RTEComm.h"

#if defined (URI_BUILDER_TEST)
#include "gos00.h" /*nocheck*/ 
#include "geo80.h" /*nocheck*/
#include "geo81.h" /*nocheck*/
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define URI_SEPARATOR               "/"     ///<separates the option part of a URI from the path part (cf. URI specification)
#define PORT_SEPARATOR              ":"     ///<separates the port from the host (cf. URI specification)
#define PROTOCOL_SEPARATOR          "://"   ///<separates the protocol from the location (cf. URI specification)
#define SAPNI_PROTOCOL_SEPARATOR    ":"     ///<separates the SAPNI protocol from the location (cf. URI specification)
#define HOST_DEFAULT_LOCALHOST      "."     ///<location for the local communication (cf. URI specification)

#define OPTION_PART_SEPARATOR       "?"	    ///<separates the option part of a URI from the path part (cf. URI specification) 
#define OPTION_SEPARATOR            "&"	    ///<separates the option inside the option part of a URI
#define OPTION_ASSIGNMENT           "="	    ///<assigns a value to an option


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/
#if defined (URI_BUILDER_TEST)
class URIBuilderArguments : public teo81_EvalProgArgs
{
public:
  URIBuilderArguments                               ( int argc, char** argv );
  ~URIBuilderArguments                              () {;};

  SAPDB_Char *    GetHostname         ()
    {return m_ServerNode;};

  SAPDB_Char *    GetPort             ()
    {return m_Port;};

  SAPDB_Char *    GetServerDB         ()
    {return m_ServerDB;};

  SAPDB_Char *    GetPath             ()
    {return m_Path;};

  SAPDB_Bool    GetEncryption         ()
    {return m_Encryption;};

private:
  bool          eo80_vNextValidOption         (const int            OptionIndex,
                                               const char*          OptionArgument );

 teo81_OptionDesc *   m_OptionDesc;
 unsigned long        m_NumOfOptionDesc;
 SAPDB_Char *         m_ServerNode;
 SAPDB_Char *         m_Port;
 SAPDB_Char *         m_Path;
 SAPDB_Char *         m_ServerDB;
 SAPDB_Bool           m_Encryption;
};


#define OPTION_HOST                     1
#define OPTION_PORT                     2
#define OPTION_SERVERDB                 3
#define OPTION_PATH                     4
#define OPTION_ENCRYPTION               5
#define OPTION_HELP                     20


static teo81_EvalProgArgs::teo81_OptionDesc Options[] = 
       {{ OPTION_HOST       , _T('n'), _T("node")       , true, true, _T("\"<Server node>\"")  , _T("Server node or SAPRouter string") },
        { OPTION_PORT       , _T('p'), _T("port")       , true, true, _T("\"<Port number>\"")  , _T("Port number") },
        { OPTION_SERVERDB   , _T('d'), _T("database")   , true, true, _T("\"<Database name>\""), _T("Database name") },
        { OPTION_PATH       , _T('w'), _T("path")       , true, true, _T("\"<URI path>\""), _T("URI path: DBM, DB or LISTENER") },
        { OPTION_ENCRYPTION , _T('e'), _T("encryption") , false, true, _T("\"<Encryption>\"")   , _T("Encryption") },
        { OPTION_HELP       , _T('h'), _T("help")       , true, true,  _T("[<Option> | LONG]")  , _T("help") },
        { OPTION_HELP       , _T('?'), _T("?")          , true, true,  _T("[<Option> | LONG]")  , _T("help") },
};

#endif

/*===========================================================================*
*  STATIC/INLINE FUNCTION PROTOTYPES                                        *
*===========================================================================*/
//RTE_Nodename    RTEComm_URIBuilder::m_LocalNodeName = "";


/*===========================================================================*
*  METHODS                                                                  *
*===========================================================================*/
RTEComm_URIBuilder::URIBuildRC 
RTEComm_URIBuilder::BuildDatabaseURI (SAPDB_UTF8 const * const  serverDB,
                                      SAPDB_Char const * const  host,
                                      SAPDB_Char const * const  port,
                                      Msg_List &                msgList,
                                      SAPDB_Bool                encryptionWanted)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::BuildDatabaseURI", 
                Communication_Trace, 9);

    SAPDB_Char const * const    protocol = GetProtocol (host, encryptionWanted, msgList);
    SAPDB_UInt4                 lenServerDB = 0;


    if (NULL == protocol)   
    {
        msgList.Overrule (Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST,
            Msg_Arg("HOSTNAME", host),
            Msg_Arg("BOOL", SAPDB_ToString (encryptionWanted))
            ));
        return Error;
    }

    if (NULL == serverDB || '\0' == *serverDB)
    {
        msgList = Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_MISSING_SERVERDB
            );
        return Error;
    }

    lenServerDB = EscapeURIPart (PathPart, serverDB) - 1;
    if (0 == lenServerDB)
    {
        msgList = Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_ESCAPE_DATABASE,
            Msg_Arg("DATABASE_NAME", (SAPDB_Char *)serverDB)
            );
        return Error;
    }

    return (BuildURI (PATH_DATABASE, protocol, serverDB, lenServerDB, host, port, encryptionWanted, msgList));
}

/*---------------------------------------------------------------------------*/

RTEComm_URIBuilder::URIBuildRC 
RTEComm_URIBuilder::BuildListenerURI (SAPDB_Char const * const  host,
                                      SAPDB_Char const * const  port,
                                      Msg_List &                msgList,
                                      SAPDB_Bool                encryptionWanted)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::BuildListenerURI", 
                Communication_Trace, 9);

    SAPDB_Char const * const    protocol = GetProtocol (host, encryptionWanted, msgList);


    if (NULL == protocol)   
    {
        msgList.Overrule (Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST,
            Msg_Arg("HOSTNAME", host),
            Msg_Arg("BOOL", SAPDB_ToString (encryptionWanted))
            ));
        return Error;
    }

    if (0 == strcmp (protocol, PROTOCOL_LOCAL))   
    {
        msgList.Overrule (Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_ILLEGAL_PROTOCOL,
            Msg_Arg("URI_PROTOCOL", PROTOCOL_LOCAL),
            Msg_Arg("URI_PATH", PATH_LISTENER)
            ));
        return WrongProtocol;
    }

    return (BuildURI (PATH_LISTENER, protocol, NULL, 0, host, port, encryptionWanted, msgList));
}
//-------------------------------------------------------------------

RTEComm_URIBuilder::URIBuildRC 
RTEComm_URIBuilder::BuildDBMURI   (SAPDB_UTF8 const * const  serverDB,
                                   SAPDB_Char const * const  host,
                                   SAPDB_Char const * const  port,
                                   Msg_List &                msgList,
                                   SAPDB_Bool                encryptionWanted)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::BuildDBMURI", 
                Communication_Trace, 9);

    SAPDB_Char const * const    protocol = GetProtocol (host, encryptionWanted, msgList);
    SAPDB_UInt4                 lenServerDB = 0;


    if (NULL == protocol)   
    {
        msgList.Overrule (Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_UNKNOWN_PROTOCOL_FROM_HOST,
            Msg_Arg("HOSTNAME", host),
            Msg_Arg("BOOL", SAPDB_ToString (encryptionWanted))
            ));
        return Error;
    }


    if (NULL != serverDB && '\0' != *serverDB)
    {
        lenServerDB = EscapeURIPart (PathPart, serverDB) - 1;
        if (0 == lenServerDB)
        {
            msgList = Msg_List(
                Msg_List::Error,
                SDBMSG_RTECOMM_URI_ESCAPE_DATABASE,
                Msg_Arg("DATABASE_NAME", (SAPDB_Char *)serverDB)
                );
            return Error;
        }
    }
    return (BuildURI (PATH_DBMSERVER, protocol, serverDB, lenServerDB, host, port, encryptionWanted, msgList));
}

/*---------------------------------------------------------------------------*/

RTEComm_URIBuilder::URIBuildRC 
RTEComm_URIBuilder::BuildURI (SAPDB_Char const * const  path,
                              SAPDB_Char const * const  protocol,
                              SAPDB_UTF8 const * const  serverDB,
                              SAPDB_UInt4               lenServerDB,
                              SAPDB_Char const * const  host,
                              SAPDB_Char const * const  port,
                              SAPDB_Bool                encryptionWanted,
                              Msg_List &                msgList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::BuildURI", 
                Communication_Trace, 9);

    SAPDB_ULong                 neededURILength = 0;
    SAPDB_Bool                  isSAPRouter = false;
    SAPDB_Bool                  isLocal = false;

    if (0 == strcmp (protocol, PROTOCOL_SAPNI) || 
        0 == strcmp (protocol, PROTOCOL_SAPNISSL))
    {
        isSAPRouter = true;
    }
    else if (0 == strcmp (protocol, PROTOCOL_LOCAL))
    {
        if (NULL != port)
        {
            msgList = Msg_List(
                Msg_List::Warning,
                SDBMSG_RTECOMM_URI_PORT_NOT_SUPPORTED
                );
            return NeedlessPort;
        }
        else if (encryptionWanted)
        {
            msgList = Msg_List(
                Msg_List::Warning,
                SDBMSG_RTECOMM_URI_ENCRYPTION_NOT_SUPPORTED
                );
            return NeedlessEncryption;
        }

        isLocal = true;
    }

    neededURILength  = strlen (protocol) + strlen (isSAPRouter ? SAPNI_PROTOCOL_SEPARATOR : PROTOCOL_SEPARATOR) + 
                       strlen (NULL != host  && !isLocal ? host : HOST_DEFAULT_LOCALHOST) +  
                       (NULL != port && !isSAPRouter ? 1 + strlen (port) : 0) +
                       (isSAPRouter ? strlen (PROTOCOL_SAPNI_END) : 0) + 1 +
                       strlen (path) +
                       (0 == strcmp (path, PATH_DBMSERVER) ? 1 + strlen (PATH_DATABASE) : 0) +
                       (NULL != serverDB ? 1 + lenServerDB : 0);


    //If the instance was constructed with a given buffer we can not reallocate memory if the buffer is to small!
    if (0 != m_MaxLengthURI && neededURILength > m_MaxLengthURI)
    {
        msgList = Msg_List(
            Msg_List::Warning,
            SDBMSG_RTECOMM_URI_BUFFER_TO_SMALL,
            Msg_Arg("BUFFERSIZE_NEEDED",SAPDB_ToString (neededURILength + 1)),
            Msg_Arg("BUFFERSIZE",SAPDB_ToString (m_MaxLengthURI + 1))
            );
        return URIBufferToSmall;
    }

    if (NULL == m_URI || (0 == m_MaxLengthURI && m_LengthURI < neededURILength))
    {
        CleanUp ();
        //Alloc memory for URI
        m_URI = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (neededURILength + 1));
        if (NULL == m_URI)
        {
            msgList = Msg_List(
                Msg_List::Error,
                SDBMSG_RTECOMM_URI_MEMORY_ALLOCATION,
                Msg_Arg("ALLOC_SIZE",SAPDB_ToString (neededURILength + 1))
                );
            return Error;
        }
        
        m_UseOwnMemory = true;
    }

    memset (m_URI, '\0', 0 != m_MaxLengthURI ? m_MaxLengthURI + 1 : neededURILength + 1);
    strcpy (m_URI, protocol);
    if (isSAPRouter)
    {
        strcat (m_URI, SAPNI_PROTOCOL_SEPARATOR);
    }
    else
    {
        strcat (m_URI, PROTOCOL_SEPARATOR);
    }

    NULL != host && !isLocal ? strcat (m_URI, host) : strcat (m_URI, HOST_DEFAULT_LOCALHOST);

    if (NULL != port && !isSAPRouter)
    { 
        strcat (m_URI, PORT_SEPARATOR);
        strcat (m_URI, port);
    }

    if (isSAPRouter)
    {
        strcat (m_URI, PROTOCOL_SAPNI_END);
    }

    strcat (m_URI, URI_SEPARATOR);
    strcat (m_URI, path);

    if (0 == strcmp (path, PATH_DBMSERVER) && NULL != serverDB)
    {
        strcat (m_URI, URI_SEPARATOR);
        strcat (m_URI, PATH_DATABASE);
    }

    if (NULL != serverDB)
    {
        strcat (m_URI, URI_SEPARATOR);
        EscapeURIPart (PathPart, serverDB, lenServerDB + 1, m_URI + strlen (m_URI));
    }

    m_LengthURI = neededURILength;

    return NoError;
}

/*---------------------------------------------------------------------------*/

void  RTEComm_URIBuilder::AddSeparator ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::AddSeparator", Communication_Trace, 9);

    if (m_FirstOption)
    {
        m_FirstOption = false;
        if (NULL == strstr (m_URI, OPTION_PART_SEPARATOR))
        {
            strcat (m_URI, OPTION_PART_SEPARATOR);
            ++m_LengthURI;
            return;
        }
    }

    strcat (m_URI, OPTION_SEPARATOR);
    ++m_LengthURI;
    return;
}

/*---------------------------------------------------------------------------*/

RTEComm_URIBuilder::URIBuildRC  
RTEComm_URIBuilder::Append (SAPDB_UTF8 const * const       optionName,
                            SAPDB_UTF8 const * const       optionValue,
                            Msg_List &                     msgList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::Append", Communication_Trace, 9);

    SAPDB_UInt4     lenOptionName = 0;
    SAPDB_UInt4     lenOptionValue = 0;
    SAPDB_UInt4     neededURILength = 0;

    if (NULL == optionName || '\0' == *optionName)
    {
        msgList = Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_MISSING_OPTION
            );
        return Error;
    }

    lenOptionName = EscapeURIPart (QueryContentPart, optionName);
    if (1 >= lenOptionName)
    {
        msgList = Msg_List(
            Msg_List::Error,
            SDBMSG_RTECOMM_URI_ESCAPE_OPTION,
            Msg_Arg("URI_OPTION_NAME", (SAPDB_Char *) optionName)
            );
        return Error;
    }

    --lenOptionName;

    if (NULL != optionValue && '\0' != *optionValue)
    {
        lenOptionValue = EscapeURIPart (QueryContentPart, optionValue);
        if (1 >= lenOptionValue)
        {
            msgList = Msg_List(
                Msg_List::Error,
                SDBMSG_RTECOMM_URI_ESCAPE_VALUE,
                Msg_Arg("URI_OPTION_VALUE", (SAPDB_Char *) optionValue)
                );
            return Error;
        }

        --lenOptionValue;
    }

    neededURILength = m_LengthURI + 1 + lenOptionName + (0 != lenOptionValue ? 1 + lenOptionValue : 0);

    //given buffer is to short. Given Memory can not be reallocated! Therefore we have to return.
    if (0 != m_MaxLengthURI && neededURILength > m_MaxLengthURI)
    {
        msgList = Msg_List(
            Msg_List::Warning,
            SDBMSG_RTECOMM_URI_BUFFER_TO_SMALL,
            Msg_Arg("BUFFERSIZE_NEEDED",SAPDB_ToString (neededURILength + 1)),
            Msg_Arg("BUFFERSIZE",SAPDB_ToString (m_MaxLengthURI + 1))
            );
        return URIBufferToSmall;
    }

    if (0 == m_MaxLengthURI)
    {
        SAPDB_Char *    tmpBuffer;
        SAPDB_UInt4     tmpLengthURI = m_LengthURI;

        tmpBuffer = (SAPDB_Char *)(RTEMem_RteAllocator::Instance().Allocate (neededURILength + 1));
        if (NULL == tmpBuffer)
        {
            msgList = Msg_List(
                Msg_List::Error,
                SDBMSG_RTECOMM_URI_MEMORY_ALLOCATION,
                Msg_Arg("ALLOC_SIZE",SAPDB_ToString (neededURILength + 1))
                );
            return Error;
        }

        memset (tmpBuffer, '\0', neededURILength + 1);
        URICpy (tmpBuffer, m_URI);
        CleanUp ();

        m_URI = tmpBuffer;
        m_LengthURI = tmpLengthURI;
        m_UseOwnMemory = true;
    }


    AddSeparator ();
    EscapeURIPart (QueryContentPart, optionName, lenOptionName + 1, m_URI + strlen (m_URI));
    if (0 != lenOptionValue)
    {
        strcpy (m_URI + strlen (m_URI), OPTION_ASSIGNMENT);
        EscapeURIPart (QueryContentPart, optionValue, lenOptionValue + 1, m_URI + strlen (m_URI));
    }

    m_LengthURI = neededURILength;

    return NoError;
}


/*---------------------------------------------------------------------------*/

void RTEComm_URIBuilder::CleanUp() 
{ 
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::CleanUp", Communication_Trace, 9);

    if (m_UseOwnMemory && NULL != m_URI)
    {
        destroy ( m_URI, RTEMem_RteAllocator::Instance());
    }

    m_UseOwnMemory = false;
    m_LengthURI = 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * const    RTEComm_URIBuilder::GetProtocol  (SAPDB_Char const * const      hostName, 
                                                              SAPDB_Bool                    encryptionWanted,
                                                              Msg_List &                    msgList)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEComm_URIBuilder::GetProtocol", Communication_Trace, 9);

    SAPDB_Char *            protocol = NULL;
    SAPDB_UInt4             hostNameLen = 0;
//    RTE_Nodename            tmpHostName;
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;
    int                     isSaprouterString = false;

    if (NULL != hostName)
    {   
        hostNameLen = (SAPDB_UInt4)strlen (hostName);
    }

    if (0 != hostNameLen)
    {
        isSaprouterString = eo40NiIsSaprouterSring ((SAPDB_Char *)hostName) == 0 ? false : true;
/*
        if (!isSaprouterString && '\0' == *m_LocalNodeName)
        {
            rc = sql43_get_my_host_name (tmpHostName, sizeof (tmpHostName));

            if (RTE_SYSTEMRC_NO_ERROR == rc)
            {
                rc = sql43_get_official_node_name (tmpHostName, 
                                                   m_LocalNodeName,
                                                   sizeof (m_LocalNodeName));
                if (RTE_SYSTEMRC_NO_ERROR != rc)
                {
                    strcpy (m_LocalNodeName, tmpHostName);
                }
            }
            else
            {
                strcpy (m_LocalNodeName, tmpHostName);
            }
        }


        if (!isSaprouterString)
        {    
            if (hostNameLen >= sizeof (RTE_Nodename))
            {
                msgList = Msg_List(
                    Msg_List::Error,
                    SDBMSG_RTECOMM_URI_HOSTNAME_LENGTH,
                    Msg_Arg("STRING_LENGTH_MAX",sizeof (RTE_Nodename) - 1),
                    Msg_Arg("STRING_LENGTH", hostNameLen)
                    );
                return protocol;
            }

            if ((0 != sql43_is_inet_addr (hostName)) || 
                (SAPDB_strcasecmp(hostName, URI_LOCALHOST_NAME) == 0))
            {
                strcpy (tmpHostName, hostName);
            }
            else
            {
                rc = sql43_get_official_node_name (hostName, 
                                                   tmpHostName,
                                                   sizeof(tmpHostName));
                if (RTE_SYSTEMRC_NO_ERROR != rc)
                {
                    strcpy (tmpHostName, hostName);
                }
            }
        }
*/
    }

    if (isSaprouterString )
    {
        if (encryptionWanted)
        {
            protocol = (SAPDB_Char *)PROTOCOL_SAPNISSL;
        }
        else
        {
            protocol = (SAPDB_Char *)PROTOCOL_SAPNI;
        }
    }
    else if (hostNameLen != 0)// && SAPDB_strcasecmp (tmpHostName, m_LocalNodeName) != 0)
    {
       if (encryptionWanted)
        {
            protocol = (SAPDB_Char *)PROTOCOL_REMOTESSL;
        }
        else
        {
            protocol = (SAPDB_Char *)PROTOCOL_REMOTE;
        }
     }    
    else
    {
        protocol = (SAPDB_Char *)PROTOCOL_LOCAL;
    }

    return protocol;
}

#if defined (URI_BUILDER_TEST)

URIBuilderArguments::URIBuilderArguments (int argc, char ** argv)
           : m_ServerNode (NULL),
             m_Port (NULL),
             m_ServerDB (NULL),
             m_Path (NULL),
             m_Encryption (false),
             m_OptionDesc         (Options),
             m_NumOfOptionDesc    (NUM_OF_OPTIONS_EO80(Options)),
             teo81_EvalProgArgs   (argc, argv, 
                                   Options, NUM_OF_OPTIONS_EO80(Options),
                                   OPTION_HELP)
{
  ROUTINE_DBG_MEO60 (_T("URIBuilderArguments::URIBuilderArguments"));
};

bool URIBuilderArguments::eo80_vNextValidOption (const int            OptionIndex,
                                                 const char*          OptionArgument )
{
  ROUTINE_DBG_MEO60 (_T("URIBuilderArguments::eo80_vNextValidOption"));
  bool   Ok = true;


  switch (OptionIndex)
  {
  case OPTION_HOST:
    m_ServerNode = (SAPDB_Char *)OptionArgument;
    break;
  case OPTION_PORT:
    m_Port = (SAPDB_Char *)OptionArgument;
    break;
  case OPTION_SERVERDB:
    m_ServerDB = (SAPDB_Char *)OptionArgument;
    break;
  case OPTION_PATH:
    m_Path = (SAPDB_Char *)OptionArgument;
    break;
  case OPTION_ENCRYPTION:
    m_Encryption = true;
    break;

  default:
    printf ( _T("Unknown option index '%d'\n"), OptionIndex  );
    Ok = false;
    break;
  }

  return Ok;
}


int main (int argc, char *argv[])
{

    SAPDB_Char const *              uriString = NULL;
    SAPDB_Char *                    port = NULL;
    SAPDB_Char *                    host = NULL;
    SAPDB_Char *                    serverDB = NULL;
    SAPDB_Char *                    path = NULL;
    SAPDB_Bool                      encryption = false;
    URIBuilderArguments             arguments( argc, argv);
#define USE_BUFFER
#ifdef USE_BUFFER
    SAPDB_Char                      uriBuffer[100] = "";//"local://./DATABASE/TESTDB";
    RTEComm_URIBuilder              uriBuilder (uriBuffer, sizeof (uriBuffer) - 1);    
#else
    RTEComm_URIBuilder              uriBuilder;
#endif
    Msg_List                        msgList;
    RTEComm_URIBuilder::URIBuildRC  rc = RTEComm_URIBuilder::NoError;

    sql60_enable_console ();

    if (!arguments.eo80_ParseArguments())
    {
        return 1;
    }

    host = arguments.GetHostname ();
    port = arguments.GetPort (); 
    serverDB = arguments.GetServerDB (); 
    encryption = arguments.GetEncryption ();
    path = arguments.GetPath ();

    if (NULL != path && 0 == strcmp (path, "DBM"))
    {
        rc = uriBuilder.BuildDBMURI ((SAPDB_UTF8 *)serverDB, host, port, msgList, encryption);
    }
    else if (NULL != path && 0 == strcmp (path, "LISTENER"))
    {
        rc = uriBuilder.BuildListenerURI (host, port, msgList, encryption);
    }
    else
    {
        rc = uriBuilder.BuildDatabaseURI ((SAPDB_UTF8 *)serverDB, host, port, msgList, encryption);
    }

    if (RTEComm_URIBuilder::NoError == rc)
    {
        printf ("Build URI: %s\n\n", uriBuilder.GetURI ());

        rc = uriBuilder.Append ((SAPDB_UTF8 *)("UnknownOption1"), 1, msgList);
        if (RTEComm_URIBuilder::NoError == rc)
        {
            printf ("Build URI: %s\n\n", uriBuilder.GetURI ());

            rc = uriBuilder.Append ((SAPDB_UTF8 *)("UnknownOption2"), "no", msgList);
            if (RTEComm_URIBuilder::NoError == rc)
            {
                printf ("Build URI: %s\n\n", uriBuilder.GetURI ());
            }
        }
    }

    if (RTEComm_URIBuilder::NoError != rc)
    {
        RTE_Message (msgList, MSG_DIAG_CONSOLE);
    }
}

#endif