/*!========================================================================

  @file         RTEComm_ParseURI.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Parse a liveCache Uniform Resource Identifier (URI)

                The liveCache Uniform Resource Identifier (URI) is a compact
                string of characters used to identify liveCache specific 
                resources.
                The following class is used to separate this URI ASCII
                string in particular unescaped UTF8 parts.

  @since        2006-01-11  13:32
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

#define USE_SAPNI_HEADER_FILE

#include <string.h> /* no check */  // for strlen(), strchr(), strcpy()
#include <ctype.h>  /* no check */  // isalnum()
#include <stdlib.h> /* no check */  // strtod, strtol, strtoul

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Comm_Messages.hpp"


#if defined (USE_SAPNI_HEADER_FILE)
#ifdef WIN32
  #define SAPonNT
#endif

#ifdef _IBMR2
  #define SAPonRS6000
#endif

#if PA11 || PA20W
  #define SAPonHPPA
#endif

#if defined (HP_IA64)
  #define SAPonHPPA
  #define SAPonHPIA64
#endif

#if defined(OSF1)
  #define SAPonALPHA
#endif

#if (defined(SVR4) && defined(SUN)) || defined (SOLARIS)
  #define SAPonSUN
#endif

#if defined(LINUX)
#define SAPonLIN
#endif

#if defined(FREEBSD)
#define SAPonFREEBSD
/* defined somewhere else ... */
#endif

//UNICODE
#include "sap/700/saptype.h"	/* nocheck */
#include "sap/700/saptypeb.h"	/* nocheck */
#include "sap/700/saptypec.h"	/* nocheck */
#include "sap/700/sapuc.h"		/* nocheck */
#include "sap/700/sapuc2.h"		/* nocheck */

//NI
#include "sap/700/err.h"		/* nocheck */
#include "sap/700/nixx.h"		/* nocheck */
#include "sap/700/dptrace.h"	/* nocheck */

#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#if !defined (USE_SAPNI_HEADER_FILE)
# define   NI_HOSTSEP   "/H/"  /* Separator fuer Hostname in Route */
# define   NI_SERVSEP   "/S/"  /* Separator fuer Service in Route  */
# define   NI_SEPSIZ    3
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/



/*===========================================================================*
*  STATIC/INLINE FUNCTION PROTOTYPES                                        *
*===========================================================================*/



/*===========================================================================*
*  METHODS                                                                  *
*===========================================================================*/

RTEComm_ParseURI::~RTEComm_ParseURI() 
{ 
    CleanUp();
}

/*---------------------------------------------------------------------------*/

void RTEComm_ParseURI::CleanUp() 
{ 
    m_LocalLocation = false;
    m_isNISSLLocation = false;
    m_isSSLLocation = false;

    m_hasListenerPath = false;

    FreeUnescapedString( m_ProtocolName ); 
    FreeUnescapedString( m_Fragment );
    destroy ( m_IPLocation, RTEMem_RteAllocator::Instance());
    destroy ( m_NILocation, RTEMem_RteAllocator::Instance());
    destroy ( m_DBPath, RTEMem_RteAllocator::Instance());
    destroy ( m_ProcServerPath, RTEMem_RteAllocator::Instance());
    destroy ( m_QueryOptList, RTEMem_RteAllocator::Instance());
    destroy ( m_OpaquePathElements, RTEMem_RteAllocator::Instance());

    if ( 0 != m_escapedURI )
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_escapedURI);
        m_escapedURI = 0;
    }
    if ( 0 != m_escapedPath )
    {
        RTEMem_RteAllocator::Instance().Deallocate(m_escapedPath);
        m_escapedPath = 0;
    }
}

/*---------------------------------------------------------------------------*/

RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::Parse ( SAPDB_Char const * const   uri,
                              SAPDBErr_MessageList      &messageList )
{
    URIRet        retVal = NoError;
    SAPDB_UInt4   len    = URILen( uri ) + 1;

    // clean up resource from last parse call 
    CleanUp();

    // Parse() messes up the string, so we make a copy first
    SAPDB_Char* URI = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate(len * sizeof(SAPDB_Char));

    if ( 0  != URI )
      m_escapedURI = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate(len * sizeof(SAPDB_Char));

    if (( 0  == URI ) || ( 0  == m_escapedURI ))
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
        retVal      = OutOfMemory;
    }
    else
    {
        SAPDB_Char *     protocolName;

        // Copy input string and remove white space from the front and the end.
        URICpy( URI, uri );
        URITrim(URI);

        URICpy( m_escapedURI, URI );
        
        SAPDB_Char*  remainURI = URI;

        // 1. Remove fragment, if any, from the tail.
        retVal = StripFragment( remainURI, messageList );

        // 2. Separate scheme on the front.
        if ( NoError == retVal )
            retVal = SeparateScheme( remainURI, messageList );

        // 3. Get protocol name.
        if ( NoError == retVal )
            retVal = ParseProtocol( remainURI, protocolName, messageList );

        // 4. Parse location.
        if ( NoError == retVal )
            retVal = ParseLocation( remainURI, protocolName, messageList );

        // 5. Parse path
        if ( NoError == retVal )
            retVal = ParsePath( remainURI, messageList );

        // 5. Parse query part
        if ( NoError == retVal )
            retVal = ParseQuery( remainURI, messageList );

        RTEMem_RteAllocator::Instance().Deallocate(URI);

        if ( NoError != retVal )
            CleanUp();
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Remove fragment, if any, from the tail.

            If the URI string contains a '#', then the substring
            after the first '#' and up to the end of the URI string
            is the fragment identifier. If the '#' is the last
            character, or if no '#' is present, then the fragment
            identifier is empty. The matched substring, including
            the '#' is removed from the parse string.

  @param    uri        [in]  - The URI string which has to
                               be parsed. The matched substring, 
                               including the '#' is removed from 
                               the parse string.

  @return   NoError, OutOfMemory.
*/
RTEComm_URIUtils::URIRet
    RTEComm_ParseURI::StripFragment( SAPDB_Char *              const uri,
                                     SAPDBErr_MessageList      &messageList )

{
    URIRet       retVal = NoError;
    SAPDB_Char*  currStr;

    if ( 0 != ( currStr = URIFindChar( uri,'#' )) ) 
    {
        currStr[0] = URI_NULL_CHAR;
        retVal = CreateUnescapedString (m_Fragment, currStr + 1, messageList);
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/


/*!
  @brief    Separate scheme on the front.

            If the parse string contain a colon after the 1st character
            and before any characters not allowed as part of a scheme 
            name (i.e. any not alphanumeric, '+', '.' or '-'),
            the scheme of the uri is the substring of chars up
            to but not including the first colon.
            These chars and the colon are then removed from the
            parse string.

  @param    uri    [in/out]  - The URI string which has to
                               be parsed. If the parse string 
                               contains a scheme name, this name 
                               and the colon is removed from the 
                               parse string. After calling this 
                               methode the URI variable points 
                               to the remaining URI string.

  @return   NoError, ParseError.
*/

RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::SeparateScheme( SAPDB_Char *              &uri,
                                      SAPDBErr_MessageList      &messageList )

{
    SAPDB_Char*  currStr;

    if ( 0 != ( currStr = SkipAllowedCharacters( SchemePart, uri ) )) 
    {
        if ( ':' == currStr[0] )
        {   // delimiter found!
            currStr[0] = URI_NULL_CHAR;

            // look for allowed sheme names
            if ( URIsIdentical( uri, SCHEME_LIVECACHE ) ||
                 URIsIdentical( uri, SCHEME_MAXDB ))
            {
                uri = currStr + 1;
                // currently nothing to do witch that scheme information.
                // later on, we might use it.
            }
            else
                currStr[0] = ':'; // not a scheme name restore delimiter
        }
    }

    return NoError;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Detect protocol name.
            Parse protocol.
            If the parse string contain a colon after the 1st character
            and before any characters not allowed as part of a protocol  
            name (i.e. any not alphanumeric, '+', '.' or '-'), the 
            protocol of the uri is the substring of chars up to but 
            not including the first colon.
            These chars and the colon are then removed from the
            parse string before continuing.

  @param    uri    [in/out]  - The URI string which has to
                               be parsed. If the parse string 
                               contains a protocol name, this name 
                               and the colon is removed from the 
                               parse string. After calling this 
                               methode the URI variable points 
                               to the remaining URI string.

  @return   NoError, ProtocolParseError, OutOfMemory.
*/
RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::ParseProtocol( SAPDB_Char *              &uri,
                                     SAPDB_Char *              &protocolName,
                                     SAPDBErr_MessageList      &messageList )
{
    URIRet       retVal = NoError;
    SAPDB_Char*  currStr;

    m_ProtocolName = 0;
    protocolName   = 0;

    // The next part might be the protocol name
    if ( 0 != ( currStr = SkipAllowedCharacters( ProtnamePart, uri ) )) 
    {
        if ( ':' == currStr[0] )
        {   // delimiter found!
            currStr[0] = URI_NULL_CHAR;
            protocolName = uri; 
            retVal       = CreateUnescapedString (m_ProtocolName, uri, messageList );
            uri          = currStr + 1;
        }
    }
    // There is no need to give a protocol name for remote and local communication

    return retVal;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Parse location part.

  The location part is protocol dependent. It is separated 
  in its elemetns like hostname, port, saprouter etc

  @param    uri    [in/out]  - The URI string which has to
                               be parsed. If the parse string 
                               contains a location, this 
                               information is removed from the 
                               parse string. After calling this 
                               methode the URI variable points 
                               to the remaining URI string.

  @return   NoError, LocationParseError, OutOfMemory.
*/
RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::ParseLocation( SAPDB_Char *              &uri,
                                     SAPDB_Char *              protocolName,
                                     SAPDBErr_MessageList      &messageList )

{
    URIRet  retVal = NoError;
    SAPDB_Char*  querySeparator = URIFindStr( uri, "?" );

    if ( 0 != querySeparator ) 
        *querySeparator = URI_NULL_CHAR; 

    if ( 0 == m_ProtocolName )
    {
        // Currently there is no need to give a protocol name for
        // remote and local communication. We have to look forwart into 
        // the location part of the URI string. If the parse string begins
        // with '//', then the substring of characters after '//' and
        // up to, but not including the next '/' is the network location
        // of the uri. If only one slash characters is present, the local
        // system is requested.
        if ( uri[0] == '/' )
        {
            if (( uri[1] == '/' ) && ( uri[2] != '.' ))
                retVal = CreateUnescapedString (m_ProtocolName, PROTOCOL_REMOTE, messageList );
            else
                retVal = CreateUnescapedString (m_ProtocolName, PROTOCOL_LOCAL, messageList );
        }
        else
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_WRONG_OR_MISSING_LOCATION  );
            retVal      = ProtocolParseError;
        }
    }

    if ( NoError == retVal )
    {   // The locations get it's own location object to parse
        // the URI location string and hold the results. 
        //
        if ( URIsIdentical( m_ProtocolName, PROTOCOL_LOCAL ) )
        {   // we parse  the locale location directly it needs
            // location object
            m_LocalLocation = true;
            retVal          = ParseLocalLocation( uri, messageList );
        }
        else if ( URIsIdentical( m_ProtocolName, PROTOCOL_REMOTE ) ||
                  URIsIdentical( m_ProtocolName, PROTOCOL_REMOTESSL ))
        {
            m_IPLocation = new (RTEMem_RteAllocator::Instance()) RTEComm_URI_IPLocation;

            if ( 0  == m_IPLocation )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
            {
                retVal = m_IPLocation->Parse( uri, messageList );
                if (URIsIdentical(m_ProtocolName, PROTOCOL_REMOTESSL))
                {
                    m_isSSLLocation = true;
                }
            }
        }
        else if ( URIsIdentical( m_ProtocolName, PROTOCOL_SAPNI ) ||
                  URIsIdentical( m_ProtocolName, PROTOCOL_SAPNISSL ))
        {
            m_NILocation = new (RTEMem_RteAllocator::Instance()) RTEComm_URI_NILocation;

            if ( 0  == m_NILocation )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
            {
                retVal = m_NILocation->Parse( uri, messageList );
                if (URIsIdentical(m_ProtocolName, PROTOCOL_SAPNISSL))
                {
                    m_isNISSLLocation = true;
                }
            }
        }
        else
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_UNEXPECTED_PROTOCOL, protocolName );
            retVal      = ProtocolParseError;
        }
    }

    if ( 0 != querySeparator ) 
        *querySeparator = '?'; 

    return retVal;
}

/*---------------------------------------------------------------------------*/

inline RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::ParseLocalLocation( SAPDB_Char *              &uri,
                                          SAPDBErr_MessageList      &messageList )
{
    URIRet  retVal = NoError;

    // if we have no '//' the URI starts with a path part
    if (( uri[0] == '/' ) && ( uri[1] == '/' ))
    {
        if ( uri[2] == '.' )
        {
            // The '//.' is removed from the parse string.
            uri += 3;
        }
        else 
        {
          messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_WRONG_LOCATION_STRING, "//."   );
          retVal      = LocationParseError; // we are expecting a slash!
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Parse path part.

  The path part depends on the first path element. The element 
  describes the destinations like database, dbmserver, procserver etc.

  @param    uri    [in/out]  - The URI string which has to
                                be parsed. If the parse string 
                                contains a path, this 
                                information is removed from the 
                                parse string. After calling this 
                                methode the URI variable points 
                                 to the remaining URI string.

   @return   NoError, PathParseError, OutOfMemory.
   */
RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::ParsePath( SAPDB_Char *              &uri,
                                 SAPDBErr_MessageList      &messageList )
{
    URIRet       retVal = NoError;
    SAPDB_Char*  currStr;
    SAPDB_Char*  querySeparator = URIFindStr( uri, "?" );
    SAPDB_Char*  escapedPath    = uri;

    if ( 0 != querySeparator ) 
        *querySeparator = URI_NULL_CHAR; 

    // 'SkipAllowedCharacters' returns null if no unallowed character is found.
    // Because the difference between 'PathContentPart' and 'PathPart' is the 
    // slash as an addition allowed character in 'PathPart', the following 
    // path parse classes have no need to check for unallowed characters.
    //
    // Does the URI start with a slash and has at least on following character which
    // is not a slash.
    if ( 0 != ( currStr = SkipAllowedCharacters( PathPart, uri )))
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_PATH_CONTAINS_WRONG_CHARS  );
        retVal      = PathParseError;
    }
    else if  (( '/' != uri[0] ) || ( '/' == uri[1] ) || ( URI_NULL_CHAR == uri[1] ))
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT,  RTEERR_COMM_WRONG_OR_MISSING_PATH );
        retVal      = PathParseError;
    }
    else
    {
        SAPDB_UInt4  escapedPathLen = URILen(escapedPath);

        m_escapedPath = (SAPDB_Char*)RTEMem_RteAllocator::Instance().Allocate((escapedPathLen * sizeof(SAPDB_Char) + 1));

        if ( 0 == m_escapedPath )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
            retVal      = OutOfMemory;
        }
        else
        {
            URICpy( m_escapedPath, escapedPath );

            // separate first path segment
            if ( 0 != ( currStr = URIFindChar( ++uri, '/' )) )
                currStr[0] = URI_NULL_CHAR;

            // Normally this substring does not contain escaped characters, 
            // but we can not be sure!
            SAPDB_UTF8*  pathType;

            retVal = CreateUnescapedString ( pathType, uri, messageList );

            if ( NoError == retVal )
            {
                if ( URIsIdentical( pathType, PATH_DATABASE ) )
                {   // strip of pathType  "/database"
                    uri = currStr ? currStr + 1 : uri + URILen(uri);

                    m_DBPath = new (RTEMem_RteAllocator::Instance()) RTEComm_URI_DBPath;

                    if ( 0  == m_DBPath )
                    {
                        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                        retVal      = OutOfMemory;
                    }
                    else
                        retVal = m_DBPath->Parse( uri, messageList );
                }
                else if ( URIsIdentical( pathType, PATH_PROCSERVER ) )
                {    // strip of pathType:  "/procserver"
                    uri = currStr ? currStr + 1 : uri + URILen(uri);

                    m_ProcServerPath = new (RTEMem_RteAllocator::Instance()) RTEComm_URI_ProcSrvPath;

                    if ( 0  == m_ProcServerPath )
                    {
                        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                        retVal      = OutOfMemory;
                    }
                    else
                        retVal = m_ProcServerPath->Parse( uri, messageList );
                }
                /*
                else if ( URIsIdentical( pathType, PATH_DBMSERVER ) )
                {
                // currently not used, needs implementation !!!!
                retVal = PathParseError;
                }
                */
                else if ( URIsIdentical( pathType, PATH_LISTENER ) )
                {    // strip of pathType:  "/listener"
                    uri = currStr ? currStr + 1 : uri + URILen(uri);

                    m_hasListenerPath = true;
                }
                else
                {
                    if ( 0 != currStr ) *currStr = '/'; // restore path separator

                    // store unknown path elements in a list
                    m_OpaquePathElements = new (RTEMem_RteAllocator::Instance()) RTEComm_URIPathSegmentList;

                    if ( 0 == m_OpaquePathElements )
                    {
                        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                        retVal      = OutOfMemory;
                    }
                    else
                        retVal = m_OpaquePathElements->Create( uri, messageList );
                }
                FreeUnescapedString( pathType );
            }
        }

        if ( NoError == retVal )
        {
            uri = escapedPath + escapedPathLen;
        }
    }

    if ( 0 != querySeparator ) 
        *querySeparator = '?'; 

    return retVal;
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Parse query part.

  The query part starting with '?' is separated into single options 
  which managed in a list.

  @param    uri    [in/out]  - The URI query part which has to
                               be parsed.

   @return   NoError, QueryParseError, OutOfMemory.
   */
RTEComm_URIUtils::URIRet 
    RTEComm_ParseURI::ParseQuery( SAPDB_Char *              &uri,
                                  SAPDBErr_MessageList      &messageList )
{
    URIRet       retVal = NoError;
    SAPDB_Char*  currStr;

    if ( URI_NULL_CHAR != uri[0] )
    {
        if ( '?' != uri[0] ) // Does the uri start with a question mark?
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_QUERY_QUESTION_MARK_EXP );
            retVal      = QueryParseError;
        }
        // 'SkipAllowedCharacters' returns null if no unallowed character is found.
        else if ( 0 != ( currStr = SkipAllowedCharacters( QueryPart, uri )))
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_QUERY_CONTAINS_WRONG_CHARS );
            retVal      = QueryParseError;
        }
        else if ( URI_NULL_CHAR != uri[1] )
        {
            m_QueryOptList = new (RTEMem_RteAllocator::Instance()) RTEComm_URIQueryOptList;

            if ( 0  == m_QueryOptList )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
                retVal = m_QueryOptList->Create( ++uri, messageList );
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/
/*------------------------- RTEComm_URI_IPLocation --------------------------*/
/*---------------------------------------------------------------------------*/

inline RTEComm_URIUtils::URIRet 
    RTEComm_URI_IPLocation::Parse( SAPDB_Char *               &uri,
                                    SAPDBErr_MessageList      &messageList )
{
    URIRet        retVal = NoError;
    SAPDB_Char*   currStr;

    // The parse string begins with '//', then the substring
    // of characters after '//' and up to, but not including
    // the next '/' is the network location.
    if (( uri[0] != '/' ) || ( uri[1] != '/' ) || ( uri[2] == '/' )  || ( uri[2] == '.' ))
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_WRONG_LOCATION_STRING, "e.g. //myhost or //myhost:3342" );
        retVal      = LocationParseError;
    }
    else
    {
        // The '//' is removed from the parse string.
        uri += 2;

        SAPDB_Char* hostPart = uri;
        SAPDB_Char  delimiter;

        if ( 0 != ( currStr = URIFindChar( uri, '/' )) ) 
        {
            delimiter  = currStr[0];
            currStr[0] = URI_NULL_CHAR;
            uri        = currStr; // set uri pointer to end of string
        }
        else
        {
            delimiter = '\0';
            uri += URILen(uri); // set uri pointer to end of string
        }

        //  unallowed characters found?
        if ( 0 != SkipAllowedCharacters( HostPart, hostPart )) 
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_IPLOC_CONTAINS_WRONG_CHARS );
            retVal      = LocationParseError;
        }
        else
        {
            if ( 0 != ( currStr = URIFindChar( hostPart,':' )) ) 
            {
                currStr[0] = URI_NULL_CHAR;

                char* endPtr;
                strtoul( currStr + 1, &endPtr, 10);

                if ( endPtr == uri )
                {
                    retVal = CreateUnescapedString (m_PortNo, currStr + 1, messageList );

                    if ( NoError == retVal )
                        retVal = CreateUnescapedString( m_Hostname, hostPart, messageList );
                }
                else
                {
                    messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_IPLOC_CONTAINS_WRONG_PORTNO );
                    retVal      = LocationParseError;
                }
            }
            else
                retVal = CreateUnescapedString (m_Hostname, hostPart, messageList );

            uri[0] = delimiter; // restore delimiter
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTEComm_URI_IPLocation::~RTEComm_URI_IPLocation()
{ 
    FreeUnescapedString( m_Hostname );
    FreeUnescapedString( m_PortNo );
}


/*---------------------------------------------------------------------------*/
/*------------------------- RTEComm_URI_NILocation --------------------------*/
/*---------------------------------------------------------------------------*/

inline RTEComm_URIUtils::URIRet 
    RTEComm_URI_NILocation::Parse( SAPDB_Char *              &uri,
                                   SAPDBErr_MessageList      &messageList )
{
    URIRet         retVal = NoError;
    SAPDB_Char*    currStr;

    if ( 0 == ( currStr = URIFindStr( uri, PROTOCOL_SAPNI_END )) ) 
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_NILOC_DELIMITER_MISSING, PROTOCOL_SAPNI_END );
        retVal      = LocationParseError;
    }
    else
    {
        currStr[0] = URI_NULL_CHAR;

        SAPDB_Char*  endSAPNI = currStr + URILen(PROTOCOL_SAPNI_END);

        if ( !URIsIdentical(uri, NI_HOSTSEP, NI_SEPSIZ ) )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_NILOC_HOST_SEP_MISSING, NI_HOSTSEP );
            retVal      = LocationParseError;
        }
        else
        {
            SAPDB_Char*  hostName;

            hostName = currStr = uri + NI_SEPSIZ;

            /* step to the last host in routerstring '/h/' */
            while ( 0 != (currStr = URIFindStr( currStr, NI_HOSTSEP )) )
                hostName = currStr += NI_SEPSIZ;

            // find end of host name
            currStr  = URIFindChar(hostName, NI_SERVSEP[0] );

            // Normaly we would check the hotname for valid characters, but it's
            // better not interpret too much because the NI development is not 
            // under our concern.

            if ( 0 == currStr )
            {   // no separator follows, take the whole string
                retVal = CreateUnescapedString( m_LastHostname, hostName, messageList );
            }
            else
            {
                /* cut off hostname from next separator string starting with '/' */
                currStr[0] = URI_NULL_CHAR;

                retVal = CreateUnescapedString( m_LastHostname, hostName, messageList );

                currStr[0] = NI_HOSTSEP[0]; // restore saprouter separator character
                
                if ( NoError == retVal )
                {
                    // find server separator '/S/'
                    if ( 0 != ( currStr = URIFindStr ( currStr, NI_SERVSEP ))) 
                    {
                        SAPDB_Char*  portNo;

                        portNo = currStr += NI_SEPSIZ;

                        // find end of port number 
                        currStr  = URIFindChar(currStr, NI_SERVSEP[0] );

                        /* cut off port number from next separator string starting with '/' */
                        if ( 0 != currStr ) currStr[0] = URI_NULL_CHAR;

                        char* endPtr;
                        strtoul( portNo, &endPtr, 10);

                        if ((( endPtr == currStr ) ||  // no character left (fast check), if not end of uri
                             ( endPtr == (portNo +  URILen(portNo))) ) &&  // no character left
                            ( endPtr != portNo ))     // at least one character found
                        {
                            retVal = CreateUnescapedString ( m_LastPortNo, portNo, messageList );
                        }
                        else
                        {
                            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_NILOC_CONTAINS_WRONG_PORTNO );
                            retVal      = LocationParseError;
                        }
                    }
                    else
                        currStr = 0;
                }
                else
                    currStr = 0;
            }

            if ( NoError == retVal )
            {
                retVal  = CreateUnescapedString ( m_SAPRouter, uri, messageList );
                uri     = endSAPNI;
            }
            if ( 0 != currStr )  currStr[0] = NI_HOSTSEP[0]; // restore saprouter separator character
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTEComm_URI_NILocation::~RTEComm_URI_NILocation()
{ 
    FreeUnescapedString( m_LastHostname );
    FreeUnescapedString( m_LastPortNo );
    FreeUnescapedString( m_SAPRouter );
}

/*---------------------------------------------------------------------------*/
/*--------------------------- RTEComm_URI_DBPath ----------------------------*/
/*---------------------------------------------------------------------------*/

inline RTEComm_URIUtils::URIRet 
    RTEComm_URI_DBPath::Parse( SAPDB_Char *              &uri,
                               SAPDBErr_MessageList      &messageList )

{
    URIRet   retVal = NoError;

    // There is no need to search for unallowed characters becauses the 
    // 'ParsePath' has done this. 

    // We are expecting the database name!
    if ( URI_NULL_CHAR == uri[0] )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_DBPATH_MISSING_DBNAME );
        retVal      = PathParseError;
    }
    else
    {
        SAPDB_Char*   currStr;
  
        //
        // 1. sepaerate and check ServerDB name
        //
        SAPDB_Char*   serverDB = uri;

        if ( 0 != ( currStr = URIFindChar( uri, '/' )) )
        {
            currStr[0] = URI_NULL_CHAR;
            uri        = currStr + 1;
        }
        else 
            uri += URILen(uri);

        retVal = CreateUnescapedString ( m_ServerDB, serverDB, messageList );

        //
        // 2. store remaining path elements in a list
        //
        if (( NoError == retVal ) && ( URI_NULL_CHAR != uri[0] ))
        {
            m_OpaquePathElements = new (RTEMem_RteAllocator::Instance()) RTEComm_URIPathSegmentList;

            if ( 0 == m_OpaquePathElements )
            {
                messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
                retVal      = OutOfMemory;
            }
            else
            {
                retVal = m_OpaquePathElements->Create( uri, messageList );
                uri += URILen(uri);
            }
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTEComm_URI_DBPath::~RTEComm_URI_DBPath()
{ 
    FreeUnescapedString( m_ServerDB );
    destroy ( m_OpaquePathElements, RTEMem_RteAllocator::Instance());
}


/*---------------------------------------------------------------------------*/
/*------------------------- RTEComm_URI_ProcSrvPath -------------------------*/
/*---------------------------------------------------------------------------*/

inline RTEComm_URIUtils::URIRet 
    RTEComm_URI_ProcSrvPath::Parse( SAPDB_Char *              &uri,
                                    SAPDBErr_MessageList      &messageList )
{
    URIRet   retVal = NoError;

    // There is no need to search for unallowed characters becauses the 
    // 'ParsePath' has done this. 
/*
    // We are expecting some input!
    if ( URI_NULL_CHAR == uri[0] )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, );
        retVal      = PathParseError;
    }
    else
*/
    {
        //
        // store path elements in a list
        m_OpaquePathElements = new (RTEMem_RteAllocator::Instance()) RTEComm_URIPathSegmentList;

        if ( 0 == m_OpaquePathElements )
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_COMM_OUT_OF_MEMORY );
            retVal      = OutOfMemory;
        }
        else
        {
            retVal = m_OpaquePathElements->Create( uri, messageList );
            uri += URILen(uri);
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTEComm_URI_ProcSrvPath::~RTEComm_URI_ProcSrvPath()
{ 
    destroy ( m_OpaquePathElements, RTEMem_RteAllocator::Instance());
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
